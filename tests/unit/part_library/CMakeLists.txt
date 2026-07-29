add_executable(test_part_library
  FragmentBuilder.test.cpp
  JointBuilder.test.cpp
  fragment_library.test.cpp
  joint_library.test.cpp
  PartGraphBuilder.test.cpp
  # part_graph_library.test.cpp
)

target_link_libraries(test_part_library
  PRIVATE
  part_library
  Catch2::Catch2WithMain
)

catch_discover_tests(test_part_library)
