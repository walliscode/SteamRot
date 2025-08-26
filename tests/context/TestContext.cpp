/////////////////////////////////////////////////
/// @file
/// @brief Implementation of TestContext object
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestContext.h"
#include "GameContext.h"
#include "PathProvider.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
TestContext::TestContext()
    : render_window(sf::VideoMode({800, 600}), "SteamRot Test Window"),
      event_handler(), asset_manager(EnvironmentType::Test),

      game_context(render_window, event_handler, mouse_position, loop_number,
                   asset_manager, EnvironmentType::Test),
      logic_context(scene_entities, archetypes, render_texture, render_window,
                    asset_manager, event_handler) {}

/////////////////////////////////////////////////
const GameContext &TestContext::GetGameContext() const { return game_context; }

/////////////////////////////////////////////////
const LogicContext &TestContext::GetLogicContext() const {
  return logic_context;
}

} // namespace steamrot::tests
