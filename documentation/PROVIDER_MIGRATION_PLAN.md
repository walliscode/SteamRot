# Provider Migration Plan

## Overview

This document provides a detailed, step-by-step migration plan for standardizing all Provider and Configurator interfaces in the SteamRot codebase.

## Migration Principles

1. **Incremental**: Migrate one provider at a time
2. **Test-Driven**: Ensure tests pass at each step
3. **Non-Breaking**: Maintain backward compatibility during migration
4. **Documented**: Update docs as we go

## Current Inventory

### Data Providers (6 interfaces)

| Interface | Concrete Implementation | Complexity | Status |
|-----------|------------------------|------------|--------|
| `IEngineDataProvider` | `FlatbuffersEngineDataProvider` | Low | ✅ **COMPLETE** |
| `ISaveDataProvider` | `FlatbuffersSaveDataProvider` | Low | ✅ **COMPLETE** |
| `ISceneManagerDataProvider` | `FlatbuffersSceneManagerDataProvider` | Medium | ✅ **COMPLETE** |
| `IUIStyleDataProvider` | `FlatbuffersUIStyleDataProvider` | Medium | ✅ **COMPLETE** |
| `ISceneDataProvider` | `FlatbuffersSceneDataProvider` | High | ✅ **COMPLETE** |
| `IFontProvider` | (AssetManager) | Low | ⏳ Not Started |

### Configurators (4 interfaces)

| Interface | Concrete Implementation | Complexity | Status |
|-----------|------------------------|------------|--------|
| `ISceneConfigurator` | `FlatbuffersSceneConfigurator` | Medium | ⚠️ **NEEDS REVIEW** (see analysis below) |
| `IUIElementConfigurator` | `FlatbuffersUIElementConfigurator` | High | ⏳ Not Started |
| `IEntityConfigurator` | `FlatbuffersEntityConfigurator` | Very High | ⏳ Not Started |
| `IUIStyleConfigurator` | (Not implemented) | N/A | N/A |

## Phase 1: Foundation ✅ COMPLETE

- [x] Analyze current state
- [x] Document patterns and problems
- [x] Create proposal document
- [x] Create implementation guide
- [x] Create migration plan
- [x] Make templating decision (no templates)

## Phase 2: Data Provider Migration ✅ **COMPLETE**

All data providers have been successfully migrated to the Create/Configure pattern with free configuration functions.

### Migration Summary

All providers now follow the standardized pattern:
- **Create Method**: Creates and returns a new configured object (e.g., `CreateEngineData()`)
- **Configure Method**: Configures an existing object (e.g., `ConfigureEngineData()`)
- **Free Functions**: Configuration logic extracted to `src/data_providers/configure/` directory

### Completed Migrations

#### Task 2.1: IEngineDataProvider ✅ **COMPLETE**

**What was done:**
- ✅ Interface updated with `CreateEngineData()` and `ConfigureEngineData()` methods
- ✅ Free functions created in `src/data_providers/configure/configure_engine_data.h/cpp`
- ✅ Tests added: `tests/unit/data_providers/configure_engine_data.test.cpp`
- ✅ Provider uses free functions from `steamrot::config` namespace

**Files changed:**
- `src/types/interfaces/IEngineDataProvider.h`
- `src/data_providers/FlatbuffersEngineDataProvider.h/cpp`
- `src/data_providers/configure/configure_engine_data.h/cpp`
- `tests/unit/data_providers/FlatbuffersEngineDataProvider.test.cpp`
- `tests/unit/data_providers/configure_engine_data.test.cpp`

---

#### Task 2.2: ISaveDataProvider ✅ **COMPLETE**

**What was done:**
- ✅ Interface updated with `CreateSaveData()` and `ConfigureSaveData()` methods
- ✅ Free functions created in `src/data_providers/configure/configure_save_data.h/cpp`
- ✅ Tests added: `tests/unit/data_providers/configure_save_data.test.cpp`

**Files changed:**
- `src/types/interfaces/ISaveDataProvider.h`
- `src/data_providers/FlatbuffersSaveDataProvider.h/cpp`
- `src/data_providers/configure/configure_save_data.h/cpp`
- `tests/unit/data_providers/configure_save_data.test.cpp`

---

#### Task 2.3: ISceneManagerDataProvider ✅ **COMPLETE**

**What was done:**
- ✅ Interface updated with `CreateSceneManagerData()` and `ConfigureSceneManagerData()` methods
- ✅ Free functions created in `src/data_providers/configure/configure_scene_manager_data.h/cpp`
- ✅ Tests added: `tests/unit/data_providers/configure_scene_manager_data.test.cpp`

**Files changed:**
- `src/types/interfaces/ISceneManagerDataProvider.h`
- `src/data_providers/FlatbuffersSceneManagerDataProvider.h/cpp`
- `src/data_providers/configure/configure_scene_manager_data.h/cpp`
- `tests/unit/data_providers/configure_scene_manager_data.test.cpp`

---

#### Task 2.4: IUIStyleDataProvider ✅ **COMPLETE**

**What was done:**
- ✅ Interface updated with `CreateUIStyles()` and `ConfigureUIStyles()` methods
- ✅ Legacy `ProvideUIStyles()` method kept for backward compatibility
- ✅ Free functions created in `src/data_providers/configure/configure_ui_styles.h/cpp`
- ✅ Tests added: `tests/unit/data_providers/configure_ui_styles.test.cpp`

**Files changed:**
- `src/types/interfaces/IUIStyleDataProvider.h`
- `src/data_providers/FlatbuffersUIStyleDataProvider.h/cpp`
- `src/data_providers/configure/configure_ui_styles.h/cpp`
- `tests/unit/data_providers/configure_ui_styles.test.cpp`

---

#### Task 2.5: ISceneDataProvider ✅ **COMPLETE**

**What was done:**
- ✅ Interface updated with `CreateSceneData()` and `ConfigureSceneData()` methods
- ✅ Free functions created in `src/data_providers/configure/configure_scene_data.h/cpp`
- ✅ Asset config free functions in `src/data_providers/configure/configure_asset_config.h/cpp`
- ✅ Tests added for all configuration functions
- ✅ FlatBuffers schemas updated for scene data structures

**Files changed:**
- `src/types/interfaces/ISceneDataProvider.h`
- `src/data_providers/FlatbuffersSceneDataProvider.h/cpp`
- `src/data_providers/configure/configure_scene_data.h/cpp`
- `src/data_providers/configure/configure_asset_config.h/cpp`
- `tests/unit/data_providers/configure_scene_data.test.cpp`
- `tests/unit/data_providers/configure_asset_config.test.cpp`
- `src/types/flatbuffers/scenes/*.fbs` (schema updates)

---

#### Task 2.6: IFontProvider ⏳ **NOT STARTED**

**Current State:**
- Simple getter interface: `GetFont(const std::string& font_name)`
- Implemented by AssetManager
- No migration needed - interface is appropriate for a simple resource getter

**Recommendation:**
- **Keep as-is**: The `GetFont()` method is appropriate for a simple resource accessor
- Does not need Create/Configure pattern
- Can be marked as complete without changes

---

## Phase 2 Notes

### Free Function Organization

All free functions were placed in `src/data_providers/configure/` directory:
- `configure_engine_data.h/cpp`
- `configure_save_data.h/cpp`
- `configure_scene_manager_data.h/cpp`
- `configure_ui_styles.h/cpp`
- `configure_scene_data.h/cpp`
- `configure_asset_config.h/cpp`

This follows the layering guidance (free functions in same library as provider) while organizing them in a dedicated subdirectory for clarity.

### Pattern Consistency

All migrated providers follow the same pattern:
```cpp
class IDataProvider {
  // Create: Returns new configured object
  virtual std::expected<DataType, FailInfo> CreateDataType() const = 0;
  
  // Configure: Configures existing object using free functions
  virtual std::expected<std::monostate, FailInfo> 
  ConfigureDataType(DataType& data) const = 0;
};
```

### Test Coverage

Each provider has two test files:
1. Provider integration tests: `Flatbuffers<Domain>Provider.test.cpp`
2. Free function unit tests: `configure_<domain>.test.cpp`

This ensures both the free functions and provider implementations are thoroughly tested.

---

## Phase 3: Configurator Migration

### ISceneConfigurator Analysis ⚠️

**Current State:**
- Interface: `ISceneConfigurator`
- Implementation: `FlatbuffersSceneConfigurator`
- Used by: `SceneFactory::CreateSceneFromSceneData()`

**Key Observation:**
`FlatbuffersSceneConfigurator` contains **NO FlatBuffers-specific logic**. All methods work with native `SceneData` structs, not FlatBuffers types. The implementation is completely generic.

**Current Implementation Analysis:**

1. **ConfigureSceneInfo()** - 7 lines
   - Generates UUID if nil
   - Copies scene type
   - **No FlatBuffers dependency**

2. **ConfigureSceneResources()** - 12 lines
   - Validates texture dimensions
   - Resizes render texture
   - **No FlatBuffers dependency**

3. **ConfigureSceneConfig()** - 2 lines
   - Empty (returns success)
   - **No FlatBuffers dependency**

4. **Base class methods in ISceneConfigurator:**
   - `ConfigureScene()` - Orchestrates configuration steps
   - `ImportEntities()` - Uses entity importer from SceneData
   - `PassAssetConfig()` - Loads assets
   - `ConfigureLogicMap()` - Creates logic from LogicFactory
   - **None are FlatBuffers-specific**

**Recommendation: Eliminate Configurator Abstraction**

The configurator abstraction provides **no value**:
- Only one implementation exists
- Implementation is not FlatBuffers-specific
- Could be free functions or methods on Scene
- Adds unnecessary complexity

**Proposed Refactoring:**

**Option A: Move to SceneFactory** (Recommended)
```cpp
// In SceneFactory
private:
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene& scene, const SceneData& scene_data);
  
  std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene& scene, const SceneData& scene_data);
  
  // ... other methods
```

**Benefits:**
- Removes unnecessary abstraction layer
- SceneFactory is the only caller
- Simpler code, easier to maintain
- Follows principle: don't create abstractions until needed

**Option B: Move to Scene class**
```cpp
// In Scene class
public:
  std::expected<std::monostate, FailInfo>
  Configure(const SceneData& scene_data);
```

**Benefits:**
- Scene configures itself
- More encapsulated
- SceneFactory just creates and calls Configure

**Option C: Keep but Rename**
- Remove "Flatbuffers" prefix
- Rename to just `SceneConfigurator`
- Acknowledge it's not data-source-specific

**Recommendation:**
**Option A (Move to SceneFactory)** because:
1. SceneFactory is the only user
2. Configuration is part of factory's responsibility
3. Removes unnecessary interface
4. Simpler overall design

---

### Task 3.1: Remove ISceneConfigurator Abstraction (NEW)

**Priority**: High (simplification opportunity identified)  
**Estimated effort**: 2-3 hours  
**Status**: ⏳ Ready to start

#### Rationale

`FlatbuffersSceneConfigurator` is misnamed - it contains **zero** FlatBuffers-specific logic. All methods work with native C++ `SceneData` structs. The abstraction provides no value and can be eliminated.

#### Migration Steps

1. **Move configuration methods to SceneFactory**
   - [ ] Add private methods to `SceneFactory`:
     - `ConfigureScene(Scene&, const SceneData&)`
     - `ConfigureSceneInfo(Scene&, const SceneData&)`
     - `ConfigureSceneResources(Scene&, const SceneData&)`
     - `ConfigureSceneConfig(Scene&, const SceneData&)`
     - `ImportEntities(Scene&, const SceneData&)`
     - `ConfigureLogicMap(Scene&)`
     - `PassAssetConfig(Scene&, const SceneData&)` (if still needed)
   
2. **Update SceneFactory::CreateSceneFromSceneData()**
   - [ ] Remove `GetSceneConfigurator()` call
   - [ ] Call configuration methods directly
   - [ ] Update logic to use new private methods

3. **Remove old files**
   - [ ] Delete `src/scenes/FlatbuffersSceneConfigurator.h`
   - [ ] Delete `src/scenes/FlatbuffersSceneConfigurator.cpp`
   - [ ] Delete `src/scenes/ISceneConfigurator.cpp`
   - [ ] Delete `src/types/interfaces/ISceneConfigurator.h`

4. **Update tests**
   - [ ] Update `tests/unit/scenes/SceneFactory.test.cpp`
   - [ ] Remove configurator-specific tests
   - [ ] Verify scene creation still works

5. **Update CMakeLists**
   - [ ] Remove configurator files from `src/scenes/CMakeLists.txt`

#### Files to Modify
- `src/scenes/SceneFactory.h` (add private methods)
- `src/scenes/SceneFactory.cpp` (implement moved methods)
- `tests/unit/scenes/SceneFactory.test.cpp` (update tests)
- `src/scenes/CMakeLists.txt` (remove old files)

#### Files to Delete
- `src/scenes/FlatbuffersSceneConfigurator.h`
- `src/scenes/FlatbuffersSceneConfigurator.cpp`
- `src/scenes/ISceneConfigurator.cpp`
- `src/types/interfaces/ISceneConfigurator.h`

#### Success Criteria
- [ ] All tests pass
- [ ] SceneFactory creates and configures scenes correctly
- [ ] No unnecessary abstraction layer
- [ ] Code is simpler and easier to understand
- [ ] Documentation updated

---

### Task 3.2: IUIElementConfigurator (Priority 8)

**Complexity**: High  
**Estimated effort**: 10-12 hours  
**Status**: ⏳ Waiting for 3.1

#### Current State
- Interface: `IUIElementConfigurator::CreateRootUIElement()`
- Implementation: `FlatbuffersUIElementConfigurator`
- Complex: handles multiple UI element types
- Has EventHandler dependency

#### Migration Steps

1. **Create free function files**
   - [ ] Create `src/user_interface/ui_element_config.h`
   - [ ] Create `src/user_interface/ui_element_config.cpp`

2. **Extract free functions by element type**
   - [ ] Extract `ConfigurePanelElement` → `config::configure_panel_element()`
   - [ ] Extract `ConfigureButtonElement` → `config::configure_button_element()`
   - [ ] Extract `ConfigureDropDownListElement` → `config::configure_dropdown_list_element()`
   - [ ] Extract other element configurations
   - [ ] Extract helper functions (ConvertLayout, ConvertSpacingAndSizing)

3. **Add comprehensive tests**
   - [ ] Create `tests/unit/user_interface/ui_element_config.test.cpp`
   - [ ] Test each element type configuration
   - [ ] Test helper functions
   - [ ] Test nested element creation
   - [ ] Test error cases

4. **Update configurator**
   - [ ] Refactor to use free functions
   - [ ] Simplify `CreateRootUIElement()`
   - [ ] Simplify `CreateUIElement()`

5. **Consider merging into Provider**
   - [ ] Evaluate if `IUIElementConfigurator` should merge with a UI provider
   - [ ] Or keep separate if it's purely a factory
   - [ ] Document decision

6. **Standardize naming**
   - [ ] Consider `ProvideRootUIElement()` or keep `Create*()`
   - [ ] Document naming decision

7. **Documentation**
   - [ ] Document UI element configuration pattern
   - [ ] Add examples for each element type

#### Success Criteria
- [ ] Free functions for all element types
- [ ] All tests pass
- [ ] Configurator simplified
- [ ] Clear pattern for adding new element types
- [ ] Documentation comprehensive

---

### Task 3.3: IEntityConfigurator (Priority 9)

**Complexity**: Very High  
**Estimated effort**: 16-20 hours  
**Status**: ⏳ Waiting for 3.2

#### Current State
- Interface: `IEntityConfigurator` with many methods:
  - `ConfigureEntityMemoryPool()`
  - `ConfigureFirstLayerComponents()`
  - `ConfigureSecondLayerComponents()`
  - `ConfigureComponent()` (base Component)
  - `ConfigureCUserInterface()`
  - `ConfigureCUIState()`
  - `ConfigureCGrimoireMachina()`
  - `ConfigureCMachinaForm()`
- Implementation: `FlatbuffersEntityConfigurator`
- Has EventHandler dependency
- Most complex configurator

#### Migration Steps

1. **Create free function files**
   - [ ] Create `src/entity/entity_config.h`
   - [ ] Create `src/entity/entity_config.cpp`
   - [ ] Consider separate files per component type

2. **Extract component configuration functions**
   - [ ] Extract base `ConfigureComponent` → `config::configure_component()`
   - [ ] Extract `ConfigureCUserInterface` → `config::configure_c_user_interface()`
   - [ ] Extract `ConfigureCUIState` → `config::configure_c_ui_state()`
   - [ ] Extract `ConfigureCGrimoireMachina` → `config::configure_c_grimoire_machina()`
   - [ ] Extract `ConfigureCMachinaForm` → `config::configure_c_machina_form()`

3. **Extract pool configuration functions**
   - [ ] Extract logic from `ConfigureEntityMemoryPool()`
   - [ ] Extract logic from `ConfigureFirstLayerComponents()`
   - [ ] Extract logic from `ConfigureSecondLayerComponents()`

4. **Add comprehensive tests**
   - [ ] Create `tests/unit/entity/entity_config.test.cpp`
   - [ ] Test base component configuration
   - [ ] Test each component type configuration
   - [ ] Test pool configuration
   - [ ] Test layered configuration
   - [ ] Test dependencies between layers
   - [ ] Test error cases

5. **Update configurator**
   - [ ] Refactor to use free functions
   - [ ] Simplify implementation
   - [ ] Keep interface for now

6. **Consider creating Entity Provider**
   - [ ] Evaluate creating `IEntityProvider`
   - [ ] Plan merger of configurator into provider
   - [ ] Document strategy

7. **Update call sites**
   - [ ] Find all uses
   - [ ] Plan updates carefully (complex)
   - [ ] Update incrementally

8. **Documentation**
   - [ ] Document entity configuration pattern
   - [ ] Document component registration
   - [ ] Document layered configuration approach
   - [ ] Add comprehensive examples

#### Success Criteria
- [ ] All component configurations extracted
- [ ] All tests pass
- [ ] Configurator significantly simplified
- [ ] Clear pattern for adding new components
- [ ] EventHandler dependency handled
- [ ] Documentation comprehensive
- [ ] Migration path clear

---

## Phase 4: Final Cleanup

### Task 4.1: Documentation Update

**Status**: ⏳ Waiting for Phase 3

- [ ] Update main README with Provider pattern
- [ ] Update architecture documentation
- [ ] Create migration guide for external users (if applicable)
- [ ] Add examples to documentation
- [ ] Update PROVIDER_INTERFACE_STANDARDIZATION.md status
- [ ] Update PROVIDER_IMPLEMENTATION_GUIDE.md with real examples

### Task 4.2: Code Review and Cleanup

**Status**: ⏳ Waiting for 4.1

- [ ] Review all migrated code
- [ ] Check for duplicated logic
- [ ] Verify naming consistency
- [ ] Check documentation completeness
- [ ] Look for further consolidation opportunities

### Task 4.3: Performance Review

**Status**: ⏳ Waiting for 4.2

- [ ] Check for performance regressions
- [ ] Verify no unnecessary copies
- [ ] Check allocation patterns
- [ ] Run performance benchmarks if available

### Task 4.4: Final Testing

**Status**: ⏳ Waiting for 4.3

- [ ] Run full test suite
- [ ] Check test coverage
- [ ] Add any missing tests
- [ ] Verify all tests pass

---

## Migration Metrics

Track progress with these metrics:

| Metric | Current | Target | Status |
|--------|---------|--------|--------|
| Providers using Create/Configure pattern | 5/6 | 6/6 | ⚠️ 83% (IFontProvider N/A) |
| Providers with free functions | 5/6 | 6/6 | ⚠️ 83% (IFontProvider N/A) |
| Providers with tests for free functions | 5/6 | 6/6 | ⚠️ 83% |
| Configurators merged or eliminated | 0/3 | 3/3 | ⏳ 0% |
| Documentation pages updated | 5/5 | 5/5 | ✅ 100% |
| Test coverage (config functions) | Good | >90% | ✅ Good |

### Phase Completion

| Phase | Status | Notes |
|-------|--------|-------|
| Phase 1: Foundation | ✅ Complete | All documentation created |
| Phase 2: Data Providers | ✅ Complete | All 5 providers migrated (IFontProvider N/A) |
| Phase 3: Configurators | ⏳ In Progress | ISceneConfigurator needs review/elimination |
| Phase 4: Cleanup | ⏳ Not Started | Awaiting Phase 3 |

### What's Been Completed

**Phase 2 Achievements:**
- ✅ IEngineDataProvider - migrated with free functions
- ✅ ISaveDataProvider - migrated with free functions
- ✅ ISceneManagerDataProvider - migrated with free functions
- ✅ IUIStyleDataProvider - migrated with free functions
- ✅ ISceneDataProvider - migrated with free functions
- ℹ️ IFontProvider - simple getter, no migration needed

**Key Improvements:**
- All providers use consistent Create/Configure pattern
- Configuration logic extracted to testable free functions
- Free functions organized in `src/data_providers/configure/` directory
- Comprehensive test coverage for all free functions
- Proper layering maintained (functions with providers)

## Risk Assessment

### High Risk
- **Entity Configurator Migration**: Most complex, many dependencies
  - Mitigation: Do last, extensive testing, incremental approach

### Medium Risk
- **Interface Changes**: Breaking changes for users
  - Mitigation: Maintain backward compatibility during migration, clear deprecation notices

### Low Risk
- **Free Function Extraction**: Internal refactoring
  - Mitigation: Good test coverage

## Timeline Estimate

| Phase | Tasks | Original Estimate | Actual Time | Status |
|-------|-------|-------------------|-------------|---------|
| Phase 1 | Foundation | 8 hours | ~8 hours | ✅ Complete |
| Phase 2 | Data Providers | 30-36 hours | ~25-30 hours | ✅ Complete |
| Phase 3 | Configurators | 32-40 hours | TBD | ⏳ In Progress |
| Phase 4 | Cleanup | 8-12 hours | TBD | ⏳ Not Started |
| **Total** | | **78-96 hours** | **~35-40 hours so far** | **~45% Complete** |

**Updated Estimate for Remaining Work:**
- Phase 3: ~15-20 hours (reduced due to simpler approach for ISceneConfigurator)
- Phase 4: ~8-12 hours
- **Remaining**: ~23-32 hours (~0.5-1 week)

## Notes

- This is analysis and planning work, not implementation
- Actual implementation should be done incrementally
- Each task should be a separate PR for easier review
- Tests must pass after each task
- Documentation should be updated with each task

## Review Checklist

Before considering migration complete:

- [ ] All providers use consistent `Provide()` naming
- [ ] All configuration logic extracted to testable free functions
- [ ] Free functions have comprehensive test coverage
- [ ] All configurators merged or justified as separate
- [ ] Documentation complete and accurate
- [ ] Examples provided for common patterns
- [ ] Performance impact assessed
- [ ] All tests pass
- [ ] Code review completed
- [ ] Migration guide created for users

## Questions for Discussion

1. Should we maintain backward compatibility during migration?
2. What's the acceptable timeline for this migration?
3. Should we create deprecation warnings before removing old interfaces?
4. How should we handle external users (if any) who depend on these interfaces?
5. Should we do all this in one large PR or multiple smaller PRs?

## Conclusion

This migration plan provides a clear path to standardizing all Provider and Configurator interfaces. By following this incremental approach, we can improve code quality while minimizing risk and maintaining functionality throughout the process.
