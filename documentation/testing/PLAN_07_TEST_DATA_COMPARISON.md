# Plan 07 — test_data_comparison

## Component identity

**Files (to be created):**
- `tests/harness/test_data_comparison.h`
- `tests/harness/test_data_comparison.cpp`

**Responsibility:** Provide focused, reusable comparison functions that accept
actual data (from `TestEngine::GetDataBank()`) and expected data (from
`TestData::expected_engine_snapshots`), then use the matcher library to assert
equality. These functions are used by `test_harness::RunTestEngineTest()` and
can also be called directly from integration tests that need fine-grained
control over what is compared.

---

## Current state

Comparison logic currently exists inside `harness_runner.cpp`:

- `CompareEngineSnapshots(actual, expected, test_name, tick)` — uses
  `REQUIRE_THAT` with `EngineSnapshotEqualsMatcher`. This function exists and
  is used in `RunSnapshotComparisons()`.
- `RunSnapshotComparisons(test_engine, test_data)` — loops over
  `test_data.expected_engine_snapshots` and calls `CompareEngineSnapshots` for
  each expected tick.

### What works today

- `CompareEngineSnapshots()` produces detailed Catch2 output on failure.
- `RunSnapshotComparisons()` correctly iterates expected ticks and invokes the
  matcher.

### Gaps

1. **`CompareEngineSnapshots` does not support tick-snapshot comparison
   separately from entity-collection comparison.** The README describes two
   distinct comparison surfaces: `tick_snapshots` (entity state per tick) and
   `expected_entity_collection` (final entity state). Currently these are both
   routed through `EngineSnapshotEqualsMatcher`, which is broad. A narrower
   function `CompareTickSnapshotEntityPool` is described but not implemented.

2. **`CompareDataBankWithTickSnapshot` is not implemented.** This function (from
   the README) should compare a data-bank entry (keyed by tick number) with the
   corresponding entry in `TestData::expected_engine_snapshots`, extracting only
   the entity pool from each.

3. **No `TestContext` passed through to matchers from `RunSnapshotComparisons`.**
   The `EngineSnapshotEqualsMatcher` accepts an optional `TestContext` for
   richer error output (test name, tick number). Today `RunSnapshotComparisons`
   passes a plain string; it should construct and pass a full `TestContext`.

4. **No comparison for `EventBus` state at tick snapshots.** `EngineSnapshot`
   has an optional `global_event_bus` field. If a test specifies expected event
   bus state at a given tick, the comparison should include it.

5. **No helper to compare `SceneCollectionData` across snapshots.** Integration
   tests that care only about scene entity state (not the full `EngineSnapshot`)
   have no focused comparison API.

---

## Plan

### Step 1 — Extract `test_data_comparison.h` / `.cpp` from `harness_runner`

**Files (new):** `tests/harness/test_data_comparison.h` / `.cpp`

Move `CompareEngineSnapshots()` and `RunSnapshotComparisons()` from
`harness_runner.cpp` into the new files.

Update `harness_runner.cpp` to `#include "test_data_comparison.h"` and remove
the moved implementations.

This is a refactor only — no behaviour change. All existing tests should still
pass.

### Step 2 — Implement `CompareTickSnapshotEntityPool`

```cpp
// Compare the entity pool from a single data-bank entry against
// the entity pool from an expected snapshot.
// Uses REQUIRE_THAT internally.
void CompareTickSnapshotEntityPool(
    const EngineSnapshot &actual_snapshot,
    const EngineSnapshot &expected_snapshot,
    const TestContext &context);
```

**Implementation notes:**

- Extract the `SceneCollectionData` from each snapshot.
- For each scene (matched by UUID or scene type), extract the
  `EntityMemoryPool` from the `entity_transport` variant.
- Call `REQUIRE_THAT(actual_pool, EqualsEntityMemoryPool(expected_pool, context))`.
- If the variant holds the wrong type, `FAIL()` with a descriptive message.

### Step 3 — Implement `CompareDataBankWithTickSnapshot`

```cpp
// Compare a data bank entry (actual) against the corresponding expected
// snapshot from TestData, using the TestContext for error output.
void CompareDataBankWithTickSnapshot(
    const std::map<size_t, EngineSnapshot> &data_bank,
    const std::map<size_t, EngineSnapshot> &expected_snapshots,
    size_t tick,
    const TestContext &context);
```

**Implementation notes:**

- Look up `data_bank.at(tick)` (fail if missing with a descriptive FAIL message).
- Look up `expected_snapshots.at(tick)` (fail if missing).
- Call `CompareTickSnapshotEntityPool` for entity state (if present in expected).
- Call an event-bus comparison if `expected_snapshot.global_event_bus` has a
  value.

### Step 4 — Add `TestContext` construction in `RunSnapshotComparisons`

**File:** `tests/harness/test_data_comparison.cpp`

Update `RunSnapshotComparisons()`:

```cpp
for (const auto &[tick, expected] : test_data.expected_engine_snapshots) {
  TestContext context;
  context.test_name = test_data.meta_data.test_name;
  context.description = test_data.meta_data.test_description;
  context.current_tick = static_cast<uint32_t>(tick);
  context.total_ticks = static_cast<uint32_t>(test_data.number_of_ticks);
  CompareEngineSnapshots(actual, expected, context);
}
```

### Step 5 — Add `EventBus` comparison in snapshot loop

In `RunSnapshotComparisons()`, after entity comparison, if the expected snapshot
has `global_event_bus`, call:

```cpp
if (expected.global_event_bus.has_value()) {
  const auto &actual_bus = /* extract from data bank entry */;
  REQUIRE_THAT(actual_bus,
      EqualsEventBus(expected.global_event_bus.value(), context));
}
```

### Step 6 — Add unit tests

**File (new):** `tests/unit/harness/test_data_comparison.test.cpp`

Test cases to include:

1. `CompareTickSnapshotEntityPool` passes when actual matches expected.
2. `CompareTickSnapshotEntityPool` fires a Catch2 assertion when pools differ.
3. `CompareDataBankWithTickSnapshot` fails with a descriptive message when tick
   is not in the data bank.
4. `RunSnapshotComparisons` succeeds when all expected ticks match actual data.
5. `RunSnapshotComparisons` includes `TestContext` in failure output (verify the
   context fields are set).

Add to `tests/unit/harness/CMakeLists.txt`:
```cmake
test_data_comparison.test.cpp
```

### Step 7 — Link to `harness` CMake target

**File:** `tests/harness/CMakeLists.txt`

Add `test_data_comparison.cpp` to the `harness` library source list.

---

## Acceptance criteria

- [ ] `CompareEngineSnapshots()` and `RunSnapshotComparisons()` live in
  `test_data_comparison.h` / `.cpp`.
- [ ] `CompareTickSnapshotEntityPool()` exists and compares entity pools from
  snapshots using the matcher library.
- [ ] `CompareDataBankWithTickSnapshot()` exists and looks up the correct
  data-bank entry for a given tick.
- [ ] `TestContext` is populated with test name, description, tick, and total
  ticks in `RunSnapshotComparisons()`.
- [ ] Expected `EventBus` state is compared when present in an expected snapshot.
- [ ] All unit tests pass.
- [ ] No existing unit tests broken.
