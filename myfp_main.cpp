
#include "src/libmyfp/myfp.h"
#include <iostream>

int main() {
  mycomanager manager;

  // ==== Producer ====
  auto fut = myco_async(manager, []() -> int {
    std::cout << "[producer] Start\n";
    for (int i = 0; i < 3; ++i) {
      std::cout << "[producer] Working " << i << "\n";
      mycotask::current_task()->yield(); // передаємо управління
    }
    std::cout << "[producer] Done\n";
    return 777;
  });

  myco_async(manager, [fut]() mutable {
    std::cout << "[consumer] Start\n";
    try {
      int result = fut.get(); // кооперативне очікування
      std::cout << "[consumer] Got result: " << result << "\n";
    } catch (const std::exception &e) {
      std::cout << "[consumer] Exception: " << e.what() << "\n";
    }
    std::cout << "[consumer] Finish\n";
  });

  manager.run();
  return 0;
}
