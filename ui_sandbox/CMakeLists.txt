add_executable(UISandbox main.cpp)

target_compile_options(UISandbox PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wpedantic -Werror -Wshadow -Wconversion -Wsign-conversion
    >
)

target_link_libraries(UISandbox PRIVATE
    engine
    error_loop
    display
    types
    # logger
)
