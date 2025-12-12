/**
* @file switch_context.h
 * @brief Core data structures for coroutine context switching
 *
 * Defines execution context structure for saving and restoring
 * coroutine CPU state during context switches.
 */

#pragma once
#include <cstdint>

/**
 * @struct coro_context
 * @brief Coroutine execution context
 *
 * Stores all CPU state necessary for suspending and resuming coroutine
 * execution. Aligned to 64 bytes for cache optimization.
 */
struct alignas(64) coro_context {
    uint64_t old_rsp;              /**< Saved stack pointer */
    unsigned char pad[56];         /**< Alignment padding */
    unsigned char xsave_area[4096]; /**< Extended CPU state (AVX, SSE, FPU) */
    char* stack_base;              /**< Bottom of stack memory */
    char* stack_top;               /**< Top of stack memory */
};

/**
 * @brief Switch execution between two coroutine contexts
 * @param old_context Context to save current CPU state into
 * @param new_context Context to restore and resume execution from
 *
 * Performs complete context switch by saving all registers and extended
 * CPU state to old_context, then restoring from new_context.
 * Implemented in assembly for both Windows and Linux calling conventions.
 */
extern "C" void switch_context(coro_context* old_context, coro_context* new_context);
