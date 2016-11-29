#include <Common/Log.h>
#include <Common/EventDispatcher.h>
#include <server/Acceptor.h>

#include <algorithm>

Common::EventDispatcher::EventDispatcher() : mStopListen(false) {
    mEpollFd = epoll_create(1);
    if (mEpollFd < 0) {
        throw std::system_error(std::error_code(errno, std::generic_category()),
            "epoll_create()");
    }
    Log::d() << "Created epoll fd=" << std::to_string(mEpollFd) << "\n";
}

Common::EventDispatcher::~EventDispatcher() {
    stopListen();
    mListenThread.join();
    close();
}

void Common::EventDispatcher::run() {
    if (mListenThread.get_id() != std::thread::id())
        std::logic_error("There is one listen thread existed");

    mListenThread = std::thread(&Common::EventDispatcher::listen, this);
}

void Common::EventDispatcher::listen() {
    // Waiting for events
    epoll_event lRcvEvents[kMaxEvents] = {0};
    while(!mStopListen) {
        // Block, until any event comes up or timeout occures
        const auto lRcvFdCount = epoll_wait(mEpollFd, lRcvEvents, kMaxEvents,
                kWaitTimeoutMs);
        // Error occured
        if (lRcvFdCount < 0) {
            if (errno == EINTR) {
               Log::d() << "epoll_wait() interrupted by signal\n";
               stopListen();
               continue;
            }
            else {
                close();
                stopListen();
                throw std::system_error(std::error_code(errno,
                            std::generic_category()), "epoll_wait()");
            }
        }
        // Timeout occured
        else if (lRcvFdCount == 0) {
            Log::d() << "epoll_wait() timeout occurs\n";
            continue;
        }
        // Valid events list has been received
        else {
            // Process all of notified fd
            for (auto lRcvFdIt  = 0; lRcvFdIt < lRcvFdCount; ++lRcvFdIt) {
                const auto& lRcvFd = lRcvEvents[lRcvFdIt];
                // Find current fd with its handlers
                const auto& lRegFd = mHandlers.find(
                        lRcvFd.data.fd);
                if (lRegFd == mHandlers.end()) {
                    throw std::runtime_error("There is no fd= " +
                            std::to_string(lRcvFd.data.fd) +
                            " registered");
                }
                // Invoke handlers for all received and registred events
                const auto& lRegEvents = lRegFd->second;
                std::for_each(lRegEvents.begin(), lRegEvents.end(),
                        [&](auto& aE) {
                            if (lRcvFd.events & static_cast<uint32_t>(aE.first)) {
                                aE.second();
                            }
                        });
            }
        }
    }
}

void Common::EventDispatcher::registerHandler(Socket::Fd aFd, Event aEvent,
        const Handler& aHandler) {
    // Get the current handlers set of provided fd or insert the new one
    auto lRegFd = mHandlers.insert(std::make_pair(aFd, Handlers()));
    auto lRegEvents = lRegFd.first->second;
    auto lOperation = (lRegFd.second == true) ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;

    // Check if there is such event already registered
    if ((lRegFd.second == false) && (lRegEvents.find(aEvent) != lRegEvents.end())) {
        throw std::runtime_error("Event " +
                std::to_string(static_cast<uint32_t>(aEvent)) +
                "is already registered");
    }

    // Evaluate current events mask
    epoll_event lEventData;
    lEventData.events = kDefaultEvents;
    for (const auto& lHandlerIt : lRegEvents)
        lEventData.events |= static_cast<uint32_t>(lHandlerIt.first);
    Log::d() << std::hex << "current events mask: fd=" << aFd << " events=" <<
        lEventData.events << "\n";

    // Add new event
    lRegEvents.insert(std::make_pair(aEvent, aHandler));

    // Register event
    lEventData.events |= static_cast<uint32_t>(aEvent);
    lEventData.data.fd = aFd;
    if (epoll_ctl(mEpollFd, lOperation, aFd, &lEventData) < 0) {
        throw std::system_error(std::error_code(errno, std::generic_category()),
            "epoll_ctl(op=" + std::to_string(lOperation) + "), fd=" +
            std::to_string(aFd) + " event=" +
            std::to_string(static_cast<int>(aEvent)));
    }
}

void Common::EventDispatcher::unregisterHandler(Socket::Fd aFd, Event aEvent) {
    // Find provided Fd handlers' map
    auto lRegFd = mHandlers.find(aFd);
    if (lRegFd == mHandlers.end()) {
        throw std::runtime_error("There is no such socket(fd=" +
                std::to_string(aFd) + ") registered");
    }

    // Read the already registered events mask
    epoll_event lEventData;
    lEventData.events = kDefaultEvents;
    auto lRegEvents = lRegFd->second;
    std::for_each(lRegEvents.begin(), lRegEvents.end(), [&](auto& aE) {
                lEventData.events |= static_cast<uint32_t>(aE.first);
            });

    // Check if the provided event is already registered
    if (!(lEventData.events & static_cast<uint32_t>(aEvent))) {
        throw std::runtime_error("There is no such event (" +
                std::to_string(static_cast<uint32_t>(aEvent)) +
                ") registered for socket(fd=" + std::to_string(aFd));
    }

    // Remove provided event from event mask
    lEventData.events &= !(static_cast<uint32_t>(aEvent));

    // Unregister event or entire fd in case of default events mask
    auto lOperation =
        (lEventData.events != kDefaultEvents) ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    if (epoll_ctl(mEpollFd, lOperation, aFd, &lEventData) < 0) {
        throw std::system_error(std::error_code(errno, std::generic_category()),
            "epoll_ctl(op=" + std::to_string(lOperation) + "), fd=" +
            std::to_string(aFd) + " event=" +
            std::to_string(static_cast<int>(aEvent)));
    }

    // Remove event or entire fd from global handlers list
    if (lOperation == EPOLL_CTL_MOD)
        lRegFd->second.erase(aEvent);
    else if (lOperation == EPOLL_CTL_DEL)
        mHandlers.erase(lRegFd);
}
