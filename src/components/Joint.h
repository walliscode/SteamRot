/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the Joint struct. No implementation is needed
/////////////////////////////////////////////////

#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace steamrot {

using FragmentIndex = uint8_t;
using SocketIndex = uint8_t;

struct Joint {
  std::string m_joint_name{"base"};
  /////////////////////////////////////////////////
  /// @brief The number of connections this joint can maintain
  /////////////////////////////////////////////////
  uint8_t m_number_of_connections;

  /////////////////////////////////////////////////
  /// @brief The posiiton of the middle of the joint in the RenderTexture
  /////////////////////////////////////////////////
  sf::Vector2f m_global_position;

  /////////////////////////////////////////////////
  /// @brief Connects joint to fragments
  ///
  /// The index is the connection point in the joint and the value is the index
  /////////////////////////////////////////////////
  std::vector<std::pair<FragmentIndex, SocketIndex>> m_connected_fragments;

  /////////////////////////////////////////////////
  /// @brief Constructed vertex array for the visual solution of the joint
  /////////////////////////////////////////////////
  sf::VertexArray m_render_overlay;

  sf::Transform m_transform;
};
} // namespace steamrot
