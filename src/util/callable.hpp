#ifndef CALLABLE_H_
#define CALLABLE_H_

class Callable
{
    // This wraps a function and makes it move-only. std::function callables are
    // copy-constructible, which would not be thread safe since the std::function
    // could be modified or destroyed anytime after after being passed to a
    // std::packaged_task
private:
    struct ImplBase
    {
        virtual void call() = 0;
        virtual ~ImplBase() {}
    };
    std::unique_ptr<ImplBase> impl;

    template <typename F>
    struct ImplType : ImplBase
    {
        F f;
        ImplType(F &&f_) : f(std::move(f_)) {}
        void call() { f(); }
    };

    template <typename F>
    struct ImplType<std::unique_ptr<F>> : ImplBase
    {
        // Specialization for unique_ptr pointers to callable types
        F f;
        ImplType(F f_) : f(std::move(f_)) {}
        void call()
        {
            f();
        }
    };

public:
    template <typename F>
    Callable(F &&f)
    {
        impl = std::unique_ptr<ImplBase>{new ImplType<F>(std::move(f))};
    }
    void operator()()
    {
        impl->call();
    }
    Callable() = default;
    Callable(Callable &&other) : impl(std::move(other.impl))
    {
    }
    Callable &operator=(Callable &&other)
    {
        impl = std::move(other.impl);
        return *this;
    }
    Callable(const Callable &) = delete;
    Callable(Callable &) = delete;
    Callable &operator=(const Callable &) = delete;
};

#endif // CALLABLE_H_