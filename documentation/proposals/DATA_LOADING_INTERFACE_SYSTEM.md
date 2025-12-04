# Data Loading Interface System - Comprehensive Analysis

## Executive Summary

This document provides a comprehensive analysis of implementing an interface-based data loading system for the SteamRot game engine. The goal is to abstract away serialization format details (FlatBuffers, JSON, XML, Lua, etc.) from the game code by introducing clean C++ structs and interface-based loaders.

## Table of Contents
1. [Current State Analysis](#current-state-analysis)
2. [Proposed Architecture](#proposed-architecture)
3. [Nomenclature System](#nomenclature-system)
4. [Configurators vs Data Providers](#configurators-vs-data-providers)
5. [Integration Points](#integration-points)
6. [Migration Strategy](#migration-strategy)
7. [Implementation Plan](#implementation-plan)

---

## Current State Analysis

### Existing Data Loading Patterns

#### 1. DataLoader Abstract Base Class
**Location**: `src/data_handlers/DataLoader.h`

```cpp
class DataLoader {
  virtual std::expected<Fragment, FailInfo>
  ProvideFragment(const std::string &fragment_name) const = 0;
  
  virtual std::expected<std::map<std::string, Fragment>, FailInfo>
  ProvideAllFragments(std::vector<std::string> fragment_names) const = 0;
};
```

**Analysis**:
- ✅ Already has abstract interface pattern
- ✅ Uses `std::expected` for error handling
- ❌ Only handles Fragment data
- ❌ Limited to 2 methods

#### 2. FlatbuffersDataLoader Implementation
**Location**: `src/data_handlers/FlatbuffersDataLoader.h`

```cpp
class FlatbuffersDataLoader : public DataLoader {
  // Inherits Fragment methods
  std::expected<const EngineData *, FailInfo> ProvideEngineData() const;
  std::expected<const SceneManagerData *, FailInfo> ProvideSceneManagerData() const;
  std::expected<const SceneDataData *, FailInfo> ProvideDefaultSceneData(SceneType) const;
  std::expected<const AssetCollection *, FailInfo> ProvideAssetData() const;
  std::expected<const UIStyleData *, FailInfo> ProvideUIStylesData(string) const;
  std::expected<const ContextData *, FailInfo> ProvideContextData() const;
  // ... and 6 more methods
};
```

**Issues**:
- ❌ Returns FlatBuffers pointers directly (`const EngineData *`)
- ❌ Exposes FlatBuffers types to consumers
- ❌ Non-virtual methods - can't be overridden for other formats
- ❌ Tightly coupled to FlatBuffers implementation
- ⚠️ Used in 7+ locations throughout codebase

**Usage Locations**:
1. `Engine::StartUp()` - loads EngineCoreData
2. `SceneFactory::CreateDefaultScene()` - loads SceneCoreData
3. `FlatbuffersConfigurator` - member variable for entity data
4. `AssetManager` - loads asset data
5. `StylesConfigurator` - loads UI style data
6. `FlatbuffersUserPreferencesProvider` - loads preferences

#### 3. Configuration Patterns

**Pattern A: Free Function Configurators** (`src/core/core_configuration.h`)
```cpp
namespace core {
  std::expected<std::monostate, FailInfo>
  ConfigureGameCore(GameCore &game_core, const EngineCoreData *core_data);
  
  std::expected<std::monostate, FailInfo>
  ConfigureSceneCore(SceneCore &scene_core, const SceneCoreData *scene_data);
}
```

**Analysis**:
- ✅ Clean separation: data loading vs configuration
- ✅ No state needed (free functions)
- ✅ Easy to test
- ❌ Still takes FlatBuffers types as input

**Pattern B: Class-Based Configurators** (`src/entity/FlatbuffersConfigurator.h`)
```cpp
class FlatbuffersConfigurator : public EntityConfigurator {
private:
  FlatbuffersDataLoader m_data_loader;  // Has state!
  
public:
  std::expected<std::monostate, FailInfo>
  ConfigureEntitiesFromDefaultData(EntityMemoryPool&, SceneType);
  
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(const UserInterfaceData *ui_data, CUserInterface &ui_component);
};
```

**Analysis**:
- ✅ Encapsulates configuration logic
- ✅ Overloaded methods for each component type
- ❌ Owns a DataLoader (stateful)
- ❌ Takes FlatBuffers types directly
- ⚠️ Name is format-specific (Flatbuffers)

**Pattern C: Provider Interfaces** (`src/configuration/ISaveDataProvider.h`)
```cpp
class ISaveDataProvider {
  virtual std::expected<SaveData, FailInfo> LoadSave(uint32_t slot) const = 0;
  virtual std::expected<std::monostate, FailInfo> SaveGame(const SaveData&) = 0;
};

// Clean C++ struct - no FlatBuffers!
struct SaveData {
  struct Metadata {
    std::string save_name;
    std::string created_at;
    uint32_t slot_index{0};
  } metadata;
  SceneType current_scene_type;
  uint32_t version{1};
};
```

**Analysis**:
- ✅✅✅ **This is the ideal pattern!**
- ✅ Interface defines contract
- ✅ Returns clean C++ structs
- ✅ Implementation handles format details
- ✅ Format-agnostic naming (`ISaveDataProvider`)
- ✅ Easy to add JSON/XML/Lua implementations

#### 4. StylesConfigurator Pattern
**Location**: `src/user_interface/styles/StylesConfigurator.h`

```cpp
class StylesConfigurator {
public:
  std::expected<UIStyle, FailInfo>
  ConfigureStyle(const UIStyleData &style_data, const AssetManager &asset_manager);
  
  std::expected<std::unordered_map<std::string, UIStyle>, FailInfo>
  ProvideUIStylesMap(const AssetManager &asset_manager, vector<string> style_names);
};
```

**Analysis**:
- ✅ Returns clean C++ objects (`UIStyle`)
- ⚠️ But input is still FlatBuffers (`UIStyleData`)
- ⚠️ Mixed responsibilities (Configure + Provide)

### Data Structures Analysis

#### FlatBuffers Schemas (28 total)
1. **Component Data**: `entities.fbs`, `user_interface.fbs`, `grimoire_machina.fbs`, `ui_state.fbs`
2. **Configuration Data**: `engine_config.fbs`, `context_data.fbs`, `core_data.fbs`
3. **Scene Data**: `scene_data.fbs`, `scene_manager_data.fbs`, `logic_data.fbs`
4. **Asset Data**: `assets.fbs`, `fragments.fbs`, `joints.fbs`
5. **Style Data**: `ui_style.fbs`
6. **Runtime Data**: `save_data.fbs`, `user_preferences.fbs`
7. **Event Data**: `events.fbs`, `event_bus_data.fbs`, `event_packet_data.fbs`, `subscriber_data.fbs`
8. **Test Data**: `test_data.fbs`, `simulation.fbs`, `event_test_data.fbs`, `input_test_data.fbs`
9. **Support Types**: `scene_types.fbs`, `types.fbs`, `user_input.fbs`

#### Components (Pure Data Structs)
**Location**: `src/components/`
- `Component` (base struct with `m_active`)
- `CUserInterface` - UI root element and visibility
- `CGrimoireMachina` - Fragment collection
- `CUIState` - UI state management
- `CMachinaForm` - Form data
- `CMeta` - Metadata
- `Fragment` - Already a clean C++ struct!
- `Joint` - Connection data

**Key Observation**: 
- Components are already clean C++ structs
- Only loaded via Configurators
- FlatBuffers types only appear in configuration code

---

## Proposed Architecture

### Design Principles

1. **Game code works with native C++ structs only**
   - No FlatBuffers types in game logic
   - No serialization format awareness
   - Simple, testable structures

2. **Interfaces abstract data sources**
   - Interface per data domain
   - Implementations handle format details
   - Easy to swap or add formats

3. **Clear separation of concerns**
   - Data loading (Providers/Loaders)
   - Configuration (Configurators)
   - Runtime usage (Game code)

4. **Format-agnostic naming**
   - Interfaces: `ISceneDataProvider` not `FlatbuffersSceneDataLoader`
   - Implementations: `FlatbuffersSceneDataProvider` specific
   - Data structs: `SceneData` not `SceneDataData`

### Three-Layer Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    Game Code Layer                       │
│  (Logic, Systems, Components - uses native structs)     │
└─────────────────────┬───────────────────────────────────┘
                      │ Uses native structs
                      ↓
┌─────────────────────────────────────────────────────────┐
│              Interface Layer (Abstract)                  │
│  - ISceneDataProvider                                   │
│  - IEngineDataProvider                                  │
│  - IAssetDataProvider                                   │
│  - IUserPreferencesProvider (already exists!)           │
│  - ISaveDataProvider (already exists!)                  │
└─────────────────────┬───────────────────────────────────┘
                      │ Implementations
                      ↓
┌─────────────────────────────────────────────────────────┐
│          Implementation Layer (Concrete)                 │
│  - FlatbuffersSceneDataProvider                         │
│  - FlatbuffersEngineDataProvider                        │
│  - JsonSceneDataProvider                                │
│  - LuaConfigProvider                                    │
└─────────────────────────────────────────────────────────┘
```

### Core Interfaces

#### 1. IEngineDataProvider
```cpp
// Clean C++ structs
struct EngineCoreData {
  uint32_t window_width{800};
  uint32_t window_height{600};
  std::string window_title{"SteamRot"};
  uint32_t framerate_limit{60};
  std::string environment_type{"Production"};
};

struct EngineData {
  EngineCoreData core;
  // Future: other engine-level data
};

// Interface
class IEngineDataProvider {
public:
  virtual ~IEngineDataProvider() = default;
  
  virtual std::expected<EngineCoreData, FailInfo>
  LoadEngineCoreData() const = 0;
  
  virtual std::expected<EngineData, FailInfo>
  LoadEngineData() const = 0;
};
```

#### 2. ISceneDataProvider
```cpp
// Clean C++ structs
struct SceneCoreData {
  uint32_t entity_pool_size{100};
  uint32_t render_texture_width{800};
  uint32_t render_texture_height{600};
};

struct SceneData {
  SceneType scene_type;
  std::string scene_id;
  SceneCoreData core;
  // Note: Entity data handled by EntityConfigurator
  // Note: Logic data handled by LogicFactory
  // Note: Assets handled by IAssetDataProvider
};

// Interface
class ISceneDataProvider {
public:
  virtual ~ISceneDataProvider() = default;
  
  virtual std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType scene_type) const = 0;
  
  virtual std::expected<SceneCoreData, FailInfo>
  LoadSceneCoreData(SceneType scene_type) const = 0;
};
```

#### 3. IAssetDataProvider
```cpp
struct AssetData {
  std::vector<std::string> texture_paths;
  std::vector<std::string> font_paths;
  std::vector<std::string> sound_paths;
  // etc.
};

class IAssetDataProvider {
public:
  virtual ~IAssetDataProvider() = default;
  
  virtual std::expected<AssetData, FailInfo>
  LoadAssetData() const = 0;
  
  virtual std::expected<AssetData, FailInfo>
  LoadSceneAssetData(SceneType scene_type) const = 0;
};
```

#### 4. IFragmentDataProvider
```cpp
// Fragment already exists as clean struct!
class IFragmentDataProvider {
public:
  virtual ~IFragmentDataProvider() = default;
  
  virtual std::expected<Fragment, FailInfo>
  LoadFragment(const std::string &fragment_name) const = 0;
  
  virtual std::expected<std::map<std::string, Fragment>, FailInfo>
  LoadAllFragments(const std::vector<std::string> &fragment_names) const = 0;
};
```

#### 5. IUIStyleDataProvider
```cpp
// UIStyle already exists as clean struct!
class IUIStyleDataProvider {
public:
  virtual ~IUIStyleDataProvider() = default;
  
  virtual std::expected<UIStyle, FailInfo>
  LoadStyle(const std::string &style_name) const = 0;
  
  virtual std::expected<std::unordered_map<std::string, UIStyle>, FailInfo>
  LoadAllStyles(const std::vector<std::string> &style_names) const = 0;
};
```

---

## Nomenclature System

### Naming Convention Rules

#### 1. Game-Facing Data Structs
**Pattern**: `{Domain}{Purpose}Data` or just `{Domain}Data`
- **Simplest name** - used most frequently by game code
- No format suffix (not `SceneDataData`)
- Descriptive domain prefix

**Examples**:
- `EngineData` - engine configuration
- `SceneData` - scene configuration
- `AssetData` - asset lists
- `SaveData` ✅ (already exists)
- `UserPreferences` ✅ (already exists)
- NOT: `EngineCoreDataFlatbuffers` ❌
- NOT: `SceneDataData` ❌ (current FlatBuffers name)

#### 2. Interface Names
**Pattern**: `I{Domain}DataProvider` or `I{Domain}Provider`
- Prefix with `I` for interface
- Domain-specific
- Suffix with `Provider` or `DataProvider`
- **No format mention**

**Examples**:
- `IEngineDataProvider`
- `ISceneDataProvider`
- `IAssetDataProvider`
- `IFragmentDataProvider`
- `IUIStyleDataProvider`
- `ISaveDataProvider` ✅ (already exists)
- `IUserPreferencesProvider` ✅ (already exists)

#### 3. Implementation Names
**Pattern**: `{Format}{Domain}DataProvider` or `{Format}{Domain}Provider`
- Format prefix (Flatbuffers, Json, Xml, Lua)
- Matches interface name but with format
- Implementation detail - rarely seen by game code

**Examples**:
- `FlatbuffersEngineDataProvider`
- `FlatbuffersSceneDataProvider`
- `JsonSceneDataProvider`
- `LuaConfigProvider`
- `FlatbuffersSaveDataProvider` ✅ (already exists)
- `FlatbuffersUserPreferencesProvider` ✅ (already exists)

#### 4. FlatBuffers Schema Names
**Pattern**: `{Domain}DataFbs` (generated files use `_generated.h`)
- Keep existing FlatBuffers names
- Add `Fbs` suffix to avoid conflicts
- Only used in implementation layer

**Examples**:
- `EngineCoreDataFbs` (from `core_data.fbs`)
- `SceneDataDataFbs` (from `scene_data.fbs`)
- `UserInterfaceDataFbs` (from `user_interface.fbs`)
- Maps to: `core_data_generated.h`, `scene_data_generated.h`

#### 5. Sub-structs and Nested Data
**Pattern**: Follow parent struct naming
```cpp
struct EngineData {
  struct CoreConfig {  // or EngineCoreConfig
    uint32_t window_width;
    uint32_t framerate_limit;
  } core;
  
  struct DisplayConfig {
    bool fullscreen;
    bool vsync;
  } display;
};
```

### Migration Mapping Table

| Current FlatBuffers Type | New Game Struct | Interface | FB Implementation |
|-------------------------|-----------------|-----------|-------------------|
| `EngineCoreData*` | `EngineCoreData` | `IEngineDataProvider` | `FlatbuffersEngineDataProvider` |
| `SceneDataData*` | `SceneData` | `ISceneDataProvider` | `FlatbuffersSceneDataProvider` |
| `SceneCoreData*` | `SceneCoreData` | `ISceneDataProvider` | `FlatbuffersSceneDataProvider` |
| `AssetCollection*` | `AssetData` | `IAssetDataProvider` | `FlatbuffersAssetDataProvider` |
| `EngineData*` | `EngineData` | `IEngineDataProvider` | `FlatbuffersEngineDataProvider` |
| `ContextData*` | `ContextData` | `IContextDataProvider` | `FlatbuffersContextDataProvider` |
| `UIStyleData*` | `UIStyle` ✅ | `IUIStyleDataProvider` | `FlatbuffersUIStyleProvider` |
| `UserPreferencesData*` | `UserPreferences` ✅ | `IUserPreferencesProvider` ✅ | `FlatbuffersUserPreferencesProvider` ✅ |
| `SaveDataData*` | `SaveData` ✅ | `ISaveDataProvider` ✅ | `FlatbuffersSaveDataProvider` ✅ |

✅ = Already follows proposed pattern

---

## Configurators vs Data Providers

### Decision Framework

Use **Data Provider (Interface)** when:
1. ✅ Loading data that's stored externally (files, network)
2. ✅ Format might vary (FlatBuffers, JSON, XML, Lua)
3. ✅ Data is **read** into native structs
4. ✅ Multiple implementations likely
5. ✅ Can be pure/stateless
6. ✅ Returns complete data structures

Use **Configurator (Class)** when:
1. ✅ Need to **transform** loaded data into runtime objects
2. ✅ Configuration logic is complex
3. ✅ Need access to other dependencies (EventHandler, AssetManager)
4. ✅ Modifying existing objects rather than creating new ones
5. ✅ Order of operations matters
6. ✅ May need internal state during configuration

Use **Free Function** when:
1. ✅ Simple, single-purpose configuration
2. ✅ No state needed
3. ✅ Just copying data into struct
4. ✅ Can be pure function

### Application to Current System

#### Data Providers (Interfaces)

| Domain | Interface | Rationale |
|--------|-----------|-----------|
| Engine Config | `IEngineDataProvider` | Static config loaded once, format may vary |
| Scene Config | `ISceneDataProvider` | Scene data could come from files, save games, or editor |
| Assets | `IAssetDataProvider` | Asset lists vary by environment/platform |
| Fragments | `IFragmentDataProvider` | Already has DataLoader pattern, just extend it |
| UI Styles | `IUIStyleDataProvider` | Styles could be defined in multiple formats |
| User Prefs | `IUserPreferencesProvider` ✅ | Already implemented perfectly! |
| Save Data | `ISaveDataProvider` ✅ | Already implemented perfectly! |

#### Configurators (Classes)

| Domain | Class | Rationale |
|--------|-------|-----------|
| Entities | `EntityConfigurator` | Complex, needs EventHandler, creates objects recursively |
| UI Elements | (part of EntityConfigurator) | Complex hierarchy, needs EventHandler for subscribers |
| Components | `EntityConfigurator::ConfigureComponent()` | Multiple overloads per component type |
| Styles | `StyleConfigurator` | Needs AssetManager for font loading |

#### Free Functions

| Domain | Function | Rationale |
|--------|----------|-----------|
| GameCore | `core::ConfigureGameCore()` | Simple data copy into existing struct |
| SceneCore | `core::ConfigureSceneCore()` | Simple data copy into existing struct |

### Existing Good Examples

**Already Follows Pattern** ✅
1. `ISaveDataProvider` / `FlatbuffersSaveDataProvider`
   - Interface defines contract
   - Returns clean `SaveData` struct
   - Implementation handles FlatBuffers

2. `IUserPreferencesProvider` / `FlatbuffersUserPreferencesProvider`
   - Interface defines contract  
   - Returns clean `UserPreferences` struct
   - Implementation handles FlatBuffers

**Needs Update** ⚠️
1. `FlatbuffersDataLoader`
   - Should be split into multiple providers
   - Should return native structs not FlatBuffers pointers
   - Should use interfaces

2. `FlatbuffersConfigurator`
   - Name is format-specific
   - Should become `EntityConfigurator` (already base class exists)
   - Should take native structs as input

---

## Integration Points

### Current Integration Points (Need Update)

#### 1. Engine::StartUp()
**Current**:
```cpp
FlatbuffersDataLoader data_loader;
auto engine_core_result = data_loader.ProvideEngineCoreData();
// Returns: const EngineCoreData* (FlatBuffers type!)
auto configure_core_result = core::ConfigureGameCore(m_game_core, engine_core_result.value());
```

**Proposed**:
```cpp
IEngineDataProvider& data_provider = GetEngineDataProvider();
auto engine_core_result = data_provider.LoadEngineCoreData();
// Returns: EngineCoreData (native struct!)
auto configure_core_result = core::ConfigureGameCore(m_game_core, engine_core_result.value());
```

**Changes**:
- Use interface reference instead of concrete class
- Method renamed `Load` instead of `Provide`
- Returns struct by value
- Free function signature updated to take native struct

#### 2. SceneFactory::CreateDefaultScene()
**Current**:
```cpp
FlatbuffersDataLoader data_loader;
auto scene_core_result = data_loader.ProvideSceneCoreData(scene_type);
// Returns: const SceneCoreData* (FlatBuffers type!)
auto configure_core_result = core::ConfigureSceneCore(scene_ptr->m_scene_core, scene_core_result.value());
```

**Proposed**:
```cpp
ISceneDataProvider& data_provider = GetSceneDataProvider();
auto scene_core_result = data_provider.LoadSceneCoreData(scene_type);
// Returns: SceneCoreData (native struct!)
auto configure_core_result = core::ConfigureSceneCore(scene_ptr->m_scene_core, scene_core_result.value());
```

#### 3. FlatbuffersConfigurator
**Current**:
```cpp
class FlatbuffersConfigurator : public EntityConfigurator {
private:
  FlatbuffersDataLoader m_data_loader;  // Concrete class!
  
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(const UserInterfaceData *ui_data,  // FlatBuffers type!
                    CUserInterface &ui_component);
};
```

**Proposed Option A - Keep Flatbuffers-specific**:
```cpp
class FlatbuffersEntityConfigurator : public EntityConfigurator {
private:
  FlatbuffersSceneDataProvider m_data_provider;
  
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(const UserInterfaceDataFbs *ui_data,  // Still FlatBuffers
                    CUserInterface &ui_component);
};
```

**Proposed Option B - Abstract away FlatBuffers** (Better!):
```cpp
// New native struct
struct UserInterfaceData {
  std::string ui_name;
  bool is_visible{false};
  // UIElement construction data
};

class EntityConfigurator {
private:
  ISceneDataProvider& m_data_provider;  // Interface!
  
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(const UserInterfaceData &ui_data,  // Native struct!
                    CUserInterface &ui_component);
};
```

**Recommendation**: Start with Option A, migrate to Option B
- Entities are complex, heavily FlatBuffers-dependent
- Phase 1: Keep configurators FlatBuffers-specific
- Phase 2: Create native component data structs
- Phase 3: Abstract configurators

#### 4. AssetManager
**Current**:
```cpp
// In AssetManager constructor/init
FlatbuffersDataLoader data_loader;
auto asset_result = data_loader.ProvideAssetData(scene_type);
// Returns: const AssetCollection* (FlatBuffers type!)
```

**Proposed**:
```cpp
IAssetDataProvider& asset_provider = GetAssetDataProvider();
auto asset_result = asset_provider.LoadSceneAssetData(scene_type);
// Returns: AssetData (native struct with paths)
```

#### 5. StylesConfigurator
**Current**:
```cpp
class StylesConfigurator {
  std::expected<UIStyle, FailInfo>
  ConfigureStyle(const UIStyleData &style_data,  // FlatBuffers type!
                const AssetManager &asset_manager);
};

// Usage
FlatbuffersDataLoader loader;
auto style_data = loader.ProvideUIStylesData(style_name);
StylesConfigurator configurator;
auto style = configurator.ConfigureStyle(*style_data.value(), asset_manager);
```

**Proposed**:
```cpp
class StylesConfigurator {
  std::expected<UIStyle, FailInfo>
  ConfigureStyle(const UIStyleConfig &style_config,  // Native struct!
                const AssetManager &asset_manager);
};

// Or even simpler - merge into provider
IUIStyleDataProvider& provider = GetUIStyleProvider();
auto style = provider.LoadStyle(style_name);
// Returns fully constructed UIStyle!
```

**Recommendation**: 
- StylesConfigurator is simple enough to merge into provider
- Provider can handle AssetManager dependency
- Single call: load style, return ready-to-use `UIStyle`

### Provider Injection Strategy

**Option 1: Constructor Injection** (Recommended)
```cpp
class Engine {
private:
  IEngineDataProvider& m_engine_data_provider;
  IUserPreferencesProvider& m_preferences_provider;
  
public:
  Engine(IEngineDataProvider& engine_provider,
         IUserPreferencesProvider& prefs_provider)
    : m_engine_data_provider(engine_provider),
      m_preferences_provider(prefs_provider) {}
};

// Usage
FlatbuffersEngineDataProvider engine_provider;
FlatbuffersUserPreferencesProvider prefs_provider;
Engine engine(engine_provider, prefs_provider);
```

**Option 2: Factory Function**
```cpp
IEngineDataProvider& GetEngineDataProvider() {
  static FlatbuffersEngineDataProvider provider;
  return provider;
}

// Usage
Engine engine;
auto data = GetEngineDataProvider().LoadEngineCoreData();
```

**Option 3: Service Locator** (For later)
```cpp
class ServiceLocator {
public:
  static IEngineDataProvider& GetEngineDataProvider();
  static ISceneDataProvider& GetSceneDataProvider();
  // etc.
};
```

**Recommendation**: Start with Factory Functions (Option 2)
- Simplest to integrate
- Minimal changes to existing code
- Can switch to Constructor Injection in Phase 2

---

## Migration Strategy

### Phase 1: Create Interfaces and Native Structs (Minimal Risk)

**Goal**: Establish new patterns alongside existing code

**Steps**:
1. Create new directory: `src/data_providers/`
2. Create interface headers:
   - `IEngineDataProvider.h` with `EngineCoreData` struct
   - `ISceneDataProvider.h` with `SceneData`, `SceneCoreData` structs
   - `IAssetDataProvider.h` with `AssetData` struct
   - `IFragmentDataProvider.h` (use existing `Fragment`)
   - `IUIStyleDataProvider.h` (use existing `UIStyle`)

3. Implement FlatBuffers providers:
   - `FlatbuffersEngineDataProvider` - delegates to old `FlatbuffersDataLoader`, converts types
   - `FlatbuffersSceneDataProvider` - delegates to old loader
   - `FlatbuffersAssetDataProvider` - delegates to old loader
   - `FlatbuffersFragmentDataProvider` - wraps existing Fragment loading
   - `FlatbuffersUIStyleProvider` - wraps existing style loading

4. Create factory functions in `src/data_providers/provider_factory.h`:
```cpp
IEngineDataProvider& GetEngineDataProvider();
ISceneDataProvider& GetSceneDataProvider();
IAssetDataProvider& GetAssetDataProvider();
// etc.
```

**Risk Level**: ⬜ LOW
- No changes to existing code
- New code only
- Can be tested independently

### Phase 2: Update Integration Points (Moderate Risk)

**Goal**: Switch usage sites to new interfaces

**Steps**:
1. Update `core::ConfigureGameCore()` signature:
```cpp
// Old
std::expected<std::monostate, FailInfo>
ConfigureGameCore(GameCore&, const EngineCoreData* fb_data);

// New
std::expected<std::monostate, FailInfo>
ConfigureGameCore(GameCore&, const EngineCoreData& native_data);
```

2. Update `Engine::StartUp()`:
```cpp
// Replace
FlatbuffersDataLoader data_loader;
auto result = data_loader.ProvideEngineCoreData();

// With
IEngineDataProvider& provider = GetEngineDataProvider();
auto result = provider.LoadEngineCoreData();
```

3. Update `SceneFactory::CreateDefaultScene()` similarly

4. Update `AssetManager` to use `IAssetDataProvider`

5. Update `StylesConfigurator` to use `IUIStyleDataProvider`

**Risk Level**: 🟡 MODERATE
- Changes existing code paths
- Needs thorough testing
- But changes are localized

### Phase 3: Deprecate Old FlatbuffersDataLoader (Low Risk)

**Goal**: Remove old loader, keep only new providers

**Steps**:
1. Mark `FlatbuffersDataLoader` methods as `[[deprecated]]`
2. Ensure all usages migrated to new providers
3. Remove `FlatbuffersDataLoader` class
4. Move FlatBuffers loading code into individual providers

**Risk Level**: ⬜ LOW
- Only cleanup
- All functionality already migrated
- Compile-time errors if missed any usage

### Phase 4: Entity Configuration (High Risk, Future)

**Goal**: Abstract entity data away from FlatBuffers

**Steps**:
1. Create native component data structs
2. Create `IEntityDataProvider` interface
3. Update `EntityConfigurator` to use native structs
4. Complex migration of entity/component system

**Risk Level**: 🔴 HIGH
- Extensive changes
- Many dependencies
- Recommend separate project

**Decision**: Defer to separate effort
- Current component system works well
- FlatBuffers integration is deep
- Benefits unclear vs effort required

### Migration Timeline

| Phase | Duration | Risk | Dependencies |
|-------|----------|------|--------------|
| Phase 1: Interfaces & Structs | 1-2 weeks | Low | None |
| Phase 2: Integration Points | 2-3 weeks | Moderate | Phase 1 |
| Phase 3: Deprecate Old Loader | 1 week | Low | Phase 2 |
| Phase 4: Entity Config | 4-6 weeks | High | Phases 1-3 |

**Recommended Approach**: 
- Complete Phases 1-3 (4-6 weeks total)
- Evaluate Phase 4 separately
- Delivers value without high-risk changes

---

## Implementation Plan

### Step 1: Create Directory Structure

```
src/
├── data_providers/           # NEW
│   ├── CMakeLists.txt
│   ├── IEngineDataProvider.h
│   ├── ISceneDataProvider.h
│   ├── IAssetDataProvider.h
│   ├── IFragmentDataProvider.h
│   ├── IUIStyleDataProvider.h
│   ├── FlatbuffersEngineDataProvider.h
│   ├── FlatbuffersEngineDataProvider.cpp
│   ├── FlatbuffersSceneDataProvider.h
│   ├── FlatbuffersSceneDataProvider.cpp
│   ├── FlatbuffersAssetDataProvider.h
│   ├── FlatbuffersAssetDataProvider.cpp
│   ├── FlatbuffersFragmentDataProvider.h
│   ├── FlatbuffersFragmentDataProvider.cpp
│   ├── FlatbuffersUIStyleProvider.h
│   ├── FlatbuffersUIStyleProvider.cpp
│   ├── provider_factory.h
│   └── provider_factory.cpp
├── data_handlers/            # EXISTING
│   ├── DataLoader.h          # Keep for now (deprecated later)
│   ├── FlatbuffersDataLoader.h  # Keep for now (deprecated later)
│   └── FlatbuffersDataLoader.cpp
└── configuration/            # EXISTING
    ├── ISaveDataProvider.h   # Good example!
    ├── IUserPreferencesProvider.h  # Good example!
    └── ...
```

### Step 2: Create Interface Headers (Example)

**File**: `src/data_providers/IEngineDataProvider.h`
```cpp
#pragma once
#include "FailInfo.h"
#include <cstdint>
#include <expected>
#include <string>

namespace steamrot {

/////////////////////////////////////////////////
/// @struct EngineCoreData
/// @brief Native C++ struct for engine core configuration.
///
/// This replaces the FlatBuffers EngineCoreData type in game code.
/// Provides a simple, mutable structure for engine configuration.
/////////////////////////////////////////////////
struct EngineCoreData {
  uint32_t window_width{800};
  uint32_t window_height{600};
  std::string window_title{"SteamRot"};
  uint32_t framerate_limit{60};
  std::string environment_type{"Production"};
};

/////////////////////////////////////////////////
/// @struct EngineData
/// @brief Complete engine data including core and other subsystems.
/////////////////////////////////////////////////
struct EngineData {
  EngineCoreData core;
  // Future: Add other engine-level configuration
};

/////////////////////////////////////////////////
/// @class IEngineDataProvider
/// @brief Interface for loading engine configuration data.
///
/// Implementations handle the actual data source (files, network, etc.)
/// and format (FlatBuffers, JSON, XML, Lua, etc.).
///
/// Usage:
/// ```cpp
/// IEngineDataProvider& provider = GetEngineDataProvider();
/// auto result = provider.LoadEngineCoreData();
/// if (result.has_value()) {
///   const EngineCoreData& data = result.value();
///   // Use native C++ struct
/// }
/// ```
/////////////////////////////////////////////////
class IEngineDataProvider {
public:
  virtual ~IEngineDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Load engine core configuration.
  ///
  /// @return Engine core data or failure information
  /////////////////////////////////////////////////
  virtual std::expected<EngineCoreData, FailInfo>
  LoadEngineCoreData() const = 0;

  /////////////////////////////////////////////////
  /// @brief Load complete engine data.
  ///
  /// @return Engine data or failure information
  /////////////////////////////////////////////////
  virtual std::expected<EngineData, FailInfo>
  LoadEngineData() const = 0;
};

} // namespace steamrot
```

### Step 3: Implement FlatBuffers Provider (Example)

**File**: `src/data_providers/FlatbuffersEngineDataProvider.h`
```cpp
#pragma once
#include "IEngineDataProvider.h"
#include "FlatbuffersDataLoader.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @class FlatbuffersEngineDataProvider
/// @brief FlatBuffers implementation of IEngineDataProvider.
///
/// Loads engine data from FlatBuffers binary files and converts
/// to native C++ structs for use by game code.
/////////////////////////////////////////////////
class FlatbuffersEngineDataProvider : public IEngineDataProvider {
private:
  FlatbuffersDataLoader m_loader;

public:
  FlatbuffersEngineDataProvider() = default;

  std::expected<EngineCoreData, FailInfo>
  LoadEngineCoreData() const override;

  std::expected<EngineData, FailInfo>
  LoadEngineData() const override;
};

} // namespace steamrot
```

**File**: `src/data_providers/FlatbuffersEngineDataProvider.cpp`
```cpp
#include "FlatbuffersEngineDataProvider.h"
#include "core_data_generated.h"

namespace steamrot {

std::expected<EngineCoreData, FailInfo>
FlatbuffersEngineDataProvider::LoadEngineCoreData() const {
  // Use existing loader
  auto fb_result = m_loader.ProvideEngineCoreData();
  if (!fb_result.has_value()) {
    return std::unexpected(fb_result.error());
  }

  const auto* fb_data = fb_result.value();

  // Convert FlatBuffers type to native struct
  EngineCoreData native_data;
  native_data.window_width = fb_data->window_width();
  native_data.window_height = fb_data->window_height();
  if (fb_data->window_title()) {
    native_data.window_title = fb_data->window_title()->str();
  }
  native_data.framerate_limit = fb_data->framerate_limit();
  if (fb_data->environment_type()) {
    native_data.environment_type = fb_data->environment_type()->str();
  }

  return native_data;
}

std::expected<EngineData, FailInfo>
FlatbuffersEngineDataProvider::LoadEngineData() const {
  auto core_result = LoadEngineCoreData();
  if (!core_result.has_value()) {
    return std::unexpected(core_result.error());
  }

  EngineData engine_data;
  engine_data.core = core_result.value();
  // Future: Load other engine data

  return engine_data;
}

} // namespace steamrot
```

### Step 4: Create Provider Factory

**File**: `src/data_providers/provider_factory.h`
```cpp
#pragma once
#include "IEngineDataProvider.h"
#include "ISceneDataProvider.h"
#include "IAssetDataProvider.h"
// ... other interfaces

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Get the engine data provider instance.
///
/// Returns a singleton instance of the FlatBuffers engine provider.
/// Future: Could be made configurable to return different implementations.
/////////////////////////////////////////////////
IEngineDataProvider& GetEngineDataProvider();

/////////////////////////////////////////////////
/// @brief Get the scene data provider instance.
/////////////////////////////////////////////////
ISceneDataProvider& GetSceneDataProvider();

/////////////////////////////////////////////////
/// @brief Get the asset data provider instance.
/////////////////////////////////////////////////
IAssetDataProvider& GetAssetDataProvider();

// ... other provider getters

} // namespace steamrot
```

**File**: `src/data_providers/provider_factory.cpp`
```cpp
#include "provider_factory.h"
#include "FlatbuffersEngineDataProvider.h"
#include "FlatbuffersSceneDataProvider.h"
#include "FlatbuffersAssetDataProvider.h"
// ... other implementations

namespace steamrot {

IEngineDataProvider& GetEngineDataProvider() {
  static FlatbuffersEngineDataProvider provider;
  return provider;
}

ISceneDataProvider& GetSceneDataProvider() {
  static FlatbuffersSceneDataProvider provider;
  return provider;
}

IAssetDataProvider& GetAssetDataProvider() {
  static FlatbuffersAssetDataProvider provider;
  return provider;
}

// ... other provider implementations

} // namespace steamrot
```

### Step 5: Update Integration Point (Example)

**File**: `src/engine/Engine.cpp`

**Before**:
```cpp
std::expected<std::monostate, FailInfo> Engine::StartUp() {
  FlatbuffersDataLoader data_loader;

  auto engine_core_result = data_loader.ProvideEngineCoreData();
  if (!engine_core_result) {
    return std::unexpected(engine_core_result.error());
  }

  auto configure_core_result =
      core::ConfigureGameCore(m_game_core, engine_core_result.value());
  // ...
}
```

**After**:
```cpp
#include "provider_factory.h"  // NEW

std::expected<std::monostate, FailInfo> Engine::StartUp() {
  IEngineDataProvider& data_provider = GetEngineDataProvider();  // CHANGED

  auto engine_core_result = data_provider.LoadEngineCoreData();  // CHANGED
  if (!engine_core_result) {
    return std::unexpected(engine_core_result.error());
  }

  auto configure_core_result =
      core::ConfigureGameCore(m_game_core, engine_core_result.value());
  // ...
}
```

**File**: `src/core/core_configuration.h`

**Before**:
```cpp
std::expected<std::monostate, FailInfo>
ConfigureGameCore(GameCore &game_core, const EngineCoreData *core_data);
```

**After**:
```cpp
std::expected<std::monostate, FailInfo>
ConfigureGameCore(GameCore &game_core, const EngineCoreData &core_data);  // Changed * to &
```

### Step 6: Testing Strategy

#### Unit Tests for Providers
**File**: `tests/unit/data_providers/FlatbuffersEngineDataProvider.test.cpp`
```cpp
#include "FlatbuffersEngineDataProvider.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersEngineDataProvider loads engine core data", 
          "[unit][data_providers]") {
  steamrot::FlatbuffersEngineDataProvider provider;
  
  auto result = provider.LoadEngineCoreData();
  REQUIRE(result.has_value());
  
  const auto& data = result.value();
  REQUIRE(data.window_width > 0);
  REQUIRE(data.window_height > 0);
  REQUIRE(!data.window_title.empty());
  REQUIRE(data.framerate_limit > 0);
}

TEST_CASE("FlatbuffersEngineDataProvider returns native struct",
          "[unit][data_providers]") {
  steamrot::FlatbuffersEngineDataProvider provider;
  
  auto result = provider.LoadEngineCoreData();
  REQUIRE(result.has_value());
  
  // Should be able to mutate the result
  auto data = result.value();
  data.window_width = 1920;
  data.window_title = "Modified";
  REQUIRE(data.window_width == 1920);
  REQUIRE(data.window_title == "Modified");
}
```

#### Integration Tests
**File**: `tests/integration/data_loading/engine_data_loading.integration.test.cpp`
```cpp
#include "Engine.h"
#include "provider_factory.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Engine loads data via provider interface",
          "[integration][data_loading]") {
  // Verify factory returns provider
  steamrot::IEngineDataProvider& provider = steamrot::GetEngineDataProvider();
  
  // Load data
  auto result = provider.LoadEngineCoreData();
  REQUIRE(result.has_value());
  
  // Verify Engine can use the data
  // (actual Engine test would go here)
}
```

---

## Summary and Recommendations

### Assessment of Proposed Plan

The proposed interface system is **well-suited** to the SteamRot architecture:

✅ **Strengths**:
1. **Already Proven** - `ISaveDataProvider` and `IUserPreferencesProvider` demonstrate the pattern works
2. **Incremental** - Can be implemented alongside existing code
3. **Low Risk** - Phases 1-3 have minimal impact on working systems
4. **Format Agnostic** - Easy to add JSON, Lua, or other formats later
5. **Cleaner Code** - Game code works with native structs, not FlatBuffers pointers
6. **Better Testing** - Can mock providers in tests

⚠️ **Cautions**:
1. **Entity System** - Deep FlatBuffers integration, defer to Phase 4
2. **Migration Effort** - 4-6 weeks for Phases 1-3
3. **Two Systems** - Temporary period with old and new code

### Final Recommendations

#### Immediate Actions (Phase 1)
1. ✅ Create `src/data_providers/` directory
2. ✅ Implement `IEngineDataProvider` + FlatBuffers implementation
3. ✅ Implement `ISceneDataProvider` + FlatBuffers implementation
4. ✅ Implement `IAssetDataProvider` + FlatBuffers implementation
5. ✅ Implement `IFragmentDataProvider` (migrate DataLoader)
6. ✅ Implement `IUIStyleDataProvider`
7. ✅ Create provider factory functions
8. ✅ Write unit tests for all providers

#### Short-term (Phase 2)
1. ✅ Update `Engine::StartUp()` to use new providers
2. ✅ Update `SceneFactory` to use new providers
3. ✅ Update `AssetManager` to use new providers
4. ✅ Update `StylesConfigurator` to use new providers
5. ✅ Update free function signatures in `core_configuration.h`
6. ✅ Write integration tests

#### Medium-term (Phase 3)
1. ✅ Deprecate `FlatbuffersDataLoader` methods
2. ✅ Verify all usages migrated
3. ✅ Remove old loader
4. ✅ Update documentation

#### Long-term (Phase 4 - Separate Project)
1. ⏸️ Evaluate entity system abstraction needs
2. ⏸️ Design native component data structs if needed
3. ⏸️ Migrate entity configurators if valuable

### Success Criteria

After Phases 1-3 complete:
- [ ] All engine/scene data loading uses provider interfaces
- [ ] Game code receives native C++ structs only
- [ ] No FlatBuffers types in public APIs (except entity system)
- [ ] Easy to add JSON provider as proof-of-concept
- [ ] All tests passing
- [ ] Documentation updated
- [ ] No performance regression

---

## Appendix: Code Examples

### Complete Provider Example

See implementation plan Step 3 above for full example.

### Migration Example

**Before**: Direct FlatBuffers usage
```cpp
FlatbuffersDataLoader loader;
auto fb_result = loader.ProvideEngineCoreData();
const EngineCoreData* fb_data = fb_result.value();  // FlatBuffers pointer

// Access via FlatBuffers API
uint32_t width = fb_data->window_width();
std::string title = fb_data->window_title()->str();
```

**After**: Native struct via provider
```cpp
IEngineDataProvider& provider = GetEngineDataProvider();
auto result = provider.LoadEngineCoreData();
const EngineCoreData& data = result.value();  // Native struct

// Access as regular struct
uint32_t width = data.window_width;
std::string title = data.window_title;
```

### JSON Provider Example (Future)

```cpp
class JsonEngineDataProvider : public IEngineDataProvider {
  std::expected<EngineCoreData, FailInfo>
  LoadEngineCoreData() const override {
    // Load JSON file
    std::ifstream file("engine_config.json");
    nlohmann::json j;
    file >> j;
    
    // Convert to native struct
    EngineCoreData data;
    data.window_width = j["window"]["width"];
    data.window_height = j["window"]["height"];
    data.window_title = j["window"]["title"];
    data.framerate_limit = j["framerate_limit"];
    data.environment_type = j["environment"];
    
    return data;
  }
};

// Usage - exactly the same!
IEngineDataProvider& provider = GetEngineDataProvider();
auto data = provider.LoadEngineCoreData();
```

---

## Document History

- **Version 1.0** - Initial comprehensive analysis
- **Author** - GitHub Copilot
- **Date** - December 2024
- **Status** - Proposal for Review
