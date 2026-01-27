# UUID Assignment Implementation Summary

## What Was Implemented

This PR implements automatic UUID assignment for scenes in TestData engine snapshots, solving the problem of manual UUID management in test_data.json files.

### Problem Solved
- **Before**: Test authors had to manually add UUIDs to each scene in test_data.json files, which is error-prone and not user-friendly
- **After**: UUIDs are automatically assigned to scenes in the starting snapshot and propagated to expected snapshots based on SceneType

## Key Changes

### 1. FlatBuffers Schema Extension
**File**: `src/types/flatbuffers/testing/test_data.fbs`
- Added `TickSnapshotPair` table to map tick numbers to expected snapshots
- Added `expected_engine_snapshots` field to `TestDataFbs`

### 2. UUID Assignment Infrastructure
**Files**: `tests/harness/UUIDAssignmentTracker.{h,cpp}`
- New class that manages UUID assignment and tracking
- Maps SceneType to UUID
- Ensures consistency across starting and expected snapshots

### 3. Snapshot Configuration
**Files**: `tests/harness/configure_test_engine_snapshot.{h,cpp}`
- New function `ConfigureEngineSnapshotWithUUIDs()` that integrates UUID tracking
- Processes starting snapshot to assign/record UUIDs
- Processes expected snapshots to propagate UUIDs

### 4. Provider Integration
**File**: `tests/harness/FlatbuffersTestDataProvider.cpp`
- Updated `CreateTestData()` to use UUID-aware configuration
- Creates UUIDAssignmentTracker for each TestData
- Configures both starting and expected snapshots with UUID tracking

### 5. Tests
**Files**: 
- `tests/unit/harness/UUIDAssignmentTracker.test.cpp` - Unit tests
- `tests/unit/harness/uuid_assignment_integration.test.cpp` - Integration tests
- `tests/unit/harness/data/uuid_assignment.test_data.json` - Example test data

### 6. Documentation
**File**: `tests/harness/UUID_ASSIGNMENT.md`
- Complete documentation of the UUID assignment system
- Architecture overview
- Usage examples
- Error handling guide

## How to Test

### Step 1: Build the Project
```bash
# The FlatBuffers schema needs to be compiled
cmake --preset Debug
cmake --build --preset Debug
```

### Step 2: Run Tests
```bash
# Run all harness tests
ctest --preset Debug -R test_harness

# Run specific UUID tests
ctest --preset Debug -R test_harness -V | grep -i uuid
```

### Step 3: Verify the Feature

The tests validate:
1. ✅ UUIDs are auto-generated for scenes without explicit IDs
2. ✅ UUIDs are preserved when explicitly provided
3. ✅ UUIDs are propagated from starting to expected snapshots
4. ✅ UUID mismatches are detected and reported
5. ✅ Multiple scenes with different SceneTypes get different UUIDs

## Example Usage

### Before (Manual UUID Management - Still Supported)
```json
{
  "starting_engine_snapshot": {
    "scene_collection_data": {
      "scene_data": [{
        "scene_info": {
          "scene_type": "TITLE",
          "scene_id": "550e8400-e29b-41d4-a716-446655440000"  // Manual UUID
        }
      }]
    }
  }
}
```

### After (Automatic UUID Assignment - Recommended)
```json
{
  "starting_engine_snapshot": {
    "scene_collection_data": {
      "scene_data": [{
        "scene_info": {
          "scene_type": "TITLE"
          // No scene_id needed - UUID auto-generated!
        }
      }]
    }
  },
  "expected_engine_snapshots": [{
    "tick": 1,
    "snapshot": {
      "scene_collection_data": {
        "scene_data": [{
          "scene_info": {
            "scene_type": "TITLE"
            // Same UUID as starting snapshot automatically!
          }
        }]
      }
    }
  }]
}
```

## Benefits

1. **Easier Test Authoring**: No manual UUID management
2. **Fewer Errors**: Eliminates UUID copy-paste mistakes
3. **Consistency**: Automatic UUID propagation across snapshots
4. **Validation**: Detects UUID mismatches early
5. **Flexibility**: Supports both auto-generated and explicit UUIDs

## Architecture Overview

```
FlatbuffersTestDataProvider::CreateTestData()
    |
    ├─> Create UUIDAssignmentTracker
    |
    ├─> ConfigureEngineSnapshotWithUUIDs(starting_snapshot, tracker, true)
    |       |
    |       ├─> For each scene in starting snapshot:
    |       |   ├─> Check if scene has explicit UUID
    |       |   ├─> Generate UUID if not present
    |       |   └─> Track SceneType -> UUID mapping
    |       |
    |       └─> Assign UUIDs to scenes
    |
    └─> For each expected snapshot:
            ConfigureEngineSnapshotWithUUIDs(expected_snapshot, tracker, false)
                |
                └─> For each scene in expected snapshot:
                    ├─> Get UUID from tracker for this SceneType
                    ├─> Validate if scene has explicit UUID
                    └─> Assign UUID from starting snapshot
```

## Files Modified/Added

### Modified Files
- `src/types/flatbuffers/testing/test_data.fbs`
- `tests/harness/CMakeLists.txt`
- `tests/harness/FlatbuffersTestDataProvider.cpp`
- `tests/unit/harness/CMakeLists.txt`

### New Files
- `tests/harness/UUIDAssignmentTracker.h`
- `tests/harness/UUIDAssignmentTracker.cpp`
- `tests/harness/configure_test_engine_snapshot.h`
- `tests/harness/configure_test_engine_snapshot.cpp`
- `tests/harness/UUID_ASSIGNMENT.md`
- `tests/unit/harness/UUIDAssignmentTracker.test.cpp`
- `tests/unit/harness/uuid_assignment_integration.test.cpp`
- `tests/unit/harness/data/uuid_assignment.test_data.json`

## Next Steps

1. **Build the project** to compile FlatBuffers schemas
2. **Run tests** to verify functionality
3. **Review the documentation** in `UUID_ASSIGNMENT.md`
4. **Update existing test data** if desired (optional - they'll still work with explicit UUIDs)

## Questions?

See the complete documentation in:
- `tests/harness/UUID_ASSIGNMENT.md` - Detailed technical documentation
- Test files for usage examples
