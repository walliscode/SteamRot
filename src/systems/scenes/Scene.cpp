#pragma once
#include "Scene.h"
#include "GameEngine.h"


Scene::Scene(const std::string& name, size_t poolSize, GameEngine& game)
    : m_name(name), m_entityManager(name, poolSize), m_engine(game) {

}


void Scene::update() {

	// put systems here
}

void Scene::sRender() {
	// put render code here
	sf::RenderWindow* window = m_engine.getWindow();
	auto entities = m_entityManager.getEntities();


	for (const auto& entity : entities) {
		auto& text = m_entityManager.getComponent<CText>(entity);
		window->draw(text.getText());
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