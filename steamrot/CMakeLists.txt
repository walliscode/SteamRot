
add_executable(SteamRot main.cpp)

target_include_directories(SteamRot PUBLIC

    ${CMAKE_SOURCE_DIR}/include

)

target_link_libraries(SteamRot PRIVATE
    systems
)

message("Include dir: ${CMAKE_SOURCE_DIR}/include")


