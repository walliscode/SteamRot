#include "Component.h"
#include <SFML/Graphics/RectangleShape.hpp>

class CShape : public Component {

public:
  CShape() = default;

  sf::RectangleShape m_shape;
};

void to_json(json &j, const CShape &p);

void from_json(const json &j, CShape &p);
