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
#include "libmycotask/mycotask.h"

int main() {

    const auto task = std::make_shared<mycotask>([](){
        std::cout << "[Task] Started" << std::endl;
        mycotask::current_task()->yield();
        std::cout << "[Task] Resumed" << std::endl;
        mycotask::current_task()->yield();
        std::cout << "[Task] Complete" << std::endl;
    });

    std::cout << "[Main] Task created" << std::endl;
    task->start();

    std::cout << "[Main] After first yield" << std::endl;
    task->resume();

    std::cout << "[Main] After second yield" << std::endl;
    task->resume();

    std::cout << "[Main] Task finished" << std::endl;
    return 0;
}
