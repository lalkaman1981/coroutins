//
// Created by julfy on 10/12/25.
//

#ifndef COROUTINES_MYCOTASK_H
#define COROUTINES_MYCOTASK_H

#include <cstring>
#include <memory>
#include <vector>
#include "coroutines.h"
#include <cstdlib>


class mycotask {
private:
    coro_context* ctx_;
    void (*func_)();
    bool started_ = false;

    static mycotask* current_task_;

    static void wrapper_func();

public:
    static coro_context* main_ctx_;

    explicit mycotask(void (*func)());

    void resume();
    void yield();

    static mycotask* current_task();

    template <typename... Args>
    void start(Args &&...args) {
        if (!started_) {
            started_ = true;
            current_task_ = this;
            call_coro(main_ctx_, ctx_, wrapper_func, std::forward<Args>(args)...);
        }
        else {
            std::cerr << "mycotask: you can only start not started coroutines! Please call mycotask::resume() now!" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

};

#endif // COROUTINES_MYCOTASK_H
