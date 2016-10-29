#pragma once

#include <iostream>
#include <algorithm>
#include <thread>

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

private:
    const std::function<void(void)> iThreadBody = [this](){
        while (1)
        {
            auto lWork = this->iWorkQueue.dequeue();
            lWork();
        }
    };

    std::vector<std::thread> iPool;
    MsgQueue<Work> iWorkQueue;
};
