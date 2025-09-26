/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the Subscriber class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "EventPacket.h"
#include "FailInfo.h"
#include <expected>
#include <optional>
#include <utility>
#include <variant>
namespace steamrot {

/////////////////////////////////////////////////
/// @class Subscriber
/// @brief For registering with the EventHandler
///
/// This is deigned to be faily lightweight and does not handle any
/// events/actions itself. It is simply toggled
///
/////////////////////////////////////////////////
struct Subscriber {

private:
  /////////////////////////////////////////////////
  /// @brief Indicates whether the subscriber is active or not.
  /////////////////////////////////////////////////
  bool m_active{false};

  /////////////////////////////////////////////////
  /// @brief Used to register the subscriber with the EventHandler.
  /////////////////////////////////////////////////
  const EventType m_event_type;

  /////////////////////////////////////////////////
  /// @brief Used as a key to store the subscriber in the EventHandler.
  /////////////////////////////////////////////////
  EventData m_event_data;

  /////////////////////////////////////////////////
  /// @brief Optional trigger condition - if present, must match EventPacket data for activation
  /////////////////////////////////////////////////
  std::optional<EventData> m_trigger_event;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for the Subscriber class.
  /////////////////////////////////////////////////
  Subscriber(const EventType event_type);

  /////////////////////////////////////////////////
  /// @brief Constructor for the Subscriber class with optional trigger event.
  ///
  /// @param event_type The event type to register for
  /// @param trigger_event Optional trigger condition that must match for activation
  /////////////////////////////////////////////////
  Subscriber(const EventType event_type, const std::optional<EventData>& trigger_event);

  /////////////////////////////////////////////////
  /// @brief Delete the default constructor to prevent instantiation without
  /// parameters.
  /////////////////////////////////////////////////
  Subscriber() = delete;

  /////////////////////////////////////////////////
  /// @brief Inspect the subscriber to see if it is active.
  ///
  /// @return Boolean indicating whether the subscriber is active.
  /////////////////////////////////////////////////
  bool IsActive() const;

  /////////////////////////////////////////////////
  /// @brief Turn the subscriber on
  ///
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> SetActive();

  /////////////////////////////////////////////////
  /// @brief Turn the subscriber off
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> SetInactive();

  /////////////////////////////////////////////////
  /// @brief Provide the registration information for the subscriber.
  /////////////////////////////////////////////////
  std::pair<EventType, EventData> GetRegistrationInfo() const;

  /////////////////////////////////////////////////
  /// @brief Return a reference to the event type the subscriber is registered
  /// for.
  ///
  /// @return a reference to the event type.
  /////////////////////////////////////////////////
  const EventType &GetEventType() const;

  const EventData &GetEventData() const;

  /////////////////////////////////////////////////
  /// @brief Copy the event data for the subscriber.
  ///
  /// @param event_data Event data to be copied.
  /////////////////////////////////////////////////
  void SetEventData(const EventData &event_data);
  /////////////////////////////////////////////////
  /// @brief Replace the event data for the subscriber.
  ///
  /// @param new_event_data
  /////////////////////////////////////////////////
  void UpdateEventData(const EventData &new_event_data);

  /////////////////////////////////////////////////
  /// @brief Get the optional trigger event condition
  ///
  /// @return Optional trigger event data
  /////////////////////////////////////////////////
  const std::optional<EventData>& GetTriggerEvent() const;

  /////////////////////////////////////////////////
  /// @brief Check if trigger event condition matches the provided event data
  ///
  /// @param event_data Event data to compare against trigger condition
  /// @return true if no trigger is set or if trigger matches, false otherwise
  /////////////////////////////////////////////////
  bool ShouldActivate(const EventData& event_data) const;
};
} // namespace steamrot
