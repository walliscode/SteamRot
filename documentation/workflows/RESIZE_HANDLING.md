# Window Resize Handling

This document explains how window resize events flow through SteamRot and how
to extend the system to support per-UI-element sizing rules in the future.

## Table of Contents

- [Architecture Overview](#architecture-overview)
- [Data Flow](#data-flow)
- [Current Implementation: ResizeSceneTexture](#current-implementation-resizescenetexture)
- [Marrying Resize with the UI Layer](#marrying-resize-with-the-ui-layer)
  - [The Extension Point](#the-extension-point)
  - [Adding a UI Resize Free Function](#adding-a-ui-resize-free-function)
  - [Example Sizing Rules](#example-sizing-rules)
    - [Scale proportionally with the window](#scale-proportionally-with-the-window)
    - [Fixed size (menu stays constant)](#fixed-size-menu-stays-constant)
    - [Capped size (e.g. a drop-down with a max width)](#capped-size-eg-a-drop-down-with-a-max-width)
- [Why No New Logic Is Needed Throughout the System](#why-no-new-logic-is-needed-throughout-the-system)
- [Future Enhancements](#future-enhancements)
  - [Persistent base sizes](#persistent-base-sizes)
  - [Strategy objects on UIElement](#strategy-objects-on-uielement)
- [Files at a Glance](#files-at-a-glance)

---

## Architecture Overview

The resize pipeline follows the same separation-of-concerns pattern used
throughout the engine:

| Layer | File(s) | Responsibility |
|-------|---------|----------------|
| SFML → Event | `sfml_event_convert.h/.cpp` | `ConvertResizeEvents()` detects `sf::Event::Resized` and converts it to a `SystemPayload{RESIZE, new_size}` `EventPacket` |
| Event routing | `SFMLEventConverter.cpp` | Step 5 of `ConvertSFMLEvents()` calls `ConvertResizeEvents()` |
| Event bus | `EventHandler` | Standard subscription/notification — no resize-specific code |
| Logic | `ResizeLogic.h/.cpp` | Subscribes to `SYSTEM/RESIZE`; dispatches to `resize_logic` free functions |
| Free functions | `resize_logic.h/.cpp` | Isolated, independently testable operations |

---

## Data Flow

```
sf::Event::Resized
      │ ConvertResizeEvents() [sfml_event_convert.cpp]
      ▼
EventPacket { SYSTEM, SystemPayload { RESIZE, sf::Vector2u{w,h} } }
      │ enters EventHandler waiting-room bus
      ▼
EventHandler::UpdateSubscribersFromGlobalEventBus()
      │ activates ResizeLogic's Subscriber
      ▼
ResizeLogic::ProcessLogic()          [ResizeLogic.cpp]
      │ reads captured_payload → SystemPayload.optional_resize_size
      ▼
resize::ResizeSceneTexture()         [resize_logic.cpp]
      │ sf::RenderTexture::resize(new_size)
      ▼
Scene texture resized — next render cycle uses new dimensions
```

---

## Current Implementation: ResizeSceneTexture

`resize::ResizeSceneTexture` in `src/logic/resize_logic.h/.cpp` is the only
operation currently performed on resize.  It resizes the scene's
`sf::RenderTexture` to match the new window dimensions so that the render
output fills the window exactly.

This is the simplest possible behaviour and is intentionally kept minimal so
that the system is easy to extend without restructuring.

---

## Marrying Resize with the UI Layer

### The Extension Point

`ResizeLogic::ProcessLogic()` is the single place where a window resize event
is handled.  To apply sizing rules to UI elements you only need to:

1. Add a new free function in `resize_logic.h/.cpp` (or a sibling file) that
   accepts the `CUserInterface` component (or individual `UIElement`s) and the
   new window size.
2. Call that function from `ResizeLogic::ProcessLogic()` after resizing the
   scene texture.

No other part of the engine needs to change — the event routing, the
subscription system, and the render/collision logic are all unaffected.

### Adding a UI Resize Free Function

```cpp
// src/logic/resize_logic.h — add declaration

namespace steamrot::logic::resize {

/// @brief Apply sizing rules to every active UI entity after a window resize.
///
/// @param new_size       New window dimensions.
/// @param scene_entities Entity memory pool for the scene.
/// @param archetypes     Archetype map for the scene.
void ResizeUIElements(const sf::Vector2u &new_size,
                      EntityMemoryPool &scene_entities,
                      const std::unordered_map<ArchetypeID, Archetype> &archetypes);

} // namespace steamrot::logic::resize
```

```cpp
// src/logic/resize_logic.cpp — add implementation

void resize::ResizeUIElements(
    const sf::Vector2u &new_size,
    EntityMemoryPool &scene_entities,
    const std::unordered_map<ArchetypeID, Archetype> &archetypes) {

  auto entity_ids =
      archetypes::GenerateEntityIndexesFromComponents<CUserInterface>(
          archetypes, /*active_only=*/true);

  for (size_t id : entity_ids) {
    CUserInterface &ui =
        entity::memory::GetComponent<CUserInterface>(id, scene_entities);
    if (ui.m_root_element) {
      ApplySizingRule(*ui.m_root_element, new_size);
    }
  }
}
```

Then in `ResizeLogic::ProcessLogic()` call it after `ResizeSceneTexture`:

```cpp
resize::ResizeUIElements(system_payload.optional_resize_size.value(),
                         m_scene_context.scene_entities,
                         m_scene_context.archetypes);
```

### Example Sizing Rules

All rules are free functions that accept a `UIElement &` and the new window
size.  They live in `resize_logic.cpp` (or a dedicated
`resize_ui_elements.cpp`) and are unit-tested in isolation.

#### Scale proportionally with the window

```cpp
void ScaleProportional(UIElement &element,
                       const sf::Vector2f &original_window_size,
                       const sf::Vector2u &new_size) {
  float scale_x = static_cast<float>(new_size.x) / original_window_size.x;
  float scale_y = static_cast<float>(new_size.y) / original_window_size.y;
  element.size.x *= scale_x;
  element.size.y *= scale_y;
  element.position.x *= scale_x;
  element.position.y *= scale_y;
}
```

#### Fixed size (menu stays constant)

```cpp
void KeepFixed(UIElement &element) {
  // No changes — position and size are unchanged by the resize.
  (void)element;
}
```

#### Capped size (e.g. a drop-down with a max width)

```cpp
void ApplyMaxWidth(UIElement &element,
                   float max_width,
                   const sf::Vector2u &new_size) {
  float desired = static_cast<float>(new_size.x) * 0.3f; // 30 % of window
  element.size.x = std::min(desired, max_width);
}
```

Each rule is a free function: easy to unit-test, easy to compose, and
completely isolated from the event system.

---

## Why No New Logic Is Needed Throughout the System

The architecture ensures that adding new sizing behaviours does **not** require
touching:

- **SFMLEventConverter** — the RESIZE event is already emitted and routed.
- **EventHandler / Subscriber** — ResizeLogic is already subscribed; no new
  subscriber registration is needed.
- **UIActionLogic / UICollisionLogic / UIRenderLogic** — they read `position`
  and `size` from `UIElement` directly.  As long as resize functions update
  those fields before the render logic runs (which is guaranteed because
  `ResizeLogic` runs first in the Action grouping), the render logic will pick
  up the new values automatically.
- **LogicFactory** — `ResizeLogic` is already registered as the first Action
  logic in every scene.  Any per-scene sizing rules can be implemented inside
  `ResizeLogic::ProcessLogic()` by querying the scene entities/archetypes that
  are already available through `m_scene_context`.

The only files that change when a new sizing rule is added are:

| File | Change |
|------|--------|
| `src/logic/resize_logic.h` | Add declaration |
| `src/logic/resize_logic.cpp` | Add implementation |
| `src/logic/ResizeLogic.cpp` | Call new function from `ProcessLogic()` |
| `tests/unit/logic/resize_logic.test.cpp` | Add unit tests |

---

## Future Enhancements

### Persistent base sizes

Currently `UIElement::size` stores the *current* size.  If resize functions
scale relative to a previous size they need an original reference.  A clean
approach is to add an `original_size` field to `UIElement` (set once at load
time, never mutated by resize) so that scaling is always calculated from a
stable baseline:

```cpp
struct UIElement {
  sf::Vector2f size{0.f, 0.f};          // current size — updated on resize
  sf::Vector2f original_size{0.f, 0.f}; // set at load, never changes
  // ...
};
```

### Strategy objects on UIElement

For fine-grained, per-element control a `ResizeBehaviour` enum (or a small
interface) can be attached to each `UIElement`:

```cpp
enum class ResizeBehaviour {
  ScaleProportional,   // scales with the window
  Fixed,               // always keeps its loaded size/position
  CappedWidth,         // scales but respects a maximum width
};
```

`ResizeLogic` would then dispatch to the correct free function based on that
field.  Because the field lives on `UIElement` and the dispatch lives in a free
function, neither the event routing nor the render/collision logic needs to
change.

---

## Files at a Glance

| File | Purpose |
|------|---------|
| `src/events/sfml_event_convert.h/.cpp` | `ConvertResizeEvents()` — SFML → EventPacket |
| `src/events/SFMLEventConverter.cpp` | Pipeline step 5 — calls `ConvertResizeEvents()` |
| `src/types/events/EventPayload.h` | `SystemPayload::SystemAction::RESIZE` + `optional_resize_size` |
| `src/logic/ResizeLogic.h/.cpp` | Logic class — subscribes, dispatches |
| `src/logic/resize_logic.h/.cpp` | Free functions — isolated resize operations |
| `tests/unit/events/sfml_event_convert.test.cpp` | Tests for `ConvertResizeEvents()` |
| `tests/unit/events/SFMLEventConverter.test.cpp` | Integration tests for the full converter |
| `tests/unit/logic/resize_logic.test.cpp` | Tests for free functions and `ResizeLogic` |
