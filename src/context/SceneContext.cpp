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
                           EngineResources &engine_resources,
                           EntityManager &entity_manager,
                           DataAccessFactory &data_access_factory,
                           MrGhost &mr_ghost)
    : scene_entities(entity_manager.GetEntityMemoryPool()),
      archetypes(entity_manager.GetArchetypeManager().GetArchetypes()),
      scene_texture(scene_texture), game_window(engine_resources.game_window),
      asset_manager(engine_resources.asset_manager),
      event_handler(engine_resources.event_handler),
      mouse_position(engine_resources.mouse_position),
      data_access_factory(data_access_factory), mr_ghost(mr_ghost) {}

} // namespace steamrot
