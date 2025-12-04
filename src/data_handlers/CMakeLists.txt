
# New namespace-based paths (compile-time environment selection)
configure_file(
paths.cpp.in
paths.cpp
@ONLY)

add_library(data_handlers
  DataLoader.cpp
  FlatbuffersDataLoader.cpp
  FlatBuffersGameDataProvider.cpp
  data_file_utils.cpp
  paths.cpp
)

target_include_directories(data_handlers
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(data_handlers
  PUBLIC
  components
  flatbuffers
  flatbuffers_headers
  logger
  magic_enum
)
