
//////////////////
/// @file
/// @brief Unit tests for the functions related to the rendering of the grimoire
/// machina.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_grimoire_machina.h"
#include "ColorEqualsMatcher.h"
#include "GrimoireMachina.h"
#include "MachinaFormScaffold.h"
#include "action_grimoire_machina.h"
#include "grimoire_machina_test_helpers.h"
#include "test_fonts.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

using namespace steamrot::logic::render::grimoire_machina;

TEST_CASE("draw_view draws a populated view without throwing",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  auto views = steamrot::tests::MakeViewsWithFrontTriangle();

  REQUIRE_NOTHROW(steamrot::logic::render::grimoire_machina::draw_view(
      texture, views, steamrot::ViewDirection::Front));
}

TEST_CASE("draw_view draws an empty VertexArray without throwing",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  steamrot::Views views; // no entries populated

  REQUIRE_NOTHROW(steamrot::logic::render::grimoire_machina::draw_view(
      texture, views, steamrot::ViewDirection::Front));
}

TEST_CASE("draw_view produces pixels for a Front-direction triangle",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  auto views = steamrot::tests::MakeViewsWithFrontTriangle(sf::Color::Red);
  steamrot::logic::render::grimoire_machina::draw_view(
      texture, views, steamrot::ViewDirection::Front);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();

  sf::Color centroid_pixel = image.getPixel({15, 15});
  REQUIRE(centroid_pixel == sf::Color::Red);
}

TEST_CASE("draw_view selects the correct direction slot",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{10.f, 10.f}, sf::Color::White});
  va.append(sf::Vertex{sf::Vector2f{20.f, 10.f}, sf::Color::White});
  va.append(sf::Vertex{sf::Vector2f{15.f, 20.f}, sf::Color::White});

  steamrot::Views views;
  views.insert_or_assign(steamrot::ViewDirection::Back, std::move(va));

  steamrot::logic::render::grimoire_machina::draw_view(
      texture, views, steamrot::ViewDirection::Front);
  texture.display();
  sf::Image image_front = texture.getTexture().copyToImage();
  REQUIRE(image_front.getPixel({15, 15}) == sf::Color::Black);

  texture.clear(sf::Color::Black);
  steamrot::logic::render::grimoire_machina::draw_view(
      texture, views, steamrot::ViewDirection::Back);
  texture.display();
  sf::Image image_back = texture.getTexture().copyToImage();
  REQUIRE(image_back.getPixel({15, 15}) == sf::Color::White);
}

TEST_CASE("draw_view with RenderStates draws Fragment positioning_views at "
          "transformed position",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  auto fragment = steamrot::tests::MakeFragmentWithFrontView(sf::Color::Green);

  sf::RenderStates states;
  states.transform.translate({10.f, 10.f});
  steamrot::logic::render::grimoire_machina::draw_view(
      texture, fragment.positioning_views, steamrot::ViewDirection::Front,
      states);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();
  REQUIRE(image.getPixel({25, 22}) == sf::Color::Green);
}

TEST_CASE("draw_view with RenderStates draws Joint positioning_views at "
          "transformed position",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  auto joint = steamrot::tests::MakeJointWithFrontView(sf::Color::Blue);

  sf::RenderStates states;
  states.transform.translate({10.f, 10.f});
  steamrot::logic::render::grimoire_machina::draw_view(
      texture, joint.positioning_views, steamrot::ViewDirection::Front, states);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();
  REQUIRE(image.getPixel({30, 27}) == sf::Color::Blue);
}

/////////////////////////////////////////////////
/// render_machina_form smoke tests
/////////////////////////////////////////////////

TEST_CASE("render_machina_form draws without throwing when no scaffold is "
          "active",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  steamrot::GrimoireMachina grimoire_machina;
  auto font_result = ProvideDefaultFont();
  if (!font_result) {
    FAIL("Failed to load default font: " + font_result.error().message);
  }
  sf::Font &font = font_result.value();

  REQUIRE_NOTHROW(
      steamrot::logic::render::grimoire_machina::render_machina_form(
          texture, grimoire_machina, font));
}

TEST_CASE("render_machina_form draws without throwing when scaffold has placed "
          "fragments",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};

  auto font_result = ProvideDefaultFont();
  if (!font_result) {
    FAIL("Failed to load default font: " + font_result.error().message);
  }
  sf::Font &font = font_result.value();

  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_all_fragments["frag"] =
      steamrot::tests::MakeFragmentWithOriginTriangle();

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance.emplace<FragmentInstance>(
      0, grimoire_machina.m_all_fragments["frag"]);

  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
      grimoire_machina, mr_ghost);

  REQUIRE_NOTHROW(
      steamrot::logic::render::grimoire_machina::render_machina_form(
          texture, grimoire_machina, font));
}

TEST_CASE("render_machina_form draws without throwing when scaffold has placed "
          "joints",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};

  auto font_result = ProvideDefaultFont();
  if (!font_result) {
    FAIL("Failed to load default font: " + font_result.error().message);
  }
  sf::Font &font = font_result.value();

  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_all_joints["joint"] =
      steamrot::tests::MakeJointWithOriginTriangle();

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance.emplace<JointInstance>(
      0, grimoire_machina.m_all_joints["joint"]);

  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
      grimoire_machina, mr_ghost);

  REQUIRE_NOTHROW(
      steamrot::logic::render::grimoire_machina::render_machina_form(
          texture, grimoire_machina, font));
}

TEST_CASE("draw_status_box tests", "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{200, 100}};
  texture.clear(sf::Color::Black);

  auto font_result = ProvideDefaultFont();
  if (!font_result) {
    FAIL("Failed to load default font: " + font_result.error().message);
  }
  const sf::Font &font = font_result.value();

  SECTION("draw_status_box draws a red box with text") {
    sf::FloatRect box{{10.f, 10.f}, {180.f, 80.f}};
    sf::Color color = sf::Color::Red;
    std::string text = "Test Status";

    REQUIRE_NOTHROW(draw_status_box(box, color, text, font, texture));
    texture.display();
    sf::Image image = texture.getTexture().copyToImage();

    REQUIRE_THAT(image.getPixel({10, 10}), EqualsColor(sf::Color::Red));
    REQUIRE_THAT(image.getPixel({187, 10}), EqualsColor(sf::Color::Red));
    REQUIRE_THAT(image.getPixel({10, 87}), EqualsColor(sf::Color::Red));
    REQUIRE_THAT(image.getPixel({187, 87}), EqualsColor(sf::Color::Red));
  }
}

} // namespace steamrot::tests
