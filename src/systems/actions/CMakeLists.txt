
add_library(Actions Action.cpp)

message("Assets current source dir: ${CMAKE_CURRENT_SOURCE_DIR}/include")

target_include_directories(Actions PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

get_target_property(ACTIONS_INCLUDE_DIRS Actions INCLUDE_DIRECTORIES)
message("Actions include directories: ${ACTIONS_INCLUDE_DIRS}")
