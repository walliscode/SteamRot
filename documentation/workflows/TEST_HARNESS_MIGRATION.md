# Test Harness Migration Guide

This guide explains how to migrate existing tests to use the new execution level framework, which provides a unified architecture where the test harness operates as an application of the game engine core.

## Overview of Changes

The new architecture introduces:

1. **Execution Level Framework** - Tests can run at 4 levels: Logic, Scene, SceneManager, GameLoop
2. **Extracted Core Functions** - Reusable free functions in `logic_execution`, `event_processing`, `game_loop`
3. **Unified Execution Runner** - Auto-detection and dispatch to appropriate executor
4. **Compile-Time Configuration** - `STEAMROT_TEST_MODE` for test-specific features

## Execution Levels

| Level | Scope | When to Use |
|-------|-------|-------------|
| Level 1: Logic | Single Logic class or free function | Unit testing individual logic classes |
| Level 2: Scene | Full scene tick (Action → Movement → Collision → Render) | Testing scene behavior without scene transitions |
| Level 3: SceneManager | SceneManager update cycle | Testing scene transitions and subscriptions |
| Level 4: GameLoop | Complete game loop iteration | End-to-end integration tests |

## Migration Steps

### Step 1: Identify Current Test Type

Examine your existing test to determine which execution level is appropriate:

**Level 1 (Logic)** - Tests that:
- Execute specific Logic classes (e.g., `UICollisionLogic`)
- Call free functions (e.g., `ProcessButtonElementActions`)
- Have `simulation_data` with specific steps

**Level 2 (Scene)** - Tests that:
- Execute full scene ticks
- Need Action → Movement → Collision → Render order
- Test scene behavior without scene changes

**Level 3 (SceneManager)** - Tests that:
- Test scene transitions
- Test event subscriptions
- Need SceneManager update cycles

**Level 4 (GameLoop)** - Tests that:
- Need full game loop behavior
- Test display rendering integration
- End-to-end tests

### Step 2: Update Test Code

#### Before (Old Pattern)

```cpp
#include "test_data_harness.h"

TEST_CASE("My test", "[unit]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  const auto *config = GENERATE_COPY(from_range(configs.value()));

  // Old pattern - RunFixtureTest handles everything internally
  auto result = steamrot::tests::RunFixtureTest(config);
  REQUIRE(result.has_value());
}
```

#### After (New Pattern with Explicit Level)

```cpp
#include "test_data_harness.h"
#include "execution/execution.h"  // Include execution framework

TEST_CASE("My test", "[unit]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  const auto *config = GENERATE_COPY(from_range(configs.value()));

  // New pattern - explicit execution level
  auto fixture_result = steamrot::tests::CreateFixtureFromTestData(config);
  REQUIRE(fixture_result.has_value());

  // Run at specific level
  auto result = steamrot::tests::execution::RunTestAtLevel(
      config, 
      steamrot::tests::execution::ExecutionLevel::Logic,
      fixture_result.value());
  REQUIRE(result.has_value());

  // Compare results
  if (config->expected_data_collection()) {
    auto comparison_result = steamrot::tests::RunDataStructComparisonTest(
        config->expected_data_collection(), 
        fixture_result.value(),
        steamrot::tests::TestContext{});
    REQUIRE(comparison_result.has_value());
  }
}
```

#### After (New Pattern with Auto-Detection)

```cpp
#include "test_data_harness.h"
#include "execution/execution.h"

TEST_CASE("My test", "[unit]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  const auto *config = GENERATE_COPY(from_range(configs.value()));

  // New pattern - auto-detect level from config
  auto fixture_result = steamrot::tests::CreateFixtureFromTestData(config);
  REQUIRE(fixture_result.has_value());

  auto result = steamrot::tests::execution::RunTestAutoLevel(
      config, fixture_result.value());
  REQUIRE(result.has_value());
}
```

### Step 3: Update Test Data (Optional)

The test data format remains compatible. However, you can add execution hints:

```json
{
  "metadata": {
    "test_name": "my_test",
    "description": "Test using execution levels"
  },
  "num_ticks": 3,
  "simulation_data": {
    "steps": [
      {
        "simulation_type": "Collision",
        "execution_mode": "LogicClass",
        "logic_class_type": "UICollisionLogic"
      }
    ]
  }
}
```

## Examples by Execution Level

### Level 1: Logic Execution

```cpp
#include "execution/logic_executor.h"

TEST_CASE("Test single logic class", "[unit][logic]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;
  
  // Execute specific logic class
  auto result = steamrot::tests::execution::ExecuteLogicByType(
      steamrot::LogicClassType::LogicClassType_UICollisionLogic,
      test_context.GetSceneContext());
  
  REQUIRE(result.has_value());
}
```

### Level 2: Scene Execution

```cpp
#include "execution/scene_executor.h"

TEST_CASE("Test full scene tick", "[unit][scene]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  auto fixture = steamrot::tests::TestFixture(steamrot::SceneType::SceneType_TEST);
  fixture.Intialize(nullptr);

  // Create logic collection for the scene
  steamrot::LogicCollection logics;
  // ... populate logics ...

  // Execute scene tick
  auto result = steamrot::tests::execution::ExecuteSceneTick(
      fixture.GetSceneContext(), logics);
  
  REQUIRE(result.has_value());
}
```

### Level 3: SceneManager Execution

```cpp
#include "execution/scene_manager_executor.h"

TEST_CASE("Test scene manager update", "[integration][scene_manager]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  auto fixture = steamrot::tests::TestFixture(steamrot::SceneType::SceneType_TITLE);
  fixture.Intialize(nullptr);

  // Execute SceneManager update
  auto result = steamrot::tests::execution::ExecuteSceneManagerWithFixture(
      fixture, 5);  // 5 updates
  
  REQUIRE(result.has_value());
}
```

### Level 4: GameLoop Execution

```cpp
#include "execution/game_loop_executor.h"

TEST_CASE("Test full game loop", "[integration][game_loop]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  auto fixture = steamrot::tests::TestFixture(steamrot::SceneType::SceneType_TITLE);
  fixture.Intialize(nullptr);

  steamrot::tests::execution::GameLoopConfig config;
  config.num_iterations = 10;
  config.headless = true;

  auto result = steamrot::tests::execution::ExecuteGameLoopWithFixture(
      fixture, config);
  
  REQUIRE(result.has_value());
}
```

## Backwards Compatibility

The existing `RunFixtureTest()` function continues to work and internally uses the execution framework. You can migrate incrementally:

1. Start with new tests using the execution level framework
2. Migrate existing tests as you modify them
3. Eventually all tests will use the unified framework

## Benefits of Migration

1. **Clarity** - Explicit execution level makes test intent clear
2. **Performance** - Run at the minimum level needed
3. **Debugging** - Easier to isolate issues at specific levels
4. **Reusability** - Same execution functions used by game engine and tests
5. **Flexibility** - Choose the right abstraction level for each test

## Common Migration Issues

### Issue: Test runs but results differ

**Cause**: Different execution levels process events differently.

**Solution**: Ensure you're using the correct level. Level 1 (Logic) doesn't process events between logics. Use Level 2 (Scene) for full tick processing.

### Issue: Events not being processed

**Cause**: Missing event tick processing.

**Solution**: Use `events::processing::ProcessEventTickStart()` and `ProcessEventTickEnd()` explicitly, or use Level 2+ which handles this automatically.

### Issue: LogicCollection is empty

**Cause**: At Level 2+, you need to provide the LogicCollection.

**Solution**: Either create the LogicCollection manually or use the test fixture's scene which provides default logics.

## API Reference

### Execution Level Functions

```cpp
// Level 1: Logic
ExecuteLogic(logic, scene_context)
ExecuteLogicByType(logic_class_type, scene_context)
ExecuteFunction(function_type, scene_context)
ExecuteSimulationStep(step, scene_context)
ExecuteWorkflow(simulation_data, scene_context)
ExecuteWorkflowWithFixture(simulation_data, fixture)

// Level 2: Scene
ExecuteSceneTick(scene_context, logics)
ExecuteSceneTickWithFixture(fixture, logics)
ExecuteConfiguredSceneTicks(scene_context, logics, num_ticks)
ExecuteSceneTicksWithEvents(fixture, logics, num_ticks)

// Level 3: SceneManager
ExecuteSceneManagerUpdate(scene_manager, game_context)
ExecuteConfiguredSceneManager(scene_manager, game_context, num_updates)
ExecuteSceneManagerUpdatesWithEvents(fixture, num_updates)
ExecuteSceneManagerWithFixture(fixture, num_updates)

// Level 4: GameLoop
ExecuteHeadlessGameLoop(game_context, scene_manager, num_iterations)
ExecuteFullGameLoop(game_context, scene_manager, display, num_iterations)
ExecuteConfiguredGameLoop(fixture, config)
ExecuteGameLoopWithFixture(fixture, config)

// Unified Runner
RunTestAtLevel(config, level, fixture)
DetectExecutionLevel(config)
RunTestAutoLevel(config, fixture)
```

## See Also

- [Test Harness README](../../tests/harness/README.md)
- [Test Data Configuration](../testing/TEST_DATA_CONFIGURATION.md)
- [Architecture Analysis](../analysis/TEST_HARNESS_GAME_ENGINE_ANALYSIS.md)
