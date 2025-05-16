#include "Component.h"
#include <SFML/Graphics/RectangleShape.hpp>

class CShape : public Component {

public:
  CShape() = default;

  sf::RectangleShape m_shape;

  const std::string &Name() override;
  void Configure(const json &data) override;
};
