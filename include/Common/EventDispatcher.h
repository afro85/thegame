#pragma once

#include <Common/Socket.h>

#include <thread>
#include <atomic>
#include <memory>
#include <functional>
#include <unordered_map>

#include <unistd.h>
#include <sys/epoll.h>

namespace Common {

class EventDispatcher {

public:
    using Ptr = std::shared_ptr<EventDispatcher>;
    // Definition types of events those handler can be registered for
    enum class Event : uint32_t {
        kCanRead = EPOLLIN,
        kCanWrite = EPOLLOUT,
        kClosed = (EPOLLRDHUP | EPOLLHUP)
    };
    // Event handler type
    using Handler = std::function<void(void)>;

private:
    // Hash function for Event enum.
    struct hashEvent {
        size_t operator() (const Common::EventDispatcher::Event& aE) const {
            return std::hash<int>()(static_cast<int>(aE));
        }
    };
    // Type for assocation the particular handler and event
    using Handlers = std::unordered_map<Event, Handler, hashEvent>;

public:
    static EventDispatcher& getInstance() {
        static EventDispatcher lObj{};
        return lObj;
    }

    virtual ~EventDispatcher();

    void run();
    void registerHandler(Socket::Fd aFd, Event aEvent, const Handler& handler);
    void unregisterHandler(Socket::Fd aFd, Event aEvent);

private:
    explicit EventDispatcher();

    void close() noexcept {
        if (mEpollFd >= 0) {
            ::close(mEpollFd);
            mEpollFd = -1;
        }
    }
    void listen();
    void stopListen() noexcept { mStopListen = true; }

private:
    static const uint16_t kMaxEvents = 50;
    static const int kWaitTimeoutMs = 1000;
    static const uint32_t kDefaultEvents = (EPOLLERR | EPOLLHUP | EPOLLET);

    std::thread mListenThread;
    std::atomic<bool> mStopListen;

    int mEpollFd = -1;
    std::unordered_map<Socket::Fd, Handlers> mHandlers;
};

}
