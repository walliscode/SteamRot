////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "EventHandler.h"
#include "FailInfo.h"
#include "event_helpers.h"
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
    // get the user input bitset from the subscriber
    const UserInputBitset user_input_bitset =
        std::get<UserInputBitset>(subscriber->GetRegistrationInfo().second);
    // add the subscriber to the user input register
    m_user_input_register[user_input_bitset].push_back(subscriber);
    break;
  }
  default:
    // if the event type is not handled, return an error
    return std::unexpected(
        FailInfo{FailMode::EnumValueNotHandled,
                 "Event type not handled in Subscriber registration."});
  }
  return std::monostate{};
}
/////////////////////////////////////////////////
void EventHandler::PreloadEvents(sf::RenderWindow &window) {

  // handle SMFL events
  HandleSFMLEvents(window);
}
/////////////////////////////////////////////////
void EventHandler::CleanUpEventBus() {
  // decrement the lifetime of all events in the event bus
  DecrementEventLifetimes();
  // remove all events with a lifetime of 0
  RemoveDeadEvents();
}

/////////////////////////////////////////////////

const EventBus &EventHandler::GetEventBus() {
  // return the global event bus
  return m_global_event_bus;
}

/////////////////////////////////////////////////
void EventHandler::DecrementEventLifetimes() {
  // go over all events in the event bus and decrement their lifetimes
  for (auto &event : m_global_event_bus) {
    event.DecrementLifetime();
  }
}
/////////////////////////////////////////////////
void EventHandler::RemoveDeadEvents() {
  // remove all events with a lifetime of 0
  m_global_event_bus.erase(std::remove_if(m_global_event_bus.begin(),
                                          m_global_event_bus.end(),
                                          [](const EventPacket &event) {
                                            return event.GetLifetime() == 0;
                                          }),
                           m_global_event_bus.end());
}

/////////////////////////////////////////////////
void EventHandler::AddEvent(const EventPacket &event) {
  // add the event to the global event bus
  m_global_event_bus.push_back(event);

  // add any sorting algorithms here if needed
}

/////////////////////////////////////////////////
void EventHandler::AddEvents(const std::vector<EventPacket> &events) {

  // add each one separately, so that any sorting algorithms do not to be
  // defined twice
  for (const auto &event : events) {
    // add each event to the global event bus
    AddEvent(event);
  }
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
  AddEvent(event_packet);
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
                         std::vector<std::weak_ptr<Subscriber>>>
EventHandler::GetUserInputRegister() const {
  return m_user_input_register;
}
} // namespace steamrot
