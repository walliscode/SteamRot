/////////////////////////////////////////////////
/// @file
/// @brief Base class template for component-specific matchers
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "Component.h"
#include "conmat.h"
#include <catch2/matchers/catch_matchers.hpp>
#include <sstream>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class ComponentMatcherBase
/// @brief Base template class for component equality matchers
///
/// Provides common describe() implementation for component matchers.
/// Derived classes must implement the match() method.
///
/// @tparam TComponent The component type to match (must derive from Component)
/////////////////////////////////////////////////
template <typename TComponent>
class ComponentMatcherBase : public Catch::Matchers::MatcherBase<TComponent> {
  static_assert(std::is_base_of<Component, TComponent>::value,
                "TComponent must derive from Component");

protected:
  /////////////////////////////////////////////////
  /// @brief Expected component object to compare against
  /////////////////////////////////////////////////
  const TComponent &m_expected;

  /////////////////////////////////////////////////
  /// @brief Store mismatch description for reporting
  ///
  /// @note mutable to allow modification in const match method
  /////////////////////////////////////////////////
  mutable std::string m_mismatch_description;

  /////////////////////////////////////////////////
  /// @brief Get the component type name for display
  ///
  /// Override this in derived classes to provide specific component name
  /// Default implementation uses type_info, but override for cleaner names
  ///
  /// @return Component type name string
  /////////////////////////////////////////////////
  virtual std::string GetComponentName() const {
    return typeid(TComponent).name();
  }

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for ComponentMatcherBase
  ///
  /// @param expected Expected component object to compare against
  /////////////////////////////////////////////////
  explicit ComponentMatcherBase(const TComponent &expected)
      : m_expected(expected) {}

  /////////////////////////////////////////////////
  /// @brief Virtual destructor for proper cleanup
  /////////////////////////////////////////////////
  virtual ~ComponentMatcherBase() = default;

  /////////////////////////////////////////////////
  /// @brief Describe method to provide detailed mismatch description
  ///
  /// Provides consistent formatting across all component matchers:
  /// - Success: Shows component name with TestPassed marker
  /// - Failure: Shows component name with TestFailed marker and details
  ///
  /// @return Formatted description string
  /////////////////////////////////////////////////
  std::string describe() const override {
    // if mismatch description is empty then we can assume the test passed
    if (m_mismatch_description.empty()) {

      std::ostringstream oss;
      oss << conmat::Header(conmat::TestPassed() + GetComponentName() + " Match:", 3) << "\n";

      return oss.str();

    } else {

      std::ostringstream oss;
      oss << conmat::Header(conmat::TestFailed() + GetComponentName() + " Match:", 3) << "\n";
      oss << m_mismatch_description;

      return oss.str();
    }
  }
};

} // namespace steamrot::tests
