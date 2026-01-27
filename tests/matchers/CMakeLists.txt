# Matchers library for test utilities
# Provides custom Catch2 matchers for component and entity comparisons

add_library(matchers
  CMetaEqualsMatcher.cpp
  CGrimoireMachinaEqualsMatcher.cpp
  CUserInterfaceEqualsMatcher.cpp
  CUIStateEqualsMatcher.cpp
  CMachinaFormEqualsMatcher.cpp
  EventPacketEqualsMatcher.cpp
  EventDataEqualsMatcher.cpp
  EventBusEqualsMatcher.cpp
  FragmentEqualsMatcher.cpp
  JointEqualsMatcher.cpp
  UIElementEqualsMatcher.cpp
  EntityMemoryPoolEqualsMatcher.cpp
  SubscriberEqualsMatcher.cpp
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
)
