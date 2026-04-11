# Logic Decomposition: Keeping `ProcessLogic` Clean

## Overview

`Logic::ProcessLogic()` is the override point for every Logic class. Without care, it
accumulates subscriber iteration, archetype loops, asset fetches, and conditional
dispatch in one function, becoming hard to read and impossible to test in isolation.
This document establishes the rules for splitting `ProcessLogic` into named sections
and for placing that code so that it is independently unit-testable.

---

## The Three Section Types

Every body of code that ends up in `ProcessLogic` belongs to one of three categories.
Recognising the category determines where the wrapper code lives and what parameters
it needs.

### 1. Event Sections

**What they are**: Iterating `m_subscribers`, checking `m_active`, and dispatching
based on `EventType` and the concrete payload type (e.g. `GhostPayload`,
`LogicPayload`).

**Example** (current `GhostActionLogic::ProcessLogic`):

```cpp
for (auto &subscriber : m_subscribers) {
  if (!subscriber->m_active)
    continue;
  // dispatch based on captured_payload ...
}
```

**Where the wrapper lives**: In the existing free-function action module for the
relevant domain (`action_ghost.h/cpp`, `action_grimoire_machina.h/cpp`, etc.) inside
its namespace.

**What to pass**: The `Subscriber` by reference, plus only the concrete data the
function needs to mutate. **Do not pass `SceneContext`** — extract the specific field
at the call site.

```cpp
// action_ghost.h
namespace steamrot::logic::action::ghost {
  void ProcessGhostSubscriber(Subscriber &subscriber, MrGhost &mr_ghost);
}
```

```cpp
// GhostActionLogic.cpp – ProcessLogic section
for (auto &subscriber : m_subscribers) {
  if (!subscriber->m_active)
    continue;
  action::ghost::ProcessGhostSubscriber(*subscriber, m_scene_context.mr_ghost);
}
```

### 2. Archetype Sections

**What they are**: Fetching entity indices via `archetypes::`, iterating them, and
reading/mutating components.

**Where the wrapper lives**: In the free-function module that matches the domain
(`action_ui.h/cpp`, `positioning_ui.h/cpp`, etc.) or a new one if no suitable module
exists yet.

**What to pass**: The specific components or ranges required, or the archetype map and
entity pool if a full range is needed. Avoid passing the full `SceneContext` unless
every field of it is genuinely required.

```cpp
// Preferred – pass only what is needed
void ProcessDropDownContainerElementActions(
    DropDownContainerElement &element,
    const SceneContext &scene_context); // SceneContext acceptable here because
                                        // the function needs several fields
```

### 3. Asset Sections

**What they are**: Fetching a shared asset (e.g. `GrimoireMachina`) from the
`AssetManager` and performing operations on it.

**Where the wrapper lives**: In the domain free-function module. The asset is
retrieved in `ProcessLogic` and the pointer/reference is forwarded.

```cpp
// GrimoireMachinaActionLogic.cpp – ProcessLogic
auto result = m_scene_context.asset_manager.GetGrimoireMachina();
if (!result.has_value()) return;
GrimoireMachina &gm = *result.value();

// All actual work is in a free function:
action::grimoire_machina::ProcessLogicSubscribers(m_subscribers, gm);
```

---

## Naming and Namespace Strategy

The concern about `ProcessEventTypeX` colliding across Logic classes is solved by the
existing namespace hierarchy. Each domain module already owns a distinct namespace:

| Module file                       | Namespace                                      |
|-----------------------------------|------------------------------------------------|
| `action_ghost.h/cpp`              | `steamrot::logic::action::ghost`               |
| `action_grimoire_machina.h/cpp`   | `steamrot::logic::action::grimoire_machina`    |
| `action_ui.h/cpp`                 | `steamrot::logic::action::ui`                  |
| `render_ui.h/cpp`                 | `steamrot::logic::render::ui`                  |
| `positioning_ui.h/cpp`            | `steamrot::logic::positioning::ui`             |

Because the wrapper functions live in their own namespace, the same short name (e.g.
`ProcessSubscriber`) is unambiguous at the call site:

```cpp
// Unambiguous even if both exist
action::ghost::ProcessSubscriber(*sub, m_scene_context.mr_ghost);
action::grimoire_machina::ProcessSubscriber(*sub, gm);
```

This pattern should be preferred over names like `ProcessGhostEvents` that embed the
domain in the function name — the namespace already carries that information.

---

## `ProcessLogic` as an Orchestrator

After decomposition, `ProcessLogic` should read like a table of contents, not an
implementation:

```cpp
void GhostActionLogic::ProcessLogic() {

  // --- Event section ---
  for (auto &subscriber : m_subscribers) {
    if (!subscriber->m_active)
      continue;
    action::ghost::ProcessSubscriber(*subscriber, m_scene_context.mr_ghost);
  }
}
```

If a function body ever needs a comment block to explain *what* it is doing rather
than just *why*, that code is a candidate for extraction into a named free function.

---

## Free Functions vs. Member Functions

### Prefer free functions

Free functions in a domain namespace:
- Have no access to `m_scene_context` or `m_subscribers`, so they cannot develop
  hidden dependencies on them.
- Can be called from a unit test with a minimal set of constructed objects.
- Are visible in the header for the module, making the module's contract explicit.

### When member functions are acceptable

A thin **private** member function is acceptable when:
- It exists solely to call free functions in a fixed order (i.e. it is still an
  orchestrator, not an implementer).
- It genuinely needs two or more fields from `m_scene_context` that would be
  awkward to pass individually and where passing the whole context is warranted.

`ProcessGhostEvents(Subscriber &)` as it currently stands is a member function that
accesses `m_scene_context.mr_ghost` directly. Extracting `mr_ghost` at the call site
and making the function a free function would make it testable without a full
`SceneContext`:

```cpp
// Before (member, depends on m_scene_context)
void GhostActionLogic::ProcessGhostEvents(Subscriber &subscriber);

// After (free function, independently testable)
// action_ghost.h
void ProcessSubscriber(Subscriber &subscriber, MrGhost &mr_ghost);
```

---

## Guide: Adding a New Wrapper Function

Follow these steps each time `ProcessLogic` grows beyond a simple loop or requires a
new logical grouping.

### Step 1 – Identify the section type

Is this an **event section** (subscriber iteration), an **archetype section** (entity
loop), or an **asset section** (asset fetch + operate)?

### Step 2 – Locate or create the free-function module

Check whether a matching module file already exists in `src/logic/`:

- Event logic for ghost actions → `action_ghost.h/cpp`
- Event logic for grimoire machina → `action_grimoire_machina.h/cpp`
- Archetype/render logic for UI → `render_ui.h/cpp`

If no module exists, create `<domain>_<category>.h/cpp` following the snake_case file
naming convention and add it to `src/logic/CMakeLists.txt`.

### Step 3 – Determine the minimal parameter list

Start with no parameters and add only what the function body actually reads or
mutates. Prefer specific types (`MrGhost &`, `Subscriber &`) over `SceneContext &`
unless the function genuinely uses three or more of its fields.

### Step 4 – Declare in the header, implement in the `.cpp`

Place the function in the appropriate `steamrot::logic::<category>::<domain>`
namespace. Add a Doxygen `@brief` and document each parameter.

```cpp
////////////////////////////////////////////////////////////
/// @brief Dispatch a single GHOST subscriber to the correct action function.
///
/// Checks that the subscriber carries a GhostPayload, then switches on
/// GhostPayload::GhostAction to call SelectGhostItem or ClearGhostSelection.
///
/// @param subscriber Active subscriber whose captured_payload holds the action.
/// @param mr_ghost   MrGhost instance to mutate.
////////////////////////////////////////////////////////////
void ProcessSubscriber(Subscriber &subscriber, MrGhost &mr_ghost);
```

### Step 5 – Call from ProcessLogic

Replace the inline code in `ProcessLogic` with a call to the new function. Extract
any `SceneContext` fields at the call site before passing them in:

```cpp
void GhostActionLogic::ProcessLogic() {
  for (auto &subscriber : m_subscribers) {
    if (!subscriber->m_active)
      continue;
    action::ghost::ProcessSubscriber(*subscriber, m_scene_context.mr_ghost);
  }
}
```

### Step 6 – Write a unit test

Because the wrapper takes explicit parameters, it can be tested without a
`SceneContext`. Construct only what the function touches:

```cpp
TEST_CASE("ProcessSubscriber – SELECT updates MrGhost selection",
          "[unit][action_ghost]") {
  steamrot::MrGhost mr_ghost;

  steamrot::Subscriber sub;
  sub.event_type = steamrot::EventType::GHOST;
  sub.captured_payload = steamrot::GhostPayload{
      steamrot::GhostPayload::GhostAction::SELECT,
      steamrot::FragmentTag{"fragment_a"}};

  steamrot::logic::action::ghost::ProcessSubscriber(sub, mr_ghost);

  REQUIRE(std::holds_alternative<steamrot::FragmentTag>(mr_ghost.m_selection));
  REQUIRE(std::get<steamrot::FragmentTag>(mr_ghost.m_selection).name ==
          "fragment_a");
}
```

---

## Summary

| Concern                                 | Rule                                                                                    |
|-----------------------------------------|-----------------------------------------------------------------------------------------|
| ProcessLogic readability                | Orchestrates only; all work is in named free functions                                  |
| Free function vs. member function       | Prefer free functions; member functions only for orchestration that needs two or more context fields |
| Naming ambiguity across Logic classes   | Use namespace to carry domain; short, reused names (e.g. `ProcessSubscriber`) are fine |
| Parameter discipline                    | Pass the minimum set of concrete types; extract from `SceneContext` at the call site    |
| Testability                             | Every non-trivial free function should have a unit test that does not need `SceneContext` |
