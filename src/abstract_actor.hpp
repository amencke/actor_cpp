#ifndef ABSTRACT_ACTOR_H_
#define ABSTRACT_ACTOR_H_

#include "util/threadsafe_queue.hpp"
#include "util/thread_pool.hpp"
#include "actor_system.hpp"

#include <queue>
#include <future>
#include <utility>
#include <tuple>
#include <memory>
#include <functional>

template <typename MessageType, typename ResultType>
class AbstractActor
{
public:
    AbstractActor() = default;
    AbstractActor(ActorSystem *system_) { system = system_; }
    virtual ~AbstractActor() {}
    /* Actor behaviour */
    virtual void OnMessageReceived(MessageType &&msg, AbstractActor *sender = nullptr) = 0;
    virtual ResultType OnMessageReceivedWithResult(MessageType msg, AbstractActor *sender = nullptr) = 0;

    /* Actor interface */
    void Tell(MessageType &&msg, AbstractActor *sender = nullptr)
    {
        Callable callable{[&]
                          { OnMessageReceived(std::move(msg), sender); }};
        std::packaged_task<void()> pt{std::move(callable)};
        std::unique_ptr<Callable> ptUptr{new Callable(std::move(pt))};
        queue.Push(std::move(ptUptr));
    }

    std::future<ResultType> Ask(MessageType msg, AbstractActor *sender = nullptr)
    {
        // TODO: figure out how to use move semantics for the passed in message here. Could not get the permutations of move-only
        //  and copy-only types to play nicely here ("call to explicitly deleted X")
        auto func = [=]()
        { return this->OnMessageReceivedWithResult(msg, sender); };
        std::packaged_task<ResultType()> pt = std::packaged_task<ResultType()>{func};
        // NOTE: I ran into a (probably typical unique_ptr) bug here with having `return ptUptr->get_future();` as the final statement. Once `ptUptr` is
        // pushed onto the queue, `ptUptr` actually becomes a `nullptr` because the reference in the queue now "owns" the instance it is pointing to.
        auto fut = pt.get_future();
        //std::packaged_task<ResultType()> *ptPtr = new std::packaged_task<ResultType()>{std::move(pt)};
        std::unique_ptr<Callable> ptUptr{new Callable(std::move(pt))};
        queue.Push(std::move(ptUptr));
        return fut;
    }

    uint32_t GetQueueSize() { return queue.GetSize(); };
    BoundedThreadsafeQueue<std::unique_ptr<Callable>> *GetQueueRef() { return &queue; };

private:
    BoundedThreadsafeQueue<std::unique_ptr<Callable>> queue;
    ActorSystem *system;
};

#endif // ABSTRACT_ACTOR_H_