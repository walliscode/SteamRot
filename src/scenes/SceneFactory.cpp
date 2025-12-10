/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ISceneFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneFactory.h"
#include "CraftingScene.h"
#include "TitleScene.h"
#include <memory>

namespace steamrot {

/////////////////////////////////////////////////
SceneFactory::SceneFactory(const GameContext &game_context,
                           ISceneConfigurator &scene_configurator)

    : m_game_context(game_context), m_scene_configurator(scene_configurator) {}

/////////////////////////////////////////////////
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateAndConfigureScene(const SceneType scene_type) {

  // Create a unique pointer to a Scene object
  std::unique_ptr<Scene> scene_ptr{nullptr};

  // Create Scene by type
  if (auto scene_ptr_result = CreateSceneByType(scene_type);
      !scene_ptr_result) {
    return std::unexpected(scene_ptr_result.error());
  } else {
    // set the scene_ptr to the created Scene
    scene_ptr = std::move(scene_ptr_result.value());
  }

  // Configure the Scene using the configurator
  if (auto config_result = m_scene_configurator.ConfigureScene(*scene_ptr);
      !config_result) {
    return std::unexpected(config_result.error());
  }

  return scene_ptr;
}

/////////////////////////////////////////////////
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateSceneByType(const SceneType scene_type) {
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

} // namespace steamrot
