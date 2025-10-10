#include "coroutines.h"

extern inline save_registers();
extern inline create_coro_stack();
extern inline create_stack();

void call_coro() {
    save_registers();
    create_stack();
}

void yield() {
    save_registers();
    create_coro_stack();
}