# Plan 09 — Integration Test Suite (`tests/integration/`)

## Component identity

**Files:**
- `tests/integration/CMakeLists.txt` (exists but empty)
- `tests/integration/*.integration.test.cpp` (none exist yet)

**Responsibility:** Contain purpose-written Catch2 integration tests that
exercise multiple game systems together. Unlike `simulation_tests/` (which
runs whatever `.bin` files it finds) these tests are written in C++ and can
express complex assertions, parametric scenarios, and preconditions that are
awkward to express purely in JSON.

---

## Current state

`tests/integration/` exists and contains only a two-line `CMakeLists.txt`.
The directory is commented out in `tests/CMakeLists.txt`:

```cmake
# add_subdirectory(integration)
```

No integration test files have been written. No CMake target is built from this
directory.

---

## Plan

### Step 1 — Enable the subdirectory in `tests/CMakeLists.txt`

**File:** `tests/CMakeLists.txt`

Uncomment:
```cmake
add_subdirectory(integration)
```

This should only be done once at least one integration test exists in the
directory, otherwise CMake will warn about an empty target.

### Step 2 — Set up `tests/integration/CMakeLists.txt`

**File:** `tests/integration/CMakeLists.txt`

Replace the placeholder content with:

```cmake
add_executable(integration_tests
  # Test files added here as they are created
)

target_link_libraries(integration_tests
  PRIVATE
  Catch2::Catch2WithMain
  harness
  matchers
  context_test_helpers
)

catch_discover_tests(integration_tests PROPERTIES LABELS "integration")
```

### Step 3 — Write the first integration test

**Scope of the first test:**

The first integration test should be the simplest possible multi-system
scenario that provides genuine confidence in the pipeline. A good candidate is:

> *A `CUserInterface` component is visible; `UICollisionLogic` runs with the
> mouse positioned over the element; `UIStateLogic` then runs; the component is
> checked to be in the hover/active state afterwards.*

This exercises `EntityMemoryPool`, `ArchetypeManager`, `UICollisionLogic`,
`UIStateLogic`, `SceneContext`, and the event system all together.

**File (new):** `tests/integration/ui_hover.integration.test.cpp`

```cpp
#include "TestFixture.h"
#include "UICollisionLogic.h"
#include "UIStateLogic.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UI element enters hover state when mouse moves over it",
          "[integration][ui]") {

  // Arrange: create a scene with one visible UI element
  // Use TestFixture to set up scene context
  // Position the element at (100, 100) with size (50, 30)
  // Set mouse position inside the element bounds

  // Act: run UICollisionLogic then UIStateLogic

  // Assert: the UI element's hover/active flag is set
}
```

Flesh out the body once the component setup utilities (from the test helpers
and context libraries) are confirmed to support it.

### Step 4 — Write integration tests for each scene type

Once the first test is green, add one integration test per scene type to verify
the full logic pipeline:

| File | Scenario |
|------|----------|
| `title_scene_render.integration.test.cpp` | Title scene renders without crash over N ticks |
| `crafting_scene_render.integration.test.cpp` | Crafting scene renders without crash over N ticks |
| `ui_click_action.integration.test.cpp` | Button click generates expected action event |
| `event_lifetime.integration.test.cpp` | Event expires correctly over N ticks |

### Step 5 — Consider data-driven integration tests

Some integration scenarios are better expressed as JSON test data (e.g., a
sequence of mouse moves and clicks leading to a specific entity state). These
can live in `tests/integration/data/` and be loaded via `load_test_data_configs()`
from an integration test executable.

**File (new, optional):** `tests/integration/data_driven.integration.test.cpp`

```cpp
#include "test_harness.h"
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Data-driven integration scenarios", "[integration][data-driven]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  const auto *config = GENERATE_COPY(from_range(configs.value()));
  auto result = steamrot::tests::RunTestEngineTest(config);
  REQUIRE(result.has_value());
}
```

This allows new integration scenarios to be added purely by dropping a new
`.test_data.json` file in `tests/integration/data/` without writing any C++.

### Step 6 — Label integration tests correctly

All integration test cases must carry the `[integration]` tag so they can be
run selectively:

```cpp
TEST_CASE("my scenario", "[integration][ui]") { ... }
```

This allows:
```bash
ctest --preset Debug -L integration   # run only integration tests
ctest --preset Debug -L unit          # run only unit tests
```

---

## Acceptance criteria

- [ ] `add_subdirectory(integration)` is uncommented in `tests/CMakeLists.txt`.
- [ ] `tests/integration/CMakeLists.txt` defines an `integration_tests`
  executable.
- [ ] At least one `.integration.test.cpp` file exists and contains a
  meaningful (non-trivial) integration test.
- [ ] All integration tests carry the `[integration]` Catch2 tag.
- [ ] `ctest --preset Debug -L integration` runs the integration tests.
- [ ] The initial integration test passes.
