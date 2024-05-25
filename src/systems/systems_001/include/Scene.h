

class Scene {
protected:

	bool    m_paused = false; // If the scene is paused
	bool    m_active = true; // if the scene is active, should the Scene manager call the updated function
	size_t  m_current_frame = 0; // The current frame of the scene, used for animations and systems logic

public:
	Scene();


	virtual void update() = 0;
	virtual void render() = 0;
};