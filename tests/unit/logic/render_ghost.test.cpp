/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the render_ghost free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_ghost.h"
#include "Fragment.h"
#include "Joint.h"
#include "MrGhost.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Angle.hpp>
#include <catch2/catch_test_macros.hpp>
#include <vector>

namespace steamrot::tests {
namespace {

/////////////////////////////////////////////////
/// @brief Make a filled 20x20 square VertexArray (Triangles) starting at (x,
/// y).
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
/// @brief Build a Fragment with no view geometry.
/////////////////////////////////////////////////
steamrot::Fragment MakeEmptyFragment(const std::string &name) {
  steamrot::Fragment fragment;
  fragment.name = name;
  // positioning_views is empty by default
  return fragment;
}

/////////////////////////////////////////////////
/// @brief Build a Fragment whose Front view is a solid white 20x20 square at
///        the origin.
/////////////////////////////////////////////////
steamrot::Fragment MakePopulatedFragment(const std::string &name) {
  steamrot::Fragment fragment;
  fragment.name = name;
  fragment.positioning_views.insert_or_assign(
      steamrot::ViewDirection::Front,
      MakeFilledSquare(0.f, 0.f, sf::Color::White));
  return fragment;
}

/////////////////////////////////////////////////
/// @brief Build a Joint with no view geometry.
/////////////////////////////////////////////////
steamrot::Joint MakeEmptyJoint(const std::string &name) {
  steamrot::Joint joint;
  joint.name = name;
  return joint;
}

/////////////////////////////////////////////////
/// @brief Build a Joint whose Front view is a solid white 20x20 square at the
///        origin.
/////////////////////////////////////////////////
steamrot::Joint MakePopulatedJoint(const std::string &name) {
  steamrot::Joint joint;
  joint.name = name;
  joint.positioning_views.insert_or_assign(
      steamrot::ViewDirection::Front,
      MakeFilledSquare(0.f, 0.f, sf::Color::White));
  return joint;
}

} // anonymous namespace

/////////////////////////////////////////////////
/// draw_ghost_item — monostate (nothing selected)
/////////////////////////////////////////////////

TEST_CASE("draw_ghost_item draws nothing when instance is monostate",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  steamrot::MrGhost mr_ghost;
  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_instance));

  REQUIRE_NOTHROW(
      steamrot::logic::render::ghost::draw_ghost_item(texture, mr_ghost));

  texture.display();
  const sf::Image image = texture.getTexture().copyToImage();

  // Entire texture should remain black — nothing was drawn
  REQUIRE(image.getPixel({50, 50}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// draw_ghost_item — FragmentInstance: empty view draws nothing
/////////////////////////////////////////////////

TEST_CASE("draw_ghost_item draws nothing when the fragment view is empty",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  steamrot::Fragment fragment = MakeEmptyFragment("stone");
  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance.emplace<FragmentInstance>(0, fragment);

  REQUIRE_NOTHROW(
      steamrot::logic::render::ghost::draw_ghost_item(texture, mr_ghost));

  texture.display();
  const sf::Image image = texture.getTexture().copyToImage();
  REQUIRE(image.getPixel({50, 50}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// draw_ghost_item — FragmentInstance: populated view draws pixels
/////////////////////////////////////////////////

TEST_CASE("draw_ghost_item draws fragment geometry at the instance transform",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{200, 200}};
  texture.clear(sf::Color::Black);

  // Fragment has a 20x20 white square at local origin.
  // Set transform to translate(25, 25) — square lands at [25..45, 25..45].
  Fragment fragment = MakePopulatedFragment("arm");
  FragmentInstance instance{0, fragment};
  instance.setRotation(sf::degrees(0));
  instance.setPosition({0.f, 0.f});
  instance.move({25.f, 25.f});

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance.emplace<FragmentInstance>(instance);

  steamrot::logic::render::ghost::draw_ghost_item(texture, mr_ghost);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();

  // Centre of the translated square should be white
  REQUIRE(image.getPixel({35, 35}) == sf::Color::White);
  // A point well outside the translated square should be black
  REQUIRE(image.getPixel({5, 5}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// draw_ghost_item — JointInstance: empty view draws nothing
/////////////////////////////////////////////////

TEST_CASE("draw_ghost_item draws nothing when the joint view is empty",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  steamrot::Joint joint = MakeEmptyJoint("hinge");
  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance.emplace<JointInstance>(0, joint);

  REQUIRE_NOTHROW(
      steamrot::logic::render::ghost::draw_ghost_item(texture, mr_ghost));

  texture.display();
  const sf::Image image = texture.getTexture().copyToImage();
  REQUIRE(image.getPixel({50, 50}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// draw_ghost_item — position is respected (far corner)
/////////////////////////////////////////////////

TEST_CASE("draw_ghost_item renders fragment geometry at a far-corner position",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{300, 300}};
  texture.clear(sf::Color::Black);

  steamrot::Fragment fragment = MakePopulatedFragment("rock");
  steamrot::FragmentInstance instance{0, fragment};

  // translate(175, 175) — square lands at [175..195, 175..195]
  instance.move({175.f, 175.f});

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance.emplace<FragmentInstance>(instance);

  steamrot::logic::render::ghost::draw_ghost_item(texture, mr_ghost);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();

  // Centre of the translated square [175..195, 175..195]
  REQUIRE(image.getPixel({185, 185}) == sf::Color::White);
  // Near origin — nothing
  REQUIRE(image.getPixel({5, 5}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// draw_ghost_item — sockets: socket drawn at transform-mapped position
/////////////////////////////////////////////////

TEST_CASE("draw_ghost_item draws socket circle for a FragmentInstance",
          "[unit][render_ghost]") {
  // Texture large enough to contain geometry + socket
  sf::RenderTexture texture{{200, 200}};
  texture.clear(sf::Color::Black);

  // Fragment with a 20x20 white square and one socket at local (25, 10)
  steamrot::Fragment fragment = MakePopulatedFragment("leaf");
  steamrot::SocketData socket{{25.f, 10.f}, {1.f, 0.f}};
  fragment.sockets.push_back(socket);

  steamrot::FragmentInstance instance{0, fragment};

  // translate(75, 75): square at [75..95, 75..95]
  // socket at world = transform.transformPoint(25, 10) = (100, 85)
  instance.move({75.f, 75.f});

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance.emplace<FragmentInstance>(instance);

  steamrot::logic::render::ghost::draw_ghost_item(texture, mr_ghost);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();

  // The socket circle centre must have been painted (not black)
  REQUIRE(image.getPixel({100, 85}) != sf::Color::Black);
  // A point well away from both geometry and socket must remain black
  REQUIRE(image.getPixel({10, 10}) == sf::Color::Black);
}

/////////////////////////////////////////////////
/// draw_ghost_item — sockets: no sockets → socket area stays black
/////////////////////////////////////////////////

TEST_CASE("draw_ghost_item draws no socket pixels when fragment has no sockets",
          "[unit][render_ghost]") {
  sf::RenderTexture texture{{200, 200}};
  texture.clear(sf::Color::Black);

  // Fragment with geometry but NO sockets
  steamrot::Fragment fragment = MakePopulatedFragment("bare");
  steamrot::FragmentInstance instance{0, fragment};

  instance.move({75.f, 75.f}); // translate(75, 75): square at [75..95, 75..95]

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance.emplace<FragmentInstance>(instance);

  steamrot::logic::render::ghost::draw_ghost_item(texture, mr_ghost);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();

  // Position (100, 85) is where a socket at local (25, 10) would have been
  // painted — with no sockets it must remain black.
  REQUIRE(image.getPixel({100, 85}) == sf::Color::Black);
}
} // namespace steamrot::tests
