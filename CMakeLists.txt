cmake_minimum_required(VERSION 3.15)

#! Check every comment after the "#!"

#! CHANGE YOUR PROJECT NAME
#  It is used as your project's main executable name.
set(PROJECT_NAME coroutines)
set(TEST_COROUTINES test_coroutines)
set(STRUCTURED_PROJECT_NAME libmycor)
set(TEST_MY_COTASK test_mycotask)
set(TEST_MYCOMANAGER test_mycomanager)

project(${PROJECT_NAME} C CXX ASM) # project(${PROJECT_NAME} C CXX ASM)

enable_language(ASM)


set(CMAKE_CXX_STANDARD 20)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fno-omit-frame-pointer")

##########################################################
# User configurable options of the template
##########################################################
set(WARNINGS_AS_ERRORS OFF)

set(ENABLE_PVS_STUDIO OFF)

set(ENABLE_UBSan OFF)
set(ENABLE_ASAN OFF)
set(ENABLE_TSan OFF)
set(ENABLE_MSAN OFF)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif ()

# Warnings as errors should be imported here -- do not move this line
include(cmake/CompilerWarnings.cmake)

##########################################################
# Project files, packages, libraries and so on
##########################################################

add_subdirectory(src)
#================================================================
#! Project main executable source compilation
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE libcoro)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_link_options(${PROJECT_NAME} PRIVATE "-no-pie")
endif()

#================================================================
add_executable(${TEST_MY_COTASK} test_mycotask.cpp)

target_link_libraries(${TEST_MY_COTASK} PRIVATE libcoro)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_link_options(${TEST_MY_COTASK} PRIVATE "-no-pie")
endif()
#================================================================

# add_executable(${TEST_MYCOMANAGER} test_mycomanager.cpp)

# target_link_libraries(${TEST_MYCOMANAGER} PRIVATE libcoro)
# if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
#   target_link_options(${TEST_MYCOMANAGER} PRIVATE "-no-pie")
# endif()
#================================================================

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -no-pie")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-pie -no-pie")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-pie -no-pie")

##########################################################
# Fixed CMakeLists.txt part
##########################################################

INSTALL(PROGRAMS
  $<TARGET_FILE:${PROJECT_NAME}> # ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
  DESTINATION bin)

# Define ALL_TARGETS variable to use in PVS and Sanitizers
set(ALL_TARGETS ${PROJECT_NAME})

# Include CMake setup
include(cmake/main-config.cmake)
