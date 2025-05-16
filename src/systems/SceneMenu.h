#pragma once
#include "Scene.h"

namespace steamrot {

class SceneMenu : public Scene {
private:
public:
  SceneMenu(const std::string &name, size_t pool_size,
            const EntityConfigData &config_data);
  void sMovement() override;
  SceneDrawables sProvideDrawables() override;
};
} // namespace steamrot
