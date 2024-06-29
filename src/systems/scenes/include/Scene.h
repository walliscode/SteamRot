#pragma once
#include "EntityManager.h"
#include "Action.h"

class GameEngine; // forward declaration. DO NOT INCLUDE "GameEngine.h" here, as it will cause a circular dependency error

typedef std::map<int, std::string> ActionMap;

class Scene {
protected:

	EntityManager m_entityManager; // The entity manager of the scene
	GameEngine& m_engine; // The game engine, reference is used here because currently the GameEngine object will outlive any scene object
	ActionMap m_actionMap; // The actions of the scene

	bool    m_paused = false; // If the scene is paused
	bool    m_active = true; // if the scene is active, should the Scene manager call the updated function
	size_t  m_current_frame = 0; // The current frame of the scene, used for animations and systems logic
	std::string m_name; // The name of the scene

	Scene(const std::string& name, size_t poolSize, GameEngine& game);

public:
	

	virtual void update() = 0; //this will be the public call to the Scene which call its relevant systems
	virtual void sRender(); //render function which should be ubiquitous across all scenes

	bool getActive() const; // returning by value here as it's just as efficient when getting a bool and safer. but may need to change if getter function gets more complex
	void setActive(bool active);

	GameEngine& getEngine(); // Get the game engine

	// ####### Actions Functions #######
	void registerActions(const std::string& sceneName); // Register an action, sfml provides the action type as an int
	ActionMap& getActionMap(); // Get the action map
};