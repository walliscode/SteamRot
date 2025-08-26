/////////////////////////////////////////////////
/// @file
/// @brief Implementation of helper functions for logic tests
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "logic_helpers.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
LogicContext CreateLogicContext() {

  // construct EntityMemoryPool
  EntityMemoryPool entity_pool;
  // construct ArchetypeManager
  ArchetypeManager archetype_manager(entity_pool);
  // construct a render texture
  sf::RenderTexture render_texture;
  // create a render window
  sf::RenderWindow render_window(sf::VideoMode({800, 600}), "Test Window");
  // construct AssetManager
  AssetManager asset_manager{EnvironmentType::Test};
  // construct EventHandler
  EventHandler event_handler;
  // create a LogicContext object
  LogicContext context{entity_pool,    archetype_manager.GetArchetypes(),
                       render_texture, render_window,
                       asset_manager,  event_handler};
  return context;
}
} // namespace steamrot::tests
