#include "ContainerManager.h"
#include "Scene.h"
#include "GameEngine.h"




ContainerManager::ContainerManager()
{
	// create a root container with a tag and add to map, by creating this here createContainer does not need to deal with nullptrs
	m_containers.emplace("root", std::make_shared<Container>("root"));

}

void ContainerManager::createContainer(const std::string& tag, const std::string& parentTag) {

	
	// Create a shared pointer to the parent container
	std::shared_ptr<Container> parentContainer = getContainer(parentTag);

	// create a container with a tag and add to map, error check on iterator produced
	auto emplaceAttempt = m_containers.emplace(tag, std::make_shared<Container>(tag));
	if (!emplaceAttempt.second) {
		throw std::runtime_error("Container already exists");
	}

	// create a shared ptr for new container and add to parent container
	std::shared_ptr<Container> newContainer = getContainer(tag);
	parentContainer->addChildContainer(newContainer);



}

std::shared_ptr<Container> ContainerManager::getContainer(const std::string& tag) {

	auto it = m_containers.find(tag);
	if (it == m_containers.end()) {
		throw std::runtime_error("Container not found");
	}

	std::shared_ptr<Container> container = it->second;
	return container;
}

ContainerMap& ContainerManager::getContainerMap() {
	return m_containers;
}

