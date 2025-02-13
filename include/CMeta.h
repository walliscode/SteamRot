#pragma once
#include "Component.h"
#include "ComponentFlags.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
class CMeta : public Component {

private:
  bool m_active = false;

  // designed to be able to quickly inform of what components are active
  // default value is 1, as CMeta should always be active
  SteamRot::ComponentFlags m_component_flags{1};

public:
  CMeta() = default;
  // determines whether the entity is active or not
  const bool getActive() const;
  void activate();
  void deactivate();

  // Component Flag functions
  const SteamRot::ComponentFlags &getComponentFlags() const;

  void LoadJSONData(const json &component_config) override;
  json toJSON();
};
