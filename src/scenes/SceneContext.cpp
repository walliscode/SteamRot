/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the SceneContext struct
/////////////////////////////////////////////////

#include "SceneContext.h"

namespace steamrot {

/////////////////////////////////////////////////
SceneContext::SceneContext(SceneResources &scene_res, GameResources &game_res)
    : scene_entities(scene_res.scene_entities),
      archetypes(scene_res.archetype_manager.GetArchetypes()),
      scene_texture(scene_res.scene_texture),
      game_window(game_res.game_window),
      asset_manager(game_res.asset_manager),
      event_handler(game_res.event_handler),
      mouse_position(game_res.mouse_position) {}

} // namespace steamrot
