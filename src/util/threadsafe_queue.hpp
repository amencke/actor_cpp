#ifndef THREADSAFE_QUEUE_H_
#define THREADSAFE_QUEUE_H_

#include <memory>
#include <mutex>
#include <exception>
#include <iostream>
#include <condition_variable>

template <typename T>
class BoundedThreadsafeQueue
{
private:
    struct Node
    {
        std::shared_ptr<T> data;
        std::unique_ptr<Node> next;
    };

    std::mutex head_mutex_;
    std::unique_ptr<Node> head_;
    std::mutex tail_mutex_;
    Node *tail_;
    std::condition_variable data_cond;
    uint32_t max_size_;
    std::atomic<uint32_t> size_{0};

private:
    Node *GetTail();
    std::unique_ptr<Node> PopHead();
    std::unique_ptr<Node> TryPopHead();
    std::unique_lock<std::mutex> WaitForData();
    std::unique_ptr<Node> WaitPopHead();

public:
    BoundedThreadsafeQueue() : head_(new Node), tail_(head_.get())
    {
        max_size_ = -1;
    }
    explicit BoundedThreadsafeQueue(uint32_t max_size) : head_(new Node), tail_(head_.get())
    {
        max_size_ = max_size;
    }
    BoundedThreadsafeQueue(const BoundedThreadsafeQueue &other) = delete;

    BoundedThreadsafeQueue &operator=(const BoundedThreadsafeQueue &other) = delete;

    std::shared_ptr<T> TryPop();
    bool TryPop(T &value);
    std::shared_ptr<T> WaitAndPop();
    void WaitAndPop(T &value);
    void Push(T new_value);
    bool Empty();
    uint32_t GetSize();
};

#include "threadsafe_queue.cpp" // avoid an "undefined symbol" linker error caused by the
                                // implementation in the cpp file not being available at link time

#endif // THREADSAFE_QUEUE_H_