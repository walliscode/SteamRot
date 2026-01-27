# Implementation Plan: EngineSnapshot Matchers

## Executive Summary

This document provides a comprehensive implementation plan for building up a series of Catch2 matchers to enable comparison of `EngineSnapshot` objects. The primary goal is to validate the state of `EntityMemoryPool`(s) after running the `TestEngine` in data-driven tests.

## Background

### Current State

The SteamRot game engine has:

1. **TestEngine**: A testing harness that extends `Engine` and captures `EngineSnapshot` objects at each tick
2. **Data Bank**: A map storing `EngineSnapshot` objects keyed by tick number (`std::map<size_t, EngineSnapshot>`)
3. **Existing Matchers**: Component-level matchers (`EntityMemoryPoolEqualsMatcher`, `EventBusEqualsMatcher`, individual component matchers)
4. **EngineSnapshot Structure**: Contains optional fields for tick number, global event bus, scene manager data, and scene collection data

### Problem Statement

Currently, there is no matcher infrastructure to compare complete `EngineSnapshot` objects. To enable comprehensive data-driven testing with TestEngine, we need to build up matchers from the bottom up, with special attention to:

1. **entity_transport variant handling**: SceneData's `entity_transport` field is a variant that can hold different types
2. **EntityMemoryPool state comparison**: The core goal is comparing entity state after TestEngine execution
3. **Optional field handling**: EngineSnapshot uses std::optional for all fields to enable selective comparison
4. **Hierarchical comparison**: EngineSnapshot contains nested structures that need their own matchers

## Matcher Hierarchy

The matcher implementation follows a bottom-up approach, building from simple to complex:

```
EngineSnapshotEqualsMatcher (TOP LEVEL)
├── EventBusEqualsMatcher (ALREADY EXISTS)
├── SceneManagerDataEqualsMatcher (NEW)
│   └── SceneManagerStateEqualsMatcher (NEW)
│       └── SubscriberEqualsMatcher (ALREADY EXISTS)
└── SceneCollectionDataEqualsMatcher (NEW)
    └── SceneDataEqualsMatcher (NEW - CRITICAL)
        ├── SceneInfoEqualsMatcher (NEW)
        ├── SceneResourcesConfigEqualsMatcher (NEW)
        ├── AssetConfigEqualsMatcher (NEW)
        └── EntityTransportEqualsMatcher (NEW - CRITICAL)
            └── EntityMemoryPoolEqualsMatcher (ALREADY EXISTS)
```

### Matcher Dependencies

**ALREADY EXISTS** (No work needed):
- `EntityMemoryPoolEqualsMatcher` - Compares entity pools with component-level detail
- `EventBusEqualsMatcher` - Compares event bus state
- `SubscriberEqualsMatcher` - Compares subscriber objects
- Individual component matchers (CMeta, CUserInterface, etc.)

**NEEDS IMPLEMENTATION**:
- SceneInfo matcher
- SceneResourcesConfig matcher
- AssetConfig matcher
- EntityTransport matcher (handles variant)
- SceneData matcher (uses above matchers)
- SceneCollectionData matcher (vector of SceneData)
- SceneManagerState matcher
- SceneManagerData matcher
- EngineSnapshot matcher (top level)

## Critical Design Decision: entity_transport Comparison

### The Challenge

`SceneData` contains an `entity_transport` field defined as:

```cpp
std::variant<std::monostate, 
             std::unique_ptr<IEntityImporter>,
             std::shared_ptr<EntityMemoryPool>, 
             EntityMemoryPool>
    entity_transport;
```

### TestEngine's Approach

From `TestEngine::StoreEngineSnapShot()`:

```cpp
// TestEngine converts shared_ptr<EntityMemoryPool> to copy-constructed EntityMemoryPool
for (auto &scene_data : snapshot.scene_collection_data) {
  auto ptr = std::get_if<std::shared_ptr<EntityMemoryPool>>(&scene_data.entity_transport);
  if (ptr && *ptr) {
    // Assign a copy constructed EntityMemoryPool to the variant
    scene_data.entity_transport = **ptr;
  }
}
```

**Key Insight**: TestEngine stores snapshots with `EntityMemoryPool` value type (copy constructed from shared_ptr)

### Matcher Strategy

The `EntityTransportEqualsMatcher` must:

1. **Check variant type**: Verify both actual and expected hold the same variant alternative
2. **Handle EntityMemoryPool comparison**: When both hold `EntityMemoryPool`, use `EntityMemoryPoolEqualsMatcher`
3. **Handle other types appropriately**:
   - `std::monostate`: Simple equality (both empty)
   - `std::shared_ptr<EntityMemoryPool>`: Dereference and compare with EntityMemoryPoolEqualsMatcher
   - `std::unique_ptr<IEntityImporter>`: Not comparable in tests (matcher should fail or skip)

**Primary Use Case**: Compare two `EntityMemoryPool` values (copy-constructed from TestEngine snapshots)

**Error Cases**:
- Variant type mismatch (e.g., one is monostate, other is EntityMemoryPool)
- Null pointers when expecting shared_ptr
- IEntityImporter comparison (not supported in snapshot testing)

## Detailed Implementation Plan

### Phase 1: Foundation Matchers (Simple Structs)

These matchers compare simple data structures without complex nested comparisons.

#### 1.1 SceneInfoEqualsMatcher

**Purpose**: Compare SceneInfo objects (UUID and SceneType)

**Files to Create**:
- `tests/matchers/SceneInfoEqualsMatcher.h`
- `tests/matchers/SceneInfoEqualsMatcher.cpp`

**Structure**: `SceneInfo` contains:
```cpp
struct SceneInfo {
  uuids::uuid id;
  SceneType type;
};
```

**Comparison Logic**:
- Compare UUIDs using `uuid` equality operator
- Compare SceneType enums
- Format output with UUID string and scene type name

**Test Cases**:
- Matching UUIDs and types
- Mismatched UUIDs (same type)
- Mismatched types (same UUID)
- Mismatched both

**Implementation Notes**:
- Inherit from `Catch::Matchers::MatcherBase<SceneInfo>`
- Provide `match()` and `describe()` methods
- Use TestContext for enriched error messages (optional)
- Helper function: `EqualsSceneInfo(const SceneInfo& expected)`

#### 1.2 SceneResourcesConfigEqualsMatcher

**Purpose**: Compare SceneResourcesConfig objects

**Files to Create**:
- `tests/matchers/SceneResourcesConfigEqualsMatcher.h`
- `tests/matchers/SceneResourcesConfigEqualsMatcher.cpp`

**Structure**: `SceneResourcesConfig` contains:
```cpp
struct SceneResourcesConfig {
  size_t entity_memory_pool_size;
  sf::Vector2u render_texture_size;
};
```

**Comparison Logic**:
- Compare pool size (size_t)
- Compare render texture dimensions (x, y)

**Test Cases**:
- Exact matches
- Pool size mismatch
- Texture dimension mismatch
- Combined mismatches

**Implementation Notes**:
- Simple field-by-field comparison
- Clear error messages showing expected vs actual values

#### 1.3 AssetConfigEqualsMatcher

**Purpose**: Compare AssetConfig objects

**Files to Create**:
- `tests/matchers/AssetConfigEqualsMatcher.h`
- `tests/matchers/AssetConfigEqualsMatcher.cpp`

**Note**: Need to examine `AssetConfig` structure first. It may contain:
- Font configurations
- Texture configurations
- Audio configurations

**Comparison Logic**: TBD based on actual structure

**Test Cases**: TBD based on actual structure

**Implementation Priority**: Medium (may be simple if AssetConfig is basic)

### Phase 2: Critical Variant Matcher

#### 2.1 EntityTransportEqualsMatcher

**Purpose**: Compare entity_transport variants with emphasis on EntityMemoryPool comparison

**Files to Create**:
- `tests/matchers/EntityTransportEqualsMatcher.h`
- `tests/matchers/EntityTransportEqualsMatcher.cpp`

**This is the MOST CRITICAL matcher for entity state comparison**

**Comparison Logic**:

```cpp
bool match(const EntityTransportVariant& actual) const override {
  // 1. Check variant indices match
  if (actual.index() != m_expected.index()) {
    m_mismatch_description = "Variant type mismatch";
    return false;
  }

  // 2. Handle monostate (both empty)
  if (std::holds_alternative<std::monostate>(actual)) {
    return true; // Both empty is a match
  }

  // 3. Handle EntityMemoryPool (PRIMARY CASE for TestEngine)
  if (std::holds_alternative<EntityMemoryPool>(actual)) {
    const auto& actual_pool = std::get<EntityMemoryPool>(actual);
    const auto& expected_pool = std::get<EntityMemoryPool>(m_expected);
    
    EntityMemoryPoolEqualsMatcher pool_matcher(expected_pool, m_context);
    if (!pool_matcher.match(actual_pool)) {
      m_mismatch_description = pool_matcher.describe();
      return false;
    }
    return true;
  }

  // 4. Handle shared_ptr<EntityMemoryPool>
  if (std::holds_alternative<std::shared_ptr<EntityMemoryPool>>(actual)) {
    auto actual_ptr = std::get<std::shared_ptr<EntityMemoryPool>>(actual);
    auto expected_ptr = std::get<std::shared_ptr<EntityMemoryPool>>(m_expected);
    
    if (!actual_ptr || !expected_ptr) {
      m_mismatch_description = "Null pointer in shared_ptr<EntityMemoryPool>";
      return false;
    }
    
    EntityMemoryPoolEqualsMatcher pool_matcher(*expected_ptr, m_context);
    return pool_matcher.match(*actual_ptr);
  }

  // 5. Handle unique_ptr<IEntityImporter>
  if (std::holds_alternative<std::unique_ptr<IEntityImporter>>(actual)) {
    // IEntityImporter cannot be meaningfully compared in snapshots
    // This should not occur in TestEngine snapshots
    m_mismatch_description = "IEntityImporter comparison not supported in snapshot testing";
    return false;
  }

  return false; // Should never reach here
}
```

**Test Cases**:
- **Both EntityMemoryPool** (primary case):
  - Matching pools
  - Pools with different entity counts
  - Pools with different component states
- **Both monostate**: Should match
- **Both shared_ptr<EntityMemoryPool>**:
  - Matching dereferenced pools
  - One or both null pointers
- **Variant type mismatch**:
  - monostate vs EntityMemoryPool
  - shared_ptr vs value type
- **IEntityImporter case**: Should fail with clear message

**Implementation Notes**:
- Support TestContext for enriched error messages
- Delegate to EntityMemoryPoolEqualsMatcher for pool comparison
- Clear error messages for variant type mismatches
- Helper function: `EqualsEntityTransport(const EntityTransportVariant& expected)`

### Phase 3: Composite Matchers

#### 3.1 SceneDataEqualsMatcher

**Purpose**: Compare complete SceneData objects

**Files to Create**:
- `tests/matchers/SceneDataEqualsMatcher.h`
- `tests/matchers/SceneDataEqualsMatcher.cpp`

**Dependencies**:
- SceneInfoEqualsMatcher
- SceneResourcesConfigEqualsMatcher
- AssetConfigEqualsMatcher
- EntityTransportEqualsMatcher

**Comparison Logic**:

```cpp
bool match(const SceneData& actual) const override {
  std::ostringstream oss;
  bool all_match = true;

  // Compare scene_info
  SceneInfoEqualsMatcher info_matcher(m_expected.scene_info);
  if (!info_matcher.match(actual.scene_info)) {
    oss << info_matcher.describe() << "\n";
    all_match = false;
  }

  // Compare scene_resources_config
  SceneResourcesConfigEqualsMatcher resources_matcher(m_expected.scene_resources_config);
  if (!resources_matcher.match(actual.scene_resources_config)) {
    oss << resources_matcher.describe() << "\n";
    all_match = false;
  }

  // Compare scene_asset_config
  AssetConfigEqualsMatcher asset_matcher(m_expected.scene_asset_config);
  if (!asset_matcher.match(actual.scene_asset_config)) {
    oss << asset_matcher.describe() << "\n";
    all_match = false;
  }

  // Compare entity_transport (CRITICAL)
  EntityTransportEqualsMatcher transport_matcher(m_expected.entity_transport, m_context);
  if (!transport_matcher.match(actual.entity_transport)) {
    oss << transport_matcher.describe() << "\n";
    all_match = false;
  }

  if (!all_match) {
    m_mismatch_description = oss.str();
  }
  return all_match;
}
```

**Test Cases**:
- Complete SceneData match (all fields)
- SceneInfo mismatch
- Resources config mismatch
- Asset config mismatch
- Entity transport mismatch (EntityMemoryPool)
- Multiple field mismatches (aggregated output)

**Implementation Notes**:
- Aggregate all sub-matcher output
- Provide scene UUID in error output for multi-scene scenarios
- Support TestContext

#### 3.2 SceneCollectionDataEqualsMatcher

**Purpose**: Compare vectors of SceneData (SceneCollectionData)

**Files to Create**:
- `tests/matchers/SceneCollectionDataEqualsMatcher.h`
- `tests/matchers/SceneCollectionDataEqualsMatcher.cpp`

**Dependencies**:
- SceneDataEqualsMatcher

**Comparison Logic**:

```cpp
bool match(const SceneCollectionData& actual) const override {
  // 1. Check size
  if (actual.size() != m_expected.size()) {
    m_mismatch_description = std::format(
      "Scene collection size mismatch: expected {}, got {}",
      m_expected.size(), actual.size());
    return false;
  }

  // 2. Compare each SceneData
  std::ostringstream oss;
  bool all_match = true;

  for (size_t i = 0; i < m_expected.size(); ++i) {
    SceneDataEqualsMatcher scene_matcher(m_expected[i], m_context);
    if (!scene_matcher.match(actual[i])) {
      oss << "Scene [" << i << "] " 
          << m_expected[i].scene_info.id << ":\n"
          << scene_matcher.describe() << "\n";
      all_match = false;
    }
  }

  if (!all_match) {
    m_mismatch_description = oss.str();
  }
  return all_match;
}
```

**Test Cases**:
- Empty collections (both empty)
- Single scene match
- Multiple scenes all matching
- Size mismatch
- One scene mismatch in collection
- Multiple scene mismatches
- Scene order matters (index-based comparison)

**Implementation Notes**:
- Include scene index and UUID in error messages
- Aggregate failures from all scenes
- Clear indication of which scene(s) failed

### Phase 4: SceneManager Matchers (Optional/Future)

#### 4.1 SceneManagerStateEqualsMatcher

**Purpose**: Compare SceneManagerState objects

**Files to Create**:
- `tests/matchers/SceneManagerStateEqualsMatcher.h`
- `tests/matchers/SceneManagerStateEqualsMatcher.cpp`

**Dependencies**:
- SubscriberEqualsMatcher (ALREADY EXISTS)

**Structure**: `SceneManagerState` contains:
```cpp
struct SceneManagerState {
  std::vector<std::shared_ptr<Subscriber>> subscriptions;
  // Future: active scene tracking, transition state, etc.
};
```

**Comparison Logic**:
- Compare subscription count
- Compare each subscriber using SubscriberEqualsMatcher
- Handle future fields as added

**Implementation Priority**: LOW (EngineSnapshot.scene_manager_data is optional and may not be used initially)

#### 4.2 SceneManagerDataEqualsMatcher

**Purpose**: Compare SceneManagerData objects

**Files to Create**:
- `tests/matchers/SceneManagerDataEqualsMatcher.h`
- `tests/matchers/SceneManagerDataEqualsMatcher.cpp`

**Dependencies**:
- SceneManagerStateEqualsMatcher

**Structure**: `SceneManagerData` contains:
```cpp
struct SceneManagerData {
  SceneManagerState scene_manager_state;
};
```

**Comparison Logic**:
- Delegate to SceneManagerStateEqualsMatcher

**Implementation Priority**: LOW (same reason as SceneManagerStateEqualsMatcher)

### Phase 5: Top-Level Matcher

#### 5.1 EngineSnapshotEqualsMatcher

**Purpose**: Compare complete EngineSnapshot objects with optional field handling

**Files to Create**:
- `tests/matchers/EngineSnapshotEqualsMatcher.h`
- `tests/matchers/EngineSnapshotEqualsMatcher.cpp`

**Dependencies**:
- EventBusEqualsMatcher (ALREADY EXISTS)
- SceneManagerDataEqualsMatcher (NEW)
- SceneCollectionDataEqualsMatcher (NEW)

**Comparison Logic**:

```cpp
bool match(const EngineSnapshot& actual) const override {
  std::ostringstream oss;
  bool all_match = true;

  // 1. Compare tick_number (optional)
  if (m_expected.tick_number.has_value()) {
    if (!actual.tick_number.has_value()) {
      oss << "Expected tick_number, but actual has none\n";
      all_match = false;
    } else if (actual.tick_number.value() != m_expected.tick_number.value()) {
      oss << std::format("Tick number mismatch: expected {}, got {}\n",
                        m_expected.tick_number.value(),
                        actual.tick_number.value());
      all_match = false;
    }
  }

  // 2. Compare global_event_bus (optional)
  if (m_expected.global_event_bus.has_value()) {
    if (!actual.global_event_bus.has_value()) {
      oss << "Expected global_event_bus, but actual has none\n";
      all_match = false;
    } else {
      EventBusEqualsMatcher bus_matcher(m_expected.global_event_bus.value(), m_context);
      if (!bus_matcher.match(actual.global_event_bus.value())) {
        oss << bus_matcher.describe() << "\n";
        all_match = false;
      }
    }
  }

  // 3. Compare scene_manager_data (optional)
  if (m_expected.scene_manager_data.has_value()) {
    if (!actual.scene_manager_data.has_value()) {
      oss << "Expected scene_manager_data, but actual has none\n";
      all_match = false;
    } else {
      SceneManagerDataEqualsMatcher manager_matcher(
          m_expected.scene_manager_data.value(), m_context);
      if (!manager_matcher.match(actual.scene_manager_data.value())) {
        oss << manager_matcher.describe() << "\n";
        all_match = false;
      }
    }
  }

  // 4. Compare scene_collection_data (NOT optional - always present)
  SceneCollectionDataEqualsMatcher collection_matcher(
      m_expected.scene_collection_data, m_context);
  if (!collection_matcher.match(actual.scene_collection_data)) {
    oss << collection_matcher.describe() << "\n";
    all_match = false;
  }

  if (!all_match) {
    m_mismatch_description = oss.str();
  }
  return all_match;
}
```

**Key Features**:
- **Optional field handling**: Only compare fields present in expected
- **Selective comparison**: Tests can populate only the fields they care about
- **Aggregated output**: Shows all mismatches, not just the first
- **TestContext support**: Includes test name, description, tick info

**Test Cases**:
- **Minimal comparison** (only scene_collection_data):
  - Match
  - Mismatch
- **With tick_number**:
  - Match
  - Mismatch
  - Expected has tick, actual doesn't
- **With global_event_bus**:
  - Match
  - Mismatch
  - Optional presence mismatch
- **With scene_manager_data**:
  - Match
  - Mismatch
  - Optional presence mismatch
- **All fields populated**:
  - Complete match
  - Multiple field mismatches
- **Selective comparison** (only some optional fields):
  - Tests verify only fields they set

**Helper Functions**:

```cpp
// Basic matcher (no context)
inline EngineSnapshotEqualsMatcher 
EqualsEngineSnapshot(const EngineSnapshot& expected) {
  return EngineSnapshotEqualsMatcher(expected);
}

// With TestContext for enriched error output
inline EngineSnapshotEqualsMatcher 
EqualsEngineSnapshot(const EngineSnapshot& expected, const TestContext& context) {
  return EngineSnapshotEqualsMatcher(expected, context);
}
```

## Implementation Order

**Recommended order (bottom-up, based on dependencies):**

1. **Phase 1: Foundation**
   - [ ] SceneInfoEqualsMatcher
   - [ ] SceneResourcesConfigEqualsMatcher
   - [ ] AssetConfigEqualsMatcher (examine structure first)

2. **Phase 2: Critical Variant**
   - [ ] EntityTransportEqualsMatcher (CRITICAL - requires careful testing)

3. **Phase 3: Composite**
   - [ ] SceneDataEqualsMatcher (depends on Phase 1 & 2)
   - [ ] SceneCollectionDataEqualsMatcher (depends on SceneDataEqualsMatcher)

4. **Phase 4: Optional/Future**
   - [ ] SceneManagerStateEqualsMatcher (low priority)
   - [ ] SceneManagerDataEqualsMatcher (low priority)

5. **Phase 5: Top Level**
   - [ ] EngineSnapshotEqualsMatcher (depends on all previous phases)

## Testing Strategy

### Unit Tests Per Matcher

Each matcher should have comprehensive unit tests:

**Test File Naming**: `{MatcherName}.test.cpp` (e.g., `SceneDataEqualsMatcher.test.cpp`)

**Test Organization**:
```cpp
TEST_CASE("MatcherName - exact match", "[unit][matchers]") { ... }
TEST_CASE("MatcherName - field X mismatch", "[unit][matchers]") { ... }
TEST_CASE("MatcherName - multiple mismatches", "[unit][matchers]") { ... }
TEST_CASE("MatcherName - error message format", "[unit][matchers]") { ... }
```

**Key Test Scenarios**:
1. **Exact matches** - Verify matcher returns true for identical objects
2. **Single field mismatches** - Test each field independently
3. **Multiple field mismatches** - Verify aggregated error output
4. **Edge cases** - Empty vectors, null pointers, monostate variants
5. **Error message clarity** - Verify describe() output is helpful

### Integration Tests

**EntityTransport Integration Tests**:
- Test with actual EntityMemoryPool snapshots from TestEngine
- Verify variant type handling matches TestEngine behavior
- Test with copy-constructed vs shared_ptr scenarios

**EngineSnapshot Integration Tests**:
- Test with complete EngineSnapshot from TestEngine.GetDataBank()
- Test optional field handling (selective comparison)
- Test multi-scene scenarios
- Test across multiple ticks

**Test Data Location**: `tests/matchers/data/` for test-specific data files

## CMake Integration

**Update**: `tests/matchers/CMakeLists.txt`

Add new source files:
```cmake
target_sources(test_matchers PRIVATE
  # Existing matchers...
  EntityMemoryPoolEqualsMatcher.cpp
  EventBusEqualsMatcher.cpp
  
  # New matchers (Phase 1)
  SceneInfoEqualsMatcher.cpp
  SceneResourcesConfigEqualsMatcher.cpp
  AssetConfigEqualsMatcher.cpp
  
  # New matchers (Phase 2)
  EntityTransportEqualsMatcher.cpp
  
  # New matchers (Phase 3)
  SceneDataEqualsMatcher.cpp
  SceneCollectionDataEqualsMatcher.cpp
  
  # New matchers (Phase 4 - optional)
  SceneManagerStateEqualsMatcher.cpp
  SceneManagerDataEqualsMatcher.cpp
  
  # New matchers (Phase 5)
  EngineSnapshotEqualsMatcher.cpp
)
```

## Usage Examples

### Example 1: Basic EntityMemoryPool Comparison

```cpp
TEST_CASE("Compare entity state after TestEngine execution", "[integration]") {
  // Setup TestData
  TestData test_data = /* ... */;
  
  // Run TestEngine
  TestEngine engine(test_data);
  auto result = engine.StartUp();
  REQUIRE(result.has_value());
  engine.RunGameLoop();
  
  // Get snapshot from data bank
  const auto& data_bank = engine.GetDataBank();
  const EngineSnapshot& tick_1_snapshot = data_bank.at(1);
  
  // Expected snapshot
  EngineSnapshot expected;
  expected.scene_collection_data = /* ... */;
  
  // Compare (focuses on EntityMemoryPool via entity_transport)
  REQUIRE_THAT(tick_1_snapshot, EqualsEngineSnapshot(expected));
}
```

### Example 2: Selective Comparison with Optional Fields

```cpp
TEST_CASE("Compare only event bus state", "[unit]") {
  EngineSnapshot actual = /* from TestEngine */;
  
  // Expected only populates fields we care about
  EngineSnapshot expected;
  expected.tick_number = 1;
  expected.global_event_bus = EventBus{ /* expected events */ };
  // Note: scene_collection_data and scene_manager_data not set
  //       (but scene_collection_data is required, so set it to match or empty)
  expected.scene_collection_data = {}; // or matching actual
  
  REQUIRE_THAT(actual, EqualsEngineSnapshot(expected));
}
```

### Example 3: Multi-Scene Comparison

```cpp
TEST_CASE("Compare multiple scenes after logic execution", "[integration]") {
  // Setup multi-scene TestData
  TestData test_data = /* ... */;
  TestEngine engine(test_data);
  /* ... run engine ... */
  
  const EngineSnapshot& snapshot = engine.GetDataBank().at(1);
  
  // Expected state for multiple scenes
  EngineSnapshot expected;
  expected.scene_collection_data = {
    SceneData{ /* scene 0 expected */ },
    SceneData{ /* scene 1 expected */ },
    SceneData{ /* scene 2 expected */ }
  };
  
  // Matcher will compare each scene in order, reporting all mismatches
  REQUIRE_THAT(snapshot, EqualsEngineSnapshot(expected));
}
```

### Example 4: With TestContext for Better Error Messages

```cpp
TEST_CASE("Entity state comparison with context", "[integration]") {
  TestContext context{
    .test_name = "UI_interaction_updates_component_state",
    .description = "Verify UI click updates CUserInterface state",
    .current_tick = 5,
    .total_ticks = 10
  };
  
  EngineSnapshot actual = /* ... */;
  EngineSnapshot expected = /* ... */;
  
  // Error messages will include test name, description, and tick info
  REQUIRE_THAT(actual, EqualsEngineSnapshot(expected, context));
}
```

## Error Message Format

### Example Error Output

When comparing EngineSnapshots with mismatches:

```
❌ Test Failed: EngineSnapshot Match:
Test: "UI_interaction_updates_component_state"
Description: "Verify UI click updates CUserInterface state"
Tick: 5 / 10

Tick number mismatch: expected 5, got 4

Scene Collection Data:
  Scene [0] 550e8400-e29b-41d4-a716-446655440000:
    ❌ Test Failed: Entity [3] CUserInterface Match:
      ui_name mismatch:
        Expected: "main_menu"
        Actual:   "main_menu_clicked"
      
      start_visible mismatch:
        Expected: false
        Actual:   true
  
  Scene [1] 550e8400-e29b-41d4-a716-446655440001:
    ✓ TestPassed: All fields match
```

## Common Pitfalls and Solutions

### Pitfall 1: Variant Type Assumptions

**Problem**: Assuming entity_transport always holds EntityMemoryPool

**Solution**: Always check variant index before std::get. EntityTransportEqualsMatcher handles all variant types explicitly.

### Pitfall 2: Shared Pointer Dereferencing

**Problem**: Dereferencing null shared_ptr when comparing shared_ptr<EntityMemoryPool>

**Solution**: Null check before dereferencing in EntityTransportEqualsMatcher

### Pitfall 3: Optional Field Confusion

**Problem**: Comparing optional fields that weren't set in expected

**Solution**: EngineSnapshotEqualsMatcher only compares fields present in expected (has_value() check)

### Pitfall 4: Scene Order Sensitivity

**Problem**: Scenes in different order between expected and actual

**Solution**: SceneCollectionData is index-based. Document that scene order matters or implement UUID-based matching if needed.

### Pitfall 5: Incomplete Matcher Chain

**Problem**: Missing a matcher in the dependency chain

**Solution**: Follow the implementation order strictly, ensuring all dependencies exist before implementing dependent matchers.

## Future Enhancements

### Enhancement 1: UUID-Based Scene Matching

Current implementation compares scenes by index in the vector. Future enhancement could match scenes by UUID:

```cpp
// Current: Index-based
for (size_t i = 0; i < expected.size(); ++i) {
  compare(expected[i], actual[i]);
}

// Future: UUID-based
for (const auto& expected_scene : expected) {
  auto actual_scene = find_by_uuid(actual, expected_scene.scene_info.id);
  compare(expected_scene, actual_scene);
}
```

### Enhancement 2: Partial EntityMemoryPool Comparison

Allow specifying which entities or components to compare:

```cpp
EngineSnapshot expected;
expected.scene_collection_data = {
  SceneData{
    .entity_transport = EntityMemoryPool{
      // Only specify entities we care about
      // Matcher ignores other entities
    }
  }
};
```

### Enhancement 3: Difference Highlighting

Visual diff output for complex mismatches:

```
CUserInterface Difference:
  ui_name:        "menu" → "menu_clicked"
  start_visible:  false  → true
  active:         ✓ (both true)
  element count:  ✓ (both 5)
```

### Enhancement 4: Matcher Composition

Allow combining matchers with logic:

```cpp
REQUIRE_THAT(snapshot, 
  EqualsEngineSnapshot(expected) && 
  HasEventBusSize(0) &&
  AllScenesHaveEntityCount(10));
```

## Summary

This implementation plan provides a comprehensive roadmap for building EngineSnapshot matchers. Key points:

1. **Bottom-up approach**: Start with simple matchers, build to complex
2. **Critical focus**: EntityTransport matcher is key to EntityMemoryPool comparison
3. **Optional field handling**: EngineSnapshot supports selective comparison
4. **TestEngine integration**: Matchers designed around TestEngine's snapshot storage pattern
5. **Clear error messages**: All matchers provide detailed, actionable error output
6. **Comprehensive testing**: Each matcher has unit tests, integration tests validate the chain
7. **Future-proof**: Optional fields and variant handling allow for easy extension

The primary deliverable is the ability to write:

```cpp
REQUIRE_THAT(actual_snapshot, EqualsEngineSnapshot(expected_snapshot));
```

And get meaningful, detailed comparison results focusing on EntityMemoryPool state after TestEngine execution.
