#include "coroutines.h"

std::vector<coro_context *> manager;


void func2(double a, double b, int v, double* res) {

  std::cout << "Step 2" << std::endl;
  switch_context(manager[2], manager[1]);
  std::cout << "Step 4" << std::endl;

  double c = a * b * (double) v;
  *res = c;

  switch_context(manager[2], manager[1]);

  volatile int prevent_optimization = 0;
  (void)prevent_optimization;
}

void func1(double a, double b, int v, double* res) {

  std::cout << "Step 1" << std::endl;
  switch_context(manager[1], manager[0]);
  std::cout << "Step 3" << std::endl;
  switch_context(manager[1], manager[2]);
  std::cout << "Step 5" << std::endl;

  double c = a + b + (double) v;
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

  double *res1 = new double;
  *res1 = 0;
  double *res2 = new double;
  *res2 = 0;

  call_coro(cr1, cr2, func1, 6.7, 3.14, 6, res1);

  call_coro(cr1, cr3, func2, 4.2, 14.9, 7, res2);

  std::cout << *res1 << std::endl;
  std::cout << *res2 << std::endl;

  std::cout << "Done!" << std::endl;
}
