////////////////////////////////////////////////////////////
// Preprocessor directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////

#include "EventPacket.h"
#include "Subscriber.h"
#include "events_generated.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

namespace steamrot {

using EventBus = std::vector<EventPacket>;

class EventHandler {
private:
  /////////////////////////////////////////////////
  /// @brief EventBus from which Subscribers will be updated.
  /////////////////////////////////////////////////
  EventBus m_global_event_bus;

  /////////////////////////////////////////////////
  /// @brief  EventBus for collating all generated Events
  /////////////////////////////////////////////////
  EventBus m_waiting_room_event_bus;
  /////////////////////////////////////////////////
  /// @brief Register of all subscribers
  /////////////////////////////////////////////////
  std::unordered_map<EventType, std::vector<std::weak_ptr<Subscriber>>>
      m_subscriber_register;

  /////////////////////////////////////////////////
  /// @brief Wrapper function to specifally to add to the global event bus.
  ///
  /// @param events Vector of events to be added to the global event bus.
  /////////////////////////////////////////////////
  void AddToGlobalEventBus(const std::vector<EventPacket> &events);

public:
  ////////////////////////////////////////////////////////////
  // |brief default constructor
  ////////////////////////////////////////////////////////////
  EventHandler() = default;

  /////////////////////////////////////////////////
  /// @brief store a subscriber in the event handler.
  ///
  /// @param subscriber Shared pointer to the subscriber to be registered.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  RegisterSubscriber(const std::shared_ptr<Subscriber> subscriber);

  /////////////////////////////////////////////////
  /// @brief Handle all events pre Scene updates.
  /////////////////////////////////////////////////
  void PreloadEvents(sf::RenderWindow &window);

  /////////////////////////////////////////////////
  /// @brief Adds an event to the waiting room event bus.
  ///
  /// @param event Newly created event to be added to the waiting room event
  /// bus.
  /////////////////////////////////////////////////
  void AddEvent(const EventPacket &event);

  /////////////////////////////////////////////////
  /// @brief Add all events from the waiting room event bus to the global event
  /// bus
  /////////////////////////////////////////////////
  void ProcessWaitingRoomEventBus();

  /////////////////////////////////////////////////
  /// @brief Update all subscribers based on the events in the global event bus.
  /////////////////////////////////////////////////
  void UpateSubscribersFromGlobalEventBus();

  /////////////////////////////////////////////////
  /// @brief Wrapper function for handling lifetimes and removing dead events.
  /////////////////////////////////////////////////
  void TickGlobalEventBus();

  /////////////////////////////////////////////////
  /// @brief Get the global event bus.
  ///
  /// @return A reference to the global event bus.
  /////////////////////////////////////////////////
  const EventBus &GetGlobalEventBus();

  /////////////////////////////////////////////////
  /// @brief Return the subscriber register.
  /////////////////////////////////////////////////
  const std::unordered_map<EventType, std::vector<std::weak_ptr<Subscriber>>> &
  GetSubcriberRegister() const;
};

/////////////////////////////////////////////////
/// @brief Decrement the lifetime of a single event by 1.
///
/// @param event Event to decrement the lifetime of.
/////////////////////////////////////////////////
void DecrementLifteime(EventPacket &event);

/////////////////////////////////////////////////
/// @brief Go over all events in the event bus and decrement their lifetimes.
/////////////////////////////////////////////////
void DecrementEventLifetimes(EventBus &event_bus);

/////////////////////////////////////////////////
/// @brief Remove all events with a lifetimve of 0 from the event bus.
///
/////////////////////////////////////////////////
void RemoveDeadEvents(EventBus &event_bus);

/////////////////////////////////////////////////
/// @brief Update releveant informtion for a subscriber object
///
/// @param subscriber Weak pointer to the subscriber to be updated.
/////////////////////////////////////////////////
void UpdateSubscriber(std::weak_ptr<Subscriber> &subscriber,
                      const EventData &event_data);

/////////////////////////////////////////////////
/// @brief Adapater function to turn SFML events into the game engine's event
/// system.
///
/// @param window Reference to the SFML window to poll events from.
/// @param event_bus Reference to the event bus to add events to.
/////////////////////////////////////////////////
void HandleSFMLEvents(sf::RenderWindow &window, EventHandler &event_handler);
} // namespace steamrot
