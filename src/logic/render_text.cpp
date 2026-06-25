/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the free functions related to the rendering of text in
/// the game.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_text.h"
#include <SFML/System/Vector2.hpp>

namespace steamrot::logic::render {

/////////////////////////////////////////////////
void fit_text_to_box(sf::Text &text, const sf::Vector2f &box_size,
                     float padding, bool allow_scale_up) {

  // check whether the text is too big and shrink the character size until the
  // text fits within the box
  if (text.getLocalBounds().size.x > (box_size.x - (padding * 2.f)) ||
      text.getLocalBounds().size.y > (box_size.y - (padding * 2.f))) {
    while (text.getLocalBounds().size.x > (box_size.x - (padding * 2.f)) ||
           text.getLocalBounds().size.y > (box_size.y - (padding * 2.f))) {

      // guard statement to prevent the character size from going below 2, which
      // is the minimum size for readability
      if (text.getCharacterSize() <= 2) {
        text.setCharacterSize(2);
        break;
      }
      text.setCharacterSize(text.getCharacterSize() - 1);
    }

    // return early so we dontt try to scale up the text after scaling it down
    // to fit the box
    return;
  }
  // or check whether the text is too small and grow the character size until
  // the text fits within the box, if allow_scale_up is true
  if (allow_scale_up) {
    while (text.getLocalBounds().size.x < (box_size.x - (padding * 2.f)) &&
           text.getLocalBounds().size.y < (box_size.y - (padding * 2.f))) {
      text.setCharacterSize(text.getCharacterSize() + 1);
    }
    // if we scaled up one step too far, scale back down by one step
    if (text.getLocalBounds().size.x > (box_size.x - (padding * 2.f)) ||
        text.getLocalBounds().size.y > (box_size.y - (padding * 2.f))) {
      text.setCharacterSize(text.getCharacterSize() - 1);
    }
  }
}
} // namespace steamrot::logic::render
