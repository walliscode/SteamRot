/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the functions for error loop handling.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "error_loop.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>

namespace error_loop {

/////////////////////////////////////////////////
void RunErrorLoop(const steamrot::FailInfo &failInfo) {

  // set up SFML window and any assets
  sf::RenderWindow window(sf::VideoMode({800, 600}), "Error Loop");

  // run the program as long as the window is open
  while (window.isOpen()) {
    // check all the window's events that were triggered since the last
    // iteration of the loop
    while (const std::optional event = window.pollEvent()) {
      // "close requested" event: we close the window
      if (event->is<sf::Event::Closed>())
        window.close();
    }
    // if e is pressed exit
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
      window.close();

    // clear the window with black color
    window.clear(sf::Color::Blue);

    // draw everything here...
    // window.draw(...);

    // end the current frame
    window.display();
  }
}
} // namespace error_loop
