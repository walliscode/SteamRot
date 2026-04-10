/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the functions related to the rendering of the grimoire
/// machina.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_grimoire_machina.h"
#include "Fragment.h"
#include "Joint.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <catch2/catch_test_macros.hpp>

namespace {

/////////////////////////////////////////////////
/// @brief Helper: build a Views object with a single coloured triangle in the
/// Front slot.
/////////////////////////////////////////////////
steamrot::Views MakeViewsWithFrontTriangle(sf::Color colour = sf::Color::Red) {
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{10.f, 10.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{20.f, 10.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{15.f, 20.f}, colour});

  steamrot::Views views;
  views.insert_or_assign(steamrot::ViewDirection::Front, std::move(va));
  return views;
}

/////////////////////////////////////////////////
/// @brief Helper: build a Fragment with a coloured triangle in the Front slot.
/////////////////////////////////////////////////
steamrot::Fragment MakeFragmentWithFrontView(
    sf::Color colour = sf::Color::Green) {
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{5.f, 5.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{25.f, 5.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{15.f, 25.f}, colour});

  steamrot::Fragment fragment;
  fragment.movement_views.insert_or_assign(steamrot::ViewDirection::Front,
                                           std::move(va));
  return fragment;
}

/////////////////////////////////////////////////
/// @brief Helper: build a Joint with a coloured triangle in the Front slot.
/////////////////////////////////////////////////
steamrot::Joint MakeJointWithFrontView(sf::Color colour = sf::Color::Blue) {
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{10.f, 10.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{30.f, 10.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{20.f, 30.f}, colour});

  steamrot::Joint joint;
  joint.movement_views.insert_or_assign(steamrot::ViewDirection::Front,
                                        std::move(va));
  return joint;
}

} // anonymous namespace

/////////////////////////////////////////////////
/// DrawView tests
/////////////////////////////////////////////////

TEST_CASE("DrawView draws a populated view without throwing",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  auto views = MakeViewsWithFrontTriangle();

  REQUIRE_NOTHROW(
      steamrot::logic::render::grimoire_machina::DrawView(
          texture, views, steamrot::ViewDirection::Front));
}

TEST_CASE("DrawView draws an empty VertexArray without throwing",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  steamrot::Views views; // no entries populated

  REQUIRE_NOTHROW(
      steamrot::logic::render::grimoire_machina::DrawView(
          texture, views, steamrot::ViewDirection::Front));
}

TEST_CASE("DrawView produces pixels for a Front-direction triangle",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  auto views = MakeViewsWithFrontTriangle(sf::Color::Red);
  steamrot::logic::render::grimoire_machina::DrawView(
      texture, views, steamrot::ViewDirection::Front);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();

  // The triangle spans roughly x=[10,20], y=[10,20]; the centroid pixel
  // (15,15) should be red after drawing.
  sf::Color centroid_pixel = image.getPixel({15, 15});
  REQUIRE(centroid_pixel == sf::Color::Red);
}

TEST_CASE("DrawView selects the correct direction slot",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  // Populate only Back slot
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{10.f, 10.f}, sf::Color::White});
  va.append(sf::Vertex{sf::Vector2f{20.f, 10.f}, sf::Color::White});
  va.append(sf::Vertex{sf::Vector2f{15.f, 20.f}, sf::Color::White});

  steamrot::Views views;
  views.insert_or_assign(steamrot::ViewDirection::Back, std::move(va));

  // Drawing Front (unpopulated) should produce no visible pixels
  steamrot::logic::render::grimoire_machina::DrawView(
      texture, views, steamrot::ViewDirection::Front);
  texture.display();
  sf::Image image_front = texture.getTexture().copyToImage();
  REQUIRE(image_front.getPixel({15, 15}) == sf::Color::Black);

  // Drawing Back should produce white pixels
  texture.clear(sf::Color::Black);
  steamrot::logic::render::grimoire_machina::DrawView(
      texture, views, steamrot::ViewDirection::Back);
  texture.display();
  sf::Image image_back = texture.getTexture().copyToImage();
  REQUIRE(image_back.getPixel({15, 15}) == sf::Color::White);
}

/////////////////////////////////////////////////
/// DrawFragmentView tests
/////////////////////////////////////////////////

TEST_CASE("DrawFragmentView draws a Fragment view without throwing",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  auto fragment = MakeFragmentWithFrontView();

  REQUIRE_NOTHROW(
      steamrot::logic::render::grimoire_machina::DrawFragmentView(
          texture, fragment, steamrot::ViewDirection::Front));
}

TEST_CASE("DrawFragmentView produces pixels for a populated Front view",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  auto fragment = MakeFragmentWithFrontView(sf::Color::Green);
  steamrot::logic::render::grimoire_machina::DrawFragmentView(
      texture, fragment, steamrot::ViewDirection::Front);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();
  // Centroid of the triangle at (5,5),(25,5),(15,25) = (15, 11.67) → pixel (15,12)
  sf::Color centroid_pixel = image.getPixel({15, 12});
  REQUIRE(centroid_pixel == sf::Color::Green);
}

TEST_CASE("DrawFragmentView draws an empty Fragment view without throwing",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  steamrot::Fragment fragment; // no views populated

  REQUIRE_NOTHROW(
      steamrot::logic::render::grimoire_machina::DrawFragmentView(
          texture, fragment, steamrot::ViewDirection::Front));
}

/////////////////////////////////////////////////
/// DrawJointView tests
/////////////////////////////////////////////////

TEST_CASE("DrawJointView draws a Joint view without throwing",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  auto joint = MakeJointWithFrontView();

  REQUIRE_NOTHROW(
      steamrot::logic::render::grimoire_machina::DrawJointView(
          texture, joint, steamrot::ViewDirection::Front));
}

TEST_CASE("DrawJointView produces pixels for a populated Front view",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  auto joint = MakeJointWithFrontView(sf::Color::Blue);
  steamrot::logic::render::grimoire_machina::DrawJointView(
      texture, joint, steamrot::ViewDirection::Front);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();
  // Centroid of the triangle at (10,10),(30,10),(20,30) = (20, 16.67) → pixel (20,17)
  sf::Color centroid_pixel = image.getPixel({20, 17});
  REQUIRE(centroid_pixel == sf::Color::Blue);
}

TEST_CASE("DrawJointView draws an empty Joint view without throwing",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  steamrot::Joint joint; // no views populated

  REQUIRE_NOTHROW(
      steamrot::logic::render::grimoire_machina::DrawJointView(
          texture, joint, steamrot::ViewDirection::Front));
}

