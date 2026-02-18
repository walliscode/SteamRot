/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventHandler.h"
#include "EventPayload.h"
#include "FailInfo.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <expected>
#include <optional>
#include <variant>
namespace steamrot {
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
EventHandler::RegisterSubscriber(std::shared_ptr<Subscriber> subscriber) {

  // get the event type the subscriber is interested in and add it to the
  // register
  auto event_type = subscriber->event_type;
  m_subscriber_register[event_type].push_back(subscriber);

  return std::monostate{};
}
/////////////////////////////////////////////////
void EventHandler::PreloadEvents(sf::RenderWindow &window) {

  // handle SMFL events and pass to the waiting room event bus
  HandleSFMLEvents(window, *this);
}

/////////////////////////////////////////////////
void EventHandler::ProcessWaitingRoomEventBus() {
  // add all events from the waiting room event bus to the global event bus
  AddToGlobalEventBus(m_waiting_room_event_bus);
  // clear the waiting room event bus
  m_waiting_room_event_bus.clear();
}
/////////////////////////////////////////////////
void EventHandler::AddToGlobalEventBus(const std::vector<EventPacket> &events) {

  for (const auto &event : events) {
    m_global_event_bus.push_back(event);
  }
}
/////////////////////////////////////////////////
void EventHandler::TickGlobalEventBus() {
  // decrement the lifetime of all events in the global event bus
  DecrementEventLifetimes(m_global_event_bus);
  // remove all events with a lifetime of 0
  RemoveDeadEvents(m_global_event_bus);
}

/////////////////////////////////////////////////
const EventBus &EventHandler::GetGlobalEventBus() const {
  // return the global event bus
  return m_global_event_bus;
}

/////////////////////////////////////////////////
const EventBus &EventHandler::GetWaitingRoomEventBus() {
  // return the waiting room event bus
  return m_waiting_room_event_bus;
}

/////////////////////////////////////////////////
void HandleSFMLEvents(sf::RenderWindow &window, EventHandler &event_handler) {}

/////////////////////////////////////////////////
const std::unordered_map<EventType, std::vector<std::weak_ptr<Subscriber>>> &
EventHandler::GetSubcriberRegister() const {
  return m_subscriber_register;
}

/////////////////////////////////////////////////
void EventHandler::UpdateSubscribersFromGlobalEventBus() {
  // go through each event in the global event bus
  for (const auto &event : m_global_event_bus) {

    if (m_subscriber_register.contains(event.type)) {

      // go through each subscriber registered for the event type
      for (auto &subscriber_weak : m_subscriber_register.at(event.type)) {

        // pass to the UpdateSubscriber function
        UpdateSubscriber(subscriber_weak, event.payload);
      }
    }
  }
}
/////////////////////////////////////////////////
void EventHandler::AddEvent(const EventPacket &event) {
  m_waiting_room_event_bus.push_back(event);
}

/////////////////////////////////////////////////
void DecrementLifteime(EventPacket &event) {
  if (event.context.lifetime > 0) {
    --event.context.lifetime;
  }
}

/////////////////////////////////////////////////
void DecrementEventLifetimes(EventBus &event_bus) {
  for (auto &event : event_bus) {
    DecrementLifteime(event);
  }
}

/////////////////////////////////////////////////
void RemoveDeadEvents(EventBus &event_bus) {
  event_bus.erase(std::remove_if(event_bus.begin(), event_bus.end(),
                                 [](const EventPacket &event) {
                                   return event.context.lifetime == 0;
                                 }),
                  event_bus.end());
}

/////////////////////////////////////////////////
void UpdateSubscriber(std::weak_ptr<Subscriber> &subscriber,
                      const EventPayload &event_payload) {

  auto locked_subscriber = subscriber.lock();
  if (!locked_subscriber)
    // if the Subscriber has expired, do nothing
    return;

  // if the Subscriber has filter payload, compare against the event payload
  if (locked_subscriber->filter_payload.has_value()) {
    const auto &filter_payload_data = locked_subscriber->filter_payload.value();

    // Compare filter payload with event payload for exact equality
    // [TODO:] implement Matches functions for Subscribers
  }

  // activate the subscriber and store the received event data
  locked_subscriber->m_active = true;
  locked_subscriber->captured_payload = event_payload;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> CleanExpiredSubscribers(
    std::unordered_map<EventType, std::vector<std::weak_ptr<Subscriber>>>
        &subscriber_register) {
  for (auto &[event_type, subscribers] : subscriber_register) {
    // remove expired subscribers from the vector
    subscribers.erase(
        std::remove_if(subscribers.begin(), subscribers.end(),
                       [](const std::weak_ptr<Subscriber> &subscriber_weak) {
                         return subscriber_weak.expired();
                       }),
        subscribers.end());
  }
  return std::monostate{};
}
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ResetAllSubscribers(
    std::unordered_map<EventType, std::vector<std::weak_ptr<Subscriber>>>
        &subscriber_register) {
  for (auto &[event_type, subscribers] : subscriber_register) {
    for (auto &subscriber_weak : subscribers) {
      auto locked_subscriber = subscriber_weak.lock();
      locked_subscriber->m_active = false;
    }
  }
  return std::monostate{};
}

/////////////////////////////////////////////////
void EventHandler::ExecuteEventHandlerLevelLogic(sf::RenderWindow &window) {
  // clean expired subscribers from the register
  auto clean_result = CleanExpiredSubscribers(m_subscriber_register);
  // Reset all subscribers at the start of the frame
  auto reset_result = ResetAllSubscribers(m_subscriber_register);
  // Preload events from the window into the waiting room event bus
  PreloadEvents(window);
  // Process the waiting room event bus into the global event bus
  ProcessWaitingRoomEventBus();
  // Update subscribers based on the global event bus
  UpdateSubscribersFromGlobalEventBus();
  // Tick the global event bus to manage event lifetimes
  TickGlobalEventBus();
}
} // namespace steamrot
