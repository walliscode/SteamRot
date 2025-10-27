add_executable(test_context_configuration
  context_configuration.integration.test.cpp
)

target_link_libraries(test_context_configuration
PRIVATE
  Catch2::Catch2WithMain
  context
  logic
  data_handlers
  systems
  flatbuffers_headers
)

catch_discover_tests(test_context_configuration)
