#pragma once
#include "Component.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
class CMeta : public Component {

private:
  bool m_active = false; // used to determine if the entity is active and will
                         // be considered in systems

public:
  const bool getActive() const; // used to determine if the entity is active and
                                // will be considered in systems

  void activate();   // used to set the entity as active
  void deactivate(); // used to set the entity as inactive

  CMeta() = default; // default constructor for memory allocation

  void LoadJSONData(const json &component_config) override;
  json toJSON(); // used to convert the component to json data
};
