/**
 * @file mycotask.h
 * @brief Stackful coroutine implementation with cooperative multitasking
 *
 * Provides a custom coroutine class supporting start, resume, and yield operations
 * with automatic context management and lifecycle tracking.
 */

#ifndef COROUTINES_MYCOTASK_H
#define COROUTINES_MYCOTASK_H

#include <cstddef>
#include <functional>
#include <memory>
#include <atomic>
#include "../coroutines.h"
#include <iostream>
#include <utility>
#include <cassert>

/**
 * @class mycotask
 * @brief Stackful coroutine with independent execution context
 *
 * A move-only coroutine class supporting cooperative multitasking. Each task
 * has its own stack and can suspend execution via yield(), allowing other
 * coroutines to run. Tasks must be explicitly started and can be resumed
 * until completion.
 *
 * @warning Do not remove its location from the source of creation.
 * Only std::move from not started coroutine is allowed.
 */
class mycotask {
private:
    static mycotask* current_task_;
    static coro_context* main_ctx_;
    static std::atomic<size_t> global_id_counter_;

    coro_context* ctx_;
    std::function<void()> func_;
    bool started_ = false;
    bool ended_ = false;
    std::atomic<size_t> id_;

    friend class mycomanager;
    friend class MycoNodeTraits;

    mycotask* next_ = nullptr;

    /**
     * @brief Trampoline function to execute user code
     * @param arg Pointer to std::function<void()>
     *
     * Internal wrapper that executes the user's coroutine function and
     * handles cleanup upon completion.
     */
    static void trampoline(void* arg);

    /**
     * @brief Construct coroutine from function object
     * @param f Function to execute
     */
    explicit mycotask(std::function<void()> f);

    /**
     * @brief Default constructor (internal use only)
     */
    mycotask();

    [[nodiscard]] mycotask* next() const { return next_; }

public:
    mycotask(const mycotask&) = delete;
    mycotask& operator=(const mycotask&) = delete;

    /**
     * @brief Move constructor
     * @param other Coroutine to move from
     */
    mycotask(mycotask&& other) noexcept;

    /**
     * @brief Move assignment operator
     * @param other Coroutine to move from
     * @return Reference to this
     */
    mycotask& operator=(mycotask&& other) noexcept;

    /**
     * @brief Destructor - cleans up stack and context
     */
    ~mycotask();

    /**
     * @brief Create a new coroutine task
     * @tparam F Function type
     * @tparam Args Argument types
     * @param f Function to execute in coroutine
     * @param args Arguments to bind to function
     * @return New mycotask instance
     *
     * Factory method to create coroutines from callable objects with arguments.
     * Arguments are bound immediately using std::bind.
     */
    template<typename F, typename... Args>
    static mycotask create_task(F&& f, Args&&... args) {
        return mycotask(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
    }

    /**
     * @brief Compare coroutines by ID
     * @param other Coroutine to compare with
     * @return true if IDs match
     */
    bool operator==(const mycotask& other) const {
        return this->id_ == other.id_;
    }

    /**
     * @brief Start coroutine execution
     *
     * Begins execution of the coroutine from the beginning. Can only be called
     * once per coroutine. Use resume() to continue after yielding.
     *
     * @throws Calls exit(EXIT_FAILURE) if already started
     */
    void start();

    /**
     * @brief Resume suspended coroutine execution
     *
     * Continues execution from where the coroutine last yielded. Must be called
     * after start() and cannot be called on completed coroutines.
     *
     * @throws Calls exit(EXIT_FAILURE) if not started or already ended
     */
    void resume();

    /**
     * @brief Yield execution back to caller
     *
     * Suspends current coroutine and returns control to the caller (main context
     * or scheduler). Execution can be resumed later with resume().
     */
    void yield() const;

    /**
     * @brief Get currently executing coroutine
     * @return Pointer to current task, or nullptr if in main context
     */
    static mycotask* current_task();

    /**
     * @brief Check if coroutine has completed execution
     * @return true if coroutine finished, false otherwise
     */
    [[nodiscard]] bool has_ended() const;

    /**
     * @brief Get unique coroutine identifier
     * @return Coroutine ID
     */
    [[nodiscard]] size_t get_id() const { return id_; }
};

#endif // COROUTINES_MYCOTASK_H
