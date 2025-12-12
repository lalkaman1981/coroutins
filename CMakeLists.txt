cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME coroutines)
set(TEST_COROUTINES test_coroutines)
set(STRUCTURED_PROJECT_NAME libmycor)
set(TEST_MY_COTASK test_mycotask)
set(TEST_MYCOMANAGER test_mycomanager)
set(TEST_SWITCH_CONTEXT test_switch_context)

project(${PROJECT_NAME} C CXX ASM)

enable_language(ASM)


set(CMAKE_CXX_STANDARD 20)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -fno-omit-frame-pointer")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fno-omit-frame-pointer") ## set all the build types to -O0

if(CMAKE_BUILD_TYPE STREQUAL "Debug") ## Now Release is without -O0
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fno-omit-frame-pointer")
endif()

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

include(cmake/CompilerWarnings.cmake)

##########################################################
# Project files, packages, libraries and so on
##########################################################

add_subdirectory(src)
include_directories(include)

#================================================================
add_executable(${PROJECT_NAME} main.cpp)

if(WIN32)
  target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE libcoro)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_link_options(${PROJECT_NAME} PRIVATE "-no-pie")
endif()

#================================================================
add_executable(${TEST_MY_COTASK} examples/mycotask_test_example.cpp)

target_link_libraries(${TEST_MY_COTASK} PRIVATE libcoro)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_link_options(${TEST_MY_COTASK} PRIVATE "-no-pie")
endif()
#================================================================

add_executable(${TEST_MYCOMANAGER} examples/mycomanager_test_example.cpp)

target_link_libraries(${TEST_MYCOMANAGER} PRIVATE libcoro)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_link_options(${TEST_MYCOMANAGER} PRIVATE "-no-pie")
endif()
#================================================================

add_executable(${TEST_SWITCH_CONTEXT} examples/switch_context_text.cpp)

target_link_libraries(${TEST_SWITCH_CONTEXT} PRIVATE libcoro)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_link_options(${TEST_SWITCH_CONTEXT} PRIVATE "-no-pie")
endif()
#================================================================


set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -no-pie")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-pie -no-pie")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-pie -no-pie")

##########################################################
# Fixed CMakeLists.txt part
##########################################################

INSTALL(PROGRAMS
  $<TARGET_FILE:${PROJECT_NAME}>
  DESTINATION bin)
set(ALL_TARGETS ${PROJECT_NAME})
include(cmake/main-config.cmake)
# add_subdirectory(tests)
