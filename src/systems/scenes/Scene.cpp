
#include "Scene.h"
#include "GameEngine.h"
#include "actions_generated.h"
#include "flatbuffers/flatbuffers.h"
#include "general_util.h"
#include <fstream>


Scene::Scene(const std::string& name, size_t poolSize, GameEngine& game)
    : m_name(name), m_entityManager(poolSize, *this), m_engine(game) {
	// get size of asssets fonts
	size_t size = m_engine.getAssets().getFonts().size();
	std::cout << "Size of fonts at Scene Constructor: " << size << std::endl;

	this->m_entityManager.intialiseEntities(this->m_name);
	this->registerActions(this->m_name);
}


void Scene::update() {

	// put systems here
}

void Scene::sRender() {
	// put render code here
	sf::RenderWindow* window = m_engine.getWindow();
	auto entities = m_entityManager.getEntities();

	/* this section does not currently have any conditional code 
	as that will come from the archetype "masking"
	*/
	for (const auto& entity : entities) {
		auto& text = m_entityManager.getComponent<CText>(entity).getText();
		text.setPosition(m_entityManager.getComponent<CTransform>(entity).position);
		window->draw(text);
		// render code here
	}

	// Draw the mouse position, mainly just for checking rendering at the moment
	sf::CircleShape mouseCircle(25.0f);
	sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
	mouseCircle.setPosition(mousePos.x, mousePos.y);


	// Set the fill color of the circle
	mouseCircle.setFillColor(sf::Color::Red); // Coloring the circle red
	window->draw(mouseCircle);
	

}

bool Scene::getActive() const {
	return m_active;
}

void Scene::setActive(bool active) {
	m_active = active;
}

GameEngine& Scene::getEngine() {
	return m_engine;
}

// ####### Actions Functions #######
void Scene::registerActions(const std::string& sceneName) {


	
	// load the actions from the binary file
	std::string fileName = std::string(FB_BINARIES_PATH) + sceneName + "_actions.bin";

	std::cout << "Reading binary file: " << fileName << std::endl;

	std::vector<std::byte> buffer = utils::readBinaryFile(fileName);

	//if buffer is empty exit intialiseEntities
	if (buffer.empty()) {
		std::cout << "No Entities to intialise" << std::endl;
		return;
	}

	const SteamRot::rawData::ActionList* action_list = SteamRot::rawData::GetActionList(buffer.data());

	for (const auto action : *action_list->actions()) {
		int key = action->sfml_id();
		std::string actionName = action->action_name()->str();
		m_actionMap[key] = actionName;
	}
}

ActionMap& Scene::getActionMap() {
	return m_actionMap;
}