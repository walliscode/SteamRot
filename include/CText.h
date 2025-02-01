#pragma once
#include "Component.h"
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class CText : public Component {
private:
  sf::Text text; // the text object for the component

public:
  CText() = default;
  void setText(const sf::Font &f, const std::string &str,
               const size_t chr_size); // set the text object
  sf::Text &getText();                 // get the text object by reference

  // void fromFlatbuffers(const SteamRot::rawData::TextComponent *text,
  //                      const sf::Font &font); // calls the setText function
  //                      with
  //                                             // the flatbuffers data

  json toJSON(); // convert the component to json data for simulations
};
