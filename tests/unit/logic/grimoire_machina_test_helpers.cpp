/////////////////////////////////////////////////
/// @file
/// @brief Implementation of factory helper functions for Fragment, Joint,
///        FragmentInstance, JointInstance, and GrimoireMachina test objects.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "grimoire_machina_test_helpers.h"
#include "ViewDirection.h"
#include "positioning_grimoire_machina.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>

namespace {

/////////////////////////////////////////////////
/// @brief Build a filled 20×20 square VertexArray (Triangles) with its
///        top-left corner at (x, y).
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

} // namespace

namespace steamrot::tests {

/////////////////////////////////////////////////
steamrot::Views MakeViewsWithFrontTriangle(sf::Color colour) {
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{10.f, 10.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{20.f, 10.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{15.f, 20.f}, colour});

  steamrot::Views views;
  views.insert_or_assign(steamrot::ViewDirection::Front, std::move(va));
  return views;
}

/////////////////////////////////////////////////
steamrot::Fragment MakeFragment() { return steamrot::Fragment{}; }

/////////////////////////////////////////////////
steamrot::Fragment MakeFragmentWithFrontView(sf::Color colour) {
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{5.f, 5.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{25.f, 5.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{15.f, 25.f}, colour});

  steamrot::Fragment fragment;
  fragment.positioning_views.insert_or_assign(steamrot::ViewDirection::Front,
                                              std::move(va));
  return fragment;
}

/////////////////////////////////////////////////
steamrot::Fragment MakeFragmentWithOriginTriangle(sf::Color colour) {
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{0.f, 0.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{20.f, 0.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{10.f, 20.f}, colour});

  steamrot::Fragment fragment;
  fragment.positioning_views.insert_or_assign(steamrot::ViewDirection::Front,
                                              std::move(va));
  Socket socket{{5.f, 5.f}, {1.f, 0.f}};
  fragment.sockets.push_back(socket);
  return fragment;
}

/////////////////////////////////////////////////
steamrot::Fragment MakeFragmentWithSockets(std::vector<Socket> sockets) {
  steamrot::Fragment fragment;
  fragment.sockets = std::move(sockets);
  return fragment;
}

/////////////////////////////////////////////////
std::vector<steamrot::Fragment> MakeFragments(size_t count) {
  return std::vector<steamrot::Fragment>(count);
}

/////////////////////////////////////////////////
steamrot::Joint MakeJoint() { return steamrot::Joint{}; }

/////////////////////////////////////////////////
steamrot::Joint MakeJointWithFrontView(sf::Color colour) {
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{10.f, 10.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{30.f, 10.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{20.f, 30.f}, colour});

  steamrot::Joint joint;
  joint.positioning_views.insert_or_assign(steamrot::ViewDirection::Front,
                                           std::move(va));
  return joint;
}

/////////////////////////////////////////////////
steamrot::Joint MakeJointWithOriginTriangle(sf::Color colour) {
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{0.f, 0.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{20.f, 0.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{10.f, 20.f}, colour});

  steamrot::Joint joint;
  joint.positioning_views.insert_or_assign(steamrot::ViewDirection::Front,
                                           std::move(va));
  joint.socket_config.socket_count = 1;
  joint.socket_config.radius = 5.f;
  joint.socket_config.has_fixed_socket = false;
  return joint;
}

/////////////////////////////////////////////////
steamrot::Joint MakeJointWithSocketConfig(uint8_t socket_count, float radius,
                                          bool has_fixed_socket, float arc_min,
                                          float arc_max) {
  steamrot::Joint joint;
  joint.socket_config.socket_count = socket_count;
  joint.socket_config.radius = radius;
  joint.socket_config.has_fixed_socket = has_fixed_socket;
  joint.socket_config.rotation_arc_min = arc_min;
  joint.socket_config.rotation_arc_max = arc_max;
  return joint;
}

/////////////////////////////////////////////////
std::vector<steamrot::Joint> MakeJoints(size_t count) {
  return std::vector<steamrot::Joint>(count);
}

/////////////////////////////////////////////////
steamrot::GrimoireMachina
MakeGrimoireWithEmptyFragment(const std::string &name) {
  steamrot::GrimoireMachina grimoire;
  steamrot::Fragment fragment;
  fragment.name = name;
  grimoire.m_all_fragments.insert({name, std::move(fragment)});
  return grimoire;
}

/////////////////////////////////////////////////
steamrot::GrimoireMachina
MakeGrimoireWithPopulatedFragment(const std::string &name) {
  steamrot::GrimoireMachina grimoire;
  steamrot::Fragment fragment;
  fragment.name = name;
  fragment.positioning_views.insert_or_assign(
      steamrot::ViewDirection::Front,
      MakeFilledSquare(0.f, 0.f, sf::Color::White));
  grimoire.m_all_fragments.insert({name, std::move(fragment)});
  return grimoire;
}

/////////////////////////////////////////////////
steamrot::GrimoireMachina MakeGrimoireWithEmptyJoint(const std::string &name) {
  steamrot::GrimoireMachina grimoire;
  steamrot::Joint joint;
  joint.name = name;
  grimoire.m_all_joints.insert({name, std::move(joint)});
  return grimoire;
}

/////////////////////////////////////////////////
steamrot::GrimoireMachina
MakeGrimoireWithPopulatedJoint(const std::string &name) {
  steamrot::GrimoireMachina grimoire;
  steamrot::Joint joint;
  joint.name = name;
  joint.positioning_views.insert_or_assign(
      steamrot::ViewDirection::Front,
      MakeFilledSquare(0.f, 0.f, sf::Color::White));
  grimoire.m_all_joints.insert({name, std::move(joint)});
  return grimoire;
}

/////////////////////////////////////////////////
steamrot::GrimoireMachina
MakeGrimoireWithFragmentAndSocket(const std::string &name,
                                  sf::Vector2f socket_local_pos) {
  steamrot::GrimoireMachina grimoire;
  steamrot::Fragment fragment;
  fragment.name = name;
  fragment.positioning_views.insert_or_assign(
      steamrot::ViewDirection::Front,
      MakeFilledSquare(0.f, 0.f, sf::Color::White));
  Socket socket{{socket_local_pos.x, socket_local_pos.y}, {1.f, 0.f}};
  fragment.sockets.push_back(socket);
  grimoire.m_all_fragments.insert({name, std::move(fragment)});
  return grimoire;
}

/////////////////////////////////////////////////
steamrot::GrimoireMachina
MakeGrimoireWithFragmentAndSockets(const std::string &name,
                                   std::vector<sf::Vector2f> socket_positions) {
  steamrot::GrimoireMachina grimoire;
  steamrot::Fragment fragment;
  fragment.name = name;
  fragment.positioning_views.insert_or_assign(
      steamrot::ViewDirection::Front,
      MakeFilledSquare(0.f, 0.f, sf::Color::White));
  for (const auto &pos : socket_positions) {
    Socket socket{{pos.x, pos.y}, {1.f, 0.f}};
    fragment.sockets.push_back(socket);
  }

  grimoire.m_all_fragments.insert({name, std::move(fragment)});
  return grimoire;
}

/////////////////////////////////////////////////
steamrot::GrimoireMachina
MakeGrimoireWithJointAndSocket(const std::string &name) {
  steamrot::GrimoireMachina grimoire;
  steamrot::Joint joint;
  joint.name = name;
  joint.positioning_views.insert_or_assign(
      steamrot::ViewDirection::Front,
      MakeFilledSquare(0.f, 0.f, sf::Color::White));
  joint.socket_config.socket_count = 1;
  joint.socket_config.radius = 25.f;
  joint.socket_config.has_fixed_socket = false;
  grimoire.m_all_joints.insert({name, std::move(joint)});
  return grimoire;
}

/////////////////////////////////////////////////
PartGraphFragmentFixture MakePartGraphWithSingleFragmentInstance() {
  PartGraphFragmentFixture fixture;
  fixture.fragment =
      std::make_unique<steamrot::Fragment>(MakeFragmentWithOriginTriangle());

  steamrot::FragmentInstance instance{fixture.fragment.get()};
  instance.id = 0;
  fixture.parts.emplace(0, std::move(instance));

  return fixture;
}

/////////////////////////////////////////////////
PartGraphJointFixture MakePartGraphWithSingleJointInstance() {
  PartGraphJointFixture fixture;
  fixture.joint =
      std::make_unique<steamrot::Joint>(MakeJointWithOriginTriangle());

  steamrot::JointInstance instance{fixture.joint.get()};
  instance.id = 0;
  steamrot::logic::positioning::grimoire_machina::
      initialize_joint_socket_positions(instance);
  fixture.parts.emplace(0, std::move(instance));

  return fixture;
}

} // namespace steamrot::tests
