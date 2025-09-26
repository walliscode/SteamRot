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
  /// @brief If set, the subscriber will only be activated if the event data
  /// matches the trigger data.
  /////////////////////////////////////////////////
  std::optional<const EventData> m_trigger_data{std::nullopt};

  /////////////////////////////////////////////////
  /// @brief Used as a key to store the subscriber in the EventHandler.
  /////////////////////////////////////////////////
  EventData m_event_data;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for the Subscriber class.
  /////////////////////////////////////////////////
  Subscriber(const EventType event_type);

  /////////////////////////////////////////////////
  /// @brief Constructor for the Subscriber class with trigger data.
  ///
  /// @param event_type Event type to register for.
  /// @param trigger_data Event data that will trigger the subscriber.
  /////////////////////////////////////////////////
  Subscriber(const EventType event_type, const EventData &trigger_data);

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

  const std::optional<const EventData> &GetTriggerData() const;

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
};
} // namespace steamrot
