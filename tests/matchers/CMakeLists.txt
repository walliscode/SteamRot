# Matchers library for test utilities
# Provides custom Catch2 matchers for component and entity comparisons

add_library(matchers
  CMetaEqualsMatcher.cpp
  CGrimoireMachinaEqualsMatcher.cpp
  CUserInterfaceEqualsMatcher.cpp
  CUIStateEqualsMatcher.cpp
  CMachinaFormEqualsMatcher.cpp
  EventPacketEqualsMatcher.cpp
  EventBusEqualsMatcher.cpp
  FragmentEqualsMatcher.cpp
  JointEqualsMatcher.cpp
  UIElementEqualsMatcher.cpp
  EntityMemoryPoolEqualsMatcher.cpp
)

target_include_directories(matchers
PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(matchers
PUBLIC
  Catch2::Catch2WithMain
  components
  entity
  user_interface
  events
  test_harness
  conmat
)

# Test executable for all matcher tests
add_executable(test_matchers
  component_matchers.test.cpp
  CMetaEqualsMatcher.test.cpp
  CGrimoireMachinaEqualsMatcher.test.cpp
  CUserInterfaceEqualsMatcher.test.cpp
  CUIStateEqualsMatcher.test.cpp
  entity_memory_pool_matchers.test.cpp
  event_matchers.test.cpp
  CMachinaFormEqualsMatcher.test.cpp
  EventPacketEqualsMatcher.test.cpp
  EventBusEqualsMatcher.test.cpp
  FragmentEqualsMatcher.test.cpp
  JointEqualsMatcher.test.cpp
  UIElementEqualsMatcher.test.cpp
  EntityMemoryPoolEqualsMatcher.test.cpp
)

target_link_libraries(test_matchers
PRIVATE
  Catch2::Catch2WithMain
  matchers
  components
  entity
  user_interface
  events
)

catch_discover_tests(test_matchers)
