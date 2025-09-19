////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "EventHandler.h"
#include "FailInfo.h"
#include <SFML/Window/Event.hpp>
#include <expected>
#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <optional>
namespace steamrot {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
EventHandler::RegisterSubscriber(std::shared_ptr<Subscriber> subscriber) {

  switch (subscriber->GetRegistrationInfo().first) {
  case EventType::EventType_EVENT_USER_INPUT: {
    // ensure the variant holds a UserInputBitset as these bits of data are not
    // definitely coupled
    if (std::holds_alternative<UserInputBitset>(
            subscriber->GetRegistrationInfo().second)) {
      const UserInputBitset user_input_bitset =
          std::get<UserInputBitset>(subscriber->GetRegistrationInfo().second);
      m_user_input_register[user_input_bitset].push_back(subscriber);
    } else {
      // Handle the case where the variant does not hold UserInputBitset
      return std::unexpected(
          FailInfo{FailMode::VariantTypeMismatch,
                   "Subscriber with EventType_EVENT_USER_INPUT must have "
                   "UserInputBitset as event data."});
    }
    break;
  }
  // handle scene change subscribers
  case EventType::EventType_EVENT_CHANGE_SCENE: {
    m_change_scene_subscribers.push_back(subscriber);
    break;
  }

  case EventType::EventType_EVENT_NONE: {
    std::cout << "Subscriber has NONE event type, no registration needed."
              << std::endl;
    // no registration needed for NONE event type
    break;
  }
  default:

    // if the event type is not handled, return an error
    return std::unexpected(
        FailInfo{FailMode::EnumValueNotHandled,
                 "EventType not handled in Subscriber registration."});
  }
  std::cout << "Subscriber registered for event type: "
            << magic_enum::enum_name(subscriber->GetRegistrationInfo().first)
            << std::endl;
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

  // create a new UserInputBitset
  UserInputBitset user_input_events;
  // reset the event bitsets
  user_input_events.reset();

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
    // handle keyboard events
    else if (event->is<sf::Event::KeyPressed>() ||
             event->is<sf::Event::KeyReleased>()) {

      HandleKeyboardEvents(event.value(), user_input_events);
    }
    // handle mouse events
    else if (event->is<sf::Event::MouseButtonPressed>() ||
             event->is<sf::Event::MouseButtonReleased>()) {
      HandleMouseEvents(event.value(), user_input_events);
    }
  }

  // create a new event packet with a lifetime of 1
  EventPacket event_packet(1);

  // set the event type to UserInputEvent
  event_packet.m_event_type = EventType::EventType_EVENT_USER_INPUT;

  // set the data for the event packet
  event_packet.m_event_data = user_input_events;

  // add the event packet to the global event bus
  AddEvent(m_global_event_bus, event_packet);
}

////////////////////////////////////////////////////////////
void EventHandler::HandleKeyboardEvents(const sf::Event &event,
                                        UserInputBitset &user_input_events) {

  // if key pressed, set the corresponding bit in the pressed events bitset
  if (const auto *keyPressed = event.getIf<sf::Event::KeyPressed>()) {

    std::cout << "Key pressed: " << magic_enum::enum_name(keyPressed->scancode)
              << std::endl;
    std::cout << "Key code: " << static_cast<int>(keyPressed->code)
              << std::endl;
    // modify relevant bitset
    user_input_events.set(static_cast<size_t>(keyPressed->code));

  } else if (const auto *keyReleased = event.getIf<sf::Event::KeyReleased>()) {

    std::cout << "Key released: " << magic_enum::enum_name(keyReleased->code)
              << std::endl;

    // if key released, then remember add the key count
    user_input_events.set(static_cast<size_t>(keyReleased->code) +
                          sf::Keyboard::KeyCount);
  }
}

////////////////////////////////////////////////////////////
void EventHandler::HandleMouseEvents(const sf::Event &event,
                                     UserInputBitset &user_input_events) {

  // if mouse button pressed, set the corresponding bit in the pressed
  // events
  if (const auto *mouseButtonPressed =
          event.getIf<sf::Event::MouseButtonPressed>()) {

    std::cout << "Mouse button pressed: "
              << magic_enum::enum_name(mouseButtonPressed->button) << std::endl;
    // shift the mouse button by the number of keys x 2
    user_input_events.set(static_cast<size_t>(mouseButtonPressed->button) +
                          static_cast<size_t>(sf::Keyboard::KeyCount * 2));

  } else if (const auto *mouseButtonReleased =
                 event.getIf<sf::Event::MouseButtonReleased>()) {

    std::cout << "Mouse button released: "
              << magic_enum::enum_name(mouseButtonReleased->button)
              << std::endl;
    // if mouse button released, then remember to add the key count x 2 and
    // the mouse button count
    user_input_events.set(static_cast<size_t>(mouseButtonReleased->button) +
                          static_cast<size_t>((sf::Keyboard::KeyCount * 2) +
                                              sf::Mouse::ButtonCount));
  }
}

/////////////////////////////////////////////////
const std::unordered_map<UserInputBitset,
                         std::vector<std::weak_ptr<Subscriber>>> &
EventHandler::GetUserInputRegister() const {
  return m_user_input_register;
}

/////////////////////////////////////////////////
void EventHandler::UpateSubscribersFromGlobalEventBus() {
  // go through each event in the global event bus
  for (const auto &event : m_global_event_bus) {
    switch (event.m_event_type) {
    case EventType::EventType_EVENT_USER_INPUT: {

      // get the vector of subscribers interested in this event
      auto subscriber_vec =
          m_user_input_register[std::get<UserInputBitset>(event.m_event_data)];

      // go through each subscriber and update them
      for (auto &subscriber : subscriber_vec) {
        UpdateSubscriber(subscriber);
      }

      break;
    }
    default:
      // if the event type is not handled, do nothing
      break;
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
void UpdateSubscriber(std::weak_ptr<Subscriber> &subscriber) {
  // update any releveant information for the subscriber
  auto activate_result = subscriber.lock()->SetActive();
}
} // namespace steamrot
