#pragma once

#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct Component {
  virtual ~Component() = default;
  virtual const std::string &Name() = 0;

  /**
   * @brief Generates a unique index for the component based on its type and its
   * position in the component register.
   *
   * @return Index of the component in the component register.
   */
  virtual const size_t GetComponentRegisterIndex() const = 0;
  bool m_active{false};
};
