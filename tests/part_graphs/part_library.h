/////////////////////////////////////////////////
/// @file
/// @brief Header-only catalog of pre-defined Fragment and Joint definitions
///        for test infrastructure.
///
/// Each part is exposed as an @c inline @c const variable in the
/// @c steamrot::tests namespace. They can be referenced directly in test
/// builders and assertions without any factory call.
///
/// Fragments and Joints are read-only definitions; all mutable state lives in
/// FragmentInstance and JointInstance wrappers. The @c const pointers stored
/// on those instance types enforce this contract at compile time.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Fragment.h"
#include "Joint.h"
#include "ViewDirection.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/VertexArray.hpp>

/////////////////////////////////////////////////
/// Internal helpers — not part of the public API
/////////////////////////////////////////////////
namespace steamrot::tests::detail {

/////////////////////////////////////////////////
/// @brief Build a filled 20×20 square VertexArray (Triangles primitive).
///
/// @param x     Left edge x-coordinate.
/// @param y     Top edge y-coordinate.
/// @param color Fill color.
/// @return VertexArray with 6 vertices forming two triangles.
/////////////////////////////////////////////////
inline sf::VertexArray MakeFilledSquare(float x, float y, sf::Color color) {
  sf::VertexArray va(sf::PrimitiveType::Triangles, 6);
  va[0] = sf::Vertex{{x, y}, color};
  va[1] = sf::Vertex{{x + 20.f, y}, color};
  va[2] = sf::Vertex{{x + 20.f, y + 20.f}, color};
  va[3] = sf::Vertex{{x, y}, color};
  va[4] = sf::Vertex{{x + 20.f, y + 20.f}, color};
  va[5] = sf::Vertex{{x, y + 20.f}, color};
  return va;
}

/////////////////////////////////////////////////
/// @brief Build a green origin-aligned triangle VertexArray.
///
/// Vertices at (0, 0), (20, 0), (10, 20).
/////////////////////////////////////////////////
inline sf::VertexArray MakeGreenOriginTriangle() {
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{0.f, 0.f}, sf::Color::Green});
  va.append(sf::Vertex{sf::Vector2f{20.f, 0.f}, sf::Color::Green});
  va.append(sf::Vertex{sf::Vector2f{10.f, 20.f}, sf::Color::Green});
  return va;
}

/////////////////////////////////////////////////
/// @brief Build a blue origin-aligned triangle VertexArray.
///
/// Vertices at (0, 0), (20, 0), (10, 20).
/////////////////////////////////////////////////
inline sf::VertexArray MakeBlueOriginTriangle() {
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{0.f, 0.f}, sf::Color::Blue});
  va.append(sf::Vertex{sf::Vector2f{20.f, 0.f}, sf::Color::Blue});
  va.append(sf::Vertex{sf::Vector2f{10.f, 20.f}, sf::Color::Blue});
  return va;
}

} // namespace steamrot::tests::detail

/////////////////////////////////////////////////
/// Part catalog
/////////////////////////////////////////////////
namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief fragment_no_socket: green origin triangle, no sockets.
/////////////////////////////////////////////////
inline const Fragment fragment_no_socket = []() {
  Fragment f;
  f.name = "fragment_no_socket";
  f.positioning_views.insert_or_assign(steamrot::ViewDirection::Front,
                                       detail::MakeGreenOriginTriangle());
  return f;
}();

/////////////////////////////////////////////////
/// @brief fragment_one_socket: green origin triangle + one socket at (5, 5).
/////////////////////////////////////////////////
inline const Fragment fragment_one_socket = []() {
  Fragment f;
  f.name = "fragment_one_socket";
  f.positioning_views.insert_or_assign(steamrot::ViewDirection::Front,
                                       detail::MakeGreenOriginTriangle());

  Socket socket{{5.f, 5.f}, {1.f, 0.f}};
  f.sockets.push_back(socket);
  return f;
}();

/////////////////////////////////////////////////
/// @brief fragment_two_sockets: white 20×20 square + sockets at (0, 10) and
///        (20, 10).
/////////////////////////////////////////////////
inline const Fragment fragment_two_sockets = []() {
  Fragment f;
  f.name = "fragment_two_sockets";
  f.positioning_views.insert_or_assign(
      steamrot::ViewDirection::Front,
      detail::MakeFilledSquare(0.f, 0.f, sf::Color::White));
  Socket socket1{{0.f, 10.f}, {1.f, 0.f}};
  Socket socket2{{20.f, 10.f}, {-1.f, 0.f}};
  f.sockets.push_back(socket1);
  f.sockets.push_back(socket2);
  return f;
}();

/////////////////////////////////////////////////
/// @brief fragment_three_sockets: white 20×20 square + sockets at (0, 10),
///        (10, 10) and (20, 10).
/////////////////////////////////////////////////
inline const Fragment fragment_three_sockets = []() {
  Fragment f;
  f.name = "fragment_three_sockets";
  f.positioning_views.insert_or_assign(
      steamrot::ViewDirection::Front,
      detail::MakeFilledSquare(0.f, 0.f, sf::Color::White));
  Socket socket1{{0.f, 10.f}, {1.f, 0.f}};
  Socket socket2{{10.f, 10.f}, {0.f, 1.f}};
  Socket socket3{{20.f, 10.f}, {-1.f, 0.f}};
  f.sockets.push_back(socket1);
  f.sockets.push_back(socket2);
  f.sockets.push_back(socket3);

  return f;
}();

/////////////////////////////////////////////////
/// @brief joint_no_socket: blue origin triangle, no sockets.
/////////////////////////////////////////////////
inline const Joint joint_no_socket = []() {
  Joint j;
  j.name = "joint_no_socket";
  j.positioning_views.insert_or_assign(steamrot::ViewDirection::Front,
                                       detail::MakeBlueOriginTriangle());
  return j;
}();

/////////////////////////////////////////////////
/// @brief joint_one_socket: 1 socket at radius 10, full rotation arc.
/////////////////////////////////////////////////
inline const Joint joint_one_socket = []() {
  Joint j;
  j.name = "joint_one_socket";
  j.socket_config.socket_count = 1;
  j.socket_config.radius = 10.f;
  j.socket_config.rotation_arc_min = 0.f;
  j.socket_config.rotation_arc_max = 360.f;
  return j;
}();

/////////////////////////////////////////////////
/// @brief joint_two_sockets: 2 sockets at radius 15, full rotation arc.
/////////////////////////////////////////////////
inline const Joint joint_two_sockets = []() {
  Joint j;
  j.name = "joint_two_sockets";
  j.socket_config.socket_count = 2;
  j.socket_config.radius = 15.f;
  j.socket_config.rotation_arc_min = 0.f;
  j.socket_config.rotation_arc_max = 360.f;
  return j;
}();

/////////////////////////////////////////////////
/// @brief joint_three_sockets: 3 sockets at radius 15, full rotation arc.
/////////////////////////////////////////////////
inline const Joint joint_three_sockets = []() {
  Joint j;
  j.name = "joint_three_sockets";
  j.socket_config.socket_count = 3;
  j.socket_config.radius = 15.f;
  j.socket_config.rotation_arc_min = 0.f;
  j.socket_config.rotation_arc_max = 360.f;
  return j;
}();

} // namespace steamrot::tests
