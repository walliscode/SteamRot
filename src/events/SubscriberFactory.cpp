/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the SubscriberFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SubscriberFactory.h"

namespace steamrot {
/////////////////////////////////////////////////
SubscriberFactory::SubscriberFactory(EventHandler &event_handler)
    : m_event_handler(event_handler) {}

/////////////////////////////////////////////////
std::expected<std::shared_ptr<Subscriber>, FailInfo>
SubscriberFactory::CreateAndRegisterSubscriber(const EventType &event_type) {

  std::shared_ptr<Subscriber> subscriber =
      std::make_shared<Subscriber>(event_type);

  auto result = m_event_handler.RegisterSubscriber(subscriber);
  if (!result.has_value())
    return std::unexpected(result.error());

  return subscriber;
}

/////////////////////////////////////////////////
std::expected<std::shared_ptr<Subscriber>, FailInfo>
SubscriberFactory::CreateAndRegisterSubscriber(const EventType &event_type, const std::optional<EventData>& trigger_event) {

  std::shared_ptr<Subscriber> subscriber =
      std::make_shared<Subscriber>(event_type, trigger_event);

  auto result = m_event_handler.RegisterSubscriber(subscriber);
  if (!result.has_value())
    return std::unexpected(result.error());

  return subscriber;
}
} // namespace steamrot
