/////////////////////////////////////////////////
/// @file
/// @brief Declaration of Catch2 matcher for CUIState comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "CUIState.h"
#include "ComponentMatcherBase.h"
#include <sstream>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class CUIStateEqualsMatcher
/// @brief CUIState specific matcher for detailed comparison
/////////////////////////////////////////////////
class CUIStateEqualsMatcher : public ComponentMatcherBase<CUIState> {
private:
  /////////////////////////////////////////////////
  /// @brief Get the component type name for display
  ///
  /// @return "CUIState" string
  /////////////////////////////////////////////////
  std::string GetComponentName() const override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for CUIStateEqualsMatcher
  ///
  /// @param expected Expected CUIState object to compare against
  /////////////////////////////////////////////////
  explicit CUIStateEqualsMatcher(const CUIState &expected);

  /////////////////////////////////////////////////
  /// @brief Constructor for CUIStateEqualsMatcher with entity index
  ///
  /// @param expected Expected CUIState object to compare against
  /// @param entity_index Index of the entity in the EntityMemoryPool
  /////////////////////////////////////////////////
  CUIStateEqualsMatcher(const CUIState &expected, size_t entity_index);

  /////////////////////////////////////////////////
  /// @brief Match method to compare actual CUIState with expected
  ///
  /// @param actual CUIState object to compare
  /// @return Whether the actual CUIState matches the expected
  /////////////////////////////////////////////////
  bool match(const CUIState &actual) const override;
};

/////////////////////////////////////////////////
/// @brief Helper function to create CUIStateEqualsMatcher
///
/// @param expected Expected CUIState object
/// @return CUIStateEqualsMatcher instance
/////////////////////////////////////////////////
inline CUIStateEqualsMatcher EqualsCUIState(const CUIState &expected) {
  return CUIStateEqualsMatcher(expected);
}

/////////////////////////////////////////////////
/// @brief Helper function to create CUIStateEqualsMatcher with entity index
///
/// @param expected Expected CUIState object
/// @param entity_index Index of the entity in the EntityMemoryPool
/// @return CUIStateEqualsMatcher instance
/////////////////////////////////////////////////
inline CUIStateEqualsMatcher EqualsCUIState(const CUIState &expected, 
                                            size_t entity_index) {
  return CUIStateEqualsMatcher(expected, entity_index);
}

/////////////////////////////////////////////////
/// @brief Takes in two UIVisibilityState objects adds any differences to the
/// provided ostringstream
///
/// @param expected UIVisibilityState object
/// @param actual UIVisibilityState object
/// @param oss stream to append differences to
/////////////////////////////////////////////////
void CompareUIVisibilityState(const std::string &state_key,
                              const UIVisibilityState &expected,
                              const UIVisibilityState &actual,
                              std::ostringstream &oss);

void CompareMapStateToUIVisibility(
    const std::unordered_map<std::string, UIVisibilityState> &expected,
    const std::unordered_map<std::string, UIVisibilityState> &actual,
    std::ostringstream &oss);

void CompareMapStateValues(
    const std::unordered_map<std::string, bool> &expected,
    const std::unordered_map<std::string, bool> &actual,
    std::ostringstream &oss);

void CompareMapStateSubscribers(
    const std::unordered_map<
        std::string, std::vector<std::shared_ptr<Subscriber>>> &expected,
    const std::unordered_map<std::string,
                             std::vector<std::shared_ptr<Subscriber>>> &actual,
    std::ostringstream &oss);
} // namespace steamrot::tests
