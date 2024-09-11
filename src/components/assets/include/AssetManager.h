#pragma once
#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


class AssetManager
{
private:

	// maps containing the assets
	std::map<std::string, sf::Font>              m_fonts;



	//###### Fonts ######
	void loadFonts(const std::string& sceneType); // load assets from a FlatBuffers file
	void addFont(const std::string& tag, const std::string& fileName); // add a font

public:
	AssetManager(); // for now, created once in scene manager

	void loadSceneAssets(const std::string& sceneType); // load all assets for this particular scene type


	//###### Fonts ######
	const sf::Font& getFont(const std::string& name) const; // get a font


	// export data to json for testing
	json toJSON();
};