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
SceneContext::SceneContext(sf::RenderTexture &scene_texture,
                           SceneState &scene_state,
                           EngineResources &engine_resources,
                           EntityManager &entity_manager,
                           DataAccessFactory &data_access_factory,
                           MrGhost &mr_ghost, CameraState &camera_state,
                           sf::Vector2f &world_mouse_position)
    : scene_entities(entity_manager.GetEntityMemoryPool()),
      scene_state(scene_state),
      archetypes(entity_manager.GetArchetypeManager().GetArchetypes()),
      scene_texture(scene_texture), game_window(engine_resources.game_window),
      asset_manager(engine_resources.asset_manager),
      event_handler(engine_resources.event_handler),
      mouse_position(engine_resources.mouse_position),
      data_access_factory(data_access_factory), mr_ghost(mr_ghost),
      camera_state(camera_state), world_mouse_position(world_mouse_position) {}

} // namespace steamrot
