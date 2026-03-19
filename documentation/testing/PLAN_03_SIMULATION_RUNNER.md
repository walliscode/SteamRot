# Plan 03 — SimulationRunner

## Component identity

**Files:**
- `tests/harness/SimulationRunner.h` / `.cpp`

**Responsibility:** Given a `SimulationData` object (an ordered list of
`SimulationStep` values) and a `SceneContext`, instantiate the correct Logic
class for each step and call `RunLogic()` on it. This is the mechanism by which
an integration test can prescribe exactly which game systems execute and in
which order within a tick.

---

## Current state

`SimulationRunner` exists and is unit-tested in
`tests/unit/harness/SimulationRunner.test.cpp`.

`ExecuteSimulation()` iterates `m_simulation_data.steps` and calls
`ExecuteStep()` for each one.

The free function `ExecuteLogicClass()` (in the `.cpp` file) contains a `switch`
on `LogicClassEnum` and instantiates the matching Logic class:

```
UIActionLogic      → logic::UIActionLogic
UICollisionLogic   → logic::UICollisionLogic
UIRenderLogic      → logic::UIRenderLogic
UIStateLogic       → logic::UIStateLogic
CraftingRenderLogic → case exists but has no matching include/class → compile error risk
None / default     → returns FailInfo{FailMode::NonExistentEnumValue, ...}
```

The `LogicClassEnum` in `SimulationData.h` currently defines:

```cpp
enum class LogicClassEnum {
  None = 0,
  UIActionLogic = 1,
  UICollisionLogic = 2,
  UIRenderLogic = 3,
  UIStateLogic = 4,
  CraftingRenderLogic = 5,
};
```

### What works today

- The four UI Logic classes dispatch and execute correctly.
- Steps execute in declaration order.
- A failure from any step is propagated out of `ExecuteSimulation()`.

### Gaps

1. **`CraftingRenderLogic` is in the enum but has no dispatch case in the switch
   statement.** The `switch` falls through to `default` (which returns an error)
   for this value. Any test that specifies `CraftingRenderLogic` as a simulation
   step will fail immediately.

2. **No free-function dispatch.** The README describes a `FunctionType` enum
   for calling free functions such as `ProcessUIActionsAndEvents`. This has not
   been implemented. `SimulationStep` only holds a `SimulationElement`
   (which is a `LogicClassEnum`); there is no union or variant for selecting a
   free function.

3. **`SimulationData` is per-tick but the runner treats all steps as happening
   once.** For a single-tick test this is fine. For multi-tick tests the same
   steps run every tick, which is by design according to the README. However,
   if a future requirement needs different steps per tick, the struct will need
   a `tick` field on `SimulationStep`. This is not a current blocker but should
   be noted.

4. **No coverage for `CraftingRenderLogic` in unit tests.** Once the dispatch
   case is added, a test should verify it executes without error.

---

## Plan

### Step 1 — Add `CraftingRenderLogic` dispatch

**File:** `tests/harness/SimulationRunner.cpp`

- Add `#include "CraftingRenderLogic.h"` (verify the exact header path in
  `src/logic/`).
- Add a case to `ExecuteLogicClass()`:
  ```cpp
  case LogicClassEnum::CraftingRenderLogic: {
    logic::CraftingRenderLogic logic(context);
    logic.RunLogic();
    return std::monostate{};
  }
  ```
- Confirm the constructor signature matches the existing pattern (`SceneContext`
  parameter).

### Step 2 — Define `FunctionType` enum and update `SimulationStep` (optional / future)

This step is not required to reach the integration-test viability threshold but
is noted here so the plan is complete.

**Files:** `src/types/test_structs/SimulationData.h`,
`tests/harness/SimulationRunner.h` / `.cpp`

- Add `enum class FunctionType { None = 0, ProcessUIActionsAndEvents = 1, ... }`.
- Change `SimulationStep::element` from `SimulationElement` (alias for
  `LogicClassEnum`) to `std::variant<LogicClassEnum, FunctionType>`.
- Add a dispatch branch in `ExecuteStep()` that handles `FunctionType` by
  calling the appropriate free function from `src/logic/`.
- Update `FlatbuffersTestDataProvider` to convert `FunctionTypeFbs` values into
  the new native `FunctionType`.

### Step 3 — Add unit tests for new dispatch cases

**File:** `tests/unit/harness/SimulationRunner.test.cpp`

- Add a test case `SimulationRunner dispatches CraftingRenderLogic without
  error` using a `TestFixture` configured with a crafting-scene context.
- If Step 2 is implemented, add a test case for each free function.

---

## Acceptance criteria

- [ ] `CraftingRenderLogic` is dispatched correctly by `ExecuteLogicClass()`.
- [ ] No `LogicClassEnum` value returns an error from `ExecuteLogicClass()` in
  the normal (non-`None`) path.
- [ ] Unit test exists for every dispatched Logic class.
- [ ] No existing unit tests broken.
