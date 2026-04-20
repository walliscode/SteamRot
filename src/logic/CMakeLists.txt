add_library(logic
  LogicFactory.cpp
  UIRenderLogic.cpp
  UICollisionLogic.cpp
  UIActionLogic.cpp
  UIStateLogic.cpp
  UIPositioningLogic.cpp
  Logic.cpp


  GrimoireMachinaPositioningLogic.cpp
  GhostPositioningLogic.cpp
  GrimoireMachinaActionLogic.cpp
  GrimoireMachinaRenderLogic.cpp
  GrimoireMachinaCollisionLogic.cpp

  GhostActionLogic.cpp
  GhostRenderLogic.cpp

  CameraPositioningLogic.cpp

  action_ui.cpp
  state_ui.cpp
  render_ui.cpp


  render_grimoire_machina.cpp

  render_ghost.cpp

  action_grimoire_machina.cpp

  action_ghost.cpp

  action_camera.cpp

  positioning_camera.cpp

  positioning_grimoire_machina.cpp

  positioning_ui.cpp

  positioning_ghost.cpp

  collision_mouse.cpp

  collision_grimoire_machina.cpp
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
