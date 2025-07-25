add_library(data_handlers
  FlatbuffersDataLoader.cpp
)

target_include_directories(data_handlers
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(data_handlers
  PUBLIC
  components
)
