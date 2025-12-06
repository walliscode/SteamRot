/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersSceneManagerDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneManagerDataProvider.h"

namespace steamrot {

/////////////////////////////////////////////////
std::expected<SceneManagerState, FailInfo>
FlatbuffersSceneManagerDataProvider::LoadSceneManagerState() const {

  // get SceneManagerDataFbs from loader
  auto sceneManagerDataFbsResult = m_loader.ProvideSceneManagerData();
  if (!sceneManagerDataFbsResult.has_value()) {
    return std::unexpected(sceneManagerDataFbsResult.error());
  }

  // extract SceneManagerStateFbs
  const SceneManagerDataFbs &sceneManagerDataFbs =
      *sceneManagerDataFbsResult.value();

  // create SceneManagerState object to configure
  SceneManagerState scene_manager_state;

  // configure subscribers

  return scene_manager_state;
}
} // namespace steamrot
