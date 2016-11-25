#pragma once

#include <server/Network/Socket.h>

#include <thread>
#include <atomic>
#include <memory>
#include <functional>
#include <unordered_map>

#include <unistd.h>
#include <sys/epoll.h>

namespace Network {

class TcpServer {

public:
    using Ptr = std::shared_ptr<TcpServer>;
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
        size_t operator() (const Network::TcpServer::Event& aE) const {
            return std::hash<int>()(static_cast<int>(aE));
        }
    };
    // Type for assocation the particular handler and event
    using Handlers = std::unordered_map<Event, Handler, hashEvent>;

public:
    static TcpServer& getInstance() {
        static TcpServer lObj{};
        return lObj;
    }

    virtual ~TcpServer();

    void run();
    void registerHandler(Socket::Fd aFd, Event aEvent, const Handler& handler);
    void unregisterHandler(Socket::Fd aFd, Event aEvent);

private:
    explicit TcpServer();

    void close() noexcept {
        if (mEpollFd >= 0)
            ::close(mEpollFd);
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
