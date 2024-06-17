#pragma once
#include <SFML/Graphics.hpp>
#include "Component.h"

class CText : public Component
{
private:
	sf::Text text;
	
public:
	
	CText() = default;
	void setText(const sf::Font& f, const std::string& str, const size_t chr_size);
	sf::Text& getText();
	
};
