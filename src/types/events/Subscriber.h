/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the Subscriber class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPayload.h"
#include "EventType.h"
#include <optional>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Subscriber struct for registering with the EventHandler
///
/// This is designed to be fairly lightweight and does not handle any
/// events/actions itself. It is simply toggled on/off based on events.
/// Systems check the m_active flag directly to determine if the subscriber
/// was triggered.
///
/// This is a POD (Plain Old Data) struct with no methods or constructors.
/////////////////////////////////////////////////
struct Subscriber {

  /////////////////////////////////////////////////
  /// @brief Indicates whether the subscriber is active or not.
  /////////////////////////////////////////////////
  bool m_active{false};

  /////////////////////////////////////////////////
  /// @brief Event type that this subscriber listens for.
  /////////////////////////////////////////////////
  EventType m_trigger_event_type{EventType::NONE};

  /////////////////////////////////////////////////
  /// @brief If set, the subscriber will only be activated if the event payload
  /// matches this trigger data.
  /////////////////////////////////////////////////
  std::optional<EventPayload> m_trigger_event_data{std::nullopt};

  /////////////////////////////////////////////////
  /// @brief Stores the actual event payload that triggered the activation.
  /// This is set when the subscriber is activated and can be used by
  /// handlers to access the event payload for processing.
  /////////////////////////////////////////////////
  std::optional<EventPayload> m_received_event_data{std::nullopt};
};
} // namespace steamrot
