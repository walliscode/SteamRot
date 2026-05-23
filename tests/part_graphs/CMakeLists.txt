add_library(part_graphs
  PartGraphBuilder.cpp
  part_library.cpp
)

target_include_directories(part_graphs
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(part_graphs
  PRIVATE
  types
)
