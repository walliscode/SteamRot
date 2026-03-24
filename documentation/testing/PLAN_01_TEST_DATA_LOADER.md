# Plan 01 — FlatBuffers Test Data Loader

## Component identity

**Files:**
- `tests/harness/FlatbuffersTestDataLoader.h` / `.cpp`
- `tests/harness/FlatbuffersTestDataProvider.h` / `.cpp`
- `tests/harness/ITestDataProvider.h`

**Responsibility:** Discover `.test_data.bin` files on disk, parse them using
the generated FlatBuffers accessors, and return strongly-typed objects to
callers.

---

## Current state

Both files exist and are unit-tested in
`tests/unit/harness/FlatbuffersTestDataLoader.test.cpp` and
`FlatbuffersTestDataProvider.test.cpp`.

`FlatbuffersTestDataLoader` owns raw binary data buffers in a
`std::vector<LoadedTestData>` and returns `const TestDataFbs *` pointers into
those buffers.

`FlatbuffersTestDataProvider` wraps the loader and converts the raw FlatBuffers
objects into native C++ `TestData` structs (which contain `SimulationData`,
`EngineSnapshot`, and metadata).

The loader is called by `harness_runner::RunHarnessTests()` and from several
unit tests. It is essentially complete for the existing `TestData` struct shape.

### What works today

- Recursive file discovery of `*.test_data.bin` files.
- Conversion of `meta_data`, `simulation_data.steps`, `num_ticks`,
  `starting_engine_snapshot`, and `expected_engine_snapshots` into native types.
- Loading of `EventBus` state from `starting_engine_snapshot.global_event_bus`.
- Loading of `SceneCollectionData` (entity collections per scene).

### Gaps

1. **`tick_snapshots` field not converted.** The `TestDataFbs` schema includes
   a `tick_snapshots` vector (per-tick expected snapshots) but
   `FlatbuffersTestDataProvider` does not currently map this into `TestData`.
   `TestData::expected_engine_snapshots` (a `std::map<size_t, EngineSnapshot>`)
   is the correct destination; the provider needs an additional conversion path
   from `TickSnapshotFbs` objects.

2. **`input_sequence` field not converted.** The schema contains an
   `InputSequenceFbs` table but the provider has no conversion for it yet.
   When Plan 04 (input simulation) is implemented the provider will need to
   produce a corresponding native type.

3. **`event_sequence` field not converted.** The schema contains an
   `EventSequenceFbs` table but the provider has no conversion for it yet.
   When Plan 05 (event simulation) is implemented the provider will need to
   produce a corresponding native type.

4. **`SceneManagerData` not fully converted.** The `scene_manager_data` optional
   field in `EngineSnapshot` exists in the schema but the conversion function
   does not yet populate it.

5. **No standalone `load_test_data_configs()` free function.** The README
   describes a simple free function `load_test_data_configs()` that tests can
   call in one line. Today callers must instantiate `EventHandler`,
   `FlatbuffersTestDataProvider`, and call `ProvideAllTestData()` manually.

6. **No mechanism to load default scene data without specifying all entities.**
   When `starting_engine_snapshot.scene_collection_data` is used, all entity
   and component data must be written out in full in the test JSON. There is no
   way to say "load the title scene from its production defaults" — i.e., from
   `data/defaults/scenes/title.scene_data.bin` — without duplicating all that
   data into the test file.

   The production engine has `SceneManager::AddSceneFromDefault(SceneType)` and
   `SceneManager::LoadTitleScene()` which load directly from the binary default
   files, but `TestEngine::StartUp()` only calls
   `AddScenesFromSceneCollectionData()` and has no path to trigger the default
   loader.

---

## Plan

### Step 1 — Add `tick_snapshots` conversion

**File:** `tests/harness/FlatbuffersTestDataProvider.cpp`

- Locate the `CreateTestData()` method.
- After populating `expected_engine_snapshots` from the existing
  `expected_engine_snapshots` field, also iterate over `tick_snapshots` (if
  present in the FlatBuffers object).
- For each `TickSnapshotFbs` entry extract `tick` (the key) and call
  `ConfigureEngineSnapshot()` on the nested `snapshot` field, then insert the
  result into `test_data.expected_engine_snapshots[tick]`.
- Add a unit test in `FlatbuffersTestDataProvider.test.cpp` using a test data
  file that has `tick_snapshots` populated.

### Step 2 — Add `input_sequence` conversion stub

**Files:** `tests/harness/FlatbuffersTestDataProvider.h` / `.cpp`

- Add a `ConfigureInputSequence()` private method that accepts the raw
  `InputSequenceFbs *` pointer and returns `std::expected<InputSequenceData,
  FailInfo>`.
- Define `InputSequenceData` in `src/types/test_structs/` (a struct holding a
  `std::vector<InputEvent>` where `InputEvent` carries tick number, type, and
  position/button data).
- Wire the conversion into `CreateTestData()` and store the result in `TestData`
  (add a new optional field `input_sequence` to the `TestData` struct).
- This step is a stub: leave the execution side empty; Plan 04 implements the
  execution logic.

### Step 3 — Add `event_sequence` conversion stub

Mirror Step 2 for event sequences. Define `EventSequenceData` in
`src/types/test_structs/` (a struct holding a `std::vector<TickEvent>` where
`TickEvent` carries a tick number and an `EventPacket`). Store in `TestData` as
an optional field `event_sequence`.

### Step 4 — Implement `load_test_data_configs()` free function

**Files (new):** `tests/harness/test_data_loader.h` / `.cpp`

```cpp
// Returns all TestDataFbs configs from the adjacent data/ directory.
// Caller's __FILE__ is used to locate the directory.
std::expected<std::vector<const TestDataFbs *>, FailInfo>
load_test_data_configs();

// Loads from tests/<subdirectory>/data/
std::expected<std::vector<const TestDataFbs *>, FailInfo>
load_test_data_configs(const std::string &subdirectory);
```

The implementation creates an `EventHandler` and
`FlatbuffersTestDataLoader` internally; callers do not need to know either
class exists.

Add `test_data_loader.cpp` to the `harness` CMake target.

### Step 5 — Add `default_scenes` field to the schema and converter

This step adds the mechanism for loading scene defaults without manually
specifying all entity data in the test JSON.

**Background:** `SceneManager::AddSceneFromDefault(SceneType)` loads a scene
from its production binary file (e.g., `data/defaults/scenes/title.scene_data.bin`).
Today `TestEngine::StartUp()` only calls `AddScenesFromSceneCollectionData()`,
so there is no path through the test harness to invoke the default loader.
Manually duplicating all entity data from the production JSON into every test
file is impractical and creates maintenance overhead.

**Schema change** — add to `src/types/flatbuffers/testing/test_data.fbs`:

```fbs
table TestDataFbs
{
  meta_data: TestMetadataFbs (required);
  simulation_data: SimulationDataFbs;
  num_ticks: uint32 = 1;
  starting_engine_snapshot: EngineSnapshotFbs;
  expected_engine_snapshots: [TickSnapshotPairFbs];

  // NEW: scene types to load from production defaults.
  // Each entry triggers SceneManager::AddSceneFromDefault() in TestEngine.
  // Use this instead of starting_engine_snapshot.scene_collection_data
  // when you want the real production scene data without re-specifying it.
  default_scenes: [SceneTypeFbs];
}
```

**TestData struct change** — add to `src/types/test_structs/TestData.h`:

```cpp
/// Scene types to load from production defaults during TestEngine::StartUp().
/// Populated from the default_scenes FlatBuffers field.
std::vector<SceneType> default_scenes{};
```

**Converter change** — in `FlatbuffersTestDataProvider::CreateTestData()`:

```cpp
// Convert default_scenes (if present)
if (fbs_test_data->default_scenes()) {
  for (const auto scene_type_fbs : *fbs_test_data->default_scenes()) {
    test_data.default_scenes.push_back(
        ConvertSceneType(scene_type_fbs));  // existing enum conversion
  }
}
```

**Usage in test JSON** — with this field, a title-scene integration test
becomes:

```json
{
  "meta_data": {
    "test_name": "title_scene_smoke",
    "test_description": "One tick of the real title scene",
    "will_pass": true
  },
  "default_scenes": ["TITLE"],
  "num_ticks": 1
}
```

No `starting_engine_snapshot` needed. The engine loads the complete entity and
UI hierarchy from `data/defaults/scenes/title.scene_data.bin`.

**Both fields can coexist.** If a test specifies both `default_scenes` and
`starting_engine_snapshot.scene_collection_data`, the engine loads the defaults
first and then applies the explicit collection on top. This allows tests to
start from a production-like baseline and then tweak individual entities.

### Step 6 — Add unit tests for new conversions

Extend `FlatbuffersTestDataProvider.test.cpp` to cover:
- `tick_snapshots` loaded into `expected_engine_snapshots`.
- `input_sequence` present in converted `TestData`.
- `event_sequence` present in converted `TestData`.
- `default_scenes` containing `SceneType::TITLE` when `"default_scenes": ["TITLE"]`
  is present in the JSON.

Add a unit test file `test_data_loader.test.cpp` to verify the free function
finds and returns data correctly.

---

## Acceptance criteria

- [ ] `FlatbuffersTestDataProvider::CreateTestData()` maps `tick_snapshots` into
  `TestData::expected_engine_snapshots`.
- [ ] `TestData` has an `input_sequence` optional field populated from FlatBuffers.
- [ ] `TestData` has an `event_sequence` optional field populated from FlatBuffers.
- [ ] `TestDataFbs` schema has a `default_scenes: [SceneTypeFbs]` field.
- [ ] `TestData` has a `default_scenes: std::vector<SceneType>` field populated
  from the FlatBuffers conversion.
- [ ] `load_test_data_configs()` exists and can be called with no arguments from
  a test file to retrieve configs from the adjacent `data/` directory.
- [ ] All new code has unit tests.
- [ ] No existing unit tests broken.
