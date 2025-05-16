#pragma once
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Component {
  virtual ~Component() = default;
  virtual void Configure(const json &data) = 0;
  virtual const std::string &Name() = 0;
  bool m_component_active{false};
};
