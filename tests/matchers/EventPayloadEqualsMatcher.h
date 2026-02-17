/////////////////////////////////////////////////
/// @file
/// @brief Declaration of Catch2 matcher for EventPayload comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPayload.h"
#include "catch2/matchers/catch_matchers.hpp"

namespace steamrot::tests {
class EventPayloadEqualsMatcher
    : public Catch::Matchers::MatcherBase<EventPayload> {

private:
  /////////////////////////////////////////////////
  /// @brief Storage of the expected EventPayload for comparison
  /////////////////////////////////////////////////
  const EventPayload &m_expected;

  /////////////////////////////////////////////////
  /// @brief String to hold mismatch description
  /////////////////////////////////////////////////
  mutable std::string m_mismatch_description;

  /////////////////////////////////////////////////
  /// @brief Get the name of the EventPayload variant type by index
  ///
  /// @param index Index of the variant type
  /// @return Name of the variant type
  /////////////////////////////////////////////////
  std::string GetNameForEventPayloadIndex(size_t index) const;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for EventPayloadEqualsMatcher
  ///
  /// @param expected EventPayload object to compare against
  /////////////////////////////////////////////////
  explicit EventPayloadEqualsMatcher(const EventPayload &expected);

  /////////////////////////////////////////////////
  /// @brief Match method to compare actual EventPayload with expected
  ///
  /// @param actual EventPayload object to compare
  /// @return Whether the actual EventPayload matches the expected
  ///////////////////////////////////////////////
  bool match(const EventPayload &actual) const override;

  /////////////////////////////////////////////////
  /// @brief Describe the result of the match
  ///
  /// @return Description string
  /////////////////////////////////////////////////
  std::string describe() const override;
};

inline EventPayloadEqualsMatcher
EqualsEventPayload(const EventPayload &expected) {
  return EventPayloadEqualsMatcher(expected);
}
} // namespace steamrot::tests
