////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "EventHandler.h"
#include "FailInfo.h"
#include <SFML/Window/Event.hpp>
#include <expected>
#include <optional>
namespace steamrot {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
EventHandler::RegisterSubscriber(std::shared_ptr<Subscriber> subscriber) {

  // get the event type the subscriber is interested in and add it to the
  // register
  auto event_type = subscriber->GetRegistrationInfo().first;
  m_subscriber_register[event_type].push_back(subscriber);

  return std::monostate{};
}
/////////////////////////////////////////////////
void EventHandler::PreloadEvents(sf::RenderWindow &window) {

  // handle SMFL events
  HandleSFMLEvents(window);
}

/////////////////////////////////////////////////
void EventHandler::AddToGlobalEventBus(const std::vector<EventPacket> &events) {

  for (const auto &event : events) {
    AddEvent(m_global_event_bus, event);
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

const EventBus &EventHandler::GetGlobalEventBus() {
  // return the global event bus
  return m_global_event_bus;
}

////////////////////////////////////////////////////////////
void EventHandler::HandleSFMLEvents(sf::RenderWindow &window) {

  // create a vector to hold user input events
  std::vector<sf::Event> user_input_events;

  // poll events from the window
  while (const std::optional<sf::Event> event = window.pollEvent()) {
    // add in ctrl + c to close the window
    if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
      if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
        window.close();
    } // if the event is a close event, set the close window flag to true
    if (event->is<sf::Event::Closed>()) {
      window.close();
    }
    // handle keyboard and mouse events by adding to the user input events
    // vector
    if (event->is<sf::Event::KeyPressed>() ||
        event->is<sf::Event::KeyReleased>() ||
        event->is<sf::Event::MouseButtonPressed>() ||
        event->is<sf::Event::MouseButtonReleased>()) {
      user_input_events.push_back(*event);
    }
  }

  // specifically handle keyboard and mouse events
  if (!user_input_events.empty()) {

    // create a new event packet with a lifetime of 1
    EventPacket event_packet(1);

    // set the event type to UserInputEvent
    event_packet.m_event_type = EventType::EventType_EVENT_USER_INPUT;

    // set the data for the event packet
    event_packet.m_event_data = user_input_events;

    // add the event packet to the global event bus
    AddEvent(m_global_event_bus, event_packet);
  }
}

/////////////////////////////////////////////////
const std::unordered_map<EventType, std::vector<std::weak_ptr<Subscriber>>> &
EventHandler::GetSubcriberRegister() const {
  return m_subscriber_register;
}

/////////////////////////////////////////////////
void EventHandler::UpateSubscribersFromGlobalEventBus() {
  // go through each event in the global event bus
  for (const auto &event : m_global_event_bus) {
    if (m_subscriber_register.contains(event.m_event_type)) {
      // go through each subscriber registered for the event type
      for (auto &subscriber_weak :
           m_subscriber_register.at(event.m_event_type)) {

        // pass to the UpdateSubscriber function
        UpdateSubscriber(subscriber_weak, event.m_event_data);
      }
    }
  }
}
/////////////////////////////////////////////////
void AddEvent(EventBus &event_bus, const EventPacket &event) {
  event_bus.push_back(event);
}

/////////////////////////////////////////////////
void DecrementLifteime(EventPacket &event) {
  if (event.event_lifetime > 0) {
    --event.event_lifetime;
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
                                   return event.event_lifetime == 0;
                                 }),
                  event_bus.end());
}

/////////////////////////////////////////////////
void UpdateSubscriber(std::weak_ptr<Subscriber> &subscriber,
                      const EventData &event_data) {
  // Check if subscriber should be activated based on trigger condition
  auto subscriber_ptr = subscriber.lock();
  if (!subscriber_ptr) {
    return; // Subscriber no longer exists
  }
  
  if (subscriber_ptr->ShouldActivate(event_data)) {
    // update any releveant information for the subscriber
    auto activate_result = subscriber_ptr->SetActive();

    // copy the event data to the subscriber
    subscriber_ptr->SetEventData(event_data);
  }
}
} // namespace steamrot
