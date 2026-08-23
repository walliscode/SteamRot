add_library(part_library
  FragmentBuilder.cpp
  JointBuilder.cpp
  PartGraphBuilder.cpp
  part_graph_library.cpp
  machina_archetype_packages.cpp
)

target_include_directories(part_library
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(part_library
  PUBLIC
    types
    Catch2::Catch2
)

