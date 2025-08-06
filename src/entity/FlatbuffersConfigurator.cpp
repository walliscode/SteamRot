/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersConfigurator.h"
#include "scene_types_generated.h"
#include <expected>
#include <variant>

namespace steamrot {
/////////////////////////////////////////////////
FlatbuffersConfigurator::FlatbuffersConfigurator(const EnvironmentType env_type)
    : m_data_loader(env_type) {}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersConfigurator::ConfigureEntitiesFromDefaultData(
    EntityMemoryPool &entity_memory_pool, const SceneType scene_type) {

  // get the entity collection from the flatbuffers data loader
  const SceneData *scene_data =
      m_data_loader.ProvideSceneData(scene_type).value();

  // check the list of entities is not empty
  if (scene_data->entity_collection()->entities()->empty()) {

    FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                       "Entity data not found in the collection."};

    return std::unexpected(fail_info);
  }

  return std::monostate{};
};
} // namespace steamrot
