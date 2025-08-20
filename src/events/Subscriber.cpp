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
Subscriber::Subscriber(const EventType event_type, const EventData &event_data)
    : m_event_type(event_type), m_event_data(event_data) {}

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
} // namespace steamrot
