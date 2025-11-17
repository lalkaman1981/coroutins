#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>
#include "coroutines.h"
#include "libmycotask/mycotask.h"


void task_body(int a, int b, int& result) {
    std::cout << "[Task] Start  a=" << a << "  b=" << b << "\n";
    mycotask::current_task()->yield();

    std::cout << "[Task] Resume1\n";
    mycotask::current_task()->yield();

    std::cout << "[Task] Resume2\n";
    mycotask::current_task()->yield();

    std::cout << "[Task] Resume2\n";
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
    auto t2 = mycotask::create_task([](){
        std::cout << "[Task1] Started" << std::endl;
        mycotask::current_task()->yield(); // yield back to main
        std::cout << "[Task1] Resumed1" << std::endl;
        mycotask::current_task()->yield(); // yield back to main
        std::cout << "[Task1] Resumed2" << std::endl;
        mycotask::current_task()->yield(); // yield back to main
        std::cout << "[Task1] Resumed3" << std::endl;
    });


    // mycotask task = mycotask::create_task(task_body, 1, 2, std::ref(result));
    //
    // std::cout << "[Main] Task created\n";
    //
    // t3.start();
    // std::cout << "[Main] After first yield\n";
    //
    // t3.resume();
    // std::cout << "[Main] After second yield\n";
    //
    // std::cout << "RESULT: " << result << std::endl;
    t2.start();
    t1.start();

    t2.resume();
    t1.resume();

    t2.resume();
    t1.resume();

    t2.resume();
    t1.resume();

    t1.resume();


}

