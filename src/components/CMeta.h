////////////////////////////////////////////////////////////
// preprocessor directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "Component.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace steamrot {
struct CMeta : public Component {
  CMeta() = default;
  bool m_entity_active = false;

  /**
   * @brief Generates a unique index for the CMeta component based on its
   * position in the Component register.
   *
   * @return Index of the CMeta component in the component register.
   */
  size_t GetComponentRegisterIndex() const override;
};
} // namespace steamrot
