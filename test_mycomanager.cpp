#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>
#include "coroutines.h"
#include "mycomanager.h"
#include "libmycotask/mycotask.h"

int main() {
    auto task1 = mycotask([](){
        std::cout << "[Task1] Started" << std::endl;
        mycotask::current_task()->yield(); // yield back to main
        std::cout << "[Task1] Resumed" << std::endl;
    });

    auto task2 = mycotask([](){
        std::cout << "[Task2] Started" << std::endl;
        mycotask::current_task()->yield(); // yield back to main
        std::cout << "[Task2] Resumed" << std::endl;
    });

    auto task3 = mycotask([](){
        std::cout << "[Task3] Started" << std::endl;
        mycotask::current_task()->yield(); // yield back to main
        std::cout << "[Task3] Resumed" << std::endl;
    });

    mycomanager manager{};
    manager.add_task(task1);



    return 0;
}
