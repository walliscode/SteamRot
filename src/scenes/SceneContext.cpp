/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the SceneContext struct.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneContext.h"

namespace steamrot {

/////////////////////////////////////////////////
SceneContext::SceneContext(SceneCore &scene_core, GameCore &game_core,
                           EntityManager &entity_manager)
    : scene_entities(entity_manager.GetEntityMemoryPool()),
      archetypes(entity_manager.GetArchetypeManager().GetArchetypes()),
      scene_texture(scene_core.scene_texture),
      game_window(game_core.game_window),
      asset_manager(game_core.asset_manager),
      event_handler(game_core.event_handler),
      mouse_position(game_core.mouse_position) {}

} // namespace steamrot
