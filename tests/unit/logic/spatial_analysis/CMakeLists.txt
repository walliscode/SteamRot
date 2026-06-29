add_executable(test_spatial_analysis
    grab_analysis.test.cpp
)


target_include_directories(test_spatial_analysis
PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_spatial_analysis
  PRIVATE
  Catch2::Catch2WithMain
  spatial_analysis
  part_library
  matchers
)

catch_discover_tests(test_spatial_analysis)
