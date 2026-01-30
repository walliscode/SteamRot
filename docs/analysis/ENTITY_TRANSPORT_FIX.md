# Entity Transport Type Mismatch - Quick Reference

## The Problem in Simple Terms

When testing the game engine:
- **Expected data** (from JSON files): Entities stored as FlatBuffers pointers
- **Actual data** (from running engine): Entities stored as EntityMemoryPool objects
- **Result**: Cannot compare them because they're different types

## Why This Happens

```
Test Data Path:              Engine Runtime Path:
JSON → FlatBuffers          Scene runs logic
  ↓                            ↓
Stays as pointer            Creates EntityMemoryPool
  ↓                            ↓
Expected: EntityCollectionFbs*   Actual: EntityMemoryPool
```

Comparison fails because variant types don't match (index 3 vs index 1).

## The Solution (Recommended)

**Convert FlatBuffers to EntityMemoryPool after TestEngine runs, in harness runner**

### Where to Fix
File: `tests/harness/harness_runner.cpp`
Location: After `test_engine.RunGame()` completes (after line 73)

### What to Add
```cpp
// After TestEngine runs, convert expected snapshots to EntityMemoryPool format
// This allows comparison without interfering with how EngineSnapshots are generated
for (auto &[tick, expected_snapshot] : test_data.expected_engine_snapshots) {
  for (auto &scene_data : expected_snapshot.scene_collection_data) {
    if (std::holds_alternative<const EntityCollectionFbs*>(
            scene_data.entity_transport)) {
      const auto* entity_collection_fbs = 
          std::get<const EntityCollectionFbs*>(scene_data.entity_transport);
      
      auto pool_result = scene_data.entity_configurator->ImportEntities(
          entity_collection_fbs);
      if (!pool_result.has_value()) {
        return std::unexpected(pool_result.error());
      }
      
      scene_data.entity_transport = std::move(pool_result.value());
    }
  }
}

// Also convert starting_engine_snapshot if needed for comparison
for (auto &scene_data : test_data.starting_engine_snapshot.scene_collection_data) {
  if (std::holds_alternative<const EntityCollectionFbs*>(
          scene_data.entity_transport)) {
    const auto* entity_collection_fbs = 
        std::get<const EntityCollectionFbs*>(scene_data.entity_transport);
    
    auto pool_result = scene_data.entity_configurator->ImportEntities(
        entity_collection_fbs);
    if (!pool_result.has_value()) {
      return std::unexpected(pool_result.error());
    }
    
    scene_data.entity_transport = std::move(pool_result.value());
  }
}
```

## Why This Solution

✅ Simple - Conversion happens in one place
✅ Clean - Uses existing entity import logic
✅ Non-invasive - Doesn't interfere with EngineSnapshot generation
✅ Isolated - Only affects test comparison, not runtime
✅ No matcher changes needed

## Time Estimate

- Implementation: 2 hours
- Testing: 1 hour
- Validation: 1 hour
- **Total: 4 hours**

## Alternative Solutions

### Option 2: Convert at Load Time
- Modify data provider to convert during EngineSnapshot loading
- More invasive (~4 hours)
- Affects EngineSnapshot generation behavior

### Option 3: Add Cross-Type Comparison
- Modify the matcher to handle different types
- More complex (~6 hours)
- Adds complexity to matcher

### Option 4: Store Both Formats
- Keep both FlatBuffers and EntityMemoryPool
- Over-engineered (~8 hours)
- Not recommended

## Key Files

- `src/types/core/EntityTransportVariant.h` - The variant type definition
- `tests/harness/harness_runner.cpp` - Where to add fix (after TestEngine runs)
- `tests/matchers/EntityTransportEqualsMatcher.cpp` - The matcher that fails
- `tests/harness/TestEngine.cpp` - Creates runtime snapshots

## See Also

- Full analysis: `docs/analysis/entity_transport_variant_mismatch.md`
- Test harness README: `tests/harness/README.md`

## Status

✅ **Analysis Complete** - Ready for implementation decision
