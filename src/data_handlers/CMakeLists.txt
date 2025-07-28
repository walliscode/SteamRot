
cmake_path(SET data_dir ${CMAKE_SOURCE_DIR}/data)
cmake_path(SET test_data_dir ${CMAKE_SOURCE_DIR}/tests/data)

configure_file(
PathProvider.cpp.in
PathProvider.cpp
@ONLY)

add_library(data_handlers
DataLoader.cpp
  FlatbuffersDataLoader.cpp
  PathProvider.cpp
)

target_include_directories(data_handlers
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(data_handlers
  PUBLIC
  components
)
