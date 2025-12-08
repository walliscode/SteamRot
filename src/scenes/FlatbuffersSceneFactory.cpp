/////////////////////////////////////////////////
/// @file
/// @brief Implements a scene factory for Flatbuffers scenes.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneFactory.h"
#include "FlatbuffersEntityConfigurator.h"

namespace steamrot {

/////////////////////////////////////////////////
FlatbuffersSceneFactory::FlatbuffersSceneFactory(
    const GameContext &game_context, const SceneDataFbs *scene_data_fbs)
    : ISceneFactory(game_context), m_scene_data_fbs(scene_data_fbs) {

  m_entity_configurator = std::make_unique<FlatbuffersEntityConfigurator>(
      game_context.event_handler, *scene_data_fbs->entity_collection());

  // [TODO: Implement a FlatbuffersSceneDataProvider and set it here]
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSceneFactory::ConfigureSceneResources(Scene &scene) {
  // guard statement for null scene_data_fbs
  if (!m_scene_data_fbs) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer,
                 "SceneDataFbs is null in "
                 "FlatbuffersSceneFactory::ConfigureSceneResources"});
  }

  // configure scene texture

  // configure the Scenes EntityMemoryPool
  auto configure_emp_result = m_entity_configurator->ConfigureEntityMemoryPool(
      scene.GetEntityManager().GetEntityMemoryPool());
  if (!configure_emp_result) {
    return std::unexpected(configure_emp_result.error());
  }
  return std::monostate{};
}
} // namespace steamrot
