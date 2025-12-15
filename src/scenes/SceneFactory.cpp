/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ISceneFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneFactory.h"
#include "CraftingScene.h"
#include "FlatbuffersSceneConfigurator.h"
#include "ISceneDataProvider.h"
#include "TitleScene.h"
#include "provider_factory.h"
#include <memory>

namespace steamrot {
/////////////////////////////////////////////////
SceneFactory::SceneFactory(const GameContext &game_context)

    : m_game_context(game_context) {}

/////////////////////////////////////////////////
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateEmptyScene(const SceneType scene_type) {
  // guard statement if scene_type is unknown
  if (scene_type == SceneType::SceneType_UNKNOWN) {
    return std::unexpected(FailInfo{FailMode::EnumValueNotHandled,
                                    "SceneType is UNKNOWN in ISceneFactory"});
  }

  std::unique_ptr<Scene> scene_ptr{nullptr};

  switch (scene_type) {

  case SceneType::SceneType_TITLE: {
    auto title_ptr = new TitleScene(m_game_context);
    scene_ptr = std::unique_ptr<TitleScene>(title_ptr);
    return scene_ptr;
  }
  case SceneType::SceneType_CRAFTING: {
    auto crafting_ptr = new CraftingScene(m_game_context);
    scene_ptr = std::unique_ptr<CraftingScene>(crafting_ptr);
    return scene_ptr;
  }
  default: {
    return std::unexpected(FailInfo{FailMode::EnumValueNotHandled,
                                    "SceneType not handled in ISceneFactory"});
  }
  }
}

/////////////////////////////////////////////////
ISceneConfigurator &GetSceneConfigurator() {
  static FlatbuffersSceneConfigurator configurator;
  return configurator;
}

/////////////////////////////////////////////////
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateSceneFromDefault(SceneType type) {

  // Step 1: Get provider and configurator
  ISceneDataProvider &provider = GetSceneDataProvider();
  ISceneConfigurator &configurator = GetSceneConfigurator();

  // Step 2: Provider loads data
  std::unique_ptr<SceneData> data = provider.ProvideDefaultSceneData(type);
  if (!data) {
    // Log error
    return nullptr;
  }

  // Step 3: Create empty scene
  std::unique_ptr<Scene> scene = CreateEmptyScene(type).value();

  // Step 4: Configurator applies data
  auto config_result = configurator.ConfigureScene(*scene, data.get());
  if (!config_result.has_value()) {
    // Log error
    return nullptr;
  }

  return std::move(scene);
}

} // namespace steamrot
