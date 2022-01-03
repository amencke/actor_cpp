#include <gtest/gtest.h>
#include <thread>
#include <vector>

#include "util/threadsafe_queue.hpp"

TEST(BoundedThreadsafeQueueTest, BasicUsageTest)
{
    BoundedThreadsafeQueue<uint32_t> q;
    ASSERT_TRUE(q.Empty());
    q.Push(10);
    ASSERT_FALSE(q.Empty());
    q.TryPop();
    ASSERT_TRUE(q.Empty());
}

TEST(BoundedThreadsafeQueueTest, ThreadedUsageTest)
{
    BoundedThreadsafeQueue<uint32_t> q;
    std::vector<std::thread> threads;
    for (auto i = 0; i < 10; ++i)
    {
        threads.push_back(
            std::thread([&q, i]
                        { q.Push(i); }));
    }
    for (auto &t : threads)
    {
        t.join();
    }
    threads.clear();
    for (auto i = 0; i < 10; ++i)
    {
        threads.push_back(
            std::thread([&, i]
                        {
                            int val = *q.TryPop();
                            ASSERT_TRUE(val >= 0 && val <= 9);
                        }));
    }
    for (auto &t : threads)
    {
        t.join();
    }
    ASSERT_TRUE(q.Empty());
}

TEST(BoundedThreadsafeQueueTest, QueueBoundsExceededTest)
{
    BoundedThreadsafeQueue<uint32_t> bounded_q{10};
    std::vector<std::thread> threads;
    for (auto i = 0; i < 10; ++i)
    {
        threads.push_back(
            std::thread([&bounded_q, i]
                        { bounded_q.Push(i); }));
    }
    for (auto &t : threads)
    {
        t.join();
    }
    EXPECT_THROW(bounded_q.Push(1000), std::logic_error);
    bounded_q.TryPop();
    EXPECT_NO_THROW(bounded_q.Push(1000));
}