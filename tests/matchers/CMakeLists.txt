# Matchers library for test utilities
# Provides custom Catch2 matchers for component and entity comparisons

add_library(matchers
  CMetaEqualsMatcher.cpp
  CUserInterfaceEqualsMatcher.cpp
  CUIStateEqualsMatcher.cpp
  EventPacketEqualsMatcher.cpp
  EventPayloadEqualsMatcher.cpp
  EventBusEqualsMatcher.cpp
  UIElementEqualsMatcher.cpp
  EntityMemoryPoolEqualsMatcher.cpp
  EntityTransportEqualsMatcher.cpp
  SubscriberEqualsMatcher.cpp
  SceneInfoEqualsMatcher.cpp
  SceneDataEqualsMatcher.cpp
  SceneCollectionDataEqualsMatcher.cpp
  EngineSnapshotEqualsMatcher.cpp
  Vector2fEqualsMatcher.cpp
  PixelImageEqualsMatcher.cpp
  AnalysisTraceBuilder.cpp
  TraceEqualsMatcher.cpp
)

target_include_directories(matchers
PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(matchers
PUBLIC
  Catch2::Catch2WithMain
  types
  entity
  events
  conmat
  magic_enum
  SFML::Graphics
  logic
)
