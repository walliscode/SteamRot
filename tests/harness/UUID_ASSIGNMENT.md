# UUID Assignment for TestData Engine Snapshots

## Overview

This document describes the automatic UUID assignment system for scenes in TestData engine snapshots. The system ensures that scenes across multiple snapshots (starting and expected) maintain consistent UUIDs without requiring manual UUID entry in JSON files.

## Problem Statement

- **Scenes need UUIDs**: Every scene in the engine must have a unique identifier (UUID) for proper tracking and management
- **UUIDs are not human-friendly**: Manual UUID entry in test_data.json files is error-prone and makes tests harder to write
- **Consistency is critical**: The same scene must have the same UUID across all snapshots in a single test

## Solution

The UUID assignment system automatically:
1. Generates UUIDs for scenes in the starting snapshot that don't have one
2. Propagates those UUIDs to all expected snapshots based on SceneType matching
3. Validates that manually provided UUIDs are consistent across snapshots

## Architecture

### Components

#### 1. UUIDAssignmentTracker
**Location**: `tests/harness/UUIDAssignmentTracker.h/cpp`

Manages UUID assignment and tracking across snapshots:
- Maps SceneType to UUID
- Generates new UUIDs when needed
- Ensures consistency across starting and expected snapshots

**Key Methods**:
- `ProcessStartingSceneUUID()`: Handles UUID assignment for starting snapshot scenes
- `GetOrAssignUUID()`: Retrieves or generates UUID for expected snapshot scenes
- `HasUUID()`: Checks if a SceneType has been assigned a UUID
- `GetUUID()`: Retrieves UUID for a SceneType if it exists

#### 2. ConfigureEngineSnapshotWithUUIDs
**Location**: `tests/harness/configure_test_engine_snapshot.h/cpp`

Configures EngineSnapshot from FlatBuffers data with UUID tracking:
- Wraps standard EngineSnapshot configuration
- Integrates UUIDAssignmentTracker for consistent UUID assignment
- Validates UUID consistency between snapshots

#### 3. FlatBuffers Schema Extension
**Location**: `src/types/flatbuffers/testing/test_data.fbs`

Added support for expected engine snapshots:
```fbs
table TickSnapshotPair {
  tick: uint64 (required);
  snapshot: EngineSnapshotFbs (required);
}

table TestDataFbs {
  // ... existing fields ...
  expected_engine_snapshots: [TickSnapshotPair];
}
```

## Workflow

### 1. Loading TestData

When `FlatbuffersTestDataProvider::CreateTestData()` is called:

```cpp
// Create UUID tracker for this TestData
UUIDAssignmentTracker uuid_tracker;

// Configure starting snapshot (assigns UUIDs)
ConfigureEngineSnapshotWithUUIDs(
    starting_snapshot, 
    fb_starting_snapshot,
    event_handler,
    uuid_tracker,
    true  // is_starting_snapshot = true
);

// Configure expected snapshots (propagates UUIDs)
for (each expected snapshot) {
    ConfigureEngineSnapshotWithUUIDs(
        expected_snapshot,
        fb_expected_snapshot,
        event_handler,
        uuid_tracker,
        false  // is_starting_snapshot = false
    );
}
```

### 2. Starting Snapshot Processing

For each scene in the starting snapshot:

1. **Check for existing UUID**: If scene_id is provided in JSON and is valid, use it
2. **Generate if missing**: If no UUID, generate a new one
3. **Track the mapping**: Store SceneType → UUID mapping in the tracker
4. **Assign to scene**: Set the UUID on the SceneInfo

### 3. Expected Snapshot Processing

For each scene in expected snapshots:

1. **Lookup UUID**: Get the UUID for this SceneType from the tracker
2. **Validate if provided**: If scene has a UUID and it doesn't match, raise error
3. **Assign from tracker**: Use the UUID from the starting snapshot
4. **Maintain consistency**: Same SceneType = Same UUID across all snapshots

## Usage in Test Data

### Basic Example (No UUIDs in JSON)

```json
{
  "meta_data": { ... },
  "starting_engine_snapshot": {
    "scene_collection_data": {
      "scene_data": [
        {
          "scene_info": {
            "scene_type": "TITLE"
            // No scene_id - UUID will be auto-generated
          },
          // ... rest of scene config ...
        }
      ]
    }
  },
  "expected_engine_snapshots": [
    {
      "tick": 1,
      "snapshot": {
        "scene_collection_data": {
          "scene_data": [
            {
              "scene_info": {
                "scene_type": "TITLE"
                // Same SceneType = will get same UUID as starting snapshot
              },
              // ... expected state ...
            }
          ]
        }
      }
    }
  ]
}
```

### Advanced Example (Explicit UUID)

```json
{
  "starting_engine_snapshot": {
    "scene_collection_data": {
      "scene_data": [
        {
          "scene_info": {
            "scene_type": "TITLE",
            "scene_id": "550e8400-e29b-41d4-a716-446655440000"
            // Explicit UUID will be used and validated
          }
        }
      ]
    }
  },
  "expected_engine_snapshots": [
    {
      "tick": 1,
      "snapshot": {
        "scene_collection_data": {
          "scene_data": [
            {
              "scene_info": {
                "scene_type": "TITLE"
                // Must have same UUID or no UUID
                // "scene_id": "550e8400-e29b-41d4-a716-446655440000" // Optional - will be validated if present
              }
            }
          ]
        }
      }
    }
  ]
}
```

## UUID Assignment Rules

### Rule 1: SceneType-Based Matching
- UUIDs are assigned per SceneType, not per scene instance
- All scenes with the same SceneType in a single TestData get the same UUID
- Different SceneTypes get different UUIDs

### Rule 2: Starting Snapshot Priority
- Starting snapshot establishes the UUID mapping
- Expected snapshots inherit UUIDs from the starting snapshot
- A scene in an expected snapshot cannot have a different UUID than its starting snapshot counterpart

### Rule 3: Optional Explicit UUIDs
- scene_id can be provided in JSON for explicit UUID control
- If provided in starting snapshot, it's used instead of generating
- If provided in expected snapshot, it must match the starting snapshot's UUID

### Rule 4: Generation Strategy
- UUIDs are generated using `uuids::uuid_system_generator`
- Generated UUIDs are version 1 (time-based) by default
- Each TestData has its own UUIDAssignmentTracker (UUIDs are not shared across different tests)

## Error Handling

### Invalid UUID String
```
FailMode: InvalidUUID
Message: "Scene ID in SceneInfoFbs is not a valid UUID string"
```
Occurs when scene_id is provided but is not a valid UUID format.

### Mismatched UUID
```
FailMode: InvalidUUID
Message: "Expected snapshot scene UUID doesn't match starting snapshot UUID for same SceneType"
```
Occurs when an expected snapshot has an explicit UUID that differs from the starting snapshot for the same SceneType.

### Null FlatBuffers Data
```
FailMode: FlatbuffersDataNotFound
Message: "EngineSnapshotFbs is null"
```
Occurs when snapshot data is missing or null.

## Testing

### Unit Tests
**Location**: `tests/unit/harness/UUIDAssignmentTracker.test.cpp`

Tests for UUIDAssignmentTracker functionality:
- UUID generation for new scene types
- UUID preservation for existing UUIDs
- Consistency across multiple calls
- Multiple scene type handling

### Integration Tests
**Location**: `tests/unit/harness/uuid_assignment_integration.test.cpp`

End-to-end tests for the complete workflow:
- Loading TestData with auto-generated UUIDs
- UUID propagation from starting to expected snapshots
- UUID validation and error cases

### Test Data
**Location**: `tests/unit/harness/data/uuid_assignment.test_data.json`

Example test data demonstrating UUID assignment.

## Benefits

1. **Simplified Test Authoring**: No need to manually create and manage UUIDs in JSON files
2. **Consistency Guaranteed**: UUIDs are automatically consistent across snapshots
3. **Error Prevention**: Eliminates copy-paste errors with UUIDs
4. **Flexibility**: Supports both auto-generated and explicit UUIDs
5. **Validation**: Detects and reports UUID mismatches early

## Future Enhancements

Potential improvements to consider:

1. **Deterministic UUID Generation**: Use seed-based generation for reproducible test UUIDs
2. **Scene Instance Tracking**: Support multiple instances of the same SceneType with different UUIDs
3. **UUID Mapping Export**: Export UUID mappings for debugging and documentation
4. **Cross-Test UUID Sharing**: Allow UUID reuse across related test cases (use with caution)

## See Also

- [TestData Configuration Documentation](../../documentation/testing/TEST_DATA_CONFIGURATION.md)
- [Test Data Naming Conventions](../../documentation/testing/TEST_DATA_NAMING_CONVENTIONS.md)
- [FlatBuffers Schema](../../src/types/flatbuffers/testing/test_data.fbs)
