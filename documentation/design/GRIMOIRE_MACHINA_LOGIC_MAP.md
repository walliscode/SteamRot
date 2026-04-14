# GrimoireMachina Logic Responsibility Map

This document audits and maps all logic related to `GrimoireMachina`, categorising
each block by responsibility type: **Collision**, **Action**, **Positioning**, or
**Render**. It serves as the foundation for the modularisation effort tracked under
[#1395 – Restructure GrimoireMachina Logic](https://github.com/walliscode/SteamRot/issues/1395).

---

## Table of Contents

- [Overview](#overview)
- [Logic Classes](#logic-classes)
  - [Collision — GrimoireMachinaCollisionLogic](#collision--grimoiremachinacollisionlogic)
  - [Action — GrimoireMachinaActionLogic](#action--grimoiremachinaactionlogic)
  - [Positioning — GrimoireMachinaPositioningLogic](#positioning--grimoiremachinapositioninglogic)
  - [Render — GrimoireMachinaRenderLogic](#render--grimoiremachinarenderlogic)
- [Free-Function Modules](#free-function-modules)
  - [Collision — collision_mouse](#collision--collision_mouse)
  - [Action — action_grimoire_machina](#action--action_grimoire_machina)
  - [Positioning — positioning_grimoire_machina](#positioning--positioning_grimoire_machina)
  - [Render — render_grimoire_machina](#render--render_grimoire_machina)
- [Data Types Referenced](#data-types-referenced)
- [Execution Order in the CRAFTING Scene](#execution-order-in-the-crafting-scene)
- [Gaps and TODOs](#gaps-and-todos)

---

## Overview

`GrimoireMachina` is the runtime workspace for the crafting system. It holds the
library of available `Fragment` and `Joint` parts (`m_all_fragments`,
`m_all_joints`), the catalogue of completed `MachinaForm` blueprints
(`m_machina_forms`), and an in-progress `MachinaFormScaffold` (`m_scaffold_form`)
that the player builds one piece at a time.

All four responsibility types (Collision, Action, Positioning, Render) are already
separated into distinct `Logic` classes and matching free-function modules. The
table below shows the mapping at a glance.

| Responsibility | Logic class                          | Free-function module            | Namespace                                         |
|----------------|--------------------------------------|---------------------------------|---------------------------------------------------|
| Collision      | `GrimoireMachinaCollisionLogic`      | `collision_mouse.h/cpp`         | `steamrot::logic::collision::mouse`               |
| Action         | `GrimoireMachinaActionLogic`         | `action_grimoire_machina.h/cpp` | `steamrot::logic::action::grimoire_machina`       |
| Positioning    | `GrimoireMachinaPositioningLogic`    | `positioning_grimoire_machina.h/cpp` | `steamrot::logic::positioning::grimoire_machina` |
| Render         | `GrimoireMachinaRenderLogic`         | `render_grimoire_machina.h/cpp` | `steamrot::logic::render::grimoire_machina`       |

---

## Logic Classes

### Collision — GrimoireMachinaCollisionLogic

**Files**: `src/logic/GrimoireMachinaCollisionLogic.h/cpp`

**Position in CRAFTING scene**: 2nd in the Collision group (runs after
`UICollisionLogic`).

**What it does**:

1. Fetches `GrimoireMachina` from the `AssetManager`.
2. Checks whether an active `MachinaFormScaffold` exists on the returned object.
3. If a scaffold is present, delegates to
   `collision::mouse::ProcessScaffoldCollisions` (see below) to update
   per-socket `is_mouse_over` flags for every placed `JointInstance` and
   `FragmentInstance`.

**Why Collision**: All work here updates hover-state booleans that other systems
(Action, Render) read later in the same tick. No state is written other than
`SocketState::is_mouse_over` on scaffold parts.

```
GrimoireMachinaCollisionLogic::ProcessLogic()
  └── collision::mouse::ProcessScaffoldCollisions(scaffold, world_mouse_pos)
        ├── CheckMouseOver(world_mouse, JointInstance &)    // for each joint
        └── CheckMouseOver(world_mouse, FragmentInstance &) // for each fragment
```

---

### Action — GrimoireMachinaActionLogic

**Files**: `src/logic/GrimoireMachinaActionLogic.h/cpp`

**Position in CRAFTING scene**: 4th in the Action group, after `CameraActionLogic`,
`UIActionLogic`, and `UIStateLogic`; before `GhostActionLogic`.

**What it does**:

1. Fetches `GrimoireMachina` from the `AssetManager`.
2. Calls `action::grimoire_machina::ProcessSubscribers` which dispatches each
   active subscriber to either `ProcessLogicEvents` (for `EventType::LOGIC`) or
   `ProcessUserInputEvents` (for `EventType::USER_INPUT`).

**Why Action**: This is pure state-mutation triggered by events. No positions are
updated and nothing is drawn.

```
GrimoireMachinaActionLogic::ProcessLogic()
  └── action::grimoire_machina::ProcessSubscribers(subscribers, scene_context, grimoire_machina)
        ├── ProcessLogicEvents(subscriber, grimoire_machina)          // LOGIC events
        │     ├── InitialiseActiveMachinaFormScaffold(grimoire_machina)  // INITIATE_MACHINA_FORM_SCAFFOLD
        │     └── ClearActiveMachinaFormScaffold(grimoire_machina)       // CLEAR_MACHINA_FORM_SCAFFOLD
        └── ProcessUserInputEvents(subscriber, scene_context, grimoire_machina) // USER_INPUT events
              ├── ProcessSocketVisibilitySubscribers(subscriber, grimoire_machina) // TOGGLE_SOCKET_VISIBILITY
              └── ProcessPlacementSubscribers(subscriber, scene_context, grimoire_machina) // SELECT
                    └── PlaceGhostOnScaffold(grimoire_machina, mr_ghost, world_pos)
                          └── PlaceFirstPiece(grimoire_machina, mr_ghost, world_pos)  // when scaffold is empty
```

---

### Positioning — GrimoireMachinaPositioningLogic

**Files**: `src/logic/GrimoireMachinaPositioningLogic.h/cpp`

**Position in CRAFTING scene**: 2nd in the Movement group (after
`UIPositioningLogic`, before `GhostPositioningLogic`).

**What it does**: **Intentionally empty.** The crafting canvas uses an infinite
world-space coordinate system driven by the camera. There is no fixed viewport
or canvas boundary that requires per-tick position recalculation for scaffold
parts. The class is retained to preserve `LogicFactory` and `LogicType` enum
compatibility for future use.

**Why retained**: When socket-based snapping or layout-assist features are added,
this class and the companion `positioning_grimoire_machina.h/cpp` module are the
correct insertion points.

---

### Render — GrimoireMachinaRenderLogic

**Files**: `src/logic/GrimoireMachinaRenderLogic.h/cpp`

**Position in CRAFTING scene**: 2nd in the Render group (after `UIRenderLogic`,
before `GhostRenderLogic`).

**What it does**:

1. Applies the world (zoomed) view to the `scene_texture` using
   `camera_state.GetWorldView()` so that subsequent draw calls are in world
   space.
2. Fetches `GrimoireMachina` from the `AssetManager`.
3. Delegates to `render::grimoire_machina::DrawScaffoldOrPlaceholder`.

**Why Render**: All work here is limited to drawing vertices/shapes to
`scene_texture`. No game state is mutated.

```
GrimoireMachinaRenderLogic::ProcessLogic()
  ├── scene_texture.setView(camera_state.GetWorldView(...))
  └── render::grimoire_machina::DrawScaffoldOrPlaceholder(scene_texture, grimoire_machina)
        ├── (no scaffold) → DrawNoMachinaFormIndicator(texture)
        └── (scaffold present) → DrawJointInstance / DrawFragmentInstance for each placed part
              ├── DrawJointInstance(texture, joint_instance, draw_sockets)
              │     ├── DrawJointView(texture, joint, ViewDirection::Front, states)
              │     └── DrawJointInstanceSockets(texture, joint_instance)
              │           └── DrawSocket(texture, world_pos, socket_state)  // for each socket
              └── DrawFragmentInstance(texture, fragment_instance, draw_sockets)
                    ├── DrawFragmentView(texture, fragment, ViewDirection::Front, states)
                    └── DrawFragmentInstanceSockets(texture, fragment_instance)
                          └── DrawSocket(texture, world_pos, socket_state)  // for each socket
```

---

## Free-Function Modules

### Collision — `collision_mouse`

**Files**: `src/logic/collision_mouse.h/cpp`
**Namespace**: `steamrot::logic::collision::mouse`

The module is shared between UI collision and GrimoireMachina collision. The
GrimoireMachina-specific surface area is:

| Function | Signature | Responsibility |
|----------|-----------|----------------|
| `CheckMouseOver` | `(sf::Vector2f world_mouse, sf::Vector2f world_pos, SocketState &)` | Sets `is_mouse_over` on a single `SocketState` based on distance from the mouse to the socket's world centre. Hit radius matches the visual draw radius. |
| `CheckMouseOver` | `(sf::Vector2f world_mouse, FragmentInstance &)` | Iterates all sockets of a `FragmentInstance`. For each socket, applies the instance's transform to the local socket position to get world space, then calls the `SocketState` overload. |
| `CheckMouseOver` | `(sf::Vector2f world_mouse, JointInstance &)` | Like the `FragmentInstance` overload but uses `ComputeSocketLocalPos` to account for `joint_instance.current_rotation` before transforming to world space. |
| `ProcessScaffoldCollisions` | `(MachinaFormScaffold &, sf::Vector2f world_mouse)` | Top-level entry point. Iterates all `JointInstance`s then all `FragmentInstance`s in the scaffold and calls the appropriate `CheckMouseOver` overload for each. |

---

### Action — `action_grimoire_machina`

**Files**: `src/logic/action_grimoire_machina.h/cpp`
**Namespace**: `steamrot::logic::action::grimoire_machina`

| Function | Responsibility |
|----------|----------------|
| `InitialiseActiveMachinaFormScaffold` | Resets any existing `m_scaffold_form` and allocates a fresh `MachinaFormScaffold`. Triggered by `LogicPayload::INITIATE_MACHINA_FORM_SCAFFOLD`. |
| `ClearActiveMachinaFormScaffold` | Sets `m_scaffold_form` to `nullptr`. Triggered by `LogicPayload::CLEAR_MACHINA_FORM_SCAFFOLD`. |
| `GetAllFragmentNames` | Returns a `vector<string>` of all keys in `m_all_fragments`. Used to populate UI drop-down lists with available fragment names. |
| `GetAllJointNames` | Returns a `vector<string>` of all keys in `m_all_joints`. Used to populate UI drop-down lists with available joint names. |
| `ProcessLogicEvents` | Extracts a `LogicPayload` from `subscriber.captured_payload` and dispatches to `Initialise…` or `Clear…` based on the `LogicToggle` value. |
| `ProcessSocketVisibilitySubscribers` | Toggles `MachinaFormScaffold::are_sockets_visible` when the subscriber carries `InputPayload::TOGGLE_SOCKET_VISIBILITY`. |
| `ProcessPlacementSubscribers` | Places the currently selected ghost item onto the scaffold when a `SELECT` input arrives and all three placement guards pass: (1) a ghost item is selected, (2) the click does not land on a visible UI element, (3) an active scaffold exists. |
| `ProcessUserInputEvents` | Routes a `USER_INPUT` subscriber to `ProcessSocketVisibilitySubscribers` and `ProcessPlacementSubscribers`. |
| `PlaceFirstPiece` | Handles the first piece on an empty scaffold. Looks up the `Fragment` or `Joint` identified by the ghost selection tag, centers it on `world_pos`, and appends a new instance to the scaffold with a fresh `next_id`. |
| `PlaceGhostOnScaffold` | Routes to `PlaceFirstPiece` when the scaffold is empty. For subsequent pieces a positive socket-proximity result is required; because that collision detection is not yet implemented this path always returns an error. |
| `ProcessSubscribers` | Top-level dispatcher. Iterates `subscribers`, skips inactive ones, and routes each to `ProcessLogicEvents` or `ProcessUserInputEvents` based on `event_type`. |

---

### Positioning — `positioning_grimoire_machina`

**Files**: `src/logic/positioning_grimoire_machina.h/cpp`
**Namespace**: `steamrot::logic::positioning::grimoire_machina`

Currently empty. Reserved for future positioning logic such as socket-snap
assistance, layout-constraint resolution, or piece-alignment helpers.

---

### Render — `render_grimoire_machina`

**Files**: `src/logic/render_grimoire_machina.h/cpp`
**Namespace**: `steamrot::logic::render::grimoire_machina`

| Function | Responsibility |
|----------|----------------|
| `DrawEmptyActiveMachinaForm` | Stub — currently empty body. Intended for drawing visual indicators for an active but empty `MachinaForm`. |
| `DrawNoMachinaFormIndicator` | Draws a red 200×200 outline rectangle centred at world-space origin `(0, 0)` to signal that no scaffold is active. |
| `DrawSocket` | Draws a small circle at a given world position. Colour is yellow when `socket_state.is_mouse_over` is true, white otherwise. |
| `DrawFragmentInstanceSockets` | Iterates all sockets of a `FragmentInstance`, applies the instance's transform to the local socket position, and calls `DrawSocket` for each. |
| `DrawJointInstanceSockets` | Like `DrawFragmentInstanceSockets` but uses `ComputeSocketLocalPos` to resolve the rotation-aware local position before transforming to world space. |
| `DrawFragmentInstance` | Sets `RenderStates` from the instance's stored transform, draws the fragment's `Front` view geometry, and optionally draws all socket indicators. |
| `DrawJointInstance` | Same as `DrawFragmentInstance` for joints. |
| `DrawScaffoldOrPlaceholder` | Checks whether `m_scaffold_form` is set. If not, calls `DrawNoMachinaFormIndicator`. If set, iterates all joints then all fragments and calls the appropriate `Draw*Instance` function for each. |
| `DrawView` | Draws a single vertex array from a `Views` object selected by `ViewDirection`. |
| `DrawFragmentView` (no states) | Draws a fragment's movement view for the given direction at its stored world positions. Used when no transform is needed. |
| `DrawFragmentView` (with states) | Like above but applies `sf::RenderStates` (e.g. a translation transform). Used by `GhostRenderLogic` to position ghost geometry at the cursor. |
| `DrawJointView` (no states) | Draws a joint's movement view for the given direction at its stored world positions. |
| `DrawJointView` (with states) | Like above but applies `sf::RenderStates`. Used by `GhostRenderLogic`. |

---

## Data Types Referenced

| Type | Location | Notes |
|------|----------|-------|
| `GrimoireMachina` | `src/types/entity/GrimoireMachina.h` | Top-level asset. Holds `m_all_fragments`, `m_all_joints`, `m_machina_forms`, and `m_scaffold_form`. Owned by `AssetManager`. |
| `MachinaFormScaffold` | `src/types/entity/MachinaFormScaffold.h` | The in-progress editor workspace. Contains `next_id`, `joints` (`vector<JointInstance>`), `fragments` (`vector<FragmentInstance>`), and `are_sockets_visible`. |
| `MachinaForm` | `src/types/entity/MachinaForm.h` | Baked, read-only blueprint. Holds `m_fragments` and `m_joints`. Not mutated at runtime. |
| `Fragment` / `FragmentInstance` | `src/types/entity/Fragment.h` | `Fragment` is the library entry; `FragmentInstance` is a placed copy on the scaffold with its own `transform`, `socket_states`, and `id`. |
| `Joint` / `JointInstance` | `src/types/entity/Joint.h` | Same pattern as Fragment/FragmentInstance. `JointInstance` additionally carries `current_rotation` and a `socket_config`. |
| `SocketState` | `src/types/entity/MachinaFormScaffold.h` | Runtime hover state for a single socket (`is_mouse_over`). |
| `Views` | `src/types/entity/ViewDirection.h` | `std::map<ViewDirection, sf::VertexArray>` — one static shape per direction. |
| `MrGhost` | `src/types/core/MrGhost.h` | Holds `m_selection` (`std::variant<std::monostate, FragmentTag, JointTag>`) and `m_position`. Lives in `SceneResources::mr_ghost`, exposed via `SceneContext`. |

---

## Execution Order in the CRAFTING Scene

The following order is defined in `src/logic/LogicFactory.cpp::ConfigureCraftingLogics`:

```
Collision group (runs first)
  1. UICollisionLogic
  2. GrimoireMachinaCollisionLogic   ← sets is_mouse_over on scaffold sockets

Action group
  1. CameraActionLogic
  2. UIActionLogic
  3. UIStateLogic
  4. GrimoireMachinaActionLogic      ← reads collision state; mutates scaffold
  5. GhostActionLogic

Render group
  1. UIRenderLogic
  2. GrimoireMachinaRenderLogic      ← draws scaffold to world-space texture
  3. GhostRenderLogic

Movement group
  1. UIPositioningLogic
  2. GrimoireMachinaPositioningLogic ← empty; reserved for future positioning
  3. GhostPositioningLogic
```

The collision → action → render ordering is intentional: collision sets hover
booleans, action reads them to decide whether a placement or socket-toggle is valid,
and render uses the same booleans to choose socket colours.

---

## Gaps and TODOs

The following items are incomplete and are noted here to guide future subtasks
within the [#1395 epic](https://github.com/walliscode/SteamRot/issues/1395).

| Location | Description |
|----------|-------------|
| `action_grimoire_machina.cpp :: PlaceGhostOnScaffold` | Socket-proximity collision for subsequent pieces is not yet implemented. `PlaceGhostOnScaffold` always returns an error for non-first placements. Tracked by `// TODO:` comment at line 170. |
| `action_grimoire_machina.cpp :: ProcessLogicEvents` | The `expected` results from `InitialiseActiveMachinaFormScaffold` and `ClearActiveMachinaFormScaffold` are discarded. Failures are silently ignored. Tracked by `// TODO: handle the result` comments. |
| `action_grimoire_machina.cpp :: ProcessPlacementSubscribers` | Same — the result of `PlaceGhostOnScaffold` is discarded. |
| `render_grimoire_machina.cpp :: DrawEmptyActiveMachinaForm` | Function body is empty. Intended to draw a visual indicator for an active but empty `MachinaForm` blueprint (distinct from the scaffold-absent placeholder). |
| `positioning_grimoire_machina.h/cpp` | Empty module. Future home for socket-snapping, layout-constraint resolution, or piece-alignment helpers. |
