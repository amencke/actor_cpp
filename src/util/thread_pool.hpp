#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include <thread>
#include <vector>
#include <future>
#include <type_traits>

#include "threadsafe_queue.hpp"
#include "callable.hpp"

template <typename Iterable = std::vector<std::thread>>
class ThreadPool
{
    class ThreadJoiner
    {
        Iterable &threads;

    public:
        explicit ThreadJoiner(Iterable &threads_) : threads(threads_)
        {
        }
        ~ThreadJoiner()
        {
            for (auto it = threads.begin(); it != threads.end(); ++it)
            {
                if (it->joinable())
                {
                    it->join();
                }
            }
        }
    };

private:
    std::atomic<bool> done;
    BoundedThreadsafeQueue<Callable> work_queue;
    Iterable threads;
    ThreadJoiner joiner;

public:
    ThreadPool();
    ~ThreadPool();
    void Submit(Callable c);

private:
    void WorkerThread();
};

#include "thread_pool.cpp"

#endif // THREAD_POOL_H_