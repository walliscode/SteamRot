#include "Component.h"
#include <SFML/Graphics/RectangleShape.hpp>

class CShape : public Component {

public:
  CShape() = default;

  sf::RectangleShape &getShape();
  void LoadJSONData(const json &component_config) override;
  sf::RectangleShape m_shape;
};
