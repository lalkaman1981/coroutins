#pragma once
#include <cstdint>

typedef struct coro_regs {
    // put all user regs here
} coro_regs;

struct coro_context {
    uint64_t old_rsp;
    uint64_t pad;
    alignas(16) unsigned char fxsave_area[512];
    void* stack_base;
    void* stack_top;
};

extern "C" void switch_context(coro_context* old_context, coro_context* new_context);