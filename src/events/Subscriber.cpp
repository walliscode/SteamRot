/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the Subscriber class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Subscriber.h"

namespace steamrot {

/////////////////////////////////////////////////
Subscriber::Subscriber(const EventType trigger_event_type)
    : m_trigger_event_type(trigger_event_type) {};

/////////////////////////////////////////////////
Subscriber::Subscriber(const EventType trigger_event_type,
                       const EventData &trigger_event_data)
    : m_trigger_event_type(trigger_event_type),
      m_trigger_event_data(trigger_event_data) {};

} // namespace steamrot
