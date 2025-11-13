#include <functional>
#include <utility>

#include "coroutines.h"

#define STACK_SIZE 16384

#include <functional>

extern "C" void call_fn(void *p) {
  auto pf = static_cast<std::function<void()> *>(p);

  try {
    (*pf)();
  } catch (...) {
    delete pf;
    throw;
  }

  delete pf;
}

coro_context *create_coro_context() {
  coro_context *cr_context = new coro_context;

  char *stack_base = new char[STACK_SIZE];

  cr_context->stack_base = stack_base;
  cr_context->stack_top = stack_base + STACK_SIZE;

  return cr_context;
}
