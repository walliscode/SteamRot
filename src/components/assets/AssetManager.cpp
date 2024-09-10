#include "AssetManager.h"
#include "general_util.h"
#include <iostream>
#include "load_fonts_generated.h"


AssetManager::AssetManager() : m_fonts()
{
}

void AssetManager::loadSceneAssets(const std::string& sceneType)
{
	// load the fonts for the scene
	loadFonts(sceneType);
}


void AssetManager::loadFonts(const std::string& sceneType)
{
	// create the file name. check naming convention carefully
	const std::string fileName = std::string(FB_BINARIES_PATH) + "load_fonts_" + sceneType + ".bin";

	// using utils function, check file exists. if it doesn't this should exit the function
	if (!utils::fileExists(fileName)) {
		std::cout << "File does not exist: " << fileName << std::endl;
		return;
	}
	std::cout << "######  Loading fonts ###### " << std::endl;

	// util function that reads the fileinto a vector of bytes. This function should set the size
	std::vector<std::byte> buffer = utils::readBinaryFile(fileName);

	// flatbuffers interprets the buffer as a FontList object
	const SteamRot::rawData::FontList* font_list = SteamRot::rawData::GetFontList(buffer.data());

	// iterate hrough the list of fonts and add them to the asset manager
	for (const auto font : *font_list->fonts()) {
		// ssearch the map to see if key already exists
		auto it = m_fonts.find(font->tag()->str());
		if (it != m_fonts.end())
		{
			std::cout << "Font already loaded: " << font->tag()->str() << std::endl;
			continue; // skip this font
		}
		// add the font to the asset manager
		addFont(font->tag()->str(), font->file_name()->str());
	}
}

void AssetManager::addFont(const std::string& tag, const std::string& fileName)
{
	// create a font object and load the font from the file
	sf::Font font;
	std::string path = std::string(RESOURCES_PATH) + "fonts/" + fileName;
	std::cout << "Loading font: " << path << std::endl;
	font.loadFromFile(path);
	m_fonts[tag] = font;
	std::cout << "Loaded font: " << tag << std::endl;
}

const sf::Font& AssetManager::getFont(const std::string& name) const {
	// search the map for the font and if it exists return it
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