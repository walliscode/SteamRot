/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the Subscriber class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Subscriber.h"
#include "FailInfo.h"
#include <expected>
#include <variant>

namespace steamrot {

/////////////////////////////////////////////////
Subscriber::Subscriber(const EventType event_type)
    : m_event_type(event_type) {};

/////////////////////////////////////////////////
Subscriber::Subscriber(const EventType event_type, const std::optional<EventData>& trigger_event)
    : m_event_type(event_type), m_trigger_event(trigger_event) {};

/////////////////////////////////////////////////
std::pair<EventType, EventData> Subscriber::GetRegistrationInfo() const {
  return {m_event_type, m_event_data};
}

/////////////////////////////////////////////////
bool Subscriber::IsActive() const { return m_active; }

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> Subscriber::SetActive() {

  m_active = true;

  if (!m_active) {
    return std::unexpected(FailInfo{FailMode::ParameterOutOfBounds,
                                    "Failed to activate subscriber."});
  }
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> Subscriber::SetInactive() {

  m_active = false;
  if (m_active) {
    return std::unexpected(FailInfo{FailMode::ParameterOutOfBounds,
                                    "Failed to deactivate subscriber."});
  }
  return std::monostate{};
}
/////////////////////////////////////////////////
void Subscriber::UpdateEventData(const EventData &new_event_data) {
  m_event_data = new_event_data;
}

/////////////////////////////////////////////////
const EventType &Subscriber::GetEventType() const { return m_event_type; }

/////////////////////////////////////////////////
const EventData &Subscriber::GetEventData() const { return m_event_data; }

/////////////////////////////////////////////////
void Subscriber::SetEventData(const EventData &event_data) {
  m_event_data = event_data;
}

/////////////////////////////////////////////////
const std::optional<EventData>& Subscriber::GetTriggerEvent() const {
  return m_trigger_event;
}

/////////////////////////////////////////////////
bool Subscriber::ShouldActivate(const EventData& event_data) const {
  // If no trigger is set, always activate
  if (!m_trigger_event.has_value()) {
    return true;
  }
  
  // If trigger is set, check if it matches the provided event data
  return m_trigger_event.value() == event_data;
}

} // namespace steamrot
