/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersDataLoader class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersDataLoader.h"
#include "Fragment.h"
#include "fragments_generated.h"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <iostream>
#include <utility>

namespace steamrot {
/////////////////////////////////////////////////
FlatbuffersDataLoader::FlatbuffersDataLoader(const EnvironmentType env_type)
    : DataLoader(env_type) {}

/////////////////////////////////////////////////
std::expected<Fragment, FailureData>
FlatbuffersDataLoader::ProvideFragment(const std::string &fragment_name) const {
  // check if the bin file exists
  std::filesystem::path fragment_path = m_path_provider.GetFragmentDirectory() /
                                        (fragment_name + ".fragment.bin");

  if (!std::filesystem::exists(fragment_path)) {

    return std::unexpected(
        std::make_pair(DataFailMode::FileNotFound, "file not found"));
  }

  const steamrot::FragmentData *fragment_data =
      GetFragmentData(LoadBinaryData(fragment_path));

  Fragment fragment;

  // check every possible field, not all flatbuffers data types are required
  // as this operation is not frequent we shall make it belts and braces
  if (!fragment_data->name())
    return std::unexpected(std::make_pair(DataFailMode::FlatbufferDataNotFound,
                                          "fragment name not found"));
  fragment.m_name = fragment_data->name()->str();

  // handle socket data
  if (!fragment_data->socket_data())
    return std::unexpected(std::make_pair(DataFailMode::FlatbufferDataNotFound,
                                          "fragment socket data not found"));

  // handle socket data vertices
  if (fragment_data->socket_data()->vertices()->size() == 0)
    return std::unexpected(
        std::make_pair(DataFailMode::FlatbufferDataNotFound,
                       "fragment socket data vertices not found"));

  for (const auto &vertex : *fragment_data->socket_data()->vertices()) {
    if (!vertex->x() || !vertex->y())
      return std::unexpected(
          std::make_pair(DataFailMode::FlatbufferDataNotFound,
                         "vertex from socket data is incomplete"));

    // add vector data to fragment sockets
    fragment.m_sockets.emplace_back(vertex->x(), vertex->y());
  }

  // handle render overlays
  if (fragment_data->render_overlay_data()->views()->empty())
    return std::unexpected(std::make_pair(DataFailMode::FlatbufferDataNotFound,
                                          "fragment render views not found"));

  // handle view triangles
  for (const auto &view : *fragment_data->render_overlay_data()->views()) {
    if (view->triangles()->empty()) {
      return std::unexpected(std::make_pair(
          DataFailMode::FlatbufferDataNotFound, "view triangles not found"));
    }
    // handle triangle vertices
    for (const auto &triangle : *view->triangles()) {
      if (triangle->vertices()->size() != 3) {
        return std::unexpected(
            std::make_pair(DataFailMode::FlatbufferDataNotFound,
                           "fragment triangles must have 3 vertices"));
      }
    }

    // handle view direction
    if (!view->direction()) {
      std::cout << "View direction not found for view: " << view->direction()
                << std::endl;
      return std::unexpected(std::make_pair(
          DataFailMode::FlatbufferDataNotFound, "view direction not found"));
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
std::expected<std::unordered_map<std::string, Fragment>, FailureData>
FlatbuffersDataLoader::ProvideAllFragments(
    std::vector<std::string> fragment_names) const {
  std::unordered_map<std::string, Fragment> fragments;

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
} // namespace steamrot
