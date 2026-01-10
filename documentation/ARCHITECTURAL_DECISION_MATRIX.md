# Architectural Decision Matrix: Data Loading/Exporting

This document provides decision trees and matrices to guide architectural decisions when working with SaveData, TestData, and related systems.

## Decision Tree: Where Should This Data Live?

```
┌─────────────────────────────────────────────────────────────┐
│ I need to add new data to the system. Where should it go?  │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
              ┌──────────────────────┐
              │ Is this data needed  │
              │ for production       │
              │ save/load?           │
              └──────┬───────────────┘
                     │
         ┌───────────┴───────────┐
         │                       │
        YES                     NO
         │                       │
         ▼                       ▼
   ┌──────────┐         ┌──────────────────┐
   │ Add to   │         │ Is this data     │
   │ SaveData │         │ needed for       │
   └──────────┘         │ testing?         │
                        └─────┬────────────┘
                              │
                   ┌──────────┴──────────┐
                   │                     │
                  YES                   NO
                   │                     │
                   ▼                     ▼
            ┌─────────────┐      ┌────────────────┐
            │ Add to      │      │ Consider if    │
            │ TestData    │      │ this is truly  │
            └─────────────┘      │ needed         │
                                 └────────────────┘
```

## Decision Tree: Should This Be Shared Code?

```
┌──────────────────────────────────────────────────────────────┐
│ I'm writing code that both GameEngine and TestEngine need.  │
│ Should I share it?                                           │
└────────────────────────┬─────────────────────────────────────┘
                         │
                         ▼
              ┌──────────────────────┐
              │ Is this STATE        │
              │ EXPORT logic?        │
              │ (getting data OUT)   │
              └──────┬───────────────┘
                     │
         ┌───────────┴───────────┐
         │                       │
        YES                     NO
         │                       │
         ▼                       ▼
   ┌─────────────────┐   ┌──────────────────┐
   │ Create utility  │   │ Is this STATE    │
   │ in engine::     │   │ IMPORT logic?    │
   │ export          │   │ (getting data IN)│
   │ namespace       │   └─────┬────────────┘
   └─────────────────┘         │
                    ┌──────────┴──────────┐
                    │                     │
                   YES                   NO
                    │                     │
                    ▼                     ▼
         ┌─────────────────────┐  ┌──────────────────┐
         │ Already shared via  │  │ Is behavior      │
         │ IEntityImporter ✅  │  │ identical?       │
         │ Use that pattern    │  └─────┬────────────┘
         └─────────────────────┘        │
                             ┌──────────┴──────────┐
                             │                     │
                            YES                   NO
                             │                     │
                             ▼                     ▼
                  ┌──────────────────┐    ┌────────────────┐
                  │ Create shared    │    │ Keep separate  │
                  │ utility in       │    │ implementations│
                  │ appropriate      │    └────────────────┘
                  │ namespace        │
                  └──────────────────┘
```

## Matrix: SaveData vs TestData vs EngineSnapshot

| Feature | SaveData | TestData | EngineSnapshot | Notes |
|---------|----------|----------|----------------|-------|
| **Purpose** | Production save/load | Testing validation | State capture | - |
| **Metadata** | SaveMetaData | TestMetaData | Optional tick_number | Different metadata types |
| **Scene Data** | SceneCollectionData | Via starting_snapshot | SceneCollectionData | All use same structure ✅ |
| **Entity Import** | Via IEntityImporter | Via IEntityImporter | Via IEntityImporter | Properly shared ✅ |
| **EngineConfig** | ⚪ Should NOT have | N/A | ⚪ Should NOT have | Global user settings, not per-save |
| **EngineState** | ⚠️ Should have | N/A | ⚠️ Should have (optional) | Phase 1 recommendation |
| **EventBus** | ⚠️ Should have | N/A | ✅ Has (optional) | Phase 1 for SaveData |
| **SimulationData** | ❌ No | ✅ Has | N/A | Test-specific only |
| **Multiple Snapshots** | ❌ No (single point) | ✅ Has (per tick) | N/A | Test-specific only |
| **Tick Execution** | N/A | ✅ Has | N/A | Test-specific only |
| **Field Optionality** | Required fields | Required fields | Optional fields | EngineSnapshot is flexible |

**Legend:**
- ✅ Has this feature
- ❌ Doesn't have (by design)
- ⚠️ Should have but currently missing
- ⚪ Should NOT have (handled separately)
- N/A - Not applicable for this use case

## Matrix: When to Add New Data Type

| Question | Add to SaveData | Add to TestData | Add to EngineSnapshot | Create New Type |
|----------|----------------|-----------------|----------------------|-----------------|
| Do players need to save/load it? | ✅ Yes | ❌ No | Maybe (optional) | ❌ No |
| Is it only for testing? | ❌ No | ✅ Yes | Maybe (optional) | ❌ No |
| Is it metadata about save/test? | Maybe (SaveMetaData) | Maybe (TestMetaData) | ❌ No | Consider new metadata type |
| Is it simulation configuration? | ❌ No | ✅ Yes (SimulationData) | ❌ No | ❌ No |
| Is it runtime engine state? | ✅ Yes | ❌ No | ✅ Yes (optional) | ❌ No |
| Is it user preferences/display settings? | ❌ No (global settings) | ❌ No | ❌ No | ❌ No (use preference system) |
| Is it scene data? | ✅ SceneCollectionData | ✅ Via snapshot | ✅ SceneCollectionData | ❌ No (use existing) |
| Is it something entirely new? | Consider carefully | Consider carefully | Consider carefully | ✅ Maybe |

## Decision Matrix: Should I Modify Existing vs Create New?

| Scenario | Modify SaveData | Modify TestData | Create Utility | Create New Type |
|----------|----------------|-----------------|----------------|-----------------|
| Need to export scene state | ❌ No | ❌ No | ✅ Yes (engine::export) | ❌ No |
| Need to import scene entities | ❌ No (use IEntityImporter) | ❌ No (use IEntityImporter) | ❌ No (already exists) | ❌ No |
| SaveData is missing engine state | ✅ Yes (Phase 1) | ❌ No | ❌ No | ❌ No |
| Need new simulation capability | ❌ No | ✅ Yes (extend SimulationData) | Maybe (simulation utilities) | ❌ No |
| Need to convert SaveData to TestData | ❌ No | ❌ No | ✅ Yes (conversion utility) | ❌ No |
| Need new metadata field | ✅ SaveMetaData | ✅ TestMetaData | ❌ No | ❌ No |
| Need entirely new concept | ❌ No | ❌ No | Consider | ✅ Maybe |

## Anti-Patterns to Avoid

### ❌ Don't: Force Unification

```cpp
// BAD: Making TestData wrap SaveData
struct TestData {
  SaveData save_data;           // Don't do this!
  SimulationData simulation;
};

// WHY: This couples testing to production structure
// INSTEAD: Keep separate, share via utilities
```

### ❌ Don't: Add Test Features to Production

```cpp
// BAD: Adding test features to SaveData
struct SaveData {
  SaveMetaData meta_data;
  SceneCollectionData scenes;
  SimulationData simulation;    // Don't do this!
  std::map<size_t, EngineSnapshot> expected;  // Don't do this!
};

// WHY: Production doesn't need these, bloats SaveData
// INSTEAD: Keep test features in TestData
```

### ❌ Don't: Duplicate Export Logic

```cpp
// BAD: Duplicating export code
std::expected<SaveData, FailInfo> GameEngine::SaveGame() {
  // Manually export scenes
  for (auto& scene : scenes) {
    // Copy entity pool logic here...
  }
}

std::expected<EngineSnapshot, FailInfo> TestEngine::CaptureSnapShot() {
  // DUPLICATE: Same entity pool copy logic here...
}

// WHY: Code duplication, maintenance burden
// INSTEAD: Create shared engine::export utilities
```

### ❌ Don't: Mix Metadata Types

```cpp
// BAD: Using SaveMetaData in TestData
struct TestData {
  SaveMetaData meta_data;       // Don't do this!
  EngineSnapshot snapshot;
};

// WHY: Different purposes, different fields needed
// INSTEAD: Use appropriate metadata type (TestMetaData)
```

## Good Patterns to Follow

### ✅ Do: Share Implementation via Utilities

```cpp
// GOOD: Shared export utilities
namespace engine::export {
  std::expected<SceneCollectionData, FailInfo>
  ExportActiveScenes(const SceneManager& manager,
                     EventHandler& event_handler) {
    // Implementation shared by both GameEngine and TestEngine
  }
}

// GameEngine uses it
SaveData save_data;
save_data.scenes = engine::export::ExportActiveScenes(...);

// TestEngine uses it
EngineSnapshot snapshot;
snapshot.scene_collection_data = engine::export::ExportActiveScenes(...);
```

### ✅ Do: Keep Separate Structures

```cpp
// GOOD: Separate structures for different purposes
struct SaveData {
  SaveMetaData meta_data;
  EngineState state;          // Runtime state (IS per-save)
  SceneCollectionData scenes;
  // Note: EngineConfig NOT here - user prefs are global, not per-save
};

struct TestData {
  TestMetaData meta_data;
  SimulationData simulation;
  EngineSnapshot starting_snapshot;
  std::map<size_t, EngineSnapshot> expected_snapshots;
};
```

### ✅ Do: Use Existing Patterns

```cpp
// GOOD: Leveraging IEntityImporter
struct SceneLoadData {
  SceneData scene_data;
  std::unique_ptr<IEntityImporter> entity_importer;  // Shared pattern ✅
};

// Both SaveData and TestData use SceneCollectionData
// which uses IEntityImporter
```

### ✅ Do: Make EngineSnapshot Flexible

```cpp
// GOOD: Optional fields for flexibility
struct EngineSnapshot {
  std::optional<size_t> tick_number;
  SceneCollectionData scene_collection_data;
  std::optional<EventBus> global_event_bus;
  std::optional<EngineState> engine_state;  // Tests can choose what to capture
  // Note: EngineConfig NOT here - user prefs are global settings
};
```

## Checklist: Before Adding New Functionality

Before implementing new data-related functionality, ask:

- [ ] **Purpose**: Is this for production, testing, or both?
- [ ] **Existing Patterns**: Can I use IEntityImporter or other existing patterns?
- [ ] **Code Reuse**: Is there already similar code I can extract to a utility?
- [ ] **Separation**: Am I maintaining clear boundaries between production and testing?
- [ ] **Coupling**: Am I avoiding tight coupling between SaveData and TestData?
- [ ] **Extensibility**: Will this change break existing code or tests?
- [ ] **Optional Fields**: For EngineSnapshot, should new fields be optional?
- [ ] **Documentation**: Have I documented why this change is needed?

## Checklist: Before Modifying SaveData

- [ ] **Production Need**: Is this genuinely needed for save/load?
- [ ] **Not Testing**: Am I sure this isn't a test-only feature?
- [ ] **Schema Update**: Will I update save_data.fbs schema?
- [ ] **Provider Update**: Will I update FlatbuffersSaveDataProvider?
- [ ] **Backward Compatibility**: How will this affect existing saves?
- [ ] **GameEngine Integration**: How will GameEngine use this?
- [ ] **Documentation**: Updated documentation to reflect changes?

## Checklist: Before Modifying TestData

- [ ] **Testing Need**: Is this genuinely needed for testing?
- [ ] **Not Production**: Am I sure this isn't a production feature?
- [ ] **Schema Update**: Will I update test_data.fbs schema?
- [ ] **Provider Update**: Will I update FlatbuffersTestDataProvider?
- [ ] **TestEngine Integration**: How will TestEngine use this?
- [ ] **Test Harness**: Do I need to update test_harness functions?
- [ ] **Documentation**: Updated tests/harness/README.md?

## Checklist: Before Creating Shared Utility

- [ ] **Genuine Sharing**: Is this code truly used by both GameEngine and TestEngine?
- [ ] **Implementation**: Am I sharing implementation, not forcing interface unification?
- [ ] **Namespace**: Is engine::export the right namespace?
- [ ] **Naming**: Is the function name clear and descriptive?
- [ ] **Error Handling**: Using std::expected for error handling?
- [ ] **Testing**: Have I written tests for the utility?
- [ ] **Documentation**: Documented the utility function?
- [ ] **Dependencies**: Does this utility have minimal dependencies?

## Phase-Specific Guidance

### Phase 1: Core State Capture

**Focus:** Making SaveData complete for per-save-game state

**Checklist:**
- [ ] Add EngineState to SaveData struct
- [ ] Add optional<EventBus> to SaveData struct
- [ ] Update save_data.fbs schema with new fields
- [ ] Update FlatbuffersSaveDataProvider::ProvideSaveData()
- [ ] Create configuration methods for new fields
- [ ] Implement GameEngine::SaveGame() method
- [ ] Implement GameEngine::LoadGame() method
- [ ] Add tests for save/load functionality
- [ ] Update documentation

**Note:** Do NOT add EngineConfig to SaveData. User preferences and display
settings are global user settings managed by the preference system, not
per-save-game data.

**Priority:** HIGH - Enables complete production save/load

### Phase 2: Code Reuse

**Focus:** Eliminating duplication via utilities

**Checklist:**
- [ ] Create src/engine/engine_export.h header
- [ ] Create src/engine/engine_export.cpp implementation
- [ ] Implement ExportActiveScenes() function
- [ ] Implement ExportEventBus() function
- [ ] Implement ExportEngineState() function
- [ ] Implement ExportEngineSnapshot() function (composite)
- [ ] Refactor TestEngine::CaptureSnapShot() to use utilities
- [ ] Refactor GameEngine::SaveGame() to use utilities
- [ ] Add tests for export utilities
- [ ] Update documentation

**Note:** ExportEngineConfig() NOT needed - user preferences are global
settings, not per-save or per-snapshot data.

**Priority:** MEDIUM - Improves maintainability

### Phase 3: Testing Integration

**Focus:** Enable test generation from saves

**Checklist:**
- [ ] Extend EngineSnapshot with optional<EngineState>
- [ ] Extend EngineSnapshot with optional<SceneManagerData>
- [ ] Create SaveDataToTestDataConverter.h
- [ ] Implement ConvertSaveDataToTestData() function
- [ ] Add tools for automated test generation
- [ ] Update TestEngine to use extended EngineSnapshot
- [ ] Add documentation for test generation workflow
- [ ] Add examples of converting saves to tests
- [ ] Update tests/harness/README.md

**Note:** Do NOT add EngineConfig to EngineSnapshot - user preferences are
global settings managed separately, not part of test state snapshots.

**Priority:** LOW - Nice to have for advanced testing

## Summary

Use this decision matrix when:
- Adding new data to the system
- Deciding where functionality belongs
- Evaluating whether to share code
- Avoiding anti-patterns
- Following implementation phases

Remember the core principles:
1. **Separate structures** for production (SaveData) and testing (TestData)
2. **Share implementation** via utilities (engine::export namespace)
3. **Maintain boundaries** - don't force unification
4. **Leverage existing patterns** - IEntityImporter works well
5. **Keep TestEngine simple** - don't complicate it for SaveData compatibility

Refer to the comprehensive analysis documents for detailed rationale behind these decisions.
