add_library(descriptors
  ChainDescriptorBuilder.cpp
  descriptors_general.cpp
  descriptors_node_descriptors.cpp
  descriptors_chain_descriptors.cpp
  descriptors_machina_archetypes.cpp
  TerminalDescriptorFormatter.cpp
  depth_first_search.cpp
  descriptors_runner.cpp
)

target_include_directories(descriptors
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(descriptors PUBLIC
      types
      conmat

)
