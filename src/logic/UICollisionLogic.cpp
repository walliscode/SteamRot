#include "UICollisionLogic.h"
#include "ArchetypeHelpers.h"
#include "CUserInterface.h"
#include "EntityHelpers.h"
#include <SFML/Window/Mouse.hpp>

namespace steamrot {
/////////////////////////////////////////////////
UICollisionLogic::UICollisionLogic(const LogicContext logic_context)
    : BaseLogic(logic_context) {}

/////////////////////////////////////////////////
void UICollisionLogic::ProcessLogic() {

  ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<CUserInterface>();

  // if it is not in the archetyps map, then skip
  if (m_logic_context.archetypes.contains(archetype_id)) {

    Archetype &archetype = m_logic_context.archetypes[archetype_id];

    // cycle through all the entity indexs in the archetype
    for (size_t entity_id : archetype) {

      // get the CUserInterface component
      CUserInterface &ui_component = GetComponent<CUserInterface>(
          entity_id, m_logic_context.scene_entities);

      // group collision logic here
      CheckMouseCollision(ui_component);
    };
  }
}

/////////////////////////////////////////////////
void UICollisionLogic::CheckMouseCollision(CUserInterface &ui_component) {

  // get mouse position relative to the game window
  sf::Vector2i mouse_position =
      sf::Mouse::getPosition(m_logic_context.game_window);
  // recursively check the root element for mouse collision
  RecursiveCheckMouseCollision(ui_component.m_root_element, mouse_position);
}

/////////////////////////////////////////////////
void UICollisionLogic::RecursiveCheckMouseCollision(
    UIElement &element, sf::Vector2i mouse_position) {

  // reset mouse_over and mouse_over_child for the element
  element.mouse_over = false;
  element.mouse_over_child = false;

  // lamda function to check if mouse is within bounds of the element
  bool is_mouse_over = [&]() -> bool {
    // Assuming we have a way to get the mouse position
    return (mouse_position.x >= element.position.x &&
            mouse_position.x <= element.position.x + element.size.x &&
            mouse_position.y >= element.position.y &&
            mouse_position.y <= element.position.y + element.size.y);
  }();

  // check is mouse over the element
  if (is_mouse_over) {

    // set mouse_over to true, reset to false if we are over a child
    // element
    element.mouse_over = true;
  }
  // recursively check child elements
  for (auto &child : element.child_elements) {

    // if mouse is over a child element, we set the mouse_over_child to
    // true and mouse_over to false
    RecursiveCheckMouseCollision(child, mouse_position);
  }

  // adding in mouse_over_child prevents recursive calls every tick
}

void UICollisionLogic::RecursiveResetMouseOver(UIElement &element) {
  // reset mouse_over state for the element
  element.mouse_over = false;
  element.mouse_over_child = false;
  // recursively reset child elements
  for (auto &child : element.child_elements) {
    RecursiveResetMouseOver(child);
  }
}
} // namespace steamrot
