add_executable(test_context
  GameContextBuilder.test.cpp
  LogicContextBuilder.test.cpp
  ContextConfigurator.test.cpp
  ContextDirector.test.cpp
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
)

catch_discover_tests(test_context)
