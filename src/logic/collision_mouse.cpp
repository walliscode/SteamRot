/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for mouse collision checking of
/// MachinaFormScaffold parts
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "collision_mouse.h"
#include "MachinaFormScaffold.h"
#include "UIElement.h"
#include "entity_memory.h"
#include <array>
#include <vector>

namespace steamrot::logic::collision::mouse {

/////////////////////////////////////////////////
bool IsMouseOverBounds(const sf::Vector2i &mouse_position,
                       const sf::FloatRect &bounds) {
  return bounds.contains(sf::Vector2f(mouse_position));
}

/////////////////////////////////////////////////
void ClearMouseOver(UIElement &element) {
  element.is_mouse_over = false;
  for (auto &child : element.child_elements) {
    ClearMouseOver(*child);
  }
}

/////////////////////////////////////////////////
bool AnyMouseOver(const UIElement &element) {
  if (element.is_mouse_over) {
    return true;
  }
  for (const auto &child : element.child_elements) {
    if (AnyMouseOver(*child)) {
      return true;
    }
  }
  return false;
}

/////////////////////////////////////////////////
void CheckMouseOver(const sf::Vector2i &mouse_position, UIElement &element) {

  // disabled elements cannot be hovered; clear any stale hover state and skip
  if (element.is_disabled) {
    ClearMouseOver(element);
    return;
  }

  // bool to keep track if any child is hovered over
  bool child_hovered = false;

  if (element.children_active) {
    static constexpr std::array k_collision_pass_order{UIPriorityTier::Modal,
                                                       UIPriorityTier::Elevated,
                                                       UIPriorityTier::Normal};

    for (const UIPriorityTier tier : k_collision_pass_order) {
      // cycle through all child elements and check if any are hovered
      for (auto &child : element.child_elements) {
        if (!child || child->m_priority_tier != tier)
          continue;

        // go as deep as possible first; stops when no children are detected
        CheckMouseOver(mouse_position, *child);
        // Use AnyMouseOver so that a hover on any descendant (not just the
        // immediate child) short-circuits the remaining lower-priority siblings
        if (AnyMouseOver(*child)) {
          // for the parent to evaluate
          child_hovered = true;
          // if any descendant is hovered, no need to check further siblings
          break;
        }
      }
      if (child_hovered) {
        break;
      }
    }
  } else {
    // children are not active (not visible): clear any stale hover state so
    // invisible children cannot influence collision or action processing
    for (auto &child : element.child_elements) {
      ClearMouseOver(*child);
    }
  }

  if (child_hovered) {
    // if a child is hovered, parent cannot be hovered
    element.is_mouse_over = false;
  } else {
    // this will occur if no child is hovered (or no children exist), or
    // when children are inactive and the element itself should be tested
    element.is_mouse_over = IsMouseOverBounds(
        mouse_position, sf::FloatRect(element.position, element.size));
  }
}

/////////////////////////////////////////////////
void CheckMouseOverAllCUserInterfaceComponents(
    const std::vector<size_t> &entity_indexes, EntityMemoryPool &scene_entities,
    const sf::Vector2i &mouse_position, bool &is_mouse_over_ui_layer) {

  // Pass 1: clear all hover state to remove stale state from the previous tick
  for (size_t entity_id : entity_indexes) {
    CUserInterface &ui_component =
        entity::memory::GetComponent<CUserInterface>(entity_id, scene_entities);
    ClearMouseOver(*ui_component.m_root_element);
  }
  // clear the scene state flag at the start of the check
  is_mouse_over_ui_layer = false;

  // Pass 2: check collision in descending priority order; the first visible
  // entity that reports a hover claims the mouse and blocks all lower-priority
  // entities
  bool higher_priority_claimed_mouse = false;

  for (size_t entity_id : entity_indexes) {
    CUserInterface &ui_component =
        entity::memory::GetComponent<CUserInterface>(entity_id, scene_entities);

    if (!ui_component.m_visible) {
      continue;
    }

    if (higher_priority_claimed_mouse) {
      ClearMouseOver(*ui_component.m_root_element);
    } else {
      CheckMouseOver(mouse_position, *ui_component.m_root_element);

      if (AnyMouseOver(*ui_component.m_root_element)) {
        // if this entity reports a hover, it claims the mouse and blocks all
        higher_priority_claimed_mouse = true;

        // also, set the scene state flag to indicate the mouse is over a UI
        // layer
        is_mouse_over_ui_layer = true;
      }
    }
  }
}

/////////////////////////////////////////////////
void CheckMouseOverAllCUserInterfaceComponentsInTier(
    const std::vector<size_t> &entity_indexes, EntityMemoryPool &scene_entities,
    const sf::Vector2i &mouse_position, const UIPriorityTier tier,
    bool &higher_tier_claimed_mouse, bool &is_mouse_over_ui_layer) {
  for (size_t entity_id : entity_indexes) {
    CUserInterface &ui_component =
        entity::memory::GetComponent<CUserInterface>(entity_id, scene_entities);

    if (ui_component.m_priority_tier != tier || !ui_component.m_visible) {
      continue;
    }

    if (higher_tier_claimed_mouse) {
      ClearMouseOver(*ui_component.m_root_element);
      continue;
    }

    CheckMouseOver(mouse_position, *ui_component.m_root_element);
    if (AnyMouseOver(*ui_component.m_root_element)) {
      higher_tier_claimed_mouse = true;
      is_mouse_over_ui_layer = true;
    }
  }
}

/////////////////////////////////////////////////
void ProcessScaffoldCollisions(MachinaFormScaffold &scaffold,
                               sf::Vector2f world_mouse) {
  for (auto &[id, part] : scaffold.parts) {
    std::visit(
        [&](auto &instance) { instance.CheckMouseOverSockets(world_mouse); },
        part);
  }
}

} // namespace steamrot::logic::collision::mouse
