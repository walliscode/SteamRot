#pragma once
#include "Component.h"
#include "ComponentFlags.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct CMeta : public Component {
  CMeta() = default;
  bool m_entity_active = false;

  // designed to be able to quickly inform of what components are active
  SteamRot::ComponentFlags m_component_flags{0};
};

void to_json(json &j, const CMeta &meta);
void from_json(const json &j, CMeta &meta);
