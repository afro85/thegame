#pragma once

#include <server/Network/Socket.h>

#include <memory>

namespace Network {

class Acceptor {
public:
    explicit Acceptor(uint16_t aPort = kDefaultPort);
    virtual ~Acceptor() = default;

    void onCanRead();

private:
    static const uint16_t kDefaultPort = 4004;
    Socket mSocket;
};

}