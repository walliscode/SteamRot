/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the render_ghost free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "grimoire_machina_test_helpers.h"
#include "render_ghost.h"
#include "GrimoireMachina.h"
#include "MrGhost.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <catch2/catch_test_macros.hpp>
#include <vector>

// ---------------------------------------------------------------------------
// Coordinate reference used by socket tests
//
// MakeFilledSquare(0, 0, White) produces a 20×20 square with:
//   bounds.position = (0, 0)   bounds.size = (20, 20)
//
// draw_ghost_item computes:
//   translate = mr_ghost.m_position - bounds.position - bounds.size - (5, 5)
//
// With mr_ghost.m_position = (100, 100):
//   translate = (100-0-20-5, 100-0-20-5) = (75, 75)
//
// A socket at local position (25, 10) therefore lands at texture (100, 85),
// which is outside the white square [75..95, 75..95] so the background there
// is pure black before the socket circle is painted.
// ---------------------------------------------------------------------------



/////////////////////////////////////////////////
/// draw_ghost_item — monostate (nothing selected)
/////////////////////////////////////////////////

TEST_CASE("draw_ghost_item draws nothing when selection is monostate",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_selection));

  steamrot::GrimoireMachina grimoire;

  REQUIRE_NOTHROW(steamrot::logic::render::ghost::draw_ghost_item(
      texture, mr_ghost, grimoire));

  texture.display();
  const sf::Image image = texture.getTexture().copyToImage();

  // Entire texture should remain black — nothing was drawn
  REQUIRE(image.getPixel({50, 50}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// draw_ghost_item — FragmentTag: key not in grimoire
/////////////////////////////////////////////////

TEST_CASE("draw_ghost_item does not throw when FragmentTag key is not found",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"missing"};
  mr_ghost.m_position = {50.f, 50.f};

  steamrot::GrimoireMachina grimoire; // empty — no fragments

  REQUIRE_NOTHROW(steamrot::logic::render::ghost::draw_ghost_item(
      texture, mr_ghost, grimoire));

  texture.display();
  const sf::Image image = texture.getTexture().copyToImage();
  REQUIRE(image.getPixel({50, 50}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// draw_ghost_item — FragmentTag: empty view draws nothing
/////////////////////////////////////////////////

TEST_CASE("draw_ghost_item draws nothing when the fragment view is empty",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"stone"};
  mr_ghost.m_position = {50.f, 50.f};

  // fragment exists but has an empty movement_views
  steamrot::GrimoireMachina grimoire = steamrot::tests::MakeGrimoireWithEmptyFragment("stone");

  REQUIRE_NOTHROW(steamrot::logic::render::ghost::draw_ghost_item(
      texture, mr_ghost, grimoire));

  texture.display();
  const sf::Image image = texture.getTexture().copyToImage();
  REQUIRE(image.getPixel({50, 50}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// draw_ghost_item — FragmentTag: populated view draws pixels at cursor
/////////////////////////////////////////////////

TEST_CASE("draw_ghost_item uses draw_view to draw at the cursor position",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{200, 200}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"arm"};
  mr_ghost.m_position = {50.f, 50.f};

  // fragment has a 20x20 white square at local origin.
  // translate = m_position(50,50) - bounds.position(0,0) - bounds.size(20,20)
  //             - corner_offset(5,5) = (25,25).  Square lands at
  //             [25..45, 25..45].
  steamrot::GrimoireMachina grimoire = steamrot::tests::MakeGrimoireWithPopulatedFragment("arm");

  steamrot::logic::render::ghost::draw_ghost_item(texture, mr_ghost, grimoire);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();

  // Centre of the translated square should be white
  REQUIRE(image.getPixel({35, 35}) == sf::Color::White);
  // A point well outside the translated square should be black
  REQUIRE(image.getPixel({5, 5}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// draw_ghost_item — JointTag: key not in grimoire
/////////////////////////////////////////////////

TEST_CASE("draw_ghost_item does not throw when JointTag key is not found",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"missing"};
  mr_ghost.m_position = {50.f, 50.f};

  steamrot::GrimoireMachina grimoire; // empty — no joints

  REQUIRE_NOTHROW(steamrot::logic::render::ghost::draw_ghost_item(
      texture, mr_ghost, grimoire));

  texture.display();
  const sf::Image image = texture.getTexture().copyToImage();
  REQUIRE(image.getPixel({50, 50}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// draw_ghost_item — JointTag: empty view draws nothing
/////////////////////////////////////////////////

TEST_CASE("draw_ghost_item draws nothing when the joint view is empty",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"hinge"};
  mr_ghost.m_position = {50.f, 50.f};

  steamrot::GrimoireMachina grimoire = steamrot::tests::MakeGrimoireWithEmptyJoint("hinge");

  REQUIRE_NOTHROW(steamrot::logic::render::ghost::draw_ghost_item(
      texture, mr_ghost, grimoire));

  texture.display();
  const sf::Image image = texture.getTexture().copyToImage();
  REQUIRE(image.getPixel({50, 50}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// draw_ghost_item — position is respected
/////////////////////////////////////////////////

TEST_CASE(
    "draw_ghost_item renders fragment geometry at the stored cursor position",
    "[unit][render_ghost]") {
  sf::RenderTexture texture{{300, 300}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"rock"};
  mr_ghost.m_position = {200.f, 200.f}; // far corner

  steamrot::GrimoireMachina grimoire =
      steamrot::tests::MakeGrimoireWithPopulatedFragment("rock");
  steamrot::logic::render::ghost::draw_ghost_item(texture, mr_ghost, grimoire);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();

  // translate = (200,200) - (0,0) - (20,20) - (5,5) = (175,175)
  // Centre of the translated square [175..195, 175..195]
  REQUIRE(image.getPixel({185, 185}) == sf::Color::White);
  // Near origin — nothing
  REQUIRE(image.getPixel({5, 5}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// draw_ghost_item — sockets: FragmentTag, socket drawn outside geometry
/////////////////////////////////////////////////

TEST_CASE("draw_ghost_item draws socket circle for a FragmentTag selection",
          "[unit][render_ghost]") {
  // Texture large enough to contain geometry + socket
  sf::RenderTexture texture{{200, 200}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"leaf"};
  mr_ghost.m_position = {100.f, 100.f};

  // Socket at local (25, 10) → texture (100, 85): outside the white square
  // [75..95, 75..95] so the background there is black before the socket is
  // drawn.
  steamrot::GrimoireMachina grimoire =
      steamrot::tests::MakeGrimoireWithFragmentAndSocket("leaf", {25.f, 10.f});

  steamrot::logic::render::ghost::draw_ghost_item(texture, mr_ghost, grimoire);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();

  // The socket circle centre must have been painted (not black)
  REQUIRE(image.getPixel({100, 85}) != sf::Color::Black);
  // A point well away from both geometry and socket must remain black
  REQUIRE(image.getPixel({10, 10}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// draw_ghost_item — sockets: FragmentTag, no sockets → socket area stays black
/////////////////////////////////////////////////

TEST_CASE("draw_ghost_item draws no socket pixels when fragment sockets list "
          "is empty",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{200, 200}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"bare"};
  mr_ghost.m_position = {100.f, 100.f};

  // Fragment with geometry but NO sockets
  steamrot::GrimoireMachina grimoire =
      steamrot::tests::MakeGrimoireWithPopulatedFragment("bare");

  steamrot::logic::render::ghost::draw_ghost_item(texture, mr_ghost, grimoire);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();

  // Position (100, 85) is where a socket at local (25, 10) would have been
  // painted — with no sockets it must remain black.
  REQUIRE(image.getPixel({100, 85}) == sf::Color::Black);
}
