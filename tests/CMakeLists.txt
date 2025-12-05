# tests/CMakeLists.txt
project(mycotask_tests)

# 1. Setup Unit Tests
add_executable(unit_tests unit_test_mycotask.cpp)
target_link_libraries(unit_tests PRIVATE libcoro) # Add Boost if needed here
target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_link_options(unit_tests PRIVATE "-no-pie")
endif()

# 2. Add the Benchmark Subdirectory
add_subdirectory(benchmark_mycotask)
