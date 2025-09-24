/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the EntityManager class
/////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "EntityManager.h"
#include "EventHandler.h"
#include "FailInfo.h"
#include "FlatbuffersConfigurator.h"
#include "PathProvider.h"
#include "emp_helpers.h"
#include <expected>
#include <variant>

namespace steamrot {

////////////////////////////////////////////////////////////
EntityManager::EntityManager(EventHandler &event_handler)
    : m_archetype_manager(m_entity_memory_pool),
      m_event_handler(event_handler) {}

/////////////////////////////////////////////////
EntityManager::EntityManager(const size_t pool_size,
                             EventHandler &event_handler)
    : m_archetype_manager(m_entity_memory_pool),
      m_event_handler(event_handler) {
  // resize the entity memory pool to the given size
  ResizeEntityMemoryPool(pool_size);
}
/////////////////////////////////////////////////
EntityMemoryPool &EntityManager::GetEntityMemoryPool() {
  // return a reference to the memory pool
  return m_entity_memory_pool;
}

/////////////////////////////////////////////////
const EntityMemoryPool &EntityManager::GetEntityMemoryPool() const {
  // return a const reference to the memory pool
  return m_entity_memory_pool;
}

/////////////////////////////////////////////////
const ArchetypeManager &EntityManager::GetArchetypeManager() const {
  // return a const reference to the archetype manager
  return m_archetype_manager;
}

////////////////////////////////////////////////////////////
void EntityManager::ResizeEntityMemoryPool(const size_t pool_size) {

  // use std::apply to resize the memory pool with lambda function
  std::apply(
      [pool_size](auto &...component_vector) {
        (component_vector.resize(pool_size), ...);
      },
      m_entity_memory_pool);
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
EntityManager::ConfigureEntitiesFromDefaultData(
    const SceneType scene_type, const DataType data_type,
    const EnvironmentType env_type) {

  switch (data_type) {
  case DataType::Flatbuffers: {

    FlatbuffersConfigurator configurator{m_event_handler};
    auto configure_result = configurator.ConfigureEntitiesFromDefaultData(
        m_entity_memory_pool, scene_type);
    if (!configure_result.has_value())
      return std::unexpected<FailInfo>(configure_result.error());

    break;
  }
  default:
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue, "Invalid enum value"});
  }

  return std::monostate();
}
////////////////////////////////////////////////////////////
size_t EntityManager::GetNextFreeEntityIndex() {

  // get CMeta vector from the pool
  std::vector<CMeta> meta_data =
      emp_helpers::GetComponentVector<CMeta>(m_entity_memory_pool);

  // find next inactive entity index
  for (size_t i = 0; i < meta_data.size(); ++i) {
    if (!meta_data[i].m_entity_active) {
      return i;
    }
  }
  // if no inactive entity found, return the size of the vector
  return meta_data.size();
};

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> EntityManager::GenerateAllArchetypes() {
  auto generate_result = m_archetype_manager.GenerateAllArchetypes();
  if (!generate_result.has_value()) {
    return std::unexpected(generate_result.error());
  }
  return std::monostate{};
}

} // namespace steamrot
