#pragma once
#include "Container.h"

class Scene; // forward declaration. DO NOT INCLUDE "Scene.h" here, as it will cause a circular dependency error

typedef std::map<std::string, std::shared_ptr<Container>> ContainerMap;


class ContainerManager
{
private:
	
	ContainerMap m_containers; // Map of  containers for the scene and their identifying tags


public:
	ContainerManager();

	void createContainer(const std::string& tag, const std::string& parentTag); // create a container with a tag and add to map
	std::shared_ptr<Container> getContainer(const std::string& tag); // get a container from the map
	ContainerMap& getContainerMap(); // get the container map

};