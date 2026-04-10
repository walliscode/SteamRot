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
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <catch2/catch_test_macros.hpp>

namespace {

/////////////////////////////////////////////////
/// @brief Helper: build a GrimoireMachina with one named fragment.
/////////////////////////////////////////////////
steamrot::GrimoireMachina MakeGrimoireWithFragment(const std::string &name) {
  steamrot::GrimoireMachina grimoire;
  steamrot::Fragment fragment;
  fragment.name = name;
  grimoire.m_all_fragments.insert({name, std::move(fragment)});
  return grimoire;
}

/////////////////////////////////////////////////
/// @brief Helper: build a GrimoireMachina with one named joint.
/////////////////////////////////////////////////
steamrot::GrimoireMachina MakeGrimoireWithJoint(const std::string &name) {
  steamrot::GrimoireMachina grimoire;
  steamrot::Joint joint;
  joint.name = name;
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
  // m_selection is std::monostate by default
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
/// DrawGhostItem — FragmentTag selection
/////////////////////////////////////////////////

TEST_CASE("DrawGhostItem draws a cyan shape when a FragmentTag is selected",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"stone"};
  mr_ghost.m_position = {50.f, 50.f};

  steamrot::GrimoireMachina grimoire = MakeGrimoireWithFragment("stone");

  REQUIRE_NOTHROW(steamrot::logic::render::ghost::DrawGhostItem(
      texture, mr_ghost, grimoire));

  texture.display();
  const sf::Image image = texture.getTexture().copyToImage();

  // The centre pixel should not be black — a cyan circle was drawn there
  REQUIRE(image.getPixel({50, 50}) != sf::Color::Black);
}

TEST_CASE(
    "DrawGhostItem with FragmentTag produces a cyan-tinted pixel at position",
    "[unit][render_ghost]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"arm"};
  mr_ghost.m_position = {50.f, 50.f};

  steamrot::GrimoireMachina grimoire = MakeGrimoireWithFragment("arm");
  steamrot::logic::render::ghost::DrawGhostItem(texture, mr_ghost, grimoire);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();
  sf::Color centre = image.getPixel({50, 50});

  // Cyan has G > 0 and B > 0, R == 0.  Verify the hue roughly matches.
  REQUIRE(centre.g > 0);
  REQUIRE(centre.b > 0);
}

/////////////////////////////////////////////////
/// DrawGhostItem — JointTag selection
/////////////////////////////////////////////////

TEST_CASE("DrawGhostItem draws a yellow shape when a JointTag is selected",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"hinge"};
  mr_ghost.m_position = {50.f, 50.f};

  steamrot::GrimoireMachina grimoire = MakeGrimoireWithJoint("hinge");

  REQUIRE_NOTHROW(steamrot::logic::render::ghost::DrawGhostItem(
      texture, mr_ghost, grimoire));

  texture.display();
  const sf::Image image = texture.getTexture().copyToImage();

  // The centre pixel should not be black — a yellow circle was drawn there
  REQUIRE(image.getPixel({50, 50}) != sf::Color::Black);
}

TEST_CASE(
    "DrawGhostItem with JointTag produces a yellow-tinted pixel at position",
    "[unit][render_ghost]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"pivot"};
  mr_ghost.m_position = {50.f, 50.f};

  steamrot::GrimoireMachina grimoire = MakeGrimoireWithJoint("pivot");
  steamrot::logic::render::ghost::DrawGhostItem(texture, mr_ghost, grimoire);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();
  sf::Color centre = image.getPixel({50, 50});

  // Yellow has R > 0 and G > 0, B == 0.
  REQUIRE(centre.r > 0);
  REQUIRE(centre.g > 0);
}

/////////////////////////////////////////////////
/// DrawGhostItem — position is respected
/////////////////////////////////////////////////

TEST_CASE("DrawGhostItem draws at the position stored in MrGhost",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{200, 200}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"rock"};
  mr_ghost.m_position = {150.f, 150.f}; // far corner

  steamrot::GrimoireMachina grimoire = MakeGrimoireWithFragment("rock");
  steamrot::logic::render::ghost::DrawGhostItem(texture, mr_ghost, grimoire);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();

  // Near the specified position there should be pixels
  REQUIRE(image.getPixel({150, 150}) != sf::Color::Black);
  // Near the origin (0,0) there should be nothing
  REQUIRE(image.getPixel({5, 5}) == sf::Color::Black);
}
