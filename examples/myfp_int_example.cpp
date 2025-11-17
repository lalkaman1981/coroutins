#include "src/libmyfp/myfp.h"
#include <iostream>

int main() {
    mycomanager manager;

    // Producer
    auto fut = myco_async(manager, []() -> int {
        std::cout << "[producer] Start\n";
        for (int i = 0; i < 3; i++) {
            std::cout << "[producer] Working " << i << "\n";
            mycotask::current_task()->yield();
        }
        std::cout << "[producer] Done\n";
        return 42;
    });

    // Consumer
    myco_async(manager, [fut]() mutable {
        std::cout << "[consumer] Start\n";
        int r = fut.get();
        std::cout << "[consumer] Got result: " << r << "\n";
        std::cout << "[consumer] Finish\n";
    });

    // Scheduler
    manager.run();
}
