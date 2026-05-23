add_library(part_graphs
  PartGraphBuilder.cpp
  part_graph_library.cpp
)

target_include_directories(part_graphs
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(part_graphs
  PUBLIC
    types
    logic
    Catch2::Catch2
)

add_executable(test_part_graphs
  PartGraphBuilder.test.cpp
)

target_link_libraries(test_part_graphs
  PRIVATE
    Catch2::Catch2WithMain
    part_graphs
    matchers
)

catch_discover_tests(test_part_graphs)
