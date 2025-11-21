/////////////////////////////////////////////////
/// @file
/// @brief Declaration of Catch2 matcher for CMeta comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "CMeta.h"
#include <catch2/matchers/catch_matchers.hpp>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class CMeta specific matcher for detailed comparison
/////////////////////////////////////////////////
class CMetaEqualsMatcher : public Catch::Matchers::MatcherBase<CMeta> {
private:
  /////////////////////////////////////////////////
  /// @brief Expected CMeta object to compare against
  /////////////////////////////////////////////////
  const CMeta &m_expected;

  /////////////////////////////////////////////////
  /// @brief store mismatch description for reporting
  ///
  /// @note mutable to allow modification in const match method
  /////////////////////////////////////////////////
  mutable std::string m_mismatch_description;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for CMetaEqualsMatcher
  ///
  /// @param expected Expected CMeta object to compare against
  /////////////////////////////////////////////////
  explicit CMetaEqualsMatcher(const CMeta &expected);

  /////////////////////////////////////////////////
  /// @brief match method to compare actual CMeta with expected
  ///
  /// @param actual CMeta object to compare
  /// @return Whether the actual CMeta matches the expected
  /////////////////////////////////////////////////
  bool match(const CMeta &actual) const override;

  /////////////////////////////////////////////////
  /// @brief describe method to provide detailed mismatch description.
  /////////////////////////////////////////////////
  std::string describe() const override;
};

/////////////////////////////////////////////////
/// @brief Helper function to create CMetaEqualsMatcher
///
/// @param expected [TODO:parameter]
/// @return [TODO:return]
/////////////////////////////////////////////////
inline CMetaEqualsMatcher EqualsCMeta(const CMeta &expected) {
  return CMetaEqualsMatcher(expected);
}

} // namespace steamrot::tests
