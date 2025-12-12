/**
* @file create_stack.cpp
 * @brief Stack allocation and initialization for coroutine contexts
 *
 * Provides functions for creating coroutine execution stacks with proper
 * memory layout for independent coroutine execution.
 */

#include <cstddef>
#include <functional>
#include "../coroutines.h"

#define STACK_SIZE 16384

#include <iostream>

/**
 * @brief Execute a function object with exception safety
 * @param p Pointer to std::function<void()> to execute
 *
 * Wrapper called from assembly code to execute coroutine function.
 * Ensures proper cleanup even if exceptions are thrown.
 */
extern "C" void call_fn(void *p) {
  auto pf = static_cast<std::function<void()>*>(p);
  try {
    (*pf)();
  } catch (...) {
    delete pf;
    throw;
  }
  delete pf;
}

/**
 * @brief Allocate and initialize a new coroutine context
 * @return Pointer to newly created coro_context structure
 *
 * Allocates 16KB stack memory and initializes context structure.
 * Stack grows downward from stack_top toward stack_base.
 */
coro_context *create_coro_context() {
  coro_context *cr_context = new coro_context;
  char *stack_base = new char[STACK_SIZE];
  cr_context->stack_base = stack_base;
  cr_context->stack_top = stack_base + STACK_SIZE;
  return cr_context;
}
