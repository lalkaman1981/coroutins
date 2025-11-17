#include "myfp.h"
#include <iostream>

int main() {
  mycomanager manager;

  auto fut = myco_async(manager, []() -> std::string {
    mycotask::current_task()->yield();
    return "Hello async world!";
  });

  myco_async(manager, [fut]() mutable {
    std::cout << "[consumer] Waiting...\n";
    std::string msg = fut.get();
    std::cout << "[consumer] Message: " << msg << "\n";
  });

  manager.run();
  return 0;
}
