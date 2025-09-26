/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the SubscriberFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SubscriberFactory.h"
#include "Subscriber.h"
#include "event_helpers.h"
#include "subscriber_config_generated.h"
#include <memory>

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
SubscriberFactory::CreateAndRegisterSubscriber(const EventType &event_type,
                                               const EventData &trigger_data) {
  std::shared_ptr<Subscriber> subscriber =
      std::make_shared<Subscriber>(event_type, trigger_data);
  auto result = m_event_handler.RegisterSubscriber(subscriber);
  if (!result.has_value())
    return std::unexpected(result.error());
  return subscriber;
}
/////////////////////////////////////////////////
std::expected<std::shared_ptr<Subscriber>, FailInfo>
SubscriberFactory::CreateAndRegisterSubscriber(
    const SubscriberData &subscriber_data) {

  // create subscriber pointer
  std::shared_ptr<Subscriber> subscriber{nullptr};

  // if EventType is NONE, return nullptr
  if (subscriber_data.event_type_data() == EventType::EventType_NONE)
    return subscriber;

  // set EventType
  EventType event_type = subscriber_data.event_type_data();

  // check for trigger data
  if (subscriber_data.trigger_data()) {

    // convert flatbuffers data to EventData
    auto convert_result = ConvertFlatbuffersEventDataDataToEventData(
        subscriber_data.trigger_data_type(), subscriber_data.trigger_data());
    if (!convert_result.has_value()) {
      return std::unexpected(convert_result.error());
    }
    EventData trigger_data = convert_result.value();

    subscriber = std::make_shared<Subscriber>(event_type, trigger_data);

    // if no trigger data, create subscriber without it
  } else {

    subscriber = std::make_shared<Subscriber>(event_type);
  }
  // register subscriber
  auto result = m_event_handler.RegisterSubscriber(subscriber);
  if (!result.has_value())
    return std::unexpected(result.error());

  return subscriber;
}
} // namespace steamrot
