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

**Convert FlatBuffers to EntityMemoryPool when loading test data**

### Where to Fix
File: `src/data_providers/configure/configure_engine_snapshot.cpp`
Location: After `scene_provider.ConfigureSceneData(scene_data)` (around line 93)

### What to Add
```cpp
// Convert entity_transport to EntityMemoryPool format
if (std::holds_alternative<const EntityCollectionFbs*>(scene_data.entity_transport)) {
  const auto* entity_collection_fbs = 
      std::get<const EntityCollectionFbs*>(scene_data.entity_transport);
  
  auto pool_result = scene_data.entity_configurator->ImportEntities(
      entity_collection_fbs);
  if (!pool_result.has_value()) {
    return std::unexpected(pool_result.error());
  }
  
  scene_data.entity_transport = std::move(pool_result.value());
}
```

## Why This Solution

✅ Simple - Only ~10 lines of code
✅ Clean - Uses existing entity import logic
✅ Consistent - Everything uses EntityMemoryPool
✅ No matcher changes needed

## Time Estimate

- Implementation: 2 hours
- Testing: 1 hour
- Validation: 1 hour
- **Total: 4 hours**

## Alternative Solutions

### Option 2: Add Cross-Type Comparison
- Modify the matcher to handle different types
- More complex (~6 hours)
- Keeps FlatBuffers format in test data

### Option 3: Store Both Formats
- Keep both FlatBuffers and EntityMemoryPool
- Over-engineered (~8 hours)
- Not recommended

## Key Files

- `src/types/core/EntityTransportVariant.h` - The variant type definition
- `src/data_providers/configure/configure_engine_snapshot.cpp` - Where to add fix
- `tests/matchers/EntityTransportEqualsMatcher.cpp` - The matcher that fails
- `tests/harness/TestEngine.cpp` - Creates runtime snapshots

## See Also

- Full analysis: `docs/analysis/entity_transport_variant_mismatch.md`
- Test harness README: `tests/harness/README.md`

## Status

✅ **Analysis Complete** - Ready for implementation decision
