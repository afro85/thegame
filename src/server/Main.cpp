#include <server/Log.h>
#include <server/ThreadPool.h>
#include <Common/EventDispatcher.h>

#include <iostream>

#include <cstdlib>
#include <unistd.h>

int main()
{
    std::cout << "The Game has just begun\n";

    ThreadPool lThreadPool(5);

    //Network::ConnectionManager::getInstance().run();

    //auto lDispatcher = Network::EventDispatcher::getInstance();
    //lDispatcher->run();

    auto& lServer = Common::EventDispatcher::getInstance();
    try
    {
        lServer.run();
    }
    catch(std::exception& lE)
    {
        std::cout << lE.what() << "\n";
    }

    while(1);

    //while(1)
    //{
        //lThreadPool.delegate([](){std::cout << time(nullptr) << "\n";});
    //}

    //ThreadPool lThreadPool2(5, );

    //lThreadPool.stop();
    return EXIT_SUCCESS;
}
