/////////////////////////////////////////////////
/// @file
/// @brief Declaration of FlatbuffersSceneFactory
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ISceneFactory.h"
#include "scene_data_generated.h"

namespace steamrot {
class FlatbuffersSceneFactory : public ISceneFactory {

private:
  const SceneDataFbs *m_scene_data_fbs;

public:
  FlatbuffersSceneFactory(const GameContext &game_context,
                          const SceneDataFbs *scene_data_fbs);

  std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene &scene) override;

  std::expected<std::monostate, FailInfo>
  ConfigureSceneConfig(Scene &scene) override;
};
} // namespace steamrot
