/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the abstract Scene class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Scene.h"
#include "EntityManager.h"
#include "LogicFactory.h"
#include "SceneContext.h"
#include "SceneResources.h"
#include "containers.h"
#include "scene_change_packet_generated.h"

namespace steamrot {

/////////////////////////////////////////////////
Scene::Scene(const SceneType scene_type, const uuids::uuid &id,
             const GameContext &game_context)
    : m_scene_info{id, scene_type},
      m_entity_manager(game_context.event_handler),
      m_game_context(game_context) {}

/////////////////////////////////////////////////
bool Scene::GetActive() const { return m_active; }

/////////////////////////////////////////////////
void Scene::SetActive(bool active) { m_active = active; }

/////////////////////////////////////////////////
#ifdef DEBUG
const EntityMemoryPool &Scene::GetEntityMemoryPool() const {
  return m_entity_manager.GetEntityMemoryPool();
}
#endif

/////////////////////////////////////////////////
sf::RenderTexture &Scene::GetRenderTexture() {
  return m_scene_resources.scene_texture;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
Scene::ConfigureFromDefault(const DataType &data_type) {

  // configure the entity memory pool
  auto emp_configure_result = m_entity_manager.ConfigureEntitiesFromDefaultData(
      m_scene_info.type, data_type);
  if (!emp_configure_result)
    return std::unexpected(emp_configure_result.error());

  return std::monostate{};
}

/////////////////////////////////////////////////
const LogicCollection &Scene::GetLogicMap() const { return m_logic_map; }

/////////////////////////////////////////////////
void Scene::SetLogicMap(
    std::unordered_map<LogicType, std::vector<std::unique_ptr<Logic>>>
        logic_map) {
  // only set the logic map if it is empty
  if (m_logic_map.empty()) {
    m_logic_map = std::move(logic_map);
  }
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

  SceneContext scene_context{m_scene_resources, m_game_context.game_resources,
                             m_entity_manager};

  return scene_context;
}
#ifdef DEBUG
/////////////////////////////////////////////////
const std::unordered_map<ArchetypeID, Archetype> &Scene::GetArchetypes() const {
  return m_entity_manager.GetArchetypeManager().GetArchetypes();
}
#endif
} // namespace steamrot
