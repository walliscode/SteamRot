#include "Scene.h"

class SceneTest : public Scene
{
public:
	SceneTest(std::string tag, size_t poolSize, GameEngine& game, SceneManager& sceneManager) : Scene(tag, poolSize, game, sceneManager) {}
	~SceneTest() {}

	void update() override
	{
		std::cout << "SceneTest update called\n";
	}

	void sRender() override
	{
		std::cout << "SceneTest render called\n";
	}

	void sDoAction(const Action& action) override
	{
		std::cout << "SceneTest action called\n";
	}	
};