#include <gtest/gtest.h>
#include <vector>

#include "util/thread_pool.hpp"
#include "util/callable.hpp"

uint32_t test_add(uint32_t &a, uint32_t b)
{
    a = 1000;
    return a + b;
}

// TEST(BoundedThreadPoolTest, BasicUsageTest)
// {
//     ThreadPool<> pool;
//     std::function<uint32_t(uint32_t, uint32_t)> add = [](uint32_t a, uint32_t b) -> uint32_t
//     { return a + b; };
//     std::future<uint32_t> fut = pool.Submit(std::bind(add, 5000, 5000));
//     ASSERT_TRUE(fut.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready);
//     ASSERT_EQ(fut.get(), 10000);
// }

TEST(BoundedThreadPoolTest, BasicUsageTestWithCallable)
{
    ThreadPool<> pool;
    std::function<uint32_t(uint32_t, uint32_t)> add = [](uint32_t a, uint32_t b) -> uint32_t
    { return a + b; };
    pool.Submit(
        Callable(std::bind(add, 5000, 5000)));
}

// TEST(BoundedThreadPoolTest, SharedStateTest)
// {
//     ThreadPool<> pool;

//     uint32_t x = 1;
//     std::function<uint32_t(uint32_t &, uint32_t)> add = [](uint32_t &a, uint32_t b) -> uint32_t
//     {
//         a = 1000;
//         return a + b;
//     };
//     std::future<uint32_t> fut1 = pool.Submit(std::bind(add, x, 1000));
//     ASSERT_TRUE(fut1.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready);
//     ASSERT_EQ(fut1.get(), 2000);
//     ASSERT_TRUE(x == 1);

//     std::future<uint32_t> fut2 = pool.Submit(std::bind(test_add, x, 1000));
//     ASSERT_TRUE(fut2.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready);
//     ASSERT_EQ(fut2.get(), 2000);
//     ASSERT_TRUE(x == 1);

//     struct MyInt
//     {
//         int value;
//         MyInt(int x_) : value(x_) {}
//     };
//     std::function<void(MyInt &)> add_10 = [](MyInt &a) -> void
//     {
//         a.value += 10;
//     };
//     MyInt mi{10};
//     std::future<void> fut3 = pool.Submit(std::bind(add_10, mi));
//     ASSERT_TRUE(fut3.wait_for(std::chrono::milliseconds(100000000)) == std::future_status::ready);
//     ASSERT_TRUE(mi.value == 10);
// }