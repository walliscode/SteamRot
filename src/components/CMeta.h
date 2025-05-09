#pragma once
#include "Component.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct CMeta : public Component {
  CMeta() = default;
  bool m_entity_active = false;
};

void to_json(json &j, const CMeta &meta);
void from_json(const json &j, CMeta &meta);
