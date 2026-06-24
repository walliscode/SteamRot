/////////////////////////////////////////////////
/// @file
/// @brief Quick sfml window for testing whatever
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <iostream>
#include <ostream>

int main() {

  // set up a vertex array of type triangles
  sf::VertexArray va;
  va.setPrimitiveType(sf::PrimitiveType::Triangles);
  const sf::FloatRect &bounds = va.getBounds();

  std::cout << "bounds: " << std::endl;
  std::cout << "size (x): " << bounds.size.x << std::endl;

  // add 3 vertices
  sf::Vertex v_one{{0, 0}, sf::Color::White};
  sf::Vertex v_two{{50, 0}, sf::Color::White};
  sf::Vertex v_three{{0, 50}, sf::Color::White};

  va.append(v_one);
  va.append(v_two);
  va.append(v_three);

  // spit out bounding box

  return 0;
}
