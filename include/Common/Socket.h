#pragma once

#include <system_error>
#include <memory>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

namespace Common {

class Socket {
public:
    using UniquePtr = std::unique_ptr<Socket>;
    using Ptr = std::shared_ptr<Socket>;
    using Fd = int;

public:
    explicit Socket();
    Socket(const Socket&) = delete;
    Socket(const Socket&&) = delete;
    virtual ~Socket() = default;
    Socket& operator=(const Socket&) = delete;
    Socket& operator=(const Socket&&) = delete;

    template<typename T>
    void setOption(int aName, T aValue, int aLevel = SOL_SOCKET);
    void bind(uint16_t aPort);
    void listen(int aMaxConnection = SOMAXCONN);
    void setNonBlocking();

    virtual void close() const noexcept {};
    virtual void read() const noexcept {};
    virtual void write() const noexcept {};

    int getFd() const { return mFd; }

private:
    Fd mFd;
};

template<typename T>
void Socket::setOption(int aName, T aValue, int aLevel) {
    if (::setsockopt(mFd, aLevel, aName, &aValue, sizeof(T)) < 0 ) {
        throw std::system_error(std::error_code(errno, std::generic_category()),
            "setsockopt(): fd=" + std::to_string(mFd));
    }
}

}
