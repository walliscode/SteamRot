# Quick Reference: SaveData vs TestData

## When to Use What

### Use SaveData When:
- ✅ Implementing production save/load functionality
- ✅ Persisting game state to disk for players
- ✅ Restoring game session after restart
- ✅ Implementing autosave or manual save features

### Use TestData When:
- ✅ Writing data-driven tests
- ✅ Validating game logic with TestEngine
- ✅ Testing specific gameplay scenarios
- ✅ Regression testing with known states
- ✅ Simulating multi-tick interactions

### Use Both When:
- ✅ Generating test data from real gameplay (SaveData → TestData)
- ✅ Debugging save files by converting to tests
- ✅ Validating save/load round-trip correctness

## Key Differences

| Aspect | SaveData | TestData |
|--------|----------|----------|
| **Purpose** | Production save/load | Testing and validation |
| **Consumer** | GameEngine | TestEngine |
| **Snapshots** | Single point-in-time | Multiple (start + expected per tick) |
| **Metadata** | SaveMetaData (name, UUID) | TestMetaData (name, will_pass) |
| **Simulation** | N/A | SimulationData (Logic, Functions, Inputs) |
| **Tick Model** | N/A | Tick-based execution |
| **State Capture** | Complete game state | State per tick for validation |

## Current State (As of Analysis)

### SaveData Status: ⚠️ Incomplete

**What it has:**
- ✅ SaveMetaData
- ✅ SceneManagerData
- ✅ SceneCollectionData (via IEntityImporter)

**What it's missing:**
- ❌ EngineState (running, paused, subscriptions)
- ❌ EventBus state (global events)

**What it should NOT have (intentionally separate):**
- ⚪ EngineConfig (user preferences, display settings) - These are global user settings, NOT per-save data. Already handled separately via default.preferences.bin and user preference system.

**Recommendation:** Extend SaveData to include EngineState and EventBus (see Phase 1 in architecture analysis). Do NOT add EngineConfig.

### TestData Status: ✅ Complete

**What it has:**
- ✅ TestMetaData
- ✅ SimulationData (Logic execution configuration)
- ✅ Tick-based execution model
- ✅ EngineSnapshot (starting + expected per tick)
- ✅ SceneCollectionData (via IEntityImporter)
- ✅ EventBus state capture

**Recommendation:** No changes needed, already well-designed

## Shared Components (Working Well ✅)

### IEntityImporter
Both systems use the same entity import mechanism:
- `IEntityImporter` interface
- `FlatbuffersEntityImporter` implementation
- `SceneCollectionData` structure
- `FlatbuffersEntityConfigurator` under the hood

**This sharing is intentional and beneficial** - ensures tests use the same data path as production.

## Code Location Map

### SaveData Related Files
```
src/types/core/SaveData.h                    - SaveData struct
src/types/core/SaveMetaData.h                - Metadata struct
src/types/interfaces/ISaveDataProvider.h     - Provider interface
src/data_providers/FlatbuffersSaveDataProvider.{h,cpp} - Implementation
src/types/flatbuffers/configuration/save_data.fbs - FlatBuffers schema
```

### TestData Related Files
```
src/types/test_structs/TestData.h            - TestData struct
src/types/test_structs/TestMetaData.h        - Metadata struct
tests/harness/ITestDataProvider.h            - Provider interface
tests/harness/FlatbuffersTestDataProvider.{h,cpp} - Implementation
src/types/flatbuffers/testing/test_data.fbs  - FlatBuffers schema
```

### TestEngine Related Files
```
tests/harness/TestEngine.{h,cpp}             - TestEngine implementation
tests/harness/test_harness.{h,cpp}           - Test orchestration
tests/harness/simulation_runner.{h,cpp}      - Simulation execution
tests/harness/input_simulation.{h,cpp}       - Input sequence handling
tests/harness/event_simulation.{h,cpp}       - Event sequence handling
```

### Shared Components
```
src/types/core/SceneLoadData.h               - SceneCollectionData
src/types/interfaces/IEntityImporter.h       - Import interface
src/entity/FlatbuffersEntityImporter.{h,cpp} - Import implementation
src/entity/FlatbuffersEntityConfigurator.{h,cpp} - Entity configuration
```

### EngineSnapshot
```
src/types/core/EngineSnapshot.h              - Snapshot structure
```

## Future Utilities (Recommended)

### engine::export Namespace (To Be Created)
```
src/engine/engine_export.h                   - Export utilities
src/engine/engine_export.cpp                 - Implementation

Functions:
- ExportActiveScenes()      - Export scene entity data
- ExportEventBus()          - Export event bus state
- ExportEngineState()       - Export engine state
- ExportEngineConfig()      - Export engine config
- ExportEngineSnapshot()    - Export complete snapshot
```

### conversion Namespace (To Be Created)
```
src/data_providers/SaveDataToTestDataConverter.{h,cpp}

Functions:
- ConvertSaveDataToTestData() - Generate test data from save files
```

## Common Patterns

### Loading SaveData (Current - Incomplete)
```cpp
// Create provider
FlatbuffersSaveDataProvider provider;

// Load save data
auto save_data_result = provider.ProvideSaveData();
if (!save_data_result.has_value()) {
  // Handle error
}

SaveData save_data = save_data_result.value();
// Use save_data to restore GameEngine state
```

### Loading TestData (Current - Complete)
```cpp
// Create provider with directory path
std::filesystem::path test_dir = "tests/my_test/data";
FlatbuffersTestDataProvider provider(test_dir);

// Load all test data
auto test_data_result = provider.ProviderAllTestData();
if (!test_data_result.has_value()) {
  // Handle error
}

std::vector<TestData> all_tests = test_data_result.value();
// Use with TestEngine or Catch2 generators
```

### Using TestEngine (Current - Complete)
```cpp
#include "TestEngine.h"
#include "test_harness.h"

// Load test configurations
auto configs = steamrot::tests::load_test_data_configs();
REQUIRE(configs.has_value());

// Use with Catch2 generator
const auto *config = GENERATE_COPY(from_range(configs.value()));

// Run test - creates TestEngine, runs it, compares snapshots
auto result = steamrot::tests::RunTestEngineTest(config);
REQUIRE(result.has_value());
```

### Recommended: Saving Game State (Future)
```cpp
#include "engine_export.h"

// In GameEngine::SaveGame()
std::expected<SaveData, FailInfo> GameEngine::SaveGame() {
  SaveData save_data;
  
  // Use shared export utilities
  auto scenes = engine::export::ExportActiveScenes(m_scene_manager, 
                                                   m_engine_resources.event_handler);
  if (!scenes.has_value()) return std::unexpected(scenes.error());
  
  save_data.scene_collection_data = std::move(scenes.value());
  save_data.event_bus = engine::export::ExportEventBus(m_engine_resources.event_handler);
  save_data.engine_state = engine::export::ExportEngineState(m_engine_state);
  save_data.engine_config = engine::export::ExportEngineConfig(m_engine_config);
  
  return save_data;
}
```

### Recommended: Converting Save to Test (Future)
```cpp
#include "SaveDataToTestDataConverter.h"

// Load a save file
SaveData save_data = LoadSaveFile("my_save.save");

// Convert to test data
auto test_data_result = steamrot::conversion::ConvertSaveDataToTestData(
    save_data,
    "regression_test_001",
    "Test generated from gameplay save"
);

if (test_data_result.has_value()) {
  TestData test_data = test_data_result.value();
  // Use with TestEngine or serialize to .test_data.bin
}
```

## FAQ

### Q: Can I use SaveData for testing?
**A:** Not directly. SaveData is designed for production save/load. For testing, use TestData with TestEngine. However, you can convert SaveData to TestData using the conversion utility (once implemented).

### Q: Can I use TestData in production?
**A:** No. TestData includes test-specific features (SimulationData, expected snapshots) that aren't relevant for production. Use SaveData for production save/load.

### Q: Why do both systems use IEntityImporter?
**A:** This is intentional code reuse. It ensures that tests use the same entity import path as production, which validates that the production import mechanism works correctly.

### Q: Should I add new fields to SaveData or EngineSnapshot?
**A:** Depends on the use case:
- **SaveData**: Add fields that are necessary for production save/load (engine state, config)
- **EngineSnapshot**: Add optional fields that are useful for testing (any engine-level data)
- When in doubt, add to EngineSnapshot first (with optional), then to SaveData if needed for production

### Q: How do I generate test data from gameplay?
**A:** This feature is planned but not yet implemented. The recommended approach is:
1. Play the game and save
2. Use `ConvertSaveDataToTestData()` utility (to be implemented in Phase 3)
3. Optionally add simulation steps or expected snapshots
4. Serialize to `.test_data.bin` for use in tests

### Q: What's the difference between EngineState and EngineConfig?
**A:**
- **EngineState**: Runtime operational state that changes during execution (running, paused, subscriptions, performance metrics). This IS per-save-game state.
- **EngineConfig**: Configuration settings that rarely change (user preferences like volume/language, display settings like framerate/fullscreen). This is NOT per-save-game - it's global user settings loaded at engine startup from default.preferences.bin and user preference files.

### Q: Should SaveData fields be optional like EngineSnapshot?
**A:** No. SaveData represents a complete production save and should have required fields. EngineSnapshot uses optional fields for flexibility in testing (tests can choose what to validate).

### Q: Why isn't EngineConfig in SaveData?
**A:** EngineConfig contains user preferences (master volume, language) and display settings (framerate, fullscreen, vsync) that are global user settings, not per-save-game state. These settings are stored separately in preference files (default.preferences.bin and user-specific files) and loaded at engine startup. A user's volume preferences should apply across all save games, not be different per save.

## See Also

- **Full Analysis**: `DATA_LOADING_ARCHITECTURE_ANALYSIS.md`
- **Visual Diagrams**: `DATA_FLOW_DIAGRAMS.md`
- **Test Harness Documentation**: `tests/harness/README.md`
- **Entity Import/Export**: See `IEntityImporter` interface and implementations
