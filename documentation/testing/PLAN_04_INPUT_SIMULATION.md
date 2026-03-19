# Plan 04 — Input Simulation

## Component identity

**Files (to be created):**
- `tests/harness/input_simulation.h`
- `tests/harness/input_simulation.cpp`

**Responsibility:** Accept an `InputSequenceData` object and a tick number, then
inject the correct mouse or keyboard input events into `EventHandler` and update
`mouse_position` (the `sf::Vector2i` held by `EngineResources`). This allows
integration tests to simulate a user moving the mouse, clicking, or pressing
keys at a specific tick without needing a real SFML window to generate events.

---

## Current state

This component does not exist. There is no mechanism to inject input per tick.
As a consequence, integration tests that involve UI interaction (e.g., clicking
a button, hovering over an element) cannot be written until this component is in
place.

### Related infrastructure that already exists

- `EventHandler` has `AddEvent(EventPacket)` and
  `ProcessWaitingRoomEventBus()`.
- `EventPacket` carries an `EventType` and an `EventPayload` variant.
- `SFMLEventConverter` converts raw `sf::Event` objects into `EventPacket`
  objects, but is coupled to the SFML window; it cannot be used in headless
  tests.
- The `EventType` enum includes `USER_INPUT` and the `InputPayload` struct
  carries a `UserInputBitset` (set of active actions).
- `InputActionRegistry` maps raw keys/buttons to game `Actions`; `EventHandler`
  can be given this registry via `SetInputActionRegistry()`.
- `mouse_position` is an `sf::Vector2i` reference inside `SceneContext`; Logic
  classes read it directly for collision tests.

The architecture means that injecting mouse position is a direct write to
`mouse_position`, while injecting a click or key press means constructing an
`EventPacket` with the appropriate `InputPayload` and calling `AddEvent()`.

---

## Plan

### Step 1 — Define native input data types

**File (new):** `src/types/test_structs/InputSequenceData.h`

```cpp
namespace steamrot {

enum class InputType {
  None = 0,
  MouseMove = 1,
  MouseClick = 2,
  MouseRelease = 3,
  KeyPress = 4,
  KeyRelease = 5,
};

struct MouseInputData {
  float x{0.0f};
  float y{0.0f};
  int button{0};  // 0 = left, 1 = right
};

struct KeyInputData {
  int key_code{0};  // sf::Keyboard::Key value
};

using InputData = std::variant<MouseInputData, KeyInputData>;

struct InputEvent {
  uint32_t tick{0};
  InputType input_type{InputType::None};
  InputData input_data{MouseInputData{}};
  std::string description{};
};

struct InputSequenceData {
  std::string description{};
  std::vector<InputEvent> events{};
};

} // namespace steamrot
```

### Step 2 — Add `input_sequence` field to `TestData`

**File:** `src/types/test_structs/TestData.h`

Add:
```cpp
std::optional<InputSequenceData> input_sequence{};
```

### Step 3 — Implement `input_simulation.h` / `.cpp`

**File (new):** `tests/harness/input_simulation.h`

```cpp
namespace steamrot::tests {

// Execute all input events scheduled for a specific tick.
// Updates mouse_position and adds EventPackets to the event handler.
std::expected<std::monostate, FailInfo>
execute_input_events_for_tick(
    const InputSequenceData &sequence,
    uint32_t tick,
    EventHandler &event_handler,
    sf::Vector2i &mouse_position);

// Execute all events in the entire sequence (all ticks).
std::expected<std::monostate, FailInfo>
execute_input_sequence(
    const InputSequenceData &sequence,
    EventHandler &event_handler,
    sf::Vector2i &mouse_position);

} // namespace steamrot::tests
```

**Implementation notes for `input_simulation.cpp`:**

- Filter `sequence.events` by `event.tick == tick`.
- For `InputType::MouseMove`: write `event.input_data` position into
  `mouse_position` as `sf::Vector2i{(int)x, (int)y}`.
- For `InputType::MouseClick` / `MouseRelease`: construct an `EventPacket`
  of type `USER_INPUT` with an `InputPayload` carrying the relevant
  action bit (e.g., `Action::LEFT_CLICK`). Call `event_handler.AddEvent()`.
- For `InputType::KeyPress` / `KeyRelease`: construct an `EventPacket`
  of type `USER_INPUT` with an `InputPayload` carrying the mapped action.
  Look up the action via the `InputActionRegistry` if available.
- Return `FailInfo{FailMode::NonExistentEnumValue}` for `InputType::None`.

### Step 4 — Add FlatBuffers conversion in `FlatbuffersTestDataProvider`

**File:** `tests/harness/FlatbuffersTestDataProvider.cpp`

This step requires the `InputSequenceFbs` generated header to exist (it is
defined in `src/flatbuffers_headers/test_data.fbs` or an adjacent schema file).

- Add `ConfigureInputSequence(const InputSequenceFbs *) ->
  std::expected<InputSequenceData, FailInfo>`.
- Call it from `CreateTestData()` and store the result in
  `test_data.input_sequence`.
- Map `InputTypeFbs` enum values to `InputType` enum values.
- Convert `MouseInputDataFbs` and `KeyInputDataFbs` to the native structs.

### Step 5 — Add unit tests

**File (new):** `tests/unit/harness/input_simulation.test.cpp`

Test cases to include:

1. `execute_input_events_for_tick` with `MouseMove` updates `mouse_position`.
2. `execute_input_events_for_tick` with `MouseClick` adds an event to the
   event handler waiting room.
3. `execute_input_events_for_tick` for a tick with no matching events does
   nothing and returns success.
4. `execute_input_events_for_tick` with `InputType::None` returns
   `FailInfo{FailMode::NonExistentEnumValue}`.
5. `execute_input_sequence` processes all events across all ticks.

Add to `tests/unit/harness/CMakeLists.txt`:
```cmake
input_simulation.test.cpp
```

### Step 6 — Link to `harness` CMake target

**File:** `tests/harness/CMakeLists.txt`

Add `input_simulation.cpp` to the `harness` library source list.

---

## Acceptance criteria

- [ ] `InputSequenceData` struct exists in `src/types/test_structs/`.
- [ ] `execute_input_events_for_tick()` correctly updates `mouse_position` for
  `MouseMove` events.
- [ ] `execute_input_events_for_tick()` adds `EventPacket` objects to the event
  handler for click and key events.
- [ ] `FlatbuffersTestDataProvider` converts `InputSequenceFbs` into
  `InputSequenceData` and stores it on `TestData`.
- [ ] All five unit test cases pass.
- [ ] No existing unit tests broken.
