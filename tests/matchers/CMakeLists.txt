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
  events
  test_harness
  conmat
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

# Test executable for event matchers
add_executable(test_event_matchers
  event_matchers.test.cpp
)

target_link_libraries(test_event_matchers
PRIVATE
  Catch2::Catch2WithMain
  test_matchers
  events
)

catch_discover_tests(test_event_matchers)

# Test executable for isolated CMeta matcher tests
add_executable(test_cmeta_matchers_isolated
  cmeta_matchers_isolated.test.cpp
)

target_link_libraries(test_cmeta_matchers_isolated
PRIVATE
  Catch2::Catch2WithMain
  test_matchers
  components
)

catch_discover_tests(test_cmeta_matchers_isolated)

# Test executable for isolated EntityMemoryPool matcher tests
add_executable(test_entity_memory_pool_matchers_isolated
  entity_memory_pool_matchers_isolated.test.cpp
)

target_link_libraries(test_entity_memory_pool_matchers_isolated
PRIVATE
  Catch2::Catch2WithMain
  test_matchers
  components
  entity
)

catch_discover_tests(test_entity_memory_pool_matchers_isolated)

# Test executable for isolated CUserInterface matcher tests
add_executable(test_cuser_interface_matchers_isolated
  cuser_interface_matchers_isolated.test.cpp
)

target_link_libraries(test_cuser_interface_matchers_isolated
PRIVATE
  Catch2::Catch2WithMain
  test_matchers
  components
  user_interface
)

catch_discover_tests(test_cuser_interface_matchers_isolated)

# Test executable for isolated UIElement matcher tests
add_executable(test_ui_element_matchers_isolated
  ui_element_matchers_isolated.test.cpp
)

target_link_libraries(test_ui_element_matchers_isolated
PRIVATE
  Catch2::Catch2WithMain
  test_matchers
  user_interface
)

catch_discover_tests(test_ui_element_matchers_isolated)

# Test executable for isolated Event matcher tests
add_executable(test_event_matchers_isolated
  event_matchers_isolated.test.cpp
)

target_link_libraries(test_event_matchers_isolated
PRIVATE
  Catch2::Catch2WithMain
  test_matchers
  events
)

catch_discover_tests(test_event_matchers_isolated)
