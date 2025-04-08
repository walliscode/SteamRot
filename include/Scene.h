#pragma once
#include "ActionManager.h"
#include "EntityManager.h"

#include <SFML/Graphics/Drawable.hpp>
#include <nlohmann/json.hpp>
#include <string>

typedef std::vector<std::shared_ptr<sf::Drawable>> SceneDrawables;
class Scene {
protected:
  // objects live within the Scene
  EntityManager m_entity_manager;
  ActionManager m_action_manager;

  // Scene properties
  bool m_paused = false;
  bool m_active = true;
  std::string m_name;
  bool m_interactable = false;
  size_t m_current_frame = 0;

  Scene(const std::string &name, const size_t &pool_size);

  friend void to_json(nlohmann::json &j, const Scene &scene);

public:
  // common systems between Scenes
  virtual void sUpdate() = 0;
  virtual SceneDrawables sProvideDrawables() = 0;

  bool getActive() const;
  void setActive(bool active);
};

void to_json(nlohmann::json &j, const Scene &scene);
