/////////////////////////////////////////////////
/// @file
/// @brief Declaration of Catch2 matcher for CMeta comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "CMeta.h"
#include "ComponentMatcherBase.h"
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class CMetaEqualsMatcher
/// @brief CMeta specific matcher for detailed comparison
/////////////////////////////////////////////////
class CMetaEqualsMatcher : public ComponentMatcherBase<CMeta> {
private:
  /////////////////////////////////////////////////
  /// @brief Get the component type name for display
  ///
  /// @return "CMeta" string
  /////////////////////////////////////////////////
  std::string GetComponentName() const override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for CMetaEqualsMatcher
  ///
  /// @param expected Expected CMeta object to compare against
  /////////////////////////////////////////////////
  explicit CMetaEqualsMatcher(const CMeta &expected);

  /////////////////////////////////////////////////
  /// @brief Match method to compare actual CMeta with expected
  ///
  /// @param actual CMeta object to compare
  /// @return Whether the actual CMeta matches the expected
  /////////////////////////////////////////////////
  bool match(const CMeta &actual) const override;
};

/////////////////////////////////////////////////
/// @brief Helper function to create CMetaEqualsMatcher
///
/// @param expected Expected CMeta object
/// @return CMetaEqualsMatcher instance
/////////////////////////////////////////////////
inline CMetaEqualsMatcher EqualsCMeta(const CMeta &expected) {
  return CMetaEqualsMatcher(expected);
}

} // namespace steamrot::tests
