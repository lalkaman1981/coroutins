/**
 * @file myfp.h
 * @brief Promise/Future pattern implementation for coroutines
 *
 * Provides asynchronous result handling through Promise and Future objects,
 * allowing coroutines to wait for values produced by other coroutines.
 */

/** @addtogroup async
 *  @{
 */

#pragma once

#include <exception>
#include <vector>
#include <utility>
#include <type_traits>
#include "../mycomanager/mycomanager.h"
#include "../mycotask/mycotask.h"

/**
 * @class MyPromise
 * @brief Producer side of promise-future pair
 * @tparam T Type of value to be produced
 *
 * Stores a result value and notifies waiting coroutines when ready.
 * Supports exception propagation for error handling.
 */
template <typename T>
class MyPromise {
public:
    T value{};                          /**< Stored result value */
    bool ready = false;                 /**< Indicates if value is ready */
    std::exception_ptr exception;       /**< Stored exception if error occurred */
    std::vector<mycotask*> waiters;     /**< Coroutines waiting for result */
    mycomanager *manager = nullptr;     /**< Associated coroutine manager */

    /**
     * @brief Construct promise with manager
     * @param mgr Coroutine manager handling waiters
     */
    explicit MyPromise(mycomanager *mgr) : manager(mgr) {}

    /**
     * @brief Set the result value and wake waiters
     * @param v Value to store
     *
     * Stores the provided value, marks promise as ready, and resumes
     * all coroutines waiting on the associated future.
     */
    void set_value(T v) {
        value = std::move(v);
        ready = true;
        wake_waiters();
    }

    /**
     * @brief Set exception and wake waiters
     * @param e Exception pointer to store
     *
     * Stores an exception, marks promise as ready, and resumes all
     * waiting coroutines which will rethrow the exception.
     */
    void set_exception(std::exception_ptr e) {
        exception = e;
        ready = true;
        wake_waiters();
    }

private:
    void wake_waiters() {
        for (mycotask *t : waiters)
            t->resume();
        waiters.clear();
    }
};

/**
 * @class MyPromise<void>
 * @brief Promise specialization for void return type
 *
 * Handles completion notification without storing a value.
 */
template <>
class MyPromise<void> {
public:
    bool ready = false;
    std::exception_ptr exception;
    std::vector<mycotask*> waiters;
    mycomanager *manager = nullptr;

    explicit MyPromise(mycomanager *mgr) : manager(mgr) {}

    /**
     * @brief Mark promise as complete and wake waiters
     */
    void set_value();

    /**
     * @brief Set exception and wake waiters
     * @param e Exception pointer to store
     */
    void set_exception(std::exception_ptr e);

private:
    void wake_waiters() {
        for (mycotask *t : waiters)
            t->resume();
        waiters.clear();
    }
};

/**
 * @class MyFuture
 * @brief Consumer side of promise-future pair
 * @tparam T Type of value to retrieve
 *
 * Allows coroutines to wait for and retrieve values from promises.
 * Suspends execution until value becomes available.
 */
template <typename T>
class MyFuture {
public:
    MyPromise<T> *promise = nullptr;  /**< Associated promise */

    /**
     * @brief Wait for and retrieve result value
     * @return The stored value from promise
     * @throws Rethrows stored exception if set
     *
     * Yields execution repeatedly until promise is ready. If promise
     * contains an exception, it is rethrown to the caller.
     */
    T get() {
        mycotask *self = mycotask::current_task();
        while (!promise->ready) {
            promise->waiters.push_back(self);
            self->yield();
        }
        if (promise->exception)
            std::rethrow_exception(promise->exception);
        return promise->value;
    }
};

/**
 * @class MyFuture<void>
 * @brief Future specialization for void promises
 *
 * Waits for completion without retrieving a value.
 */
template <>
class MyFuture<void> {
public:
    MyPromise<void> *promise = nullptr;

    /**
     * @brief Wait for promise completion
     * @throws Rethrows stored exception if set
     */
    void get();
};

/**
 * @brief Launch asynchronous coroutine task
 * @tparam F Callable type
 * @param manager Coroutine manager to schedule task
 * @param func Function to execute asynchronously
 * @return MyFuture for retrieving result
 *
 * Creates a new coroutine task that executes the provided function
 * and stores the result in a promise. Returns a future that can be
 * used to wait for and retrieve the result.
 *
 * Handles both void and non-void return types through template
 * specialization. Exceptions are captured and propagated through
 * the promise-future mechanism.
 *
 * @code
 * mycomanager mgr;
 * auto future = myco_async(mgr, []() { return 42; });
 * mgr.run();
 * int result = future.get();
 * @endcode
 */
template <typename F>
auto myco_async(mycomanager &manager, F &&func) {
    using R = std::invoke_result_t<F>;
    if constexpr (std::is_void_v<R>) {
        auto *promise = new MyPromise<void>(&manager);
        mycotask task =
            mycotask::create_task([promise, f = std::forward<F>(func)]() mutable {
                try {
                    f();
                    promise->set_value();
                } catch (...) {
                    promise->set_exception(std::current_exception());
                }
            });
        manager.append_task(std::move(task));
        return MyFuture<void>{promise};
    } else {
        auto *promise = new MyPromise<R>(&manager);
        mycotask task =
            mycotask::create_task([promise, f = std::forward<F>(func)]() mutable {
                try {
                    promise->set_value(f());
                } catch (...) {
                    promise->set_exception(std::current_exception());
                }
            });
        manager.append_task(std::move(task));
        return MyFuture<R>{promise};
    }
}

/** @} */
