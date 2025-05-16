#pragma once
#include "Scene.h"

namespace steamrot {

class MenuScene : public Scene {
private:
public:
  MenuScene(const std::string &name, size_t pool_size, const json &config_data);
  void sMovement() override;
  SceneDrawables sProvideDrawables() override;
};
} // namespace steamrot
