//
// Created by julfy on 12/5/25.
//

// tests/test_utils.h
#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <chrono>
#include <atomic>
#include <iostream>
#include <cassert>

inline std::chrono::high_resolution_clock::time_point get_current_time_fenced() {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "[-] Assertion failed: " << #condition << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            std::exit(EXIT_FAILURE); \
        } \
    } while(0)

#define ASSERT_EQ(val1, val2) \
    do { \
        if ((val1) != (val2)) { \
            std::cerr << "[-] Assertion failed: " << val1 << " != " << val2 << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            std::exit(EXIT_FAILURE); \
        } \
    } while(0)

#endif //TEST_UTILS_H
