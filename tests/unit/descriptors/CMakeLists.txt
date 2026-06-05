add_executable(test_descriptors
  ../logic/descriptors/descriptors_node_descriptors.test.cpp
  ../logic/descriptors/descriptors_chain_descriptors.test.cpp
  ../logic/descriptors/descriptors_machina_archetypes.test.cpp
  ../logic/descriptors/descriptors_analysis_event_helpers.test.cpp
  ../logic/descriptors/ChainDescriptorBuilder.test.cpp
  ../logic/descriptors/depth_first_search.test.cpp
  ../logic/descriptors/MachinaArchetypeBuilder.test.cpp
)

target_link_libraries(test_descriptors
  PRIVATE
    Catch2::Catch2WithMain
    logic
    context
    resources
    entity
    matchers
    context_test_helpers
    harness
    part_graphs
    reporters_objects
)

catch_discover_tests(test_descriptors
  REPORTER
    "descriptor"
)
