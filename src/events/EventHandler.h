////////////////////////////////////////////////////////////
// Preprocessor directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////

#include "EventPacket.h"
#include "Subscriber.h"
#include "event_helpers.h"
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
  /// @brief This will be the only event bus used in the game engine.
  /////////////////////////////////////////////////
  EventBus m_global_event_bus;

  /////////////////////////////////////////////////
  /// @brief Register of subscribers interested in user input events.
  /////////////////////////////////////////////////
  std::unordered_map<UserInputBitset, std::vector<std::weak_ptr<Subscriber>>>
      m_user_input_register;

  ////////////////////////////////////////////////////////////
  // |brief process keyboard events: pressed and released
  ////////////////////////////////////////////////////////////
  void HandleKeyboardEvents(const sf::Event &event,
                            UserInputBitset &user_input_events);

  ////////////////////////////////////////////////////////////
  // |brief process mouse events: pressed and released
  ////////////////////////////////////////////////////////////
  void HandleMouseEvents(const sf::Event &event,
                         UserInputBitset &user_input_events);

  /////////////////////////////////////////////////
  /// @brief Go over all events in the event bus and decrement their lifetimes.
  /////////////////////////////////////////////////
  void DecrementEventLifetimes();

  /////////////////////////////////////////////////
  /// @brief Remove all events with a lifetimve of 0 from the event bus.
  ///
  /////////////////////////////////////////////////
  void RemoveDeadEvents();

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
  /// @brief Handle all events that dont originate from the Scenes/Logic
  /// [TODO: Give this a better name]
  /////////////////////////////////////////////////
  void PreloadEvents(sf::RenderWindow &window);

  /////////////////////////////////////////////////
  /// @brief Container function for all clean up/tear down functions at the end
  /// of the tick.
  /////////////////////////////////////////////////
  void CleanUpEventBus();

  /////////////////////////////////////////////////
  /// @brief Adds an event to the global event bus and calls any sorting algos.
  ///
  /// @param event Newly created event to be added to the event bus.
  /////////////////////////////////////////////////
  void AddEvent(const EventPacket &event);

  /////////////////////////////////////////////////
  /// @brief Add a collection of events to the global event bus.
  ///
  /// @param events A holding bus of events to be added to the global event bus.
  /////////////////////////////////////////////////
  void AddEvents(const EventBus &events);

  /////////////////////////////////////////////////
  /// @brief Get the global event bus.
  ///
  /// @return A reference to the global event bus.
  /////////////////////////////////////////////////
  const EventBus &GetEventBus();

  /////////////////////////////////////////////////
  /// @brief Adapater function to turn SFML events into the game engine's event
  /// system.
  ///
  /// @param window Reference to the SFML window to poll events from.
  /////////////////////////////////////////////////
  void HandleSFMLEvents(sf::RenderWindow &window);

  /////////////////////////////////////////////////
  /// @brief Return the user input register.
  /////////////////////////////////////////////////
  const std::unordered_map<UserInputBitset,
                           std::vector<std::weak_ptr<Subscriber>>> &
  GetUserInputRegister() const;
};
} // namespace steamrot
