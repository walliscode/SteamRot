#pragma once
#include "Scene.h"

class SceneMenu : public Scene {
private:
public:
  SceneMenu(const std::string &name, size_t poolSize);
  void sUpdate() override;
  SceneDrawables sProvideDrawables() override;
};
