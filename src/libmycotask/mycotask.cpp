//
// Created by julfy on 10/12/25.
//

#include "mycotask.h"


coro_context* mycotask::main_ctx_ = create_coro_context();
mycotask* mycotask::current_task_ = nullptr;


void mycotask::wrapper_func() {
    // std::cout << "A wrapper called!" << std::endl;
    mycotask* self = current_task_;

    self->func_();
    // std::cout << "Hello" << std::endl;

    // a func returned
    self->started_ = false;
    current_task_ = nullptr;

    switch_context(self->ctx_, main_ctx_);
    volatile int prevent_optimization = 0;
    (void)prevent_optimization;
}


mycotask::mycotask(void (*func)())
    : func_(func) {
    ctx_ = create_coro_context();
}

void mycotask::start() {
    if (!started_) {
        started_ = true;
        current_task_ = this;
        call_coro(main_ctx_, ctx_, wrapper_func);
    }
    else {
        std::cerr << "mycotask: you can only start not started coroutines! Please call mycotask::resume() now!" << std::endl;
    }
}

void mycotask::resume() {
    if (started_) {
        current_task_ = this;

        switch_context(main_ctx_, ctx_);
    }
    else {
        std::cerr << "mycotask: you can only resume a started coroutine! Please call mycotask::start() first!" << std::endl;
    }
    volatile int prevent_optimization = 0;
    (void)prevent_optimization;
}

void mycotask::yield() {
    switch_context(ctx_, main_ctx_);
    volatile int prevent_optimization = 0;
    (void)prevent_optimization;
}

mycotask* mycotask::current_task() {
    return current_task_;
}
