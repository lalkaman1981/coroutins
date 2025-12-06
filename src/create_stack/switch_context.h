#pragma once
#include <cstdint>


struct alignas(64) coro_context {
    uint64_t old_rsp;
    unsigned char pad[56];
    unsigned char xsave_area[4096];
    char* stack_base;
    char* stack_top;
};


extern "C" void switch_context(coro_context* old_context, coro_context* new_context);
