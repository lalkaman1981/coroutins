//
// Created by julfy on 10/12/25.
//

#ifndef COROUTINES_MYCOTASK_H
#define COROUTINES_MYCOTASK_H

#include <atomic>
#include <cstring>
#include <memory>
#include <vector>
#include "../coroutines.h"
#include <cstdlib>
#include <functional>
#include <iostream>

/**
 * Custom coroutine class.
 * Do not remove its location from the source of creation
 * For now, only std::move from not started coroutine is allowed
 * <(raw pointer in the private fields)>
 */
class mycotask {
private:
    static mycotask* current_task_;
    static coro_context* main_ctx_;
    static std::atomic<std::size_t> global_id_counter_;

    coro_context* ctx_;
    std::function<void()> func_;
    bool started_ = false;
    bool ended_ = false;
    std::atomic<std::size_t> id_;

    friend class mycomanager;
    friend class MycoNodeTraits;

    // for mycomanager
    mycotask* next_ = nullptr;

    // wrapper function
    static void trampoline(void* arg);

    explicit mycotask(std::function<void()> f);

    // void switch_2coro_execution(); // may be needed here (ask petro xd)

    mycotask(); // should not be called. to satisfy the compiler only

    [[nodiscard]] mycotask* next() const { return next_; }

public:
    // can not copy this object, move only
    mycotask(const mycotask&) = delete;
    mycotask& operator=(const mycotask&) = delete;

    mycotask(mycotask&& other) noexcept;
    mycotask& operator=(mycotask&& other) noexcept;

    ~mycotask();

    template<typename F, typename... Args>
    static mycotask create_task(F&& f, Args&&... args) {
        return mycotask(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
    }

    bool operator==(const mycotask& other) const {
        return this->id_ == other.id_;
    }

    void start();
    void resume();
    void yield() const;

    static mycotask* current_task();

    [[nodiscard]] bool has_ended() const;
    [[nodiscard]] size_t get_id() const { return id_; }
};


#endif // COROUTINES_MYCOTASK_H
