/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the TestPartLibrary struct and PartLibraryBuilder
///        class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "part_library.h"
#include "ViewDirection.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>

namespace {

/////////////////////////////////////////////////
/// @brief Build a filled 20×20 square VertexArray (Triangles primitive).
///
/// @param x     Left edge x-coordinate.
/// @param y     Top edge y-coordinate.
/// @param color Fill color.
/// @return VertexArray with 6 vertices forming two triangles.
/////////////////////////////////////////////////
sf::VertexArray MakeFilledSquare(float x, float y, sf::Color color) {
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
sf::VertexArray MakeGreenOriginTriangle() {
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
sf::VertexArray MakeBlueOriginTriangle() {
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{0.f, 0.f}, sf::Color::Blue});
  va.append(sf::Vertex{sf::Vector2f{20.f, 0.f}, sf::Color::Blue});
  va.append(sf::Vertex{sf::Vector2f{10.f, 20.f}, sf::Color::Blue});
  return va;
}

} // namespace

namespace steamrot::tests {

/////////////////////////////////////////////////
TestPartLibrary TestPartLibrary::Create() {
  TestPartLibrary lib;

  // ── Fragments ────────────────────────────────────────────────────────── //

  // "fragment_no_socket": green triangle in Front view, no sockets
  {
    Fragment f;
    f.name = "fragment_no_socket";
    f.movement_views.insert_or_assign(steamrot::ViewDirection::Front,
                                      MakeGreenOriginTriangle());
    lib.fragments.emplace("fragment_no_socket", std::move(f));
  }

  // "fragment_one_socket": green origin triangle + one socket at (5, 5)
  {
    Fragment f;
    f.name = "fragment_one_socket";
    f.movement_views.insert_or_assign(steamrot::ViewDirection::Front,
                                      MakeGreenOriginTriangle());
    f.sockets = {{5.f, 5.f}};
    lib.fragments.emplace("fragment_one_socket", std::move(f));
  }

  // "fragment_two_sockets": white 20×20 square + sockets at (0, 10) and (20, 10)
  {
    Fragment f;
    f.name = "fragment_two_sockets";
    f.movement_views.insert_or_assign(
        steamrot::ViewDirection::Front,
        MakeFilledSquare(0.f, 0.f, sf::Color::White));
    f.sockets = {{0.f, 10.f}, {20.f, 10.f}};
    lib.fragments.emplace("fragment_two_sockets", std::move(f));
  }

  // ── Joints ───────────────────────────────────────────────────────────── //

  // "joint_no_socket": blue triangle in Front view, no sockets configured
  {
    Joint j;
    j.name = "joint_no_socket";
    j.movement_views.insert_or_assign(steamrot::ViewDirection::Front,
                                      MakeBlueOriginTriangle());
    lib.joints.emplace("joint_no_socket", std::move(j));
  }

  // "joint_one_socket": 1 socket at radius 10, full rotation arc
  {
    Joint j;
    j.name = "joint_one_socket";
    j.socket_config.socket_count = 1;
    j.socket_config.radius = 10.f;
    j.socket_config.rotation_arc_min = 0.f;
    j.socket_config.rotation_arc_max = 360.f;
    lib.joints.emplace("joint_one_socket", std::move(j));
  }

  // "joint_two_sockets": 2 sockets at radius 15, full rotation arc
  {
    Joint j;
    j.name = "joint_two_sockets";
    j.socket_config.socket_count = 2;
    j.socket_config.radius = 15.f;
    j.socket_config.rotation_arc_min = 0.f;
    j.socket_config.rotation_arc_max = 360.f;
    lib.joints.emplace("joint_two_sockets", std::move(j));
  }

  return lib;
}

/////////////////////////////////////////////////
PartLibraryBuilder::PartLibraryBuilder(TestPartLibrary &library)
    : m_library{library} {}

/////////////////////////////////////////////////
FragmentInstance
PartLibraryBuilder::MakeFragmentInstance(const std::string &name,
                                         sf::Transform initial_transform) {
  auto it = m_library.fragments.find(name);
  REQUIRE(it != m_library.fragments.end());

  FragmentInstance instance{&it->second, initial_transform};
  instance.id = m_next_id++;
  return instance;
}

/////////////////////////////////////////////////
JointInstance
PartLibraryBuilder::MakeJointInstance(const std::string &name,
                                      sf::Transform initial_transform) {
  auto it = m_library.joints.find(name);
  REQUIRE(it != m_library.joints.end());

  JointInstance instance{&it->second, initial_transform};
  instance.id = m_next_id++;
  return instance;
}

/////////////////////////////////////////////////
PartMap
PartLibraryBuilder::MakePartMap(const std::vector<std::string> &fragment_names,
                                const std::vector<std::string> &joint_names) {
  PartMap parts;

  for (const auto &name : fragment_names) {
    FragmentInstance instance = MakeFragmentInstance(name);
    parts.emplace(instance.id, std::move(instance));
  }

  for (const auto &name : joint_names) {
    JointInstance instance = MakeJointInstance(name);
    parts.emplace(instance.id, std::move(instance));
  }

  return parts;
}

} // namespace steamrot::tests
