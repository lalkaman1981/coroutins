#pragma once
#include <cstdint>

struct alignas(16) coro_context {
    uint64_t old_rsp;
    uint64_t pad;
    alignas(16) unsigned char fxsave_area[512]; 
    char* stack_base;
    char* stack_top;
};


extern "C" void switch_context(coro_context* old_context, coro_context* new_context);
