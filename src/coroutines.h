/**
* @mainpage Coroutines Library
 *
 * @section modules Main Modules
 *
 * - @ref core "Core API" - Main coroutine functions
 * - @ref task "Task Management" - Individual coroutines
 * - @ref scheduler "Scheduler" - Round-robin manager
 * - @ref async "Async Operations" - Promise/Future
 * - @ref context "Context" - Low-level switching
 */

/**
 * @file coroutines.h
 * @brief Main coroutine API header
 *
 * Provides high-level functions and templates for creating and managing
 * coroutine contexts, stack initialization, and function execution on
 * coroutine stacks.
 */

/** @addtogroup core
 *  @{
 */
#ifndef CREATE_STACK
#define CREATE_STACK

#include <cstddef>
#include <functional>
#include <utility>
#include "create_stack/switch_context.h"

/**
 * @brief Initialize and execute function on new coroutine stack
 * @param fn Function pointer to execute
 * @param arg Argument to pass to function
 * @param new_context Coroutine context being initialized
 * @param old_context Caller's context to save
 *
 * Assembly routine that switches to new coroutine's stack, calls the
 * provided function, then returns to the old context. Implemented in
 * create_stack.S for x86-64 platforms.
 */
extern "C" void create_coro_stack(void (*fn)(void *), void *arg,
                                   coro_context *new_context,
                                   coro_context *old_context);

/**
 * @brief Initialize world/main context (legacy)
 * @param world_context Context structure to initialize
 */
extern "C" void create_world_context_s(coro_context *world_context);

/**
 * @brief Execute std::function wrapper
 * @param p Pointer to std::function<void()>
 *
 * Called from assembly to execute C++ function objects with proper
 * exception handling and cleanup.
 */
extern "C" void call_fn(void *p);

/**
 * @brief Create world/main context (legacy)
 * @return Pointer to initialized world context
 */
coro_context *create_world_context();

/**
 * @brief Execute callable on new coroutine with arguments
 * @tparam F Callable type (function, lambda, functor)
 * @tparam Args Argument types to bind
 * @param old_context Caller's context to save
 * @param new_context Coroutine context to execute on
 * @param f Callable to execute
 * @param args Arguments to bind to callable
 *
 * Convenience template that binds arguments to a callable and executes
 * it on the specified coroutine context. Arguments are bound immediately
 * using std::bind and stored on the heap until execution completes.
 */
template<typename F, typename... Args>
void call_coro(coro_context *old_context, coro_context *new_context, F &&f,
               Args &&...args) {
    auto bound = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    auto *pf = new std::function<void()>(std::move(bound));
    create_coro_stack(&call_fn, pf, new_context, old_context);
}

/**
 * @brief Allocate new coroutine execution context
 * @return Pointer to initialized coro_context with dedicated stack
 *
 * Creates a new coroutine context with allocated stack memory.
 * The returned context is ready for use with switch_context or
 * create_coro_stack. Caller is responsible for cleanup via destructor
 * or manual deallocation.
 */
coro_context *create_coro_context();

#endif

/** @} */