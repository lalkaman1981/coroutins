#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>
#include "coroutines.h"
#include "libmycotask/mycotask.h"


void task_body(int a, int b, int& result) {
    std::cout << "[Task] Start  a=" << a << "  b=" << b << "\n";
    mycotask::current_task()->yield();

    std::cout << "[Task] Resume\n";
    mycotask::current_task()->yield();

    result = a + b;
}

int main() {
    int result = -69420;
    //
    // mycotask task = mycotask::create_task(task_body, 1, 2, std::ref(result));
    //
    // std::cout << "[Main] Task created\n";
    //
    // task.start();
    // std::cout << "[Main] After first yield\n";
    //
    // task.resume();
    // std::cout << "[Main] After second yield\n";
    //
    // std::cout << "RESULT: " << result << std::endl;
    mycotask t1 = mycotask::create_task(task_body, 1, 2, std::ref(result));
    mycotask t2 = mycotask::create_task(task_body, 3, 4, std::ref(result));
    mycotask t3 = std::move(t1);


    // mycotask task = mycotask::create_task(task_body, 1, 2, std::ref(result));

    std::cout << "[Main] Task created\n";

    t3.start();
    std::cout << "[Main] After first yield\n";

    t3.resume();
    std::cout << "[Main] After second yield\n";

    std::cout << "RESULT: " << result << std::endl;
}
