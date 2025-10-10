#include <functional>
#include <utility>

#include "create_stack.h"

extern "C" void call_fn(void* p) {
    auto* pf = static_cast<std::function<void()>*>(p);
    try {
        (*pf)();
    } catch(...) {
        delete pf;
        throw;
    }
    delete pf;
}

