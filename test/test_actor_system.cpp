#include <gtest/gtest.h>

#include <vector>

#include "abstract_actor.hpp"
#include "actor_system.hpp"
#include "scheduler.cpp"

#include "test_fixtures.hpp"

TEST(ActorSystemTest, BasicTellTest)
{
    ActorSystem system = ActorSystem();

    //class TestActor : public AbstractActor<Message, std::vector<int>>
    SortingActor t;
    system.Register(t);

    Message m{
        .v = std::vector<int>{5, 4, 3, 2, 1}};
    t.Tell(std::move(m), nullptr);

    // The behaviour specified in TestActor.OnMessageReceived should be scheduled
    // on the thread pool, which should result in the vector sent to the actor getting sorted
    while (!isSorted(m.v))
        ;
    ASSERT_TRUE(isSorted(m.v));

    system.Shutdown();
}

TEST(ActorSystemTest, BasicAskTest)
{
    ActorSystem system = ActorSystem();

    //class TestActor : public AbstractActor<Message, std::vector<int>>
    SortingActor t;
    system.Register(t);

    Message m{
        .v = std::vector<int>{5, 4, 3, 2, 1}};
    std::future<std::vector<int>> fut = t.Ask(std::move(m), nullptr);

    // The behaviour specified in TestActor.OnMessageReceivedWithResult should be scheduled
    // on the thread pool, which should result in the vector sent to the actor getting sorted
    ASSERT_TRUE(fut.valid());
    std::vector<int> res = fut.get();

    ASSERT_TRUE(isSorted(res));

    system.Shutdown();
}

TEST(ActorSystemTest, ReplyTest)
{
    ActorSystem system;
    SortingActor s;
    ReversingActor r;
    system.Register(s);
    system.Register(r);

    Message m{
        .v = std::vector<int>{5, 4, 3, 2, 1}};
    std::future<std::vector<int>> fut = s.Ask(std::move(m), &r); // pass the address of the reversing actor to allow "replying"

    // The behaviour specified in TestActor.OnMessageReceivedWithResult should be scheduled
    // on the thread pool, which should result in the vector sent to the actor getting sorted
    ASSERT_TRUE(fut.valid());
    std::vector<int> res = fut.get();

    ASSERT_TRUE(isReversed(res));

    system.Deregister(s); // not strictly required
    system.Deregister(r); 
    system.Shutdown();
}