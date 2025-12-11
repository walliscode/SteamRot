/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIElement struct
/////////////////////////////////////////////////
#include "UIElement.h"

namespace steamrot {

/////////////////////////////////////////////////
void UIElement::CloneBaseUIElementData(UIElement &target) const {
  target.position = position;
  target.size = size;
  target.subscription = subscription; // shared_ptr copies fine
  target.response_event = response_event;
  target.is_mouse_over = is_mouse_over;
  target.children_active = children_active;
  target.spacing_strategy = spacing_strategy;
  target.layout = layout;

  // Deep copy children
  target.child_elements.clear();
  target.child_elements.reserve(child_elements.size());
  for (const auto &child : child_elements) {
    if (child) {
      target.child_elements.push_back(child->Clone());
    }
  }
}

} // namespace steamrot
