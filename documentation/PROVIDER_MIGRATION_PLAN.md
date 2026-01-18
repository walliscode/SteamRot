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

| Interface | Concrete Implementation | Complexity | Priority |
|-----------|------------------------|------------|----------|
| `IEngineDataProvider` | `FlatbuffersEngineDataProvider` | Low | 1 (Start here) |
| `ISaveDataProvider` | `FlatbuffersSaveDataProvider` | Low | 2 |
| `ISceneManagerDataProvider` | `FlatbuffersSceneManagerDataProvider` | Medium | 3 |
| `IUIStyleDataProvider` | `FlatbuffersUIStyleDataProvider` | Medium | 4 |
| `ISceneDataProvider` | `FlatbuffersSceneDataProvider` | High | 5 |
| `IFontProvider` | (AssetManager) | Low | 6 |

### Configurators (4 interfaces)

| Interface | Concrete Implementation | Complexity | Priority |
|-----------|------------------------|------------|----------|
| `ISceneConfigurator` | `FlatbuffersSceneConfigurator` | Medium | 7 |
| `IUIElementConfigurator` | `FlatbuffersUIElementConfigurator` | High | 8 |
| `IEntityConfigurator` | `FlatbuffersEntityConfigurator` | Very High | 9 |
| `IUIStyleConfigurator` | (Not implemented) | N/A | N/A |

## Phase 1: Foundation ✅ COMPLETE

- [x] Analyze current state
- [x] Document patterns and problems
- [x] Create proposal document
- [x] Create implementation guide
- [x] Create migration plan
- [x] Make templating decision (no templates)

## Phase 2: Data Provider Migration

### Task 2.1: IEngineDataProvider (Priority 1)

**Complexity**: Low  
**Estimated effort**: 4-6 hours  
**Status**: ⏳ Ready to start

#### Current State
- Interface: `IEngineDataProvider::LoadEngineData()`
- Implementation: `FlatbuffersEngineDataProvider`
- Private methods: `PopulateEngineResourcesConfig()`, `PopulateEngineConfig()`, `PopulateEngineState()`, `PopulateInitialAssetConfig()`

#### Migration Steps

1. **Create free function files**
   - [ ] Create `src/data_providers/engine_data_config.h`
   - [ ] Create `src/data_providers/engine_data_config.cpp`
   - [ ] Add to `src/data_providers/CMakeLists.txt`
   - [ ] **Note**: Place in `data_providers` library to maintain current layering (see PROVIDER_IMPLEMENTATION_GUIDE.md, "Layering and CMake Dependencies")

2. **Extract free functions**
   - [ ] Move `PopulateEngineResourcesConfig` → `config::configure_engine_resources_config()`
   - [ ] Move `PopulateEngineConfig` → `config::configure_engine_config()`
   - [ ] Move `PopulateEngineState` → `config::configure_engine_state()`
   - [ ] Move `PopulateInitialAssetConfig` → `config::configure_initial_asset_config()`

3. **Add tests for free functions**
   - [ ] Create `tests/unit/data_providers/engine_data_config.test.cpp`
   - [ ] Test `configure_engine_resources_config()`
   - [ ] Test `configure_engine_config()`
   - [ ] Test `configure_engine_state()`
   - [ ] Test `configure_initial_asset_config()`
   - [ ] Test null handling
   - [ ] Test missing required fields

4. **Update provider implementation**
   - [ ] Update `FlatbuffersEngineDataProvider` to use free functions
   - [ ] Remove private methods
   - [ ] Run existing provider tests
   - [ ] Verify all tests pass

5. **Rename interface method**
   - [ ] Update `IEngineDataProvider::LoadEngineData()` → `Provide()`
   - [ ] Update `FlatbuffersEngineDataProvider::LoadEngineData()` → `Provide()`
   - [ ] Find and update all call sites
   - [ ] Run all tests

6. **Documentation**
   - [ ] Update interface documentation
   - [ ] Add example to implementation guide
   - [ ] Mark as migrated in this document

#### Files to Modify
- `src/types/interfaces/IEngineDataProvider.h`
- `src/data_providers/FlatbuffersEngineDataProvider.h`
- `src/data_providers/FlatbuffersEngineDataProvider.cpp`
- Any files that instantiate or use `IEngineDataProvider`

#### Success Criteria
- [ ] All tests pass
- [ ] Free functions tested independently
- [ ] Interface uses `Provide()` method
- [ ] No duplicated configuration logic
- [ ] Documentation updated

---

### Task 2.2: ISaveDataProvider (Priority 2)

**Complexity**: Low  
**Estimated effort**: 3-4 hours  
**Status**: ⏳ Waiting for 2.1

#### Current State
- Interface: `ISaveDataProvider::ProvideSaveData()`
- Implementation: `FlatbuffersSaveDataProvider`
- Already uses `Provide*` naming (good!)

#### Migration Steps

1. **Create free function files**
   - [ ] Create `src/data_providers/save_data_config.h`
   - [ ] Create `src/data_providers/save_data_config.cpp`

2. **Extract free functions**
   - [ ] Identify configuration logic in `FlatbuffersSaveDataProvider`
   - [ ] Extract to `config::configure_save_data()`
   - [ ] Extract any sub-configurations

3. **Add tests**
   - [ ] Create `tests/unit/data_providers/save_data_config.test.cpp`
   - [ ] Test all configuration paths
   - [ ] Test error cases

4. **Update provider**
   - [ ] Refactor to use free functions
   - [ ] Run existing tests
   - [ ] Verify all tests pass

5. **Standardize naming**
   - [ ] Update to `Provide()` if needed (check current method name)
   - [ ] Update call sites

6. **Documentation**
   - [ ] Update documentation

#### Success Criteria
- [ ] All tests pass
- [ ] Free functions tested
- [ ] Consistent naming
- [ ] Documentation updated

---

### Task 2.3: ISceneManagerDataProvider (Priority 3)

**Complexity**: Medium  
**Estimated effort**: 4-5 hours  
**Status**: ⏳ Waiting for 2.2

#### Current State
- Interface: `ISceneManagerDataProvider::ProvideSceneManagerData()`
- Implementation: `FlatbuffersSceneManagerDataProvider`

#### Migration Steps

1. **Create free function files**
   - [ ] Create `src/data_providers/scene_manager_data_config.h`
   - [ ] Create `src/data_providers/scene_manager_data_config.cpp`

2. **Extract free functions**
   - [ ] Identify configuration logic
   - [ ] Extract to `config::configure_scene_manager_data()`

3. **Add tests**
   - [ ] Create test file
   - [ ] Test configurations
   - [ ] Test error handling

4. **Update provider**
   - [ ] Use free functions
   - [ ] Run tests

5. **Standardize naming**
   - [ ] Update to `Provide()` if needed

6. **Documentation**
   - [ ] Update docs

#### Success Criteria
- [ ] All tests pass
- [ ] Free functions tested
- [ ] Documentation updated

---

### Task 2.4: IUIStyleDataProvider (Priority 4)

**Complexity**: Medium  
**Estimated effort**: 5-6 hours  
**Status**: ⏳ Waiting for 2.3

#### Current State
- Interface: `IUIStyleDataProvider::ProvideUIStyles()`
- Implementation: `FlatbuffersUIStyleDataProvider`
- Has protected member: `fonts_map`
- Returns vector of styles

#### Migration Steps

1. **Create free function files**
   - [ ] Create `src/user_interface/ui_style_config.h`
   - [ ] Create `src/user_interface/ui_style_config.cpp`

2. **Extract free functions**
   - [ ] Extract single style configuration logic
   - [ ] Handle fonts_map dependency (pass as parameter)
   - [ ] Extract vector configuration logic

3. **Add tests**
   - [ ] Create test file
   - [ ] Test single style configuration
   - [ ] Test multiple styles
   - [ ] Test with font dependencies

4. **Update provider**
   - [ ] Use free functions
   - [ ] Pass fonts_map to free functions

5. **Standardize naming**
   - [ ] Review method naming
   - [ ] Update if needed

6. **Documentation**
   - [ ] Update docs

#### Success Criteria
- [ ] All tests pass
- [ ] Free functions handle dependencies
- [ ] Documentation updated

---

### Task 2.5: ISceneDataProvider (Priority 5)

**Complexity**: High  
**Estimated effort**: 8-10 hours  
**Status**: ⏳ Waiting for 2.4

#### Current State
- Interface: `ISceneDataProvider` with multiple methods:
  - `ProvideDefaultSceneData()`
  - `ProvideSceneDataFromData()`
- Implementation: `FlatbuffersSceneDataProvider`
- Has additional `Configure*` methods:
  - `ConfigureSceneInfo()`
  - `ConfigureSceneResourcesConfig()`
  - `ConfigureSceneDataFromData()`

#### Migration Steps

1. **Create free function files**
   - [ ] Create `src/scenes/scene_data_config.h`
   - [ ] Create `src/scenes/scene_data_config.cpp`

2. **Extract free functions**
   - [ ] Extract `ConfigureSceneInfo` → `config::configure_scene_info()`
   - [ ] Extract `ConfigureSceneResourcesConfig` → `config::configure_scene_resources_config()`
   - [ ] Extract `ConfigureSceneDataFromData` → `config::configure_scene_data_from_data()`
   - [ ] Consider additional helper functions

3. **Add comprehensive tests**
   - [ ] Create `tests/unit/scenes/scene_data_config.test.cpp`
   - [ ] Test each free function
   - [ ] Test SceneInfo configuration
   - [ ] Test SceneResourcesConfig configuration
   - [ ] Test complete SceneData configuration
   - [ ] Test error cases

4. **Update provider implementation**
   - [ ] Refactor to use free functions
   - [ ] Keep multiple Provide methods
   - [ ] Remove Configure methods or make them use free functions

5. **Standardize interface**
   - [ ] Keep `ProvideDefault()` and `ProvideFromData()` naming
   - [ ] Consider adding base `Provide()` method
   - [ ] Update documentation

6. **Update call sites**
   - [ ] Find all uses of `ISceneDataProvider`
   - [ ] Update if method signatures changed

7. **Documentation**
   - [ ] Document multiple Provide methods pattern
   - [ ] Update examples

#### Success Criteria
- [ ] All tests pass
- [ ] Free functions tested independently
- [ ] Multiple Provide methods work correctly
- [ ] EventHandler dependency handled properly
- [ ] Documentation clear

---

### Task 2.6: IFontProvider (Priority 6)

**Complexity**: Low  
**Estimated effort**: 2-3 hours  
**Status**: ⏳ Waiting for 2.5

#### Current State
- Interface: `IFontProvider::GetFont()`
- No separate concrete implementation (AssetManager implements)
- Simple getter interface

#### Migration Steps

1. **Analyze current usage**
   - [ ] Check if AssetManager is the only implementer
   - [ ] Determine if renaming is beneficial

2. **Consider renaming**
   - [ ] Evaluate `GetFont()` vs `ProvideFont()`
   - [ ] Check call sites for impact

3. **Make decision**
   - [ ] Option A: Rename to `ProvideFont()` for consistency
   - [ ] Option B: Keep `GetFont()` (acceptable for simple getter)
   - [ ] Document decision rationale

4. **Update if renaming**
   - [ ] Update interface
   - [ ] Update AssetManager
   - [ ] Update call sites

5. **Documentation**
   - [ ] Document IFontProvider pattern
   - [ ] Add note about getter vs provider methods

#### Success Criteria
- [ ] Decision documented
- [ ] Tests pass
- [ ] Documentation updated

---

## Phase 3: Configurator Migration

### Task 3.1: ISceneConfigurator (Priority 7)

**Complexity**: Medium  
**Estimated effort**: 6-8 hours  
**Status**: ⏳ Waiting for Phase 2

#### Current State
- Interface: `ISceneConfigurator` with methods:
  - `ConfigureScene()` (non-virtual, calls others)
  - `ImportEntities()` (non-virtual)
  - `ConfigureSceneInfo()` (virtual)
  - `ConfigureSceneResources()` (virtual)
  - `ConfigureSceneConfig()` (virtual)
  - `PassAssetConfig()` (non-virtual)
  - `ConfigureLogicMap()` (non-virtual)
- Implementation: `FlatbuffersSceneConfigurator`

#### Migration Steps

1. **Analyze overlap with ISceneDataProvider**
   - [ ] Identify duplicated responsibilities
   - [ ] Plan merger strategy

2. **Create free function files** (if not done in 2.5)
   - [ ] Create `src/scenes/scene_config.h`
   - [ ] Create `src/scenes/scene_config.cpp`

3. **Extract free functions**
   - [ ] Extract logic from `ConfigureSceneInfo()`
   - [ ] Extract logic from `ConfigureSceneResources()`
   - [ ] Extract logic from `ConfigureSceneConfig()`
   - [ ] Extract logic from `PassAssetConfig()`
   - [ ] Extract logic from `ConfigureLogicMap()`

4. **Add tests for free functions**
   - [ ] Create test file
   - [ ] Test each configuration function
   - [ ] Test complete scene configuration

5. **Merge into ISceneProvider**
   - [ ] Plan new combined interface
   - [ ] Add `Configure(Scene&, SceneData&)` method to `ISceneProvider`
   - [ ] Implement in `FlatbuffersSceneProvider`

6. **Update call sites**
   - [ ] Find all uses of `ISceneConfigurator`
   - [ ] Update to use `ISceneProvider::Configure()`
   - [ ] Verify all tests pass

7. **Deprecate old interface**
   - [ ] Mark `ISceneConfigurator` as deprecated
   - [ ] Plan removal for future release
   - [ ] Or remove if no external dependencies

8. **Documentation**
   - [ ] Document merged interface
   - [ ] Update examples

#### Success Criteria
- [ ] Functionality merged into Provider
- [ ] All tests pass
- [ ] No duplicated configuration logic
- [ ] Documentation updated
- [ ] Migration path clear for users

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

| Metric | Current | Target |
|--------|---------|--------|
| Providers using `Provide()` | 0/6 | 6/6 |
| Providers with free functions | 0/6 | 6/6 |
| Providers with tests for free functions | 0/6 | 6/6 |
| Configurators merged | 0/4 | 3/4 (IUIStyleConfigurator not implemented) |
| Documentation pages updated | 2/5 | 5/5 |
| Test coverage (config functions) | N/A | >90% |

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

| Phase | Tasks | Estimated Time | Dependencies |
|-------|-------|----------------|--------------|
| Phase 1 | Foundation | 8 hours | None (Complete ✅) |
| Phase 2 | Data Providers | 30-36 hours | Phase 1 |
| Phase 3 | Configurators | 32-40 hours | Phase 2 |
| Phase 4 | Cleanup | 8-12 hours | Phase 3 |
| **Total** | | **78-96 hours** | ~2-2.5 weeks |

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
