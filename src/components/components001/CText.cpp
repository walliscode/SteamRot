#include "CText.h"


void CText::setText(const sf::Font& f, const std::string& words, const size_t chr_size)
{
	text.setFont(f);
	text.setString(words);
	text.setCharacterSize(chr_size); // in pixels, not points!

	// origin to center of text
	sf::FloatRect textRect = text.getLocalBounds();
	text.setOrigin(textRect.width / 2.0f, textRect.height / 2.0f);
}

sf::Text& CText::getText()
{
	return text;
}