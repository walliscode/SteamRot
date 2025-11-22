/////////////////////////////////////////////////
/// @file
/// @brief Declaration of Catch2 matcher for EventPacket comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "EventPacket.h"
#include "UserInputBitset.h"
#include "events_generated.h"
#include <catch2/matchers/catch_matchers.hpp>
#include <sstream>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class EventPacketEqualsMatcher
/// @brief EventPacket specific matcher for detailed comparison
/////////////////////////////////////////////////
class EventPacketEqualsMatcher
    : public Catch::Matchers::MatcherBase<EventPacket> {
private:
  const EventPacket &m_expected;
  mutable std::string m_mismatch_description;

  /////////////////////////////////////////////////
  /// @brief Get the name of the EventData variant type by index
  ///
  /// @param index Index of the variant type
  /// @return Name of the variant type
  /////////////////////////////////////////////////
  std::string GetNameForEventDataIndex(size_t index) const;

  /////////////////////////////////////////////////
  /// @brief Compare EventData variants
  ///
  /// @param actual_data Actual event data
  /// @param expected_data Expected event data
  /// @param oss Output stream for mismatch description
  /// @return true if data matches, false otherwise
  /////////////////////////////////////////////////
  bool CompareEventData(const EventData &actual_data,
                        const EventData &expected_data,
                        std::ostringstream &oss) const;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for EventPacketEqualsMatcher
  ///
  /// @param expected Expected EventPacket object to compare against
  /////////////////////////////////////////////////
  explicit EventPacketEqualsMatcher(const EventPacket &expected);

  /////////////////////////////////////////////////
  /// @brief Match method to compare actual EventPacket with expected
  ///
  /// @param actual EventPacket object to compare
  /// @return Whether the actual EventPacket matches the expected
  /////////////////////////////////////////////////
  bool match(const EventPacket &actual) const override;

  /////////////////////////////////////////////////
  /// @brief Describe method to provide detailed mismatch description
  ///
  /// @return Formatted description string
  /////////////////////////////////////////////////
  std::string describe() const override;
};

/////////////////////////////////////////////////
/// @brief Helper function to create EventPacketEqualsMatcher
///
/// @param expected Expected EventPacket object
/// @return EventPacketEqualsMatcher instance
/////////////////////////////////////////////////
inline EventPacketEqualsMatcher EqualsEventPacket(const EventPacket &expected) {
  return EventPacketEqualsMatcher(expected);
}

} // namespace steamrot::tests
