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
#include "FailInfo.h"
#include "Subscriber.h"
#include "subscriber_config_generated.h"
#include <expected>
#include <memory>
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
  /// @brief Create a Subscriber from flatbuffers data
  ///
  /// @param subscriber_data Flatbuffers SubscriberData to create the subscriber
  /// from.
  /////////////////////////////////////////////////
  std::expected<std::shared_ptr<Subscriber>, FailInfo>
  CreateAndRegisterSubscriber(const SubscriberData &subscriber_data);

  /////////////////////////////////////////////////
  /// @brief Given event type and data, create and register a subscriber.
  ///
  /// @param event_type Reference to the EventType for the subscriber
  /////////////////////////////////////////////////
  std::expected<std::shared_ptr<Subscriber>, FailInfo>
  CreateAndRegisterSubscriber(const EventType &event_type);

  /////////////////////////////////////////////////
  /// @brief Given event type and trigger data, create and register a
  /// subscriber.
  ///
  /// @param event_type Reference to the EventType for the subscriber
  /// @param trigger_data Reference to the EventData for the subscriber trigger
  /////////////////////////////////////////////////
  std::expected<std::shared_ptr<Subscriber>, FailInfo>
  CreateAndRegisterSubscriber(const EventType &event_type,
                              const EventData &trigger_data);
};
} // namespace steamrot
