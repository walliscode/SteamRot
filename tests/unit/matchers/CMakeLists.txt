# Test executable for all matcher tests
add_executable(test_matchers
  CMetaEqualsMatcher.test.cpp
  CGrimoireMachinaEqualsMatcher.test.cpp
  CUserInterfaceEqualsMatcher.test.cpp
  CUIStateEqualsMatcher.test.cpp
  CMachinaFormEqualsMatcher.test.cpp
  EventPacketEqualsMatcher.test.cpp
  EventDataEqualsMatcher.test.cpp
  EventBusEqualsMatcher.test.cpp
  FragmentEqualsMatcher.test.cpp
  JointEqualsMatcher.test.cpp
  UIElementEqualsMatcher.test.cpp
  EntityMemoryPoolEqualsMatcher.test.cpp
  SubscriberEqualsMatcher.test.cpp
)

target_link_libraries(test_matchers
PRIVATE
  Catch2::Catch2WithMain
  matchers
  entity
  events
)

catch_discover_tests(test_matchers)
