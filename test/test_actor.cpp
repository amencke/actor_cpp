#include <gtest/gtest.h>
#include <vector>
#include <algorithm>

#include "abstract_actor.hpp"
#include "test_fixtures.hpp"

TEST(ActorTest, BasicTellTest)
{
    Message m{
        .v = std::vector<int>{5, 4, 3, 2, 1}};
    SortingActor t;
    t.Tell(std::move(m), nullptr);
}

TEST(ActorTest, BasicAskTest)
{
    Message m{
        .v = std::vector<int>{5, 4, 3, 2, 1}};
    SortingActor t;
    std::future<std::vector<int>> fut = t.Ask(m, nullptr);

    ASSERT_TRUE(fut.valid());
    // fut.get() would block forever, because the packaged_task that would complete this future is never scheduled
}