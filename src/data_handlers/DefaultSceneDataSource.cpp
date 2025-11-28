/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the DefaultSceneDataSource class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DefaultSceneDataSource.h"
#include "magic_enum.hpp"

namespace steamrot {

/////////////////////////////////////////////////
DefaultSceneDataSource::DefaultSceneDataSource(SceneType scene_type)
    : m_scene_type(scene_type) {}

/////////////////////////////////////////////////
void DefaultSceneDataSource::EnsureDataLoaded() const {
  if (m_data_loaded) {
    return;
  }

  auto result = m_loader.ProvideSceneData(m_scene_type);
  if (result.has_value()) {
    m_scene_data = result.value();
  }

  m_data_loaded = true;
}

/////////////////////////////////////////////////
const EntityCollection *DefaultSceneDataSource::GetEntityCollection() const {
  EnsureDataLoaded();

  if (m_scene_data && m_scene_data->entity_collection()) {
    return m_scene_data->entity_collection();
  }

  return nullptr;
}

/////////////////////////////////////////////////
size_t DefaultSceneDataSource::GetEntityCount() const {
  const EntityCollection *collection = GetEntityCollection();

  if (collection && collection->entities()) {
    return collection->entities()->size();
  }

  return 0;
}

/////////////////////////////////////////////////
std::string DefaultSceneDataSource::GetSourceIdentifier() const {
  return std::string("DefaultSceneDataSource(") +
         std::string(magic_enum::enum_name(m_scene_type)) + ")";
}

/////////////////////////////////////////////////
SceneType DefaultSceneDataSource::GetSceneType() const { return m_scene_type; }

} // namespace steamrot
