#include "Assets.h" // for some reason, the CMake file won't link assets.h without the include
#include <iostream>
#include <fstream>


#include "load_fonts_generated.h"


Assets::Assets():m_fonts() {

	init();
}

void Assets::init() {

	std::cout << "####### Loading assets... ########" << std::endl;
	// Load assets here

	loadFonts((std::string(FB_BINARIES_PATH) + "load_fonts.bin")); // pass the binary that contains the json configurations
}

void Assets::loadFonts(const std::string& filename)
{
	if (!utils::fileExists(filename)) {
		std::cout << "File does not exist: " << filename << std::endl;
		return;
	}
	std::cout << "######  Loading fonts ###### " << std::endl;

	std::ifstream infile;
	infile.open(filename, std::ios::binary | std::ios::in);
	infile.seekg(0, std::ios::end);
	int length = infile.tellg();
	infile.seekg(0, std::ios::beg);
	char* data = new char[length];
	infile.read(data, length);
	infile.close();

	const SteamRot::rawData::FontList* font_list = SteamRot::rawData::GetFontList(data);

	for (const auto font : *font_list->fonts()) {
		addFont(font->tag()->str(), font->file_name()->str());
		getFont(font->tag()->str());

	}
}

void Assets::addFont(const std::string& tag, const std::string& fileName)
{
	sf::Font font;
	std::string path = std::string(RESOURCES_PATH) + "fonts/" +  fileName;
	std::cout << "Loading font: " << path << std::endl;
	font.loadFromFile(path);
	m_fonts[tag] = font;
	std::cout << "Loaded font: " << tag << std::endl;
}

const sf::Font& Assets::getFont(const std::string& name) const {
	auto it = m_fonts.find(name);
	if (it != m_fonts.end()) {
		std::cout << "Success: Font '" << name << "' found." << std::endl;
		return it->second;
	}
	else {
		std::cout << "Error: Font '" << name << "' not found." << std::endl;
		return m_fonts.begin()->second;

	}
}

const std::map<std::string, sf::Font>& Assets::getFonts() const
{
	return m_fonts;
}

