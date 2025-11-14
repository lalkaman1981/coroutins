#include "coroutines.h"

std::vector<coro_context *> manager;

int a = 0;

void func2(int a, int b, int* res) {

  std::cout << "Step 2" << std::endl;
  switch_context(manager[2], manager[1]);
  std::cout << "Step 4" << std::endl;

  int c = a * b;
  *res = c;

  switch_context(manager[2], manager[1]);

  volatile int prevent_optimization = 0;
  (void)prevent_optimization;
}

void func1(int a, int b, int* res) {

  std::cout << "Step 1" << std::endl;
  switch_context(manager[1], manager[0]);
  std::cout << "Step 3" << std::endl;
  switch_context(manager[1], manager[2]);
  std::cout << "Step 5" << std::endl;

  int c = a + b;
  *res = c;

  switch_context(manager[1], manager[0]);

  volatile int prevent_optimization = 0;
  (void)prevent_optimization;
}

int main() {
  coro_context *cr1 = create_coro_context();
  coro_context *cr2 = create_coro_context();
  coro_context *cr3 = create_coro_context();

  manager.push_back(cr1);
  manager.push_back(cr2);
  manager.push_back(cr3);

  int *res1 = new int;
  *res1 = 0;
  int *res2 = new int;
  *res2 = 0;

  call_coro(cr1, cr2, func1, 1, 2, res1);

  call_coro(cr1, cr3, func2, 3, 4, res2);

  std::cout << *res1 << std::endl;
  std::cout << *res2 << std::endl;

  std::cout << "Done!" << std::endl;
}
