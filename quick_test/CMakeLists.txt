add_executable(quick_test
    main.cpp
)

target_compile_options(UISandbox PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wpedantic -Werror -Wshadow -Wconversion -Wsign-conversion
    >
)
target_link_libraries(quick_test PRIVATE
  SFML::Graphics
  logic
  data_providers
)

