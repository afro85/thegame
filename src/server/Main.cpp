#include <cstdlib>
#include <unistd.h>
#include <iostream>

#include <server/ThreadPool.hh>

int main()
{
    std::cout << "The Game has just been begun\n";
 
    ThreadPool lThreadPool(5);

    while(1)
    {
        lThreadPool.delegate([](){std::cout << time(nullptr) << "\n";});
        //sleep(1);
    }

    //ThreadPool lThreadPool2(5, );

    return EXIT_SUCCESS;
}
