//
// Created by julfy on 10/12/25.
//

#include "mycotask.h"


coro_context* mycotask::main_ctx_ = create_coro_context();
mycotask* mycotask::current_task_ = nullptr;

void mycotask::start() {
    if (started_) {
        std::cerr << "mycotask: you can only start not started coroutines! Please call mycotask::resume() now!" << std::endl;
        exit(EXIT_FAILURE);
    }

    started_ = true;
    current_task_ = this;

    // copy func_ onto heap for the trampoline
    auto* pf = new std::function<void()>(func_);
    create_coro_stack(&trampoline, pf, ctx_, main_ctx_);

    switch_context(main_ctx_, ctx_);
}



void mycotask::resume() {
    if (ended_) {
        std::cerr << "mycotask: the task has ended!" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (!started_) {
        std::cerr << "mycotask: you can only resume a started coroutine! Please call mycotask::start() first!" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "RESUME CALLED" << std::endl;
    current_task_ = this;

    switch_context(main_ctx_, ctx_);

}

void mycotask::yield() const
{
    switch_context(ctx_, main_ctx_);
    // volatile int prevent_optimization = 0;
    // (void)prevent_optimization;
}

bool mycotask::has_ended() const { return ended_;}

mycotask* mycotask::current_task() {return current_task_;}
