#pragma once
#include <SFML/Graphics.hpp>


class Assets {

private:
	std::map<std::string, sf::Font>              m_fonts;

	void init();

public:
	Assets();

	//###### Fonts ######
	void loadFonts(const std::string& filename); // load assets from a FlatBuffers file
	void addFont(const std::string& tag, const std::string& fileName); // add a font
	const sf::Font& getFont(const std::string& name) const; // get a font
	const std::map<std::string, sf::Font>& getFonts() const; // get all fonts


	//###### Helper functions ######
	bool fileExists(const std::string& filename) const; // check if a file exists
	
};