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
#include "containers.h"

namespace steamrot {

/////////////////////////////////////////////////
Scene::Scene(const SceneType scene_type, const uuids::uuid &id,
             const GameContext &game_context)
    : m_scene_info{id, scene_type},
      m_scene_resources(game_context, game_context.event_handler) {

  // may need to intialize other members here in the future
}

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
std::expected<std::monostate, FailInfo>
Scene::ConfigureFromDefault(const DataType &data_type) {

  // configure the entity memory pool
  auto emp_configure_result =
      m_scene_resources.entity_manager.ConfigureEntitiesFromDefaultData(
          m_scene_info.type, data_type);
  if (!emp_configure_result)
    return std::unexpected(emp_configure_result.error());

  return std::monostate{};
}

/////////////////////////////////////////////////
const LogicCollection &Scene::GetLogicMap() const {
  return m_scene_resources.logic_map;
}

/////////////////////////////////////////////////
void Scene::SetLogicMap(LogicCollection logic_map) {
  // only set the logic map if it is empty
  if (m_scene_resources.logic_map.empty()) {
    m_scene_resources.logic_map = std::move(logic_map);
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
