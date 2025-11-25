/////////////////////////////////////////////////
/// @file
/// @brief Declaration of Catch2 matcher for CMachinaForm comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "CMachinaForm.h"
#include "ComponentMatcherBase.h"
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class CMachinaFormEqualsMatcher
/// @brief CMachinaForm specific matcher for detailed comparison
/////////////////////////////////////////////////
class CMachinaFormEqualsMatcher : public ComponentMatcherBase<CMachinaForm> {
private:
  /////////////////////////////////////////////////
  /// @brief Get the component type name for display
  ///
  /// @return "CMachinaForm" string
  /////////////////////////////////////////////////
  std::string GetComponentName() const override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for CMachinaFormEqualsMatcher
  ///
  /// @param expected Expected CMachinaForm object to compare against
  /////////////////////////////////////////////////
  explicit CMachinaFormEqualsMatcher(const CMachinaForm &expected);

  /////////////////////////////////////////////////
  /// @brief Constructor for CMachinaFormEqualsMatcher with entity index
  ///
  /// @param expected Expected CMachinaForm object to compare against
  /// @param entity_index Index of the entity in the EntityMemoryPool
  /////////////////////////////////////////////////
  CMachinaFormEqualsMatcher(const CMachinaForm &expected, size_t entity_index);

  /////////////////////////////////////////////////
  /// @brief Match method to compare actual CMachinaForm with expected
  ///
  /// @param actual CMachinaForm object to compare
  /// @return Whether the actual CMachinaForm matches the expected
  /////////////////////////////////////////////////
  bool match(const CMachinaForm &actual) const override;
};

/////////////////////////////////////////////////
/// @brief Helper function to create CMachinaFormEqualsMatcher
///
/// @param expected Expected CMachinaForm object
/// @return CMachinaFormEqualsMatcher instance
/////////////////////////////////////////////////
inline CMachinaFormEqualsMatcher
EqualsCMachinaForm(const CMachinaForm &expected) {
  return CMachinaFormEqualsMatcher(expected);
}

/////////////////////////////////////////////////
/// @brief Helper function to create CMachinaFormEqualsMatcher with entity index
///
/// @param expected Expected CMachinaForm object
/// @param entity_index Index of the entity in the EntityMemoryPool
/// @return CMachinaFormEqualsMatcher instance
/////////////////////////////////////////////////
inline CMachinaFormEqualsMatcher
EqualsCMachinaForm(const CMachinaForm &expected, size_t entity_index) {
  return CMachinaFormEqualsMatcher(expected, entity_index);
}

} // namespace steamrot::tests
