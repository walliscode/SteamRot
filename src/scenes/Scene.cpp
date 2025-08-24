/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the abstract Scene class.
/////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "Scene.h"
#include "EntityManager.h"
#include "scene_types_generated.h"

namespace steamrot {

////////////////////////////////////////////////////////////
Scene::Scene(const SceneType scene_type, const uuids::uuid &id,
             const GameContext game_context)
    : m_scene_info{id, scene_type}, m_entity_manager(),
      m_game_context(game_context) {}

/////////////////////////////////////////////////
sf::RenderTexture &Scene::GetRenderTexture() { return m_render_texture; }

////////////////////////////////////////////////////////////
bool Scene::GetActive() const { return m_active; }

////////////////////////////////////////////////////////////
void Scene::SetActive(bool active) { m_active = active; }

/////////////////////////////////////////////////
#ifdef DEBUG
const EntityMemoryPool &Scene::GetEntityMemoryPool() const {
  return m_entity_manager.GetEntityMemoryPool();
}
#endif

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
Scene::ConfigureFromDefault(const DataType &data_type) {

  // configure the entity memory pool
  auto emp_configure_result = m_entity_manager.ConfigureEntitiesFromDefaultData(
      m_scene_info.type, m_game_context.env_type, data_type);
  if (!emp_configure_result)
    return std::unexpected(emp_configure_result.error());

  return std::monostate{};
}

/////////////////////////////////////////////////
const SceneInfo &Scene::GetSceneInfo() const { return m_scene_info; }
} // namespace steamrot
