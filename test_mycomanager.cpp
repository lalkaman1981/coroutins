#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>
#include "coroutines.h"
#include "mycomanager.h"
#include "libmycotask/mycotask.h"

int main() {
    auto task1 = mycotask::create_task([](){
        std::cout << "[Task1] Started" << std::endl;
        mycotask::current_task()->yield(); // yield back to main
        std::cout << "[Task1] Resumed" << std::endl;
    });

    auto task2 = mycotask::create_task([](){
        std::cout << "[Task2] Started" << std::endl;
        mycotask::current_task()->yield(); // yield back to main
        std::cout << "[Task2] Resumed" << std::endl;
    });

    auto task3 = mycotask::create_task([](){
        std::cout << "[Task3] Started" << std::endl;
        mycotask::current_task()->yield(); // yield back to main
        std::cout << "[Task3] Resumed" << std::endl;
    });

    mycomanager manager{std::move(task1), std::move(task2)};

    return 0;
}
