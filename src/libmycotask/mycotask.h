//
// Created by julfy on 10/12/25.
//

#ifndef COROUTINES_MYCOTASK_H
#define COROUTINES_MYCOTASK_H

#include <cstring>
#include <memory>
#include <vector>
#include "coroutines.h"



class mycotask {
private:
    coro_context ctx_{};
    std::vector<uint8_t> stack_;
    void (*func_)();
    bool started_ = false;

    static coro_context main_ctx_;
    static mycotask* current_task_;

    void setup_coroutine();
    static void wrapper_func();

public:
    explicit mycotask(void (*func)(), size_t stack_size = 16384);

    void start();
    void resume();
    void yield();

    static mycotask* current_task();
};

#endif // COROUTINES_MYCOTASK_H
