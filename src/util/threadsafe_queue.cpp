#include "threadsafe_queue.hpp"

template <typename T>
typename BoundedThreadsafeQueue<T>::Node *BoundedThreadsafeQueue<T>::GetTail()
{
    std::lock_guard<std::mutex> tail_lock{tail_mutex_};
    return tail_;
}

template <typename T>
std::unique_ptr<typename BoundedThreadsafeQueue<T>::Node> BoundedThreadsafeQueue<T>::PopHead()
{
    std::unique_ptr<Node> old_head = std::move(head_);
    head_ = std::move(old_head->next);
    return old_head;
}

template <typename T>
std::unique_ptr<typename BoundedThreadsafeQueue<T>::Node> BoundedThreadsafeQueue<T>::TryPopHead()
{
    std::lock_guard<std::mutex> head_lock(head_mutex_);
    if (head_.get() == GetTail())
    {
        return std::unique_ptr<Node>();
    }
    size_.fetch_sub(1);
    return PopHead();
}

template <typename T>
std::unique_lock<std::mutex> BoundedThreadsafeQueue<T>::WaitForData()
{
    std::unique_lock<std::mutex> head_lock(head_mutex_);
    data_cond.wait(head_lock, [&]
                   { return head_.get() != GetTail(); });
    return std::move(head_lock);
}

template <typename T>
std::unique_ptr<typename BoundedThreadsafeQueue<T>::Node> BoundedThreadsafeQueue<T>::WaitPopHead()
{
    std::unique_lock<std::mutex> head_lock(WaitForData());
    return PopHead();
}

template <typename T>
std::shared_ptr<T> BoundedThreadsafeQueue<T>::WaitAndPop()
{
    std::unique_ptr<Node> const old_head = WaitPopHead();
    return old_head->data;
}

template <typename T>
bool BoundedThreadsafeQueue<T>::TryPop(T &value)
{
    std::unique_ptr<Node> old_head = TryPopHead();
    if (old_head)
    {
        value = std::move(*old_head->data);
        return true;
    }
    return false;
}

template <typename T>
std::shared_ptr<T> BoundedThreadsafeQueue<T>::TryPop()
{
    std::unique_ptr<Node> old_head = TryPopHead();
    return old_head ? old_head->data : std::shared_ptr<T>();
}

template <typename T>
void BoundedThreadsafeQueue<T>::Push(T new_value)
{
    if (size_.load(std::memory_order_relaxed) == max_size_)
    {
        throw std::length_error("queue size exceeded limit!");
    }
    std::shared_ptr<T> new_data(
        std::make_shared<T>(std::move(new_value)));
    std::unique_ptr<Node> p(new Node);
    Node *const new_tail = p.get();
    std::lock_guard<std::mutex> tail_lock(tail_mutex_);
    tail_->data = new_data;
    tail_->next = std::move(p);
    tail_ = new_tail;
    size_.fetch_add(1);
}

template <typename T>
bool BoundedThreadsafeQueue<T>::Empty()
{
    std::lock_guard<std::mutex> lk{head_mutex_};
    return head_.get() == GetTail();
}

template <typename T>
uint32_t BoundedThreadsafeQueue<T>::GetSize()
{
    return size_.load(std::memory_order_relaxed);
}