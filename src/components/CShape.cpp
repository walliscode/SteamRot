#include "CShape.h"

void to_json(json &j, const CShape &shape) {
  j = json{
      {"m_shape",
       {{"size",
         {{"x", shape.m_shape.getSize().x}, {"y", shape.m_shape.getSize().y}}},
        {"position",
         {{"x", shape.m_shape.getPosition().x},
          {"y", shape.m_shape.getPosition().y}}},
        {"fill_color",
         {{"r", shape.m_shape.getFillColor().r},
          {"g", shape.m_shape.getFillColor().g},
          {"b", shape.m_shape.getFillColor().b},
          {"a", shape.m_shape.getFillColor().a}}}}}};
}

void from_json(const json &j, CShape &p) {
  sf::Vector2f size;
  sf::Vector2f position;
  sf::Color fill_color;

  j.at("rectangle").at("size").at("x").get_to(size.x);
  j.at("rectangle").at("size").at("y").get_to(size.y);
  j.at("rectangle").at("position").at("x").get_to(position.x);
  j.at("rectangle").at("position").at("y").get_to(position.y);
  j.at("rectangle").at("fill_color").at("r").get_to(fill_color.r);
  j.at("rectangle").at("fill_color").at("g").get_to(fill_color.g);
  j.at("rectangle").at("fill_color").at("b").get_to(fill_color.b);
  j.at("rectangle").at("fill_color").at("a").get_to(fill_color.a);
}
