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
