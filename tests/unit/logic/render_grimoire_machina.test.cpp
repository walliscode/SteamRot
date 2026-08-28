
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
