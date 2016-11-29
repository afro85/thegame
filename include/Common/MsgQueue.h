#pragma once

#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace Common {

template <typename T>
class MsgQueue {
public:
    using Ptr = std::shared_ptr<MsgQueue>;

public:
    explicit MsgQueue() : mRelease(false) {}
    ~MsgQueue() {}

    auto size() {
        std::unique_lock<std::mutex> lLock{mAccessMutex};
        return mQueue.size();
    }
    void enqueue(const T& aItem);
    T dequeue(void);
    void release() {
        mRelease = true;
        mNotEmptyCV.notify_all();
    }
    bool isReleased() { return mRelease; }


private:
    std::mutex mAccessMutex;
    std::condition_variable mNotEmptyCV;
    std::queue<T> mQueue;
    std::atomic<bool> mRelease;
};

template <typename T>
void MsgQueue<T>::enqueue(const T& aItem) {
    std::unique_lock<std::mutex> lLock{mAccessMutex};
    mQueue.push(std::move(aItem));
    mNotEmptyCV.notify_one();
}

template <typename T>
T MsgQueue<T>::dequeue(void) {
    std::unique_lock<std::mutex> lLock{mAccessMutex};

    if (mQueue.empty())
        mNotEmptyCV.wait(lLock, [&](){ return !(mQueue.empty()) || mRelease; });
    if (mRelease)
        return T();

    T lItem = mQueue.front();
    mQueue.pop();
    return lItem;
}

}
