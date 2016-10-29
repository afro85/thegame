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
    explicit MsgQueue() : iRelease(false) {}
    ~MsgQueue() {}

    auto size()
    {
        std::unique_lock<std::mutex> lLock{iAccessMutex};
        return iQueue.size();
    }
    void enqueue(const T& aItem);
    T dequeue(void);
    void release() {
        iRelease = true;
        iNotEmptyCV.notify_all();
    }
    bool isReleased() { return iRelease; }


private:
    std::mutex iAccessMutex;
    std::condition_variable iNotEmptyCV;
    std::queue<T> iQueue;
    std::atomic<bool> iRelease;
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
        iNotEmptyCV.wait(lLock, [&](){ return !(iQueue.empty()) || iRelease; });
    if (iRelease)
        return T();


    T lItem = iQueue.front();
    iQueue.pop();
    return lItem;
}
