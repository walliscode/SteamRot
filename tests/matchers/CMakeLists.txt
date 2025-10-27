# Matchers library for test utilities
# Provides custom Catch2 matchers for component and entity comparisons

add_library(test_matchers INTERFACE)

target_include_directories(test_matchers
INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_matchers
INTERFACE
  Catch2::Catch2WithMain
  components
  entity
  user_interface
)

# Test executable for component matchers
add_executable(test_component_matchers
  component_matchers.test.cpp
)

target_link_libraries(test_component_matchers
PRIVATE
  Catch2::Catch2WithMain
  test_matchers
  components
  user_interface
)

catch_discover_tests(test_component_matchers)

# Test executable for entity memory pool matchers
add_executable(test_entity_memory_pool_matchers
  entity_memory_pool_matchers.test.cpp
)

target_link_libraries(test_entity_memory_pool_matchers
PRIVATE
  Catch2::Catch2WithMain
  test_matchers
  components
  entity
)

catch_discover_tests(test_entity_memory_pool_matchers)
