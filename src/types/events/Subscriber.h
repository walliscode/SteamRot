/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the Subscriber class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "EventPacket.h"
#include <optional>

namespace steamrot {

/////////////////////////////////////////////////
/// @class Subscriber
/// @brief For registering with the EventHandler
///
/// This is designed to be fairly lightweight and does not handle any
/// events/actions itself. It is simply toggled on/off based on events.
/// Systems check the m_active flag directly to determine if the subscriber
/// was triggered.
///
/////////////////////////////////////////////////
struct Subscriber {

  /////////////////////////////////////////////////
  Subscriber(const EventType trigger_event_type)
      : m_trigger_event_type(trigger_event_type) {};

  /////////////////////////////////////////////////
  Subscriber(const EventType trigger_event_type,
             const EventData &trigger_event_data)
      : m_trigger_event_type(trigger_event_type),
        m_trigger_event_data(trigger_event_data) {};

  /////////////////////////////////////////////////
  /// @brief Indicates whether the subscriber is active or not.
  /////////////////////////////////////////////////
  bool m_active{false};

  /////////////////////////////////////////////////
  /// @brief Event type that this subscriber listens for.
  /////////////////////////////////////////////////
  EventType m_trigger_event_type{EventType::EventType_NONE};

  /////////////////////////////////////////////////
  /// @brief If set, the subscriber will only be activated if the event data
  /// matches this trigger data.
  /////////////////////////////////////////////////
  std::optional<EventData> m_trigger_event_data{std::nullopt};

  /////////////////////////////////////////////////
  /// @brief Stores the actual event data that triggered the activation.
  /// This is set when the subscriber is activated and can be used by
  /// handlers to access the event data for processing.
  /////////////////////////////////////////////////
  std::optional<EventData> m_received_event_data{std::nullopt};
};
} // namespace steamrot
