/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ISceneFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneFactory.h"
#include "CraftingScene.h"
#include "IEntityConfigurator.h"
#include "LogicFactory.h"
#include "Scene.h"
#include "TitleScene.h"
#include "uuid.h"
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <variant>

namespace steamrot {
/////////////////////////////////////////////////
SceneFactory::SceneFactory(const GameContext &game_context)

    : m_game_context(game_context) {}

/////////////////////////////////////////////////
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateEmptyScene(const SceneType scene_type) {

  // guard statement if scene_type is unknown
  if (scene_type == SceneType::UNKNOWN) {
    return std::unexpected(FailInfo{FailMode::EnumValueNotHandled,
                                    "SceneType is UNKNOWN in SceneFactory"});
  }

  std::unique_ptr<Scene> scene_ptr{nullptr};

  switch (scene_type) {

  case SceneType::TITLE: {
    auto title_ptr = new TitleScene(m_game_context);
    scene_ptr = std::unique_ptr<TitleScene>(title_ptr);
    return scene_ptr;
  }
  case SceneType::CRAFTING: {
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
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateSceneFromSceneData(const SceneData &scene_data) {

  // Step 1: Create empty scene
  auto create_scene_result = CreateEmptyScene(scene_data.scene_info.type);
  if (!create_scene_result.has_value()) {
    return std::unexpected(create_scene_result.error());
  }
  std::unique_ptr<Scene> scene = std::move(create_scene_result.value());

  // Step 2: Configure scene using scene data
  auto config_result = ConfigureScene(*scene, scene_data);
  if (!config_result.has_value()) {
    return std::unexpected(config_result.error());
  }

  // Step 3: Return configured scene
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
  ISceneDataProvider &provider = *get_provider_result.value();

  // Step 2: Provider loads data
  auto get_data_result = provider.CreateSceneData(type);
  if (!get_data_result.has_value()) {
    return std::unexpected(get_data_result.error());
  }

  // Step 3: Create scene from data
  return CreateSceneFromSceneData(get_data_result.value());
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SceneFactory::ConfigureScene(Scene &scene, const SceneData &scene_data) {

  // Configure SceneInfo
  auto info_result = ConfigureSceneInfo(scene, scene_data);
  if (!info_result.has_value())
    return std::unexpected(info_result.error());

  // Configure SceneResources
  auto resources_result = ConfigureSceneResources(scene, scene_data);
  if (!resources_result.has_value())
    return std::unexpected(resources_result.error());

  // Configure SceneConfig
  auto config_result = ConfigureSceneConfig(scene, scene_data);
  if (!config_result.has_value())
    return std::unexpected(config_result.error());

  // Import entities from entity importer
  auto import_result = ConfigureEntities(scene, scene_data);
  if (!import_result.has_value())
    return std::unexpected(import_result.error());

  // Configure LogicMap
  auto logic_result = ConfigureLogicMap(scene);
  if (!logic_result.has_value())
    return std::unexpected(logic_result.error());

  // Generate archetypes
  auto generate_result = scene.GetEntityManager().GenerateAllArchetypes();
  if (!generate_result.has_value())
    return std::unexpected(generate_result.error());

  return std::monostate();
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SceneFactory::ConfigureSceneInfo(Scene &scene, const SceneData &scene_data) {

  // if UUID is nil, generate one
  if (scene_data.scene_info.id.is_nil()) {
    scene.GetSceneInfo().id = uuids::uuid_system_generator{}();
  } else {
    scene.GetSceneInfo().id = scene_data.scene_info.id;
  }

  scene.GetSceneInfo().type = scene_data.scene_info.type;
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SceneFactory::ConfigureSceneResources(Scene &scene,
                                      const SceneData &scene_data) {

  // check texture sizes are not 0
  if (scene_data.scene_resources_config.texture_width == 0 ||
      scene_data.scene_resources_config.texture_height == 0) {
    return std::unexpected(
        FailInfo(FailMode::BadValue,
                 "SceneResourcesConfig has invalid texture dimensions (0)"));
  }

  // create size vector
  sf::Vector2u size{scene_data.scene_resources_config.texture_width,
                    scene_data.scene_resources_config.texture_height};

  auto texture_resize_result =
      scene.m_scene_resources.scene_texture.resize(size);
  if (!texture_resize_result)
    return std::unexpected(FailInfo(FailMode::ResourceCreationFailure,
                                    "Failed to resize scene render texture"));
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SceneFactory::ConfigureSceneConfig(Scene &scene, const SceneData &scene_data) {
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SceneFactory::ConfigureEntities(Scene &scene, const SceneData &scene_data) {

  // if entity transport is monostate, nothing to import so skip
  // this has been implemented for testing purposes. make become code smell, so
  // remove if not needed
  if (std::holds_alternative<std::monostate>(scene_data.entity_transport)) {
    return std::monostate();
  }

  // check if entity configurator is null
  if (!scene_data.entity_configurator) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer, "Entity configurator is null"});
  }

  // check if EntityConfigurator is not null
  if (scene_data.entity_configurator == nullptr) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer, "Entity configurator is null"});
  }
  // pass entity transport to entity configurator to set up entities
  auto configure_result =
      scene_data.entity_configurator->ConfigureEntityMemoryPoolFromSource(
          scene.GetEntityManager().GetEntityMemoryPool(),
          scene_data.entity_transport);
  if (!configure_result.has_value())
    return std::unexpected(configure_result.error());

  return std::monostate();
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SceneFactory::PassAssetConfig(Scene &scene, const SceneData &scene_data) {
  // get AssetManager reference
  AssetManager &asset_manager = scene.GetSceneContext().asset_manager;

  // pass AssetConfig to AssetManager
  auto asset_result =
      asset_manager.LoadAssetsFromConfig(scene_data.scene_asset_config);
  if (!asset_result.has_value())
    return std::unexpected(asset_result.error());

  return std::monostate();
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SceneFactory::ConfigureLogicMap(Scene &scene) {

  // create LogicFactory instance
  logic::LogicFactory logic_factory(scene.GetSceneContext());

  // create logic map and check info_result
  auto logic_map_result =
      logic_factory.ProvideLogicCollection(scene.m_scene_info.type);

  if (!logic_map_result.has_value())
    return std::unexpected(logic_map_result.error());

  // move logic map to scene
  scene.m_scene_resources.logic_map.clear();
  scene.m_scene_resources.logic_map = std::move(logic_map_result.value());

  return std::monostate();
}

} // namespace steamrot
