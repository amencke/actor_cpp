#include "scheduler.hpp"

Scheduler::Scheduler(std::unique_ptr<ThreadPool<>> poolPtr_) : poolPtr(std::move(poolPtr_)), work_queues(), done(false)
{
    auto func = [this]()
    { return this->Schedule(); };
    poolPtr->Submit(Callable(std::move(func)));
}

template <typename ActorType>
void Scheduler::Register(ActorType &actor)
{
    if (!done)
    {
        work_queues.push_front(actor.GetQueueRef());
    }
}

template <typename ActorType>
void Scheduler::Deregister(ActorType &actor)
{

    work_queues.remove_if([&actor](BoundedThreadsafeQueue<std::unique_ptr<Callable>> *qPtr)
                          { return (qPtr == actor.GetQueueRef()); });
}

void Scheduler::Schedule()
{
    while (!done)
    {
        work_queues.for_each([=](BoundedThreadsafeQueue<std::unique_ptr<Callable>> *qPtr)
                             {
                                 while (!qPtr->Empty()) // `while` because calling shutdown should not prevent submitted work from being scheduled
                                 {
                                     std::shared_ptr<std::unique_ptr<Callable>> workPtr = qPtr->WaitAndPop();
                                     poolPtr->Submit(std::move(*(*workPtr)));
                                     workPtr = nullptr;
                                 }
                             });
    }
}

void Scheduler::Shutdown()
{
    done = true;
}