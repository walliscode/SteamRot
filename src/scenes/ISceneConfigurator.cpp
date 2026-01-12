/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ISceneConfigurator interface.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ISceneConfigurator.h"
#include "Scene.h"
namespace steamrot {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ISceneConfigurator::ConfigureScene(Scene &scene, const SceneData &scene_data) {

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
  auto import_result = ImportEntities(scene, scene_data);
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
ISceneConfigurator::ImportEntities(Scene &scene, const SceneData &scene_data) {

  // check if entity importer variant holds type we want
  if (!std::holds_alternative<std::unique_ptr<IEntityImporter>>(
          scene_data.entity_transport)) {
    return std::unexpected(
        FailInfo{FailMode::VariantTypeMismatch,
                 "Entity importer variant does not hold IEntityImporter type"});
  }
  // Check if entity importer exists
  if (std::get<std::unique_ptr<IEntityImporter>>(scene_data.entity_transport) ==
      nullptr) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer, "Entity importer pointer is null"});
  }

  // Import entities using the importer
  auto &importer =
      std::get<std::unique_ptr<IEntityImporter>>(scene_data.entity_transport);
  auto import_result =
      importer->ImportEntities(scene.GetEntityManager().GetEntityMemoryPool());
  if (!import_result.has_value())
    return std::unexpected(import_result.error());

  return std::monostate();
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ISceneConfigurator::PassAssetConfig(Scene &scene, const SceneData &scene_data) {
  // get AssetManager reference
  AssetManager &asset_manager = scene.GetSceneContext().asset_manager;

  // pass AssetConfig to AssetManager
  auto asset_result = asset_manager.LoadAssets(scene_data.scene_asset_config);
  if (!asset_result.has_value())
    return std::unexpected(asset_result.error());

  return std::monostate();
}
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ISceneConfigurator::ConfigureLogicMap(Scene &scene) {

  // create LogicFactory instance
  LogicFactory logic_factory(scene.GetSceneContext());

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
