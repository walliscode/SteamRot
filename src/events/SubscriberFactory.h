/////////////////////////////////////////////////
/// @file
/// @brief Definition of the SubscriberFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventHandler.h"
#include "Subscriber.h"
#include <expected>
namespace steamrot {

/////////////////////////////////////////////////
/// @class SubscriberFactory
/// @brief Creates and Registers Subscribers for various event types.
///
/////////////////////////////////////////////////
class SubscriberFactory {

private:
  /////////////////////////////////////////////////
  /// @brief Reference to the EventHandler for registering subscribers.
  /////////////////////////////////////////////////
  EventHandler &m_event_handler;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for the SubscriberFactory class taking an EventHandler
  /// reference.
  ///
  /// @param event_handler Reference to the EventHandler for registering
  /// subscribers.
  /////////////////////////////////////////////////
  SubscriberFactory(EventHandler &event_handler);

  /////////////////////////////////////////////////
  /// @brief Given event type and data, create and register a subscriber.
  ///
  /// @param event_type Reference to the EventType for the subscriber
  /// @param event_data Reference to the EventData for the subscriber
  /////////////////////////////////////////////////
  std::expected<std::shared_ptr<Subscriber>, FailInfo>
  CreateAndRegisterSubscriber(const EventType &event_type);

  /////////////////////////////////////////////////
  /// @brief Given event type and optional trigger event, create and register a subscriber.
  ///
  /// @param event_type Reference to the EventType for the subscriber
  /// @param trigger_event Optional trigger condition for subscriber activation
  /////////////////////////////////////////////////
  std::expected<std::shared_ptr<Subscriber>, FailInfo>
  CreateAndRegisterSubscriber(const EventType &event_type, const std::optional<EventData>& trigger_event);
};
} // namespace steamrot
