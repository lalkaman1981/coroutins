#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>
#include "coroutines.h"
#include "libmycotask/mycotask.h"

mycotask test_input_coro_task(int a, int b, int& result) {
    std::cout << "[Task] Started\ta:" << a << "\tb:" << b << "\tresult:" << result << std::endl;
    mycotask::current_task()->yield();
    std::cout << "[Task] Resumed\tb:" << b << std::endl;
    mycotask::current_task()->yield();
    std::cout << "[Task] Complete" << std::endl;
    result = a + b;
}

int main() {

    // const auto task = std::make_shared<mycotask>([](){
    //     std::cout << "[Task] Started" << std::endl;
    //     mycotask::current_task()->yield();
    //     std::cout << "[Task] Resumed" << std::endl;
    //     mycotask::current_task()->yield();
    //     std::cout << "[Task] Complete" << std::endl;
    // });
    int result = -69420;
    const auto task = std::make_shared<mycotask>(test_input_coro_task(1, 2, result));

    std::cout << "[Main] Task created" << std::endl;
    task->start();

    std::cout << "[Main] After first yield" << std::endl;
    task->resume();

    std::cout << "[Main] After second yield" << std::endl;
    task->resume();

    std::cout << "[Main] Task finished" << std::endl;

    std::cout << "[Main] Result: " << result << std::endl;

    return 0;
}
