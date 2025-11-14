add_library(libcoro STATIC
    ./create_stack/create_stack.cpp
    ./create_stack/create_stack.S
    ./create_stack/switch_context.S
    libmycotask/mycotask.cpp
    libmycomanager/mycomanager.cpp
)

target_include_directories(libcoro PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

