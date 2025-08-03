
add_executable(SteamRot main.cpp)

target_compile_options(SteamRot PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wpedantic -Werror -Wshadow -Wconversion -Wsign-conversion
    >
)
target_include_directories(SteamRot PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(SteamRot PUBLIC
    systems
    flatbuffers
)


