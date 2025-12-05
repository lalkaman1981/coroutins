cmake_minimum_required(VERSION 3.15)

project(benchmark_mycotask)

# --- 1. Define the path to your local libcopp ---
# We go up from: tests/benchmark_mycotask -> tests -> root -> (out of project) -> libcopp
set(LIBCOPP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../libcopp")

if(NOT EXISTS "${LIBCOPP_PATH}")
    set(LIBCOPP_PATH "/home/julfy/Documents/5th_term/os/coroutine_library_project/libcopp")
endif()

message(STATUS "Looking for libcopp at: ${LIBCOPP_PATH}")

if(EXISTS "${LIBCOPP_PATH}/CMakeLists.txt")
    # --- 2. Configure LibCopp Options (Disable their tests to save time) ---
    set(LIBCOPP_ENABLE_TEST OFF CACHE BOOL "" FORCE)
    set(LIBCOPP_ENABLE_SAMPLE OFF CACHE BOOL "" FORCE)

    # --- 3. Add it as a subdirectory ---
    # We specify a binary directory (2nd arg) to keep build files organized
    add_subdirectory("${LIBCOPP_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/libcopp_build" EXCLUDE_FROM_ALL)
else()
    message(FATAL_ERROR "Could not find libcopp at ${LIBCOPP_PATH}. Please check the path.")
endif()

# --- 4. Build the Benchmark ---
add_executable(bench_compare compare_vs_libcopp.cpp)

# Link MycoTask dependencies
target_link_libraries(bench_compare PRIVATE libcoro)

# --- 5. Link against the 'copp' target created by add_subdirectory ---
# When building from source, the target is usually named 'copp', not 'libcopp::libcopp'
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
