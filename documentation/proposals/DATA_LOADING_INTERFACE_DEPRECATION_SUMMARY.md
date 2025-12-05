# Data Loading Interface - Deprecation Summary

## Overview

This document summarizes the deprecation work completed as part of Phase 3 of the DATA_LOADING_INTERFACE plan. The goal was to mark methods in `FlatbuffersDataLoader` that have been migrated to the new provider system as deprecated.

## Deprecated Methods in FlatbuffersDataLoader

The following methods have been marked with `[[deprecated]]` attributes:

### 1. ProvideEngineCoreData()
- **Status**: ✅ Deprecated  
- **Migrated To**: `IEngineDataProvider::LoadEngineCoreData()`
- **Implementation**: `FlatbuffersEngineDataProvider`
- **Usage**: Wrapped by the provider, not called directly in production code

### 2. ProvideSceneCoreData()
- **Status**: ✅ Deprecated
- **Migrated To**: `ISceneDataProvider::LoadSceneCoreData()`
- **Implementation**: `FlatbuffersSceneDataProvider`
- **Usage**: Wrapped by the provider, not called directly in production code

### 3. ProvideAssetData() (both overloads)
- **Status**: ✅ Deprecated
- **Migrated To**: `IAssetDataProvider::LoadAssetData()` and `LoadSceneAssetData()`
- **Implementation**: `FlatbuffersAssetDataProvider`
- **Usage**: Wrapped by the provider, not called directly in production code

### 4. ProvideUIStylesData()
- **Status**: ✅ Deprecated
- **Migrated To**: N/A (deferred - StylesConfigurator pattern is adequate)
- **Usage**: Still used internally by `StylesConfigurator::ProvideUIStylesMap()`
- **Note**: StylesConfigurator now accepts FlatbuffersDataLoader as a parameter instead of instantiating it internally

## Deprecated Classes

### 1. IGameDataProvider
- **Status**: ✅ Deprecated
- **Reason**: Replaced by domain-specific providers (IEngineDataProvider, ISceneDataProvider, etc.)
- **Usage**: Not used in production code (leftover from earlier abstraction attempt)

### 2. FlatBuffersGameDataProvider
- **Status**: ✅ Deprecated  
- **Reason**: Replaced by domain-specific providers (FlatbuffersEngineDataProvider, FlatbuffersSceneDataProvider, etc.)
- **Usage**: Not used in production code (leftover from earlier abstraction attempt)

## Methods NOT Deprecated

The following methods remain in `FlatbuffersDataLoader` and are NOT deprecated:

### 1. ProvideEngineData()
- **Reason**: Still used by `GameEngine::ConfigureEngineStateFromData()` for subscriptions and scene manager configuration
- **Status**: In use, not yet migrated

### 2. ProvideSceneManagerData()
- **Reason**: Used via `ProvideEngineData()`
- **Status**: In use, not yet migrated

### 3. ProvideDefaultSceneData()
- **Reason**: Used by `FlatbuffersConfigurator` (Phase 4 - entity system not yet migrated)
- **Status**: In use, not yet migrated

### 4. ProvideLogicCollectionData()
- **Reason**: Used by `SceneFactory::CreateDefaultScene()`
- **Status**: In use, not yet migrated

### 5. ProvideFragment() / ProvideAllFragments()
- **Reason**: Used by `FlatbuffersConfigurator` (Phase 4 - entity system)
- **Status**: In use, wrapped by `FlatbuffersFragmentDataProvider` but still called directly

### 6. ProvideDefaultUserPreferencesData()
- **Reason**: Used by `FlatbuffersUserPreferencesProvider`
- **Status**: In use, wrapped by existing provider

### 7. ProvideEngineConfigData()
- **Reason**: Used by deprecated `FlatBuffersGameDataProvider`
- **Status**: In use, but only by deprecated code

### 8. ProvideContextData()
- **Reason**: Used elsewhere in the codebase
- **Status**: In use, not yet migrated

## Code Changes Made

### FlatbuffersDataLoader.h
- Added `[[deprecated]]` attributes with helpful migration messages to:
  - `ProvideEngineCoreData()`
  - `ProvideSceneCoreData()`
  - `ProvideAssetData()` (both overloads)
  - `ProvideUIStylesData()`

### StylesConfigurator
- **Header (StylesConfigurator.h)**:
  - Added forward declaration for `FlatbuffersDataLoader`
  - Updated `ProvideUIStylesMap()` signature to accept `const FlatbuffersDataLoader &data_loader` parameter

- **Implementation (StylesConfigurator.cpp)**:
  - Removed internal instantiation of `FlatbuffersDataLoader`
  - Now receives `data_loader` as a parameter

### AssetManager.cpp
- Updated `LoadUIStyles()` to:
  - Instantiate `FlatbuffersDataLoader` 
  - Pass it to `StylesConfigurator::ProvideUIStylesMap()`

### IGameDataProvider.h
- Added `[[deprecated]]` attribute to `IGameDataProvider` class
- Updated documentation explaining replacement providers

### FlatBuffersGameDataProvider.h
- Added `[[deprecated]]` attribute to `FlatBuffersGameDataProvider` class
- Updated documentation explaining replacement providers

### Test Helpers

#### asset_test_helpers.h / asset_test_helpers.cpp
- Updated `CheckAssetConfiguration()` to use `IAssetDataProvider` instead of `FlatbuffersDataLoader`
- Updated `CheckFontConfiguration()` to accept `AssetData` instead of `AssetCollection*`
- Changed from FlatBuffers types to native C++ structs

#### entity_test_helpers.cpp
- No changes needed - uses `ProvideDefaultSceneData()` which is not deprecated

## Migration Guide for Developers

### If you see deprecation warnings for ProvideEngineCoreData():
```cpp
// Old (deprecated):
FlatbuffersDataLoader loader;
auto result = loader.ProvideEngineCoreData();
const EngineCoreDataFbs *fb_data = result.value();

// New (use provider):
IEngineDataProvider &provider = GetEngineDataProvider();
auto result = provider.LoadEngineCoreData();
const EngineCoreData &data = result.value(); // Native struct, not FlatBuffers
```

### If you see deprecation warnings for ProvideSceneCoreData():
```cpp
// Old (deprecated):
FlatbuffersDataLoader loader;
auto result = loader.ProvideSceneCoreData(scene_type);
const SceneCoreDataFbs *fb_data = result.value();

// New (use provider):
ISceneDataProvider &provider = GetSceneDataProvider();
auto result = provider.LoadSceneCoreData(scene_type);
const SceneCoreData &data = result.value(); // Native struct, not FlatBuffers
```

### If you see deprecation warnings for ProvideAssetData():
```cpp
// Old (deprecated):
FlatbuffersDataLoader loader;
auto result = loader.ProvideAssetData();
const AssetCollection *fb_data = result.value();

// New (use provider):
IAssetDataProvider &provider = GetAssetDataProvider();
auto result = provider.LoadAssetData();
const AssetData &data = result.value(); // Native struct, not FlatBuffers
```

### If you see deprecation warnings for IGameDataProvider:
Use domain-specific providers instead:
- `IEngineDataProvider` - for engine configuration
- `ISceneDataProvider` - for scene data
- `IAssetDataProvider` - for asset lists
- `IFragmentDataProvider` - for fragment geometries

## Remaining Work

### Phase 3 Cleanup (from checklist):
- [ ] Consider moving FlatBuffers loading logic directly into providers (optional)
- [ ] Update documentation (README.md, architecture docs)
- [ ] Final QA pass

### Phase 4 (Deferred - Entity System):
The following areas still use `FlatbuffersDataLoader` directly and would require significant refactoring:
- Entity/Component system (FlatbuffersConfigurator)
- Logic data loading (LogicCollectionData)
- Fragment loading (still used via FlatbuffersConfigurator)

## Testing Status

**Build Status**: ⚠️ Pre-existing build issues
- The project has a pre-existing build issue with clang++ and std::expected on Ubuntu 24.04
- This issue exists on the base branch and is not caused by these changes
- User builds locally where this likely works (possibly with g++ instead of clang++)

**Test Coverage**:
- Deprecated methods have `[[deprecated]]` attributes that will generate compiler warnings
- Test helpers updated to use new provider interfaces where appropriate
- Test helpers that use non-deprecated methods (like entity_test_helpers.cpp) were left unchanged

## Summary

Phase 3 deprecation work is **substantially complete**:

✅ **Completed**:
- Deprecated migrated methods in FlatbuffersDataLoader
- Deprecated old IGameDataProvider and FlatBuffersGameDataProvider classes
- Updated StylesConfigurator to accept FlatbuffersDataLoader as parameter
- Updated test helpers to use provider interfaces

⚠️ **Remaining**:
- Documentation updates (README.md, architecture docs)
- Optional: Move FlatBuffers loading logic into providers directly

🔮 **Future (Phase 4)**:
- Entity/component system migration
- Logic data migration
- Complete removal of FlatbuffersDataLoader

## References

- [DATA_LOADING_INTERFACE_CHECKLIST.md](DATA_LOADING_INTERFACE_CHECKLIST.md) - Phase 3 section
- [DATA_LOADING_INTERFACE_EXECUTIVE_SUMMARY.md](DATA_LOADING_INTERFACE_EXECUTIVE_SUMMARY.md)
- [DATA_LOADING_INTERFACE_SYSTEM.md](DATA_LOADING_INTERFACE_SYSTEM.md)

---

**Date**: December 5, 2024  
**Status**: Phase 3 Deprecation - Substantially Complete
