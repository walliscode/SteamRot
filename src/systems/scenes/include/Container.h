#pragma once
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include <memory>


class Container {
private:
	std::string  m_tag; // tag of the container
	std::vector<size_t> m_entityIndexes; // vector of entities in the container
	sf::RectangleShape m_shape; // shape of the container
	sf::Vector2f      m_prortionalSize{ 1.0f, 1.0f }; // size of the container as a proportion of the parent container

	std::vector<std::shared_ptr<Container>> m_childrenContainers; // vector of child containers

public:

	Container(const std::string& tag); // constructor

	// tag getter and setter
	std::string getTag() const;
	void setTag(const std::string& tag);

	// entity indexes getter and setter
	std::vector<size_t> getEntityIndexes() const;
	void addEntityIndex(size_t index);

	// shape getter and setter
	sf::RectangleShape& getShape();
	// setters built into RectangleShape

	// proportional size getter and setter
	sf::Vector2f getProportionalSize() const;
	void setProportionalSize(const sf::Vector2f& size);

	// children containers getter and setter
	std::vector<std::shared_ptr<Container>> getChildrenContainers() const;
	void addChildContainer(std::shared_ptr<Container> container);


};