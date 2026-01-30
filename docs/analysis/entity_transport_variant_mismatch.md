# Entity Transport Variant Type Mismatch Analysis

**Date**: 2026-01-30  
**Status**: Analysis Complete  
**Type**: Architecture Analysis

---

## Executive Summary

The TestEngine testing infrastructure has a type mismatch in how entity data is represented between expected (test data) and actual (runtime) snapshots. Expected snapshots use `const EntityCollectionFbs*` (FlatBuffers pointer) while actual snapshots use `EntityMemoryPool` (native C++ container). This prevents the `EntityTransportEqualsMatcher` from performing comparisons.

---

## Problem Description

### Current Behavior

When running test harness tests:
1. **Expected state** (from JSON/FlatBuffers): Entity data stored as `const EntityCollectionFbs*`
2. **Actual state** (from TestEngine runtime): Entity data stored as `EntityMemoryPool`
3. **Matcher fails** because variant indices don't match (can't compare different types)

### Impact

- Cannot validate TestEngine snapshots against expected data
- Test harness infrastructure incomplete
- Blocks data-driven testing for entity state

---

## Technical Details

### EntityTransportVariant Architecture

Located in: `src/types/core/EntityTransportVariant.h`

```cpp
using EntityTransportVariant = std::variant<
    std::monostate,                    // Index 0: Empty
    EntityMemoryPool,                  // Index 1: Value type
    std::shared_ptr<EntityMemoryPool>, // Index 2: Shared pointer
    const EntityCollectionFbs *        // Index 3: FlatBuffers pointer
>;
```

**Design Purpose**: Transport entity data in different formats depending on context
- FlatBuffers format: Serialized data from disk
- EntityMemoryPool: Runtime working format
- Shared pointer: For shared ownership scenarios

### Data Flow Analysis

#### Path 1: Loading Test Data (Expected State)

```
test_data.json
    ↓ (FlatBuffers compilation)
test_data.bin
    ↓ (FlatbuffersTestDataLoader)
TestDataFbs*
    ↓ (FlatbuffersTestDataProvider::CreateTestData)
TestData
    ├── starting_engine_snapshot: EngineSnapshot
    └── expected_engine_snapshots: map<size_t, EngineSnapshot>
            ↓ (ConfigureEngineSnapshot)
            EngineSnapshot
                ↓ (scene_collection_data)
                SceneData
                    ↓ (entity_transport)
                    const EntityCollectionFbs* ← **VARIANT INDEX 3**
```

**Critical Line**: `FlatbuffersSceneDataProvider.cpp:94`
```cpp
scene_data.entity_transport = m_scene_data_fbs->entity_collection();
```

#### Path 2: TestEngine Runtime (Actual State)

```
TestEngine::RunGameLoop()
    ↓ (each tick)
TestEngine::StoreEngineSnapShot()
    ↓
m_data_bank[tick] = EngineSnapshot
    ├── scene_collection_data
    └── SceneData
            ↓ (entity_transport)
            EntityMemoryPool ← **VARIANT INDEX 1**
```

**Critical Line**: `TestEngine.cpp:StoreEngineSnapShot()`
```cpp
// Copies EntityMemoryPool from scene into snapshot
scene_data.entity_transport = EntityMemoryPool{*scene.GetEntities()};
```

### Matcher Logic

Located in: `tests/matchers/EntityTransportEqualsMatcher.cpp`

```cpp
bool EntityTransportEqualsMatcher::match(
    const EntityTransportVariant &actual) const {
  
  // 1. Check variant indices match
  if (actual.index() != m_expected.index()) {  // ← FAILS HERE
    // Mismatch: actual=1 (EntityMemoryPool), expected=3 (EntityCollectionFbs*)
    return false;
  }
  
  // Never reaches deep comparison logic...
}
```

**Current Matcher Capabilities:**
- ✅ Compare `EntityMemoryPool` vs `EntityMemoryPool`
- ✅ Compare `shared_ptr<EntityMemoryPool>` vs `shared_ptr<EntityMemoryPool>`
- ✅ Compare `EntityCollectionFbs*` metadata (shallow)
- ❌ Compare `EntityMemoryPool` vs `EntityCollectionFbs*` (different indices)

---

## Root Cause

The architecture assumes entity data will remain in the same variant type throughout its lifecycle. However:

1. **Test data loading** preserves FlatBuffers pointers for efficiency
   - Avoids immediate conversion cost
   - Keeps original serialized format
   
2. **Runtime engine** works exclusively with EntityMemoryPool
   - Native C++ container for active entities
   - Modified during simulation

3. **No conversion bridge** exists between the two paths
   - Matcher expects same types
   - No cross-type comparison implemented

---

## Proposed Solutions

### Option 1: Convert at Load Time ⭐ **RECOMMENDED**

**Strategy**: Convert `EntityCollectionFbs*` to `EntityMemoryPool` when loading test data

**Implementation Approach:**

1. **Modify**: `src/data_providers/configure/configure_engine_snapshot.cpp`
   - After creating SceneData from FlatBuffers
   - Use existing `entity_configurator` to import entities
   - Replace FlatBuffers pointer with EntityMemoryPool

```cpp
// In ConfigureEngineSnapshot, after line 93:
SceneData scene_data;
auto configure_result = scene_provider.ConfigureSceneData(scene_data);
if (!configure_result.has_value()) {
  return std::unexpected(configure_result.error());
}

// NEW: Convert entity_transport from FlatBuffers to EntityMemoryPool
if (std::holds_alternative<const EntityCollectionFbs*>(scene_data.entity_transport)) {
  const auto* entity_collection_fbs = 
      std::get<const EntityCollectionFbs*>(scene_data.entity_transport);
  
  // Import entities using the configurator
  auto pool_result = scene_data.entity_configurator->ImportEntities(
      entity_collection_fbs);
  if (!pool_result.has_value()) {
    return std::unexpected(pool_result.error());
  }
  
  // Replace variant with EntityMemoryPool
  scene_data.entity_transport = std::move(pool_result.value());
}

snapshot.scene_collection_data.push_back(std::move(scene_data));
```

**Pros:**
- ✅ Unified data format throughout testing
- ✅ Leverages existing entity import infrastructure
- ✅ Simple, clean comparison using existing EntityMemoryPool matcher
- ✅ Validates entity import as part of test data loading
- ✅ No changes needed to matcher logic

**Cons:**
- ❌ Slightly higher memory usage (EntityMemoryPool larger than pointer)
- ❌ Conversion cost at load time (acceptable for testing)
- ❌ Loses reference to original FlatBuffers data

**Estimated Effort**: 2-4 hours
- Implement conversion logic
- Test with existing harness tests
- Verify memory impact acceptable

---

### Option 2: Add Cross-Type Comparison to Matcher

**Strategy**: Enhance matcher to compare EntityMemoryPool against EntityCollectionFbs*

**Implementation Approach:**

1. **Modify**: `tests/matchers/EntityTransportEqualsMatcher.cpp`
   - Add cross-type comparison logic
   - Import FlatBuffers on-demand for comparison

```cpp
bool EntityTransportEqualsMatcher::match(
    const EntityTransportVariant &actual) const {
  
  // Handle cross-type comparisons
  if (actual.index() != m_expected.index()) {
    return HandleCrossTypeComparison(actual, m_expected);
  }
  // ... existing logic
}

bool HandleCrossTypeComparison(
    const EntityTransportVariant &actual,
    const EntityTransportVariant &expected) const {
  
  // Case 1: actual=EntityMemoryPool, expected=EntityCollectionFbs*
  if (std::holds_alternative<EntityMemoryPool>(actual) &&
      std::holds_alternative<const EntityCollectionFbs*>(expected)) {
    
    // Import FlatBuffers to EntityMemoryPool for comparison
    const auto* fbs_ptr = std::get<const EntityCollectionFbs*>(expected);
    FlatbuffersEntityImporter importer;
    auto expected_pool_result = importer.ImportEntities(fbs_ptr);
    
    if (!expected_pool_result.has_value()) {
      m_mismatch_description = "Failed to import expected entities";
      return false;
    }
    
    // Now compare two EntityMemoryPools
    const auto& actual_pool = std::get<EntityMemoryPool>(actual);
    EntityMemoryPoolEqualsMatcher pool_matcher(expected_pool_result.value());
    return pool_matcher.match(actual_pool);
  }
  
  // Case 2: actual=EntityCollectionFbs*, expected=EntityMemoryPool
  // (symmetric case)
  
  return false; // Other combinations not supported
}
```

**Pros:**
- ✅ Preserves original FlatBuffers data
- ✅ No changes to data loading pipeline
- ✅ Tests entity import as part of comparison

**Cons:**
- ❌ Complex matcher logic
- ❌ Need entity configurator/importer in matcher (dependency injection)
- ❌ Performance: Import on every comparison
- ❌ Error handling complexity (import failures during comparison)

**Estimated Effort**: 4-6 hours
- Implement cross-type logic
- Handle entity configurator dependency
- Test all comparison paths
- Handle error cases

---

### Option 3: Hybrid - Store Both Formats

**Strategy**: Maintain both FlatBuffers pointer and EntityMemoryPool in snapshot

**Implementation Approach:**

1. **Modify**: `EngineSnapshot` struct
   - Add optional EntityMemoryPool field alongside transport variant

2. **Or**: Extend EntityTransportVariant
   - Add new variant type that holds both

**Pros:**
- ✅ Maximum flexibility
- ✅ Can verify both formats match
- ✅ Preserves all data

**Cons:**
- ❌ Most complex implementation
- ❌ Memory overhead (storing both formats)
- ❌ Architectural complexity
- ❌ Likely over-engineered for the use case

**Estimated Effort**: 6-8 hours
**Recommendation**: Not worth the complexity

---

## Decision Matrix

| Criteria | Option 1 (Convert at Load) | Option 2 (Cross-Type Matcher) | Option 3 (Hybrid) |
|----------|----------------------------|-------------------------------|-------------------|
| Implementation Complexity | ⭐⭐⭐⭐⭐ Simple | ⭐⭐⭐ Complex | ⭐ Very Complex |
| Performance | ⭐⭐⭐⭐ Good | ⭐⭐⭐ Fair | ⭐⭐ Poor |
| Maintainability | ⭐⭐⭐⭐⭐ Excellent | ⭐⭐⭐ Fair | ⭐⭐ Poor |
| Memory Usage | ⭐⭐⭐⭐ Good | ⭐⭐⭐⭐⭐ Excellent | ⭐⭐ Poor |
| Architectural Fit | ⭐⭐⭐⭐⭐ Natural | ⭐⭐⭐ Acceptable | ⭐⭐ Awkward |
| Test Coverage | ⭐⭐⭐⭐ Good | ⭐⭐⭐⭐⭐ Excellent | ⭐⭐⭐ Fair |

**Winner**: Option 1 (Convert at Load Time)

---

## Recommendation

**Implement Option 1: Convert FlatBuffers to EntityMemoryPool at load time**

### Rationale

1. **Simplicity**: Cleanest solution with minimal code changes
2. **Consistency**: Unified data format throughout testing infrastructure
3. **Maintainability**: Easy to understand and debug
4. **Proven Pattern**: Leverages existing entity import infrastructure
5. **Performance**: One-time conversion cost is acceptable for testing

### Implementation Plan

#### Phase 1: Core Conversion (2 hours)
- Modify `configure_engine_snapshot.cpp`
- Add entity import after SceneData configuration
- Replace FlatBuffers pointer with EntityMemoryPool in variant

#### Phase 2: Testing (1 hour)
- Run existing harness tests
- Verify matcher now works correctly
- Check memory usage is acceptable

#### Phase 3: Validation (1 hour)
- Test with multiple scenarios
- Verify all entity types handled correctly
- Ensure error handling robust

**Total Estimated Time**: 4 hours

### Alternative Path

If Option 1 proves problematic (e.g., unexpected issues with entity import), Option 2 (cross-type comparison) is a viable fallback with more implementation complexity.

---

## Related Files

### Core Types
- `src/types/core/EntityTransportVariant.h` - Variant definition
- `src/types/core/EngineSnapshot.h` - Snapshot container
- `src/types/core/SceneData.h` - Scene data with entity_transport

### Data Providers
- `src/data_providers/FlatbuffersSceneDataProvider.cpp` - Scene data loading
- `src/data_providers/configure/configure_engine_snapshot.cpp` - Snapshot configuration

### Testing Infrastructure
- `tests/harness/TestEngine.cpp` - Runtime snapshot creation
- `tests/matchers/EntityTransportEqualsMatcher.cpp` - Variant comparison
- `tests/matchers/EntityMemoryPoolEqualsMatcher.cpp` - Pool comparison

### Schemas
- `src/types/flatbuffers/testing/test_data.fbs` - Test data schema
- `src/types/flatbuffers/core/engine_snapshot.fbs` - Engine snapshot schema

---

## Questions & Considerations

### Q: Why not always use EntityMemoryPool?
**A**: EntityTransportVariant supports multiple formats because:
- Serialization: FlatBuffers pointer for disk I/O
- Runtime: EntityMemoryPool for active game state
- Flexibility: Different ownership models (value, shared_ptr)

### Q: Will conversion impact test performance?
**A**: Minimal impact:
- Conversion happens once at test data load
- Test data is typically small (< 100 entities)
- Alternative (Option 2) would convert on every comparison
- Acceptable tradeoff for testing infrastructure

### Q: What about entity import errors?
**A**: Existing error handling:
- Entity configurators return `std::expected`
- Errors propagate up to test harness
- Tests fail with clear error messages
- Same error handling as normal entity loading

### Q: Does this validate entity import?
**A**: Yes, as a beneficial side effect:
- Entity import runs as part of test data loading
- Validates FlatBuffers schema compatibility
- Catches entity configuration issues early
- More thorough testing overall

---

## Conclusion

The entity transport variant type mismatch is well-understood and has a clear solution path. **Option 1 (convert at load time)** is recommended for its simplicity, maintainability, and consistency with the existing architecture.

This analysis provides sufficient information for implementation. The fix is straightforward and will enable proper testing of the TestEngine infrastructure.

---

**Next Steps**:
1. Review and approve this analysis
2. Implement Option 1 (or provide feedback for alternative)
3. Test thoroughly with existing harness tests
4. Update documentation with new data flow

**Analysis Status**: ✅ Complete
