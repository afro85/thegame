#pragma once

#include <Common/MsgQueue.h>

#include <iostream>
#include <algorithm>
#include <thread>
#include <atomic>

namespace Common {

class ThreadPool
{
public:
    using Ptr = std::shared_ptr<ThreadPool>;
    using Work = std::function<void(void)>;

public:
    explicit ThreadPool(size_t aThreadsNumber);
    ThreadPool(const ThreadPool& ) = delete;
    virtual ~ThreadPool() {
        stop();
    }

    void delegate(const Work& aWork) {
        mWorkQueue.enqueue(aWork);
    }

private:
    void stop() {
        mStop = true;
        mWorkQueue.release();
        std::for_each(mPool.begin(), mPool.end(), [](auto& lIt) { lIt.join(); });
    }

private:
    const std::function<void(void)> mThreadBody = [this]() {
        while (!mStop) {
            auto lWork = this->mWorkQueue.dequeue();
            if (this->mWorkQueue.isReleased())
                break;
            lWork();
        }
    };

    std::vector<std::thread> mPool;
    MsgQueue<Work> mWorkQueue;
    std::atomic<bool> mStop;
};

}
