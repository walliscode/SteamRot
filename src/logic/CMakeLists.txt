add_library(logic
  LogicFactory.cpp
  UIRenderLogic.cpp
  UICollisionLogic.cpp
  UIActionLogic.cpp
  UIStateLogic.cpp
  Logic.cpp
  CraftingRenderLogic.cpp
  logic_collision.cpp
  logic_ui.cpp
  LogicContextBuilder.cpp
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
  ui_styles
  user_interface
)
