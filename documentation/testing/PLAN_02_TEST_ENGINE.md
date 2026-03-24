# Plan 02 — TestEngine

## Component identity

**Files:**
- `tests/harness/TestEngine.h` / `.cpp`

**Responsibility:** Drive a controlled number of engine ticks for testing.
Extends the base `Engine` class, overrides the tick pipeline to use
`SimulationRunner` instead of `SceneManager`'s normal update, suppresses
rendering to screen, captures `EngineSnapshot` objects into a data bank after
each tick, and exposes the data bank for post-run comparison.

---

## Current state

`TestEngine` exists and is unit-tested in `tests/unit/harness/TestEngine.test.cpp`.

`StartUp()` calls the base `Engine::StartUp()`, applies tick count from
`TestData`, pre-loads the starting `EventBus` state, and passes
`SceneCollectionData` to `SceneManager::AddScenesFromSceneCollectionData()`.

`RunGameLoop()` loops from 1 to `m_target_ticks`, calling `ExecuteTick()` and
`StoreEngineSnapShot()` each iteration.

`TickSceneLogic()` creates a `SimulationRunner` for every active scene and calls
`ExecuteSimulation()`.

`StoreEngineSnapShot()` captures an `EngineSnapshot` and stores it in
`m_data_bank`, keyed by the current tick number (tick 0 for the pre-run
snapshot).

### What works today

- Engine initialises from `TestData`.
- Correct number of ticks executes.
- `EngineSnapshot` captured after each tick.
- Data bank accessible via `GetDataBank()`.
- `EventBus` pre-loaded before first tick.

### Gaps

1. **No per-tick input injection.** `RunGameLoop()` does not call any
   input-simulation function. When Plan 04 is ready, `RunGameLoop()` needs to
   invoke `execute_input_events_for_tick()` at the start of each tick.

2. **No per-tick event injection.** `RunGameLoop()` does not call any
   event-simulation function. When Plan 05 is ready, `RunGameLoop()` needs to
   invoke `execute_events_for_tick()` and then
   `event_handler.ProcessWaitingRoomEventBus()` at the start of each tick.

3. **`TickSceneLogic()` silently ignores simulation errors.** The comment
   `// [TODO:] handle simulation failure` shows this is a known gap. A
   simulation failure should propagate as a `FailInfo` and be surfaced as a
   test failure, not swallowed.

4. **`RunGameLoop()` has no error return.** It is `void`. If `StoreEngineSnapShot()`
   fails or the simulation fails, the failure is discarded. It should instead
   set an error flag that `RunGame()` can return.

5. **Tick-0 snapshot always stores at key 0 unconditionally.** If `StartUp()`
   is called multiple times, or if the data bank is not empty, the key-0 entry
   is overwritten silently. The pre-run snapshot path should check explicitly
   whether a tick-0 entry already exists.

6. **`SceneManagerData` not restored from `TestData`.** The `EngineSnapshot`
   has an optional `scene_manager_data` field. If test data provides this,
   `TestEngine::StartUp()` should restore it via
   `SceneManager::AddScenesFromSceneCollectionData()` (or an equivalent
   restore path) rather than ignoring it.

### Gap 7 — No way to load scene defaults from test data

`TestEngine::StartUp()` always routes scene setup through
`AddScenesFromSceneCollectionData()`. The production path
`SceneManager::AddSceneFromDefault(SceneType)` — which reads the real binary
default files from `data/defaults/scenes/` — is never called during tests.

Writing out all entity and UI hierarchy data manually in every test JSON is
impractical. A test that just wants to start from the real title scene should
be able to say `"default_scenes": ["TITLE"]` and have the engine load it
automatically from `data/defaults/scenes/title.scene_data.bin`.

---

## Plan

### Step 1 — Propagate simulation errors from `TickSceneLogic()`

**File:** `tests/harness/TestEngine.h` / `.cpp`

- Change `TickSceneLogic()` to a non-void signature internally (store last
  error in a member `std::optional<FailInfo> m_tick_error`).
- After each call to `runner.ExecuteSimulation()`, if the result has no value,
  write to `m_tick_error` and return early.
- In `RunGameLoop()`, check `m_tick_error` after each `ExecuteTick()` call. If
  set, stop the loop.
- Expose `GetTickError()` returning `const std::optional<FailInfo> &` for
  inspection in tests.

### Step 2 — Wire per-tick input injection

**File:** `tests/harness/TestEngine.cpp`

This step depends on Plan 04 (`input_simulation`) being implemented first.

- Add `#include "input_simulation.h"`.
- In `RunGameLoop()`, before calling `ExecuteTick()` for tick `i`, call:
  ```cpp
  if (m_test_data.input_sequence.has_value()) {
    execute_input_events_for_tick(
        m_test_data.input_sequence.value(), i,
        m_engine_resources.event_handler,
        m_engine_resources.mouse_position);
  }
  ```
- No behaviour change when `input_sequence` is absent.

### Step 3 — Wire per-tick event injection

**File:** `tests/harness/TestEngine.cpp`

This step depends on Plan 05 (`event_simulation`) being implemented first.

- In `RunGameLoop()`, after input injection but before `ExecuteTick()`, call:
  ```cpp
  if (m_test_data.event_sequence.has_value()) {
    execute_events_for_tick(
        m_test_data.event_sequence.value(), i,
        m_engine_resources.event_handler);
    m_engine_resources.event_handler.ProcessWaitingRoomEventBus();
  }
  ```

### Step 4 — Restore `SceneManagerData` from snapshot

**File:** `tests/harness/TestEngine.cpp`

- In `StartUp()`, after the existing `AddScenesFromSceneCollectionData()` call,
  check whether `m_test_data.starting_engine_snapshot.scene_manager_data` has a
  value and, if so, apply it via the appropriate `SceneManager` restoration
  method.

### Step 5 — Load default scenes from `TestData::default_scenes`

**File:** `tests/harness/TestEngine.cpp`

This step depends on Plan 01 (Step 5) which adds `TestData::default_scenes`.

In `StartUp()`, after the `AddScenesFromSceneCollectionData()` call, add:

```cpp
// Load any scenes specified as defaults (from production binary data files).
// This lets tests start from real scene data without duplicating it in JSON.
for (const auto &scene_type : m_test_data.default_scenes) {
  auto add_default_result =
      m_scene_manager.AddSceneFromDefault(scene_type);
  if (!add_default_result.has_value()) {
    return std::unexpected(add_default_result.error());
  }
}
```

**What `AddSceneFromDefault` does internally:**
1. `SceneFactory::CreateSceneFromDefault(scene_type)`
2. `FlatbuffersSceneDataProvider::CreateSceneData(scene_type)`
3. `FlatbuffersDataLoader::ProvideDefaultSceneData(scene_type)`
4. Reads `data/defaults/scenes/<type>.scene_data.bin` from disk
5. Deserialises FlatBuffers and configures a full `EntityMemoryPool`

**Note on ordering:** `AddScenesFromSceneCollectionData()` runs first so that
any manually specified entities are present before default scenes are appended.
For most tests that use only `default_scenes` the collection data will be empty
(its default) and the call is a no-op.

### Step 6 — Update unit tests

**File:** `tests/unit/harness/TestEngine.test.cpp`

- Add a test `TestEngine::StartUp loads default scene when default_scenes
  contains SceneType::TEST` (use the test scene to avoid font/asset
  dependencies during CI).
- Verify `m_scene_manager.GetScenes()` has exactly one scene after `StartUp()`
  when only `default_scenes` is provided.
- Verify `GetDataBank()` at tick 0 contains scene collection data from the
  loaded scene.

---

## Acceptance criteria

- [ ] A simulation failure inside `TickSceneLogic()` is stored in `m_tick_error`
  and halts the game loop.
- [ ] `TestEngine` calls `execute_input_events_for_tick()` at the start of each
  tick when `input_sequence` is present.
- [ ] `TestEngine` calls `execute_events_for_tick()` and
  `ProcessWaitingRoomEventBus()` at the start of each tick when
  `event_sequence` is present.
- [ ] `SceneManagerData` from the starting snapshot is applied during `StartUp()`
  if present.
- [ ] `TestEngine::StartUp()` calls `SceneManager::AddSceneFromDefault()` for
  each entry in `TestData::default_scenes`.
- [ ] All new behaviour has unit test coverage.
- [ ] No existing unit tests broken.
