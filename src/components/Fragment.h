/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the Fragment struct. No impplementation is needed
/////////////////////////////////////////////////

#pragma once

#include "fragments_generated.h"
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace steamrot {
/////////////////////////////////////////////////
/// @class Fragment
/// @brief Contains the data for a Fragment in the Grimoire Machina.
///
/////////////////////////////////////////////////
struct Fragment {
  /////////////////////////////////////////////////
  /// @brief Local positions of the Fragment's sockets
  /////////////////////////////////////////////////
  sf::VertexArray m_sockets;

  /////////////////////////////////////////////////
  /// @brief Global transform of the fragment
  /////////////////////////////////////////////////
  sf::Transform m_transform;

  /////////////////////////////////////////////////
  /// @brief Contains all the render overlays for this fragment, describing each
  /// possible view.
  /////////////////////////////////////////////////
  std::unordered_map<ViewDirection, sf::VertexArray> m_overlays;

  /////////////////////////////////////////////////
  /// @brief Maps render vertices to sockets
  ///
  /// the position in the vector is the index of the vertex in the render
  /// overlay and the value is the index of the socket in the m_sockets
  /// ConvexShape.
  /////////////////////////////////////////////////
  std::vector<uint8_t> m_render_vertex_to_socket_map;
};
} // namespace steamrot
