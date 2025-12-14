add_library(interfaces
  ISceneConfigurator.cpp
)

target_include_directories(interfaces
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(interfaces
  PUBLIC
  types
  PRIVATE
  logic
  scenes
)

