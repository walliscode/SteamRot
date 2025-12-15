
add_executable(SteamRot main.cpp)

# Use production paths for the main executable
target_compile_definitions(SteamRot PRIVATE STEAMROT_ENV_PROD)

target_compile_options(SteamRot PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wpedantic -Werror -Wshadow -Wconversion -Wsign-conversion
    >
)

target_link_libraries(SteamRot PRIVATE
    engine
    display
    types
    logger
    interfaces
)

