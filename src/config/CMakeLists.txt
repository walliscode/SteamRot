
cmake_path(SET data_path ${CMAKE_SOURCE_DIR}/data)
cmake_path(SET test_path ${CMAKE_SOURCE_DIR}/tests)

configure_file(
  steamrot_directory_paths.cpp.in
  steamrot_directory_paths.cpp
  @ONLY)

add_library(steamrot_directory_paths STATIC
  ${CMAKE_CURRENT_BINARY_DIR}/steamrot_directory_paths.cpp)

target_include_directories(steamrot_directory_paths PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)
