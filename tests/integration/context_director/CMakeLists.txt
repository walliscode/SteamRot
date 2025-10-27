add_executable(test_context_director
  context_director.integration.test.cpp
)

target_link_libraries(test_context_director
PRIVATE
  Catch2::Catch2WithMain
  context
  logic
  data_handlers
  systems
  flatbuffers_headers
  context_test_helpers
)

catch_discover_tests(test_context_director)
