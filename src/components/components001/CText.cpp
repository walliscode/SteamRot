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

void CText::fromFlatbuffers(const SteamRot::rawData::TextComponent* text_component, const sf::Font& font)
{
	setText(font, text_component->text()->str(), text_component->font_size());
}

json CText::toJSON()
{
	json j;
	j["text"] = text.getString();
	j["font_size"] = text.getCharacterSize();
	j["font"] = text.getFont()->getInfo().family;
	return j;
}