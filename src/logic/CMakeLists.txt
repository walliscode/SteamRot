add_library(logic
  LogicFactory.cpp
  UIRenderLogic.cpp
  UICollisionLogic.cpp
  UIActionLogic.cpp
  UIStateLogic.cpp
  Logic.cpp


  GrimoireMachinaPositioningLogic.cpp
  GrimoireMachinaActionLogic.cpp
  GrimoireMachinaRenderLogic.cpp

  ui_collision.cpp
  ui_action.cpp
  ui_helpers.cpp
  ui_render.cpp


  render_grimoire_machina.cpp

  action_grimoire_machina.cpp


  positioning_grimoire_machina.cpp
)

target_include_directories(logic
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(logic
  PRIVATE
  entity
  types
  magic_enum
  context
  resources

)
