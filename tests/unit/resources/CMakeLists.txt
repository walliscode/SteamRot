add_executable(test_resources
  resources_configuration.test.cpp
)

target_link_libraries(test_resources
  PRIVATE
  Catch2::Catch2WithMain
  resources
  data_handlers
  flatbuffers_headers
  SFML::Graphics
)

add_test(NAME test_resources COMMAND test_resources)
set_tests_properties(test_resources PROPERTIES LABELS "unit;resources")
