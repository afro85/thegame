#include <Common/Socket.h>
#include <Common/Log.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>

Common::Socket::Socket() {
    mFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mFd < 0) {
        throw std::system_error(std::error_code(errno, std::generic_category()),
            "socket()");
    }
    Log::d() << "Socket created fd=" << mFd << "\n";
}

void Common::Socket::bind(uint16_t aPort) {
    sockaddr_in lAddr;
    lAddr.sin_family = AF_INET;
    lAddr.sin_addr.s_addr = INADDR_ANY;
    lAddr.sin_port = htons(aPort);
    memset(&(lAddr.sin_zero), 0x0, sizeof(lAddr.sin_zero));

    if (::bind(mFd, (sockaddr*)&lAddr, sizeof(lAddr)) < 0) {
        throw std::system_error(std::error_code(errno, std::generic_category()),
            "bind()");
    }
}

void Common::Socket::listen(int aMaxConnection){
    if (::listen(mFd, aMaxConnection) < 0) {
        throw std::system_error(std::error_code(errno, std::generic_category()),
            "aMaxConnection()");
    }
}

void Common::Socket::setNonBlocking() {
    auto lFlags = fcntl(mFd, F_GETFL, 0);
    if (lFlags < 0) {
        throw std::system_error(std::error_code(errno, std::generic_category()),
            "fcntl(): F_GETFL");
    }

    if (fcntl(mFd, F_SETFL, lFlags | O_NONBLOCK) < 0) {
        throw std::system_error(std::error_code(errno, std::generic_category()),
            "fcntl(): F_SETFL");
    }
}
