#include <server/ThreadPool.hh>

ThreadPool::ThreadPool(size_t aThreadsNumber)
{
    iPool.resize(aThreadsNumber);
    std::generate(iPool.begin(), iPool.end(),
        [this]() -> auto { return std::thread(this->iThreadBody);});
}
