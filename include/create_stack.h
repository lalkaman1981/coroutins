#ifndef CREATE_STACK
#define CREATE_STACK

#include <iostream>
#include <functional>

extern "C" void change_stack(void (*fn)(void*), void* arg);

extern "C" void call_fn(void* p);

template<typename F, typename... Args>
void call_coro(F&& f, Args&&... args) {
    auto bound = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    auto* pf = new std::function<void()>(std::move(bound));
    change_stack(&call_fn, pf);
}

#endif