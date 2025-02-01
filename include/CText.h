#pragma once
#include "Component.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class CText : public Component {
private:
  sf::Text text;

public:
  CText(const sf::Font &font, const std::string &str);

  void SetText(const sf::Font &font, const std::string &str,
               const size_t chr_size);

  sf::Text &GetText();

  json ToJSON(); // convert the component to json data for simulations
};
