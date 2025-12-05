cmake_minimum_required(VERSION 3.15)

project(benchmark_mycotask)

## We go up from: tests/benchmark_mycotask -> tests -> root -> (out of project) -> libcopp
## No one would run this, yes??!?!?!
set(LIBCOPP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../libcopp")

if(NOT EXISTS "${LIBCOPP_PATH}")
    set(LIBCOPP_PATH "/home/julfy/Documents/5th_term/os/coroutine_library_project/libcopp")
endif()

message(STATUS "Looking for libcopp at: ${LIBCOPP_PATH}")

if(EXISTS "${LIBCOPP_PATH}/CMakeLists.txt")
    set(LIBCOPP_ENABLE_TEST OFF CACHE BOOL "" FORCE)
    set(LIBCOPP_ENABLE_SAMPLE OFF CACHE BOOL "" FORCE)


    add_subdirectory("${LIBCOPP_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/libcopp_build" EXCLUDE_FROM_ALL)
else()
    message(FATAL_ERROR "Could not find libcopp at ${LIBCOPP_PATH}. Please check the path.")
endif()

add_executable(bench_compare compare_vs_libcopp.cpp)

# Link MycoTask dependencies
target_link_libraries(bench_compare PRIVATE libcoro)


if(TARGET copp)
    target_link_libraries(bench_compare PRIVATE copp)
    message(STATUS "Linked against local target 'copp'")
else()
    message(FATAL_ERROR "Libcopp was added, but target 'copp' was not found!")
endif()

target_include_directories(bench_compare PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../src
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_link_options(bench_compare PRIVATE "-no-pie")
endif()
