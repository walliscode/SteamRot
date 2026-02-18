# Event System Implementation Plan

## Executive Summary

This document provides a comprehensive analysis, documentation, and detailed implementation plan for enhancing the SteamRot event system. The user has recently switched to a new event system architecture, which has caused tests and systems to break. This plan addresses the following requirements:

1. **Event Factory**: Free functions for creating EventPackets from parameters with type-safe payload matching
2. **Random Event Generator**: Function for producing random EventPackets for testing
3. **EventPacket Matcher Verification**: Ensure the matcher is fully functional
4. **Payload Matching Functions**: In-game code for matching filter payloads against incoming payloads (for Subscriber activation)

---

## Current System Architecture Analysis

### Core Event System Components

#### 1. EventPacket (`src/types/events/EventPacket.h`)

```cpp
struct EventPacket {
  EventContext context;      // Lifetime management (ticks down each frame)
  EventType type;            // Broad event category
  EventPayload payload;      // Variant with specific payload data
};
```

**Purpose**: Primary event container that flows through the event system.

#### 2. EventPayload (`src/types/events/EventPayload.h`)

A `std::variant` containing five payload types:
- `InputPayload` - User input events (keyboard/mouse actions)
- `UIPayload` - User interface events (toggle UI states, etc.)
- `LogicPayload` - Logic system events (toggle flow control)
- `ScenePayload` - Scene management events (scene changes)
- `SystemPayload` - System-level events (quit game, etc.)

**Example Payloads**:

```cpp
struct InputPayload {
  enum class InputAction { NONE, SELECT } action;
};

struct UIPayload {
  std::optional<std::string> c_user_interface_name;
  std::optional<std::string> c_ui_state_name;
  enum class UIAction { TOGGLE } action;
};

struct LogicPayload {
  enum class LogicToggle {
    INITIATE_MACHINA_FORM_SCAFFOLD,
    CLEAR_MACHINA_FORM_SCAFFOLD
  } toggle_name;
};

struct ScenePayload {
  enum class SceneAction { CHANGE } action;
  std::optional<SceneType> scene_type;
  std::optional<uuids::uuid> scene_id;
};

struct SystemPayload {
  enum class SystemAction { QUIT } action;
};
```

#### 3. EventType (`src/types/events/EventType.h`)

```cpp
enum class EventType : uint64_t {
  NONE = 0,
  USER_INPUT,
  UI,
  SCENE,
  LOGIC,
  SYSTEM
};
```

**Purpose**: Coarse-grained event classification for efficient subscriber filtering.

#### 4. Subscriber (`src/types/events/Subscriber.h`)

```cpp
struct Subscriber {
  bool m_active{false};                                    // Activation flag
  EventType event_type{EventType::NONE};                   // Type filter
  std::optional<EventPayload> filter_payload{std::nullopt}; // Payload filter
  std::optional<EventPayload> captured_payload{std::nullopt}; // Received data
};
```

**Purpose**: Lightweight listener that gets activated when matching events occur.

**Filtering Logic**:
- Subscribers register for a specific `EventType`
- Optional `filter_payload` enables fine-grained filtering
- When activated, `captured_payload` stores the actual event data
- Systems check `m_active` flag to determine if subscriber was triggered

#### 5. EventHandler (`src/events/EventHandler.h`)

**Key Methods**:
- `RegisterSubscriber()` - Register subscribers for event types
- `AddEvent()` - Add event to waiting room bus
- `ProcessWaitingRoomEventBus()` - Move events to global bus
- `UpdateSubscribersFromGlobalEventBus()` - Activate matching subscribers
- `TickGlobalEventBus()` - Decrement lifetimes and remove dead events
- `ExecuteEventHandlerLevelLogic()` - Full frame cycle

**Event Flow**:
1. Events added to waiting room bus via `AddEvent()`
2. Moved to global bus via `ProcessWaitingRoomEventBus()`
3. Subscribers updated via `UpdateSubscribersFromGlobalEventBus()`
4. Event lifetimes decremented and dead events removed via `TickGlobalEventBus()`

**Critical TODO** (line 133 in `EventHandler.cpp`):
```cpp
// [TODO:] implement Matches functions for Subscribers
```

Currently, the code checks for `filter_payload` but doesn't perform actual matching:
```cpp
void UpdateSubscriber(std::weak_ptr<Subscriber> &subscriber,
                      const EventPayload &event_payload) {
  auto locked_subscriber = subscriber.lock();
  if (!locked_subscriber)
    return;

  // if the Subscriber has filter payload, compare against the event payload
  if (locked_subscriber->filter_payload.has_value()) {
    const auto &filter_payload_data = locked_subscriber->filter_payload.value();
    // [TODO:] implement Matches functions for Subscribers
  }

  // activate the subscriber and store the received event data
  locked_subscriber->m_active = true;
  locked_subscriber->captured_payload = event_payload;
}
```

### Existing Infrastructure

#### Configuration Functions (`src/data_providers/configure/configure_event.h`)

Comprehensive FlatBuffers configuration functions already exist:
- `ConfigureEventContext()` - Configure lifetime
- `ConfigureInputPayload()` - Configure input payload
- `ConfigureUIPayload()` - Configure UI payload
- `ConfigureLogicPayload()` - Configure logic payload
- `ConfigureScenePayload()` - Configure scene payload
- `ConfigureSystemPayload()` - Configure system payload
- `ConfigureEventType()` - Convert FlatBuffers enum
- `ConfigureEventPayload()` - Dispatch to correct payload configurator
- `ConfigureEventPacket()` - Configure complete event packet

**Purpose**: Load event data from FlatBuffers (JSON/binary) format.

#### Matchers (`tests/matchers/`)

Two Catch2 matchers exist for testing:

**EventPacketEqualsMatcher**:
- Compares EventType
- Compares EventPayload (delegates to EventPayloadEqualsMatcher)
- Compares EventContext.lifetime
- Provides detailed mismatch descriptions with colors

**EventPayloadEqualsMatcher**:
- Checks variant type matches
- Compares all fields for each payload type
- Handles optional fields (strings, UUIDs)
- Provides detailed mismatch descriptions

**Status**: Both matchers appear comprehensive and well-implemented.

#### Test Infrastructure

**Test Files**:
- `tests/unit/events/EventHandler.test.cpp` - Comprehensive EventHandler tests
- `tests/unit/types/events/EventPacket.test.cpp` - Basic EventPacket tests
- `tests/unit/types/events/Subscriber.test.cpp` - Subscriber tests
- `tests/unit/data_providers/configure_event.test.cpp` - Configuration tests
- `tests/unit/matchers/EventPacketEqualsMatcher.test.cpp` - Matcher tests
- `tests/unit/matchers/EventPayloadEqualsMatcher.test.cpp` - Matcher tests

**Test Coverage**:
- EventHandler registration, event buses, subscriber updates
- Subscriber activation with and without filter payloads
- Event lifetime management
- Configuration from FlatBuffers data

---

## Requirements Analysis

### Requirement 1: Event Factory

**Need**: Free functions for creating EventPackets from parameters.

**Current Gap**: 
- No centralized way to create EventPackets programmatically
- Tests create EventPackets manually with direct initialization
- No type-safe builders for ensuring payload/type consistency

**Example Current Usage** (from tests):
```cpp
steamrot::EventPacket event{5};  // Lifetime only
event.type = steamrot::EventType::USER_INPUT;
event.payload = steamrot::EventPayload{
    steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT}};
```

**Desired Usage**:
```cpp
// Factory functions
auto event = steamrot::CreateInputEvent(
    steamrot::InputPayload::InputAction::SELECT, 
    5 /* lifetime */);

auto event = steamrot::CreateUIEvent(
    "my_ui", 
    "my_state", 
    steamrot::UIPayload::UIAction::TOGGLE,
    3 /* lifetime */);
```

### Requirement 2: Random Event Generator

**Need**: Function for producing random EventPackets for testing event propagation.

**Current Gap**: No random event generation capability.

**Use Case**: 
- Testing event bus propagation
- Stress testing EventHandler
- Fuzzing subscriber matching logic
- Integration tests

**Desired Usage**:
```cpp
auto random_event = steamrot::CreateRandomEventPacket();
event_handler.AddEvent(random_event);
// Verify event was propagated correctly
```

### Requirement 3: EventPacket Matcher Verification

**Need**: Ensure EventPacket matcher is fully functional.

**Current Status**: 
- `EventPacketEqualsMatcher` exists and appears comprehensive
- `EventPayloadEqualsMatcher` exists and appears comprehensive
- Test coverage exists but may need expansion

**Assessment**: Matchers appear functional but should be verified with:
- Edge case tests (empty optionals, all enum values)
- Integration tests with factory functions
- Random event comparison tests

### Requirement 4: Payload Matching Functions

**Need**: In-game code that matches filter payloads against incoming payloads.

**Current Gap**: 
- `UpdateSubscriber()` has TODO comment
- No matching logic implemented
- Subscribers always activate regardless of filter

**Critical for**: Determining if a Subscriber should activate based on payload content.

**Design Considerations**:

**Exact Match vs. Partial Match**:
- Should filter match ALL fields or ANY field?
- How to handle optional fields in filter?

**Proposed Semantics**:
- **Exact Match**: All non-empty fields in filter must match exactly
- **Optional Handling**: If filter field is empty (nullopt), any value matches
- **Example**: 
  ```cpp
  // Filter
  UIPayload filter;
  filter.c_user_interface_name = "crafting_ui";
  filter.c_ui_state_name = std::nullopt;  // Don't care
  filter.action = UIPayload::UIAction::TOGGLE;
  
  // Incoming
  UIPayload incoming;
  incoming.c_user_interface_name = "crafting_ui";    // ✓ Matches
  incoming.c_ui_state_name = "any_state";            // ✓ Matches (filter doesn't care)
  incoming.action = UIPayload::UIAction::TOGGLE;     // ✓ Matches
  
  // Result: MATCH
  ```

---

## Detailed Implementation Plan

### Phase 1: Event Factory Functions

#### Files to Create

**`src/events/event_factory.h`**

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of factory functions for creating EventPackets
/////////////////////////////////////////////////

#pragma once

#include "EventPacket.h"
#include "EventPayload.h"
#include "EventType.h"
#include "SceneType.h"
#include "uuid.h"
#include <optional>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Create an InputPayload with specified action
///
/// @param action The input action type
/// @return Constructed InputPayload
/////////////////////////////////////////////////
InputPayload CreateInputPayload(InputPayload::InputAction action);

/////////////////////////////////////////////////
/// @brief Create a UIPayload with specified parameters
///
/// @param ui_name Optional name of the UI element
/// @param state_name Optional name of the UI state
/// @param action The UI action type
/// @return Constructed UIPayload
/////////////////////////////////////////////////
UIPayload CreateUIPayload(
    std::optional<std::string> ui_name,
    std::optional<std::string> state_name,
    UIPayload::UIAction action);

/////////////////////////////////////////////////
/// @brief Create a LogicPayload with specified toggle
///
/// @param toggle The logic toggle type
/// @return Constructed LogicPayload
/////////////////////////////////////////////////
LogicPayload CreateLogicPayload(LogicPayload::LogicToggle toggle);

/////////////////////////////////////////////////
/// @brief Create a ScenePayload with specified parameters
///
/// @param action The scene action type
/// @param scene_type Optional scene type
/// @param scene_id Optional scene UUID
/// @return Constructed ScenePayload
/////////////////////////////////////////////////
ScenePayload CreateScenePayload(
    ScenePayload::SceneAction action,
    std::optional<SceneType> scene_type = std::nullopt,
    std::optional<uuids::uuid> scene_id = std::nullopt);

/////////////////////////////////////////////////
/// @brief Create a SystemPayload with specified action
///
/// @param action The system action type
/// @return Constructed SystemPayload
/////////////////////////////////////////////////
SystemPayload CreateSystemPayload(SystemPayload::SystemAction action);

/////////////////////////////////////////////////
/// @brief Create an EventPacket with InputPayload
///
/// @param action The input action type
/// @param lifetime Event lifetime in ticks (default: 1)
/// @return Constructed EventPacket
/////////////////////////////////////////////////
EventPacket CreateInputEvent(
    InputPayload::InputAction action,
    uint8_t lifetime = 1);

/////////////////////////////////////////////////
/// @brief Create an EventPacket with UIPayload
///
/// @param ui_name Optional name of the UI element
/// @param state_name Optional name of the UI state
/// @param action The UI action type
/// @param lifetime Event lifetime in ticks (default: 1)
/// @return Constructed EventPacket
/////////////////////////////////////////////////
EventPacket CreateUIEvent(
    std::optional<std::string> ui_name,
    std::optional<std::string> state_name,
    UIPayload::UIAction action,
    uint8_t lifetime = 1);

/////////////////////////////////////////////////
/// @brief Create an EventPacket with LogicPayload
///
/// @param toggle The logic toggle type
/// @param lifetime Event lifetime in ticks (default: 1)
/// @return Constructed EventPacket
/////////////////////////////////////////////////
EventPacket CreateLogicEvent(
    LogicPayload::LogicToggle toggle,
    uint8_t lifetime = 1);

/////////////////////////////////////////////////
/// @brief Create an EventPacket with ScenePayload
///
/// @param action The scene action type
/// @param scene_type Optional scene type
/// @param scene_id Optional scene UUID
/// @param lifetime Event lifetime in ticks (default: 1)
/// @return Constructed EventPacket
/////////////////////////////////////////////////
EventPacket CreateSceneEvent(
    ScenePayload::SceneAction action,
    std::optional<SceneType> scene_type = std::nullopt,
    std::optional<uuids::uuid> scene_id = std::nullopt,
    uint8_t lifetime = 1);

/////////////////////////////////////////////////
/// @brief Create an EventPacket with SystemPayload
///
/// @param action The system action type
/// @param lifetime Event lifetime in ticks (default: 1)
/// @return Constructed EventPacket
/////////////////////////////////////////////////
EventPacket CreateSystemEvent(
    SystemPayload::SystemAction action,
    uint8_t lifetime = 1);

/////////////////////////////////////////////////
/// @brief Create a random EventPacket for testing
///
/// Generates a random event with random payload type and random data.
/// Useful for testing event propagation and subscriber matching.
///
/// @return Randomly generated EventPacket
/////////////////////////////////////////////////
EventPacket CreateRandomEventPacket();

} // namespace steamrot
```

**`src/events/event_factory.cpp`**

Implementation outline:
- Each payload factory function constructs the payload with given parameters
- Each event factory function:
  1. Creates the appropriate payload
  2. Constructs EventContext with specified lifetime
  3. Sets correct EventType to match payload
  4. Returns complete EventPacket
- `CreateRandomEventPacket()`:
  1. Randomly select EventType (excluding NONE)
  2. Generate random payload data for that type
  3. Random lifetime (1-5 ticks)
  4. Return constructed EventPacket

**Random Generation Strategy**:
```cpp
EventPacket CreateRandomEventPacket() {
  // Use std::random_device and std::mt19937 for random generation
  std::random_device rd;
  std::mt19937 gen(rd());
  
  // Random event type (1-5, excluding NONE=0)
  std::uniform_int_distribution<> type_dist(1, 5);
  int type_value = type_dist(gen);
  
  // Random lifetime (1-5)
  std::uniform_int_distribution<> lifetime_dist(1, 5);
  uint8_t lifetime = static_cast<uint8_t>(lifetime_dist(gen));
  
  // Generate event based on type
  switch (type_value) {
  case 1: // USER_INPUT
    // Random InputAction
    return CreateInputEvent(/* random action */, lifetime);
  case 2: // UI
    // Random UIPayload fields
    return CreateUIEvent(/* random params */, lifetime);
  // ... etc
  }
}
```

#### Testing Strategy

**`tests/unit/events/event_factory.test.cpp`**

Test cases:
1. **Payload Factory Tests**
   - Each payload type constructed correctly
   - Optional fields handled properly
   - Enum values set correctly

2. **Event Factory Tests**
   - Correct EventType set for each payload
   - Lifetime set correctly
   - Payload variant holds correct type
   - Default lifetime (1) works

3. **Random Event Tests**
   - Generate 100 random events
   - Verify all are valid (type matches payload)
   - Verify lifetime in expected range
   - Verify random distribution (all types generated)

4. **Integration with Matchers**
   - Create event with factory
   - Create identical event manually
   - Verify matcher says they're equal

#### CMakeLists.txt Updates

Add to `src/events/CMakeLists.txt`:
```cmake
target_sources(events PRIVATE
  EventHandler.cpp
  event_factory.cpp  # Add this
)
```

Add to `tests/unit/events/CMakeLists.txt`:
```cmake
add_executable(test_events
  EventHandler.test.cpp
  event_factory.test.cpp  # Add this
)
```

---

### Phase 2: Random Event Generator

**Status**: Implemented as part of Phase 1 (`CreateRandomEventPacket()`).

**Additional Testing**:

**`tests/unit/events/random_event_propagation.test.cpp`**

Test event propagation with random events:
```cpp
TEST_CASE("Random events propagate through EventHandler", 
          "[unit][event_factory][integration]") {
  steamrot::EventHandler handler;
  
  // Generate 50 random events
  std::vector<steamrot::EventPacket> events;
  for (int i = 0; i < 50; ++i) {
    events.push_back(steamrot::CreateRandomEventPacket());
  }
  
  // Add all to handler
  for (const auto& event : events) {
    handler.AddEvent(event);
  }
  
  // Process waiting room
  handler.ProcessWaitingRoomEventBus();
  
  // Verify all in global bus
  auto& global_bus = handler.GetGlobalEventBus();
  REQUIRE(global_bus.size() == 50);
  
  // Verify match original events
  for (size_t i = 0; i < 50; ++i) {
    REQUIRE_THAT(global_bus[i], EqualsEventPacket(events[i]));
  }
}
```

---

### Phase 3: EventPacket Matcher Verification

#### Review Existing Matchers

**EventPacketEqualsMatcher** (`tests/matchers/EventPacketEqualsMatcher.h/cpp`):
- ✓ Compares EventType
- ✓ Compares EventPayload via EventPayloadEqualsMatcher
- ✓ Compares EventContext.lifetime
- ✓ Provides detailed mismatch descriptions

**EventPayloadEqualsMatcher** (`tests/matchers/EventPayloadEqualsMatcher.h/cpp`):
- ✓ Checks variant index
- ✓ InputPayload: Compares action
- ✓ UIPayload: Compares c_user_interface_name, c_ui_state_name, action
- ✓ LogicPayload: Compares toggle_name
- ✓ ScenePayload: Compares action, scene_type, scene_id
- ✓ SystemPayload: Compares action

**Assessment**: Matchers appear complete.

#### Additional Test Coverage

**`tests/unit/matchers/EventPacketEqualsMatcher.test.cpp`** (expand existing):

Add test cases:
1. **All EventTypes**: Test each type explicitly
2. **All Payload Types**: Test each payload type explicitly
3. **Edge Cases**:
   - Empty optionals vs. filled optionals
   - Different lifetimes
   - All enum values for each action type
4. **Mismatch Detection**:
   - Type mismatch
   - Payload mismatch
   - Lifetime mismatch
   - Optional field mismatch

**`tests/unit/matchers/EventPayloadEqualsMatcher.test.cpp`** (expand existing):

Add test cases:
1. **Variant Type Mismatch**: All combinations
2. **Field Mismatch**: Each field independently
3. **Optional Field Edge Cases**:
   - Both empty
   - One empty, one filled
   - Both filled, same value
   - Both filled, different values

---

### Phase 4: Payload Matching Functions

This is the most critical phase - implementing the TODO in `EventHandler.cpp`.

#### Design: Payload Matching Semantics

**Matching Rules**:
1. **Variant Type**: Filter and event payload must be same variant type
2. **Required Fields**: All non-optional fields in filter must match exactly
3. **Optional Fields**: 
   - If filter field is `nullopt`, any event value matches (don't care)
   - If filter field has value, event must have same value
4. **Enums**: Must match exactly

**Examples**:

```cpp
// Example 1: UIPayload with partial matching
UIPayload filter;
filter.c_user_interface_name = "menu";
filter.c_ui_state_name = std::nullopt;  // Don't care
filter.action = UIPayload::UIAction::TOGGLE;

UIPayload event1;
event1.c_user_interface_name = "menu";
event1.c_ui_state_name = "main";
event1.action = UIPayload::UIAction::TOGGLE;
// Matches(filter, event1) -> true

UIPayload event2;
event2.c_user_interface_name = "menu";
event2.c_ui_state_name = "settings";
event2.action = UIPayload::UIAction::TOGGLE;
// Matches(filter, event2) -> true (state doesn't matter)

UIPayload event3;
event3.c_user_interface_name = "inventory";
event3.c_ui_state_name = "main";
event3.action = UIPayload::UIAction::TOGGLE;
// Matches(filter, event3) -> false (name differs)
```

#### Files to Create

**`src/events/payload_matcher.h`**

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of payload matching functions for Subscriber filtering
/////////////////////////////////////////////////

#pragma once

#include "EventPayload.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Check if an InputPayload matches a filter
///
/// @param filter The filter payload
/// @param event The incoming event payload
/// @return true if event matches filter, false otherwise
/////////////////////////////////////////////////
bool Matches(const InputPayload& filter, const InputPayload& event);

/////////////////////////////////////////////////
/// @brief Check if a UIPayload matches a filter
///
/// Matching rules:
/// - action must match exactly
/// - If filter.c_user_interface_name has value, must match event
/// - If filter.c_ui_state_name has value, must match event
/// - nullopt in filter means "don't care" (matches any value)
///
/// @param filter The filter payload
/// @param event The incoming event payload
/// @return true if event matches filter, false otherwise
/////////////////////////////////////////////////
bool Matches(const UIPayload& filter, const UIPayload& event);

/////////////////////////////////////////////////
/// @brief Check if a LogicPayload matches a filter
///
/// @param filter The filter payload
/// @param event The incoming event payload
/// @return true if event matches filter, false otherwise
/////////////////////////////////////////////////
bool Matches(const LogicPayload& filter, const LogicPayload& event);

/////////////////////////////////////////////////
/// @brief Check if a ScenePayload matches a filter
///
/// Matching rules:
/// - action must match exactly
/// - If filter.scene_type has value, must match event
/// - If filter.scene_id has value, must match event
/// - nullopt in filter means "don't care" (matches any value)
///
/// @param filter The filter payload
/// @param event The incoming event payload
/// @return true if event matches filter, false otherwise
/////////////////////////////////////////////////
bool Matches(const ScenePayload& filter, const ScenePayload& event);

/////////////////////////////////////////////////
/// @brief Check if a SystemPayload matches a filter
///
/// @param filter The filter payload
/// @param event The incoming event payload
/// @return true if event matches filter, false otherwise
/////////////////////////////////////////////////
bool Matches(const SystemPayload& filter, const SystemPayload& event);

/////////////////////////////////////////////////
/// @brief Check if an EventPayload (variant) matches a filter
///
/// Dispatches to appropriate typed Matches function based on variant type.
/// Returns false if filter and event are different variant types.
///
/// @param filter The filter payload variant
/// @param event The incoming event payload variant
/// @return true if event matches filter, false otherwise
/////////////////////////////////////////////////
bool Matches(const EventPayload& filter, const EventPayload& event);

} // namespace steamrot
```

**`src/events/payload_matcher.cpp`**

Implementation outline:

```cpp
#include "payload_matcher.h"
#include <variant>

namespace steamrot {

/////////////////////////////////////////////////
bool Matches(const InputPayload& filter, const InputPayload& event) {
  // InputPayload has only one field: action
  return filter.action == event.action;
}

/////////////////////////////////////////////////
bool Matches(const UIPayload& filter, const UIPayload& event) {
  // Action must always match
  if (filter.action != event.action) {
    return false;
  }
  
  // Check c_user_interface_name
  if (filter.c_user_interface_name.has_value()) {
    if (!event.c_user_interface_name.has_value() ||
        filter.c_user_interface_name.value() != event.c_user_interface_name.value()) {
      return false;
    }
  }
  
  // Check c_ui_state_name
  if (filter.c_ui_state_name.has_value()) {
    if (!event.c_ui_state_name.has_value() ||
        filter.c_ui_state_name.value() != event.c_ui_state_name.value()) {
      return false;
    }
  }
  
  return true;
}

/////////////////////////////////////////////////
bool Matches(const LogicPayload& filter, const LogicPayload& event) {
  // LogicPayload has only one field: toggle_name
  return filter.toggle_name == event.toggle_name;
}

/////////////////////////////////////////////////
bool Matches(const ScenePayload& filter, const ScenePayload& event) {
  // Action must always match
  if (filter.action != event.action) {
    return false;
  }
  
  // Check scene_type
  if (filter.scene_type.has_value()) {
    if (!event.scene_type.has_value() ||
        filter.scene_type.value() != event.scene_type.value()) {
      return false;
    }
  }
  
  // Check scene_id
  if (filter.scene_id.has_value()) {
    if (!event.scene_id.has_value() ||
        filter.scene_id.value() != event.scene_id.value()) {
      return false;
    }
  }
  
  return true;
}

/////////////////////////////////////////////////
bool Matches(const SystemPayload& filter, const SystemPayload& event) {
  // SystemPayload has only one field: action
  return filter.action == event.action;
}

/////////////////////////////////////////////////
bool Matches(const EventPayload& filter, const EventPayload& event) {
  // First check if variant types match
  if (filter.index() != event.index()) {
    return false;
  }
  
  // Dispatch to appropriate typed Matches function
  if (std::holds_alternative<InputPayload>(filter)) {
    return Matches(std::get<InputPayload>(filter), 
                   std::get<InputPayload>(event));
  } else if (std::holds_alternative<UIPayload>(filter)) {
    return Matches(std::get<UIPayload>(filter), 
                   std::get<UIPayload>(event));
  } else if (std::holds_alternative<LogicPayload>(filter)) {
    return Matches(std::get<LogicPayload>(filter), 
                   std::get<LogicPayload>(event));
  } else if (std::holds_alternative<ScenePayload>(filter)) {
    return Matches(std::get<ScenePayload>(filter), 
                   std::get<ScenePayload>(event));
  } else if (std::holds_alternative<SystemPayload>(filter)) {
    return Matches(std::get<SystemPayload>(filter), 
                   std::get<SystemPayload>(event));
  }
  
  // Should never reach here
  return false;
}

} // namespace steamrot
```

#### Update EventHandler

**`src/events/EventHandler.cpp`** - Update `UpdateSubscriber()`:

```cpp
#include "payload_matcher.h"  // Add include

void UpdateSubscriber(std::weak_ptr<Subscriber> &subscriber,
                      const EventPayload &event_payload) {

  auto locked_subscriber = subscriber.lock();
  if (!locked_subscriber)
    return;

  // if the Subscriber has filter payload, compare against the event payload
  if (locked_subscriber->filter_payload.has_value()) {
    const auto &filter_payload_data = locked_subscriber->filter_payload.value();
    
    // Use Matches function to check if event matches filter
    if (!Matches(filter_payload_data, event_payload)) {
      // Event doesn't match filter, don't activate
      return;
    }
  }

  // activate the subscriber and store the received event data
  locked_subscriber->m_active = true;
  locked_subscriber->captured_payload = event_payload;
}
```

#### Testing Strategy

**`tests/unit/events/payload_matcher.test.cpp`**

Comprehensive test coverage:

1. **InputPayload Matching**
   ```cpp
   TEST_CASE("Matches InputPayload with same action", "[unit][payload_matcher]") {
     steamrot::InputPayload filter{steamrot::InputPayload::InputAction::SELECT};
     steamrot::InputPayload event{steamrot::InputPayload::InputAction::SELECT};
     REQUIRE(steamrot::Matches(filter, event));
   }
   
   TEST_CASE("Matches InputPayload with different action", "[unit][payload_matcher]") {
     steamrot::InputPayload filter{steamrot::InputPayload::InputAction::SELECT};
     steamrot::InputPayload event{steamrot::InputPayload::InputAction::NONE};
     REQUIRE_FALSE(steamrot::Matches(filter, event));
   }
   ```

2. **UIPayload Matching**
   - Action must match
   - Optional fields with nullopt (don't care)
   - Optional fields with value (must match)
   - Optional fields mismatch
   - All combinations

3. **LogicPayload Matching**
   - All toggle types
   - Match and mismatch cases

4. **ScenePayload Matching**
   - Action must match
   - Optional scene_type handling
   - Optional scene_id handling
   - UUID matching

5. **SystemPayload Matching**
   - All action types

6. **EventPayload Variant Matching**
   - Different variant types (should not match)
   - Same variant types (delegate to typed function)
   - All payload combinations

7. **Integration with Subscriber**
   ```cpp
   TEST_CASE("Subscriber activated only when filter matches", 
             "[unit][payload_matcher][integration]") {
     // Create subscriber with filter
     auto subscriber = std::make_shared<steamrot::Subscriber>();
     subscriber->event_type = steamrot::EventType::UI;
     
     steamrot::UIPayload filter;
     filter.c_user_interface_name = "menu";
     filter.c_ui_state_name = std::nullopt;  // Don't care
     filter.action = steamrot::UIPayload::UIAction::TOGGLE;
     subscriber->filter_payload = filter;
     
     // Matching event
     steamrot::UIPayload matching_event;
     matching_event.c_user_interface_name = "menu";
     matching_event.c_ui_state_name = "main";
     matching_event.action = steamrot::UIPayload::UIAction::TOGGLE;
     
     std::weak_ptr<steamrot::Subscriber> weak_sub = subscriber;
     steamrot::UpdateSubscriber(weak_sub, matching_event);
     REQUIRE(subscriber->m_active);
     
     // Reset
     subscriber->m_active = false;
     
     // Non-matching event
     steamrot::UIPayload nonmatching_event;
     nonmatching_event.c_user_interface_name = "inventory";
     nonmatching_event.c_ui_state_name = "main";
     nonmatching_event.action = steamrot::UIPayload::UIAction::TOGGLE;
     
     steamrot::UpdateSubscriber(weak_sub, nonmatching_event);
     REQUIRE_FALSE(subscriber->m_active);
   }
   ```

#### CMakeLists.txt Updates

Add to `src/events/CMakeLists.txt`:
```cmake
target_sources(events PRIVATE
  EventHandler.cpp
  event_factory.cpp
  payload_matcher.cpp  # Add this
)
```

Add to `tests/unit/events/CMakeLists.txt`:
```cmake
add_executable(test_events
  EventHandler.test.cpp
  event_factory.test.cpp
  payload_matcher.test.cpp  # Add this
)
```

---

### Phase 5: Integration and Validation

#### Update Broken Tests

**Identify failing tests**:
```bash
ctest --preset Debug -R event --output-on-failure
```

**Common failures likely include**:
- Tests that expect subscribers to always activate (now filtered)
- Tests that don't set up filter payloads correctly
- Tests that create events manually (should use factory)

#### Refactor Tests to Use Factory

**Before**:
```cpp
steamrot::EventPacket event{2};
event.type = steamrot::EventType::USER_INPUT;
event.payload = steamrot::EventPayload{
    steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT}};
```

**After**:
```cpp
auto event = steamrot::CreateInputEvent(
    steamrot::InputPayload::InputAction::SELECT, 2);
```

#### End-to-End Integration Test

**`tests/integration/event_system/event_propagation.integration.test.cpp`**

```cpp
TEST_CASE("Full event system integration", "[integration][event_system]") {
  // Create EventHandler
  steamrot::EventHandler handler;
  
  // Create subscribers with filters
  auto ui_subscriber = std::make_shared<steamrot::Subscriber>();
  ui_subscriber->event_type = steamrot::EventType::UI;
  ui_subscriber->filter_payload = steamrot::CreateUIPayload(
      "menu", std::nullopt, steamrot::UIPayload::UIAction::TOGGLE);
  handler.RegisterSubscriber(ui_subscriber);
  
  auto input_subscriber = std::make_shared<steamrot::Subscriber>();
  input_subscriber->event_type = steamrot::EventType::USER_INPUT;
  // No filter - catches all USER_INPUT events
  handler.RegisterSubscriber(input_subscriber);
  
  // Create events using factory
  auto ui_event = steamrot::CreateUIEvent(
      "menu", "main", steamrot::UIPayload::UIAction::TOGGLE, 2);
  auto input_event = steamrot::CreateInputEvent(
      steamrot::InputPayload::InputAction::SELECT, 1);
  auto other_ui_event = steamrot::CreateUIEvent(
      "inventory", "main", steamrot::UIPayload::UIAction::TOGGLE, 1);
  
  // Add events
  handler.AddEvent(ui_event);
  handler.AddEvent(input_event);
  handler.AddEvent(other_ui_event);
  
  // Process
  handler.ProcessWaitingRoomEventBus();
  handler.UpdateSubscribersFromGlobalEventBus();
  
  // Verify
  REQUIRE(ui_subscriber->m_active);  // "menu" matches filter
  REQUIRE(input_subscriber->m_active);  // No filter, matches all input
  
  // Verify captured payloads
  REQUIRE(ui_subscriber->captured_payload.has_value());
  auto captured_ui = std::get<steamrot::UIPayload>(
      ui_subscriber->captured_payload.value());
  REQUIRE(captured_ui.c_user_interface_name == "menu");
}
```

#### Documentation Updates

**Files to update**:
1. **`README.md`** - Add event factory usage examples
2. **`documentation/examples/EVENT_SYSTEM_USAGE.md`** (new) - Comprehensive guide
3. **`.github/agents/copilot_instructions.md`** - Add event factory patterns

**Example documentation**:
```markdown
## Event System Usage

### Creating Events

Use factory functions for type-safe event creation:

```cpp
#include "event_factory.h"

// Input event
auto input_event = steamrot::CreateInputEvent(
    steamrot::InputPayload::InputAction::SELECT);

// UI event
auto ui_event = steamrot::CreateUIEvent(
    "my_ui",           // UI name (optional)
    "my_state",        // State name (optional)
    steamrot::UIPayload::UIAction::TOGGLE,
    3);                // Lifetime (default: 1)

// Scene change event
auto scene_event = steamrot::CreateSceneEvent(
    steamrot::ScenePayload::SceneAction::CHANGE,
    steamrot::SceneType::CRAFTING);

// System quit event
auto quit_event = steamrot::CreateSystemEvent(
    steamrot::SystemPayload::SystemAction::QUIT);
```

### Subscriber Filtering

Subscribers can filter events by payload content:

```cpp
// Create subscriber
auto subscriber = std::make_shared<steamrot::Subscriber>();
subscriber->event_type = steamrot::EventType::UI;

// Set filter - only activate for "crafting_ui"
subscriber->filter_payload = steamrot::CreateUIPayload(
    "crafting_ui",     // Must match
    std::nullopt,      // Don't care about state
    steamrot::UIPayload::UIAction::TOGGLE);

// Register with handler
event_handler.RegisterSubscriber(subscriber);
```

### Testing with Random Events

Generate random events for testing:

```cpp
for (int i = 0; i < 100; ++i) {
  auto random_event = steamrot::CreateRandomEventPacket();
  event_handler.AddEvent(random_event);
}
```
```

---

## Summary of Required Changes

### New Files (8 files)

1. `src/events/event_factory.h` - Factory function declarations
2. `src/events/event_factory.cpp` - Factory function implementations
3. `src/events/payload_matcher.h` - Matching function declarations
4. `src/events/payload_matcher.cpp` - Matching function implementations
5. `tests/unit/events/event_factory.test.cpp` - Factory tests
6. `tests/unit/events/payload_matcher.test.cpp` - Matcher tests
7. `tests/unit/events/random_event_propagation.test.cpp` - Random event tests
8. `tests/integration/event_system/event_propagation.integration.test.cpp` - Integration tests

### Modified Files (4 files)

1. `src/events/EventHandler.cpp` - Add payload matching to `UpdateSubscriber()`
2. `src/events/CMakeLists.txt` - Add new source files
3. `tests/unit/events/CMakeLists.txt` - Add new test files
4. `tests/unit/matchers/EventPacketEqualsMatcher.test.cpp` - Expand test coverage (optional)

### Documentation Updates (3 files)

1. `README.md` - Add event factory usage
2. `documentation/examples/EVENT_SYSTEM_USAGE.md` (new) - Comprehensive guide
3. This file - Implementation plan

---

## Implementation Checklist

Use this checklist to track progress:

### Phase 1: Event Factory
- [ ] Create `src/events/event_factory.h`
- [ ] Create `src/events/event_factory.cpp`
- [ ] Implement payload factory functions
- [ ] Implement event factory functions
- [ ] Implement `CreateRandomEventPacket()`
- [ ] Update `src/events/CMakeLists.txt`
- [ ] Create `tests/unit/events/event_factory.test.cpp`
- [ ] Write tests for all factory functions
- [ ] Update `tests/unit/events/CMakeLists.txt`
- [ ] Run tests: `ctest --preset Debug -R event_factory`
- [ ] Verify all tests pass

### Phase 2: Random Event Generator
- [ ] Verify `CreateRandomEventPacket()` implementation
- [ ] Create `tests/unit/events/random_event_propagation.test.cpp`
- [ ] Write random event propagation tests
- [ ] Update `tests/unit/events/CMakeLists.txt`
- [ ] Run tests: `ctest --preset Debug -R random_event`
- [ ] Verify all tests pass

### Phase 3: Matcher Verification
- [ ] Review `EventPacketEqualsMatcher` implementation
- [ ] Review `EventPayloadEqualsMatcher` implementation
- [ ] Expand `tests/unit/matchers/EventPacketEqualsMatcher.test.cpp`
- [ ] Expand `tests/unit/matchers/EventPayloadEqualsMatcher.test.cpp`
- [ ] Add edge case tests
- [ ] Run tests: `ctest --preset Debug -R Matcher`
- [ ] Verify all tests pass

### Phase 4: Payload Matching
- [ ] Create `src/events/payload_matcher.h`
- [ ] Create `src/events/payload_matcher.cpp`
- [ ] Implement `Matches()` for `InputPayload`
- [ ] Implement `Matches()` for `UIPayload`
- [ ] Implement `Matches()` for `LogicPayload`
- [ ] Implement `Matches()` for `ScenePayload`
- [ ] Implement `Matches()` for `SystemPayload`
- [ ] Implement `Matches()` for `EventPayload` (variant dispatcher)
- [ ] Update `src/events/CMakeLists.txt`
- [ ] Update `src/events/EventHandler.cpp` - `UpdateSubscriber()`
- [ ] Create `tests/unit/events/payload_matcher.test.cpp`
- [ ] Write comprehensive matcher tests
- [ ] Update `tests/unit/events/CMakeLists.txt`
- [ ] Run tests: `ctest --preset Debug -R payload_matcher`
- [ ] Verify all tests pass

### Phase 5: Integration
- [ ] Run all event tests: `ctest --preset Debug -R event`
- [ ] Identify and fix broken tests
- [ ] Refactor tests to use factory functions
- [ ] Create integration test directory
- [ ] Create `tests/integration/event_system/event_propagation.integration.test.cpp`
- [ ] Write end-to-end integration tests
- [ ] Update integration `CMakeLists.txt`
- [ ] Run integration tests
- [ ] Run full test suite: `ctest --preset Debug`
- [ ] Verify no regressions

### Phase 6: Documentation
- [ ] Update `README.md` with event factory examples
- [ ] Create `documentation/examples/EVENT_SYSTEM_USAGE.md`
- [ ] Update `.github/agents/copilot_instructions.md`
- [ ] Add inline documentation to all new functions
- [ ] Review all docstrings for clarity

### Phase 7: Final Validation
- [ ] Build in Release mode: `cmake --build --preset Release`
- [ ] Run full test suite in Release mode
- [ ] Code review of all changes
- [ ] Performance test (if applicable)
- [ ] Commit all changes
- [ ] Create pull request

---

## Expected Outcomes

After completing this implementation plan:

1. **Type-Safe Event Creation**: Factory functions ensure correct EventType/payload pairing
2. **Testing Support**: Random event generation enables comprehensive testing
3. **Subscriber Filtering**: Fine-grained control over which events activate subscribers
4. **Broken Tests Fixed**: All existing tests updated and passing
5. **Documentation**: Clear examples and usage patterns

## Risk Mitigation

**Potential Issues**:

1. **Performance**: Variant dispatching and matching add overhead
   - **Mitigation**: Use inline functions, profile if needed
   
2. **Filter Semantics**: Users may expect different matching behavior
   - **Mitigation**: Clear documentation, examples in tests
   
3. **Breaking Changes**: Existing code may expect old behavior
   - **Mitigation**: Comprehensive testing, gradual migration
   
4. **Optional Field Complexity**: Many combinations possible
   - **Mitigation**: Exhaustive test coverage, clear examples

---

## Conclusion

This implementation plan provides a comprehensive roadmap for enhancing the SteamRot event system with factory functions, random event generation, and payload matching. The plan is designed to be followed step-by-step, with clear testing and validation at each phase.

The key insight is that the existing infrastructure (matchers, configuration functions) is already well-implemented. The main gaps are:
1. Factory functions for easier event creation
2. Random event generation for testing
3. Payload matching functions for subscriber filtering

By addressing these gaps, the event system will become more robust, testable, and user-friendly.
