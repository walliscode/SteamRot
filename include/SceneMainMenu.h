#include "Scene.h"

class SceneMainMenu : public Scene {
private:
  void sDoAction(const ActionManager &action); // do action

public:
  SceneMainMenu(const std::string &name, size_t poolSize, GameEngine &game,
                SceneManager &sceneManager);

  void update();
};
