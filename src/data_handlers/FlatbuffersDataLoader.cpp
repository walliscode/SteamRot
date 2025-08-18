/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersDataLoader class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersDataLoader.h"
#include "FailInfo.h"
#include "Fragment.h"
#include "fragments_generated.h"
#include "scene_types_generated.h"
#include "scenes_generated.h"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <expected>

namespace steamrot {
/////////////////////////////////////////////////
FlatbuffersDataLoader::FlatbuffersDataLoader(const EnvironmentType env_type)
    : DataLoader(env_type) {}

/////////////////////////////////////////////////
std::expected<Fragment, FailInfo>
FlatbuffersDataLoader::ProvideFragment(const std::string &fragment_name) const {
  // check if the bin file exists
  std::filesystem::path fragment_path =
      m_path_provider.GetFragmentDirectory().value() /
      (fragment_name + ".fragment.bin");

  if (!std::filesystem::exists(fragment_path)) {
    FailInfo fail_info(
        FailMode::FlatbuffersDataNotFound,
        std::format("Fragment file not found: {}", fragment_path.string()));
    return std::unexpected(fail_info);
  }

  const steamrot::FragmentData *fragment_data =
      GetFragmentData(LoadBinaryData(fragment_path));

  Fragment fragment;

  // check every possible field, not all flatbuffers data types are required
  // as this operation is not frequent we shall make it belts and braces
  if (!fragment_data->name()) {
    FailInfo fail_info(FailMode::FlatbuffersDataNotFound,
                       "Fragment name not found in fragment data");
    return std::unexpected(fail_info);
  }

  fragment.m_name = fragment_data->name()->str();

  // handle socket data
  if (!fragment_data->socket_data())
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "fragment socket data not found"));

  // handle socket data vertices
  if (fragment_data->socket_data()->vertices()->size() == 0)
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "fragment socket data vertices not found"));

  for (const auto &vertex : *fragment_data->socket_data()->vertices()) {
    if (!vertex->x() || !vertex->y())
      return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                      "vertex from socket data is incomplete"));

    // add vector data to fragment sockets
    fragment.m_sockets.emplace_back(vertex->x(), vertex->y());
  }

  // handle render overlays
  if (fragment_data->render_overlay_data()->views()->empty())
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "fragment render views not found"));

  // handle view triangles
  for (const auto &view : *fragment_data->render_overlay_data()->views()) {
    if (view->triangles()->empty()) {
      return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                      "view triangles not found"));
    }
    // handle triangle vertices
    for (const auto &triangle : *view->triangles()) {
      if (triangle->vertices()->size() != 3) {
        return std::unexpected(
            FailInfo(FailMode::FlatbuffersDataNotFound,
                     "fragment triangles must have 3 vertices"));
      }
    }

    // handle view direction
    if (!view->direction()) {
      return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                      "view direction not found"));
    }

    // add view to fragment
    sf::VertexArray view_to_add(sf::PrimitiveType::Triangles);
    for (const auto &triangle : *view->triangles()) {
      for (const auto &vertex : *triangle->vertices()) {
        // create a vertex with position and color
        sf::Vertex vertex_to_add(
            sf::Vector2f(vertex->position()->x(), vertex->position()->y()),
            sf::Color(vertex->color()->r(), vertex->color()->g(),
                      vertex->color()->b(), vertex->color()->a()));
        view_to_add.append(vertex_to_add);
      }
    }

    // add to m_overlays
    fragment.m_overlays[view->direction()] = view_to_add;
  }

  return fragment;
}

/////////////////////////////////////////////////
std::expected<std::map<std::string, Fragment>, FailInfo>
FlatbuffersDataLoader::ProvideAllFragments(
    std::vector<std::string> fragment_names) const {

  std::map<std::string, Fragment> fragments;

  for (const auto &fragment_name : fragment_names) {
    auto fragment_result = ProvideFragment(fragment_name);
    // pass up any errors
    if (!fragment_result.has_value()) {
      return std::unexpected(fragment_result.error());
    }
    fragments[fragment_name] = fragment_result.value();
  }
  return fragments;
}

/////////////////////////////////////////////////
std::expected<const SceneData *, FailInfo>
FlatbuffersDataLoader::ProvideSceneData(const SceneType scene_type) const {

  // get file prefix from scene type
  std::string scene_file_prefix;
  switch (scene_type) {
  case SceneType::SceneType_UNKNOWN: {
    scene_file_prefix = "unknown";
    break;
  }
  case SceneType::SceneType_TEST: {
    scene_file_prefix = "test";
    break;
  }
  case SceneType::SceneType_TITLE: {
    scene_file_prefix = "title";
    break;
  }
  case SceneType::SceneType_CRAFTING: {
    scene_file_prefix = "crafting";
    break;
  }
  default:
    return std::unexpected(
        FailInfo(FailMode::SceneTypeNotFound, "Invalid SceneType provided"));
  }

  // check the SceneDirectory is error free
  auto scene_dir_result = m_path_provider.GetSceneDirectory();
  if (!scene_dir_result.has_value()) {
    return std::unexpected(scene_dir_result.error());
  }
  // construct the file path
  std::filesystem::path scene_path =
      scene_dir_result.value() / (scene_file_prefix + ".scenes.bin");

  // check if the file exists
  if (!std::filesystem::exists(scene_path)) {
    std::string error_message =
        std::format("Scene file not found: {}", scene_file_prefix);
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }

  // load the scene data
  const steamrot::SceneData *scene_data =
      GetSceneData(LoadBinaryData(scene_path));

  return scene_data;
}

/////////////////////////////////////////////////
std::expected<const AssetCollection *, FailInfo>
FlatbuffersDataLoader::ProvideAssetData(const SceneType scene_type) const {
  // get scene data
  auto scene_data_result = ProvideSceneData(scene_type);
  if (!scene_data_result.has_value()) {
    return std::unexpected(scene_data_result.error());
  }

  const SceneData *scene_data = scene_data_result.value();
  if (!scene_data) {
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "SceneData pointer is null"));
  }

  if (!scene_data->assets()) {
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "Asset collection not found"));
  }
  return scene_data->assets();
}
} // namespace steamrot
