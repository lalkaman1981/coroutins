project(mycotask_tests)

add_executable(unit_tests unit_test_mycotask.cpp)
target_link_libraries(unit_tests PRIVATE libcoro)
target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_link_options(unit_tests PRIVATE "-no-pie")
endif()

add_subdirectory(benchmark_mycotask)
