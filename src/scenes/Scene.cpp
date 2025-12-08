/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the abstract Scene class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Scene.h"
#include "EntityManager.h"
#include "SceneContext.h"
#include "containers.h"

namespace steamrot {

/////////////////////////////////////////////////
Scene::Scene(const GameContext &game_context)
    : m_scene_resources(game_context) {}

/////////////////////////////////////////////////
bool Scene::GetActive() const { return m_scene_state.active; }

/////////////////////////////////////////////////
void Scene::SetActive(bool active) { m_scene_state.active = active; }

/////////////////////////////////////////////////
#ifdef DEBUG
const EntityMemoryPool &Scene::GetEntityMemoryPool() const {
  return m_scene_resources.entity_manager.GetEntityMemoryPool();
}
#endif

/////////////////////////////////////////////////
sf::RenderTexture &Scene::GetRenderTexture() {
  return m_scene_resources.scene_texture;
}

/////////////////////////////////////////////////
const SceneInfo Scene::GetSceneInfo() const {
  SceneInfo scene_info;

  scene_info.id = m_scene_info.id;
  scene_info.type = m_scene_info.type;

  return scene_info;
}

/////////////////////////////////////////////////
SceneContext Scene::GetSceneContext() {

  SceneContext scene_context{m_scene_resources.scene_texture,
                             m_scene_resources.game_context.engine_resources,
                             m_scene_resources.entity_manager};

  return scene_context;
}
#ifdef DEBUG
/////////////////////////////////////////////////
const std::unordered_map<ArchetypeID, Archetype> &Scene::GetArchetypes() const {
  return m_scene_resources.entity_manager.GetArchetypeManager().GetArchetypes();
}
#endif
} // namespace steamrot
