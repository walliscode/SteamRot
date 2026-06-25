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
  const sf::FloatRect bounds = va.getBounds();

  std::cout << "bounds: " << std::endl;
  std::cout << "size (x): " << bounds.size.x << std::endl;
  std::cout << "size (y): " << bounds.size.y << std::endl;
  std::cout << "position (x): " << bounds.position.x << std::endl;
  std::cout << "position (y): " << bounds.position.y << std::endl;

  // add 3 vertices
  sf::Vertex v_one{{02, 0}, sf::Color::White};
  sf::Vertex v_two{{50, 0}, sf::Color::White};
  sf::Vertex v_three{{2, 50}, sf::Color::White};

  va.append(v_one);
  va.append(v_two);
  va.append(v_three);

  const sf::FloatRect bounds_after_triangle_one = va.getBounds();
  // spit out bounding box
  std::cout << "bounds after adding vertices: " << std::endl;
  std::cout << "size (x): " << bounds_after_triangle_one.size.x << std::endl;
  std::cout << "size (y): " << bounds_after_triangle_one.size.y << std::endl;
  std::cout << "position (x): " << bounds_after_triangle_one.position.x
            << std::endl;
  std::cout << "position (y): " << bounds_after_triangle_one.position.y
            << std::endl;

  // add 3 more vertices
  sf::Vertex v_four{{50, 50}, sf::Color::White};
  sf::Vertex v_five{{100, 50}, sf::Color::White};
  sf::Vertex v_six{{50, 100}, sf::Color::White};

  va.append(v_four);
  va.append(v_five);
  va.append(v_six);

  const sf::FloatRect bounds_after_triangle_two = va.getBounds();
  // spit out bounding box
  std::cout << "bounds after adding second triangle: " << std::endl;
  std::cout << "size (x): " << bounds_after_triangle_two.size.x << std::endl;
  std::cout << "size (y): " << bounds_after_triangle_two.size.y << std::endl;
  std::cout << "position (x): " << bounds_after_triangle_two.position.x
            << std::endl;
  std::cout << "position (y): " << bounds_after_triangle_two.position.y;

  return 0;
}
