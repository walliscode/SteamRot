/////////////////////////////////////////////////
/// @file
/// @brief Implements a scene configurator that uses default settings for
/// Flatbuffers scenes.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneConfigurator.h"
#include "FailInfo.h"
#include "Scene.h"
#include "uuid.h"
#include <SFML/System/Vector2.hpp>
#include <expected>
#include <variant>

namespace steamrot {
/////////////////////////////////////////////////

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSceneConfigurator::ConfigureSceneInfo(Scene &scene,
                                                 const SceneData &scene_data) {

  // i tried to add valid uuid checking here, but as it is type uuid, it HAS to
  // be valid. Error checking should happen at data provider not configurator

  // if UUID is nil, generate one
  if (scene_data.scene_info.id.is_nil()) {
    scene.GetSceneInfo().id = uuids::uuid_system_generator{}();
  }

  scene.GetSceneInfo().type = scene_data.scene_info.type;
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSceneConfigurator::ConfigureSceneResources(
    Scene &scene, const SceneData &scene_data) {

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
FlatbuffersSceneConfigurator::ConfigureSceneConfig(
    Scene &scene, const SceneData &scene_data) {

  return std::monostate{};
}

} // namespace steamrot
