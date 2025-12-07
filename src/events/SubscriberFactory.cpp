/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the subscriber_factory namespace
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SubscriberFactory.h"
#include "Subscriber.h"
#include "event_factory.h"
#include "subscriber_config_generated.h"
#include <memory>

namespace steamrot {
namespace subscriber_factory {

/////////////////////////////////////////////////
std::expected<std::shared_ptr<Subscriber>, FailInfo>
CreateAndRegisterSubscriber(const EventType &event_type,
                            EventHandler &event_handler) {

  std::shared_ptr<Subscriber> subscriber =
      std::make_shared<Subscriber>(event_type);

  auto result = event_handler.RegisterSubscriber(subscriber);
  if (!result.has_value())
    return std::unexpected(result.error());

  return subscriber;
}

/////////////////////////////////////////////////
std::expected<std::shared_ptr<Subscriber>, FailInfo>
CreateAndRegisterSubscriber(const EventType &event_type,
                            const EventData &trigger_data,
                            EventHandler &event_handler) {
  std::shared_ptr<Subscriber> subscriber =
      std::make_shared<Subscriber>(event_type, trigger_data);
  
  auto result = event_handler.RegisterSubscriber(subscriber);
  if (!result.has_value())
    return std::unexpected(result.error());
  
  return subscriber;
}

/////////////////////////////////////////////////
std::expected<std::shared_ptr<Subscriber>, FailInfo>
CreateAndRegisterSubscriber(const SubscriberConfig &config,
                            EventHandler &event_handler) {

  // create subscriber pointer
  std::shared_ptr<Subscriber> subscriber{nullptr};

  // if EventType is NONE, return nullptr
  if (config.trigger_event_type == EventType::EventType_NONE)
    return subscriber;

  // check for trigger data
  if (config.trigger_event_data.has_value()) {
    subscriber = std::make_shared<Subscriber>(config.trigger_event_type,
                                               config.trigger_event_data.value());
  } else {
    subscriber = std::make_shared<Subscriber>(config.trigger_event_type);
  }

  // check if subscriber should start active
  if (config.active) {
    subscriber->m_active = true;
  }

  // register subscriber
  auto result = event_handler.RegisterSubscriber(subscriber);
  if (!result.has_value())
    return std::unexpected(result.error());

  return subscriber;
}

/////////////////////////////////////////////////
std::expected<std::shared_ptr<Subscriber>, FailInfo>
CreateAndRegisterSubscriber(const SubscriberConfigFbs &subscriber_config_fbs,
                            EventHandler &event_handler) {

  // create subscriber pointer
  std::shared_ptr<Subscriber> subscriber{nullptr};

  // if EventType is NONE, return nullptr
  if (subscriber_config_fbs.event_type_data() == EventType::EventType_NONE)
    return subscriber;

  // set EventType
  EventType event_type = subscriber_config_fbs.event_type_data();

  // check for trigger data
  if (subscriber_config_fbs.trigger_data()) {

    // convert flatbuffers data to EventData
    auto convert_result =
        event::CreateEventData(subscriber_config_fbs.trigger_data_type(),
                               subscriber_config_fbs.trigger_data());
    if (!convert_result.has_value()) {
      return std::unexpected(convert_result.error());
    }
    EventData trigger_data = convert_result.value();

    subscriber = std::make_shared<Subscriber>(event_type, trigger_data);

    // if no trigger data, create subscriber without it
  } else {

    subscriber = std::make_shared<Subscriber>(event_type);
  }
  // check if subscriber should start active
  if (subscriber_config_fbs.active()) {
    subscriber->m_active = true;
  }
  // register subscriber
  auto result = event_handler.RegisterSubscriber(subscriber);
  if (!result.has_value())
    return std::unexpected(result.error());

  return subscriber;
}

} // namespace subscriber_factory
} // namespace steamrot
