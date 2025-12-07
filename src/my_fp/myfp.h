
#pragma once
#include <exception>
#include <functional>
#include <type_traits>
#include <vector>

#include "../mycomanager/mycomanager.h"
#include "../mycotask/mycotask.h"

// ======================= PROMISE =======================

template <typename T> class MyPromise {
public:
  T value{};
  bool ready = false;
  std::exception_ptr exception;
  std::vector<mycotask *> waiters;
  mycomanager *manager = nullptr;

  explicit MyPromise(mycomanager *mgr) : manager(mgr) {}

  void set_value(T v) {
    value = std::move(v);
    ready = true;
    wake_waiters();
  }

  void set_exception(std::exception_ptr e) {
    exception = e;
    ready = true;
    wake_waiters();
  }

private:
  void wake_waiters() {
    for (mycotask *t : waiters)
      t->resume();
    waiters.clear();
  }
};

// ======================= PROMISE for void =======================
template <> class MyPromise<void> {
public:
  bool ready = false;
  std::exception_ptr exception;
  std::vector<mycotask *> waiters;
  mycomanager *manager = nullptr;

  explicit MyPromise(mycomanager *mgr) : manager(mgr) {}

  void set_value() {
    ready = true;
    wake_waiters();
  }

  void set_exception(std::exception_ptr e) {
    exception = e;
    ready = true;
    wake_waiters();
  }

private:
  void wake_waiters() {
    for (mycotask *t : waiters)
      t->resume();
    waiters.clear();
  }
};

// ======================= FUTURE =======================

template <typename T> class MyFuture {
public:
  MyPromise<T> *promise = nullptr;

  T get() {
    mycotask *self = mycotask::current_task();
    while (!promise->ready) { // <-- цикл до готовності
      promise->waiters.push_back(self);
      self->yield(); // передаємо управління
    }
    if (promise->exception)
      std::rethrow_exception(promise->exception);
    return promise->value;
  }
};

// ======================= FUTURE for void =======================
template <> class MyFuture<void> {
public:
  MyPromise<void> *promise = nullptr;

  void get() {
    mycotask *self = mycotask::current_task();
    while (!promise->ready) {
      promise->waiters.push_back(self);
      self->yield();
    }
    if (promise->exception)
      std::rethrow_exception(promise->exception);
  }
};

// ======================= ASYNC =======================

template <typename F> auto myco_async(mycomanager &manager, F &&func) {
  using R = std::invoke_result_t<F>;

  if constexpr (std::is_void_v<R>) {
    auto *promise = new MyPromise<void>(&manager);

    mycotask task =
        mycotask::create_task([promise, f = std::forward<F>(func)]() mutable {
          try {
            f();
            promise->set_value();
          } catch (...) {
            promise->set_exception(std::current_exception());
          }
        });

    manager.append_task(std::move(task));
    return MyFuture<void>{promise};
  } else {
    auto *promise = new MyPromise<R>(&manager);

    mycotask task =
        mycotask::create_task([promise, f = std::forward<F>(func)]() mutable {
          try {
            promise->set_value(f());
          } catch (...) {
            promise->set_exception(std::current_exception());
          }
        });

    manager.append_task(std::move(task));
    return MyFuture<R>{promise};
  }
}
