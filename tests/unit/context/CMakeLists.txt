add_executable(test_context
  ContextConfigurator.test.cpp
)

target_include_directories(test_context
PRIVATE
  ${CMAKE_SOURCE_DIR}/tests/unit/context
)

target_link_libraries(test_context
PRIVATE
  Catch2::Catch2WithMain
  context
  logic
  systems
  data_handlers
  flatbuffers_headers
  resources
)

catch_discover_tests(test_context)
