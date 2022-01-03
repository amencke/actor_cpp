#include "thread_pool.hpp"

template <typename Iterable>
ThreadPool<Iterable>::ThreadPool() : done(false), joiner(threads)
{
    unsigned const thread_count = std::thread::hardware_concurrency();
    try
    {
        for (auto i = 0; i < thread_count; ++i)
        {
            threads.push_back(std::thread(&ThreadPool::WorkerThread, this));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

template <typename Iterable>
ThreadPool<Iterable>::~ThreadPool()
{
    done = true;
}

template <typename Iterable>
void ThreadPool<Iterable>::Submit(Callable c)
{
    work_queue.Push(std::move(c));
}

template <typename Iterable>
void ThreadPool<Iterable>::WorkerThread()
{
    while (!done)
    {
        Callable task;
        if (work_queue.TryPop(task))
        {
            task();
        }
        else
        {
            std::this_thread::yield();
        }
    }
}