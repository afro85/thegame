#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class MsgQueue
{
public:
    using Ptr = std::shared_ptr<MsgQueue>;

public:
    explicit MsgQueue() {}
    ~MsgQueue() {}

    auto size()
    {
        std::unique_lock<std::mutex> lLock{iAccessMutex};
        return iQueue.size();
    }
    void enqueue(const T& aItem);
    T dequeue(void);

private:
    std::mutex iAccessMutex;
    std::condition_variable iNotEmptyCV;
    std::queue<T> iQueue;
};

template <typename T>
void MsgQueue<T>::enqueue(const T& aItem)
{
    std::unique_lock<std::mutex> lLock{iAccessMutex};
    iQueue.push(std::move(aItem));
    iNotEmptyCV.notify_one();
}

template <typename T>
T MsgQueue<T>::dequeue(void)
{
    std::unique_lock<std::mutex> lLock{iAccessMutex};

    if (iQueue.empty())
        iNotEmptyCV.wait(lLock, [this](){ return !(this->iQueue.empty()); });

    T lItem = iQueue.front();
    iQueue.pop();
    return lItem;
}
