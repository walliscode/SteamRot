add_library(logic
LogicFactory.cpp
  UIRenderLogic.cpp
  UICollisionLogic.cpp
  UIActionLogic.cpp
  BaseLogic.cpp
  CraftingRenderLogic.cpp
)

target_include_directories(logic
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(logic PUBLIC
  components
  entity
  flatbuffers
  flatbuffers_headers
  systems
)
