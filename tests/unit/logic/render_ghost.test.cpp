/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the render_ghost free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_ghost.h"
#include "Fragment.h"
#include "GrimoireMachina.h"
#include "Joint.h"
#include "MrGhost.h"
#include "ViewDirection.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <catch2/catch_test_macros.hpp>
#include <vector>

namespace {

/////////////////////////////////////////////////
/// @brief Make a filled 20x20 square VertexArray (Triangles) starting at (x, y).
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
/// @brief Build a GrimoireMachina with one named fragment that has NO view geometry.
/////////////////////////////////////////////////
steamrot::GrimoireMachina MakeGrimoireWithEmptyFragment(const std::string &name) {
  steamrot::GrimoireMachina grimoire;
  steamrot::Fragment fragment;
  fragment.name = name;
  // movement_views is empty by default
  grimoire.m_all_fragments.insert({name, std::move(fragment)});
  return grimoire;
}

/////////////////////////////////////////////////
/// @brief Build a GrimoireMachina with one named fragment whose Front view
///        is a solid white 20x20 square centred at the origin.
/////////////////////////////////////////////////
steamrot::GrimoireMachina MakeGrimoireWithPopulatedFragment(
    const std::string &name) {
  steamrot::GrimoireMachina grimoire;
  steamrot::Fragment fragment;
  fragment.name = name;
  fragment.movement_views.insert_or_assign(steamrot::ViewDirection::Front,
                                            MakeFilledSquare(0.f, 0.f,
                                                             sf::Color::White));
  grimoire.m_all_fragments.insert({name, std::move(fragment)});
  return grimoire;
}

/////////////////////////////////////////////////
/// @brief Build a GrimoireMachina with one named joint that has NO view geometry.
/////////////////////////////////////////////////
steamrot::GrimoireMachina MakeGrimoireWithEmptyJoint(const std::string &name) {
  steamrot::GrimoireMachina grimoire;
  steamrot::Joint joint;
  joint.name = name;
  grimoire.m_all_joints.insert({name, std::move(joint)});
  return grimoire;
}

/////////////////////////////////////////////////
/// @brief Build a GrimoireMachina with one named joint whose Front view
///        is a solid white 20x20 square centred at the origin.
/////////////////////////////////////////////////
steamrot::GrimoireMachina MakeGrimoireWithPopulatedJoint(
    const std::string &name) {
  steamrot::GrimoireMachina grimoire;
  steamrot::Joint joint;
  joint.name = name;
  joint.movement_views.insert_or_assign(steamrot::ViewDirection::Front,
                                         MakeFilledSquare(0.f, 0.f,
                                                          sf::Color::White));
  grimoire.m_all_joints.insert({name, std::move(joint)});
  return grimoire;
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// Coordinate reference used by socket tests
//
// MakeFilledSquare(0, 0, White) produces a 20×20 square with:
//   bounds.position = (0, 0)   bounds.size = (20, 20)
//
// DrawGhostItem computes:
//   translate = mr_ghost.m_position - bounds.position - bounds.size - (5, 5)
//
// With mr_ghost.m_position = (100, 100):
//   translate = (100-0-20-5, 100-0-20-5) = (75, 75)
//
// A socket at local position (25, 10) therefore lands at texture (100, 85),
// which is outside the white square [75..95, 75..95] so the background there
// is pure black before the socket circle is painted.
// ---------------------------------------------------------------------------

namespace {

/////////////////////////////////////////////////
/// @brief Build a GrimoireMachina with one named fragment (20×20 white Front
///        view) and a single socket at the given local position.
/////////////////////////////////////////////////
steamrot::GrimoireMachina
MakeGrimoireWithFragmentAndSocket(const std::string &name,
                                  sf::Vector2f socket_local_pos) {
  steamrot::GrimoireMachina grimoire;
  steamrot::Fragment fragment;
  fragment.name = name;
  fragment.movement_views.insert_or_assign(steamrot::ViewDirection::Front,
                                           MakeFilledSquare(0.f, 0.f,
                                                            sf::Color::White));
  fragment.sockets.push_back(socket_local_pos);
  grimoire.m_all_fragments.insert({name, std::move(fragment)});
  return grimoire;
}

/////////////////////////////////////////////////
/// @brief Build a GrimoireMachina with one named fragment (20×20 white Front
///        view) and multiple sockets at the given local positions.
/////////////////////////////////////////////////
steamrot::GrimoireMachina
MakeGrimoireWithFragmentAndSockets(const std::string &name,
                                   std::vector<sf::Vector2f> socket_positions) {
  steamrot::GrimoireMachina grimoire;
  steamrot::Fragment fragment;
  fragment.name = name;
  fragment.movement_views.insert_or_assign(steamrot::ViewDirection::Front,
                                           MakeFilledSquare(0.f, 0.f,
                                                            sf::Color::White));
  fragment.sockets = std::move(socket_positions);
  grimoire.m_all_fragments.insert({name, std::move(fragment)});
  return grimoire;
}

/////////////////////////////////////////////////
/// @brief Build a GrimoireMachina with one named joint (20×20 white Front
///        view) and a single socket at the given local position.
/////////////////////////////////////////////////
steamrot::GrimoireMachina
MakeGrimoireWithJointAndSocket(const std::string &name,
                               sf::Vector2f socket_local_pos) {
  steamrot::GrimoireMachina grimoire;
  steamrot::Joint joint;
  joint.name = name;
  joint.movement_views.insert_or_assign(steamrot::ViewDirection::Front,
                                        MakeFilledSquare(0.f, 0.f,
                                                         sf::Color::White));
  joint.sockets.push_back(socket_local_pos);
  grimoire.m_all_joints.insert({name, std::move(joint)});
  return grimoire;
}

} // anonymous namespace (second block — socket helpers)

/////////////////////////////////////////////////
/// DrawGhostItem — monostate (nothing selected)
/////////////////////////////////////////////////

TEST_CASE("DrawGhostItem draws nothing when selection is monostate",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_selection));

  steamrot::GrimoireMachina grimoire;

  REQUIRE_NOTHROW(steamrot::logic::render::ghost::DrawGhostItem(
      texture, mr_ghost, grimoire));

  texture.display();
  const sf::Image image = texture.getTexture().copyToImage();

  // Entire texture should remain black — nothing was drawn
  REQUIRE(image.getPixel({50, 50}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// DrawGhostItem — FragmentTag: key not in grimoire
/////////////////////////////////////////////////

TEST_CASE("DrawGhostItem does not throw when FragmentTag key is not found",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"missing"};
  mr_ghost.m_position = {50.f, 50.f};

  steamrot::GrimoireMachina grimoire; // empty — no fragments

  REQUIRE_NOTHROW(steamrot::logic::render::ghost::DrawGhostItem(
      texture, mr_ghost, grimoire));

  texture.display();
  const sf::Image image = texture.getTexture().copyToImage();
  REQUIRE(image.getPixel({50, 50}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// DrawGhostItem — FragmentTag: empty view draws nothing
/////////////////////////////////////////////////

TEST_CASE("DrawGhostItem draws nothing when the fragment view is empty",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"stone"};
  mr_ghost.m_position = {50.f, 50.f};

  // fragment exists but has an empty movement_views
  steamrot::GrimoireMachina grimoire = MakeGrimoireWithEmptyFragment("stone");

  REQUIRE_NOTHROW(steamrot::logic::render::ghost::DrawGhostItem(
      texture, mr_ghost, grimoire));

  texture.display();
  const sf::Image image = texture.getTexture().copyToImage();
  REQUIRE(image.getPixel({50, 50}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// DrawGhostItem — FragmentTag: populated view draws pixels at cursor
/////////////////////////////////////////////////

TEST_CASE("DrawGhostItem uses DrawFragmentView to draw at the cursor position",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{200, 200}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"arm"};
  mr_ghost.m_position = {50.f, 50.f};

  // fragment has a 20x20 white square starting at origin; with translate(50,50)
  // the square maps to [50..70, 50..70] on the texture
  steamrot::GrimoireMachina grimoire =
      MakeGrimoireWithPopulatedFragment("arm");

  steamrot::logic::render::ghost::DrawGhostItem(texture, mr_ghost, grimoire);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();

  // Centre of the translated square should be white
  REQUIRE(image.getPixel({60, 60}) == sf::Color::White);
  // A point before the translation should be black
  REQUIRE(image.getPixel({5, 5}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// DrawGhostItem — JointTag: key not in grimoire
/////////////////////////////////////////////////

TEST_CASE("DrawGhostItem does not throw when JointTag key is not found",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"missing"};
  mr_ghost.m_position = {50.f, 50.f};

  steamrot::GrimoireMachina grimoire; // empty — no joints

  REQUIRE_NOTHROW(steamrot::logic::render::ghost::DrawGhostItem(
      texture, mr_ghost, grimoire));

  texture.display();
  const sf::Image image = texture.getTexture().copyToImage();
  REQUIRE(image.getPixel({50, 50}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// DrawGhostItem — JointTag: empty view draws nothing
/////////////////////////////////////////////////

TEST_CASE("DrawGhostItem draws nothing when the joint view is empty",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"hinge"};
  mr_ghost.m_position = {50.f, 50.f};

  steamrot::GrimoireMachina grimoire = MakeGrimoireWithEmptyJoint("hinge");

  REQUIRE_NOTHROW(steamrot::logic::render::ghost::DrawGhostItem(
      texture, mr_ghost, grimoire));

  texture.display();
  const sf::Image image = texture.getTexture().copyToImage();
  REQUIRE(image.getPixel({50, 50}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// DrawGhostItem — JointTag: populated view draws pixels at cursor
/////////////////////////////////////////////////

TEST_CASE("DrawGhostItem uses DrawJointView to draw at the cursor position",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{200, 200}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"pivot"};
  mr_ghost.m_position = {50.f, 50.f};

  // joint has a 20x20 white square at origin; translate(50,50) → [50..70, 50..70]
  steamrot::GrimoireMachina grimoire = MakeGrimoireWithPopulatedJoint("pivot");

  steamrot::logic::render::ghost::DrawGhostItem(texture, mr_ghost, grimoire);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();

  REQUIRE(image.getPixel({60, 60}) == sf::Color::White);
  REQUIRE(image.getPixel({5, 5}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// DrawGhostItem — position is respected
/////////////////////////////////////////////////

TEST_CASE("DrawGhostItem renders fragment geometry at the stored cursor position",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{300, 300}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"rock"};
  mr_ghost.m_position = {200.f, 200.f}; // far corner

  steamrot::GrimoireMachina grimoire =
      MakeGrimoireWithPopulatedFragment("rock");
  steamrot::logic::render::ghost::DrawGhostItem(texture, mr_ghost, grimoire);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();

  // Centre of the translated square [200..220, 200..220]
  REQUIRE(image.getPixel({210, 210}) == sf::Color::White);
  // Near origin — nothing
  REQUIRE(image.getPixel({5, 5}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// DrawGhostItem — sockets: FragmentTag, socket drawn outside geometry
/////////////////////////////////////////////////

TEST_CASE("DrawGhostItem draws socket circle for a FragmentTag selection",
          "[unit][render_ghost]") {
  // Texture large enough to contain geometry + socket
  sf::RenderTexture texture{{200, 200}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"leaf"};
  mr_ghost.m_position = {100.f, 100.f};

  // Socket at local (25, 10) → texture (100, 85): outside the white square
  // [75..95, 75..95] so the background there is black before the socket is drawn.
  steamrot::GrimoireMachina grimoire =
      MakeGrimoireWithFragmentAndSocket("leaf", {25.f, 10.f});

  steamrot::logic::render::ghost::DrawGhostItem(texture, mr_ghost, grimoire);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();

  // The socket circle centre must have been painted (not black)
  REQUIRE(image.getPixel({100, 85}) != sf::Color::Black);
  // A point well away from both geometry and socket must remain black
  REQUIRE(image.getPixel({10, 10}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// DrawGhostItem — sockets: FragmentTag, no sockets → socket area stays black
/////////////////////////////////////////////////

TEST_CASE("DrawGhostItem draws no socket pixels when fragment sockets list is empty",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{200, 200}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"bare"};
  mr_ghost.m_position = {100.f, 100.f};

  // Fragment with geometry but NO sockets
  steamrot::GrimoireMachina grimoire =
      MakeGrimoireWithPopulatedFragment("bare");

  steamrot::logic::render::ghost::DrawGhostItem(texture, mr_ghost, grimoire);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();

  // Position (100, 85) is where a socket at local (25, 10) would have been
  // painted — with no sockets it must remain black.
  REQUIRE(image.getPixel({100, 85}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// DrawGhostItem — sockets: JointTag, socket drawn outside geometry
/////////////////////////////////////////////////

TEST_CASE("DrawGhostItem draws socket circle for a JointTag selection",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{200, 200}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"knuckle"};
  mr_ghost.m_position = {100.f, 100.f};

  // Same local position and coordinate logic as the fragment socket test
  steamrot::GrimoireMachina grimoire =
      MakeGrimoireWithJointAndSocket("knuckle", {25.f, 10.f});

  steamrot::logic::render::ghost::DrawGhostItem(texture, mr_ghost, grimoire);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();

  REQUIRE(image.getPixel({100, 85}) != sf::Color::Black);
  REQUIRE(image.getPixel({10, 10}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// DrawGhostItem — sockets: JointTag, no sockets → socket area stays black
/////////////////////////////////////////////////

TEST_CASE("DrawGhostItem draws no socket pixels when joint sockets list is empty",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{200, 200}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"bare"};
  mr_ghost.m_position = {100.f, 100.f};

  // Joint with geometry but NO sockets
  steamrot::GrimoireMachina grimoire =
      MakeGrimoireWithPopulatedJoint("bare");

  steamrot::logic::render::ghost::DrawGhostItem(texture, mr_ghost, grimoire);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();

  REQUIRE(image.getPixel({100, 85}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// DrawGhostItem — sockets: multiple sockets are all drawn
/////////////////////////////////////////////////

TEST_CASE("DrawGhostItem draws all socket circles when fragment has multiple sockets",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{200, 200}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"multi"};
  mr_ghost.m_position = {100.f, 100.f};

  // Two sockets at distinct local positions that both fall outside the white
  // square [75..95, 75..95] once the transform (75, 75) is applied:
  //   local (25, 10)  → texture (100, 85)
  //   local (30, 25)  → texture (105, 100)
  steamrot::GrimoireMachina grimoire = MakeGrimoireWithFragmentAndSockets(
      "multi", {{25.f, 10.f}, {30.f, 25.f}});

  steamrot::logic::render::ghost::DrawGhostItem(texture, mr_ghost, grimoire);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();

  // Both socket centres must have been painted
  REQUIRE(image.getPixel({100, 85}) != sf::Color::Black);
  REQUIRE(image.getPixel({105, 100}) != sf::Color::Black);
  // Control: an unrelated pixel must remain black
  REQUIRE(image.getPixel({10, 10}) == sf::Color::Black);
}
