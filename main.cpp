#include "coroutines.h"

std::vector<coro_context *> manager;

void func() { std::cout << "Before switch from 2" << std::endl; }

void func2() {
  std::cout << "Before switch from 2" << std::endl;

  // auto* old = manager[2];
  // auto* nw = manager[1];

  // switch_context(old, nw);
  std::cout << "After switch from 2" << std::endl;
  switch_context(manager[2], manager[0]);
}

void func1() {
  std::cout << "Before switch from 1" << std::endl;
  switch_context(manager[1], manager[0]);
  std::cout << "After switch from 1" << std::endl;
  switch_context(manager[1], manager[2]);
}

int main() {
  coro_context *cr1 = create_coro_context();
  coro_context *cr2 = create_coro_context();
  coro_context *cr3 = create_coro_context();

  manager.push_back(cr1);
  manager.push_back(cr2);
  manager.push_back(cr3);

  call_coro(cr1, cr2, func1);

  call_coro(cr1, cr3, func2);

  std::cout << "Done!" << std::endl;
}
