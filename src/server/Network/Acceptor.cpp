#include <server/Log.h>
#include <server/Network/Acceptor.h>
#include <Common/EventDispatcher.h>

#include <exception>

Network::Acceptor::Acceptor(uint16_t aPort)
{
    // Set appropriate socket properites in order to be server socket
    mSocket.setOption<int>(SO_REUSEADDR, 1);
    mSocket.setOption<int>(SO_KEEPALIVE, 1);
    mSocket.bind(aPort);
    mSocket.listen();
    mSocket.setNonBlocking();

    // Register socket on CAN_READ event
    using namespace Common;
    EventDispatcher::getInstance().registerHandler(mSocket.getFd(),
            EventDispatcher::Event::kCanRead,
            EventDispatcher::Handler(std::bind(&Acceptor::onCanRead, this)));
}

void Network::Acceptor::onCanRead()
{
    Log::d() << __func__ << "\n";
}
