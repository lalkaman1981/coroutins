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

/**
 * Custom coroutine class.
 * Do not remove its location from the source of creation
 * For now, only std::move from not started coroutine is allowed
 * <(raw pointer in the private fields)>
 */
class mycotask
{
private:
    static mycotask* current_task_;
    static coro_context* main_ctx_;

    coro_context* ctx_;
    // void (*func_)();
    std::function<void()> func_;
    bool started_ = false;
    bool ended_ = false;

    static void trampoline(void* arg) {
        const auto* fn = static_cast<std::function<void()>*>(arg);
        // run user code
        (*fn)();
        // a func returned
        current_task_ -> started_ = false;
        current_task_ -> ended_ = true;

        switch_context(current_task_->ctx_, main_ctx_);

        volatile int prevent_optimization = 0;
        (void)prevent_optimization;
    }

    explicit mycotask(std::function<void()> f)
        : func_(std::move(f))
    {
        ctx_ = create_coro_context();
    }

public:

    // can not copy this object, move only
    mycotask(const mycotask&) = delete;
    mycotask& operator=(const mycotask&) = delete;

    mycotask(mycotask&& other) noexcept
        : ctx_(other.ctx_), func_(std::move(other.func_)), started_(other.started_) {
        other.ctx_ = nullptr;
        other.started_ = false;
        other.ended_ = false;
    }

    mycotask& operator=(mycotask&& other) noexcept {
        if (this != &other) {
            ctx_ = other.ctx_;
            func_ = std::move(other.func_);
            started_ = other.started_;
            other.ctx_ = nullptr;
            other.started_ = false;
            other.ended_ = false;
        }
        return *this;
    }

    template<typename F, typename... Args>
    static mycotask create_task(F&& f, Args&&... args) {
        return mycotask(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
    }


    void start();
    void resume();
    void yield() const;
    bool has_ended() const;

    static mycotask* current_task();
};


#endif // COROUTINES_MYCOTASK_H
