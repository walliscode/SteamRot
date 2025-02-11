#pragma once
#include "ActionManager.h"
#include "EntityManager.h"

#include <nlohmann/json.hpp>
#include <string>

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

  virtual void sDoAction(const ActionManager &action) = 0;

  Scene(const std::string &name, const size_t &pool_size);

public:
  virtual void update() = 0;

  bool getActive() const;
  void setActive(bool active);

  // ####### Actions Functions #######

  void doAction(const ActionManager &action); // Do an action

  // data extraction
  json toJSON(); // Convert the scene to a json object
};
