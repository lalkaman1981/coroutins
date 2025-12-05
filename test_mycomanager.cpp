#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>
#include "coroutines.h"
#include "mycomanager.h"
#include "libmycotask/mycotask.h"

void task_body(int a, int b, int& result) {
    std::cout << "[Task4] Start  a=" <<
        a << "  b=" << b << "\n";
    mycotask::current_task()->yield();

    std::cout << "[Task4] Resume1\n";
    mycotask::current_task()->yield();

    result = a + b;
}

int main() {
    auto task1 = mycotask::create_task([](){
        std::cout << "[Task1] Started" << std::endl;
        mycotask::current_task()->yield();
        std::cout << "[Task1] Resumed1" << std::endl;
        mycotask::current_task()->yield();

    });

    auto task2 = mycotask::create_task([](){
        std::cout << "[Task2] Started" << std::endl;
        mycotask::current_task()->yield();
        std::cout << "[Task2] Resumed" << std::endl;

    });

    auto task3 = mycotask::create_task([](){
        std::cout << "[Task3] Started and Finished!" << std::endl;
    });
    int result = -69420;
    auto arg_task = mycotask::create_task(task_body,
                                        1, 2, std::ref(result));

    mycomanager manager{std::move(task1), std::move(task2)};

    manager.append_task(std::move(task3));

    manager.append_task(std::move(arg_task));

    manager.run();

    std::cout << "RESULT: " << result << std::endl;

    return 0;

}