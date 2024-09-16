#pragma once
#include <SFML/Graphics.hpp>

#include "Component.h"
#include "flatbuffers/flatbuffers.h"
#include "entities_generated.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class CTransform : public Component   //Inherit component base class
{
public:
	sf::Vector2f position = { 0.0, 0.0 }; //Definition of required variables to describe a transform
	sf::Vector2f velocity = { 0.0, 0.0 };


	CTransform() {} //Empty constructor for populating an empty CTransform for default memory allocation

	CTransform(const sf::Vector2f& pos, const sf::Vector2f& vel) //Constructor for assigning a CTransform, with a position and velocity required
		: position(pos), velocity(vel)
	{

	}

	void fromFlatbuffers(const SteamRot::rawData::TransformComponent* transform); //Function to assign a CTransform from a flatbuffer transform
	
	json toJSON(); // return the component data as json object
};