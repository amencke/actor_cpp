#ifndef ACTOR_SYSTEM_H_
#define ACTOR_SYSTEM_H_

#include "scheduler.hpp"
#include "util/thread_pool.hpp"

class ActorSystem
{
private:
    std::unique_ptr<Scheduler> scheduler;

public:
    ActorSystem()
    {
        std::unique_ptr<ThreadPool<>> poolUptr{new ThreadPool<>()};
        scheduler = std::unique_ptr<Scheduler>{new Scheduler(std::move(poolUptr))};
    }

    template <typename ActorType>
    void Register(ActorType &actor)
    {
        scheduler->Register(actor);
    }

    template <typename ActorType>
    void Deregister(ActorType &actor)
    {
        scheduler->Deregister(actor);
    }

    void Shutdown()
    {
        scheduler->Shutdown();
    }
};

#endif // ACTOR_SYSTEM_H_