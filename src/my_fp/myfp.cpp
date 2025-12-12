/**
* @file myfp.cpp
 * @brief Implementation of promise-future void specializations
 *
 * Provides implementations for MyPromise<void> and MyFuture<void>
 * member functions that handle completion without return values.
 */

#include "myfp.h"

void MyPromise<void>::set_value() {
  ready = true;
  for (mycotask *t : waiters)
    t->resume();
  waiters.clear();
}

void MyPromise<void>::set_exception(std::exception_ptr e) {
  exception = e;
  ready = true;
  for (mycotask *t : waiters)
    t->resume();
  waiters.clear();
}

void MyFuture<void>::get() {
  mycotask *self = mycotask::current_task();
  while (!promise->ready) {
    promise->waiters.push_back(self);
    self->yield();
  }
  if (promise->exception)
    std::rethrow_exception(promise->exception);
}
