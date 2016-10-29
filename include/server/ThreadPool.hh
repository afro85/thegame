#pragma once

#include <iostream>
#include <algorithm>
#include <thread>
#include <atomic>

#include "MsgQueue.hh"

class ThreadPool
{
public:
    using Ptr = std::shared_ptr<ThreadPool>;
    using Work = std::function<void(void)>;

public:
    explicit ThreadPool(size_t aThreadsNumber);
    ThreadPool(const ThreadPool& ) = delete;
    virtual ~ThreadPool() {}

    void delegate(const Work& aWork)
    {
        iWorkQueue.enqueue(aWork);
    }

    void stop()
    {
        iStop = true;
        iWorkQueue.release();
        std::for_each(iPool.begin(), iPool.end(), [](auto& lIt){lIt.join();});
    }

private:
    const std::function<void(void)> iThreadBody = [this](){
        while (!iStop)
        {
            auto lWork = this->iWorkQueue.dequeue();
            if (this->iWorkQueue.isReleased())
                break;
            lWork();
        }
    };

    std::vector<std::thread> iPool;
    MsgQueue<Work> iWorkQueue;
    std::atomic<bool> iStop;
};
