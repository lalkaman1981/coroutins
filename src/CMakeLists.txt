add_library(libcoro STATIC
    ./create_stack/create_stack.cpp
    ./create_stack/create_stack.S
    ./create_stack/switch_context.S
        mycotask/mycotask.cpp
        mycomanager/mycomanager.cpp
        ./my_fp/myfp.cpp
)

target_include_directories(libcoro PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(Boost REQUIRED)
target_link_libraries(libcoro PUBLIC Boost::boost)
