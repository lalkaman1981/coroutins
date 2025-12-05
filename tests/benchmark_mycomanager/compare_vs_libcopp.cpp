#include "../../src/mycomanager.h"
#include "../../src/mycotask.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <atomic>
#include <list>

// Include LibCoTask (Higher level abstraction of LibCopp)
#include <libcotask/task.h>


const int SCHEDULING_TASKS = 100;
const int SCHEDULING_YIELDS = 10'000;
const int LIFECYCLE_TASKS = 50'000;

inline auto get_current_time_fenced() {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res;
}


void myco_sched_worker(int yields) {
    for (int i = 0; i < yields; ++i) {
        mycotask::current_task()->yield();
    }
}

void myco_empty_worker() {}

double bench_myco_scheduling() {
    mycomanager manager(static_cast<size_t>(SCHEDULING_TASKS + 10));

    for(int i = 0; i < SCHEDULING_TASKS; ++i) {
        manager.append_task(mycotask::create_task(myco_sched_worker, SCHEDULING_YIELDS));
    }

    auto start = get_current_time_fenced();
    manager.run();
    auto end = get_current_time_fenced();

    long long total_ops = (long long)SCHEDULING_TASKS * SCHEDULING_YIELDS;
    return (double)std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / (total_ops * 2.0);
}

double bench_myco_lifecycle() {
    auto start = get_current_time_fenced();
    {
        mycomanager manager(static_cast<size_t>(LIFECYCLE_TASKS + 10));
        for(int i = 0; i < LIFECYCLE_TASKS; ++i) {
            manager.append_task(mycotask::create_task(myco_empty_worker));
        }
        manager.run();
    }
    auto end = get_current_time_fenced();
    return (double)std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / LIFECYCLE_TASKS;
}


using my_task_t = cotask::task<>;

int libcopp_sched_worker(void*) {
    for (int i = 0; i < SCHEDULING_YIELDS; ++i) {
        cotask::this_task::get<my_task_t>()->yield();
    }
    return 0;
}

int libcopp_empty_worker(void*) { return 0; }

double bench_libcopp_scheduling() {
    std::vector<my_task_t::ptr_t> tasks;
    tasks.reserve(SCHEDULING_TASKS);

    for(int i = 0; i < SCHEDULING_TASKS; ++i) {
        tasks.push_back(my_task_t::create(libcopp_sched_worker));
        tasks.back()->start();
    }

    auto start = get_current_time_fenced();

    bool active = true;
    while(active) {
        active = false;
        for(auto& task : tasks) {
            if(!task->is_completed()) {
                task->resume();
                active = true;
            }
        }
    }

    auto end = get_current_time_fenced();

    long long total_ops = (long long)SCHEDULING_TASKS * SCHEDULING_YIELDS;
    return (double)std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / (total_ops * 2.0);
}

double bench_libcopp_lifecycle() {
    auto start = get_current_time_fenced();

    std::vector<my_task_t::ptr_t> tasks;
    tasks.reserve(LIFECYCLE_TASKS);

    for(int i = 0; i < LIFECYCLE_TASKS; ++i) {
        auto t = my_task_t::create(libcopp_empty_worker);
        t->start();
    }

    auto end = get_current_time_fenced();
    return (double)std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / LIFECYCLE_TASKS;
}

// ==========================================
// Main
// ==========================================
int main() {
    std::cout << "Library,Metric,TimeNS\n";

    std::cout << "MycoManager,Switch," << bench_myco_scheduling() << "\n";
    std::cout << "LibCopp(Task),Switch," << bench_libcopp_scheduling() << "\n";

    std::cout << "MycoManager,Lifecycle," << bench_myco_lifecycle() << "\n";
    std::cout << "LibCopp(Task),Lifecycle," << bench_libcopp_lifecycle() << "\n";

    return 0;
}