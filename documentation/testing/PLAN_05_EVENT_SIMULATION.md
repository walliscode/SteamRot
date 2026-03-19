# Plan 05 — Event Simulation

## Component identity

**Files (to be created):**
- `tests/harness/event_simulation.h`
- `tests/harness/event_simulation.cpp`

**Responsibility:** Accept an `EventSequenceData` object and a tick number, then
inject the specified `EventPacket` objects into the `EventHandler` waiting-room
bus. Optionally promote them to the global bus immediately. This allows
integration tests to put the event system into a known state at a precise tick
without replaying real SFML input.

---

## Current state

This component does not exist.

Without it, integration tests cannot verify behaviour that depends on specific
events being present at specific ticks (e.g., a Logic class that only activates
when a `USER_INPUT` event with `Action::SELECT` is in the global event bus).

### Related infrastructure that already exists

- `EventHandler::AddEvent(EventPacket)` adds to the waiting-room bus.
- `EventHandler::ProcessWaitingRoomEventBus()` promotes waiting-room events to
  the global bus.
- `EventPacket` is fully defined with `EventType`, lifetime, and
  `EventPayload`.
- The `EngineSnapshot::global_event_bus` optional field already holds an
  `EventBus` (which is `std::vector<EventPacket>`).
- `TestEngine::StartUp()` already pre-loads an initial event bus from
  `starting_engine_snapshot.global_event_bus`, showing the pattern to follow.
- The FlatBuffers schema already has `EventBusData` and `EventPacketData`
  tables, and a conversion path from FlatBuffers to native `EventPacket`
  objects exists (used in the snapshot loading path).

---

## Plan

### Step 1 — Define native event sequence data types

**File (new):** `src/types/test_structs/EventSequenceData.h`

```cpp
namespace steamrot {

struct TickEvent {
  uint32_t tick{0};
  EventPacket event_packet{};
  std::string description{};
};

struct EventSequenceData {
  std::string description{};
  std::vector<TickEvent> events{};
};

} // namespace steamrot
```

### Step 2 — Add `event_sequence` field to `TestData`

**File:** `src/types/test_structs/TestData.h`

Add:
```cpp
std::optional<EventSequenceData> event_sequence{};
```

### Step 3 — Implement `event_simulation.h` / `.cpp`

**File (new):** `tests/harness/event_simulation.h`

```cpp
namespace steamrot::tests {

// Inject all events scheduled for a specific tick into the waiting-room bus.
// Call event_handler.ProcessWaitingRoomEventBus() separately when you want
// the events to become visible on the global bus.
std::expected<std::monostate, FailInfo>
execute_events_for_tick(
    const EventSequenceData &sequence,
    uint32_t tick,
    EventHandler &event_handler);

// Execute all events across all ticks (for tests that do not use TestEngine).
std::expected<std::monostate, FailInfo>
execute_event_sequence(
    const EventSequenceData &sequence,
    EventHandler &event_handler);

} // namespace steamrot::tests
```

**Implementation notes for `event_simulation.cpp`:**

- `execute_events_for_tick`: iterate `sequence.events`, filter by
  `event.tick == tick`, and call `event_handler.AddEvent(event.event_packet)`
  for each match.
- `execute_event_sequence`: iterate all events, grouped or ordered by tick,
  calling `AddEvent` for every entry. Note: does not call
  `ProcessWaitingRoomEventBus` — that is the caller's responsibility so that
  event promotion timing can be controlled precisely.
- Both functions return `std::monostate{}` on success. There are no known
  failure modes beyond a null-pointer in the sequence, which would be a
  programming error caught at compile time through the type system.

### Step 4 — Add FlatBuffers conversion in `FlatbuffersTestDataProvider`

**File:** `tests/harness/FlatbuffersTestDataProvider.cpp`

The `EventSequenceFbs` table in the test-data schema holds a vector of
`TickEventFbs` objects, each carrying a tick number and an `EventPacketFbs`.

- Add `ConfigureEventSequence(const EventSequenceFbs *) ->
  std::expected<EventSequenceData, FailInfo>`.
- For each `TickEventFbs`:
  - Read `tick`.
  - Convert `EventPacketFbs` to `EventPacket` using the existing conversion
    logic already present for `global_event_bus` loading in
    `TestEngine::StartUp()`.
  - Build a `TickEvent` and push it into `EventSequenceData::events`.
- Call from `CreateTestData()` and store in `test_data.event_sequence`.

### Step 5 — Add unit tests

**File (new):** `tests/unit/harness/event_simulation.test.cpp`

Test cases to include:

1. `execute_events_for_tick` with matching tick adds events to the waiting-room
   bus (visible after `ProcessWaitingRoomEventBus()`).
2. `execute_events_for_tick` with a tick that has no events does nothing and
   returns success.
3. `execute_event_sequence` adds all events from all ticks.
4. After `execute_events_for_tick` and `ProcessWaitingRoomEventBus()`, the
   global bus contains the expected `EventPacket`.
5. `EventPacket` lifetime is preserved when injected (not decremented on
   injection, only on tick).

Add to `tests/unit/harness/CMakeLists.txt`:
```cmake
event_simulation.test.cpp
```

### Step 6 — Link to `harness` CMake target

**File:** `tests/harness/CMakeLists.txt`

Add `event_simulation.cpp` to the `harness` library source list.

---

## Acceptance criteria

- [ ] `EventSequenceData` struct exists in `src/types/test_structs/`.
- [ ] `execute_events_for_tick()` correctly adds all events for the requested
  tick to the event handler waiting room.
- [ ] `execute_events_for_tick()` is a no-op (returns success) when no events
  match the requested tick.
- [ ] `FlatbuffersTestDataProvider` converts `EventSequenceFbs` into
  `EventSequenceData` and stores it on `TestData`.
- [ ] All five unit test cases pass.
- [ ] No existing unit tests broken.
