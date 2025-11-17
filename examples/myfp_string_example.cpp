#include "myfp.h"
#include <iostream>
#include <string>

int main() {

  // initialize manager
  mycomanager manager;

  // Create producer future
  auto fut = myco_async(manager, []() -> std::string {
    std::cout << "[producer] Start\n";
    std::string result;
    for (int i = 0; i < 3; ++i) {
      std::cout << "[producer] Working " << i << "\n";
      result += std::to_string(i);
      mycotask::current_task()->yield();
    }
    std::cout << "[producer] Done\n";
    result += "test";
    return result;
  });

  // create async consumer function
  myco_async(manager, [fut]() mutable {
    std::cout << "[consumer] Start\n";
    try {
      std::string s = fut.get();
      std::cout << "[consumer] Got result: " << s << "\n";
    } catch (const std::exception &e) {
      std::cout << "[consumer] Exception: " << e.what() << "\n";
    }
    std::cout << "[consumer] Finish\n";
  });

  // run manager untill every coroutine end her work
  manager.run();

  return 0;
}
