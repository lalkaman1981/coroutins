cmake_minimum_required(VERSION 3.15)

project(benchmark_mycomanager)

set(LIBCOPP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../libcopp")
if(NOT EXISTS "${LIBCOPP_PATH}")
    set(LIBCOPP_PATH "/home/julfy/Documents/5th_term/os/coroutine_library_project/libcopp")
endif()

if(NOT TARGET copp)
    if(EXISTS "${LIBCOPP_PATH}/CMakeLists.txt")
        set(LIBCOPP_ENABLE_TEST OFF CACHE BOOL "" FORCE)
        set(LIBCOPP_ENABLE_SAMPLE OFF CACHE BOOL "" FORCE)
        add_subdirectory("${LIBCOPP_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/libcopp_build" EXCLUDE_FROM_ALL)
    else()
        message(FATAL_ERROR "Could not find libcopp at ${LIBCOPP_PATH}")
    endif()
endif()

add_executable(compare_vs_libcopp compare_vs_libcopp.cpp)

target_link_libraries(compare_vs_libcopp PRIVATE libcoro)

if(TARGET copp AND TARGET cotask)
    target_link_libraries(compare_vs_libcopp PRIVATE copp cotask)
    message(STATUS "Linked against targets 'copp' and 'cotask'")
else()
    message(FATAL_ERROR "Targets 'copp' or 'cotask' not found!")
endif()

target_include_directories(compare_vs_libcopp PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../src
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_link_options(compare_vs_libcopp PRIVATE "-no-pie")
endif()
