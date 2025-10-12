cmake_minimum_required(VERSION 3.15)

#! Check every comment after the "#!"

#! CHANGE YOUR PROJECT NAME
#  It is used as your project's main executable name. 
set(PROJECT_NAME coroutines)
project(${PROJECT_NAME} C CXX ASM) # project(${PROJECT_NAME} C CXX ASM)
enable_language(ASM)


set(CMAKE_CXX_STANDARD 20)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fno-omit-frame-pointer")

##########################################################
# User configurable options of the template
##########################################################
# Note: symbols like WARNINGS_AS_ERRORS in configuration are intentionally variables
# and not CMake options --using options creates too many problems for students.

#! It is a good practice to set "WARNINGS_AS_ERRORS" ON,
#  but sometimes it creates too much trouble, so default is OFF.
set(WARNINGS_AS_ERRORS OFF)

#! Always use PVS Studio while developing.
set(ENABLE_PVS_STUDIO OFF)

#! Select appropriate sanitizers.
#  Definitely enable sanitizers while developing.
#  Disable it for the production builds and before submitting for grading.
#  Only one of the Memory (MSAN), Address (ASAN), or Thread (TSan)
#  sanitizers is applicable at the time -- the first defined.

#! UndefinedBehaviorSanitizer (UBSan).
#  Info: https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html
set(ENABLE_UBSan OFF)
#! AddressSanitizer -- detects use after free or after scope exit,
#  memory overflows and leaks.
#  Info: https://github.com/google/sanitizers/wiki/AddressSanitizer
set(ENABLE_ASAN OFF)
#! ThreadSanitizer -- detects data races.
#  Info: https://clang.llvm.org/docs/ThreadSanitizer.html
set(ENABLE_TSan OFF)
#! MemorySanitizer -- detects uninitialized memory reads
#  Info: https://github.com/google/sanitizers/wiki/MemorySanitizer
set(ENABLE_MSAN OFF)

#! By default -- build release version if not specified otherwise.
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif ()

# Warnings as errors should be imported here -- do not move this line
include(cmake/CompilerWarnings.cmake)

##########################################################
# Project files, packages, libraries and so on
##########################################################

#! Project main executable source compilation
add_executable(${PROJECT_NAME} src/create_stack/src/main.cpp
				src/create_stack/src/create_stack.cpp
				src/create_stack/src/create_stack.S
)

add_executable(test_coro_switch src/create_stack/src/test_coro_switch.cpp
								src/create_stack/src/switch_context.S
)

#! Put path to your project headers
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(test_coro_switch PRIVATE include)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_link_options(${PROJECT_NAME} PRIVATE "-no-pie")
endif()


set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -no-pie")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-pie -no-pie")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-pie -no-pie")



#! Add external packages
# options_parser requires boost::program_options library
find_package(Boost 1.71.0 COMPONENTS program_options system REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} Boost::program_options Boost::system)

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
