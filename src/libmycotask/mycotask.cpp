//
// Created by julfy on 10/12/25.
//

#include "mycotask.h"

coro_context mycotask::main_ctx_{};
mycotask* mycotask::current_task_ = nullptr;

void mycotask::setup_coroutine() {
    memset(&ctx_, 0, sizeof(coro_context));

    uintptr_t sp = reinterpret_cast<uintptr_t>(stack_.data() + stack_.size());
    sp &= -16L;

    sp -= 8; *reinterpret_cast<uint64_t*>(sp) = reinterpret_cast<uint64_t>(func_);
    sp -= 8; *reinterpret_cast<uint64_t*>(sp) = 0x202;
    sp -= 8 * 6;

    ctx_.old_rsp = sp;
    ctx_.stack_base = stack_.data();
}

mycotask::mycotask(void (*func)(), size_t stack_size)
    : stack_(stack_size), func_(func) {
    setup_coroutine();
}

void mycotask::start() {
    if (!started_) {
        started_ = true;
        switch_context(&main_ctx_, &ctx_);
    }
}

void mycotask::resume() {
    if (started_) {
        switch_context(&main_ctx_, &ctx_);
    }
}

void mycotask::yield() {
    switch_context(&ctx_, &main_ctx_);
}

mycotask* mycotask::current_task() {
    return current_task_;
}
