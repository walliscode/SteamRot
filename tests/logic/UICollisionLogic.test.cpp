/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for UICollisionLogic class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UICollisionLogic.h"
#include "ArchetypeManager.h"
#include "PanelElement.h"
#include "TestContext.h"
#include "emp_helpers.h"
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UICollisionLogic::UICollisionLogic Constructor",
          "[UICollisionLogic]") {
  // Create a dummy LogicContext
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;
  // Instantiate UICollisionLogic
  steamrot::UICollisionLogic ui_collision_logic(
      test_context.GetLogicContextForTestScene());
  SUCCEED("UICollisionLogic instantiated successfully");
}

TEST_CASE("UICollisionLogic::RunLogic changes is_mouse_over to true if mouse "
          "over element"
          "[UICollisionLogic]") {
  // Create a dummy LogicContext
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;

  // pull out the LogicContext and GameContext references
  auto &game_context = test_context.GetGameContext();
  auto logic_context = test_context.GetLogicContextForTestScene();

  // Get all active CUserInterface entities
  ArchetypeID archetype_id =
      steamrot::GenerateArchetypeIDfromTypes<steamrot::CUserInterface>();

  // check if the archetype exists
  auto const it = logic_context.archetypes.find(archetype_id);
  REQUIRE(it != logic_context.archetypes.end());

  // assign the archetype
  steamrot::Archetype archetype = it->second;
  // check the archetype has at least one entity
  REQUIRE(archetype.size() > 0);
  // get the first entity id
  size_t entity_id = archetype[0];

  // get the CUserInterface component
  steamrot::CUserInterface &ui_component =
      steamrot::emp_helpers::GetComponent<steamrot::CUserInterface>(
          entity_id, logic_context.scene_entities);

  // pull out the root element
  steamrot::PanelElement *panel_element =
      dynamic_cast<steamrot::PanelElement *>(ui_component.m_root_element.get());
  REQUIRE(panel_element != nullptr);

  // calculate the center of the button
  sf::Vector2f panel_position = panel_element->position;
  sf::Vector2f panel_size = panel_element->size;
  sf::Vector2f panel_center = panel_position + (panel_size / 2.0f);

  // move the mouse to the button center
  game_context.mouse_position = sf::Vector2i(panel_center);

  CAPTURE(logic_context.mouse_position.x, logic_context.mouse_position.y);
  CAPTURE(game_context.mouse_position.x, game_context.mouse_position.y);

  REQUIRE(logic_context.mouse_position == game_context.mouse_position);
  // check that the button is not hovered
  REQUIRE(panel_element->is_mouse_over == false);
  // Instantiate UICollisionLogic with the test context
  steamrot::UICollisionLogic ui_collision_logic(logic_context);
  // Call the RunLogic method
  ui_collision_logic.RunLogic();
  // check that the button is now hovered
  CAPTURE(logic_context.mouse_position.x, logic_context.mouse_position.y);
  CAPTURE(panel_position.x, panel_position.y);

  REQUIRE(panel_element->is_mouse_over == true);
}
