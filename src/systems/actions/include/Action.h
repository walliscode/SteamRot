#pragma once
#include <iostream>

class Action
{
private:
	std::string m_name = "NONE";
	std::string m_type = "NONE";

public:
	Action(std::string name, std::string type); //overloaded constructor with parameters

	const std::string& getName() const; //getter for name
	const std::string& getType() const; //getter for type

};