# Input Binding System

This guide explains the SFML event → `InputPayload` conversion system and how
to configure, extend, and use it in the SteamRot engine.

## Table of Contents

- [Overview](#overview)
- [Architecture](#architecture)
  - [InputPayload and InputState](#inputpayload-and-inputstate)
  - [SFMLInputEntry](#sfmlinputentry)
  - [SFMLInputBinding](#sfmlinputbinding)
  - [SFMLInputRegistry](#sfmlinputregistry)
  - [EventHandler Integration](#eventhandler-integration)
- [Configuring Bindings at Startup](#configuring-bindings-at-startup)
  - [Using the Default Bindings](#using-the-default-bindings)
  - [Defining Custom Bindings](#defining-custom-bindings)
  - [AND Logic (Multi-Key Combinations)](#and-logic-multi-key-combinations)
  - [Pressed vs Released](#pressed-vs-released)
- [How Events Flow](#how-events-flow)
- [Subscribing to Input Events](#subscribing-to-input-events)
- [Adding New InputActions](#adding-new-inputactions)
- [Future: FlatBuffers Serialisation](#future-flatbuffers-serialisation)
- [Testing](#testing)
- [Troubleshooting](#troubleshooting)

---

## Overview

The input binding system converts raw SFML window events (key presses, mouse
clicks, etc.) into typed `InputPayload` `EventPacket`s that the rest of the
engine can subscribe to. It lives on the `EventHandler` and is designed so
that bindings can be reconfigured at runtime without touching engine code.

**Key features:**

- **AND logic** — an action fires only when _all_ of its required inputs are
  held simultaneously (e.g., Ctrl + Space)
- **Pressed and Released** — each binding independently controls whether it
  fires when the combination is first satisfied (_pressed_) or when any
  required input is released after the combination was active (_released_)
- **Runtime configuration** — bindings are replaced via
  `EventHandler::ConfigureInputRegistry()` at any time
- **FlatBuffers-ready** — the static config header (`SFMLInputBinding.h`) is
  designed so that adding FlatBuffers-driven data only requires converting
  deserialized data to `std::vector<SFMLInputBinding>` and calling
  `ConfigureInputRegistry()`

---

## Architecture

### InputPayload and InputState

`InputPayload` (declared in `src/types/events/EventPayload.h`) carries the
result of a triggered binding:

```cpp
struct InputPayload {
    enum class InputAction { NONE, SELECT } action;
    enum class InputState  { PRESSED, RELEASED } state{InputState::PRESSED};
};
```

`InputState` is always set by the registry — consuming code does not need to
set it manually. The default state (`PRESSED`) preserves backward compatibility
with code that constructs `InputPayload` directly.

---

### SFMLInputEntry

Declared in `src/events/SFMLInputBinding.h`.

Represents a single physical input requirement — either a keyboard key or a
mouse button:

```cpp
struct SFMLInputEntry {
    enum class Type { Keyboard, MouseButton } type{Type::Keyboard};
    sf::Keyboard::Key  keyboard_key{sf::Keyboard::Key::Unknown};
    sf::Mouse::Button  mouse_button{sf::Mouse::Button::Left};
};
```

Only the field matching `type` is meaningful.

---

### SFMLInputBinding

Declared in `src/events/SFMLInputBinding.h`.

Maps a combination of `SFMLInputEntry`s to an `InputAction` and a
`trigger_state`:

```cpp
struct SFMLInputBinding {
    InputPayload::InputAction action{InputPayload::InputAction::NONE};
    InputPayload::InputState  trigger_state{InputPayload::InputState::PRESSED};
    std::vector<SFMLInputEntry> required_inputs;
};
```

All entries in `required_inputs` must be simultaneously held for the binding to
be _satisfied_ (AND logic). `trigger_state` controls whether the resulting
`InputPayload` EventPacket is emitted when the binding becomes satisfied
(`PRESSED`) or when it stops being satisfied (`RELEASED`).

---

### SFMLInputRegistry

Declared in `src/events/SFMLInputRegistry.h`, implemented in
`src/events/SFMLInputRegistry.cpp`.

Holds the active bindings and tracks which physical inputs are currently held.
On each call to `ProcessSFMLEvent()` it:

1. Updates the held key/button sets from the incoming SFML event.
2. Compares the new satisfaction state of every binding against its previous
   state.
3. For each transition, emits an `EventPacket` with the appropriate
   `InputPayload`.

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

`HandleSFMLEvents()` (the free function used by `PreloadEvents`) now polls the
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

The defaults map left mouse button press and release to `InputAction::SELECT`.

---

### Defining Custom Bindings

Build a `std::vector<SFMLInputBinding>` and pass it to
`ConfigureInputRegistry()`:

```cpp
using Entry       = steamrot::SFMLInputEntry;
using EntryType   = steamrot::SFMLInputEntry::Type;
using Action      = steamrot::InputPayload::InputAction;
using State       = steamrot::InputPayload::InputState;

std::vector<steamrot::SFMLInputBinding> bindings = {

    // Left mouse press fires SELECT (PRESSED)
    steamrot::SFMLInputBinding{
        Action::SELECT,
        State::PRESSED,
        {Entry{EntryType::MouseButton, sf::Keyboard::Key::Unknown,
               sf::Mouse::Button::Left}}},

    // Left mouse release fires SELECT (RELEASED)
    steamrot::SFMLInputBinding{
        Action::SELECT,
        State::RELEASED,
        {Entry{EntryType::MouseButton, sf::Keyboard::Key::Unknown,
               sf::Mouse::Button::Left}}},
};

event_handler.ConfigureInputRegistry(bindings);
```

---

### AND Logic (Multi-Key Combinations)

Add multiple entries to `required_inputs`. The action fires only when **all**
of them are held at the same time:

```cpp
// Ctrl + Space fires SELECT (PRESSED) only when both are held simultaneously
steamrot::SFMLInputBinding chord{
    Action::SELECT,
    State::PRESSED,
    {
        Entry{EntryType::Keyboard, sf::Keyboard::Key::LControl},
        Entry{EntryType::Keyboard, sf::Keyboard::Key::Space},
    }
};
```

If `LControl` is released while `Space` is still held (or vice-versa), the
binding becomes unsatisfied and any `RELEASED`-trigger binding for the same
combination fires.

---

### Pressed vs Released

Each binding is **independent**. To react to both the press and the release of
the same combination, add two bindings with the same `required_inputs` but
different `trigger_state` values:

```cpp
// Fires when Enter is first pressed
steamrot::SFMLInputBinding on_press{
    Action::SELECT, State::PRESSED,
    {Entry{EntryType::Keyboard, sf::Keyboard::Key::Enter}}};

// Fires when Enter is released after being pressed
steamrot::SFMLInputBinding on_release{
    Action::SELECT, State::RELEASED,
    {Entry{EntryType::Keyboard, sf::Keyboard::Key::Enter}}};

event_handler.ConfigureInputRegistry({on_press, on_release});
```

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
    │    - update held keys / buttons
    │    - check binding satisfaction transitions
    │    - return triggered EventPackets
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
`filter_payload` to react only to specific actions or states:

```cpp
// Fires for any USER_INPUT event
auto sub_any = std::make_shared<steamrot::Subscriber>();
sub_any->event_type = steamrot::EventType::USER_INPUT;
event_handler.RegisterSubscriber(sub_any);

// Fires only for SELECT PRESSED
auto sub_select_press = std::make_shared<steamrot::Subscriber>();
sub_select_press->event_type = steamrot::EventType::USER_INPUT;
sub_select_press->filter_payload = steamrot::InputPayload{
    steamrot::InputPayload::InputAction::SELECT,
    steamrot::InputPayload::InputState::PRESSED};
event_handler.RegisterSubscriber(sub_select_press);

// Fires only for SELECT RELEASED
auto sub_select_release = std::make_shared<steamrot::Subscriber>();
sub_select_release->event_type = steamrot::EventType::USER_INPUT;
sub_select_release->filter_payload = steamrot::InputPayload{
    steamrot::InputPayload::InputAction::SELECT,
    steamrot::InputPayload::InputState::RELEASED};
event_handler.RegisterSubscriber(sub_select_release);
```

Each frame, check `subscriber->m_active` and read `subscriber->captured_payload`
to retrieve the `InputPayload`.

> **Note**: `MatchPayload(InputPayload, InputPayload)` compares both `action`
> and `state`, so a `PRESSED` filter will not match a `RELEASED` event and vice
> versa.

---

## Adding New InputActions

1. **Add the enum value** in `src/types/events/EventPayload.h`:

   ```cpp
   enum class InputAction { NONE, SELECT, JUMP } action;
   //                                    ^^^^^ new
   ```

2. **Add one or more bindings** that map physical inputs to the new action:

   ```cpp
   steamrot::SFMLInputBinding jump_press{
       Action::JUMP,
       State::PRESSED,
       {Entry{EntryType::Keyboard, sf::Keyboard::Key::Space}}};
   ```

3. **Update any `CreateRandomEventPacket` logic** in `event_factory.cpp` if
   the random test packet generator needs to include the new action (it filters
   out `NONE` already).

4. **Register subscribers** in whatever Logic or system needs to react to the
   new action.

No changes to `SFMLInputRegistry`, `SFMLInputBinding`, or the matching
infrastructure are required.

---

## Future: FlatBuffers Serialisation

The static config header (`SFMLInputBinding.h`) intentionally mirrors the shape
of a FlatBuffers schema so that adding serialisation later is minimal. When
user preferences need to be saved and loaded:

1. Define a FlatBuffers schema (`sfml_input_binding.fbs`) with tables matching
   `SFMLInputBinding` and `SFMLInputEntry`.
2. Deserialise the binary data into `std::vector<SFMLInputBinding>`.
3. Pass the result to `EventHandler::ConfigureInputRegistry()`.

No other engine code needs to change. The `SFMLInputRegistry` and
`EventHandler` APIs are already written to accept any `std::vector<SFMLInputBinding>`.

---

## Testing

Unit tests for the registry are in
`tests/unit/events/SFMLInputRegistry.test.cpp`. They cover:

- `Configure()` storing bindings and clearing held state
- Single keyboard key: PRESSED and RELEASED triggers
- Single mouse button: PRESSED and RELEASED triggers
- AND logic: action fires only when all required inputs are held
- AND logic RELEASED: fires when the combination breaks
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

### Action fires on the wrong state (PRESSED vs RELEASED)

Check that the `trigger_state` on the binding matches the `state` in any
`filter_payload` on the subscriber. A PRESSED binding only emits a PRESSED
payload, and the matcher checks both fields.

### AND binding never fires

Verify that every `SFMLInputEntry` in `required_inputs` uses the correct `type`
field. If `type` is `Keyboard`, the `keyboard_key` field is used; if `type` is
`MouseButton`, the `mouse_button` field is used. A wrong `type` means the
registry will always look in the wrong held set.

### Bindings reset unexpectedly

`ConfigureInputRegistry()` clears all held state. If it is called while keys
are physically held (e.g., during a scene transition), those keys will not be
tracked until they are released and pressed again. Call `Configure` only when
the input focus is clean (e.g., before the first frame of a scene, not mid-frame).

### No events reaching subscribers

Confirm the full pipeline is running each frame:

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
| `src/types/events/EventPayload.h` | `InputPayload`, `InputAction`, `InputState` |
| `src/events/SFMLInputBinding.h` | `SFMLInputEntry`, `SFMLInputBinding`, `GetDefaultSFMLInputBindings()` |
| `src/events/SFMLInputRegistry.h/.cpp` | Registry that converts SFML events to EventPackets |
| `src/events/EventHandler.h/.cpp` | `ConfigureInputRegistry`, `ProcessInputEvent`, `GetInputRegistry` |
| `src/events/event_factory.h/.cpp` | `CreateInputEventPacket` (with optional `InputState` overload) |
| `src/events/payload_matchers.cpp` | `MatchPayload(InputPayload, InputPayload)` — compares action and state |
| `tests/unit/events/SFMLInputRegistry.test.cpp` | Unit tests for the registry |
