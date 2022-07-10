#include <type_traits>
#include <algorithm>

struct Message
{
    std::vector<int> v;
};

struct Response
{
    uint32_t i;
};

class SortingActor : public AbstractActor<Message, std::vector<int>>
{
    virtual void OnMessageReceived(Message &&msg, AbstractActor *sender = nullptr) override final
    {
        std::sort(msg.v.begin(), msg.v.end());
    }

    virtual std::vector<int> OnMessageReceivedWithResult(Message msg, AbstractActor *sender = nullptr) override final
    {
        std::sort(msg.v.begin(), msg.v.end());
        std::vector<int> res = msg.v;

        if (sender)
        {
            std::future<std::vector<int>> fut = sender->Ask(msg);
            res = fut.get();
        }
        return res;
    }
};

class ReversingActor : public AbstractActor<Message, std::vector<int>>
{
    virtual void OnMessageReceived(Message &&msg, AbstractActor *sender = nullptr) override final
    {
        std::sort(msg.v.rbegin(), msg.v.rend());
    }

    virtual std::vector<int> OnMessageReceivedWithResult(Message msg, AbstractActor *sender = nullptr) override final
    {
        std::sort(msg.v.rbegin(), msg.v.rend());
        return msg.v;
    }
};

class AccumulatingActor : public AbstractActor<Message, Response>
{
};

template <typename... Ts>
struct make_void
{
    typedef void type;
};
template <typename... Ts>
using void_t = typename make_void<Ts...>::type;

template <typename T, typename = void>
struct is_iterable : std::false_type
{
};

template <typename T>
struct is_iterable<T, void_t<decltype(std::declval<T>().begin()),
                             decltype(std::declval<T>().end())>>
    : std::true_type
{
};

template <typename T, typename = typename std::enable_if<is_iterable<T>::value, T>::type>
bool isSorted(T iterable)
{
    for (auto i = iterable.begin() + 1; i != iterable.end(); ++i)
    {
        if (*i < *(i - 1))
        {
            return false;
        }
    }
    return true;
}

template <typename T, typename = typename std::enable_if<is_iterable<T>::value, T>::type>
bool isReversed(T iterable)
{
    for (auto i = iterable.begin() + 1; i != iterable.end(); ++i)
    {
        if (*i > *(i - 1))
        {
            return false;
        }
    }
    return true;
}