/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the Fragment struct. No impplementation is needed
/////////////////////////////////////////////////

#pragma once

#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/VertexArray.hpp>

struct Fragment {
  /////////////////////////////////////////////////
  /// @brief Local positions of the Fragmens's sockets
  /////////////////////////////////////////////////
  const sf::ConvexShape m_sockets;

  sf::Transform m_transform;
  /////////////////////////////////////////////////
  /// @brief Designed to be draw over a skeletal mesh so it is separate from
  /// calculations
  /////////////////////////////////////////////////
  sf::VertexArray m_render_overlay;

  /////////////////////////////////////////////////
  /// @brief Maps render vertices to sockets
  ///
  /// the position in the vector is the index of the vertex in the render
  /// overlay and the value is the index of the socket in the m_sockets
  /// ConvexShape.
  /////////////////////////////////////////////////
  std::vector<uint8_t> m_render_vertex_to_socket_map;
};
