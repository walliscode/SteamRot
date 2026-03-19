# Plan 06 — test_harness (Unified Orchestration API)

## Component identity

**Files (to be created):**
- `tests/harness/test_harness.h`
- `tests/harness/test_harness.cpp`

**Responsibility:** Provide the single entry-point API that a test author calls.
Wraps `TestEngine` creation, startup, game loop execution, and snapshot
comparison into one or two simple functions. Hides all internal plumbing from
test files.

---

## Current state

This component does not exist as a file. The closest existing code is
`harness_runner.h` / `.cpp`, which contains:

- `RunHarnessTests()` — discovers test data from a directory, creates
  `TestEngine` instances, and runs comparisons.
- `CompareEngineSnapshots()` — uses `REQUIRE_THAT` with the
  `EngineSnapshotEqualsMatcher` to compare actual vs. expected snapshots.
- `RunSnapshotComparisons()` — loops over expected snapshots in `TestData`
  and calls `CompareEngineSnapshots()` for each tick.
- `ConvertEntityTransportVariant()` and helpers — type-conversion utilities.

The gap is that `harness_runner.h` is oriented around `TestData` (the native C++
struct) and the old `RunHarnessTests()` pattern, which requires an
`EventHandler` and `FlatbuffersTestDataProvider` to be set up by the caller.
There is no function that accepts a raw `TestDataFbs *` pointer (the FlatBuffers
accessor returned by `load_test_data_configs()`), creates a `TestEngine`,
runs it, and compares results — all in one call.

---

## Plan

### Step 1 — Implement `RunTestEngineTest(config)`

This is the primary function described in the README. It takes a
`const TestDataFbs *` pointer (the FlatBuffers accessor), converts it to a
`TestData` struct via `FlatbuffersTestDataProvider`, creates a `TestEngine`,
runs it, and compares the data bank with expected snapshots.

**File (new):** `tests/harness/test_harness.h`

```cpp
#pragma once
#include "FailInfo.h"
#include "test_data_fbs_generated.h"  // Generated FlatBuffers header
#include <expected>

namespace steamrot::tests {

// Primary entry point for data-driven integration tests.
// Creates a TestEngine from config, runs it, and compares the data bank
// with tick_snapshots from config.
std::expected<std::monostate, FailInfo>
RunTestEngineTest(const TestDataFbs *config);

// Convenience wrapper: loads all configs from the adjacent data/ directory
// and runs RunTestEngineTest for each one.
// Intended for use in TEST_CASE bodies with Catch2 generators.
std::expected<std::monostate, FailInfo>
RunAllAdjacentHarnessTests(const std::filesystem::path &caller_path);

} // namespace steamrot::tests
```

**Implementation notes for `test_harness.cpp`:**

`RunTestEngineTest(config)`:
1. Instantiate an `EventHandler`.
2. Instantiate `FlatbuffersTestDataProvider(caller_dir, event_handler)`.
3. Call `data_provider.CreateTestData(config)` to get a native `TestData`.
4. Instantiate `TestEngine engine(test_data)`.
5. Call `engine.StartUp()`. Propagate any error.
6. Call `engine.RunGame()`. Propagate any error.
7. Call `RunSnapshotComparisons(engine, test_data)` (from `harness_runner.h`).
   Propagate any error.
8. Return `std::monostate{}`.

`RunAllAdjacentHarnessTests(caller_path)`:
- Delegates to `RunHarnessTests(caller_path)` from `harness_runner.h`, or
  optionally refactors that function's body here to use the new API.

### Step 2 — Implement `load_test_data_configs()` (if not done in Plan 01)

If Plan 01 has not yet introduced `test_data_loader.h`, provide a
minimal version here:

```cpp
// Returns all TestDataFbs configs from the caller's adjacent data/ directory.
// Uses __FILE__ via a macro to determine the directory at compile time.
std::expected<std::vector<const TestDataFbs *>, FailInfo>
load_test_data_configs_impl(const std::filesystem::path &caller_path);
```

And expose it via a macro (consistent with the README description):
```cpp
#define load_test_data_configs() \
    steamrot::tests::load_test_data_configs_impl( \
        std::filesystem::path{__FILE__}.parent_path())
```

Alternatively, use a template/lambda approach to capture `__FILE__` at the
call site.

### Step 3 — Expose `run_entity_memory_pool_comparison_test()`

For tests that want to compare `EntityMemoryPool` instances directly (without
a full `TestEngine` run):

```cpp
void run_entity_memory_pool_comparison_test(
    const EntityMemoryPool &actual,
    const EntityMemoryPool &expected,
    const std::string &test_metadata = "");
```

This wraps `REQUIRE_THAT(actual, EqualsEntityMemoryPool(expected, context))`.

### Step 4 — Expose `run_event_bus_comparison_test()`

For tests that need to compare `EventBus` state directly:

```cpp
void run_event_bus_comparison_test(
    const EventBus &actual,
    const EventBus &expected,
    const std::string &test_metadata = "",
    bool expected_to_pass = true);
```

### Step 5 — Link `test_harness.cpp` to the `harness` CMake target

**File:** `tests/harness/CMakeLists.txt`

Add `test_harness.cpp` to the source list of the `harness` library.

### Step 6 — Add unit tests

**File (new):** `tests/unit/harness/test_harness.test.cpp`

Test cases to include:

1. `RunTestEngineTest` returns success for a valid `TestDataFbs *` with no
   simulation steps and a single tick.
2. `RunTestEngineTest` returns an error when `config` is `nullptr`.
3. `RunTestEngineTest` with a mismatch between actual and expected snapshot
   triggers a Catch2 assertion failure (use `REQUIRE_THROWS` or an equivalent
   to verify the matcher fires).
4. `load_test_data_configs()` returns a non-empty vector when adjacent test
   data exists.
5. `run_entity_memory_pool_comparison_test` with two identical pools passes.

Add to `tests/unit/harness/CMakeLists.txt`:
```cmake
test_harness.test.cpp
```

---

## Acceptance criteria

- [ ] `RunTestEngineTest(config)` creates, runs, and validates a `TestEngine`
  from a `TestDataFbs *` pointer in a single call.
- [ ] `load_test_data_configs()` can be called with no arguments from a test
  file and returns configs from the adjacent `data/` directory.
- [ ] `run_entity_memory_pool_comparison_test()` and
  `run_event_bus_comparison_test()` exist and use the matcher library.
- [ ] Unit tests exist and pass for all public functions.
- [ ] No existing unit tests broken.
