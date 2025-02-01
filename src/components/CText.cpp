#include "CText.h"
#include <SFML/Graphics/Text.hpp>

CText::CText(const sf::Font &font, const std::string &str) : text(font, str) {};

// set the text object, taking in the font, the string, and the character size
void CText::SetText(const sf::Font &f, const std::string &words,
                    const size_t chr_size) {
  text.setFont(f);
  text.setString(words);
  text.setCharacterSize(chr_size); // in pixels, not points!

  // origin to center of text
  sf::FloatRect textRect = text.getLocalBounds();
  text.setOrigin(textRect.size / 2.0f);
}

// get the text object by reference
sf::Text &CText::GetText() { return text; }

json CText::ToJSON() {
  json j;
  j["text"] = text.getString();
  j["font_size"] = text.getCharacterSize();
  j["font"] = text.getFont().getInfo().family;
  return j;
}
