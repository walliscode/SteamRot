# EventPacket Creation Analysis

## Overview

This document analyzes the EventPacket creation patterns across the SteamRot codebase, identifying duplicate code between the src code and test harness, redundant functions, and recommendations for potential refactoring.

## Summary of Findings

### Key Findings

| Finding | Severity | Location | Recommendation |
|---------|----------|----------|----------------|
| **Duplicate EventPacket-from-EventPacketData conversion** | Medium | `event_bus_conversion.cpp` vs `event_simulation.cpp` | Create shared `CreateEventPacketFromData()` function in `event_factory` |
| **Duplicate `GetNameForEventDataIndex()`** | Low | `EventPacketEqualsMatcher.cpp` vs `EventDataEqualsMatcher.cpp` | Extract to shared utility or keep as-is (test code) |
| **Duplicate EventData comparison logic** | Low | `EventPacketEqualsMatcher.cpp` vs `EventDataEqualsMatcher.cpp` | `EventPacketEqualsMatcher` could use `EventDataEqualsMatcher` |
| **Minor inconsistency: `UIElementName` vs `UserInterfaceName`** | Very Low | Matcher index name mapping | Standardize naming |

---

## Detailed Analysis

### 1. Duplicate EventPacket Creation from EventPacketData

**Files Affected:**
- `src/events/event_bus_conversion.cpp` (lines 32-52)
- `tests/harness/event_simulation.cpp` (lines 36-57)

**Duplicate Code Pattern:**

Both files contain nearly identical logic for converting `EventPacketData` (FlatBuffers) to `EventPacket`:

```cpp
// Pattern repeated in both files:
EventPacket event_packet(packet_data->event_lifetime());
event_packet.m_event_type = packet_data->event_type();

auto event_data_creation_result = event::CreateEventData(
    packet_data->event_data_data_type(), packet_data->event_data_data());

if (!event_data_creation_result.has_value()) {
  return std::unexpected(event_data_creation_result.error());
} else {
  event_packet.m_event_data = event_data_creation_result.value();
}
```

**Impact:**
- Code duplication increases maintenance burden
- Changes to EventPacketData structure require updates in multiple places
- Inconsistency risk if one location is updated but not the other

**Recommendation:**

Create a new function in `src/events/event_factory.h/.cpp`:

```cpp
////////////////////////////////////////////////////////////
/// @brief Convert EventPacketData to EventPacket
///
/// @param packet_data FlatBuffers EventPacketData to convert
/// @return EventPacket on success, FailInfo on error
////////////////////////////////////////////////////////////
std::expected<EventPacket, FailInfo>
CreateEventPacketFromData(const EventPacketData *packet_data);
```

Then both `event_bus_conversion.cpp` and `event_simulation.cpp` can call this function.

---

### 2. Duplicate GetNameForEventDataIndex() Function

**Files Affected:**
- `tests/matchers/EventPacketEqualsMatcher.cpp` (lines 17-29)
- `tests/matchers/EventDataEqualsMatcher.cpp` (lines 19-32)

**Duplicate Code:**

Both files implement identical `GetNameForEventDataIndex()`:

```cpp
// In EventPacketEqualsMatcher.cpp (line 26):
case 3: return "UIElementName";  // INCORRECT

// In EventDataEqualsMatcher.cpp (line 28):
case 3: return "UserInterfaceName";  // CORRECT
```

**Impact:**
- Minor code duplication
- **Naming inconsistency:** `EventPacketEqualsMatcher.cpp:26` returns "UIElementName" but should return "UserInterfaceName" to match `EventDataEqualsMatcher.cpp:28` and the actual type name in the codebase

**Recommendation:**

Since this is test utility code and the duplication is minimal, either:
1. Accept the duplication (matchers are self-contained)
2. Extract to a shared utility in `tests/matchers/` (e.g., `event_matcher_utils.h`)

Given that the matchers are designed to be self-contained Catch2 matchers, option 1 is acceptable. However, the naming inconsistency should be fixed.

---

### 3. Duplicate EventData Comparison Logic

**Files Affected:**
- `tests/matchers/EventPacketEqualsMatcher.cpp` (`CompareEventData()` method, lines 33-154)
- `tests/matchers/EventDataEqualsMatcher.cpp` (`match()` method, lines 35-142)

**Observation:**

`EventPacketEqualsMatcher::CompareEventData()` and `EventDataEqualsMatcher::match()` implement nearly identical comparison logic for `EventData` variants. The main difference is in formatting (indentation, output style).

**Impact:**
- Significant code duplication (~120 lines)
- If EventData variant types change, both must be updated

**Recommendation:**

`EventPacketEqualsMatcher` could internally use `EventDataEqualsMatcher` for the EventData comparison portion. However, since they have slightly different formatting requirements and are designed as independent matchers, the current approach is acceptable for test infrastructure.

---

### 4. EventPacket Creation Patterns Summary

The following table shows all locations where EventPackets are created:

| Location | Method | Purpose |
|----------|--------|---------|
| `src/events/EventPacket.h:33` | `EventPacket(uint8_t)` | Create with lifetime only |
| `src/events/EventPacket.h:35` | `EventPacket(EventType, EventData, uint8_t)` | Full constructor |
| `src/events/event_bus_conversion.cpp:39` | Manual construction from `EventPacketData` | Production: Bus conversion |
| `tests/harness/event_simulation.cpp:39` | Manual construction from `EventPacketData` | Testing: Event simulation |
| `tests/harness/input_simulation.cpp:184` | `EventPacket(EventType, EventData, uint8_t)` | Testing: Input simulation |

---

## Functions That Could Be Consolidated

### Proposed New Function

Add to `src/events/event_factory.h/.cpp`:

```cpp
namespace steamrot::event {

////////////////////////////////////////////////////////////
/// @brief Convert EventPacketData to EventPacket
///
/// Creates an EventPacket from FlatBuffers EventPacketData by:
/// 1. Extracting event_lifetime and event_type
/// 2. Converting event_data_data to EventData via CreateEventData
///
/// @param packet_data FlatBuffers EventPacketData to convert
/// @return EventPacket on success, FailInfo on error
////////////////////////////////////////////////////////////
std::expected<EventPacket, FailInfo>
CreateEventPacketFromData(const EventPacketData *packet_data);

} // namespace steamrot::event
```

**Implementation:**

```cpp
std::expected<EventPacket, FailInfo>
CreateEventPacketFromData(const EventPacketData *packet_data) {

  if (!packet_data) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "EventPacketData is null"));
  }

  EventPacket event_packet(packet_data->event_lifetime());
  event_packet.m_event_type = packet_data->event_type();

  auto event_data_result = CreateEventData(
      packet_data->event_data_data_type(), packet_data->event_data_data());

  if (!event_data_result.has_value()) {
    return std::unexpected(event_data_result.error());
  }

  event_packet.m_event_data = event_data_result.value();

  return event_packet;
}
```

---

## Existing Well-Designed Patterns

The codebase has several well-designed patterns that should be preserved:

1. **event_factory.h/cpp** - Clean factory functions for EventData conversion
   - `CreateUserInputBitset()`
   - `CreateSceneChangePacket()`
   - `CreateUserInterfaceName()`
   - `CreateEventData()`

2. **event_bus_conversion.h/cpp** - Clean EventBus conversion utilities
   - `ConvertEventBusDataToEventBus()`
   - `ConfigureEventHandlerFromEventBusData()`

3. **input_simulation.cpp** - Creates EventPackets directly without going through FlatBuffers conversion (appropriate for simulating input, not loading from data)

---

## Recommendations Summary

### High Priority (Medium Effort)

1. **Create `CreateEventPacketFromData()` function** in `event_factory.h/.cpp`
   - Consolidates duplicate code in `event_bus_conversion.cpp` and `event_simulation.cpp`
   - Follows existing factory pattern

### Low Priority (Low Effort)

2. **Fix naming inconsistency** in `EventPacketEqualsMatcher.cpp`
   - Change `"UIElementName"` to `"UserInterfaceName"` on line 23

### Optional / Deferred

3. **Consider having `EventPacketEqualsMatcher` delegate to `EventDataEqualsMatcher`**
   - Would reduce code duplication but adds coupling between matchers
   - May affect formatting consistency

---

## Conclusion

The codebase has **minor duplication** in EventPacket creation from FlatBuffers data. The most impactful improvement would be adding a `CreateEventPacketFromData()` factory function to consolidate the duplicate code in `event_bus_conversion.cpp` and `event_simulation.cpp`. 

The duplicate code in test matchers is acceptable given that matchers are typically designed to be self-contained, but the naming inconsistency should be fixed.

Overall, the event system architecture is well-designed with clear separation of concerns between:
- **FlatBuffers schema** (`event_packet_data.fbs`, `event_bus_data.fbs`, `event_test_data.fbs`)
- **Runtime types** (`EventPacket.h`)
- **Factory functions** (`event_factory.h/.cpp`)
- **Conversion utilities** (`event_bus_conversion.h/.cpp`)
- **Test harness** (`event_simulation.h/.cpp`, `input_simulation.h/.cpp`)
