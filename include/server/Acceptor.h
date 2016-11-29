#pragma once

#include <Common/Socket.h>

#include <memory>

class Acceptor {
public:
    explicit Acceptor(uint16_t aPort = kDefaultPort);
    virtual ~Acceptor() = default;

    void onCanRead();

private:
    static const uint16_t kDefaultPort = 4004;
    Common::Socket mSocket;
};
