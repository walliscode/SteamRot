# Test executable for all matcher tests
add_executable(test_matchers
  CMetaEqualsMatcher.test.cpp
  CUserInterfaceEqualsMatcher.test.cpp
  CUIStateEqualsMatcher.test.cpp
  EventPacketEqualsMatcher.test.cpp
  EventDataEqualsMatcher.test.cpp
  EventPayloadEqualsMatcher.test.cpp
  EventBusEqualsMatcher.test.cpp
  UIElementEqualsMatcher.test.cpp
  EntityMemoryPoolEqualsMatcher.test.cpp
  EntityTransportEqualsMatcher.test.cpp
  SubscriberEqualsMatcher.test.cpp
  SceneInfoEqualsMatcher.test.cpp
  SceneDataEqualsMatcher.test.cpp
  SceneCollectionDataEqualsMatcher.test.cpp
  EngineSnapshotEqualsMatcher.test.cpp
  Vector2fEqualsMatcher.test.cpp
)

target_link_libraries(test_matchers
PRIVATE
  Catch2::Catch2WithMain
  matchers
  entity
  events
)

catch_discover_tests(test_matchers)
