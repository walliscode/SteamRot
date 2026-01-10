# Documentation Index: Data Loading/Exporting Architecture

This directory contains comprehensive documentation analyzing the SteamRot data loading and exporting architecture, with specific focus on the relationship between SaveData and TestData systems.

## Documentation Files

### 1. [DATA_LOADING_ARCHITECTURE_ANALYSIS.md](DATA_LOADING_ARCHITECTURE_ANALYSIS.md)
**Comprehensive technical analysis** - Primary reference document

**Contents:**
- Executive summary
- Current state analysis of SaveData and TestData systems
- Detailed breakdown of overlaps and distinctions
- Areas of concern and recommendations
- Long-term architecture recommendations (3-phase implementation plan)
- Architectural principles

**Read this if:** You need a complete technical understanding of the current architecture and future direction.

### 2. [DATA_FLOW_DIAGRAMS.md](DATA_FLOW_DIAGRAMS.md)
**Visual representations of data flow** - Diagram reference

**Contents:**
- ASCII diagrams showing current and recommended architectures
- Production save/load workflows
- Testing workflows with TestEngine
- Shared component relationships
- SaveData vs EngineSnapshot comparison
- Implementation timeline roadmap

**Read this if:** You prefer visual understanding of system architecture and data flow.

### 3. [SAVEDATA_VS_TESTDATA_QUICK_REF.md](SAVEDATA_VS_TESTDATA_QUICK_REF.md)
**Quick reference guide** - Developer cheat sheet

**Contents:**
- When to use SaveData vs TestData
- Key differences table
- Current status of each system
- Code location map
- Common usage patterns
- FAQ section

**Read this if:** You need quick answers while implementing features or writing tests.

## Quick Start Guide

### For New Developers
1. Start with **SAVEDATA_VS_TESTDATA_QUICK_REF.md** to understand the basics
2. Review **DATA_FLOW_DIAGRAMS.md** to visualize the architecture
3. Read **DATA_LOADING_ARCHITECTURE_ANALYSIS.md** for complete technical details

### For Implementing Save/Load
1. Read **Phase 1** recommendations in **DATA_LOADING_ARCHITECTURE_ANALYSIS.md**
2. Review SaveData current limitations
3. Follow the recommended SaveData structure extension
4. Implement shared export utilities (Phase 2)

### For Writing Tests
1. Check **tests/harness/README.md** for TestEngine usage
2. Use TestData with TestEngine (already complete ✅)
3. Reference **SAVEDATA_VS_TESTDATA_QUICK_REF.md** for patterns

### For Architecture Understanding
1. Read **Executive Summary** in **DATA_LOADING_ARCHITECTURE_ANALYSIS.md**
2. Review **Summary of Recommendations** section
3. Study **DATA_FLOW_DIAGRAMS.md** for visual understanding

## Key Takeaways

### ✅ What's Working Well
1. **Shared Entity Import/Export** - IEntityImporter pattern works excellently
2. **TestEngine Design** - Well-designed, easy to use, good separation of concerns
3. **Clear Boundaries** - Production (SaveData) and Testing (TestData) are properly separated

### ❌ Current Limitations
1. **SaveData Incomplete** - Missing EngineState, EventBus (NOT EngineConfig - that's global user settings)
2. **Code Duplication** - State export logic duplicated between GameEngine and TestEngine
3. **EngineSnapshot Misalignment** - Not fully aligned with SaveData capabilities

### 📝 Recommended Path Forward

**Phase 1: Core State Capture** (High Priority)
- Extend SaveData to include EngineState, EventBus (NOT EngineConfig - that's global user settings)
- Update FlatBuffers schema (save_data.fbs)
- Update FlatbuffersSaveDataProvider
- Implement complete GameEngine save/load

**Phase 2: Code Reuse** (Medium Priority)
- Create `engine::export` namespace with shared utilities
- Refactor TestEngine::CaptureSnapShot to use utilities
- Implement GameEngine::SaveGame using utilities
- Eliminate code duplication

**Phase 3: Testing Integration** (Low Priority)
- Extend EngineSnapshot with optional EngineState, SceneManagerData (NOT EngineConfig - global settings)
- Implement SaveData → TestData conversion
- Enable test generation from gameplay saves
- Add documentation for test generation workflow

## Problem Statement Resolution

**Original Question:**
> "I've confused myself a bit with some of my data loading/exporting structures and I want to clarify and plan the way forward. We have the SaveData struct which we want to obviously be save data which we can load back and restart the game. Equally we want to make this a useful struct for exporting data as well (is it?). We have the TestEngine which takes in the TestData struct which contains the start state of the Engine and then subsequent states based on tick number. There are obviously some overlaps here as we importing and exporting data from the Engine. I would like to share as much functionality as possible so that we mimic workflows but also make sure we don't make the TestEngine hard to use because we are trying to mimic SaveData exactly."

**Analysis Conclusion:**

1. **SaveData for Production** ✅
   - Yes, SaveData should be used for save/load in production
   - Currently incomplete (missing EngineState and EventBus)
   - Needs extension to capture complete per-save state (Phase 1 recommendation)
   - Note: EngineConfig should NOT be in SaveData - user preferences are global settings

2. **SaveData for Exporting** ⚠️
   - SaveData can be used for exporting, but it's currently incomplete
   - Recommended: Create shared export utilities that both SaveData and TestData can use
   - This approach maximizes code reuse without forcing unification

3. **TestData Independence** ✅
   - TestData should remain independent with test-specific features
   - Don't force TestData to mimic SaveData exactly
   - TestEngine is already easy to use and well-designed

4. **Overlap Management** ✅
   - Entity import/export is already properly shared via IEntityImporter
   - Recommended: Create `engine::export` utilities for additional sharing
   - This eliminates duplication while maintaining clear boundaries

5. **Workflow Mimicry** 📝
   - Workflows should share implementation (export utilities) not interfaces
   - SaveData and TestData remain distinct with different purposes
   - Shared utilities ensure consistent behavior without tight coupling

## Implementation Guidance

### Don't Do This ❌
```cpp
// DON'T: Force TestData to mimic SaveData structure
struct TestData {
  SaveData save_data;  // Bad: Couples testing to production structure
  SimulationData simulation;
};

// DON'T: Add test features to SaveData
struct SaveData {
  SimulationData simulation;  // Bad: Production doesn't need this
  std::map<size_t, EngineSnapshot> expected_states;  // Bad: Testing concern
};
```

### Do This Instead ✅
```cpp
// DO: Keep structures separate
struct SaveData {
  SaveMetaData meta_data;
  EngineConfig engine_config;        // Production needs
  EngineState engine_state;
  SceneCollectionData scenes;
};

struct TestData {
  TestMetaData meta_data;
  SimulationData simulation;         // Test needs
  EngineSnapshot starting_snapshot;
  std::map<size_t, EngineSnapshot> expected_snapshots;
};

// DO: Share export implementation
namespace engine::export {
  SceneCollectionData ExportActiveScenes(...);  // Both use this
  EventBus ExportEventBus(...);                 // Both use this
}
```

## Related Documentation

### Within This Repository
- **tests/harness/README.md** - TestEngine and test harness documentation
- **src/types/core/** - Core type definitions (SaveData, EngineSnapshot, etc.)
- **src/types/test_structs/** - Test type definitions (TestData, TestMetaData, etc.)

### FlatBuffers Schemas
- **src/types/flatbuffers/configuration/save_data.fbs** - SaveData schema
- **src/types/flatbuffers/testing/test_data.fbs** - TestData schema
- **src/types/flatbuffers/testing/simulation_data.fbs** - Simulation schema

## Contributing

When extending the data loading/exporting systems:

1. **Before Adding to SaveData:**
   - Is this needed for production save/load? ✅ Add it
   - Is this only for testing? ❌ Don't add it, use TestData instead

2. **Before Adding to TestData:**
   - Is this needed for testing? ✅ Add it
   - Is this production state? ❌ Don't add it, use SaveData instead

3. **Before Duplicating Code:**
   - Can this be a shared export utility? ✅ Create utility in `engine::export`
   - Is this genuinely different? ✅ Keep separate implementations

4. **Before Coupling Systems:**
   - Can they share implementation without sharing interfaces? ✅ Do that
   - Do they need tight coupling? ❌ Probably not, reconsider

## Conclusion

The SteamRot data loading/exporting architecture has a solid foundation with clear separation of concerns and proper sharing of entity import/export logic. The main improvements needed are:

1. **Making SaveData complete** - Add engine-level state capture
2. **Creating shared utilities** - Eliminate code duplication via `engine::export`
3. **Aligning EngineSnapshot** - Make it a superset of SaveData capabilities

These changes will enable complete save/load functionality while maximizing code reuse and maintaining the excellent usability of the TestEngine system. The documentation provides a clear path forward with a 3-phase implementation plan.

---

**Document Status:** Complete Analysis (January 2026)  
**Next Review:** After Phase 1 implementation
