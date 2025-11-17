# Course project: Coroutines
Authors (team):
Pavlosiuk Roman,
Milian Bohdan,
Shtohryn Oleg,
Stetsiuk Kostantin

## Overview

This project implements a **custom coroutine library in C++**, consisting of two major components:

- **`mycotask`** — a lightweight wrapper around a manually–implemented coroutine context.  
  It supports:
    - creating tasks from functions or lambdas,
    - yielding/resuming execution,
    - passing arguments by value and reference.

- **`mycomanager`** — a scheduler that stores multiple `mycotask` objects and executes them cooperatively in a round-robin fashion.

The implementation is based on **manual stack/context switching** using Boost.Context, linked as part of `libcoro`.


## Prerequisites

To build and run the project, you need:

- **C++20 compiler**
    - GCC ≥ 11
    - Clang ≥ 14
- **Boost** (boost-context, boost-intrusive)
- **CMake ≥ 3.15**
- **Linux** (x86-64) recommended  
  (due to assembly-based context switching)

## Compilation

Use the main CMake target:

```sh
mkdir build
cd build
cmake ..
make libcoro
```


### Installation

boost, cmake

## Usage

The library exposes two key components:

### `mycotask`

Represents a single resumable coroutine.

**Basic API:**

```c++
mycotask t = mycotask::create_task(function_or_lambda, args...);

t.start();   // first run
t.resume();  // resume after yielding
```

Each task may call:

```c++
mycotask::current_task()->yield();
```
to yield control back to the caller.

### Example: Using `mycotask`

```c++
void task_body(int a, int b, int& result) {
    std::cout << "[Task] Start a=" << a << " b=" << b << "\n";
    mycotask::current_task()->yield();

    std::cout << "[Task] Resume1\n";
    mycotask::current_task()->yield();

    std::cout << "[Task] Resume2\n";
    mycotask::current_task()->yield();

    result = a + b;
}

int main() {
    int result = -69420;

    mycotask t1 = mycotask::create_task(task_body, 1, 2, std::ref(result));

    t1.start();
    t1.resume();
    t1.resume();
    t1.resume();

    std::cout << "RESULT: " << result << std::endl;
}
```

### `mycomanager`

A round-robin scheduler that stores multiple tasks:

```c++
mycomanager manager{std::move(task1), std::move(task2)};

manager.append_task(std::move(task3));
manager.run();   // runs until all tasks finish
```

### Example: Using `mycomanager`

```c++
auto task1 = mycotask::create_task([](){
    std::cout << "[Task1] Started\n";
    mycotask::current_task()->yield();
    std::cout << "[Task1] Resumed1\n";
    mycotask::current_task()->yield();
    std::cout << "[Task1] Resumed2\n";
});

auto task2 = mycotask::create_task([](){
    std::cout << "[Task2] Started\n";
    mycotask::current_task()->yield();
    std::cout << "[Task2] Resumed\n";
});

auto task3 = mycotask::create_task([](){
    std::cout << "[Task3] Started and Finished!\n";
});

mycomanager manager{std::move(task1), std::move(task2)};
manager.append_task(std::move(task3));
manager.run();
```
