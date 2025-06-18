#include "UICollisionLogic.h"
#include "EntityHelpers.h"

namespace steamrot {
/////////////////////////////////////////////////
UICollisionLogic::UICollisionLogic(const LogicContext logic_context)
    : Logic<CUserInterface>(logic_context) {}

/////////////////////////////////////////////////
void UICollisionLogic::ProcessLogic() {

  std::cout << "Processing UI Collision Logic" << std::endl;

  // cycle through all the Archetype IDs  associated with this logic class
  for (const ArchetypeID &archetype_id : m_archetype_IDs) {

    // if it is not in the archetyps map, then skip
    if (!m_logic_context.archetypes.contains(archetype_id)) {
      continue;
    } else {
      // get the archetype from the map
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
}

/////////////////////////////////////////////////
void UICollisionLogic::CheckMouseCollision(CUserInterface &ui_component) {

  // recursively check the root element for mouse collision
  RecursiveCheckMouseCollision(ui_component.m_root_element);
}

/////////////////////////////////////////////////
bool UICollisionLogic::RecursiveCheckMouseCollision(UIElement &element) {

  // lamda function to check if mouse is within bounds of the element
  bool is_mouse_over = [&]() -> bool {
    // Assuming we have a way to get the mouse position
    sf::Vector2i mouse_position = sf::Mouse::getPosition();
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

    // recursively check child elements
    for (auto &child : element.child_elements) {

      // if mouse is over a child element, we set the mouse_over_child to
      // true and mouse_over to false
      if (RecursiveCheckMouseCollision(child)) {
        element.mouse_over_child = true;
        element.mouse_over = false;
      }
    }
    return true;

    // check is mouse moves aways from child element

  } else if (!is_mouse_over && element.mouse_over_child) {
    // if mouse is not over the element, we set mouse_over to false
    element.mouse_over = false;
    element.mouse_over_child = false;
    // reset child elements mouse_over state
    for (auto &child : element.child_elements) {
      child.mouse_over = false;
      child.mouse_over_child = false;
    }
    return false;
  }

  return false;
}

} // namespace steamrot
