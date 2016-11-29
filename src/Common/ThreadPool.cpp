#include <Common/ThreadPool.h>

Common::ThreadPool::ThreadPool(size_t aThreadsNumber) : mStop(false)
{
    mPool.resize(aThreadsNumber);
    std::generate(mPool.begin(), mPool.end(),
        [this]() -> auto { return std::thread(this->mThreadBody);});
}
