# EventPacket Manual Creation Locations

This document catalogs all locations where EventPackets are manually created in the codebase. These locations are candidates for refactoring to use the event factory functions once they are implemented.

## Overview

**Total Manual Creation Sites**: 38 locations
- **Test Files**: 35 locations
- **Source Files**: 3 locations

## Priority Classification

### High Priority (Source Code)
These should be refactored first as they are in production code:

1. **src/data_providers/configure/configure_ui_elements.cpp**
2. **tests/unit/logic/action_ui.test.cpp** (integration test with production-like patterns)

### Medium Priority (Matcher Tests)
These are test infrastructure that validate EventPacket equality:

3. **tests/unit/matchers/EventPacketEqualsMatcher.test.cpp**
4. **tests/unit/matchers/EventBusEqualsMatcher.test.cpp**
5. **tests/unit/matchers/EngineSnapshotEqualsMatcher.test.cpp**

### Lower Priority (EventHandler Unit Tests)
These are unit tests that exercise EventHandler functionality:

6. **tests/unit/events/EventHandler.test.cpp**

---

## Detailed Breakdown by File

### 1. src/data_providers/configure/configure_ui_elements.cpp

**Function**: `ConfigureBaseUIElement()`

**Location**: Line 98

**Current Pattern**:
```cpp
// Configure EventPacket from EventPacketFbs data
EventPacket event_packet;
auto configure_result = ConfigureEventPacket(event_packet, event_packet_data);
```

**Context**: Loading UI elements from FlatBuffers data. The EventPacket is created empty then populated via `ConfigureEventPacket()`.

**Refactoring Notes**: 
- This is NOT a candidate for the factory because it's loading from FlatBuffers data
- The `ConfigureEventPacket()` function already handles the proper initialization
- **NO CHANGE NEEDED** - Keep as-is

**Status**: ✅ No refactoring needed (uses configure function)

---

### 2. tests/unit/logic/action_ui.test.cpp

**Test Cases**: 2 locations

#### Location 1: Line 29-33
**Test**: `ProcessButtonElementActions responds to various cases`

**Current Pattern**:
```cpp
steamrot::EventPacket event_packet;
event_packet.type = steamrot::EventType::USER_INPUT;
event_packet.payload =
    steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT};
button.response_events.push_back(event_packet);
```

**Factory Refactor**:
```cpp
auto event_packet = steamrot::CreateInputEvent(
    steamrot::InputPayload::InputAction::SELECT,
    1  // default lifetime
);
button.response_events.push_back(event_packet);
```

**Benefits**: 
- Type-safe (EventType automatically matches payload)
- More concise (3 lines → 1 line)
- Explicit lifetime

#### Location 2: Line 88-92
**Test**: `ProcessUIActionsAndEvents processes UI elements correctly`

**Current Pattern**:
```cpp
steamrot::EventPacket event_packet;
event_packet.type = steamrot::EventType::USER_INPUT;
event_packet.payload =
    steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT};
button.response_events.push_back(event_packet);
```

**Factory Refactor**: Same as Location 1

**Status**: 🔄 Should be refactored (2 locations)

---

### 3. tests/unit/events/EventHandler.test.cpp

**Test Cases**: 19 locations

This file has the most manual EventPacket creations. They fall into several patterns:

#### Pattern A: Lifetime-Only Construction (13 locations)

These create EventPackets with only a lifetime specified, used for testing lifetime management.

**Lines**: 60, 73, 74, 75, 94, 95, 96, 122, 123, 151, 152, 218, 344, 440

**Current Pattern**:
```cpp
steamrot::EventPacket event1{3};
steamrot::EventPacket event2{1};
steamrot::EventPacket event3{2};
```

**Refactoring Consideration**:
These are testing the EventContext.lifetime field in isolation. They could use the factory, but it might be clearer to keep them simple for these specific lifetime tests.

**Recommended Action**:
- **Keep as-is** for tests specifically about lifetime (e.g., `DecrementEventLifetimes`, `RemoveDeadEvents`)
- **Refactor** for tests that add events to handler but don't care about specific lifetime values

**Lines to Refactor**: 218, 344, 440 (these set type and need full events)
**Lines to Keep**: 60, 73-75, 94-96, 122-123, 151-152 (pure lifetime tests)

#### Pattern B: Lifetime + Type (3 locations)

**Lines**: 219, 345, 441

**Example (Line 219)**:
```cpp
steamrot::EventPacket event1{2};
event1.type = steamrot::EventType::NONE;
```

**Factory Refactor**:
Since EventType::NONE has no meaningful payload, this could be:
```cpp
// Option 1: Add a CreateEmptyEvent() factory
auto event1 = steamrot::CreateEmptyEvent(2);

// Option 2: Use CreateInputEvent with NONE action
auto event1 = steamrot::CreateInputEvent(
    steamrot::InputPayload::InputAction::NONE, 2);
```

**Example (Line 345)**:
```cpp
steamrot::EventPacket event1{2};
event1.type = event_type;  // EventType::USER_INPUT
event1.payload = input_payload;
```

**Factory Refactor**:
```cpp
auto event1 = steamrot::CreateInputEvent(
    steamrot::InputPayload::InputAction::SELECT, 2);
```

**Example (Line 441)**:
```cpp
steamrot::EventPacket event1{2};
event1.type = steamrot::EventType::LOGIC;
```

**Factory Refactor**:
```cpp
auto event1 = steamrot::CreateLogicEvent(
    steamrot::LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD, 2);
```

**Status**: 🔄 Should be refactored (3 locations)

#### Pattern C: Payload Creation (Not EventPacket)

**Lines**: 244-245, 251-252, 274-276, 282-283, 301-302, 317-318

These create EventPayload objects, not EventPackets. They're testing the payload matching system.

**Example**:
```cpp
subscriber->filter_payload = steamrot::EventPayload{
    steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT}};

steamrot::EventPayload non_matching_payload{
    steamrot::InputPayload{steamrot::InputPayload::InputAction::NONE}};
```

**Factory Refactor**:
The factory could provide payload creation helpers:
```cpp
subscriber->filter_payload = steamrot::CreateInputPayload(
    steamrot::InputPayload::InputAction::SELECT);

auto non_matching_payload = steamrot::CreateInputPayload(
    steamrot::InputPayload::InputAction::NONE);
```

**Status**: 🔄 Could be refactored (6 locations) - but payload factories are separate from event factories

**Summary for EventHandler.test.cpp**:
- **Refactor**: 3 locations (lines 219, 345, 441)
- **Keep**: 13 locations (pure lifetime tests)
- **Consider**: 6 payload creations (if payload factories are added)

---

### 4. tests/unit/matchers/EventPacketEqualsMatcher.test.cpp

**Test Cases**: 6 locations

**Lines**: 18, 24, 61, 67, 86, 92

These tests validate that the EventPacketEqualsMatcher correctly compares EventPackets.

**Current Pattern**:
```cpp
steamrot::EventPacket expected{};
steamrot::EventPacket actual{};
// ... set different fields to test matcher
```

**Refactoring Consideration**:
These tests are specifically testing the matcher's ability to detect differences in manually constructed EventPackets. Using a factory would obscure what's being tested.

**Recommended Action**: **Keep as-is**

**Rationale**: These are matcher tests that need explicit control over each field to verify comparison logic.

**Status**: ✅ No refactoring needed (matcher validation)

---

### 5. tests/unit/matchers/EventBusEqualsMatcher.test.cpp

**Test Cases**: 4 locations

**Lines**: 29, 41, 48, 70

**Current Pattern**:
```cpp
steamrot::EventPacket packet1{1};
// ... test EventBus matching
```

**Refactoring Consideration**:
Similar to EventPacketEqualsMatcher tests - these validate the EventBusEqualsMatcher.

**Recommended Action**: **Keep as-is**

**Status**: ✅ No refactoring needed (matcher validation)

---

### 6. tests/unit/matchers/EngineSnapshotEqualsMatcher.test.cpp

**Test Cases**: 10 locations

**Lines**: 163, 186, 189, 213, 246, 269, 294, 297, 333, 357

**Current Pattern**:
```cpp
EventPacket event1{1};
// ... test EngineSnapshot matching
```

**Context**: These create minimal EventPackets as part of larger EngineSnapshot objects for matcher testing.

**Refactoring Consideration**:
These are testing EngineSnapshotEqualsMatcher, not the EventPacket itself. The EventPackets are just data for the larger test structure.

**Recommended Action**: **Keep as-is** (or low priority refactor)

**Rationale**: The focus is on EngineSnapshot comparison, not EventPacket creation patterns.

**Status**: ⚪ Low priority / Optional

---

## Summary Table

| File | Locations | High Priority | Medium Priority | Keep As-Is | Notes |
|------|-----------|---------------|-----------------|------------|-------|
| configure_ui_elements.cpp | 1 | 0 | 0 | 1 | Uses ConfigureEventPacket (correct) |
| action_ui.test.cpp | 2 | 2 | 0 | 0 | Integration test patterns |
| EventHandler.test.cpp | 19 | 3 | 0 | 16 | Most are lifetime tests |
| EventPacketEqualsMatcher.test.cpp | 6 | 0 | 0 | 6 | Matcher validation |
| EventBusEqualsMatcher.test.cpp | 4 | 0 | 0 | 4 | Matcher validation |
| EngineSnapshotEqualsMatcher.test.cpp | 10 | 0 | 0 | 10 | EngineSnapshot focus |
| **Total** | **42** | **5** | **0** | **37** | |

---

## Refactoring Recommendations

### Phase 1: High-Priority Refactoring (5 locations)

After implementing the event factory, refactor these locations first:

1. **tests/unit/logic/action_ui.test.cpp**
   - Lines 29-33 and 88-92
   - Replace manual construction with `CreateInputEvent()`

2. **tests/unit/events/EventHandler.test.cpp**
   - Line 219: Replace with appropriate factory (or `CreateEmptyEvent()`)
   - Line 345: Replace with `CreateInputEvent()`
   - Line 441: Replace with `CreateLogicEvent()`

### Phase 2: Optional Improvements

If payload factory functions are added (separate from EventPacket factories):

3. **tests/unit/events/EventHandler.test.cpp**
   - Lines 244-245, 251-252, 274-276, 282-283, 301-302, 317-318
   - Replace EventPayload manual construction with payload factories

### Phase 3: Consider for Consistency

For consistency across the codebase (low priority):

4. **tests/unit/matchers/EngineSnapshotEqualsMatcher.test.cpp**
   - 10 locations could use factory for consistency
   - Not required, as they're part of larger test structures

---

## Factory Requirements Identified

Based on the manual creation patterns, the following factory functions are needed:

### Essential Factories

1. ✅ `CreateInputEvent(InputAction, lifetime)` - Used in action_ui tests and EventHandler tests
2. ✅ `CreateLogicEvent(LogicToggle, lifetime)` - Used in EventHandler tests
3. ✅ `CreateUIEvent(ui_name, state_name, UIAction, lifetime)` - For UI events
4. ✅ `CreateSceneEvent(SceneAction, scene_type, scene_id, lifetime)` - For scene changes
5. ✅ `CreateSystemEvent(SystemAction, lifetime)` - For system events

### Optional Factories

6. ⚪ `CreateEmptyEvent(lifetime)` - For EventType::NONE (test-only?)
7. ⚪ `CreateInputPayload(InputAction)` - Payload helper (not EventPacket)
8. ⚪ `CreateUIPayload(ui_name, state_name, UIAction)` - Payload helper
9. ⚪ Other payload helpers - For consistency

---

## Validation Strategy

After refactoring:

1. **Run affected tests** to ensure behavior unchanged
2. **Compare generated EventPackets** with original manual constructions
3. **Use EventPacketEqualsMatcher** to verify equivalence
4. **Check test coverage** remains the same

---

## Example Refactoring

### Before (Manual Construction)
```cpp
// tests/unit/logic/action_ui.test.cpp:29-33
steamrot::EventPacket event_packet;
event_packet.type = steamrot::EventType::USER_INPUT;
event_packet.payload =
    steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT};
button.response_events.push_back(event_packet);
```

### After (Factory)
```cpp
// tests/unit/logic/action_ui.test.cpp:29-30
auto event_packet = steamrot::CreateInputEvent(
    steamrot::InputPayload::InputAction::SELECT);
button.response_events.push_back(event_packet);
```

**Benefits**:
- 5 lines → 3 lines
- Type safety guaranteed
- Explicit default lifetime
- More readable intent

---

## Files NOT Requiring Changes

The following files reference EventPacket but don't create them manually:

- **src/events/EventHandler.h** - Interface only
- **src/events/EventHandler.cpp** - Takes EventPackets as parameters
- **src/types/events/EventPacket.h** - Type definition
- **src/types/user_interface/UIElement.h** - Stores EventPackets
- **src/types/core/EngineSnapshot.h** - Stores EventBus
- **src/data_providers/configure/configure_event.h** - Configure functions
- **src/data_providers/configure/configure_event.cpp** - Configure functions
- **tests/matchers/*.h** - Matcher interfaces
- **tests/matchers/*.cpp** - Matcher implementations
- **tests/unit/types/events/EventPacket.test.cpp** - Type definition test
- **tests/unit/data_providers/configure_event.test.cpp** - Tests configure functions
- **tests/unit/data_providers/configure_engine_snapshot.test.cpp** - Uses configure functions

---

## Notes

- **FlatBuffers Loading**: Files using `ConfigureEventPacket()` should NOT be refactored to use factories
- **Matcher Tests**: Tests validating matchers should remain with manual construction for clarity
- **Lifetime Tests**: Tests specifically about lifetime management can keep simple construction
- **Integration Tests**: Tests that look like production usage patterns should use factories

---

## Conclusion

**Total Locations**: 42 manual EventPacket creations
**Recommended Refactoring**: 5 high-priority locations
**Keep As-Is**: 37 locations (mostly test infrastructure)

The event factory will provide significant value for:
1. Production code consistency (though current code uses ConfigureEventPacket correctly)
2. Integration test clarity (action_ui tests)
3. EventHandler tests that need full event objects

The factory is less critical for:
1. Matcher validation tests
2. Pure lifetime management tests
3. Test infrastructure code

**Estimated Refactoring Effort**: 1-2 hours after factory implementation
