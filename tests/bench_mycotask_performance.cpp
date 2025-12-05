//
// Created by julfy on 12/5/25.
//

// tests/bench_mycotask_performance.cpp
#include "../src/mycotask.h"
#include "test_utils.h"
#include <iostream>
#include <vector>
#include <iomanip>

const int WARMUP_ITERATIONS = 1000;
const int BENCH_ITERATIONS = 2000000; // 2 Million switches

void ping_pong_body(int iterations) {
    for (int i = 0; i < iterations; ++i) {
        mycotask::current_task()->yield();
    }
}

void benchmark_switch_cost() {
    // 1. Warmup
    {
        auto t_warmup = mycotask::create_task(ping_pong_body, WARMUP_ITERATIONS);
        t_warmup.start();
        while (!t_warmup.has_ended()) {
            t_warmup.resume();
        }
    }

    // 2. Measurement
    auto task = mycotask::create_task(ping_pong_body, BENCH_ITERATIONS);

    // Start the task (enters the loop)
    task.start();

    auto start_time = get_current_time_fenced();

    // We resume BENCH_ITERATIONS times
    // Total switches = Iterations * 2 (Main->Task + Task->Main)
    for(int i = 0; i < BENCH_ITERATIONS; ++i) {
        task.resume();
    }

    auto end_time = get_current_time_fenced();

    long long total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();

    // *2 because one iteration implies: Resume (Context Switch 1) -> Yield (Context Switch 2)
    double ns_per_switch = (double)total_ns / (BENCH_ITERATIONS * 2.0);

    std::cout << "ContextSwitch," << BENCH_ITERATIONS * 2 << "," << total_ns << "," << ns_per_switch << std::endl;
}

void benchmark_creation_cost() {
    const int N = 100000;

    auto start_time = get_current_time_fenced();

    for(int i=0; i<N; ++i) {
        auto t = mycotask::create_task([](){});
        // Destructor called here immediately
    }

    auto end_time = get_current_time_fenced();
    long long total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    double ns_per_creation = (double)total_ns / N;

    std::cout << "TaskCreation," << N << "," << total_ns << "," << ns_per_creation << std::endl;
}

int main() {
    // CSV Header
    std::cout << "Metric,TotalSwitches,TotalTimeNS,AvgTimeNS_PerOp" << std::endl;

    benchmark_switch_cost();
    benchmark_creation_cost();

    return 0;
}
