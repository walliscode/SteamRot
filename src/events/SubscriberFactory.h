/////////////////////////////////////////////////
/// @file
/// @brief Definition of the SubscriberFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "EventHandler.h"
#include "EventPacket.h"
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
  EventHandler &m_event_handler;

public:
  SubscriberFactory(EventHandler &event_handler);

  std::expected<std::shared_ptr<Subscriber>, FailInfo>
  CreateAndRegisterSubscriber(const EventType &event_type,
                              const EventData &event_data);
};
} // namespace steamrot
