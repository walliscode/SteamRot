///////////////////////////////////////////////
/// @file
/// @brief Regression tests spanning placement, collision, and render behavior
///        for GrimoireMachina ghost sockets.
///////////////////////////////////////////////

///////////////////////////////////////////////
/// Headers
///////////////////////////////////////////////
#include "action_grimoire_machina.h"
#include "collision_grimoire_machina.h"
#include "grimoire_machina_test_helpers.h"
#include "render_ghost.h"
#include "render_grimoire_machina.h"
#include "test_fonts.h"
#include <SFML/Graphics.hpp>
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

TEST_CASE("Placing a first piece leaves the ghost active, and ghost render "
          "still draws socket pixels",
          "[unit][combo][regression][grimoire_machina]") {
  sf::RenderTexture texture{{200, 200}};
  texture.clear(sf::Color::Black);

  steamrot::GrimoireMachina grimoire =
      steamrot::tests::MakeGrimoireWithFragmentAndSocket("frag", {25.f, 10.f});
  grimoire.m_scaffold_form = std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance.emplace<FragmentInstance>(0,
                                                grimoire.m_all_fragments["frag"]);

  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(grimoire,
                                                                     mr_ghost);

  REQUIRE(grimoire.m_scaffold_form->parts.size() == 1);
  REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(mr_ghost.m_instance));

  steamrot::logic::render::ghost::draw_ghost_item(texture, mr_ghost);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();
  REQUIRE(image.getPixel({25, 10}) != sf::Color::Black);
}

TEST_CASE("Collision, placement, scaffold render, and ghost render preserve "
          "the ghost-socket regression chain",
          "[unit][combo][regression][grimoire_machina]") {
  auto font_result = ProvideDefaultFont();
  if (!font_result) {
    FAIL("Failed to load default font: " + font_result.error().message);
  }
  const sf::Font &font = font_result.value();

  steamrot::GrimoireMachina grimoire =
      steamrot::tests::MakeGrimoireWithFragmentAndSocket("fragment",
                                                         {25.f, 10.f});
  steamrot::GrimoireMachina joint_source =
      steamrot::tests::MakeGrimoireWithJointAndSocket("joint");
  grimoire.m_all_joints = joint_source.m_all_joints;
  grimoire.m_scaffold_form = std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::MrGhost first_ghost;
  first_ghost.m_instance.emplace<FragmentInstance>(
      0, grimoire.m_all_fragments["fragment"]);
  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
      grimoire, first_ghost);
  REQUIRE(grimoire.m_scaffold_form->parts.size() == 1);

  steamrot::MrGhost second_ghost;
  second_ghost.m_instance.emplace<JointInstance>(
      0, grimoire.m_all_joints["joint"]);
  std::get<JointInstance>(second_ghost.m_instance).setPosition({0.f, 10.f});

  steamrot::logic::collision::grimoire_machina::
      check_collisions_between_ghost_and_scaffold(*grimoire.m_scaffold_form,
                                                  second_ghost);

  const auto &ghost_joint =
      std::get<steamrot::JointInstance>(second_ghost.m_instance);
  const auto &placed_fragment =
      std::get<steamrot::FragmentInstance>(grimoire.m_scaffold_form->parts.at(
          0));

  REQUIRE(ghost_joint.GetSockets().at(0).IsWithinConnectionDistance());
  REQUIRE(placed_fragment.GetSockets().at(0).IsWithinConnectionDistance());

  REQUIRE(steamrot::logic::action::grimoire_machina::
              check_MrGhost_for_connection_readiness(second_ghost)
                  .has_value());
  REQUIRE(steamrot::logic::action::grimoire_machina::
              check_PartGraph_for_connection_readiness(
                  grimoire.m_scaffold_form->parts)
                  .has_value());

  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
      grimoire, second_ghost);

  REQUIRE(grimoire.m_scaffold_form->parts.size() == 2);
  REQUIRE(
      std::holds_alternative<steamrot::JointInstance>(second_ghost.m_instance));

  sf::RenderTexture scaffold_texture{{200, 200}};
  scaffold_texture.clear(sf::Color::Black);
  steamrot::logic::render::grimoire_machina::render_machina_form(
      scaffold_texture, grimoire, font);
  scaffold_texture.display();

  const sf::Image scaffold_image = scaffold_texture.getTexture().copyToImage();
  REQUIRE(scaffold_image.getPixel({10, 10}) != sf::Color::Black);

  sf::RenderTexture ghost_texture{{200, 200}};
  ghost_texture.clear(sf::Color::Black);
  steamrot::logic::render::ghost::draw_ghost_item(ghost_texture, second_ghost);
  ghost_texture.display();

  const sf::Image ghost_image = ghost_texture.getTexture().copyToImage();
  REQUIRE(ghost_image.getPixel({25, 10}) != sf::Color::Black);
}

} // namespace steamrot::tests
