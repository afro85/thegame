#include <Common/ThreadPool.h>

Common::ThreadPool::ThreadPool(size_t aThreadsNumber) : iStop(false)
{
    iPool.resize(aThreadsNumber);
    std::generate(iPool.begin(), iPool.end(),
        [this]() -> auto { return std::thread(this->iThreadBody);});
}
