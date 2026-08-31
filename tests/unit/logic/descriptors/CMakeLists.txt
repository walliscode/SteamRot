add_executable(test_descriptors
  descriptors_node_descriptors.test.cpp
  descriptors_chain_descriptors.test.cpp
  descriptors_machina_archetypes.test.cpp
  descriptors_analysis_event_helpers.test.cpp
  ChainDescriptorBuilder.test.cpp
  depth_first_search.test.cpp
  MachinaArchetypeBuilder.test.cpp
  descriptors_runner.test.cpp
)

target_include_directories(test_descriptors
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_descriptors
  PRIVATE
  Catch2::Catch2WithMain
  descriptors
  matchers
  part_library
)

catch_discover_tests(test_descriptors)
