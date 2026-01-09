# TestEngine Data Bank Implementation Roadmap

## Overview

This document provides a step-by-step implementation guide for adding data bank (snapshot storage) capability to the TestEngine, following the recommendation in [TESTENGINE_SIMULATION_ANALYSIS.md](TESTENGINE_SIMULATION_ANALYSIS.md).

## Implementation Phases

### Phase 1: Basic Data Bank Structure ⭐ START HERE
**Goal:** Add snapshot storage to TestEngine  
**Estimated Time:** 2-4 hours  
**Risk:** Low

### Phase 2: Snapshot Capture Integration
**Goal:** Capture EntityMemoryPool at each tick  
**Estimated Time:** 2-3 hours  
**Risk:** Low

### Phase 3: Test Harness Comparison
**Goal:** Compare snapshots using existing matchers  
**Estimated Time:** 3-4 hours  
**Risk:** Low

### Phase 4: Enhanced Snapshots (Optional)
**Goal:** Add EventBus and other state to snapshots  
**Estimated Time:** 4-6 hours  
**Risk:** Medium

## Phase 1: Basic Data Bank Structure

### 1.1 Update TestEngine Header

**File:** `tests/harness/TestEngine.h`

**Changes:**
```cpp
// Add after existing includes
#include <map>

namespace steamrot::tests {

class TestEngine : public Engine {
private:
  const TestData &m_test_data;
  size_t m_target_ticks{1};
  size_t m_current_tick{1};
  
  /////////////////////////////////////////////////
  /// @brief Storage for EntityMemoryPool snapshots at each tick
  ///
  /// Maps tick number to a copy of the EntityMemoryPool at that tick.
  /// Enables post-execution analysis and comparison.
  /////////////////////////////////////////////////
  std::map<size_t, EntityMemoryPool> m_data_bank;
  
  // ... existing methods ...
  
  /////////////////////////////////////////////////
  /// @brief Capture the current scene's EntityMemoryPool state
  ///
  /// Creates a deep copy of the EntityMemoryPool and stores it
  /// in the data bank indexed by tick number.
  ///
  /// @param tick Current tick number
  /////////////////////////////////////////////////
  void CaptureSnapshot(size_t tick);
  
public:
  // ... existing constructor ...
  
  /////////////////////////////////////////////////
  /// @brief Get the data bank containing all captured snapshots
  ///
  /// @return Const reference to the data bank
  /////////////////////////////////////////////////
  const std::map<size_t, EntityMemoryPool>& GetDataBank() const {
    return m_data_bank;
  }
};

} // namespace steamrot::tests
```

### 1.2 Implement CaptureSnapshot

**File:** `tests/harness/TestEngine.cpp`

**Add implementation:**
```cpp
/////////////////////////////////////////////////
void TestEngine::CaptureSnapshot(size_t tick) {
  // Get the current scene from SceneManager
  const auto& current_scene = m_scene_manager.GetCurrentScene();
  
  // Get the EntityMemoryPool from the scene
  const EntityMemoryPool& scene_pool = current_scene.GetEntityMemoryPool();
  
  // Deep copy the pool into the data bank
  // std::map operator[] will create the entry and copy-assign
  m_data_bank[tick] = scene_pool;
}
```

**Note:** If `Scene` doesn't have `GetEntityMemoryPool()`, you may need to add it or access it through `SceneContext`.

### 1.3 Integration Point in RunGameLoop

**File:** `tests/harness/TestEngine.cpp`

**Update existing method:**
```cpp
/////////////////////////////////////////////////
void TestEngine::RunGameLoop() {
  // Run through the specified number of loops
  for (size_t i = 0; i < m_target_ticks; i++) {
    // Update current tick
    m_current_tick = i;
    
    // Execute same tick pipeline as GameEngine
    ExecuteTick();
    
    // Capture snapshot after tick completes
    CaptureSnapshot(i);
  }
}
```

### 1.4 Unit Tests for Data Bank

**File:** `tests/unit/harness/TestEngine.test.cpp`

**Add tests:**
```cpp
TEST_CASE("TestEngine::GetDataBank returns empty map initially",
          "[unit][TestEngine]") {
  steamrot::TestData test_data;
  steamrot::tests::TestEngine engine(test_data);
  
  const auto& data_bank = engine.GetDataBank();
  REQUIRE(data_bank.empty());
}

TEST_CASE("TestEngine::RunGame captures snapshots in data bank",
          "[unit][TestEngine]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 3;
  
  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());
  
  // Assert
  const auto& data_bank = engine.GetDataBank();
  REQUIRE(data_bank.size() == 3);
  REQUIRE(data_bank.find(0) != data_bank.end());
  REQUIRE(data_bank.find(1) != data_bank.end());
  REQUIRE(data_bank.find(2) != data_bank.end());
}

TEST_CASE("TestEngine::GetDataBank contains actual EntityMemoryPool data",
          "[unit][TestEngine]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 1;
  // TODO: Configure test_data with starting entities
  
  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());
  
  // Assert
  const auto& data_bank = engine.GetDataBank();
  REQUIRE(data_bank.size() == 1);
  
  const EntityMemoryPool& snapshot = data_bank.at(0);
  // Verify the snapshot contains the expected data
  // TODO: Add specific assertions based on test_data configuration
}
```

## Phase 2: Snapshot Capture Integration

### 2.1 Verify Scene Access

Ensure TestEngine can access the scene's EntityMemoryPool:

**Option A:** Scene has direct getter
```cpp
class Scene {
public:
  const EntityMemoryPool& GetEntityMemoryPool() const {
    return m_scene_context.scene_entities;
  }
};
```

**Option B:** Access through SceneManager
```cpp
const auto& scene_context = m_scene_manager.GetCurrentScene().GetSceneContext();
const EntityMemoryPool& scene_pool = scene_context.scene_entities;
```

**Option C:** SceneManager provides direct access
```cpp
const EntityMemoryPool& scene_pool = m_scene_manager.GetCurrentSceneEntityPool();
```

Choose the option that fits the existing architecture.

### 2.2 Handle Multi-Scene Scenarios (Future)

Currently, TestEngine likely runs with a single scene. For future multi-scene support:

```cpp
void TestEngine::CaptureSnapshot(size_t tick) {
  // For now, capture only the current scene
  const auto& current_scene = m_scene_manager.GetCurrentScene();
  const EntityMemoryPool& scene_pool = current_scene.GetEntityMemoryPool();
  m_data_bank[tick] = scene_pool;
  
  // TODO: Future enhancement - capture all active scenes
  // std::map<SceneType, EntityMemoryPool> scene_snapshots;
  // for (auto& scene : m_scene_manager.GetActiveScenes()) {
  //   scene_snapshots[scene.GetType()] = scene.GetEntityMemoryPool();
  // }
  // m_data_bank[tick] = scene_snapshots;
}
```

### 2.3 Performance Monitoring (Optional)

Add optional performance logging:

```cpp
void TestEngine::CaptureSnapshot(size_t tick) {
  const auto& current_scene = m_scene_manager.GetCurrentScene();
  const EntityMemoryPool& scene_pool = current_scene.GetEntityMemoryPool();
  
  #ifdef STEAMROT_PROFILE_TESTS
  auto start = std::chrono::high_resolution_clock::now();
  #endif
  
  m_data_bank[tick] = scene_pool;
  
  #ifdef STEAMROT_PROFILE_TESTS
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  spdlog::debug("Snapshot at tick {} took {} μs", tick, duration.count());
  #endif
}
```

## Phase 3: Test Harness Comparison

### 3.1 Implement Snapshot Comparison Function

**File:** `tests/harness/test_data_comparison.h` (or new file)

```cpp
/////////////////////////////////////////////////
/// @brief Compare EntityMemoryPool from data bank with expected snapshot
///
/// @param tick Tick number for context
/// @param actual_pool Actual EntityMemoryPool from data bank
/// @param expected_pool Expected EntityMemoryPool from test data
/// @param test_context Test metadata for error messages
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> CompareDataBankSnapshot(
    size_t tick,
    const EntityMemoryPool& actual_pool,
    const EntityMemoryPool& expected_pool,
    const TestContext& test_context);
```

**File:** `tests/harness/test_data_comparison.cpp`

```cpp
std::expected<std::monostate, FailInfo> CompareDataBankSnapshot(
    size_t tick,
    const EntityMemoryPool& actual_pool,
    const EntityMemoryPool& expected_pool,
    const TestContext& test_context) {
  
  // Create matcher with test context
  EntityMemoryPoolEqualsMatcher matcher(expected_pool, test_context);
  
  // Perform comparison
  if (!matcher.match(actual_pool)) {
    return std::unexpected(FailInfo{
        FailMode::ValidationFailed,
        std::format("Snapshot comparison failed at tick {}\n{}", 
                   tick, matcher.describe())
    });
  }
  
  return std::monostate{};
}
```

### 3.2 Update RunTestEngineTest Function

**File:** `tests/harness/test_harness.cpp` (or wherever this function lives)

```cpp
std::expected<std::monostate, FailInfo> 
RunTestEngineTest(const TestDataConfig* config) {
  // Validate config
  if (!config || !config->metadata()) {
    return std::unexpected(FailInfo{
        FailMode::InvalidInput,
        "TestDataConfig or metadata is null"
    });
  }
  
  // Convert FlatBuffers TestDataConfig to TestData
  // (Implementation depends on existing conversion functions)
  TestData test_data = ConvertTestDataConfigToTestData(config);
  
  // Create and run TestEngine
  TestEngine engine(test_data);
  auto run_result = engine.RunGame();
  if (!run_result.has_value()) {
    return std::unexpected(run_result.error());
  }
  
  // Get data bank
  const auto& data_bank = engine.GetDataBank();
  
  // Prepare test context
  TestContext base_context;
  base_context.test_name = config->metadata()->test_name()->str();
  base_context.description = config->metadata()->description()->str();
  base_context.total_ticks = test_data.number_of_ticks;
  
  // Compare each expected snapshot
  for (const auto& [tick, expected_snapshot] : test_data.expected_engine_snapshots) {
    // Find actual snapshot in data bank
    auto it = data_bank.find(tick);
    if (it == data_bank.end()) {
      return std::unexpected(FailInfo{
          FailMode::MissingData,
          std::format("No snapshot found in data bank for tick {}", tick)
      });
    }
    
    const EntityMemoryPool& actual_pool = it->second;
    const EntityMemoryPool& expected_pool = expected_snapshot.entity_pool;
    
    // Update context with current tick
    TestContext tick_context = base_context;
    tick_context.current_tick = tick;
    
    // Compare using helper function
    auto compare_result = CompareDataBankSnapshot(
        tick, actual_pool, expected_pool, tick_context);
    
    if (!compare_result.has_value()) {
      return std::unexpected(compare_result.error());
    }
  }
  
  return std::monostate{};
}
```

### 3.3 Integration Tests

**File:** `tests/unit/harness/test_harness.test.cpp` (or integration test file)

```cpp
TEST_CASE("RunTestEngineTest compares data bank with expected snapshots",
          "[unit][test_harness]") {
  // Load test data
  auto configs = steamrot::tests::load_test_data_configs("harness");
  REQUIRE(configs.has_value());
  
  // Find a test config with expected snapshots
  const TestDataConfig* config = nullptr;
  for (const auto* c : configs.value()) {
    if (c->tick_snapshots() && c->tick_snapshots()->size() > 0) {
      config = c;
      break;
    }
  }
  
  REQUIRE(config != nullptr);
  
  // Run test
  auto result = steamrot::tests::RunTestEngineTest(config);
  
  // Should pass if test data is correct
  if (config->metadata()->expected_to_pass()) {
    REQUIRE(result.has_value());
  } else {
    REQUIRE(!result.has_value());
  }
}
```

## Phase 4: Enhanced Snapshots (Optional)

### 4.1 Define EngineSnapshot Structure

**File:** `src/types/test_structs/EngineSnapshot.h`

**Update from empty struct:**
```cpp
namespace steamrot {

/////////////////////////////////////////////////
/// @struct EngineSnapshot
/// @brief Container for complete engine state at a specific tick
/////////////////////////////////////////////////
struct EngineSnapshot {
  /////////////////////////////////////////////////
  /// @brief Entity state for all entities
  /////////////////////////////////////////////////
  EntityMemoryPool entity_pool;
  
  /////////////////////////////////////////////////
  /// @brief Event bus state (global events)
  /////////////////////////////////////////////////
  EventBus event_bus;
  
  /////////////////////////////////////////////////
  /// @brief Active scene type
  /////////////////////////////////////////////////
  SceneType active_scene{SceneType::SceneType_TEST};
  
  /////////////////////////////////////////////////
  /// @brief Tick number when snapshot was captured
  /////////////////////////////////////////////////
  size_t tick_number{0};
};

} // namespace steamrot
```

### 4.2 Update TestEngine Data Bank Type

**File:** `tests/harness/TestEngine.h`

```cpp
class TestEngine : public Engine {
private:
  // Change from simple EntityMemoryPool to full EngineSnapshot
  std::map<size_t, EngineSnapshot> m_data_bank;
  
  void CaptureSnapshot(size_t tick);
  
public:
  const std::map<size_t, EngineSnapshot>& GetDataBank() const {
    return m_data_bank;
  }
};
```

### 4.3 Update CaptureSnapshot

**File:** `tests/harness/TestEngine.cpp`

```cpp
void TestEngine::CaptureSnapshot(size_t tick) {
  EngineSnapshot snapshot;
  
  // Capture entity state
  const auto& current_scene = m_scene_manager.GetCurrentScene();
  snapshot.entity_pool = current_scene.GetEntityMemoryPool();
  
  // Capture event bus state
  snapshot.event_bus = m_engine_resources.event_handler.GetGlobalEventBus();
  
  // Capture scene info
  snapshot.active_scene = current_scene.GetSceneType();
  snapshot.tick_number = tick;
  
  // Store in data bank
  m_data_bank[tick] = snapshot;
}
```

### 4.4 Update Comparison Functions

**File:** `tests/harness/test_data_comparison.cpp`

```cpp
std::expected<std::monostate, FailInfo> CompareEngineSnapshot(
    size_t tick,
    const EngineSnapshot& actual,
    const EngineSnapshot& expected,
    const TestContext& test_context) {
  
  // Compare EntityMemoryPool
  EntityMemoryPoolEqualsMatcher pool_matcher(expected.entity_pool, test_context);
  if (!pool_matcher.match(actual.entity_pool)) {
    return std::unexpected(FailInfo{
        FailMode::ValidationFailed,
        std::format("Entity pool mismatch at tick {}\n{}", 
                   tick, pool_matcher.describe())
    });
  }
  
  // Compare EventBus
  EventBusEqualsMatcher bus_matcher(expected.event_bus, test_context);
  if (!bus_matcher.match(actual.event_bus)) {
    return std::unexpected(FailInfo{
        FailMode::ValidationFailed,
        std::format("Event bus mismatch at tick {}\n{}", 
                   tick, bus_matcher.describe())
    });
  }
  
  // Compare scene type
  if (actual.active_scene != expected.active_scene) {
    return std::unexpected(FailInfo{
        FailMode::ValidationFailed,
        std::format("Scene type mismatch at tick {}: actual={}, expected={}", 
                   tick, 
                   static_cast<int>(actual.active_scene),
                   static_cast<int>(expected.active_scene))
    });
  }
  
  return std::monostate{};
}
```

## Testing Strategy

### Unit Tests (Phase 1-2)
- TestEngine captures snapshots
- GetDataBank returns correct data structure
- Snapshot count matches tick count

### Integration Tests (Phase 3)
- End-to-end test with RunTestEngineTest
- Comparison with expected snapshots
- Error reporting with TestContext

### Data-Driven Tests (Phase 3)
- Use test_data.json files
- Test various simulation scenarios
- Verify tick-by-tick state changes

## Common Issues and Solutions

### Issue 1: Scene doesn't expose EntityMemoryPool

**Solution:** Add getter to Scene class
```cpp
// In Scene.h
class Scene {
public:
  const EntityMemoryPool& GetEntityMemoryPool() const {
    return m_scene_context.scene_entities;
  }
};
```

### Issue 2: Copying EntityMemoryPool is slow

**Solution:** Profile first, optimize only if needed
```cpp
// Measure copy time
auto start = std::chrono::high_resolution_clock::now();
m_data_bank[tick] = scene_pool;
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
if (duration.count() > 1000) {
  spdlog::warn("Slow snapshot copy: {} μs", duration.count());
}
```

**Optimization:** Use move semantics if possible (rarely needed)

### Issue 3: TestData doesn't have expected_engine_snapshots

**Solution:** Add to TestData struct if missing
```cpp
// In TestData.h
struct TestData {
  // ... existing fields ...
  std::map<size_t, EngineSnapshot> expected_engine_snapshots;
};
```

### Issue 4: Comparison fails with unclear errors

**Solution:** Ensure TestContext is populated
```cpp
TestContext context;
context.test_name = "my_test";
context.current_tick = tick;
context.total_ticks = total;
// Context is included in matcher output
```

## Validation Checklist

Before considering implementation complete:

- [ ] TestEngine has m_data_bank member
- [ ] CaptureSnapshot() is implemented
- [ ] RunGameLoop() calls CaptureSnapshot() after each tick
- [ ] GetDataBank() returns const reference
- [ ] Unit tests verify snapshot capture
- [ ] Comparison function uses existing matchers
- [ ] RunTestEngineTest integrates data bank
- [ ] Integration tests pass
- [ ] Documentation updated (README, examples)
- [ ] Code reviewed for edge cases

## Performance Targets

### Snapshot Capture
- **Target:** < 100 μs per snapshot for typical test (50 entities)
- **Acceptable:** < 1 ms per snapshot for stress test (1000 entities)
- **If exceeded:** Profile and optimize (unlikely to be needed)

### Memory Usage
- **Target:** < 1 MB for typical test (5 ticks, 50 entities)
- **Acceptable:** < 100 MB for stress test (100 ticks, 1000 entities)
- **If exceeded:** Consider selective snapshotting (capture only expected ticks)

## Future Enhancements

### Selective Snapshotting
Only capture snapshots for ticks with expected data:
```cpp
void TestEngine::RunGameLoop() {
  for (size_t i = 0; i < m_target_ticks; i++) {
    ExecuteTick();
    
    // Only capture if expected snapshot exists
    if (m_test_data.expected_engine_snapshots.contains(i)) {
      CaptureSnapshot(i);
    }
  }
}
```

### Snapshot Diffing
Show only differences between ticks:
```cpp
void ReportSnapshotDiff(const EngineSnapshot& from,
                       const EngineSnapshot& to) {
  // Compare and show only changed entities/components
}
```

### Snapshot Export
Export snapshots for external analysis:
```cpp
void ExportDataBankToJson(const std::map<size_t, EngineSnapshot>& data_bank,
                         const std::filesystem::path& output_path);
```

## Timeline Estimate

| Phase | Duration | Complexity |
|-------|----------|------------|
| Phase 1: Data Bank Structure | 2-4 hours | Low |
| Phase 2: Capture Integration | 2-3 hours | Low |
| Phase 3: Comparison | 3-4 hours | Low |
| Phase 4: Enhanced Snapshots | 4-6 hours | Medium |
| **Total (Phases 1-3)** | **7-11 hours** | **Low** |
| **Total (All Phases)** | **11-17 hours** | **Low-Medium** |

## Getting Started

1. **Read the analysis:** Review [TESTENGINE_SIMULATION_ANALYSIS.md](TESTENGINE_SIMULATION_ANALYSIS.md)
2. **Start with Phase 1:** Add m_data_bank to TestEngine
3. **Write tests early:** Add unit tests for GetDataBank()
4. **Iterate quickly:** Get basic functionality working before optimizing
5. **Use existing patterns:** Leverage TestContext and matchers

## Questions?

Refer to:
- [TESTENGINE_SIMULATION_ANALYSIS.md](TESTENGINE_SIMULATION_ANALYSIS.md) - Detailed analysis
- [TESTENGINE_QUICK_DECISION_GUIDE.md](TESTENGINE_QUICK_DECISION_GUIDE.md) - Quick reference
- `tests/harness/README.md` - Test harness overview
- `tests/matchers/EntityMemoryPoolEqualsMatcher.h` - Matcher implementation
