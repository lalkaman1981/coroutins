#include "../../src/mycotask.h"
#include "../test_utils.h"

#include <iostream>
#include <vector>
#include <memory>

// Include LibCopp headers
#include <libcopp/coroutine/coroutine_context_container.h>

const int ITERATIONS = 1'000'000;

// ==========================================
// 1. MycoTask Implementation
// ==========================================
void myco_worker(int count) {
    for (int i = 0; i < count; ++i) {
        mycotask::current_task()->yield();
    }
}

double bench_mycotask_switch() {
    auto task = mycotask::create_task(myco_worker, ITERATIONS);
    task.start();

    auto start = get_current_time_fenced();
    for(int i = 0; i < ITERATIONS; ++i) {
        task.resume();
    }
    auto end = get_current_time_fenced();

    long long total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    return (double)total_ns / (ITERATIONS * 2.0);
}

double bench_mycotask_creation() {
    const int N = 50000;
    auto start = get_current_time_fenced();
    for(int i=0; i<N; ++i) {
        auto t = mycotask::create_task([]{});
        t.start();
        // FIX: Do not call resume().
        // The empty task finishes inside start() because it never yields.
        // Calling resume() on an ended task causes mycotask to exit(1).
    }
    auto end = get_current_time_fenced();
    return (double)std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / N;
}

// ==========================================
// 2. LibCopp Implementation
// ==========================================

using my_co_type = copp::coroutine_context_container<copp::allocator::default_statck_allocator>;

int libcopp_worker(void*) {
    for (int i = 0; i < ITERATIONS; ++i) {
        copp::this_coroutine::yield();
    }
    return 0;
}

double bench_libcopp_switch() {
    auto co_ptr = my_co_type::create(libcopp_worker, 128 * 1024);

    if (!co_ptr) {
        std::cerr << "LibCopp creation failed!" << std::endl;
        exit(1);
    }

    co_ptr->start();

    auto start = get_current_time_fenced();
    for(int i = 0; i < ITERATIONS; ++i) {
        co_ptr->resume();
    }
    auto end = get_current_time_fenced();

    long long total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    return (double)total_ns / (ITERATIONS * 2.0);
}

double bench_libcopp_creation() {
    const int N = 50000;
    auto start = get_current_time_fenced();

    // Explicit function pointer to solve ambiguity
    int (*empty_task)(void*) = [](void*) { return 0; };

    for(int i=0; i<N; ++i) {
        auto co_ptr = my_co_type::create(empty_task, 128*1024);
        co_ptr->start();
    }

    auto end = get_current_time_fenced();
    return (double)std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / N;
}

// ==========================================
// Main
// ==========================================
int main() {
    std::cout << "Library,Metric,TimeNS\n";

    std::cout << "MycoTask,Switch," << bench_mycotask_switch() << "\n";
    std::cout << "LibCopp,Switch," << bench_libcopp_switch() << "\n";

    std::cout << "MycoTask,Creation," << bench_mycotask_creation() << "\n";
    std::cout << "LibCopp,Creation," << bench_libcopp_creation() << "\n";

    return 0;
}