#include "EntityManager.h"


class Scene {
protected:

	EntityManager m_entityManager; // The entity manager of the scene
	bool    m_paused = false; // If the scene is paused
	bool    m_active = true; // if the scene is active, should the Scene manager call the updated function
	size_t  m_current_frame = 0; // The current frame of the scene, used for animations and systems logic


public:
	Scene(int poolSize);

	virtual void update() = 0; //this will be the public call to the Scene which call its relevant systems
	
	bool getActive() const; // returning by value here as it's just as efficient when getting a bool and safer. but may need to change if getter function gets more complex
};