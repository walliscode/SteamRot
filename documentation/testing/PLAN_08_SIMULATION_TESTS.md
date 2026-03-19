# Plan 08 — simulation_tests Activation

## Component identity

**Files:**
- `tests/simulation_tests/all_simulations.test.cpp`
- `tests/simulation_tests/CMakeLists.txt`

**Responsibility:** Provide a Catch2 test executable that discovers all
`*.test_data.bin` files adjacent to it, runs each one through `TestEngine`,
and produces a pass or fail result per file. This is the closest analogue to a
traditional integration test runner: a single executable that exercises the full
pipeline without any hand-written test cases.

---

## Current state

`all_simulations.test.cpp` exists but its body is entirely commented out:

```cpp
TEST_CASE("Run all simulation tests", "[simulation][integration]") {

  // std::filesystem::path test_data_path{__FILE__};
  // auto simulation_result =
  //     steamrot::tests::RunHarnessTests(test_data_path.parent_path());
  // if (!simulation_result) {
  //   FAIL(simulation_result.error().message);
  // }
}
```

The test currently does nothing and always passes (empty test body). The
`simulation_tests` executable is linked to the `harness` library and is
included in `tests/CMakeLists.txt` as an active `add_subdirectory`, so the
executable is compiled and included in `ctest` runs.

The commented-out code calls `RunHarnessTests()`, which expects a `data/`
subdirectory adjacent to the caller. That directory does not currently exist at
`tests/simulation_tests/data/`.

### Dependency chain

This activation depends on the following plans being complete:

- **Plan 01** (data loader) — `RunHarnessTests` must be able to load test data.
- **Plan 03** (simulation runner) — All Logic classes used in test data must
  dispatch correctly.
- **Plan 02** (test engine) — `TestEngine` must handle per-tick input/event
  injection.
- **Plan 07** (data comparison) — Snapshot comparisons must be reliable.

It is acceptable to activate this test with the body using the simpler new API
from Plan 06 (`RunTestEngineTest`) rather than the original `RunHarnessTests`
pattern.

---

## Plan

### Step 1 — Create the `data/` directory and a smoke-test data file

**Directory (new):** `tests/simulation_tests/data/`

Create an initial smoke-test file that exercises the full pipeline with the
simplest possible test:

**File (new):** `tests/simulation_tests/data/smoke_test_001.test_data.json`

```json
{
  "meta_data": {
    "test_name": "smoke_test_title_scene_1_tick",
    "test_description": "Smoke test: one tick of the title scene with no simulation steps",
    "tags": ["integration", "smoke"],
    "will_pass": true,
    "version": 1
  },
  "num_ticks": 1,
  "starting_engine_snapshot": {
    "scene_collection_data": {
      "scene_data": [
        {
          "scene_info": { "scene_type": "TITLE" },
          "scene_resources_config": {
            "texture_width": 800,
            "texture_height": 600
          },
          "entity_collection": {
            "entity_memory_pool_size": 5,
            "entities": []
          },
          "asset_config": {}
        }
      ]
    }
  }
}
```

This file has no expected snapshots, so the test passes as long as the engine
runs without crashing.

**Note:** The JSON file must be compiled to `.bin` format by the FlatBuffers
compiler during the build. The CMakeLists must include a rule for this.

### Step 2 — Add FlatBuffers JSON-to-binary build rule

**File:** `tests/simulation_tests/CMakeLists.txt`

Add a `flatbuffers_generate_binary_from_json` call (or equivalent custom target)
to compile `smoke_test_001.test_data.json` to
`smoke_test_001.test_data.bin` in the build output directory.

Examine how other test directories (e.g., `tests/unit/harness/`) handle this
to use the same CMake pattern.

### Step 3 — Uncomment and update `all_simulations.test.cpp`

**File:** `tests/simulation_tests/all_simulations.test.cpp`

Replace the commented-out body with the modern API from Plan 06:

```cpp
#include "test_harness.h"  // Plan 06
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

TEST_CASE("Run all simulation tests", "[simulation][integration]") {

  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  const auto *config = GENERATE_COPY(from_range(configs.value()));

  auto result = steamrot::tests::RunTestEngineTest(config);
  REQUIRE(result.has_value());
}
```

If `test_harness.h` (Plan 06) is not yet available, use the lower-level
`RunHarnessTests` pattern as a transitional step:

```cpp
TEST_CASE("Run all simulation tests", "[simulation][integration]") {
  std::filesystem::path test_data_path{__FILE__};
  auto simulation_result =
      steamrot::tests::RunHarnessTests(test_data_path.parent_path());
  if (!simulation_result) {
    FAIL(simulation_result.error().message);
  }
}
```

### Step 4 — Verify the executable in `ctest`

Confirm `simulation_tests` appears in `ctest --preset Debug --test-dir build`
output and that the smoke-test data file produces a green result.

Label the test with `[integration]` so it can be run selectively:
```cmake
catch_discover_tests(simulation_tests PROPERTIES LABELS "integration")
```

---

## Acceptance criteria

- [ ] `tests/simulation_tests/data/` directory exists.
- [ ] At least one `*.test_data.bin` file is present in that directory after
  building.
- [ ] The `TEST_CASE` body in `all_simulations.test.cpp` is no longer
  commented out.
- [ ] The test executable produces at least one Catch2 test-case result
  (pass or fail with a meaningful message — not an empty body).
- [ ] The smoke-test passes end-to-end with the full `TestEngine` pipeline.
- [ ] `ctest --preset Debug -L integration` includes this test.
