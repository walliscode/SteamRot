cmake_path(SET data_path ${CMAKE_SOURCE_DIR}/data)

# creates directory_paths.cpp with resolved paths in it
configure_file(
  directory_paths.cpp.in
  directory_paths.cpp
  @ONLY)

add_library(directory_paths STATIC
  ${CMAKE_CURRENT_BINARY_DIR}/directory_paths.cpp)

target_include_directories(directory_paths PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)
