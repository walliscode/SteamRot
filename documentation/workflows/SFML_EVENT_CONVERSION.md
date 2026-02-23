# SFML Event Conversion Workflow

This guide explains how the SFML event conversion pipeline works and how to
extend it with new input actions or new event types.

## Table of Contents

- [Overview](#overview)
- [Architecture](#architecture)
  - [Class and File Map](#class-and-file-map)
  - [Data Flow](#data-flow)
  - [Execution Order](#execution-order)
- [Adding a New InputAction](#adding-a-new-inputaction)
  - [Step 1: Add to the InputAction Enum](#step-1-add-to-the-inputaction-enum)
  - [Step 2: Update the FlatBuffers Schema](#step-2-update-the-flatbuffers-schema)
  - [Step 3: Update the Configure Function](#step-3-update-the-configure-function)
  - [Step 4: Add the JSON Mapping](#step-4-add-the-json-mapping)
  - [Step 5: Write Tests](#step-5-write-tests)
- [Adding a New SFML Event Type](#adding-a-new-sfml-event-type)
  - [Step 1: Extend CollectInputEvents](#step-1-extend-collectinputevents)
  - [Step 2: Extend UserInputBitset (if needed)](#step-2-extend-userinputbitset-if-needed)
- [Adding a New EventPayload Type Conversion](#adding-a-new-eventpayload-type-conversion)
  - [Step 1: Add a Free Function in sfml_event_convert](#step-1-add-a-free-function-in-sfml_event_convert)
  - [Step 2: Call it from SFMLEventConverter](#step-2-call-it-from-sfmleventconverter)
- [Startup Configuration](#startup-configuration)
- [Best Practices](#best-practices)
- [Troubleshooting](#troubleshooting)
- [Summary Checklist](#summary-checklist)
- [Additional Resources](#additional-resources)

---

## Overview

The SFML event conversion system translates raw SFML window events into the
engine's `EventPacket` / `EventPayload` types.  The design separates two
concerns:

- **Ordering** — the `SFMLEventConverter` class collects the conversion
  free-functions in a fixed, visible pipeline.
- **Logic** — free-functions in `steamrot::events::convert` contain the actual
  conversion algorithms and are independently unit-testable.

Input action mappings (which key/button combination maps to which
`InputAction`) are **data-driven**: they live in a FlatBuffers JSON file and
are loaded at startup, so new mappings can be added without touching C++ code.

---

## Architecture

### Class and File Map

| File | Namespace / Class | Responsibility |
|------|--------------------|----------------|
| `src/events/sfml_event_convert.h/.cpp` | `steamrot::events::convert` | Conversion *logic* as free functions |
| `src/events/SFMLEventConverter.h/.cpp` | `steamrot::SFMLEventConverter` | Pipeline coordinator; holds registries |
| `src/events/EventHandler.h/.cpp` | `steamrot::EventHandler` | Owns `SFMLEventConverter`; exposes `Configure()` and `ConvertSFMLEventsToEventPackets()` |
| `src/types/events/UserInputBitset.h` | `steamrot::UserInputBitset` | Bitset encoding all keyboard/mouse state |
| `src/types/flatbuffers/events/input_action_config.fbs` | `steamrot` | FlatBuffers schema for input mappings |
| `src/data_providers/configure/configure_input_action.h/.cpp` | `steamrot::data::configure` | Converts FlatBuffers data into an `InputActionRegistry` |
| `src/data_providers/FlatbuffersDataLoader` | `steamrot` | Loads the binary config file |
| `data/defaults/input_actions/default.input_action_config.json` | — | Default input mappings (data-driven) |

### Data Flow

```
sf::RenderWindow
      │ pollEvent()
      ▼
HandleSFMLEvents()                      ← free function in EventHandler.cpp
      │ vector<sf::Event>
      ▼
EventHandler::ConvertSFMLEventsToEventPackets()  ← visible ordering wrapper
      │
      ▼
SFMLEventConverter::ConvertSFMLEvents()
  │
  ├─ 1. events::convert::CollectInputEvents()
  │       └─ UserInputBitset(sfml_events)    ← SFML events → bitset
  │
  ├─ 2. events::convert::ResolveInputAction()
  │       └─ first pattern in registry that is a subset of accumulated bits
  │
  ├─ 3. events::CreateInputEventPacket()
  │       └─ EventPacket{UserInput, InputPayload{action}}
  │
  └─ 4. m_waiting_room_bitset.reset()
      │
      ▼
EventHandler::AddEvent()                ← EventPacket enters waiting room
      │
      ▼
EventHandler::ProcessWaitingRoomEventBus()  ← moves to global bus
      │
      ▼
Subscribers notified
```

### Execution Order

`EventHandler::ExecuteEventHandlerLevelLogic()` calls conversions **before**
processing the waiting room so every converted packet is available to
subscribers in the same tick:

```
CleanExpiredSubscribers()
ResetAllSubscribers()
PreloadEvents()                         ← polls SFML → ConvertSFMLEventsToEventPackets()
ProcessWaitingRoomEventBus()
UpdateSubscribersFromGlobalEventBus()
TickGlobalEventBus()
```

---

## Adding a New InputAction

This is the most common extension point.  The C++ enum and FlatBuffers schema
need a new value, and then the JSON data file gets the new mapping.

### Step 1: Add to the InputAction Enum

**Location**: `src/types/events/EventPayload.h`

```cpp
struct InputPayload {
  enum class InputAction {
    NONE,
    SELECT,
    CANCEL,    // ← add your new action here
  } action;
  // ...
};
```

### Step 2: Update the FlatBuffers Schema

Two schema files need updating.

**`src/types/flatbuffers/events/event_payload.fbs`** — add the enum value:

```fbs
enum InputActionFbs: byte {
  SELECT = 0,
  CANCEL = 1    // ← add here
}
```

**`src/types/flatbuffers/events/input_action_config.fbs`** uses
`InputActionFbs` already, so no change is needed there.

### Step 3: Update the Configure Function

**Location**: `src/data_providers/configure/configure_event.cpp`

Add a case to the `ConfigureInputPayload` switch:

```cpp
switch (input_payload_data->action()) {
case InputActionFbs_SELECT:
  input_payload.action = InputPayload::InputAction::SELECT;
  break;
case InputActionFbs_CANCEL:             // ← add this
  input_payload.action = InputPayload::InputAction::CANCEL;
  break;
default:
  return std::unexpected(FailInfo{...});
}
```

Also update `configure_input_action.cpp` — the
`ConfigureInputActionMapping` switch:

```cpp
switch (mapping_data->action()) {
case InputActionFbs_SELECT:
  action = InputPayload::InputAction::SELECT;
  break;
case InputActionFbs_CANCEL:             // ← add this
  action = InputPayload::InputAction::CANCEL;
  break;
default:
  return std::unexpected(FailInfo{...});
}
```

### Step 4: Add the JSON Mapping

**Location**: `data/defaults/input_actions/default.input_action_config.json`

Add an entry to the `mappings` array.  Each field specifies which keys/buttons
must all be held at the same time for the action to fire.

```json
{
  "mappings": [
    {
      "mouse_pressed": ["LEFT_CLICK"],
      "action": "SELECT"
    },
    {
      "keyboard_pressed": ["Escape"],
      "action": "CANCEL"
    }
  ]
}
```

Available bit-field keys (all optional, combine freely):

| Field | Type | Values |
|-------|------|--------|
| `keyboard_pressed` | `[KeyboardInput]` | `A`–`Z` (see `user_input.fbs`) |
| `keyboard_released` | `[KeyboardInput]` | same as above |
| `mouse_pressed` | `[MouseInput]` | `LEFT_CLICK`, `RIGHT_CLICK`, `MIDDLE_CLICK`, `SCROLL_UP`, `SCROLL_DOWN` |
| `mouse_released` | `[MouseInput]` | same as above |

**Matching rule**: every bit set in the pattern must also be set in the
accumulated bitset for the same tick (subset check).  The first matching entry
in the list wins.

### Step 5: Write Tests

Add a test case to
`tests/unit/data_providers/configure_input_action.test.cpp` verifying the new
enum round-trips through `ConfigureInputActionMapping`.

```cpp
TEST_CASE("ConfigureInputActionMapping maps Escape key to CANCEL action",
          "[unit][configure_input_action]") {
  flatbuffers::FlatBufferBuilder builder;

  std::vector<steamrot::KeyboardInput> kb{steamrot::KeyboardInput_A}; // use actual key
  auto kb_vec = builder.CreateVector(kb);

  steamrot::InputActionMappingFbsBuilder mapping_builder(builder);
  mapping_builder.add_keyboard_pressed(kb_vec);
  mapping_builder.add_action(steamrot::InputActionFbs_CANCEL); // new enum value
  auto offset = mapping_builder.Finish();
  builder.Finish(offset);

  const auto *data = flatbuffers::GetRoot<steamrot::InputActionMappingFbs>(
      builder.GetBufferPointer());

  steamrot::UserInputBitset bitset;
  steamrot::InputPayload::InputAction action{steamrot::InputPayload::InputAction::NONE};

  auto result = steamrot::data::configure::ConfigureInputActionMapping(
      bitset, action, data);

  REQUIRE(result.has_value());
  REQUIRE(action == steamrot::InputPayload::InputAction::CANCEL);
}
```

---

## Adding a New SFML Event Type

`UserInputBitset` currently handles `KeyPressed`, `KeyReleased`,
`MouseButtonPressed`, and `MouseButtonReleased`.  If you need to encode
additional SFML event variants (e.g. scroll wheel, joystick), follow the steps
below.

### Step 1: Extend CollectInputEvents

**Location**: `src/events/sfml_event_convert.cpp`

`CollectInputEvents` simply delegates to the `UserInputBitset` constructor
which already processes a `vector<sf::Event>`.  If the `UserInputBitset` is
extended (see Step 2) you do not need to change `CollectInputEvents` itself.

If you want to produce a **different payload type** from a particular SFML
event (e.g. a `SystemPayload::QUIT` from `sf::Event::Closed`), add a separate
free-function in the same file and call it from `SFMLEventConverter` (see
[Adding a New EventPayload Type Conversion](#adding-a-new-eventpayload-type-conversion)).

### Step 2: Extend UserInputBitset (if needed)

**Location**: `src/types/events/UserInputBitset.h`

The current bit layout is:

```
[0 .. KeyCount)             keyboard pressed
[KeyCount .. 2*KeyCount)    keyboard released
[2*KeyCount .. 2*KeyCount+ButtonCount)  mouse pressed
[2*KeyCount+ButtonCount .. kTotalBits)  mouse released
```

To add scroll-wheel support you would:

1. Extend `kTotalBits` to reserve extra bits.
2. Add `setScrollUp()` / `setScrollDown()` helpers.
3. Populate them in the vector-of-events constructor by checking
   `event.getIf<sf::Event::MouseWheelScrolled>()`.

Update `user_input.fbs` (`MouseInput` enum) and the
`configure_input_action.cpp` mapping switch accordingly.

---

## Adding a New EventPayload Type Conversion

The current pipeline only produces `InputPayload` events.  The framework is
designed so additional payload types can be added without restructuring the
class.

### Step 1: Add a Free Function in sfml_event_convert

**Location**: `src/events/sfml_event_convert.h/.cpp`

Add a new free-function that inspects the `vector<sf::Event>` for the SFML
event variant you care about and returns the appropriate payload.

```cpp
// sfml_event_convert.h
namespace steamrot::events::convert {

/// Returns true if a window-close event is present in the list.
bool HasWindowCloseEvent(const std::vector<sf::Event> &sfml_events);

} // namespace steamrot::events::convert
```

```cpp
// sfml_event_convert.cpp
bool HasWindowCloseEvent(const std::vector<sf::Event> &sfml_events) {
  for (const auto &event : sfml_events) {
    if (event.getIf<sf::Event::Closed>()) {
      return true;
    }
  }
  return false;
}
```

Add a corresponding unit test in
`tests/unit/events/sfml_event_convert.test.cpp`.

### Step 2: Call it from SFMLEventConverter

**Location**: `src/events/SFMLEventConverter.cpp`

In `ConvertSFMLEvents()`, add a new step after step 3 and before step 4.  The
numbered comments make the ordering visible:

```cpp
std::vector<EventPacket>
SFMLEventConverter::ConvertSFMLEvents(const std::vector<sf::Event> &sfml_events) {
  std::vector<EventPacket> result;

  // Step 1: Collect keyboard/mouse SFML events into the waiting-room bitset.
  m_waiting_room_bitset = events::convert::CollectInputEvents(sfml_events);

  // Step 2: Resolve an InputAction from the registry.
  auto input_action =
      events::convert::ResolveInputAction(m_waiting_room_bitset,
                                          m_input_action_registry);

  // Step 3: Create an EventPacket for the resolved action (if any).
  if (input_action.has_value()) {
    auto packet = events::CreateInputEventPacket(1, input_action.value());
    if (packet.has_value()) {
      result.push_back(packet.value());
    }
  }

  // Step 4: Convert window-close event to SystemPayload::QUIT.
  if (events::convert::HasWindowCloseEvent(sfml_events)) {
    auto packet = events::CreateSystemEventPacket(
        1, SystemPayload::SystemAction::QUIT);
    if (packet.has_value()) {
      result.push_back(packet.value());
    }
  }

  // Step 5: Reset the waiting-room bitset for the next tick.
  m_waiting_room_bitset.reset();

  return result;
}
```

**Important**: Keep the numbered comments updated so the execution order
remains visible to the reader.

---

## Startup Configuration

The input-action registry must be loaded and handed to `EventHandler` before
the first tick.  The typical startup sequence is:

```cpp
// 1. Load binary data
steamrot::FlatbuffersDataLoader loader;
auto config_result = loader.ProvideDefaultInputActionConfigFbs();
if (!config_result.has_value()) {
  // handle error
}

// 2. Build the registry
steamrot::SFMLEventConverter::InputActionRegistry registry;
auto configure_result =
    steamrot::data::configure::ConfigureInputActionRegistry(
        registry, config_result.value());
if (!configure_result.has_value()) {
  // handle error
}

// 3. Hand it to the EventHandler — must happen before ExecuteEventHandlerLevelLogic()
event_handler.Configure(std::move(registry));
```

The binary file is generated automatically from
`data/defaults/input_actions/default.input_action_config.json` during the
CMake build step.

---

## Best Practices

1. **Keep conversion logic in free functions** — `sfml_event_convert.cpp`
   contains testable logic; `SFMLEventConverter.cpp` contains only ordering.
2. **Keep steps numbered** — The numbered comments in `ConvertSFMLEvents()`
   make execution order obvious.  Update them when adding new steps.
3. **First match wins** — The registry is evaluated in order.  Put more
   specific patterns (more bits set) before less specific ones.
4. **One action per tick** — The current design resolves at most one
   `InputAction` per tick from the bitset.  If you need multiple simultaneous
   actions, add multiple registries or change the resolve loop to return a
   `vector`.
5. **Null-check FlatBuffers data** — `ConfigureInputActionMapping` already
   guards against null; maintain this pattern for any new configure functions.
6. **Unit-test free functions in isolation** — Each function in
   `sfml_event_convert` should have its own test cases in
   `sfml_event_convert.test.cpp`.

---

## Troubleshooting

### No InputPayload events appear on the bus

- ✅ Verify `event_handler.Configure(registry)` is called before the first tick.
- ✅ Check `default.input_action_config.json` is correct and the binary
  (`default.input_action_config.bin`) has been regenerated by the build.
- ✅ Confirm the SFML event being tested actually fires
  (`sf::Event::MouseButtonPressed` vs `MouseButtonReleased`).
- ✅ Verify the pattern in the JSON matches the event: bit-patterns must be a
  **subset** of what the SFML event sets.

### Registry is empty at runtime

- ✅ Check `ProvideDefaultInputActionConfigFbs()` returns a value (no error).
- ✅ Confirm `ConfigureInputActionRegistry()` succeeds and the resulting
  registry is not empty.
- ✅ Check `data/defaults/input_actions/` exists and contains the `.bin` file.

### New action enum value not recognised

- ✅ Add the value to both `InputActionFbs` (`.fbs`) **and** `InputAction`
  (C++ enum).
- ✅ Add a `case` to both `configure_event.cpp` and
  `configure_input_action.cpp`.
- ✅ Rebuild to regenerate FlatBuffers headers.

### Two patterns both match, wrong action fires

- ✅ Re-order mappings in the JSON — the first matching entry wins.
- ✅ Make the more-specific pattern appear earlier in the list.

---

## Summary Checklist

### Adding a new InputAction

- [ ] Add enum value to `InputPayload::InputAction` (`EventPayload.h`)
- [ ] Add enum value to `InputActionFbs` (`event_payload.fbs`)
- [ ] Add `case` to `ConfigureInputPayload` (`configure_event.cpp`)
- [ ] Add `case` to `ConfigureInputActionMapping` (`configure_input_action.cpp`)
- [ ] Add mapping entry to `default.input_action_config.json`
- [ ] Rebuild to regenerate FlatBuffers headers and binary
- [ ] Write unit test for the new enum value

### Adding a new SFML event type conversion

- [ ] Add free function to `sfml_event_convert.h/.cpp`
- [ ] Write unit tests for the new function (`sfml_event_convert.test.cpp`)
- [ ] Add numbered step in `SFMLEventConverter::ConvertSFMLEvents()`
- [ ] (If extending `UserInputBitset`) update `UserInputBitset.h`,
  `user_input.fbs`, and `configure_input_action.cpp`

---

## Additional Resources

- **Free functions**: `src/events/sfml_event_convert.h`
- **Pipeline coordinator**: `src/events/SFMLEventConverter.h`
- **EventHandler integration**: `src/events/EventHandler.h`
- **Input bitset**: `src/types/events/UserInputBitset.h`
- **FlatBuffers schema**: `src/types/flatbuffers/events/input_action_config.fbs`
- **Configure functions**: `src/data_providers/configure/configure_input_action.h`
- **Default mappings**: `data/defaults/input_actions/default.input_action_config.json`
- **Unit tests**: `tests/unit/events/sfml_event_convert.test.cpp`,
  `tests/unit/events/SFMLEventConverter.test.cpp`,
  `tests/unit/data_providers/configure_input_action.test.cpp`
