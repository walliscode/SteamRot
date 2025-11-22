/////////////////////////////////////////////////
/// @file
/// @brief Declaration of Catch2 matcher for Fragment comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "Fragment.h"
#include <catch2/matchers/catch_matchers.hpp>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class FragmentEqualsMatcher
/// @brief Fragment specific matcher for detailed comparison
/////////////////////////////////////////////////
class FragmentEqualsMatcher : public Catch::Matchers::MatcherBase<Fragment> {
private:
  const Fragment &m_expected;
  mutable std::string m_mismatch_description;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for FragmentEqualsMatcher
  ///
  /// @param expected Expected Fragment object to compare against
  /////////////////////////////////////////////////
  explicit FragmentEqualsMatcher(const Fragment &expected);

  /////////////////////////////////////////////////
  /// @brief Match method to compare actual Fragment with expected
  ///
  /// @param actual Fragment object to compare
  /// @return Whether the actual Fragment matches the expected
  /////////////////////////////////////////////////
  bool match(const Fragment &actual) const override;

  /////////////////////////////////////////////////
  /// @brief Describe method to provide detailed mismatch description
  ///
  /// @return Formatted description string
  /////////////////////////////////////////////////
  std::string describe() const override;
};

/////////////////////////////////////////////////
/// @brief Helper function to create FragmentEqualsMatcher
///
/// @param expected Expected Fragment object
/// @return FragmentEqualsMatcher instance
/////////////////////////////////////////////////
inline FragmentEqualsMatcher EqualsFragment(const Fragment &expected) {
  return FragmentEqualsMatcher(expected);
}

} // namespace steamrot::tests
