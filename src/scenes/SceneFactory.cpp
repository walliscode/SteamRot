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
#include "ISceneConfigurator.h"
#include "Scene.h"
#include "TitleScene.h"
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
                                    "SceneType is UNKNOWN in SceneFactory"});
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
                                    "SceneType not handled in SceneFactory"});
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
SceneFactory::CreateSceneFromSceneLoadData(
    const SceneLoadData &scene_load_data) {

  // Step 1: Create empty scene
  auto create_scene_result =
      CreateEmptyScene(scene_load_data.scene_data.scene_info.type);
  if (!create_scene_result.has_value()) {
    return std::unexpected(create_scene_result.error());
  }
  std::unique_ptr<Scene> scene = std::move(create_scene_result.value());

  // Step 2: Get configurator
  ISceneConfigurator &configurator = GetSceneConfigurator();

  // Step 3: Configurator applies data
  auto config_result =
      configurator.ConfigureScene(*scene, scene_load_data.scene_data);
  if (!config_result.has_value()) {
    return std::unexpected(config_result.error());
  }

  // Step 4: Import entity data using the importer
  auto import_result = scene_load_data.entity_importer->ImportEntities(
      scene->GetSceneContext().scene_entities);
  if (!import_result)
    return std::unexpected(import_result.error());

  // Step 5: Return configured scene
  return std::move(scene);
}

/////////////////////////////////////////////////
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateSceneFromDefault(SceneType type) {

  // Step 1: Get provider and configurator
  auto get_provider_result =
      m_game_context.data_access_factory.GetSceneDataProvider();
  if (!get_provider_result.has_value())
    return std::unexpected(get_provider_result.error());
  ISceneLoadDataProvider &provider = *get_provider_result.value();

  // Step 2: Provider loads data
  auto get_data_result = provider.ProvideDefaultSceneLoadData(type);
  if (!get_data_result.has_value()) {
    return std::unexpected(get_data_result.error());
  }

  // Step 3: Create scene from data
  return CreateSceneFromSceneLoadData(get_data_result.value());
}

} // namespace steamrot
