#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "util/threadsafe_list.hpp"
#include "util/thread_pool.hpp"
// #include "abstract_actor.hpp"

/*
A scheduler that checks for tasks generated as a result of messages
being sent to actors in the system, and dispatches these tasks on the 
system's thread pool.
*/

// Role of the scheduler:
// 1. Keep track of all actors in the system
// 2. Dispatch work from each actor onto the threadpool

// New actors need to be registered with the scheduler
// The actor system provides the interface to register actors
//

class Scheduler
{
public:
    Scheduler(std::unique_ptr<ThreadPool<>> poolPtr_);

    template <typename ActorType>
    void Register(ActorType &actor); // The compiler should be able to deduce the actor type. Take a reference to the actors queue here.

    template <typename ActorType>
    void Deregister(ActorType &actor);

    void Shutdown();

private:
    void Schedule();
    threadsafe_list<BoundedThreadsafeQueue<std::unique_ptr<Callable>> *> work_queues;
    std::unique_ptr<ThreadPool<>> poolPtr;
    std::atomic<bool> done;
};


#endif // SCHEDULER_H_