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

  ////////////////////////////////////////////////////////////
  /// @brief Three-way comparison operator for CMeta
  ///
  /// @param other The CMeta to compare with
  /// @return Ordering result
  ////////////////////////////////////////////////////////////
  auto operator<=>(const CMeta &other) const = default;

  ////////////////////////////////////////////////////////////
  /// @brief Equality operator for CMeta
  ///
  /// @param other The CMeta to compare with
  /// @return true if components are equal, false otherwise
  ////////////////////////////////////////////////////////////
  bool operator==(const CMeta &other) const = default;
};
} // namespace steamrot
