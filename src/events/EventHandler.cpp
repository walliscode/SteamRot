/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventHandler.h"
#include "EventPayload.h"
#include "FailInfo.h"
#include "payload_matchers.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <expected>
#include <optional>
#include <variant>
namespace steamrot {
/////////////////////////////////////////////////
void EventHandler::SetInputActionRegistry(InputActionRegistry &&registry) {
  m_sfml_event_converter.SetInputActionRegistry(std::move(registry));
}

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
void HandleSFMLEvents(sf::RenderWindow &window, EventHandler &event_handler) {
  // Collect all SFML events from the window for this tick.
  std::vector<sf::Event> sfml_events;
  while (auto event = window.pollEvent()) {
    sfml_events.push_back(*event);
  }

  // Route SFML events through the conversion pipeline.
  event_handler.ConvertSFMLEventsToEventPackets(sfml_events);
}

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

  // if Subscriber event type is None, return without activating
  if (locked_subscriber->event_type == EventType::NONE)
    return;

  // match the event payload to the subscriber filter payload.
  locked_subscriber->m_active =
      events::MatchPayload(locked_subscriber->filter_payload, event_payload);

  if (locked_subscriber->m_active) {
    // if the subscriber is active, capture the event payload
    locked_subscriber->captured_payload = event_payload;
  } else {
    // if the subscriber is not active, clear the captured payload
    locked_subscriber->captured_payload = std::nullopt;
  }
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
void EventHandler::ConvertSFMLEventsToEventPackets(
    const std::vector<sf::Event> &sfml_events) {
  // Delegate to SFMLEventConverter which orchestrates all conversion logic.
  auto converted_events = m_sfml_event_converter.ConvertSFMLEvents(sfml_events);
  for (const auto &event : converted_events) {
    AddEvent(event);
  }
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
EventHandler::ExecuteEventHandlerLevelLogic(sf::RenderWindow &window) {
  // clean expired subscribers from the register
  auto clean_result = CleanExpiredSubscribers(m_subscriber_register);
  if (!clean_result.has_value())
    return std::unexpected(clean_result.error());
  // Reset all subscribers at the start of the frame
  auto reset_result = ResetAllSubscribers(m_subscriber_register);
  if (!reset_result.has_value())
    return std::unexpected(reset_result.error());
  // Preload events from the window into the waiting room event bus
  PreloadEvents(window);
  // Process the waiting room event bus into the global event bus
  ProcessWaitingRoomEventBus();
  // Update subscribers based on the global event bus
  UpdateSubscribersFromGlobalEventBus();
  // Tick the global event bus to manage event lifetimes
  TickGlobalEventBus();
  return std::monostate{};
}
} // namespace steamrot
