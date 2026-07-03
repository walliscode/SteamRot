/////////////////////////////////////////////////
/// @file
/// @brief Declaration of factory helper functions for Fragment, Joint,
///        FragmentInstance, JointInstance, and GrimoireMachina test objects.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Fragment.h"
#include "GrimoireMachina.h"
#include "Joint.h"
#include "MachinaFormScaffold.h"
#include "ViewDirection.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <string>
#include <vector>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// Views helpers
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// @brief Build a Views object with a single coloured triangle in the Front
///        slot. Vertices at (10,10), (20,10), (15,20).
///
/// @param colour Fill colour of the triangle. Defaults to red.
/// @return Views map with one Front entry.
/////////////////////////////////////////////////
steamrot::Views MakeViewsWithFrontTriangle(sf::Color colour = sf::Color::Red);

/////////////////////////////////////////////////
/// Fragment factory functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// @brief Build a default-constructed empty Fragment.
///
/// @return Fragment with no view geometry and no sockets.
/////////////////////////////////////////////////
steamrot::Fragment MakeFragment();

/////////////////////////////////////////////////
/// @brief Build a Fragment with a coloured triangle in the Front slot.
///        Vertices at (5,5), (25,5), (15,25). No sockets.
///
/// @param colour Fill colour of the triangle. Defaults to green.
/// @return Fragment with one Front view entry.
/////////////////////////////////////////////////
steamrot::Fragment
MakeFragmentWithFrontView(sf::Color colour = sf::Color::Green);

/////////////////////////////////////////////////
/// @brief Build a Fragment with an origin-aligned coloured triangle in the
///        Front slot and a single socket at local position (5, 5).
///        Vertices at (0,0), (20,0), (10,20).
///
/// @param colour Fill colour of the triangle. Defaults to green.
/// @return Fragment with one Front view entry and one socket.
/////////////////////////////////////////////////
steamrot::Fragment
MakeFragmentWithOriginTriangle(sf::Color colour = sf::Color::Green);

/////////////////////////////////////////////////
/// @brief Build a Fragment with the given local socket positions and no view
///        geometry.
///
/// @param sockets Local socket positions to assign.
/// @return Fragment whose sockets vector matches the argument.
/////////////////////////////////////////////////
steamrot::Fragment MakeFragmentWithSockets(std::vector<SocketData> sockets);

/////////////////////////////////////////////////
/// @brief Build a vector of N default-constructed empty Fragments.
///
/// @param count Number of fragments to create.
/// @return Vector of empty Fragment objects.
/////////////////////////////////////////////////
std::vector<steamrot::Fragment> MakeFragments(size_t count);

/////////////////////////////////////////////////
/// Joint factory functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// @brief Build a default-constructed empty Joint.
///
/// @return Joint with no view geometry and a zero socket_config.
/////////////////////////////////////////////////
steamrot::Joint MakeJoint();

/////////////////////////////////////////////////
/// @brief Build a Joint with a coloured triangle in the Front slot.
///        Vertices at (10,10), (30,10), (20,30). No sockets configured.
///
/// @param colour Fill colour of the triangle. Defaults to blue.
/// @return Joint with one Front view entry.
/////////////////////////////////////////////////
steamrot::Joint MakeJointWithFrontView(sf::Color colour = sf::Color::Blue);

/////////////////////////////////////////////////
/// @brief Build a Joint with an origin-aligned coloured triangle in the Front
///        slot and a SocketConfig of one socket at radius 5 (angle 0).
///        Vertices at (0,0), (20,0), (10,20).
///
/// @param colour Fill colour of the triangle. Defaults to blue.
/// @return Joint with one Front view entry and a one-socket SocketConfig.
/////////////////////////////////////////////////
steamrot::Joint MakeJointWithOriginTriangle(sf::Color colour = sf::Color::Blue);

/////////////////////////////////////////////////
/// @brief Build a Joint with the given SocketConfig parameters and no view
///        geometry.
///
/// @param socket_count      Total number of sockets.
/// @param radius            Distance of sockets from the Joint's origin.
/// @param has_fixed_socket  Whether socket index 0 is a fixed anchor.
/// @param arc_min           Minimum rotation arc angle in degrees (default 0).
/// @param arc_max           Maximum rotation arc angle in degrees (default
/// 360).
/// @return Joint with the specified SocketConfig.
/////////////////////////////////////////////////
steamrot::Joint MakeJointWithSocketConfig(uint8_t socket_count, float radius,
                                          bool has_fixed_socket = false,
                                          float arc_min = 0.f,
                                          float arc_max = 360.f);

/////////////////////////////////////////////////
/// @brief Build a vector of N default-constructed empty Joints.
///
/// @param count Number of joints to create.
/// @return Vector of empty Joint objects.
/////////////////////////////////////////////////
std::vector<steamrot::Joint> MakeJoints(size_t count);

/////////////////////////////////////////////////
/// GrimoireMachina factory functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// @brief Build a GrimoireMachina containing one named Fragment with no view
///        geometry.
///
/// @param name Key under which the Fragment is stored.
/// @return GrimoireMachina with one empty Fragment.
/////////////////////////////////////////////////
steamrot::GrimoireMachina
MakeGrimoireWithEmptyFragment(const std::string &name);

/////////////////////////////////////////////////
/// @brief Build a GrimoireMachina containing one named Fragment whose Front
///        view is a solid white 20×20 square centred at the origin.
///
/// @param name Key under which the Fragment is stored.
/// @return GrimoireMachina with one white-square Fragment.
/////////////////////////////////////////////////
steamrot::GrimoireMachina
MakeGrimoireWithPopulatedFragment(const std::string &name);

/////////////////////////////////////////////////
/// @brief Build a GrimoireMachina containing one named Joint with no view
///        geometry.
///
/// @param name Key under which the Joint is stored.
/// @return GrimoireMachina with one empty Joint.
/////////////////////////////////////////////////
steamrot::GrimoireMachina MakeGrimoireWithEmptyJoint(const std::string &name);

/////////////////////////////////////////////////
/// @brief Build a GrimoireMachina containing one named Joint whose Front view
///        is a solid white 20×20 square centred at the origin.
///
/// @param name Key under which the Joint is stored.
/// @return GrimoireMachina with one white-square Joint.
/////////////////////////////////////////////////
steamrot::GrimoireMachina
MakeGrimoireWithPopulatedJoint(const std::string &name);

/////////////////////////////////////////////////
/// @brief Build a GrimoireMachina containing one named Fragment with a white
///        20×20 Front view and a single socket at the given local position.
///
/// @param name            Key under which the Fragment is stored.
/// @param socket_local_pos Local position of the single socket.
/// @return GrimoireMachina with one Fragment carrying one socket.
/////////////////////////////////////////////////
steamrot::GrimoireMachina
MakeGrimoireWithFragmentAndSocket(const std::string &name,
                                  sf::Vector2f socket_local_pos);

/////////////////////////////////////////////////
/// @brief Build a GrimoireMachina containing one named Fragment with a white
///        20×20 Front view and the given local socket positions.
///
/// @param name             Key under which the Fragment is stored.
/// @param socket_positions Local positions for all sockets.
/// @return GrimoireMachina with one Fragment carrying multiple sockets.
/////////////////////////////////////////////////
steamrot::GrimoireMachina
MakeGrimoireWithFragmentAndSockets(const std::string &name,
                                   std::vector<sf::Vector2f> socket_positions);

/////////////////////////////////////////////////
/// @brief Build a GrimoireMachina containing one named Joint with a white
///        20×20 Front view and a SocketConfig that places one socket at local
///        position (25, 0): socket_count=1, radius=25, arc_min=arc_max=0.
///
/// @param name Key under which the Joint is stored.
/// @return GrimoireMachina with one Joint carrying one socket.
/////////////////////////////////////////////////
steamrot::GrimoireMachina
MakeGrimoireWithJointAndSocket(const std::string &name);

/////////////////////////////////////////////////
/// PartGraph fixture structs
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// @struct PartGraphFragmentFixture
/// @brief Owns a Fragment definition and a PartGraph containing one
///        FragmentInstance that points into the owned Fragment.
///
/// Keeps the Fragment definition alive for the lifetime of the fixture so
/// that the raw pointer inside the FragmentInstance remains valid.
/////////////////////////////////////////////////
struct PartGraphFragmentFixture {
  std::unique_ptr<steamrot::Fragment> fragment;
  steamrot::PartGraph parts;
};

/////////////////////////////////////////////////
/// @struct PartGraphJointFixture
/// @brief Owns a Joint definition and a PartGraph containing one JointInstance
///        that points into the owned Joint.
///
/// Keeps the Joint definition alive for the lifetime of the fixture so that
/// the raw pointer inside the JointInstance remains valid.
/////////////////////////////////////////////////
struct PartGraphJointFixture {
  std::unique_ptr<steamrot::Joint> joint;
  steamrot::PartGraph parts;
};

/////////////////////////////////////////////////
/// PartGraph factory functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// @brief Build a PartGraphFragmentFixture containing a single fully-configured
///        FragmentInstance at key 0. The Fragment has an origin-aligned
///        triangle Front view and one socket at local position (5, 5).
///
/// @return Fixture owning the Fragment definition and the PartGraph.
/////////////////////////////////////////////////
PartGraphFragmentFixture MakePartGraphWithSingleFragmentInstance();

/////////////////////////////////////////////////
/// @brief Build a PartGraphJointFixture containing a single fully-configured
///        JointInstance at key 0. The Joint has an origin-aligned triangle
///        Front view, a one-socket SocketConfig (radius 5, full-circle arc),
///        and socket positions initialised via the even-spread algorithm.
///
/// @return Fixture owning the Joint definition and the PartGraph.
/////////////////////////////////////////////////
PartGraphJointFixture MakePartGraphWithSingleJointInstance();

} // namespace steamrot::tests
