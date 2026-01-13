# Event System Refactoring Plan

## Overview

This document provides a detailed, step-by-step plan to refactor the event system to align with the component architecture patterns established in the SteamRot codebase. This plan implements the recommendations from `EVENT_SYSTEM_ANALYSIS.md`.

## Goals

1. **Decouple runtime types from FlatBuffers** - Remove FlatBuffers includes from event type headers
2. **Standardize FlatBuffers naming** - Apply consistent naming conventions
3. **Reduce cross-schema coupling** - Minimize dependencies between subsystems
4. **Consolidate conversion logic** - Single configurator for event conversions

## Prerequisites

- Read `EVENT_SYSTEM_ANALYSIS.md` for context
- Understand component/FlatBuffers patterns in existing codebase
- Familiarize with existing event system usage

## Phase 1: Create Independent Runtime Types

**Goal:** Define event types without FlatBuffers dependencies

**Estimated Effort:** 2-3 hours

### Step 1.1: Create Core Event Enum

**File:** `src/types/core/EventType.h` (NEW)

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of EventType enum
/////////////////////////////////////////////////

#pragma once

#include <cstdint>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Types of events in the game engine
///
/// Uses bit flags to allow combining event types
/////////////////////////////////////////////////
enum class EventType : uint64_t {
  None = 0,
  Test = 1,
  UserInput = 2,
  ToggleUI = 4,
  ChangeScene = 8,
  QuitGame = 16,
  ToggleDropdown = 32
};

/////////////////////////////////////////////////
/// @brief Bitwise OR operator for EventType
/////////////////////////////////////////////////
inline EventType operator|(EventType lhs, EventType rhs) {
  return static_cast<EventType>(
      static_cast<uint64_t>(lhs) | static_cast<uint64_t>(rhs));
}

/////////////////////////////////////////////////
/// @brief Bitwise AND operator for EventType
/////////////////////////////////////////////////
inline EventType operator&(EventType lhs, EventType rhs) {
  return static_cast<EventType>(
      static_cast<uint64_t>(lhs) & static_cast<uint64_t>(rhs));
}

/////////////////////////////////////////////////
/// @brief Check if event type has specific flag
/////////////////////////////////////////////////
inline bool HasEventType(EventType value, EventType flag) {
  return (value & flag) == flag;
}

} // namespace steamrot
```

**Rationale:** 
- Independent of FlatBuffers
- Bitwise operations enable combining multiple event types
- Can evolve without changing serialization format

### Step 1.2: Create Event Type Conversion Utilities

**File:** `src/events/event_type_conversion.h` (NEW)

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Conversion utilities for EventType
/////////////////////////////////////////////////

#pragma once

#include "EventType.h"
#include "FailInfo.h"
#include "events_generated.h"
#include <expected>

namespace steamrot::event {

/////////////////////////////////////////////////
/// @brief Convert FlatBuffers EventType to runtime EventType
///
/// @param fbs_type FlatBuffers EventType enum value
/// @return Runtime EventType or FailInfo on error
/////////////////////////////////////////////////
std::expected<EventType, FailInfo>
ConvertFromFlatBuffers(EventTypeFbs fbs_type);

/////////////////////////////////////////////////
/// @brief Convert runtime EventType to FlatBuffers EventType
///
/// @param type Runtime EventType enum value
/// @return FlatBuffers EventTypeFbs or FailInfo on error
/////////////////////////////////////////////////
std::expected<EventTypeFbs, FailInfo>
ConvertToFlatBuffers(EventType type);

} // namespace steamrot::event
```

**File:** `src/events/event_type_conversion.cpp` (NEW)

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Implementation of EventType conversion utilities
/////////////////////////////////////////////////

#include "event_type_conversion.h"

namespace steamrot::event {

/////////////////////////////////////////////////
std::expected<EventType, FailInfo>
ConvertFromFlatBuffers(EventTypeFbs fbs_type) {
  switch (fbs_type) {
  case EventTypeFbs::EVENT_NONE:
    return EventType::None;
  case EventTypeFbs::EVENT_TEST:
    return EventType::Test;
  case EventTypeFbs::EVENT_USER_INPUT:
    return EventType::UserInput;
  case EventTypeFbs::EVENT_TOGGLE_UI:
    return EventType::ToggleUI;
  case EventTypeFbs::EVENT_CHANGE_SCENE:
    return EventType::ChangeScene;
  case EventTypeFbs::EVENT_QUIT_GAME:
    return EventType::QuitGame;
  case EventTypeFbs::EVENT_TOGGLE_DROPDOWN:
    return EventType::ToggleDropdown;
  default:
    return std::unexpected(
        FailInfo{FailMode::EnumValueNotHandled,
                 "ConvertFromFlatBuffers: Unknown EventTypeFbs value"});
  }
}

/////////////////////////////////////////////////
std::expected<EventTypeFbs, FailInfo>
ConvertToFlatBuffers(EventType type) {
  switch (type) {
  case EventType::None:
    return EventTypeFbs::EVENT_NONE;
  case EventType::Test:
    return EventTypeFbs::EVENT_TEST;
  case EventType::UserInput:
    return EventTypeFbs::EVENT_USER_INPUT;
  case EventType::ToggleUI:
    return EventTypeFbs::EVENT_TOGGLE_UI;
  case EventType::ChangeScene:
    return EventTypeFbs::EVENT_CHANGE_SCENE;
  case EventType::QuitGame:
    return EventTypeFbs::EVENT_QUIT_GAME;
  case EventType::ToggleDropdown:
    return EventTypeFbs::EVENT_TOGGLE_DROPDOWN;
  default:
    return std::unexpected(
        FailInfo{FailMode::EnumValueNotHandled,
                 "ConvertToFlatBuffers: Unknown EventType value"});
  }
}

} // namespace steamrot::event
```

### Step 1.3: Update EventPacket to Use New Enum

**File:** `src/types/events/EventPacket.h` (MODIFY)

**Changes:**
1. Remove `#include "events_generated.h"`
2. Add `#include "EventType.h"`
3. Change `EventType event_type` from FlatBuffers enum to runtime enum

**Before:**
```cpp
#include "events_generated.h"  // FlatBuffers
namespace steamrot {
    struct EventPacket {
        EventType event_type{EventType::EventType_NONE};  // FlatBuffers
    };
}
```

**After:**
```cpp
#include "EventType.h"  // Runtime type
namespace steamrot {
    struct EventPacket {
        EventType event_type{EventType::None};  // Runtime enum
    };
}
```

### Step 1.4: Update Subscriber to Use New Enum

**File:** `src/types/events/Subscriber.h` (MODIFY)

Similar changes to EventPacket.h - replace FlatBuffers enum with runtime enum.

### Step 1.5: Update All Event Consumers

**Files to modify:**
- `src/events/EventHandler.h` - Update function signatures
- `src/events/EventHandler.cpp` - Update implementations
- All Logic classes using events
- All UI elements with subscribers

**Pattern:**
```cpp
// Before
if (event.event_type == EventType::EventType_USER_INPUT) { ... }

// After  
if (event.event_type == EventType::UserInput) { ... }
```

### Step 1.6: Update Event Factory to Use Conversion

**File:** `src/events/event_factory.cpp` (MODIFY)

Add conversion calls when creating events from FlatBuffers:

```cpp
#include "event_type_conversion.h"

std::expected<EventPacket, FailInfo>
CreateEventPacketFromData(const EventPacketData *packet_data) {
  // ...
  
  // Convert FlatBuffers enum to runtime enum
  auto event_type_result = 
      event::ConvertFromFlatBuffers(packet_data->event_type());
  if (!event_type_result.has_value()) {
    return std::unexpected(event_type_result.error());
  }
  
  event_packet.event_type = event_type_result.value();
  
  // ...
}
```

### Step 1.7: Add Tests for Conversion

**File:** `tests/unit/events/event_type_conversion.test.cpp` (NEW)

```cpp
#include "event_type_conversion.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConvertFromFlatBuffers handles all EventTypeFbs values", 
          "[unit][event_type_conversion]") {
  
  auto result = steamrot::event::ConvertFromFlatBuffers(
      steamrot::EventTypeFbs::EVENT_TEST);
  
  REQUIRE(result.has_value());
  REQUIRE(result.value() == steamrot::EventType::Test);
}

TEST_CASE("ConvertToFlatBuffers handles all EventType values",
          "[unit][event_type_conversion]") {
  
  auto result = steamrot::event::ConvertToFlatBuffers(
      steamrot::EventType::Test);
  
  REQUIRE(result.has_value());
  REQUIRE(result.value() == steamrot::EventTypeFbs::EVENT_TEST);
}

// Add more test cases for all enum values and error cases
```

**Testing:** Run unit tests to verify conversion works correctly

```bash
ctest --preset Debug -R event_type_conversion
```

## Phase 2: Standardize FlatBuffers Naming

**Goal:** Apply consistent naming conventions to all event-related FlatBuffers schemas

**Estimated Effort:** 1-2 hours

### Step 2.1: Rename EventDataData to EventDataUnion

**File:** `src/types/flatbuffers/events/events.fbs` (MODIFY)

**Before:**
```flatbuffers
union EventDataData {
  UserInputBitsetData,
  SceneChangePacketData,
  UserInterfaceNameData
}
```

**After:**
```flatbuffers
union EventDataUnion {
  UserInputBitsetData,
  SceneChangePacketData,
  UserInterfaceNameData
}
```

**Impact:** Need to update all references to `EventDataData`

### Step 2.2: Rename SubscriberFbs to SubscriberData

**File:** `src/types/flatbuffers/events/subscriber.fbs` (MODIFY)

**Before:**
```flatbuffers
table SubscriberFbs {
  event_type_data: EventType;
  trigger_data: EventDataData;
  active: bool;
}
```

**After:**
```flatbuffers
table SubscriberData {
  event_type: EventTypeFbs;
  trigger_data: EventDataUnion;
  active: bool;
}
```

**Changes:**
- Rename `SubscriberFbs` → `SubscriberData`
- Rename `event_type_data` → `event_type` (clearer)
- Update union type reference

### Step 2.3: Rename EventType to EventTypeFbs

**File:** `src/types/flatbuffers/events/events.fbs` (MODIFY)

**Before:**
```flatbuffers
enum EventType: ulong (bit_flags) {
  EVENT_NONE = 0,
  EVENT_TEST,
  // ...
}
```

**After:**
```flatbuffers
enum EventTypeFbs: ulong (bit_flags) {
  EVENT_NONE = 0,
  EVENT_TEST,
  // ...
}
```

**Rationale:** Distinguishes FlatBuffers enum from runtime enum

### Step 2.4: Update All Schema References

**Files to modify:**
- `event_packet_data.fbs` - Update EventType references
- `subscriber.fbs` - Update EventDataData references
- `user_interface.fbs` - Update includes and type references
- `ui_state.fbs` - Update includes and type references

### Step 2.5: Rebuild FlatBuffers Headers

After schema changes, regenerate headers:

```bash
cmake --build --preset Debug --target flatbuffers_generation
```

### Step 2.6: Update Code References

**Files to update:**
- `src/events/event_factory.cpp` - Update type names
- `src/events/subscriber_factory.cpp` - Update type names
- All test files using these types

**Pattern:**
```cpp
// Before
const SubscriberFbs *subscriber_fbs
EventDataData data_type

// After
const SubscriberData *subscriber_data
EventDataUnion data_union
```

## Phase 3: Reduce Cross-Schema Coupling

**Goal:** Minimize dependencies between entity and event schemas

**Estimated Effort:** 3-4 hours

### Option A: Extract Event References to Core

Create lightweight reference types instead of embedding full event data in entity schemas.

### Option B: Use String Identifiers

Replace embedded event data with string identifiers and maintain separate configuration.

**Recommendation:** Start with Option A (cleaner architecture), fall back to Option B if needed.

### Step 3.1: Create Core Event Reference Schema

**File:** `src/types/flatbuffers/core/event_reference.fbs` (NEW)

```flatbuffers
include "events.fbs";

namespace steamrot;

/////////////////////////////////////////////////
/// @brief Lightweight reference to a subscriber configuration
///
/// Used in entity schemas to avoid tight coupling to full subscriber data
/////////////////////////////////////////////////
table SubscriberRef {
  /// @brief Type of event this subscriber listens for
  event_type: EventTypeFbs;
  
  /// @brief Whether this subscriber requires specific event data to trigger
  has_trigger_data: bool;
  
  /// @brief Optional identifier for looking up full subscriber configuration
  config_id: string;
}

/////////////////////////////////////////////////
/// @brief Lightweight reference to an event packet configuration
///
/// Used in entity schemas to avoid tight coupling to full event data
/////////////////////////////////////////////////
table EventPacketRef {
  /// @brief Type of event to send
  event_type: EventTypeFbs;
  
  /// @brief Lifetime of the event in ticks
  event_lifetime: int;
  
  /// @brief Optional identifier for looking up full event configuration
  config_id: string;
}
```

### Step 3.2: Update Entity Schemas to Use References

**File:** `src/types/flatbuffers/entities/user_interface.fbs` (MODIFY)

**Before:**
```flatbuffers
include "../events/subscriber.fbs";
include "../events/event_packet_data.fbs";

table UIElementData {
    // ...
    subscriber_data: SubscriberFbs;
    response_event_data: EventPacketData;
    // ...
}
```

**After:**
```flatbuffers
include "../core/event_reference.fbs";

table UIElementData {
    // ...
    subscriber_ref: SubscriberRef;
    response_event_ref: EventPacketRef;
    // ...
}
```

**Benefits:**
- Entity schema no longer depends on full event schemas
- Can change event data structure without affecting entities
- Clearer intent (reference vs embedded data)

### Step 3.3: Create Event Configuration Resolver

**File:** `src/events/event_config_resolver.h` (NEW)

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Utilities for resolving event references to full configurations
/////////////////////////////////////////////////

#pragma once

#include "EventPacket.h"
#include "FailInfo.h"
#include "Subscriber.h"
#include "event_reference_generated.h"
#include <expected>
#include <memory>

namespace steamrot::event {

/////////////////////////////////////////////////
/// @brief Resolve a SubscriberRef to a full Subscriber object
///
/// @param subscriber_ref FlatBuffers reference to resolve
/// @return Shared pointer to Subscriber or FailInfo on error
/////////////////////////////////////////////////
std::expected<std::shared_ptr<Subscriber>, FailInfo>
ResolveSubscriber(const SubscriberRef *subscriber_ref);

/////////////////////////////////////////////////
/// @brief Resolve an EventPacketRef to a full EventPacket object
///
/// @param event_ref FlatBuffers reference to resolve
/// @return EventPacket or FailInfo on error
/////////////////////////////////////////////////
std::expected<EventPacket, FailInfo>
ResolveEventPacket(const EventPacketRef *event_ref);

} // namespace steamrot::event
```

### Step 3.4: Update UI Element Configurator

**File:** `src/user_interface/FlatbuffersUIElementConfigurator.cpp` (MODIFY)

Update to use resolver when configuring UI elements:

```cpp
#include "event_config_resolver.h"

// In ConfigureUIElement method:
if (ui_element_data->subscriber_ref()) {
  auto subscriber_result = 
      event::ResolveSubscriber(ui_element_data->subscriber_ref());
  
  if (subscriber_result.has_value()) {
    ui_element->m_subscriber = subscriber_result.value();
  }
}

if (ui_element_data->response_event_ref()) {
  auto event_result = 
      event::ResolveEventPacket(ui_element_data->response_event_ref());
      
  if (event_result.has_value()) {
    ui_element->m_response_event = event_result.value();
  }
}
```

### Step 3.5: Update Test Data Files

Update JSON test data files to use new reference format:

**Before:**
```json
{
  "subscriber_data": {
    "event_type_data": "EVENT_TOGGLE_UI",
    "trigger_data": { ... },
    "active": false
  }
}
```

**After:**
```json
{
  "subscriber_ref": {
    "event_type": "EVENT_TOGGLE_UI",
    "has_trigger_data": true,
    "config_id": "toggle_main_menu_subscriber"
  }
}
```

## Phase 4: Consolidate Conversion Logic

**Goal:** Single configurator for all event conversions

**Estimated Effort:** 2-3 hours

### Step 4.1: Create IEventConfigurator Interface

**File:** `src/types/interfaces/IEventConfigurator.h` (NEW)

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Interface for event configuration from FlatBuffers
/////////////////////////////////////////////////

#pragma once

#include "EventHandler.h"
#include "EventPacket.h"
#include "FailInfo.h"
#include "Subscriber.h"
#include <expected>
#include <memory>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Interface for configuring events from serialized data
/////////////////////////////////////////////////
class IEventConfigurator {
public:
  virtual ~IEventConfigurator() = default;

  /////////////////////////////////////////////////
  /// @brief Configure EventBus from FlatBuffers data
  /////////////////////////////////////////////////
  virtual std::expected<EventBus, FailInfo>
  ConfigureEventBus(const void *data) = 0;

  /////////////////////////////////////////////////
  /// @brief Configure EventPacket from FlatBuffers data
  /////////////////////////////////////////////////
  virtual std::expected<EventPacket, FailInfo>
  ConfigureEventPacket(const void *data) = 0;

  /////////////////////////////////////////////////
  /// @brief Configure Subscriber from FlatBuffers data
  /////////////////////////////////////////////////
  virtual std::expected<std::shared_ptr<Subscriber>, FailInfo>
  ConfigureSubscriber(const void *data) = 0;
};

} // namespace steamrot
```

### Step 4.2: Create FlatbuffersEventConfigurator

**File:** `src/events/FlatbuffersEventConfigurator.h` (NEW)

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief FlatBuffers implementation of event configuration
/////////////////////////////////////////////////

#pragma once

#include "IEventConfigurator.h"

namespace steamrot {

class FlatbuffersEventConfigurator : public IEventConfigurator {
public:
  /////////////////////////////////////////////////
  /// @brief Configure EventBus from EventBusData
  /////////////////////////////////////////////////
  std::expected<EventBus, FailInfo>
  ConfigureEventBus(const void *data) override;

  /////////////////////////////////////////////////
  /// @brief Configure EventPacket from EventPacketData
  /////////////////////////////////////////////////
  std::expected<EventPacket, FailInfo>
  ConfigureEventPacket(const void *data) override;

  /////////////////////////////////////////////////
  /// @brief Configure Subscriber from SubscriberData
  /////////////////////////////////////////////////
  std::expected<std::shared_ptr<Subscriber>, FailInfo>
  ConfigureSubscriber(const void *data) override;

private:
  /////////////////////////////////////////////////
  /// @brief Helper: Configure EventData from union
  /////////////////////////////////////////////////
  std::expected<EventData, FailInfo>
  ConfigureEventData(const void *data, EventDataUnion data_type);

  /////////////////////////////////////////////////
  /// @brief Helper: Configure UserInputBitset from data
  /////////////////////////////////////////////////
  std::expected<UserInputBitset, FailInfo>
  ConfigureUserInputBitset(const void *data);

  /////////////////////////////////////////////////
  /// @brief Helper: Configure SceneChangePacket from data
  /////////////////////////////////////////////////
  std::expected<SceneChangePacket, FailInfo>
  ConfigureSceneChangePacket(const void *data);
};

} // namespace steamrot
```

**File:** `src/events/FlatbuffersEventConfigurator.cpp` (NEW)

Consolidate logic from:
- `event_factory.cpp`
- `event_bus_conversion.cpp`
- `subscriber_factory.cpp`

### Step 4.3: Update Event Factory to Delegate

**File:** `src/events/event_factory.cpp` (MODIFY)

Replace direct implementation with delegation to configurator:

```cpp
// Keep as thin wrapper for backward compatibility
std::expected<EventPacket, FailInfo>
CreateEventPacketFromData(const EventPacketData *packet_data) {
  FlatbuffersEventConfigurator configurator;
  return configurator.ConfigureEventPacket(packet_data);
}
```

### Step 4.4: Deprecate Old Factory Functions

Add deprecation comments and plan removal in future version:

```cpp
/////////////////////////////////////////////////
/// @deprecated Use FlatbuffersEventConfigurator::ConfigureEventPacket instead
/// This function will be removed in v2.0
/////////////////////////////////////////////////
[[deprecated("Use FlatbuffersEventConfigurator instead")]]
std::expected<EventPacket, FailInfo>
CreateEventPacketFromData(const EventPacketData *packet_data);
```

## Phase 5: Testing and Validation

**Goal:** Ensure all changes work correctly

**Estimated Effort:** 2-3 hours

### Step 5.1: Run Full Test Suite

```bash
# Build all tests
cmake --build --preset Debug

# Run all tests
ctest --preset Debug --output-on-failure
```

### Step 5.2: Visual Testing

For any UI changes involving events/subscribers:

1. Run game in test mode
2. Verify UI interactions trigger correct events
3. Verify subscribers activate as expected
4. Check event bus state during gameplay

### Step 5.3: Performance Testing

Measure conversion overhead:

```cpp
// Benchmark conversion performance
TEST_CASE("EventType conversion performance", "[.perf][event_conversion]") {
  const int iterations = 1000000;
  auto start = std::chrono::high_resolution_clock::now();
  
  for (int i = 0; i < iterations; ++i) {
    auto result = event::ConvertFromFlatBuffers(EventTypeFbs::EVENT_TEST);
  }
  
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      end - start);
  
  INFO("Average conversion time: " << (duration.count() / iterations) << "μs");
  REQUIRE(duration.count() < 1000000); // Less than 1μs per conversion
}
```

### Step 5.4: Integration Testing

Create integration tests verifying end-to-end event flow:

```cpp
TEST_CASE("Event system integration test", "[integration][events]") {
  // 1. Load event configuration from FlatBuffers
  // 2. Create event handler with subscribers
  // 3. Trigger events
  // 4. Verify subscribers activate
  // 5. Verify event bus state
}
```

## Phase 6: Documentation and Cleanup

**Goal:** Update documentation and remove deprecated code

**Estimated Effort:** 1-2 hours

### Step 6.1: Update README

Update sections about event system in main README:

- Architecture diagram showing decoupled design
- Example usage with new patterns
- Migration guide for existing code

### Step 6.2: Update Code Comments

Ensure all new code has proper Doxygen comments following SFML style:

```cpp
////////////////////////////////////////////////////////////
/// @brief Brief description
///
/// Detailed description
///
/// @param param_name Description
/// @return Description
////////////////////////////////////////////////////////////
```

### Step 6.3: Create Migration Guide

**File:** `documentation/MIGRATION_GUIDE_EVENTS.md` (NEW)

Document migration path for:
- Updating old EventType enum references
- Converting factory calls to configurator usage
- Updating test data to new schema format

### Step 6.4: Remove Deprecated Code

After migration period (e.g., 1 release cycle):

- Remove old factory functions
- Remove backward compatibility wrappers
- Clean up unused includes

## Rollback Plan

If issues arise during implementation:

### Quick Rollback
1. Revert commit(s) to working state
2. Document issues encountered
3. Plan revised approach

### Partial Rollback
1. Keep conversion layer (Phase 1)
2. Revert schema changes (Phases 2-3)
3. Address specific issues
4. Resume from stable state

## Success Metrics

Track these metrics before/after refactoring:

| Metric | Before | After | Target |
|--------|--------|-------|--------|
| FlatBuffers includes in event types | 2 | 0 | 0 |
| Schema naming consistency | 60% | 100% | 100% |
| Conversion functions count | Scattered | Centralized | 1 configurator |
| Compile time (incremental) | Baseline | +/- 5% | < +10% |
| Test pass rate | 100% | 100% | 100% |
| Code coverage | Baseline | +5% | No regression |

## Timeline Estimate

| Phase | Estimated Time | Dependencies |
|-------|---------------|--------------|
| Phase 1: Runtime Types | 2-3 hours | None |
| Phase 2: Schema Naming | 1-2 hours | Phase 1 |
| Phase 3: Decouple Schemas | 3-4 hours | Phase 2 |
| Phase 4: Consolidate Logic | 2-3 hours | Phase 3 |
| Phase 5: Testing | 2-3 hours | Phase 4 |
| Phase 6: Documentation | 1-2 hours | Phase 5 |
| **Total** | **11-17 hours** | - |

## Next Steps

1. **Review this plan** with team/stakeholders
2. **Set up feature branch** for refactoring work
3. **Implement Phase 1** and validate with tests
4. **Continue incrementally** through remaining phases
5. **Create PR** after all tests pass
6. **Deploy** after code review approval

## References

- Analysis document: `documentation/EVENT_SYSTEM_ANALYSIS.md`
- Component patterns: `src/types/components/`, `src/entity/`
- FlatBuffers documentation: https://flatbuffers.dev/
- Google C++ Style Guide: https://google.github.io/styleguide/cppguide.html
