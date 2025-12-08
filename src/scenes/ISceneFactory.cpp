/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ISceneFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ISceneFactory.h"
#include "CraftingScene.h"
#include "TitleScene.h"
#include <memory>

namespace steamrot {

/////////////////////////////////////////////////
ISceneFactory::ISceneFactory(const GameContext &game_context)
    : m_game_context(game_context) {};

/////////////////////////////////////////////////
std::expected<std::unique_ptr<Scene>, FailInfo> ISceneFactory::CreateScene() {
  // guard statements
  if (!m_scene_data_provider) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer,
                 "ISceneDataProvider is not set in ISceneFactory"});
  }
  if (!m_entity_configurator) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer,
                 "IEntityConfigurator is not set in ISceneFactory"});
  }

  // Create a unique pointer to a Scene object
  std::unique_ptr<Scene> scene_ptr{nullptr};

  // Create Scene by type
  if (auto scene_ptr_result = CreateSceneByType(); !scene_ptr_result) {
    return std::unexpected(scene_ptr_result.error());
  } else {
    // set the scene_ptr to the created Scene
    scene_ptr = std::move(scene_ptr_result.value());
  }

  // Provide and set SceneInfo
  if (auto scene_info_result = m_scene_data_provider->LoadSceneInfo();
      !scene_info_result) {
    return std::unexpected(scene_info_result.error());
  } else {
    // set the scene_info member of the Scene
    scene_ptr->m_scene_info = scene_info_result.value();
  }

  // Configures SceneResources
  if (auto result = ConfigureSceneResources(*scene_ptr); !result) {
    return std::unexpected(result.error());
  }

  return scene_ptr;
}

/////////////////////////////////////////////////
std::expected<std::unique_ptr<Scene>, FailInfo>
ISceneFactory::CreateSceneByType() {
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
