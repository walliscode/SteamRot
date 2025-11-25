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
  /// @brief Constructor for CMetaEqualsMatcher with entity index
  ///
  /// @param expected Expected CMeta object to compare against
  /// @param entity_index Index of the entity in the EntityMemoryPool
  /////////////////////////////////////////////////
  CMetaEqualsMatcher(const CMeta &expected, size_t entity_index);

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

/////////////////////////////////////////////////
/// @brief Helper function to create CMetaEqualsMatcher with entity index
///
/// @param expected Expected CMeta object
/// @param entity_index Index of the entity in the EntityMemoryPool
/// @return CMetaEqualsMatcher instance
/////////////////////////////////////////////////
inline CMetaEqualsMatcher EqualsCMeta(const CMeta &expected, 
                                      size_t entity_index) {
  return CMetaEqualsMatcher(expected, entity_index);
}

} // namespace steamrot::tests
