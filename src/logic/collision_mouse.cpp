/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for mouse collision checking of
/// MachinaFormScaffold parts
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "collision_mouse.h"
#include "UIElement.h"
#include <algorithm>
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
    // Build a sorted view of children in descending priority order so that
    // higher-priority siblings are evaluated before lower-priority ones
    std::vector<UIElement *> sorted_children;
    sorted_children.reserve(element.child_elements.size());
    for (auto &child : element.child_elements) {
      sorted_children.push_back(child.get());
    }
    std::stable_sort(sorted_children.begin(), sorted_children.end(),
                     [](const UIElement *a, const UIElement *b) {
                       return a->priority > b->priority;
                     });

    // cycle through all child elements and check if any are hovered
    for (auto *child : sorted_children) {
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

void CheckMouseOver(const sf::Vector2i &mouse_position, Socket &socket) {
  if (socket.circle.getGlobalBounds().contains(sf::Vector2f{mouse_position})) {
    socket.is_mouse_over = true;
  } else {
    socket.is_mouse_over = false;
  }
}

/////////////////////////////////////////////////
void CheckMouseOver(const sf::Vector2i &mouse_position,
                    FragmentInstance &fragment_instance) {
  for (auto &socket : fragment_instance.sockets) {
    CheckMouseOver(mouse_position, socket);
  }
}

/////////////////////////////////////////////////
void CheckMouseOver(const sf::Vector2i &mouse_position,
                    JointInstance &joint_instance) {
  for (auto &socket : joint_instance.sockets) {
    CheckMouseOver(mouse_position, socket);
  }
}

/////////////////////////////////////////////////
void ProcessUIEntityCollisions(
    const std::vector<size_t> &entity_indexes,
    EntityMemoryPool &scene_entities,
    const sf::Vector2i &mouse_position) {

  // Pass 1: clear all hover state to remove stale state from the previous tick
  for (size_t entity_id : entity_indexes) {
    CUserInterface &ui_component =
        entity::memory::GetComponent<CUserInterface>(entity_id, scene_entities);
    ClearMouseOver(*ui_component.m_root_element);
  }

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
        higher_priority_claimed_mouse = true;
      }
    }
  }
}

/////////////////////////////////////////////////
void ProcessScaffoldCollisions(MachinaFormScaffold &scaffold,
                                const sf::Vector2i &mouse_position) {
  for (auto &joint : scaffold.joints) {
    CheckMouseOver(mouse_position, joint);
  }

  for (auto &fragment : scaffold.fragments) {
    CheckMouseOver(mouse_position, fragment);
  }
}

} // namespace steamrot::logic::collision::mouse
