/**
 * @file mycotask.cpp
 * @brief Implementation of stackful coroutine operations
 *
 * Provides lifecycle management, context switching, and execution control
 * for cooperative multitasking coroutines.
 */

#include "mycotask.h"

coro_context *mycotask::main_ctx_ = create_coro_context();
mycotask *mycotask::current_task_ = nullptr;
std::atomic<size_t> mycotask::global_id_counter_ = 69;

void mycotask::trampoline(void *arg) {
    const auto *fn = static_cast<std::function<void()> *>(arg);
    (*fn)();
    delete fn;
    current_task_->started_ = false;
    current_task_->ended_ = true;
    switch_context(current_task_->ctx_, main_ctx_);
    volatile int prevent_optimization = 0;
    (void)prevent_optimization;
}

mycotask::mycotask(std::function<void()> f) : func_(std::move(f)) {
    ctx_ = create_coro_context();
    id_ = global_id_counter_.fetch_add(1, std::memory_order_relaxed);
}

mycotask::mycotask() {
    mycotask::create_task([] {
        std::cerr << "This is a default initialization for cotask" << std::endl;
        exit(EXIT_FAILURE);
    });
}

mycotask::mycotask(mycotask &&other) noexcept
    : ctx_(other.ctx_), func_(std::move(other.func_)), started_(other.started_),
      ended_(other.ended_), next_(std::move(other.next_)) {
    id_.store(other.id_.load(std::memory_order_relaxed),
              std::memory_order_relaxed);
    other.next_ = nullptr;
    other.ctx_ = nullptr;
    other.started_ = false;
    other.ended_ = false;
    other.id_.store(0, std::memory_order_relaxed);
}

mycotask &mycotask::operator=(mycotask &&other) noexcept {
    if (this != &other) {
        ctx_ = other.ctx_;
        func_ = std::move(other.func_);
        started_ = other.started_;
        ended_ = other.ended_;
        id_.store(other.id_.load(std::memory_order_relaxed),
                  std::memory_order_relaxed);
        other.ctx_ = nullptr;
        other.started_ = false;
        other.ended_ = false;
        other.id_.store(0, std::memory_order_relaxed);
    }
    return *this;
}

mycotask::~mycotask() {
    if (ctx_) {
        delete[] ctx_->stack_base;
        ctx_->stack_base = nullptr;
        ctx_->stack_top = nullptr;
        delete ctx_;
        ctx_ = nullptr;
    }
}

void mycotask::start() {
    if (started_) {
        std::cerr << "mycotask: you can only start not started coroutines! Please "
                     "call mycotask::resume() now!"
                  << std::endl;
        exit(EXIT_FAILURE);
    }

    started_ = true;
    current_task_ = this;
    auto *pf = new std::function<void()>(func_);
    create_coro_stack(&trampoline, pf, ctx_, main_ctx_);
}

void mycotask::resume() {
    if (ended_) {
        std::cerr << "mycotask: the task has ended!" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!started_) {
        std::cerr << "mycotask: you can only resume a started coroutine! Please "
                     "call mycotask::start() first!"
                  << std::endl;
        exit(EXIT_FAILURE);
    }

    current_task_ = this;
    switch_context(main_ctx_, ctx_);
}

void mycotask::yield() const { switch_context(ctx_, main_ctx_); }

bool mycotask::has_ended() const { return ended_; }

mycotask *mycotask::current_task() { return current_task_; }
