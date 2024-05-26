#pragma once
#include <tuple>

class Component {
public:
	bool has = false; // if the entity has this component
};

 // dummy tuple to allow for unit testing of entities
typedef std::tuple <Component, Component> ComponentTuple;