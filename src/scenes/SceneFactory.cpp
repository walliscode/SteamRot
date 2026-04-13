/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ISceneFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneFactory.h"
#include "CUserInterface.h"
#include "CraftingScene.h"
#include "IEntityConfigurator.h"
#include "LogicFactory.h"
#include "Scene.h"
#include "TitleScene.h"
#include "UIExplorerScene.h"
#include "entity_memory.h"
#include "uuid.h"
#include <SFML/System/Vector2.hpp>
#include <format>
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
  case SceneType::UI_EXPLORER: {
    auto explorer_ptr = new UIExplorerScene(m_game_context);
    scene_ptr = std::unique_ptr<UIExplorerScene>(explorer_ptr);
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

  // Validate that all CUserInterface style names exist in the AssetManager
  auto validate_result = ValidateUIStyles(scene);
  if (!validate_result.has_value())
    return std::unexpected(validate_result.error());

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

/////////////////////////////////////////////////
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateUIExplorerScene() {

  // Step 1: Create the scene instance directly (no data file required)
  auto explorer_ptr = new UIExplorerScene(m_game_context);
  std::unique_ptr<Scene> scene = std::unique_ptr<UIExplorerScene>(explorer_ptr);

  // Step 2: Assign a new UUID and set the scene type
  scene->GetSceneInfo().id = uuids::uuid_system_generator{}();
  scene->GetSceneInfo().type = SceneType::UI_EXPLORER;

  // Step 3: Create a 1280×720 render texture
  static constexpr uint32_t kWidth = 1280u;
  static constexpr uint32_t kHeight = 720u;
  if (!scene->m_scene_resources.scene_texture.resize(
          sf::Vector2u{kWidth, kHeight})) {
    return std::unexpected(
        FailInfo{FailMode::ResourceCreationFailure,
                 "UIExplorerScene: failed to create 1280x720 render texture"});
  }

  // Step 4: Configure an empty logic collection
  // UIExplorerScene overrides sRender/sCollision/sAction directly.
  scene->m_scene_resources.logic_map.clear();
  scene->m_scene_resources.logic_map.emplace(LogicGrouping::Collision,
                                             logic::LogicVector{});
  scene->m_scene_resources.logic_map.emplace(LogicGrouping::Action,
                                             logic::LogicVector{});
  scene->m_scene_resources.logic_map.emplace(LogicGrouping::Render,
                                             logic::LogicVector{});
  scene->m_scene_resources.logic_map.emplace(LogicGrouping::Movement,
                                             logic::LogicVector{});

  return scene;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SceneFactory::ValidateUIStyles(Scene &scene) const {
  const EntityMemoryPool &emp =
      scene.GetEntityManager().GetEntityMemoryPool();
  const std::unordered_map<std::string, UIStyle> &styles =
      m_game_context.asset_manager.GetAllUIStyles();
  const size_t pool_size = entity::memory::GetMemoryPoolSize(emp);

  for (size_t i = 0; i < pool_size; ++i) {
    const CUserInterface &ui_component =
        entity::memory::GetComponent<CUserInterface>(i, emp);

    if (!ui_component.m_active)
      continue;

    if (!styles.contains(ui_component.m_style_name)) {
      return std::unexpected(FailInfo{
          FailMode::FlatbuffersDataNotFound,
          std::format("UIStyle '{}' referenced by UI entity {} not found in "
                      "AssetManager. Available styles must be declared in the "
                      "asset config.",
                      ui_component.m_style_name, i)});
    }
  }

  return std::monostate{};
}

} // namespace steamrot
