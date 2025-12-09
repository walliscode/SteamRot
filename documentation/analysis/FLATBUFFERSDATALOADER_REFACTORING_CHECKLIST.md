# FlatbuffersDataLoader Refactoring - Implementation Checklist

**Date**: December 2025  
**Status**: Ready to Execute  
**Goal**: Create 3 missing providers to prevent god object

---

## Overview

This checklist provides step-by-step instructions for completing the provider pattern refactoring. The work is divided into 3 phases that can be executed incrementally.

---

## Pre-Implementation Setup

- [ ] Read `FLATBUFFERSDATALOADER_GOD_OBJECT_ANALYSIS.md` (full context)
- [ ] Read `FLATBUFFERSDATALOADER_REFACTORING_QUICK_REF.md` (patterns)
- [ ] Review existing providers in `src/data_providers/` (see examples)
- [ ] Review `ISaveDataProvider.h` (best example of pattern)
- [ ] Ensure build system is working
- [ ] Ensure tests are passing

---

## Phase 1: IContextDataProvider (HIGH Priority)

### 1.1 Create Interface

**File**: `src/data_providers/IContextDataProvider.h`

- [ ] Create file with proper header guards
- [ ] Add file documentation (Doxygen style)
- [ ] Define `GameContextData` struct
  - [ ] Add `window_width` (uint32_t)
  - [ ] Add `window_height` (uint32_t)
  - [ ] Add `window_title` (std::string)
  - [ ] Add `framerate_limit` (uint32_t)
  - [ ] Add `scene_contexts` (std::map<SceneType, SceneContextData>)
- [ ] Define `SceneContextData` struct
  - [ ] Add `entity_pool_size` (uint32_t)
  - [ ] Add `render_texture_width` (uint32_t)
  - [ ] Add `render_texture_height` (uint32_t)
- [ ] Define `IContextDataProvider` interface
  - [ ] Add virtual destructor
  - [ ] Add `LoadContextData()` pure virtual method
  - [ ] Add `LoadSceneContext(SceneType)` pure virtual method
  - [ ] Add Doxygen documentation for all methods
- [ ] Use visual dividers (`////////////////////////////////////////////////////////////`)
- [ ] Follow SteamRot naming conventions

### 1.2 Create FlatBuffers Implementation Header

**File**: `src/data_providers/FlatbuffersContextDataProvider.h`

- [ ] Create file with proper header guards
- [ ] Add file documentation
- [ ] Include `IContextDataProvider.h`
- [ ] Include `FlatbuffersDataLoader.h`
- [ ] Define `FlatbuffersContextDataProvider` class
  - [ ] Inherit from `IContextDataProvider`
  - [ ] Add private `FlatbuffersDataLoader m_loader` member
  - [ ] Add default constructor
  - [ ] Override `LoadContextData()` method
  - [ ] Override `LoadSceneContext()` method
  - [ ] Add Doxygen documentation
- [ ] Use visual dividers
- [ ] Follow SteamRot naming conventions

### 1.3 Create FlatBuffers Implementation Source

**File**: `src/data_providers/FlatbuffersContextDataProvider.cpp`

- [ ] Create file with file documentation
- [ ] Include header file
- [ ] Include `context_data_generated.h` (FlatBuffers schema)
- [ ] Implement `LoadContextData()`:
  - [ ] Call `m_loader.ProvideContextData()`
  - [ ] Check `has_value()` and return error if needed
  - [ ] Get FlatBuffers pointer
  - [ ] Create `GameContextData` instance
  - [ ] Convert FlatBuffers fields → native struct (with null checks)
  - [ ] Load scene contexts from FlatBuffers data
  - [ ] Return native struct
- [ ] Implement `LoadSceneContext()`:
  - [ ] Load full context data
  - [ ] Extract scene-specific context by SceneType
  - [ ] Return scene context or error if not found
- [ ] Use visual dividers
- [ ] Follow SteamRot error handling patterns (std::expected)

### 1.4 Update CMakeLists.txt

**File**: `src/data_providers/CMakeLists.txt`

- [ ] Add `IContextDataProvider.h` to target sources
- [ ] Add `FlatbuffersContextDataProvider.h` to target sources
- [ ] Add `FlatbuffersContextDataProvider.cpp` to target sources

### 1.5 Write Unit Tests

**File**: `tests/data_providers/FlatbuffersContextDataProvider.test.cpp`

- [ ] Create test file
- [ ] Include Catch2 headers
- [ ] Include provider header
- [ ] Test: Load valid context data
  - [ ] Create provider instance
  - [ ] Call `LoadContextData()`
  - [ ] Verify result has value
  - [ ] Verify fields are populated correctly
- [ ] Test: Load scene context
  - [ ] Call `LoadSceneContext(SceneType::SceneType_TITLE)`
  - [ ] Verify result has value
  - [ ] Verify entity pool size
  - [ ] Verify render texture dimensions
- [ ] Test: Scene context not found
  - [ ] Call with invalid SceneType
  - [ ] Verify error returned
- [ ] Test: File not found error handling
  - [ ] Mock missing file scenario (if possible)
  - [ ] Verify appropriate error
- [ ] Tag tests with `[unit][IContextDataProvider]`

**File**: `tests/data_providers/CMakeLists.txt`

- [ ] Add test file to test executable

### 1.6 Update Consumers

**Locations to Update**:
- Context configuration system
- Scene setup code that uses context data

**For Each Consumer**:
- [ ] Replace `FlatbuffersDataLoader` usage with `IContextDataProvider`
- [ ] Update includes
- [ ] Update method calls (FlatBuffers → native struct)
- [ ] Update tests
- [ ] Verify functionality

### 1.7 Deprecate Old Method

**File**: `src/data_handlers/FlatbuffersDataLoader.h`

- [ ] Add `[[deprecated]]` attribute to `ProvideContextData()`:
  ```cpp
  [[deprecated("Use IContextDataProvider::LoadContextData instead")]]
  std::expected<const ContextData *, FailInfo> ProvideContextData() const;
  ```

### 1.8 Verification

- [ ] All tests pass
- [ ] No compiler warnings (except deprecation)
- [ ] Context system works as before
- [ ] Code review completed
- [ ] Documentation updated

---

## Phase 2: IUIStyleProvider (MEDIUM Priority)

### 2.1 Create Interface

**File**: `src/data_providers/IUIStyleProvider.h`

- [ ] Create file with proper header guards
- [ ] Add file documentation
- [ ] Define `UIStyleConfig` struct
  - [ ] Add `style_name` (std::string)
  - [ ] Add font properties (family, size, etc.)
  - [ ] Add color properties (text, background, etc.)
  - [ ] Add spacing properties (padding, margin, etc.)
  - [ ] Review `ui_style_generated.h` for all needed fields
- [ ] Define `IUIStyleProvider` interface
  - [ ] Add virtual destructor
  - [ ] Add `LoadUIStyle(const std::string& style_name)` method
  - [ ] Add Doxygen documentation
- [ ] Use visual dividers
- [ ] Follow naming conventions

### 2.2 Create FlatBuffers Implementation Header

**File**: `src/data_providers/FlatbuffersUIStyleProvider.h`

- [ ] Create file with proper header guards
- [ ] Add file documentation
- [ ] Include `IUIStyleProvider.h`
- [ ] Include `FlatbuffersDataLoader.h`
- [ ] Define `FlatbuffersUIStyleProvider` class
  - [ ] Inherit from `IUIStyleProvider`
  - [ ] Add private `FlatbuffersDataLoader m_loader` member
  - [ ] Add default constructor
  - [ ] Override `LoadUIStyle()` method
  - [ ] Add Doxygen documentation
- [ ] Use visual dividers

### 2.3 Create FlatBuffers Implementation Source

**File**: `src/data_providers/FlatbuffersUIStyleProvider.cpp`

- [ ] Create file with file documentation
- [ ] Include header file
- [ ] Include `ui_style_generated.h`
- [ ] Implement `LoadUIStyle()`:
  - [ ] Call `m_loader.ProvideUIStylesData(style_name)`
  - [ ] Check `has_value()`
  - [ ] Get FlatBuffers pointer
  - [ ] Create `UIStyleConfig` instance
  - [ ] Convert FlatBuffers fields → native struct (with null checks)
  - [ ] Return native struct
- [ ] Use visual dividers
- [ ] Follow error handling patterns

### 2.4 Update CMakeLists.txt

**File**: `src/data_providers/CMakeLists.txt`

- [ ] Add interface header
- [ ] Add implementation header
- [ ] Add implementation source

### 2.5 Write Unit Tests

**File**: `tests/data_providers/FlatbuffersUIStyleProvider.test.cpp`

- [ ] Create test file
- [ ] Test: Load valid style
- [ ] Test: Style not found
- [ ] Test: Field conversion
- [ ] Tag with `[unit][IUIStyleProvider]`

**File**: `tests/data_providers/CMakeLists.txt`

- [ ] Add test file

### 2.6 Update Consumers

**File**: `src/user_interface/styles/StylesConfigurator.cpp`

- [ ] Replace direct `FlatbuffersDataLoader` usage
- [ ] Use `IUIStyleProvider` interface
- [ ] Update includes
- [ ] Update method calls
- [ ] Update tests
- [ ] Verify UI styling works

### 2.7 Deprecate Old Method

**File**: `src/data_handlers/FlatbuffersDataLoader.h`

- [ ] Add `[[deprecated]]` to `ProvideUIStylesData()`:
  ```cpp
  [[deprecated("Use IUIStyleProvider::LoadUIStyle instead")]]
  std::expected<const UIStyleData *, FailInfo>
  ProvideUIStylesData(const std::string &style_name) const;
  ```

### 2.8 Verification

- [ ] All tests pass
- [ ] UI styles load correctly
- [ ] StylesConfigurator works as before
- [ ] Code review completed

---

## Phase 3: ILogicDataProvider (MEDIUM Priority)

### 3.1 Create Interface

**File**: `src/data_providers/ILogicDataProvider.h`

- [ ] Create file with proper header guards
- [ ] Add file documentation
- [ ] Define `LogicSystemConfig` struct
  - [ ] Add `system_name` (std::string)
  - [ ] Add `enabled` (bool)
  - [ ] Add `execution_order` (int32_t)
  - [ ] Review logic_data.fbs for fields
- [ ] Define `LogicConfiguration` struct
  - [ ] Add `scene_type` (SceneType)
  - [ ] Add `systems` (std::vector<LogicSystemConfig>)
- [ ] Define `ILogicDataProvider` interface
  - [ ] Add virtual destructor
  - [ ] Add `LoadLogicConfiguration(SceneType)` method
  - [ ] Add Doxygen documentation
- [ ] Use visual dividers

### 3.2 Create FlatBuffers Implementation Header

**File**: `src/data_providers/FlatbuffersLogicDataProvider.h`

- [ ] Create file with proper header guards
- [ ] Add file documentation
- [ ] Include `ILogicDataProvider.h`
- [ ] Include `FlatbuffersDataLoader.h`
- [ ] Define `FlatbuffersLogicDataProvider` class
  - [ ] Inherit from `ILogicDataProvider`
  - [ ] Add private `FlatbuffersDataLoader m_loader`
  - [ ] Add default constructor
  - [ ] Override `LoadLogicConfiguration()` method
  - [ ] Add Doxygen documentation
- [ ] Use visual dividers

### 3.3 Create FlatBuffers Implementation Source

**File**: `src/data_providers/FlatbuffersLogicDataProvider.cpp`

- [ ] Create file with file documentation
- [ ] Include header file
- [ ] Include `logic_data_generated.h`
- [ ] Implement `LoadLogicConfiguration()`:
  - [ ] Call `m_loader.ProvideLogicCollectionData(scene_type)`
  - [ ] Check `has_value()`
  - [ ] Get FlatBuffers pointer
  - [ ] Create `LogicConfiguration` instance
  - [ ] Convert FlatBuffers logic collection → native struct
  - [ ] Iterate through logic systems and convert
  - [ ] Return native struct
- [ ] Use visual dividers

### 3.4 Update CMakeLists.txt

**File**: `src/data_providers/CMakeLists.txt`

- [ ] Add interface header
- [ ] Add implementation header
- [ ] Add implementation source

### 3.5 Write Unit Tests

**File**: `tests/data_providers/FlatbuffersLogicDataProvider.test.cpp`

- [ ] Create test file
- [ ] Test: Load logic config for scene
- [ ] Test: Scene type not found
- [ ] Test: Conversion to native struct
- [ ] Test: System order preserved
- [ ] Tag with `[unit][ILogicDataProvider]`

**File**: `tests/data_providers/CMakeLists.txt`

- [ ] Add test file

### 3.6 Update Consumers

**Locations**:
- Logic factory system
- Scene logic setup code

**For Each Consumer**:
- [ ] Replace `FlatbuffersDataLoader` usage
- [ ] Use `ILogicDataProvider` interface
- [ ] Update includes
- [ ] Update method calls
- [ ] Update tests
- [ ] Verify logic systems work

### 3.7 Deprecate Old Method

**File**: `src/data_handlers/FlatbuffersDataLoader.h`

- [ ] Add `[[deprecated]]` to `ProvideLogicCollectionData()`:
  ```cpp
  [[deprecated("Use ILogicDataProvider::LoadLogicConfiguration instead")]]
  std::expected<const LogicCollectionData *, FailInfo>
  ProvideLogicCollectionData(const SceneType scene_type) const;
  ```

### 3.8 Verification

- [ ] All tests pass
- [ ] Logic systems load correctly
- [ ] Scene logic works as before
- [ ] Code review completed

---

## Phase 4: Documentation and Cleanup

### 4.1 Update Provider Factory

**File**: `src/data_providers/provider_factory.h` (if exists)

- [ ] Add factory methods for new providers:
  ```cpp
  std::unique_ptr<IContextDataProvider> CreateContextDataProvider();
  std::unique_ptr<IUIStyleProvider> CreateUIStyleProvider();
  std::unique_ptr<ILogicDataProvider> CreateLogicDataProvider();
  ```
- [ ] Implement in `provider_factory.cpp`

### 4.2 Update Documentation

**Files to Update**:
- [ ] `README.md` - Add note about provider pattern
- [ ] `documentation/architecture/DATA_LOADING_HIERARCHY.md` - Update with new providers
- [ ] GitHub Copilot instructions (if needed)

**New Documentation**:
- [ ] Add examples to `documentation/examples/` for each provider
- [ ] Update quick reference guides

### 4.3 Update Memory Store

- [ ] Store fact: "Use provider pattern for all data loading (IContextDataProvider, IUIStyleProvider, ILogicDataProvider). Never add methods to FlatbuffersDataLoader directly."
- [ ] Store fact: "FlatbuffersDataLoader is internal utility, not public API. Always wrap with providers."

### 4.4 Code Style Verification

- [ ] All files use 2-space indentation
- [ ] All methods have Doxygen documentation
- [ ] Visual dividers used consistently
- [ ] Naming follows conventions (PascalCase classes, snake_case files)
- [ ] Error handling uses std::expected pattern

### 4.5 Testing

**Unit Tests**:
- [ ] All provider tests pass
- [ ] Coverage > 80% for new code

**Integration Tests**:
- [ ] Test context system with new provider
- [ ] Test UI system with new provider
- [ ] Test logic system with new provider

**Regression Tests**:
- [ ] All existing tests still pass
- [ ] No functional changes (same behavior)

---

## Phase 5: Future Work (Optional)

### 5.1 Remove Deprecated Methods

**Timing**: After 1-2 releases with deprecation warnings

- [ ] Verify no usage of deprecated methods (check compiler warnings)
- [ ] Remove deprecated methods from FlatbuffersDataLoader:
  - [ ] `ProvideContextData()`
  - [ ] `ProvideUIStylesData()`
  - [ ] `ProvideLogicCollectionData()`
- [ ] Update tests
- [ ] Verify build

### 5.2 Make FlatbuffersDataLoader Private

**Timing**: After all consumers use providers

- [ ] Move to `src/data_providers/internal/`
- [ ] Remove public includes
- [ ] Only used by provider implementations
- [ ] Add comment: "Internal utility - do not use directly"

### 5.3 Add Alternative Implementations

**Examples**:
- [ ] `JSONContextDataProvider` (loads from JSON)
- [ ] `XMLUIStyleProvider` (loads from XML)
- [ ] `LuaLogicDataProvider` (loads from Lua scripts)
- [ ] Mock providers for testing

---

## Success Metrics

### Definition of Done

- [x] 3 new providers created (Context, UIStyle, Logic)
- [x] All interfaces follow naming conventions
- [x] All implementations wrap FlatbuffersDataLoader
- [x] All consumers updated to use providers
- [x] All tests passing (unit + integration)
- [x] Methods deprecated in FlatbuffersDataLoader
- [x] Documentation updated
- [x] Code review completed

### Success Indicators

- ✅ No direct usage of FlatbuffersDataLoader in game code
- ✅ All data access through provider interfaces
- ✅ Can add new data type using provider pattern (not FlatbuffersDataLoader)
- ✅ Team understands and adopts pattern
- ✅ God object problem prevented

---

## Risk Mitigation

### Potential Issues

1. **Breaking existing code**
   - **Mitigation**: Keep old methods during migration, deprecate gradually
   - **Mitigation**: Comprehensive testing at each step

2. **Performance concerns**
   - **Mitigation**: Minimal overhead (one extra function call)
   - **Mitigation**: Benchmark critical paths if needed

3. **Incomplete migration**
   - **Mitigation**: Follow checklist systematically
   - **Mitigation**: Use compiler warnings (deprecated) to find remaining usage

4. **Team confusion**
   - **Mitigation**: Good documentation (this checklist + analysis docs)
   - **Mitigation**: Code reviews to explain pattern

---

## Timeline

| Week | Phase | Tasks | Status |
|------|-------|-------|--------|
| 1 | Phase 1 | IContextDataProvider | ⬜ Not Started |
| 2 | Phase 2 | IUIStyleProvider | ⬜ Not Started |
| 2 | Phase 3 | ILogicDataProvider | ⬜ Not Started |
| 3 | Consumer Updates | Migrate all consumers | ⬜ Not Started |
| 3-4 | Phase 4 | Documentation & cleanup | ⬜ Not Started |

**Total Estimated Time**: 3-4 weeks

---

## Notes and Decisions Log

**Use this section to track decisions during implementation**:

### Decision Log
- [ ] Date: YYYY-MM-DD - Decision about X...
- [ ] Date: YYYY-MM-DD - Changed approach for Y...

### Issues Encountered
- [ ] Date: YYYY-MM-DD - Issue: ... Solution: ...

### Open Questions
- [ ] Question about native struct design for context data?
- [ ] Should we include additional fields in UIStyleConfig?

---

## References

- **Full Analysis**: `FLATBUFFERSDATALOADER_GOD_OBJECT_ANALYSIS.md`
- **Quick Reference**: `FLATBUFFERSDATALOADER_REFACTORING_QUICK_REF.md`
- **Existing Providers**: `src/data_providers/`
- **Best Example**: `ISaveDataProvider.h` / `FlatbuffersSaveDataProvider.h`
- **Naming Conventions**: `documentation/naming/`
- **Testing Patterns**: `documentation/testing/`

---

## Sign-off

**Pre-Implementation Review**:
- [ ] Checklist reviewed by: _______________
- [ ] Approach approved by: _______________
- [ ] Date: _______________

**Post-Implementation Review**:
- [ ] Implementation completed by: _______________
- [ ] Code review by: _______________
- [ ] Tests verified by: _______________
- [ ] Date: _______________
