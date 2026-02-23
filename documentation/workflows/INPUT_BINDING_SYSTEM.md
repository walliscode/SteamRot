# Input Binding System

This guide explains the SFML event → `InputPayload` conversion system and how
to configure, extend, and use it in the SteamRot engine.

## Table of Contents

- [Overview](#overview)
- [Architecture](#architecture)
  - [InputPayload and InputAction](#inputpayload-and-inputaction)
  - [SFMLInputEntry](#sfmlinputentry)
  - [SFMLInputBinding](#sfmlinputbinding)
  - [SFMLInputRegistry](#sfmlinputregistry)
  - [EventHandler Integration](#eventhandler-integration)
- [Configuring Bindings at Startup](#configuring-bindings-at-startup)
  - [Using the Default Bindings](#using-the-default-bindings)
  - [Defining Custom Bindings](#defining-custom-bindings)
  - [Pressed vs Released Entries](#pressed-vs-released-entries)
  - [AND Logic (Multi-Input Combinations)](#and-logic-multi-input-combinations)
  - [Mixed Pressed and Released in One Binding](#mixed-pressed-and-released-in-one-binding)
- [How Events Flow](#how-events-flow)
- [Subscribing to Input Events](#subscribing-to-input-events)
- [Adding New InputActions](#adding-new-inputactions)
- [Future: FlatBuffers Serialisation](#future-flatbuffers-serialisation)
- [Testing](#testing)
- [Troubleshooting](#troubleshooting)

---

## Overview

The input binding system converts raw SFML window events (key presses, mouse
clicks, releases, etc.) into typed `InputPayload` `EventPacket`s that the rest
of the engine can subscribe to. It lives on the `EventHandler` and is designed
so that bindings can be reconfigured at runtime without touching engine code.

**Key features:**

- **Per-entry trigger condition** — each physical input in a binding specifies
  whether it must be _pressed_ (held) or _released_ for that entry to be
  satisfied
- **AND logic** — a binding fires only when _all_ of its entries are
  simultaneously satisfied
- **Action-only output** — the resulting `InputPayload` carries just the action;
  the press/release distinction lives in the binding configuration, not the
  event payload
- **Runtime configuration** — bindings are replaced via
  `EventHandler::ConfigureInputRegistry()` at any time
- **FlatBuffers-ready** — the static config header (`SFMLInputBinding.h`) is
  designed so that adding FlatBuffers-driven data only requires converting
  deserialized data to `std::vector<SFMLInputBinding>` and calling
  `ConfigureInputRegistry()`

---

## Architecture

### InputPayload and InputAction

`InputPayload` (declared in `src/types/events/EventPayload.h`) carries the
result of a triggered binding. It contains only the action — the SFML-level
press/release distinction is an internal binding concern:

```cpp
struct InputPayload {
    enum class InputAction { NONE, SELECT } action;
};
```

---

### SFMLInputEntry

Declared in `src/events/SFMLInputBinding.h`.

Represents a single physical input requirement — either a keyboard key or a
mouse button — together with whether it must be in a pressed or released state:

```cpp
struct SFMLInputEntry {
    enum class Type      { Keyboard, MouseButton } type{Type::Keyboard};
    enum class TriggerOn { Pressed, Released }     trigger_on{TriggerOn::Pressed};
    sf::Keyboard::Key  keyboard_key{sf::Keyboard::Key::Unknown};
    sf::Mouse::Button  mouse_button{sf::Mouse::Button::Left};
};
```

- `TriggerOn::Pressed` — the entry is satisfied while the key/button is held.
- `TriggerOn::Released` — the entry is satisfied only during the single SFML
  event in which that key/button was released.

Only the field matching `type` is meaningful.

---

### SFMLInputBinding

Declared in `src/events/SFMLInputBinding.h`.

Maps a combination of `SFMLInputEntry`s to an `InputAction`:

```cpp
struct SFMLInputBinding {
    InputPayload::InputAction   action{InputPayload::InputAction::NONE};
    std::vector<SFMLInputEntry> required_inputs;
};
```

All entries in `required_inputs` must be simultaneously satisfied (AND logic)
for the binding to fire and emit an `InputPayload` `EventPacket`.

---

### SFMLInputRegistry

Declared in `src/events/SFMLInputRegistry.h`, implemented in
`src/events/SFMLInputRegistry.cpp`.

Holds the active bindings and tracks which physical inputs are currently held
and which were released by the most recent event. On each call to
`ProcessSFMLEvent()` it:

1. Clears the per-event "just released" state from the previous call.
2. Updates held and just-released state from the incoming SFML event.
3. Evaluates every binding: Pressed entries check the held set; Released entries
   check the just-released set.
4. For each binding that transitions from inactive to active, emits an
   `EventPacket` carrying the binding's action.

Only keyboard and mouse button SFML events trigger binding evaluation; all
other SFML event types are ignored.

---

### EventHandler Integration

`EventHandler` (in `src/events/EventHandler.h`) owns the registry as a private
member:

| Method | Purpose |
|--------|---------|
| `ConfigureInputRegistry(bindings)` | Replaces all bindings; resets held state |
| `ProcessInputEvent(sf::Event)` | Feeds one SFML event through the registry and adds results to the waiting-room bus |
| `GetInputRegistry()` | Read-only access for inspection and testing |

`HandleSFMLEvents()` (the free function used by `PreloadEvents`) polls the
window and routes each event through `ProcessInputEvent()` automatically.

---

## Configuring Bindings at Startup

Call `ConfigureInputRegistry()` once during engine or scene initialisation,
before the main loop starts processing frames.

### Using the Default Bindings

`GetDefaultSFMLInputBindings()` (in `src/events/SFMLInputBinding.h`) provides
a code-defined set of sensible defaults:

```cpp
event_handler.ConfigureInputRegistry(steamrot::GetDefaultSFMLInputBindings());
```

The defaults map left mouse button press and left mouse button release
separately to `InputAction::SELECT`.

---

### Defining Custom Bindings

Build a `std::vector<SFMLInputBinding>` and pass it to
`ConfigureInputRegistry()`:

```cpp
using Entry     = steamrot::SFMLInputEntry;
using EntryType = steamrot::SFMLInputEntry::Type;
using TriggerOn = steamrot::SFMLInputEntry::TriggerOn;
using Action    = steamrot::InputPayload::InputAction;

std::vector<steamrot::SFMLInputBinding> bindings = {

    // Left mouse button press → SELECT
    steamrot::SFMLInputBinding{
        Action::SELECT,
        {Entry{EntryType::MouseButton, TriggerOn::Pressed,
               sf::Keyboard::Key::Unknown, sf::Mouse::Button::Left}}},

    // Left mouse button release → SELECT
    steamrot::SFMLInputBinding{
        Action::SELECT,
        {Entry{EntryType::MouseButton, TriggerOn::Released,
               sf::Keyboard::Key::Unknown, sf::Mouse::Button::Left}}},
};

event_handler.ConfigureInputRegistry(bindings);
```

---

### Pressed vs Released Entries

Each `SFMLInputEntry` independently controls its trigger condition:

| `TriggerOn` | Satisfied when |
|-------------|---------------|
| `Pressed`   | The key/button is currently held |
| `Released`  | The key/button was released by the current SFML event |

Because `Released` entries are only satisfied for the duration of the single
SFML event that released them, a binding containing a `Released` entry fires
at most once per release event and does not spuriously re-fire on subsequent
unrelated events.

---

### AND Logic (Multi-Input Combinations)

Add multiple entries to `required_inputs`. The action fires only when **all**
of them are satisfied simultaneously:

```cpp
// Both Ctrl and Space must be pressed (held) at the same time
steamrot::SFMLInputBinding chord{
    Action::SELECT,
    {
        Entry{EntryType::Keyboard, TriggerOn::Pressed,
              sf::Keyboard::Key::LControl},
        Entry{EntryType::Keyboard, TriggerOn::Pressed,
              sf::Keyboard::Key::Space},
    }
};
```

---

### Mixed Pressed and Released in One Binding

Entries with different `TriggerOn` values can be combined in a single binding:

```cpp
// SELECT fires when Space is released while Ctrl is still held
steamrot::SFMLInputBinding binding{
    Action::SELECT,
    {
        Entry{EntryType::Keyboard, TriggerOn::Pressed,
              sf::Keyboard::Key::LControl},  // Ctrl must be held
        Entry{EntryType::Keyboard, TriggerOn::Released,
              sf::Keyboard::Key::Space},      // Space must be the one just released
    }
};
```

This allows expressive mappings such as "chord release" patterns without
requiring separate bindings for the press and release phases.

---

## How Events Flow

```
SFML window
    │  pollEvent()
    ▼
EventHandler::PreloadEvents()
    │  HandleSFMLEvents()
    ▼
EventHandler::ProcessInputEvent(sf::Event)
    │  SFMLInputRegistry::ProcessSFMLEvent()
    │    1. Clear per-event just-released state
    │    2. Update held / just-released sets
    │    3. Evaluate all bindings (Pressed → held set, Released → just-released set)
    │    4. Return triggered EventPackets
    ▼
EventHandler::AddEvent()           ← waiting-room bus
    ▼
EventHandler::ProcessWaitingRoomEventBus()  ← promoted to global bus
    ▼
EventHandler::UpdateSubscribersFromGlobalEventBus()
    ▼
Subscriber::m_active = true  (for matching subscribers)
```

---

## Subscribing to Input Events

Create a `Subscriber` with `EventType::USER_INPUT` and optionally set a
`filter_payload` to react only to a specific action:

```cpp
// Fires for any USER_INPUT event
auto sub_any = std::make_shared<steamrot::Subscriber>();
sub_any->event_type = steamrot::EventType::USER_INPUT;
event_handler.RegisterSubscriber(sub_any);

// Fires only for SELECT
auto sub_select = std::make_shared<steamrot::Subscriber>();
sub_select->event_type = steamrot::EventType::USER_INPUT;
sub_select->filter_payload =
    steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT};
event_handler.RegisterSubscriber(sub_select);
```

Each frame, check `subscriber->m_active` and read `subscriber->captured_payload`
to retrieve the `InputPayload` carrying the action.

---

## Adding New InputActions

1. **Add the enum value** in `src/types/events/EventPayload.h`:

   ```cpp
   enum class InputAction { NONE, SELECT, JUMP } action;
   //                                    ^^^^^ new
   ```

2. **Add bindings** that map physical inputs to the new action:

   ```cpp
   // Space pressed → JUMP
   steamrot::SFMLInputBinding jump{
       Action::JUMP,
       {Entry{EntryType::Keyboard, TriggerOn::Pressed,
              sf::Keyboard::Key::Space}}};
   ```

3. **Update `CreateRandomEventPacket`** in `event_factory.cpp` if the random
   test packet generator needs to include the new action (it already filters
   out `NONE`).

4. **Register subscribers** in the Logic or system that reacts to the new
   action.

No changes to `SFMLInputRegistry`, `SFMLInputBinding`, or the matching
infrastructure are required.

---

## Future: FlatBuffers Serialisation

The static config header (`SFMLInputBinding.h`) intentionally mirrors the shape
of a FlatBuffers schema so that adding serialisation later is minimal. When
user preferences need to be saved and loaded:

1. Define a FlatBuffers schema (`sfml_input_binding.fbs`) with tables matching
   `SFMLInputBinding` and `SFMLInputEntry` (including the `TriggerOn` enum).
2. Deserialise the binary data into `std::vector<SFMLInputBinding>`.
3. Pass the result to `EventHandler::ConfigureInputRegistry()`.

No other engine code needs to change.

---

## Testing

Unit tests for the registry are in
`tests/unit/events/SFMLInputRegistry.test.cpp`. They cover:

- `Configure()` storing bindings and clearing held state
- Keyboard key with `TriggerOn::Pressed`: fires on press, does not re-fire
  while held, does not fire on release
- Keyboard key with `TriggerOn::Released`: fires on release only, and only for
  that single event
- Mouse button with `TriggerOn::Pressed` and `TriggerOn::Released`
- AND logic with multiple `Pressed` entries (all must be held)
- AND logic with mixed `Pressed` and `Released` entries (held condition + release
  condition must both be met simultaneously)
- Held-state tracking for keys and buttons
- Non-input SFML events being silently ignored
- `EventHandler` integration: `ConfigureInputRegistry` and `ProcessInputEvent`
- `GetDefaultSFMLInputBindings` returning non-empty defaults

To run only the input registry tests:

```bash
ctest --preset Debug -R SFMLInputRegistry
```

---

## Troubleshooting

### Action fires on unrelated events

Verify that all entries in the binding have the correct `TriggerOn` value. A
`Released` entry is satisfied only during the release event for that exact
key/button. A `Pressed` entry is satisfied while the key is held. A binding
with only `Pressed` entries won't fire on release events.

### AND binding never fires

Verify that every `SFMLInputEntry` uses the correct `type` field. If `type` is
`Keyboard`, the `keyboard_key` field is used; if `type` is `MouseButton`, the
`mouse_button` field is used. A wrong `type` means the registry will always
check the wrong set.

### Bindings reset unexpectedly

`ConfigureInputRegistry()` clears all held state. If called while keys are
physically held, those keys will not be tracked until they are released and
pressed again. Call `Configure` only when the input focus is clean (e.g.,
before the first frame of a scene, not mid-frame).

### No events reaching subscribers

Confirm the full pipeline runs each frame:

1. `ExecuteEventHandlerLevelLogic(window)` is called (or `PreloadEvents`,
   `ProcessWaitingRoomEventBus`, and `UpdateSubscribersFromGlobalEventBus` are
   called individually).
2. `ConfigureInputRegistry` was called with at least one binding before the
   first frame.
3. The subscriber's `event_type` is `EventType::USER_INPUT`.

---

## Relevant Source Files

| File | Purpose |
|------|---------|
| `src/types/events/EventPayload.h` | `InputPayload` and `InputAction` |
| `src/events/SFMLInputBinding.h` | `SFMLInputEntry` (with `TriggerOn`), `SFMLInputBinding`, `GetDefaultSFMLInputBindings()` |
| `src/events/SFMLInputRegistry.h/.cpp` | Registry that converts SFML events to EventPackets |
| `src/events/EventHandler.h/.cpp` | `ConfigureInputRegistry`, `ProcessInputEvent`, `GetInputRegistry` |
| `src/events/event_factory.h/.cpp` | `CreateInputEventPacket` |
| `src/events/payload_matchers.cpp` | `MatchPayload(InputPayload, InputPayload)` — compares action |
| `tests/unit/events/SFMLInputRegistry.test.cpp` | Unit tests for the registry |
