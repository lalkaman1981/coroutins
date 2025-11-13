#include <functional>
#include <utility>

#include "coroutines.h"

#define STACK_SIZE 16384

#include <functional>
#include <cstring>

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


