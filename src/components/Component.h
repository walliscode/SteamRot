#pragma once
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Component {
  virtual ~Component() = default;
  bool m_component_active{false};
};
