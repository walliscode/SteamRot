# Coordinate Spaces in SteamRot: Implementation Guide

> **Prerequisite**: Read [Coordinate Spaces: Screen Space vs World Space](COORDINATE_SPACES.md)
> for the foundational concepts before reading this document.

---

## The Coordinate Contract

SteamRot enforces the screen/world split through two named, typed fields on
`SceneContext` that are always available to every Logic class:

| Field | Type | Space | Canonical consumers |
|---|---|---|---|
| `scene_context.mouse_position` | `sf::Vector2i` | Screen pixels | UI collision only |
| `scene_context.world_mouse_position` | `sf::Vector2f` | World space | Ghost, grimoire collision, piece placement |

The different C++ types (`Vector2i` vs `Vector2f`) make misuse visible at the
call site: if you accidentally pass `world_mouse_position` to a function that
takes `sf::Vector2i`, or pass `mouse_position` to one that takes `sf::Vector2f`,
the compiler will warn or error. There is no silent wrong-zoom bug.

---

## Where the Conversion Happens

The conversion from screen space to world space is performed **exactly once per
tick**, in `GhostPositioningLogic::ProcessLogic()` via the free function
`positioning::ghost::UpdatePosition()` in `src/logic/positioning_ghost.cpp`.

```
GhostPositioningLogic::ProcessLogic()
  └─ positioning::ghost::UpdatePosition(
         mr_ghost,
         scene_context.world_mouse_position,   ← out-ref, written here
         scene_context.mouse_position,          ← screen-space input
         scene_context.camera_state,            ← zoom level
         scene_context.scene_texture)           ← render target
       │
       └─ world_mouse_position =
              camera_state.MapToWorldCoords(mouse_position, scene_texture)
              // internally: scene_texture.mapPixelToCoords(
              //     screen_pos, camera_state.GetWorldView(scene_texture))
```

After `GhostPositioningLogic` runs, every subsequent Logic class in the same
tick can read `scene_context.world_mouse_position` and receive the correct
world-space cursor.

### Execution Order

`GhostPositioningLogic` is registered as a **Movement** logic in the crafting
scene (via `LogicFactory`). The execution order within a tick is:

```
Collision → Render → Action → Movement
```

This means world_mouse_position is computed during the Movement phase and is
available for the *next* tick's Collision, Render, and Action phases. The value
is initialised to `{0.f, 0.f}` at scene startup and updated every tick, so it
is never stale beyond one tick.

> **Note**: If you add a new world-space Logic that runs in the same tick as
> `GhostPositioningLogic` and needs the current mouse position, make sure it
> runs *after* `GhostPositioningLogic` in the Movement phase — or reads from
> the pre-converted value produced in the previous tick (which is fine for
> a one-tick lag in most cases).

---

## The Ownership Chain

```
SceneResources
  ├── camera_state : CameraState         (owns zoom, provides GetWorldView / MapToWorldCoords)
  ├── world_mouse_position : sf::Vector2f (written once per tick by GhostPositioningLogic)
  └── mr_ghost : MrGhost                 (position set from world_mouse_position)

SceneContext (references into SceneResources)
  ├── camera_state          → SceneResources::camera_state
  ├── world_mouse_position  → SceneResources::world_mouse_position
  └── mouse_position        → EngineResources::mouse_position  (screen space, read-only)
```

`CameraState::MapToWorldCoords` is the single authoritative conversion function.
It wraps `sf::RenderTexture::mapPixelToCoords` with the correct zoomed world
view and should be the only call site for screen→world mouse conversion in the
entire codebase.

---

## How Each Consumer Uses the Fields

### UI Collision (`UICollisionLogic` / `collision_mouse.cpp`)

Uses `scene_context.mouse_position` (`sf::Vector2i`) directly. UI elements are
positioned in screen space — their bounding boxes are in screen pixels — so no
conversion is needed. **This code must never use `world_mouse_position`.**

```cpp
// UICollisionLogic.cpp — correct, no change needed
collision::mouse::ProcessUIEntityCollisions(
    entity_ids,
    scene_context.scene_entities,
    scene_context.mouse_position);           // ← screen-space, intentional
```

### Grimoire Socket Collision (`GrimoireMachinaCollisionLogic`)

Fragment and joint sockets live in world space. Their positions are transformed
by `sf::Transform` (world-space transform). Comparing them against the mouse
requires a world-space mouse.

```cpp
// GrimoireMachinaCollisionLogic.cpp
collision::mouse::ProcessScaffoldCollisions(
    *active_scaffold_form,
    scene_context.world_mouse_position);     // ← world-space, correct
```

The `ProcessScaffoldCollisions` / `CheckMouseOver` overloads that accept
`sf::Vector2f` perform a pure world↔world distance check (squared distance vs
radius²) — no coordinate conversion inside.

### Ghost Positioning (`GhostPositioningLogic`)

*Produces* `world_mouse_position` rather than consuming it. It is the only
Logic that writes to both `world_mouse_position` and `mr_ghost.m_position`.

### Piece Placement (`GrimoireMachinaActionLogic` / `action_grimoire_machina.cpp`)

Uses `scene_context.world_mouse_position` for placing a piece at the cursor.

The first piece on an empty scaffold is special: it is centred on the *canvas*
midpoint (a fixed screen position), not the mouse cursor. That calculation uses
its own explicit `mapPixelToCoords` call with a comment explaining why — it is
a canvas-pixel-to-world conversion, not a mouse-to-world conversion, and must
not be collapsed into `world_mouse_position`.

```cpp
sf::Vector2f place_pos = scene_context.world_mouse_position; // ← cursor placement

if (is_first_piece) {
    // Canvas centre is a fixed screen position — convert it explicitly.
    // This is NOT the mouse-to-world conversion; it is a canvas-pixel-to-world
    // conversion and must stay here even though the pattern looks similar.
    place_pos = scene_context.scene_texture.mapPixelToCoords(
        canvas_center_pixel, world_view);
}
```

---

## Pitfalls to Avoid

### ❌ Casting `mouse_position` to float and using it as world position

```cpp
// WRONG — compiles silently, breaks at zoom ≠ 1.0
sf::Vector2f world_pos = sf::Vector2f(scene_context.mouse_position);
```

The int→float cast does nothing to the coordinate values; it just changes the
numeric type without applying the camera transform. At zoom 1.0 the window and
world coincidentally align so this appears to work, hiding the bug until the
camera zooms.

### ❌ Calling `mapPixelToCoords` with the wrong view

```cpp
// WRONG — uses the default (unzoomed) view instead of the world view
sf::Vector2f world_pos = scene_context.scene_texture.mapPixelToCoords(
    scene_context.mouse_position
    // note: no view argument — defaults to the texture's default view
);
```

SFML's `mapPixelToCoords` without a view argument uses the current view *set on
the texture*, which may be the UI view (unzoomed default) after the render pass
has switched views. Always pass the explicit world view from `GetWorldView()` —
or better, use `CameraState::MapToWorldCoords` which does this correctly.

### ❌ Repeating the conversion in multiple Logic classes

If more than one Logic class independently calls `MapToWorldCoords` per tick,
the result is technically correct but breaks the "convert once" principle. The
cost is small, but having multiple conversion sites makes it harder to audit
where the world mouse comes from and harder to trace zoom-related bugs.

Always read `scene_context.world_mouse_position` instead of calling the
conversion yourself.

### ❌ Passing `world_mouse_position` to UI collision

```cpp
// WRONG — UI bounds are in screen space; world-space input gives wrong hover
collision::mouse::CheckMouseOver(
    scene_context.world_mouse_position,   // sf::Vector2f — wrong overload
    *ui_root_element);
```

`CheckMouseOver(sf::Vector2i, UIElement&)` takes a `Vector2i` for exactly this
reason — it will not compile if you try to pass a `Vector2f`. Do not add an
overload that accepts `sf::Vector2f` for UIElement checking.

### ❌ Adding a new world-space Logic that reads `world_mouse_position` before `GhostPositioningLogic` has run

Because `GhostPositioningLogic` runs in the Movement phase, Logic classes that
run in the same tick's Collision, Render, or Action phases will read the value
from the *previous* tick. For most use cases a one-tick lag is imperceptible,
but for precise placement or fine-grained collision, be aware of this ordering.

If you need guaranteed same-tick conversion, place your Logic after
`GhostPositioningLogic` in the Movement phase execution order
(see `LogicFactory.cpp`).

---

## Rules for Adding New World-Space Logic

1. **Read `scene_context.world_mouse_position`** — do not call `mapPixelToCoords`
   yourself.
2. **Accept `sf::Vector2f` for any world-position parameter** — the compiler will
   prevent accidental screen-space input.
3. **Never implicitly cast `mouse_position` to float** — if you see `sf::Vector2f(mouse_position)` in new code it is almost certainly a bug.
4. **Canvas-pixel conversions are different from mouse-to-world** — if you need
   to convert a fixed UI point (button centre, canvas edge) to world space, do
   it explicitly at the call site with a comment; do not fold it into the mouse
   conversion path.
5. **Update `GhostPositioningLogic` only if the conversion source changes** —
   e.g. if a panning camera is added that shifts the view centre, the change
   belongs in `CameraState::GetWorldView` and `MapToWorldCoords`, not in each
   individual Logic class.

---

## Quick Reference: Which Field to Use

| What you need | Field to use | Type |
|---|---|---|
| "Is the mouse over this UI button?" | `mouse_position` | `sf::Vector2i` |
| "Is the mouse over this socket/fragment/joint?" | `world_mouse_position` | `sf::Vector2f` |
| "Where should I place the ghost piece?" | `world_mouse_position` | `sf::Vector2f` |
| "Where should I place the first piece (canvas centre)?" | explicit `mapPixelToCoords` call | `sf::Vector2f` |
| "What is the current zoom level?" | `camera_state.m_zoom_level` | `float` |
| "Convert an arbitrary screen point to world?" | `camera_state.MapToWorldCoords(...)` | `sf::Vector2f` |
