#include "coroutines.h"

std::vector<coro_context *> manager;


void func2(double a, double b, double* res) {

  std::cout << "Step 2" << std::endl;
  switch_context(manager[2], manager[1]);
  std::cout << "Step 4" << std::endl;

  double c = a * b;
  volatile double v1 = -1.0;
  volatile double v2 = -1.0;
  volatile double v3 = -1.0;
  volatile double v4 = -1.0;
  volatile double v5 = -1.0;
  volatile double v6 = -1.0;
  volatile double v7 = -1.0;
  volatile double v8 = -1.0;
  volatile double v9 = -1.0;
  volatile double v10 = -1.0;
  volatile double v11 = -1.0;
  volatile double v12 = -1.0;
  volatile double v13 = -1.0;
  volatile double v14 = -1.0;
  volatile double v15 = -1.0;
  volatile double v16 = -1.0;
  volatile double v17 = -1.0;
  volatile double v18 = -1.0;
  volatile double v19 = -1.0;
  volatile double v20 = -1.0;
  std::cout << v1 + v2 + v3 + v4 + v5 + v6 + v7 + v8 + v9 + v10
                   + v11 + v12 + v13 + v14 + v15 + v16 + v17 + v18 + v19 + v20 << std::endl;
  *res = c;

  switch_context(manager[2], manager[1]);

  volatile int prevent_optimization = 0;
  (void)prevent_optimization;
}

void func1(double a, double b, double* res) {

  std::cout << "Step 1" << std::endl;
  switch_context(manager[1], manager[0]);
  std::cout << "Step 3" << std::endl;
  switch_context(manager[1], manager[2]);
  std::cout << "Step 5" << std::endl;

  double c = a + b;
  volatile double v1 = 1.0;
  volatile double v2 = 1.0;
  volatile double v3 = 1.0;
  volatile double v4 = 1.0;
  volatile double v5 = 1.0;
  volatile double v6 = 1.0;
  volatile double v7 = 1.0;
  volatile double v8 = 1.0;
  volatile double v9 = 1.0;
  volatile double v10 = 1.0;
  volatile double v11 = 1.0;
  volatile double v12 = 1.0;
  volatile double v13 = 1.0;
  volatile double v14 = 1.0;
  volatile double v15 = 1.0;
  volatile double v16 = 1.0;
  volatile double v17 = 1.0;
  volatile double v18 = 1.0;
  volatile double v19 = 1.0;
  volatile double v20 = 1.0;
  std::cout << v1 + v2 + v3 + v4 + v5 + v6 + v7 + v8 + v9 + v10
                   + v11 + v12 + v13 + v14 + v15 + v16 + v17 + v18 + v19 + v20 << std::endl;
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

  call_coro(cr1, cr2, func1, 6.9, 3.14, res1);

  call_coro(cr1, cr3, func2, 4.20, 14.88, res2);

  std::cout << *res1 << std::endl;
  std::cout << *res2 << std::endl;

  std::cout << "Done!" << std::endl;
}
