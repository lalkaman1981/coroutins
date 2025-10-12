//
// #include <iostream>
//
// #include "coroutines.h"
// #include "create_stack.h"
// coro_context main_ctx;
// coro_context coro_ctx;
//
// void coro_func(void* arg) {
//     std::cout << "Started coroutine\n";
//
//     // Yield back to main
//     switch_context(&coro_ctx, &main_ctx);
//
//     std::cout << "Resumed coroutine\n";
// }
//
// int main() {
//     // Create a new coroutine context and stack
//     call_coro(coro_func, nullptr);
//
//     // Switch into coroutine
//     switch_context(&main_ctx, &coro_ctx);
//
//     std::cout << "Back in main\n";
//
//     // Resume coroutine
//     switch_context(&main_ctx, &coro_ctx);
//
//     std::cout << "Coroutine finished\n";
// }


//
// Created by gllekk on 10/12/25.
//
#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>
#include "coroutines.h"
#include "mycotask.h"


// struct coro_context {
//     uint64_t old_rsp;
//     uint64_t pad;
//     alignas(16) unsigned char fxsave_area[512];
//     void* stack_base;
//     void* stack_top;
// };


// coro_context main_context;
// coro_context coroutine_context;
//
// void test_coroutine_function() {
//     std::cout << "[Coroutine] ==> Execution started." << std::endl;
//     std::cout << "[Coroutine] ==> Yielding control back to main..." << std::endl;
//     switch_context(&coroutine_context, &main_context);
//
//     std::cout << "[Coroutine] ==> Resumed. Execution is finishing." << std::endl;
//     switch_context(&coroutine_context, &main_context);
// }
//
// void setup_coroutine(coro_context* ctx, void (*func)(), void* stack_base, size_t stack_size) {
//     memset(ctx, 0, sizeof(coro_context));
//     uintptr_t sp = reinterpret_cast<uintptr_t>(stack_base) + stack_size;
//     sp &= -16L;
//
//     sp -= 8; *reinterpret_cast<uint64_t*>(sp) = reinterpret_cast<uint64_t>(func);
//     sp -= 8; *reinterpret_cast<uint64_t*>(sp) = 0x202;
//     sp -= 8 * 6;
//
//     ctx->old_rsp = sp;
//     ctx->stack_base = stack_base;
// }
//
// int main() {
//     std::cout << "Hello bohdan" << std::endl;
//     std::vector<uint8_t> coroutine_stack(16384);
//
//     std::cout << "[Main] Setting up coroutine..." << std::endl;
//     setup_coroutine(&coroutine_context, &test_coroutine_function, coroutine_stack.data(), coroutine_stack.size());
//
//     std::cout << "[Main] Switching to coroutine..." << std::endl;
//     switch_context(&main_context, &coroutine_context);
//
//     std::cout << "[Main] Control has returned from coroutine." << std::endl;
//     std::cout << "[Main] Resuming coroutine..." << std::endl;
//     switch_context(&main_context, &coroutine_context);
//
//     std::cout << "[Main] Coroutine has finished. Test successful." << std::endl;
//     return 0;
// }

// coro_context mycotask::main_ctx_{};
// thread_local mycotask* mycotask::current_task_ = nullptr;

int main() {
    const auto task = std::make_shared<mycotask>([](){
        std::cout << "[Task] Started" << std::endl;
        mycotask::current_task()->yield(); // yield back to main
        std::cout << "[Task] Resumed" << std::endl;
        mycotask::current_task()->yield();
    });

    std::cout << "[Main] Task created" << std::endl;
    task->start();

    std::cout << "[Main] After first yield" << std::endl;
    task->resume();

    std::cout << "[Main] Task finished" << std::endl;
    return 0;
}
