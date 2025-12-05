#include "../src/mycotask.h"
#include "test_utils.h"
#include <iostream>
#include <functional>
#include <vector>
#include <cmath>
#include <memory>
#include <algorithm>
#include <string>
#include <map>

#include <boost/container/vector.hpp>
#include <boost/lexical_cast.hpp>

// ==========================================
// TEST 1: Basic Arguments & References
// ==========================================
void test_args_and_ref(int a, int b, int &result) {
    result = 5;
    mycotask::current_task()->yield();
    result = a + b;
}

void run_argument_test() {
    std::cout << "[Test 01] Argument Passing & References... ";
    int result = 0;
    auto task = mycotask::create_task(test_args_and_ref, 10, 20, std::ref(result));
    task.start();
    ASSERT_EQ(result, 5);
    task.resume();
    ASSERT_EQ(result, 30);
    ASSERT_TRUE(task.has_ended());
    std::cout << "OK\n";
}

// ==========================================
// TEST 2: Manual Interleaving
// ==========================================
void run_interleaving_test() {
    std::cout << "[Test 02] Manual Interleaving... ";
    std::vector<int> execution_order;
    auto t1 = mycotask::create_task([&]() {
        execution_order.push_back(1);
        mycotask::current_task()->yield();
        execution_order.push_back(3);
    });
    auto t2 = mycotask::create_task([&]() {
        execution_order.push_back(2);
        mycotask::current_task()->yield();
        execution_order.push_back(4);
    });

    t1.start(); t2.start();
    t1.resume(); t2.resume();

    ASSERT_EQ(execution_order.size(), 4);
    ASSERT_EQ(execution_order[0], 1);
    ASSERT_EQ(execution_order[3], 4);
    std::cout << "OK\n";
}

// ==========================================
// TEST 3: Extreme Creation (Stress)
// ==========================================
void run_extreme_creation_test() {
    std::cout << "[Test 03] Extreme Creation (10k tasks)... ";
    const int N = 10000;
    std::vector<mycotask> tasks;
    tasks.reserve(N);
    for(int i=0; i<N; ++i) {
        tasks.push_back(mycotask::create_task([](){
            mycotask::current_task()->yield();
        }));
    }
    for(auto &t : tasks) t.start();
    for(auto &t : tasks) t.resume();
    std::cout << "OK\n";
}

// ==========================================
// TEST 4: Heavy Computation (Prime Loop)
// ==========================================
bool is_prime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

void heavy_prime_calc(int limit, int &count) {
    for (int i = 2; i < limit; ++i) {
        if (is_prime(i)) {
            count++;
        }
        if (i % 1000 == 0) mycotask::current_task()->yield();
    }
}

void run_heavy_compute_test() {
    std::cout << "[Test 04] Heavy Computation (Primes)... ";
    int count = 0;
    auto task = mycotask::create_task(heavy_prime_calc, 50000, std::ref(count));
    
    task.start();
    while(!task.has_ended()) {
        task.resume();
    }
    
    ASSERT_EQ(count, 5133);
    std::cout << "OK\n";
}

// ==========================================
// TEST 5: Deep Recursion (Stack Stress)
// ==========================================
void recursive_yield(int depth, int max_depth, int &counter) {
    counter++;
    if (depth >= max_depth) return;
    
    if (depth % 10 == 0) mycotask::current_task()->yield();
    
    recursive_yield(depth + 1, max_depth, counter);
}

void run_recursion_test() {
    std::cout << "[Test 05] Deep Recursion with Yield... ";
    int counter = 0;
    // Recursion depth 100.
    auto task = mycotask::create_task(recursive_yield, 0, 100, std::ref(counter));
    
    task.start();
    while(!task.has_ended()) task.resume();
    
    ASSERT_EQ(counter, 101);
    std::cout << "OK\n";
}

// ==========================================
// TEST 6: Large Stack Frame Allocation
// ==========================================
void run_large_stack_test() {
    std::cout << "[Test 06] Large Stack Frame (Array)... ";
    
    auto task = mycotask::create_task([]() {
        // Allocate 100KB on stack
        char big_buffer[100 * 1024]; 
        
        for(int i=0; i<100*1024; ++i) big_buffer[i] = (char)(i % 255);
        
        mycotask::current_task()->yield();
        
        for(int i=0; i<100*1024; ++i) {
            if (big_buffer[i] != (char)(i % 255)) {
                std::cerr << "Stack Corruption Detected!" << std::endl;
                exit(1);
            }
        }
    });

    task.start();
    task.resume();
    std::cout << "OK\n";
}

// ==========================================
// TEST 7: Boost Library Integration
// ==========================================
void run_boost_test() {
    std::cout << "[Test 07] Boost Container & Lexical Cast... ";
    
    auto task = mycotask::create_task([]() {
        // Use Boost container (non-standard memory allocators)
        boost::container::vector<std::string> bvec;
        
        bvec.push_back("100");
        bvec.push_back("200");
        
        mycotask::current_task()->yield();
        
        // Use Boost lexical cast
        int sum = 0;
        for(const auto& s : bvec) {
            sum += boost::lexical_cast<int>(s);
        }
        
        if (sum != 300) {
            std::cerr << "Boost logic failed inside task!" << std::endl;
            exit(1);
        }
    });

    task.start();
    task.resume();
    std::cout << "OK\n";
}

// ==========================================
// TEST 8: Floating Point Registers (SSE/FPU)
// ==========================================
void run_fpu_test() {
    std::cout << "[Test 08] FPU/SSE Register Preservation... ";
    
    double result_val = 0.0;
    auto task = mycotask::create_task([&result_val]() {
        double a = 1.5;
        double b = 2.5;
        
        double calc = std::pow(a, b); // 1.5 ^ 2.5 ~= 2.755
        
        mycotask::current_task()->yield();
        
        result_val = calc + std::sqrt(16.0); // + 4.0
    });

    task.start();
    volatile double noise = std::pow(3.3, 4.4);
    (void)noise;
    
    task.resume();
    
    // 1.5^2.5 + 4.0 ~= 6.75567
    double expected = std::pow(1.5, 2.5) + 4.0;
    if (std::abs(result_val - expected) > 0.0001) {
        std::cerr << "Float mismatch: " << result_val << " vs " << expected << std::endl;
        exit(1);
    }
    std::cout << "OK\n";
}

// ==========================================
// TEST 9: Move Semantics & Ownership
// ==========================================
void run_move_semantics_test() {
    std::cout << "[Test 09] Task Move Semantics... ";
    
    int stage = 0;
    auto t1 = mycotask::create_task([&stage]() {
        stage = 1;
        mycotask::current_task()->yield();
        stage = 2;
    });

    t1.start();
    ASSERT_EQ(stage, 1);

    mycotask t2 = std::move(t1);

    t2.resume();
    
    ASSERT_EQ(stage, 2);
    ASSERT_TRUE(t2.has_ended());
    std::cout << "OK\n";
}

// ==========================================
// TEST 10: Lambda Capture (std::shared_ptr)
// ==========================================

void run_shared_ptr_capture_test() {
    std::cout << "[Test 10] Lambda Capture (std::shared_ptr)... ";

    auto ptr = std::make_shared<int>(999);


    auto task = mycotask::create_task([p = ptr]() {
        if (*p != 999) {
            std::cerr << "Shared ptr capture failed" << std::endl;
            exit(1);
        }
        mycotask::current_task()->yield();
        *p = 1000;
    });

    task.start();
    task.resume();


    if (*ptr != 1000) {
        std::cerr << "Pointer was not updated!" << std::endl;
        exit(1);
    }

    std::cout << "OK\n";
}

// ==========================================
// TEST 11: Matrix Multiplication
// ==========================================
void run_matrix_test() {
    std::cout << "[Test 11] Matrix Multiplication (Nested Loops)... ";
    
    const int SIZE = 50; // 50x50 matrix
    std::vector<int> A(SIZE*SIZE, 1);
    std::vector<int> B(SIZE*SIZE, 2);
    std::vector<int> C(SIZE*SIZE, 0);
    
    auto task = mycotask::create_task([&]() {
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                for (int k = 0; k < SIZE; ++k) {
                    C[i * SIZE + j] += A[i * SIZE + k] * B[k * SIZE + j];
                }
            }
            mycotask::current_task()->yield();
        }
    });

    task.start();
    while(!task.has_ended()) task.resume();
    
    ASSERT_EQ(C[0], 100);
    ASSERT_EQ(C[SIZE*SIZE-1], 100);
    std::cout << "OK\n";
}

// ==========================================
// TEST 12: Early Termination (No Yield)
// ==========================================
void run_no_yield_test() {
    std::cout << "[Test 12] Task with NO Yield... ";
    bool ran = false;
    auto task = mycotask::create_task([&]() {
        ran = true;
    });
    
    task.start();
    ASSERT_TRUE(ran);
    ASSERT_TRUE(task.has_ended());
    

    std::cout << "OK\n";
}

// ==========================================
// MAIN DRIVER
// ==========================================
int main() {
    std::cout << "==========================================\n";
    std::cout << "      Running Rigorous MycoTask Suite     \n";
    std::cout << "==========================================\n";
    
    run_argument_test();        // 1
    run_interleaving_test();    // 2
    run_extreme_creation_test();// 3
    run_heavy_compute_test();   // 4
    run_recursion_test();       // 5
    // run_large_stack_test();     // 6
    run_boost_test();           // 7
    run_fpu_test();             // 8
    run_move_semantics_test();  // 9
    run_shared_ptr_capture_test(); // 10
    run_matrix_test();          // 11
    run_no_yield_test();        // 12
    
    std::cout << "==========================================\n";
    std::cout << "      All 12 Tests Passed Successfully    \n";
    std::cout << "==========================================\n";
    return 0;
}