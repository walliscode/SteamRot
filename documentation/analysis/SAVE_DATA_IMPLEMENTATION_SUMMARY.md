# SaveData Provider System - Implementation Summary

## Executive Summary

This document provides a comprehensive overview of the SaveData provider system implementation for the SteamRot game engine. The system enables loading game state from save files and using save data in data-driven tests.

**Status**: Phase 1 & 2 Complete (Core implementation ready for use)

**Date**: 2024-12-22

## Problem Statement

The original problem statement from the issue was:

> I now want to implement loading data from Save Data
> - I was thinking of building on the ISceneDataProvider system
> - so potentially implementing an ISaveDataProvider. There would be a concrete implementation for each data type we are loading from
> - there would be a native SaveData struct. this contain pointers to structs that need polymorphism such as SceneData and then other cheap copyable data bits.
> - would we have the ISceneDataProvider be able to take in the SaveData and pull out the bits it needs?
> - thinking ahead, for the test engine and data driven tests we would want the TestData to contain an instance of SaveData so we can configure the starting state using existing functions

## Solution Overview

### Architecture

The implementation follows the existing provider pattern in SteamRot and provides:

1. **Native SaveData struct** - C++ representation containing:
   - `SaveMetadata` (by value)
   - `std::unique_ptr<SceneData>` (polymorphic)
   - `EngineState` (by value)
   - `SceneManagerState` (by value)
   - `SceneState` (by value)

2. **ISaveDataProvider interface** - Abstract interface with methods:
   - `LoadSaveData(slot_index)` - Load complete save
   - `SaveExists(slot_index)` - Check if save exists
   - `GetSaveMetadata(slot_index)` - Load metadata only

3. **FlatbuffersSaveDataProvider** - Concrete implementation loading from FlatBuffers binary files

4. **Extended ISceneDataProvider** - New method:
   - `ProvideSceneDataFromSave(SaveData&)` - Extract scene data from save

5. **TestData integration** - Extended `TestDataConfig` schema:
   - Optional `save_data` field for test configuration
   - Priority over `starting_engine_state` when present

## Implementation Details

### File Structure

```
src/
├── types/
│   ├── core/
│   │   ├── SaveData.h             # Native SaveData struct
│   │   └── SaveMetadata.h         # Native SaveMetadata struct
│   └── flatbuffers/
│       ├── configuration/
│       │   └── save_data.fbs      # FlatBuffers schema (extended)
│       └── testing/
│           └── test_data.fbs      # Extended with save_data field
├── interfaces/
│   ├── ISaveDataProvider.h        # SaveData provider interface
│   └── ISceneDataProvider.h       # Extended with new method
└── data_providers/
    ├── FlatbuffersSaveDataProvider.h/.cpp  # Concrete implementation
    ├── FlatbuffersSceneDataProvider.h/.cpp # Extended implementation
    └── provider_factory.h/.cpp             # Added GetSaveDataProvider()

tests/
├── data/user/saves/
│   └── save_slot_0.json           # Test save file
└── unit/data_providers/
    ├── FlatbuffersSaveDataProvider.test.cpp     # Unit tests
    └── FlatbuffersSceneDataProvider.test.cpp    # Extended tests

documentation/
├── analysis/
│   └── SAVE_DATA_PROVIDER_DESIGN.md    # Architecture design doc
└── workflows/
    └── USING_SAVE_DATA.md              # User guide
```

### Key Design Decisions

#### 1. Polymorphic SceneData via unique_ptr

**Decision**: Store `SceneData` as `std::unique_ptr<SceneData>` in `SaveData`

**Rationale**:
- Supports multiple SceneData implementations (FbsSceneData, JsonSceneData, etc.)
- Enables ownership transfer via `std::move()`
- Matches existing pattern in `ISceneDataProvider`

**Trade-offs**:
- Requires dynamic_cast for concrete type access
- Single ownership model (can't share SceneData)

#### 2. Value-type for Simple State Structs

**Decision**: Store `EngineState`, `SceneManagerState`, `SceneState` by value

**Rationale**:
- These are simple, copyable structs
- No polymorphism needed
- More efficient than heap allocation
- Simpler lifetime management

#### 3. Separate ISaveDataProvider Interface

**Decision**: Create new interface rather than extending ISceneDataProvider

**Rationale**:
- Clear separation of concerns
- Single Responsibility Principle
- Easier to test independently
- Can be implemented without SceneData dependency

#### 4. ISceneDataProvider Extension

**Decision**: Add `ProvideSceneDataFromSave()` method to existing interface

**Rationale**:
- ISceneDataProvider is the natural home for scene data provision
- Enables using SaveData to configure scenes
- Maintains consistency with existing `ProvideDefaultSceneData()` method

**Alternative considered**: Separate ISaveSceneDataProvider interface
- Rejected: Too granular, would create interface proliferation

#### 5. TestData Integration

**Decision**: Add optional `save_data` field to `TestDataConfig`

**Rationale**:
- Allows tests to start from complete saved game state
- Reuses existing save file format
- No need for separate test-only configuration format

**Priority rule**: `save_data` takes precedence over `starting_engine_state`

#### 6. Save File Extension

**Decision**: Use `.save` extension instead of `.bin`

**Rationale**:
- Clearly distinguishes save files from other FlatBuffers binaries
- Makes file purpose obvious
- Prevents accidental confusion with scene data or config files

### FlatBuffers Schema Extensions

#### save_data.fbs (Complete Schema)

```fbs
table SaveMetadata {
  save_name: string;
  created_at: string;
  last_modified: string;
  game_version: string;
  play_time_seconds: uint64 = 0;
  slot_index: uint32 = 0;
}

table SceneManagerStateFbs {
  subscriptions: [SubscriberFbs];
}

table SceneStateFbs {
  active: bool = true;
}

table SaveData {
  metadata: SaveMetadata (required);
  scene_data: SceneDataFbs (required);
  engine_state: EngineStateFbs;
  scene_manager_state: SceneManagerStateFbs;
  scene_state: SceneStateFbs;
}
```

**Key points**:
- `metadata` and `scene_data` are required
- Other fields are optional for backward compatibility
- Reuses existing `SceneDataFbs`, `EngineStateFbs` schemas

#### test_data.fbs Extension

```fbs
table TestDataConfig {
  metadata: TestMetadata (required);
  starting_engine_state: EngineStateFbs;
  save_data: SaveData;  // NEW: Optional complete save state
  simulation_data: SimulationData;
  num_ticks: uint32;
  tick_snapshots: [TickSnapshot];
}
```

### Build System Integration

#### CMake Changes

**`convert_json_to_binary.cmake`**:

1. Added `USER_DIRECTORY_ROOTS` for user-writable data
2. Created `flatbuffers_generate_for_user_type` macro
3. Added generation for save files with `.save` extension

**Result**: Save JSON files automatically compiled to `.save` during build

**`src/data_providers/CMakeLists.txt`**:
- Added `FlatbuffersSaveDataProvider.cpp` to library

**`tests/unit/data_providers/CMakeLists.txt`**:
- Added `FlatbuffersSaveDataProvider.test.cpp` to test executable

## Testing Strategy

### Unit Tests

**FlatbuffersSaveDataProvider**:
- `SaveExists()` for non-existent saves (returns false)
- `SaveExists()` for existing saves (returns true)
- `GetSaveMetadata()` for non-existent saves (returns error)
- `GetSaveMetadata()` for existing saves (returns metadata)
- `LoadSaveData()` for non-existent saves (returns error)
- `LoadSaveData()` for existing saves (returns complete SaveData)

**FlatbuffersSceneDataProvider**:
- `ProvideSceneDataFromSave()` with null scene_data (returns error)
- `ProvideSceneDataFromSave()` with valid data (extracts and moves)
- Verification that scene_data is moved (not copied)

### Test Fixtures

**`tests/data/user/saves/save_slot_0.json`**:
- Complete save file with all fields populated
- Used by unit tests
- Compiled to `save_slot_0.save` during build

### Coverage

✅ Interface contracts (ISaveDataProvider methods)  
✅ File existence checking  
✅ Metadata loading  
✅ Complete save data loading  
✅ Scene data extraction  
✅ Error conditions (missing files, null pointers)  
⏳ Integration with TestEngine (Phase 3)  
⏳ End-to-end save/load workflow (Future)

## Usage Examples

### Loading a Save File

```cpp
#include "provider_factory.h"

// Get provider
ISaveDataProvider& provider = steamrot::GetSaveDataProvider();

// Load save
auto result = provider.LoadSaveData(0);
if (result.has_value()) {
  const SaveData& save = result.value();
  // Use save.metadata, save.scene_data, etc.
}
```

### Using Save in Tests

```json
{
  "metadata": {
    "test_name": "test_with_save",
    "description": "Test starting from saved state"
  },
  "save_data": {
    "metadata": {
      "save_name": "Test Save",
      "game_version": "1.0.0"
    },
    "scene_data": {
      "scene_type": "CRAFTING",
      "entities": [...]
    }
  },
  "num_ticks": 5
}
```

## API Reference

### ISaveDataProvider

```cpp
class ISaveDataProvider {
public:
  virtual std::expected<SaveData, FailInfo>
  LoadSaveData(uint32_t slot_index) const = 0;

  virtual bool SaveExists(uint32_t slot_index) const = 0;

  virtual std::expected<SaveMetadata, FailInfo>
  GetSaveMetadata(uint32_t slot_index) const = 0;
};
```

### ISceneDataProvider (Extended)

```cpp
class ISceneDataProvider {
public:
  virtual std::expected<std::unique_ptr<SceneData>, FailInfo>
  ProvideDefaultSceneData(const SceneType scene_type) const = 0;

  virtual std::expected<std::unique_ptr<SceneData>, FailInfo>
  ProvideSceneDataFromSave(SaveData &save_data) const = 0;  // NEW
};
```

### Provider Factory

```cpp
namespace steamrot {
  ISaveDataProvider& GetSaveDataProvider();  // NEW
  ISceneDataProvider& GetSceneDataProvider();
  // ... other providers
}
```

## Future Work

### Phase 3: Test Integration (Next Steps)

**Goal**: Enable TestEngine to use SaveData from TestDataConfig

**Tasks**:
1. Update `TestEngine` constructor to check for `save_data` field
2. Implement `TestEngine::LoadFromSaveData()` method
3. Add priority logic (save_data > starting_engine_state)
4. Create integration tests

**Estimated effort**: 4-6 hours

### Phase 4: Documentation & Examples

**Tasks**:
1. Add SaveData examples to `documentation/examples/`
2. Create save file templates for common scenarios
3. Update README with SaveData section
4. Add architecture diagrams

**Estimated effort**: 2-3 hours

### Future Enhancements

**Short-term** (1-2 sprints):
- Auto-save functionality
- Save file validation
- Save file migration between versions

**Medium-term** (2-4 sprints):
- Compressed save files
- Cloud save synchronization
- Save slot management UI

**Long-term** (4+ sprints):
- Encrypted saves
- Incremental saves (delta encoding)
- Save replay/rewind functionality

## Benefits Delivered

### For Development

✅ **Type-safe SaveData**: Native C++ structs with compile-time checking  
✅ **Extensible**: Easy to add new fields without breaking existing code  
✅ **Testable**: Complete unit test coverage for provider implementations  
✅ **Consistent**: Follows existing provider pattern architecture  

### For Testing

✅ **Data-driven tests**: Configure complete game state from JSON  
✅ **Reproducible**: Exact state snapshots for bug reproduction  
✅ **Reusable**: Share save fixtures across multiple tests  
✅ **Flexible**: Mix SaveData with simulation and event injection  

### For Game Features

✅ **Save/Load foundation**: Infrastructure for player save games  
✅ **Quick start**: Load directly into specific game states for debugging  
✅ **Scene configuration**: Alternative to default scene data  
⏳ **Auto-save**: Foundation in place (not yet implemented)  

## Lessons Learned

### What Went Well

1. **Clear Requirements**: Problem statement provided excellent guidance
2. **Existing Patterns**: Following established provider pattern simplified design
3. **Incremental Approach**: Phased implementation allowed for course correction
4. **Documentation First**: Design doc helped identify issues before coding

### Challenges

1. **FlatBuffers Complexity**: Schema dependencies required careful ordering
2. **Build System**: CMake macro for user data required new pattern
3. **Polymorphism**: SceneData ownership transfer needed careful consideration
4. **Test Data**: Creating valid test save files required complete entity setup

### Design Trade-offs

**Move vs. Copy for SceneData**:
- Chose: Move semantics with unique_ptr
- Benefit: Clear ownership, no accidental copies
- Cost: SaveData becomes partially invalidated after extraction

**Interface Extension vs. New Interface**:
- Chose: Extend ISceneDataProvider
- Benefit: Keeps scene data provision in one place
- Cost: ISceneDataProvider now depends on SaveData

**Optional vs. Required SaveData Fields**:
- Chose: Only metadata and scene_data required
- Benefit: Backward compatibility, gradual migration
- Cost: More null checks, potential incomplete saves

## References

### Documentation

- [Design Document](SAVE_DATA_PROVIDER_DESIGN.md) - Architecture and design
- [Usage Guide](../workflows/USING_SAVE_DATA.md) - How to use SaveData
- [Test Data Guide](../workflows/FILLING_TEST_DATA.md) - Test data patterns

### Code

- [ISaveDataProvider](../../src/interfaces/ISaveDataProvider.h) - Interface
- [SaveData](../../src/types/core/SaveData.h) - Native struct
- [FlatbuffersSaveDataProvider](../../src/data_providers/FlatbuffersSaveDataProvider.h) - Implementation
- [save_data.fbs](../../src/types/flatbuffers/configuration/save_data.fbs) - Schema

### Tests

- [FlatbuffersSaveDataProvider tests](../../tests/unit/data_providers/FlatbuffersSaveDataProvider.test.cpp)
- [FlatbuffersSceneDataProvider tests](../../tests/unit/data_providers/FlatbuffersSceneDataProvider.test.cpp)
- [Test save file](../../tests/data/user/saves/save_slot_0.json)

## Conclusion

The SaveData provider system is now complete through Phase 2, providing a solid foundation for:

1. **Loading game state from save files** via ISaveDataProvider
2. **Integrating saves with scene configuration** via extended ISceneDataProvider
3. **Using saves in data-driven tests** via extended TestDataConfig

The system is **ready for use** in its current state, with Phase 3 (TestEngine integration) being the logical next step for full test harness support.

The architecture is **extensible** and **follows established patterns**, making future enhancements straightforward to implement.

## Approval Status

- ✅ Core implementation complete (Phase 1 & 2)
- ✅ Unit tests passing
- ✅ Documentation complete
- ⏳ Awaiting user review for Phase 3 approval
- ⏳ Integration tests pending Phase 3

---

*Document prepared by: GitHub Copilot Agent*  
*Date: 2024-12-22*  
*Implementation Status: Phases 1-2 Complete, Ready for Review*
