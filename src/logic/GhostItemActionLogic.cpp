/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GhostItemActionLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GhostItemActionLogic.h"
#include "EventContext.h"
#include "EventPacket.h"
#include "EventPayload.h"
#include "EventType.h"
#include "GhostItemState.h"
#include "Subscriber.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
GhostItemActionLogic::GhostItemActionLogic(const SceneContext scene_context)
    : Logic(scene_context) {

  // Subscribe to SELECT_GHOST_ITEM logic events (item picked up from UI)
  auto select_subscriber = std::make_shared<Subscriber>();
  select_subscriber->event_type = EventType::LOGIC;
  select_subscriber->filter_payload =
      LogicPayload(LogicPayload::LogicToggle::SELECT_GHOST_ITEM);
  m_subscribers.push_back(select_subscriber);
  m_scene_context.event_handler.RegisterSubscriber(select_subscriber);

  // Subscribe to CLEAR_GHOST_ITEM logic events (ghost cancelled)
  auto clear_subscriber = std::make_shared<Subscriber>();
  clear_subscriber->event_type = EventType::LOGIC;
  clear_subscriber->filter_payload =
      LogicPayload(LogicPayload::LogicToggle::CLEAR_GHOST_ITEM);
  m_subscribers.push_back(clear_subscriber);
  m_scene_context.event_handler.RegisterSubscriber(clear_subscriber);

  // Subscribe to USER_INPUT SELECT events (left click = place)
  auto input_subscriber = std::make_shared<Subscriber>();
  input_subscriber->event_type = EventType::USER_INPUT;
  input_subscriber->filter_payload =
      InputPayload(InputPayload::InputAction::SELECT);
  m_subscribers.push_back(input_subscriber);
  m_scene_context.event_handler.RegisterSubscriber(input_subscriber);
}

/////////////////////////////////////////////////
void GhostItemActionLogic::ProcessLogic() {

  GhostItemState &ghost_state = m_scene_context.ghost_item_state;

  // Update ghost position to follow the mouse whenever it is active
  if (ghost_state.m_is_active) {
    ghost_state.m_position = sf::Vector2f(m_scene_context.mouse_position);
  }

  // Process all subscribers
  for (auto &subscriber : m_subscribers) {

    if (!subscriber->m_active) {
      continue;
    }

    // --- Logic events (SELECT / CLEAR) ---
    if (subscriber->event_type == EventType::LOGIC &&
        subscriber->captured_payload.has_value() &&
        std::holds_alternative<LogicPayload>(
            subscriber->captured_payload.value())) {

      LogicPayload &payload =
          std::get<LogicPayload>(subscriber->captured_payload.value());

      if (payload.toggle_name ==
          LogicPayload::LogicToggle::SELECT_GHOST_ITEM) {
        ghost_state.m_item_key = payload.item_key;
        ghost_state.m_is_active = true;
        ghost_state.m_position =
            sf::Vector2f(m_scene_context.mouse_position);
      }

      if (payload.toggle_name ==
          LogicPayload::LogicToggle::CLEAR_GHOST_ITEM) {
        ghost_state.m_item_key = std::nullopt;
        ghost_state.m_is_active = false;
      }
    }

    // --- Input SELECT event → place the ghost item ---
    if (subscriber->event_type == EventType::USER_INPUT &&
        ghost_state.m_is_active) {

      // Build and fire a PLACE_GHOST_ITEM logic event so that scene-specific
      // Logic classes can react to the placement
      LogicPayload place_payload(LogicPayload::LogicToggle::PLACE_GHOST_ITEM);
      place_payload.item_key = ghost_state.m_item_key;

      EventContext context{1};
      EventPacket place_event{context, EventType::LOGIC, place_payload};
      m_scene_context.event_handler.AddEvent(place_event);

      // Clear the ghost state
      ghost_state.m_item_key = std::nullopt;
      ghost_state.m_is_active = false;
    }
  }
}

} // namespace steamrot::logic
