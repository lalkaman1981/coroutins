#ifndef CREATE_STACK
#define CREATE_STACK

#include <iostream>
#include <functional>
#include <string>

#include "coroutines.h"

extern "C" void create_coro_stack(void (*fn)(void*), void* arg, coro_context* new_context, coro_context* old_context);

extern "C" void create_world_context_s(coro_context* world_context);

extern "C" void call_fn(void* p);

coro_context* create_world_context();

template<typename F, typename... Args>
void call_coro(coro_context* old_context, coro_context* new_context , F&& f, Args&&... args) {
    auto bound = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    auto* pf = new std::function<void()>(std::move(bound));
    create_coro_stack(&call_fn, pf, new_context, old_context);
}

coro_context* create_coro_context();

#endif
