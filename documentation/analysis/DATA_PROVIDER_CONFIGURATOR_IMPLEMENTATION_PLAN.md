# Data Provider & Configurator Implementation Plan

**Date:** December 12, 2025  
**Purpose:** Implementation plan for pure data_provider layer and configurator integration  
**Status:** Post-Refactor Analysis

---

## Table of Contents

1. [Current State Analysis](#current-state-analysis)
2. [Architecture Overview](#architecture-overview)
3. [Provider vs Configurator Responsibilities](#provider-vs-configurator-responsibilities)
4. [Implementation Plan](#implementation-plan)
5. [File Separation Strategy](#file-separation-strategy)
6. [Migration Checklist](#migration-checklist)

---

## Current State Analysis

### What's Already Done ✅

Based on recent commits, significant progress has been made:

1. **Types Library Created** (`src/types/`)
   - INTERFACE library with subdirectories: `core/`, `events/`, `flatbuffers/`, `user_interface/`
   - FailInfo, Fragment, Joint, EventPacket moved to types
   - Headers properly organized

2. **data_handlers Merged into data_providers**
   - `DataLoader`, `FlatbuffersDataLoader`, `data_file_utils`, `paths` now in data_providers
   - Consolidates data loading responsibility

3. **Existing Providers** (7 of 10)
   - ✅ `IEngineDataProvider` / `FlatbuffersEngineDataProvider`
   - ✅ `ISceneDataProvider` / `FlatbuffersSceneDataProvider`
   - ✅ `IAssetDataProvider` / `FlatbuffersAssetDataProvider`
   - ✅ `IFragmentDataProvider` / `FlatbuffersFragmentDataProvider`
   - ✅ `ISceneManagerDataProvider` / `FlatbuffersSceneManagerDataProvider`
   - ✅ `IUserPreferencesProvider` / `FlatbuffersUserPreferencesProvider` (in configuration)
   - ✅ `ISaveDataProvider` / `FlatbuffersSaveDataProvider` (in configuration)

4. **Configurators Established**
   - `IEntityConfigurator` / `FlatbuffersEntityConfigurator`
   - `ISceneConfigurator` / `FlatbuffersDefaultSceneConfigurator`
   - `StylesConfigurator` (UI styles)

### What's Remaining ❌

1. **Missing Providers** (2)
   - ❌ `IUIStyleProvider` / `FlatbuffersUIStyleProvider`
   - ❌ `ILogicDataProvider` / `FlatbuffersLogicDataProvider`
   - ~~`IContextDataProvider`~~ - **REMOVED** (context_data.fbs unused, will be deleted)

2. **Refactor FlatbuffersDataLoader**
   - Currently has 13+ Provide* methods
   - Should be reduced to low-level file loading only
   - Methods should migrate to respective providers

3. **Configurator-Provider Integration**
   - Configurators still directly use `FlatbuffersDataLoader`
   - Should use provider interfaces instead

4. **Remove Unused Context Data**
   - ❌ Delete `context_data.fbs` - not currently used
   - ❌ Remove `ProvideContextData()` from FlatbuffersDataLoader
   - ❌ Remove context_data_generated.h include

---

## Architecture Overview

### Three-Layer Data Flow

```
┌─────────────────────────────────────────────────────────────┐
│                      Application Layer                       │
│  (SceneFactory, EntityManager, Game Logic)                  │
└─────────────────┬───────────────────────────────────────────┘
                  │ Uses interfaces only
                  ▼
┌─────────────────────────────────────────────────────────────┐
│                    Configurator Layer                        │
│                                                              │
│  ┌──────────────────┐    ┌──────────────────┐             │
│  │ISceneConfigurator│    │IEntityConfigurator│             │
│  └────────┬─────────┘    └────────┬──────────┘             │
│           │                       │                         │
│           │ Uses providers        │ Uses providers          │
│           ▼                       ▼                         │
└─────────────────────────────────────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────────────────┐
│                     Provider Layer                           │
│                                                              │
│  ┌────────────┐  ┌────────────┐  ┌────────────┐           │
│  │ISceneData  │  │IEngineData │  │IAssetData  │           │
│  │Provider    │  │Provider    │  │Provider    │           │
│  └─────┬──────┘  └─────┬──────┘  └─────┬──────┘           │
│        │ Returns        │ Returns        │ Returns         │
│        │ native         │ native         │ native          │
│        │ structs        │ structs        │ structs         │
│        ▼                ▼                ▼                  │
└─────────────────────────────────────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────────────────┐
│                   Data Loading Layer                         │
│                                                              │
│  ┌──────────────────────────────────────────────┐          │
│  │ FlatbuffersDataLoader (internal to providers) │          │
│  │ - LoadBinaryFile()                            │          │
│  │ - VerifyBuffer()                              │          │
│  │ - Parse FlatBuffers (NO Provide* methods)    │          │
│  └──────────────────────────────────────────────┘          │
└─────────────────────────────────────────────────────────────┘
```

### Key Principles

1. **Providers own data loading**
   - Load FlatBuffers data
   - Convert to native C++ structs
   - Return through interface (I*Provider)

2. **Configurators consume providers**
   - Use provider interfaces (dependency injection)
   - Apply data to target objects (Scene, EntityMemoryPool)
   - Handle configuration logic

3. **Application uses configurators**
   - SceneFactory uses ISceneConfigurator
   - EntityManager uses IEntityConfigurator
   - No direct FlatBuffers access

---

## Provider vs Configurator Responsibilities

### Revised Definitions

**Provider:**
- **Constructs native C++ types** (structs, classes)
- **Converts data** from serialized format (FlatBuffers, JSON, XML) → native types
- **Returns data through interface** (I*Provider)
- **Does NOT modify existing objects**

**Configurator:**
- **Takes an existing object** as parameter
- **Modifies/configures the object** with data
- **May use Providers** to get data, or work directly with serialized data
- **Returns success/failure**, not new objects

### Key Distinction

```cpp
// PROVIDER: Constructs and returns new object
class IAssetDataProvider {
  virtual std::expected<AssetCollection, FailInfo>
    LoadAssets() const = 0;  // Returns NEW AssetCollection
};

// CONFIGURATOR: Modifies existing object
class ISceneConfigurator {
  virtual std::expected<std::monostate, FailInfo>
    ConfigureScene(Scene& scene, SceneType type) = 0;  // Modifies scene
};
```

### Provider Responsibilities

**What Providers Do:**
- Load data from source (file, network, database)
- Parse serialized format (FlatBuffers, JSON, XML)
- **Construct native C++ structs/objects**
- **Convert/transform data to native types**
- Return through interface

**What Providers DON'T Do:**
- Modify existing objects
- Manage object lifecycle beyond construction
- Handle business logic
- Know about Scene, Entity, or other game objects (except as return types)

**Example:**
```cpp
// Provider constructs and returns new data
class ISceneDataProvider {
  virtual std::expected<SceneData, FailInfo>
    LoadSceneData(SceneType scene_type) const = 0;
};

// Implementation
std::expected<SceneData, FailInfo>
FlatbuffersSceneDataProvider::LoadSceneData(SceneType scene_type) const {
  auto fb_data = m_loader.LoadFlatBuffers<SceneDataFbs>("scenes/title.bin");
  
  // CONSTRUCT new SceneData object
  SceneData data;
  data.scene_type = scene_type;
  data.scene_id = fb_data->scene_id()->str();
  data.render_texture_width = fb_data->width();
  
  return data;  // Returns NEW object
}
```

### Configurator Responsibilities

**What Configurators Do:**
- **Accept existing object** to configure
- May accept provider interfaces (dependency injection)
- May work directly with FlatBuffers for complex data
- **Modify/configure the existing object**
- Handle configuration logic (validation, defaults)
- Return success/failure

**What Configurators DON'T Do:**
- Construct and return new objects (that's Provider's job)
- Load files directly (use Provider or FlatbuffersDataLoader internally)

**Example:**
```cpp
// Configurator modifies existing object
class FlatbuffersDefaultSceneConfigurator : public ISceneConfigurator {
private:
  ISceneDataProvider& m_scene_data_provider;
  
public:
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene& scene, SceneType scene_type) override {
    // Get NEW data from provider
    auto scene_data = m_scene_data_provider.LoadSceneData(scene_type);
    
    // MODIFY existing scene object
    scene.m_scene_info.scene_type = scene_data->scene_type;
    scene.m_scene_info.scene_id = scene_data->scene_id;
    scene.m_scene_resources.render_texture_width = 
      scene_data->render_texture_width;
    
    return std::monostate{};  // Returns success, not object
  }
};
```

### Why This Separation Matters

1. **Clear Ownership**
   - Providers: Construct new data
   - Configurators: Modify existing objects

2. **Single Responsibility Principle**
   - Providers: Data loading and conversion to native types
   - Configurators: Object configuration logic

3. **Testability**
   - Can mock providers for configurator tests
   - Can test providers independently

4. **Flexibility**
   - Swap data sources without changing configurators
   - Swap configuration logic without changing providers

5. **Reduced Coupling**
   - Providers create native types (no game object dependencies)
   - Configurators use interfaces (no data format dependencies)

---

## Implementation Plan

### Phase 1: Create Missing Providers (Week 1)

**Goal:** Complete the provider pattern to 100%

#### 1.1 Create IContextDataProvider

**New Files:**
- `src/data_providers/IContextDataProvider.h`
- `src/data_providers/FlatbuffersContextDataProvider.h`
- `src/data_providers/FlatbuffersContextDataProvider.cpp`

**Native Data Struct:**
```cpp
// In src/types/core/ContextData.h (NEW)
namespace steamrot {
struct ContextConfigData {
  uint32_t entity_pool_size{1000};
  uint32_t render_texture_width{800};
  uint32_t render_texture_height{600};
};
}
```

**Interface:**
```cpp
// src/data_providers/IContextDataProvider.h
class IContextDataProvider {
public:
  virtual ~IContextDataProvider() = default;
  
  virtual std::expected<ContextConfigData, FailInfo>
    LoadContextConfig() const = 0;
};
```

**Implementation:**
```cpp
// src/data_providers/FlatbuffersContextDataProvider.cpp
std::expected<ContextConfigData, FailInfo>
FlatbuffersContextDataProvider::LoadContextConfig() const {
  auto fb_result = m_loader.ProvideContextData();
  if (!fb_result.has_value()) {
    return std::unexpected(fb_result.error());
  }
  
  const auto* fb_data = fb_result.value();
  ContextConfigData native_data;
  native_data.entity_pool_size = fb_data->entity_pool_size();
  // ... convert other fields
  
  return native_data;
}
```

#### 1.2 Create IUIStyleProvider

**New Files:**
- `src/data_providers/IUIStyleProvider.h`
- `src/data_providers/FlatbuffersUIStyleProvider.h`
- `src/data_providers/FlatbuffersUIStyleProvider.cpp`

**Native Data Struct:**
```cpp
// In src/types/user_interface/UIStyleData.h (NEW)
namespace steamrot {
struct UIStyleData {
  std::string style_name;
  sf::Color background_color;
  sf::Color text_color;
  uint32_t font_size;
  // ... other style properties
};
}
```

**Interface:**
```cpp
class IUIStyleProvider {
public:
  virtual ~IUIStyleProvider() = default;
  
  virtual std::expected<UIStyleData, FailInfo>
    LoadUIStyle(const std::string& style_name) const = 0;
};
```

#### 1.3 Create ILogicDataProvider

**New Files:**
- `src/data_providers/ILogicDataProvider.h`
- `src/data_providers/FlatbuffersLogicDataProvider.h`
- `src/data_providers/FlatbuffersLogicDataProvider.cpp`

**Native Data Struct:**
```cpp
// In src/types/core/LogicData.h (NEW)
namespace steamrot {
struct LogicCollectionData {
  SceneType scene_type;
  std::vector<std::string> action_logics;
  std::vector<std::string> collision_logics;
  std::vector<std::string> render_logics;
  std::vector<std::string> movement_logics;
};
}
```

**Interface:**
```cpp
class ILogicDataProvider {
public:
  virtual ~ILogicDataProvider() = default;
  
  virtual std::expected<LogicCollectionData, FailInfo>
    LoadLogicData(SceneType scene_type) const = 0;
};
```

### Phase 2: Refactor FlatbuffersDataLoader (Week 1-2)

**Goal:** Reduce FlatbuffersDataLoader to core loading functionality

#### 2.1 Identify Methods to Move

**Current FlatbuffersDataLoader methods:**
```cpp
// KEEP (low-level loading)
LoadBinaryFile(path)
VerifyBuffer(buffer)

// MOVE to ISceneDataProvider
ProvideDefaultSceneData(scene_type) → LoadSceneData()

// MOVE to IAssetDataProvider  
ProvideAssetData() → LoadAssetData()
ProvideAssetData(scene_type) → LoadSceneAssets()

// MOVE to IUIStyleProvider (NEW)
ProvideUIStylesData(style_name) → LoadUIStyle()

// MOVE to IContextDataProvider (NEW)
ProvideContextData() → LoadContextConfig()

// MOVE to ILogicDataProvider (NEW)
ProvideLogicCollectionData(scene_type) → LoadLogicData()

// MOVE to IEngineDataProvider (already done)
ProvideEngineResourcesConfigFbs() → LoadEngineResourcesConfig()
ProvideEngineConfigFbs() → LoadEngineConfig()
ProvideEngineStateFbs() → LoadEngineState()

// MOVE to ISceneManagerDataProvider (already done)
ProvideSceneManagerData() → LoadSceneManagerData()

// MOVE to IFragmentDataProvider (already done)
ProvideFragment(name) → LoadFragment()
ProvideAllFragments(names) → LoadAllFragments()

// MOVE to IUserPreferencesProvider (already in configuration)
ProvideDefaultUserPreferencesData() → LoadUserPreferences()
```

#### 2.2 Refactored FlatbuffersDataLoader

**After refactor:**
```cpp
// src/data_providers/FlatbuffersDataLoader.h
class FlatbuffersDataLoader {
public:
  // Low-level file loading
  std::expected<std::vector<uint8_t>, FailInfo>
    LoadBinaryFile(const std::string& relative_path) const;
  
  // Buffer verification
  bool VerifyBuffer(const uint8_t* buffer, size_t size) const;
  
  // Template for type-safe FlatBuffers loading
  template<typename T>
  std::expected<const T*, FailInfo>
    LoadFlatBuffersData(const std::string& relative_path) const {
    auto file_result = LoadBinaryFile(relative_path);
    if (!file_result.has_value()) {
      return std::unexpected(file_result.error());
    }
    
    const auto& buffer = file_result.value();
    if (!VerifyBuffer(buffer.data(), buffer.size())) {
      return std::unexpected(FailInfo{
        FailMode::FlatbuffersDataNotFound,
        "Invalid FlatBuffers data"
      });
    }
    
    return flatbuffers::GetRoot<T>(buffer.data());
  }

private:
  // Helper for path resolution
  std::string ResolvePath(const std::string& relative_path) const;
};
```

**Result:** Simple, focused class used internally by all providers.

### Phase 3: Update Configurators to Use Providers (Week 2)

**Goal:** Remove FlatbuffersDataLoader from configurators

#### 3.1 Refactor FlatbuffersDefaultSceneConfigurator

**Current (BAD):**
```cpp
class FlatbuffersDefaultSceneConfigurator : public ISceneConfigurator {
private:
  FlatbuffersDataLoader m_data_loader;  // Direct dependency
  
public:
  std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene& scene, SceneType scene_type) override {
    // Loads FlatBuffers directly
    auto fb_result = m_data_loader.ProvideDefaultSceneData(scene_type);
    // ...
  }
};
```

**Refactored (GOOD):**
```cpp
class FlatbuffersDefaultSceneConfigurator : public ISceneConfigurator {
private:
  ISceneDataProvider& m_scene_data_provider;
  IAssetDataProvider& m_asset_data_provider;
  
public:
  FlatbuffersDefaultSceneConfigurator(
      ISceneDataProvider& scene_provider,
      IAssetDataProvider& asset_provider)
    : m_scene_data_provider(scene_provider),
      m_asset_data_provider(asset_provider) {}
  
  std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene& scene, SceneType scene_type) override {
    // Uses provider interface
    auto scene_data = m_scene_data_provider.LoadSceneData(scene_type);
    if (!scene_data.has_value()) {
      return std::unexpected(scene_data.error());
    }
    
    // Apply native struct data
    scene.m_scene_resources.render_texture_width = 
      scene_data->render_texture_width;
    scene.m_scene_resources.render_texture_height = 
      scene_data->render_texture_height;
    
    return std::monostate{};
  }
};
```

#### 3.2 Refactor FlatbuffersEntityConfigurator

**Current (has FlatBuffers in signature):**
```cpp
class FlatbuffersEntityConfigurator : public IEntityConfigurator {
private:
  const EntityCollectionFbs& m_entity_collection_data;  // FlatBuffers type
  
public:
  FlatbuffersEntityConfigurator(
      EventHandler& event_handler,
      const EntityCollectionFbs& entity_collection_data);
};
```

**Issue:** FlatBuffers type leaks into interface

**Options for refactoring:**

**Option A: Provider-Based (RECOMMENDED)**
```cpp
class FlatbuffersEntityConfigurator : public IEntityConfigurator {
private:
  IEntityDataProvider& m_entity_data_provider;
  SceneType m_scene_type;
  
public:
  FlatbuffersEntityConfigurator(
      EventHandler& event_handler,
      IEntityDataProvider& entity_provider,
      SceneType scene_type)
    : IEntityConfigurator(event_handler),
      m_entity_data_provider(entity_provider),
      m_scene_type(scene_type) {}
  
  std::expected<std::monostate, FailInfo>
  ConfigureEntities(EntityMemoryPool& pool) override {
    // Load entity data via provider
    auto entity_data = m_entity_data_provider.LoadEntityData(m_scene_type);
    if (!entity_data.has_value()) {
      return std::unexpected(entity_data.error());
    }
    
    // Configure entities with native data
    for (const auto& entity : entity_data->entities) {
      ConfigureEntity(pool, entity);
    }
    
    return std::monostate{};
  }
};
```

**Option B: Keep FlatBuffers internally (ACCEPTABLE)**
```cpp
// Keep current design but hide FlatBuffers in .cpp
class FlatbuffersEntityConfigurator : public IEntityConfigurator {
private:
  // Forward declaration in header
  struct Impl;
  std::unique_ptr<Impl> m_impl;
  
public:
  FlatbuffersEntityConfigurator(
      EventHandler& event_handler,
      SceneType scene_type);  // No FlatBuffers in signature
};

// In .cpp file
struct FlatbuffersEntityConfigurator::Impl {
  FlatbuffersDataLoader loader;
  const EntityCollectionFbs* entity_data;
  // FlatBuffers types hidden here
};
```

#### 3.3 Update StylesConfigurator

**Change from:**
```cpp
class StylesConfigurator {
  FlatbuffersDataLoader m_loader;
};
```

**To:**
```cpp
class StylesConfigurator {
  IUIStyleProvider& m_style_provider;
  
public:
  StylesConfigurator(IUIStyleProvider& provider)
    : m_style_provider(provider) {}
};
```

### Phase 4: Update Factory Usage (Week 2)

**Goal:** Inject providers into configurators

#### 4.1 Create Provider Factory

**New File: `src/data_providers/provider_factory.h`**
```cpp
namespace steamrot {

class ProviderFactory {
public:
  // Create all providers for a scene
  static std::unique_ptr<ISceneDataProvider> CreateSceneDataProvider();
  static std::unique_ptr<IAssetDataProvider> CreateAssetDataProvider();
  static std::unique_ptr<IFragmentDataProvider> CreateFragmentDataProvider();
  static std::unique_ptr<IEngineDataProvider> CreateEngineDataProvider();
  static std::unique_ptr<ISceneManagerDataProvider> CreateSceneManagerDataProvider();
  
  // NEW providers
  static std::unique_ptr<IContextDataProvider> CreateContextDataProvider();
  static std::unique_ptr<IUIStyleProvider> CreateUIStyleProvider();
  static std::unique_ptr<ILogicDataProvider> CreateLogicDataProvider();
};

}
```

#### 4.2 Update SceneFactory

**Current:**
```cpp
class SceneFactory {
  ISceneConfigurator& m_scene_configurator;
  
public:
  SceneFactory(ISceneConfigurator& configurator);
};

// Usage
FlatbuffersDefaultSceneConfigurator configurator;
SceneFactory factory(configurator);
```

**Refactored:**
```cpp
// Usage with dependency injection
auto scene_provider = ProviderFactory::CreateSceneDataProvider();
auto asset_provider = ProviderFactory::CreateAssetDataProvider();

FlatbuffersDefaultSceneConfigurator configurator(
    *scene_provider, 
    *asset_provider
);

SceneFactory factory(configurator);
```

#### 4.3 Update SceneManager

**Add provider management:**
```cpp
class SceneManager {
private:
  // Own providers
  std::unique_ptr<ISceneDataProvider> m_scene_data_provider;
  std::unique_ptr<IAssetDataProvider> m_asset_data_provider;
  std::unique_ptr<ISceneManagerDataProvider> m_scene_manager_provider;
  
  // Own configurator
  std::unique_ptr<ISceneConfigurator> m_scene_configurator;
  
public:
  SceneManager() {
    // Create providers
    m_scene_data_provider = ProviderFactory::CreateSceneDataProvider();
    m_asset_data_provider = ProviderFactory::CreateAssetDataProvider();
    m_scene_manager_provider = ProviderFactory::CreateSceneManagerDataProvider();
    
    // Create configurator with providers
    m_scene_configurator = std::make_unique<FlatbuffersDefaultSceneConfigurator>(
        *m_scene_data_provider,
        *m_asset_data_provider
    );
  }
  
  void LoadScene(SceneType scene_type) {
    // Use configurator
    Scene scene;
    m_scene_configurator->ConfigureScene(scene, scene_type);
  }
};
```

---

## File Separation Strategy

### Current Library Structure

```
src/
├── types/                    # Layer 1 (INTERFACE library)
│   ├── core/
│   ├── events/
│   ├── flatbuffers/
│   └── user_interface/
├── data_providers/           # Layer 2
│   ├── DataLoader
│   ├── FlatbuffersDataLoader
│   ├── I*Provider interfaces
│   └── Flatbuffers*Provider implementations
├── entity/                   # Layer 2
│   ├── EntityManager
│   ├── ArchetypeManager
│   ├── IEntityConfigurator
│   └── FlatbuffersEntityConfigurator
├── scenes/                   # Layer 3
│   ├── Scene
│   ├── SceneManager
│   ├── SceneFactory
│   ├── ISceneConfigurator
│   └── FlatbuffersDefaultSceneConfigurator
└── ... other libraries
```

### Files to Create (NEW)

#### In `src/types/core/`:
- `ContextData.h` - Native struct for context config
- `LogicData.h` - Native struct for logic collection data

#### In `src/types/user_interface/`:
- `UIStyleData.h` - Native struct for UI styles

#### In `src/data_providers/`:
- `IContextDataProvider.h`
- `FlatbuffersContextDataProvider.h`
- `FlatbuffersContextDataProvider.cpp`
- `IUIStyleProvider.h`
- `FlatbuffersUIStyleProvider.h`
- `FlatbuffersUIStyleProvider.cpp`
- `ILogicDataProvider.h`
- `FlatbuffersLogicDataProvider.h`
- `FlatbuffersLogicDataProvider.cpp`

### Files to Refactor (MODIFY)

#### In `src/data_providers/`:
- `FlatbuffersDataLoader.h` - Remove Provide* methods, keep only LoadBinaryFile/VerifyBuffer
- `FlatbuffersDataLoader.cpp` - Simplify to core loading
- All existing provider implementations - Use refactored FlatbuffersDataLoader

#### In `src/scenes/`:
- `FlatbuffersDefaultSceneConfigurator.h` - Add provider dependencies
- `FlatbuffersDefaultSceneConfigurator.cpp` - Use providers instead of FlatbuffersDataLoader
- `SceneManager.h` - Own providers
- `SceneManager.cpp` - Create and inject providers

#### In `src/entity/`:
- `FlatbuffersEntityConfigurator.h` - Consider provider-based design
- `FlatbuffersEntityConfigurator.cpp` - Use provider or hide FlatBuffers
- `EntityManager.cpp` - Inject providers into configurator

#### In `src/user_interface/`:
- `StylesConfigurator.h` - Add IUIStyleProvider dependency
- `StylesConfigurator.cpp` - Use provider instead of direct loading

### Files to Remove (DELETE)

None - this is additive refactoring. Old methods will be marked deprecated then removed in Phase 5.

### CMakeLists.txt Updates

#### `src/data_providers/CMakeLists.txt`:
```cmake
add_library(data_providers
  # Core loading (keep)
  DataLoader.cpp
  FlatbuffersDataLoader.cpp
  data_file_utils.cpp
  paths.cpp
  
  # Existing providers
  FlatbuffersEngineDataProvider.cpp
  FlatbuffersSceneDataProvider.cpp
  FlatbuffersAssetDataProvider.cpp
  FlatbuffersFragmentDataProvider.cpp
  FlatbuffersSceneManagerDataProvider.cpp
  FlatbuffersSubscriberViewer.cpp
  
  # NEW providers
  FlatbuffersContextDataProvider.cpp
  FlatbuffersUIStyleProvider.cpp
  FlatbuffersLogicDataProvider.cpp
  
  # Factory
  provider_factory.cpp
)

target_link_libraries(data_providers
  PUBLIC
  types  # Only depends on types (Layer 1)
)
```

---

## Migration Checklist

### Phase 1: Create Missing Providers ✅

- [ ] Create `src/types/core/ContextData.h`
- [ ] Create `src/types/core/LogicData.h`
- [ ] Create `src/types/user_interface/UIStyleData.h`
- [ ] Create `IContextDataProvider` interface
- [ ] Implement `FlatbuffersContextDataProvider`
- [ ] Test context data loading
- [ ] Create `IUIStyleProvider` interface
- [ ] Implement `FlatbuffersUIStyleProvider`
- [ ] Test UI style loading
- [ ] Create `ILogicDataProvider` interface
- [ ] Implement `FlatbuffersLogicDataProvider`
- [ ] Test logic data loading
- [ ] Update `provider_factory` with new providers
- [ ] Update `data_providers/CMakeLists.txt`

### Phase 2: Refactor FlatbuffersDataLoader ✅

- [ ] Create template `LoadFlatBuffersData<T>()` method
- [ ] Mark all Provide* methods as `[[deprecated]]`
- [ ] Update all existing providers to use new API
- [ ] Test all providers still work
- [ ] Remove deprecated methods
- [ ] Update tests

### Phase 3: Update Configurators ✅

- [ ] Refactor `FlatbuffersDefaultSceneConfigurator` to use providers
- [ ] Update `SceneManager` to inject providers
- [ ] Refactor `FlatbuffersEntityConfigurator` (choose Option A or B)
- [ ] Update `EntityManager` to inject providers
- [ ] Refactor `StylesConfigurator` to use `IUIStyleProvider`
- [ ] Update all configurator tests
- [ ] Test integration

### Phase 4: Validation ✅

- [ ] Run full test suite
- [ ] Verify no FlatBuffers headers in configurator headers
- [ ] Verify FlatBuffersDataLoader only used internally by providers
- [ ] Measure build time improvement
- [ ] Generate dependency graph
- [ ] Update documentation

### Phase 5: Cleanup ✅

- [ ] Remove any remaining direct FlatbuffersDataLoader usage
- [ ] Update all code comments
- [ ] Update architecture documentation
- [ ] Create migration guide for future developers
- [ ] Celebrate! 🎉

---

## Expected Benefits

### Build Time Improvements

| Change | Impact | Estimated Improvement |
|--------|--------|----------------------|
| Complete provider pattern | FlatBuffers isolated to 10 .cpp files | 90% reduction in schema change rebuilds |
| Remove FlatbuffersDataLoader from configurators | Fewer header dependencies | 20-30% faster incremental builds |
| Pure interface layer | Clean separation | Easier to understand and maintain |

### Code Quality Improvements

1. **Single Responsibility Principle**
   - Each provider handles one data type
   - Each configurator handles one object type
   - FlatbuffersDataLoader only loads files

2. **Dependency Inversion**
   - Configurators depend on interfaces, not implementations
   - Easy to mock for testing
   - Can swap implementations without changing consumers

3. **Reduced Coupling**
   - No FlatBuffers types in public interfaces
   - Clear boundaries between layers
   - Changes isolated to single library

### Developer Experience

1. **Easier Testing**
   - Mock providers for configurator tests
   - Test providers without game objects
   - Isolated unit tests

2. **Better Understanding**
   - Clear data flow
   - Obvious where to add new data types
   - Self-documenting architecture

3. **Faster Development**
   - Less recompilation
   - Clearer responsibilities
   - Fewer merge conflicts

---

## Summary

This implementation plan provides a clear path to:

1. **Complete the provider pattern** - Add 3 missing providers
2. **Refactor FlatbuffersDataLoader** - Reduce to core loading only
3. **Update configurators** - Use provider interfaces
4. **Establish clean architecture** - Providers → Configurators → Application

The result is a pure data_provider layer that:
- Isolates FlatBuffers to provider .cpp files
- Provides native C++ interfaces
- Integrates cleanly with configurators
- Reduces build dependencies
- Improves code quality

**Estimated Effort:** 2 weeks (80 hours)  
**Risk Level:** Low - Additive changes, existing tests validate  
**Impact:** High - Completes architectural refactoring, significant build time improvement

---

## References

- [BUILD_DEPENDENCY_ANALYSIS.md](./BUILD_DEPENDENCY_ANALYSIS.md) - Original analysis
- [BUILD_STRATIFICATION_QUICK_REF.md](./BUILD_STRATIFICATION_QUICK_REF.md) - Quick reference
- [DATA_PROVIDER_SYSTEM.md](../DATA_PROVIDER_SYSTEM.md) - Provider pattern overview
- Existing providers in `src/data_providers/` - Reference implementations

---

## Current Setup with Refined Definitions

### Classification of Existing Components

With our refined definitions (Providers construct/return, Configurators modify), let's classify what we currently have:

#### ✅ TRUE PROVIDERS (Construct & Return Native Types)

**In `src/data_providers/`:**

1. **FlatbuffersEngineDataProvider** ✅
   - Loads FlatBuffers → Constructs `EngineResourcesConfigData`, `EngineConfig`, `EngineState`
   - Returns new objects through `IEngineDataProvider` interface
   - **Correct:** This is a true Provider

2. **FlatbuffersSceneDataProvider** ✅
   - Loads FlatBuffers → Constructs `SceneData` struct
   - Returns new `SceneData` object
   - **Correct:** This is a true Provider

3. **FlatbuffersAssetDataProvider** ⚠️
   - Currently returns `const AssetCollection*` (FlatBuffers type)
   - **Should return:** Native `AssetCollection` struct
   - **Action needed:** Create native AssetCollection, convert in provider

4. **FlatbuffersFragmentDataProvider** ✅
   - Loads FlatBuffers → Constructs `Fragment` objects
   - Returns new `Fragment` objects
   - **Correct:** This is a true Provider

5. **FlatbuffersSceneManagerDataProvider** ⚠️
   - Currently returns `const SceneManagerDataFbs*` (FlatBuffers type)
   - **Should return:** Native `SceneManagerData` struct
   - **Action needed:** Create native type, convert in provider

**In `src/configuration/`:**

6. **FlatbuffersUserPreferencesProvider** ✅
   - Constructs `UserPreferences` struct
   - Returns new object
   - **Correct:** This is a true Provider

7. **FlatbuffersSaveDataProvider** ⚠️
   - Currently returns `const SaveData*` (FlatBuffers type)
   - **Should return:** Native `SaveData` struct
   - **Action needed:** Create native type, convert in provider

#### ✅ TRUE CONFIGURATORS (Modify Existing Objects)

**In `src/scenes/`:**

1. **FlatbuffersDefaultSceneConfigurator** ✅
   - Takes `Scene&` as parameter
   - Modifies existing Scene object
   - **Correct:** This is a true Configurator
   - **Current issue:** Uses FlatbuffersDataLoader directly instead of providers
   - **Action needed:** Inject ISceneDataProvider, IAssetDataProvider

**In `src/entity/`:**

2. **FlatbuffersEntityConfigurator** ✅
   - Takes `EntityMemoryPool&` as parameter
   - Modifies existing entity pool, configures components
   - **Correct:** This is a true Configurator
   - **Current issue:** Takes `EntityCollectionFbs&` in constructor (FlatBuffers type)
   - **Action needed:** Either use IEntityDataProvider or keep FlatBuffers internal (PIMPL)

**In `src/user_interface/`:**

3. **StylesConfigurator** ✅
   - Takes `UIElement&` as parameter (assumed)
   - Modifies existing UI elements with styles
   - **Correct:** This is a true Configurator
   - **Action needed:** Inject IUIStyleProvider

#### ❌ MISNAMED or HYBRID Components

**FlatbuffersDataLoader** ❌
- Name suggests "Loader" but has "Provide*" methods
- Actually does loading AND provides data
- **Should be:** Pure loader (LoadBinaryFile, VerifyBuffer)
- **Action needed:** Move Provide* methods to respective providers

#### Missing Providers (Construct & Return)

1. **IUIStyleProvider** / **FlatbuffersUIStyleProvider** ❌
   - **Should construct:** `UIStyleData` struct
   - **Should return:** Native style configuration
   - **Used by:** StylesConfigurator

2. **ILogicDataProvider** / **FlatbuffersLogicDataProvider** ❌
   - **Should construct:** `LogicCollectionData` struct
   - **Should return:** Logic type mappings
   - **Used by:** LogicFactory

---

## Library Organization Strategy

### Where Providers and Configurators Should Live

Based on refined definitions, here's the organization strategy:

#### Option 1: Separate Libraries (RECOMMENDED)

```
src/
├── data_providers/              # All Providers
│   ├── I*Provider.h            # Interfaces
│   ├── Flatbuffers*Provider.h  # Implementations
│   ├── FlatbuffersDataLoader   # Internal loader
│   └── provider_factory        # Factory
│
├── configuration/               # Configuration-specific Configurators
│   ├── ISceneConfigurator      # Scene configuration
│   ├── FlatbuffersDefaultSceneConfigurator
│   └── SavedSceneConfigurator  # Future: load from save
│
├── entity/                      # Entity-specific Configurators
│   ├── IEntityConfigurator
│   ├── FlatbuffersEntityConfigurator
│   └── EntityManager (uses configurator)
│
├── user_interface/              # UI-specific Configurators
│   ├── StylesConfigurator
│   └── UIElementFactory
│
└── ... other libraries
```

**Rationale:**
- **Providers are centralized** - Single library, single responsibility
- **Configurators live where used** - Co-located with the objects they configure
- **Clear separation** - Data loading vs object configuration
- **Dependencies flow correctly** - Configurators depend on providers, not vice versa

#### Option 2: All in data_providers (NOT RECOMMENDED)

```
src/
└── data_providers/
    ├── Providers/
    └── Configurators/
```

**Problems:**
- Violates single responsibility
- Configurators depend on object types (Scene, Entity, UIElement)
- Creates circular dependencies (data_providers → scenes → data_providers)
- Harder to understand library purpose

#### Option 3: Separate Configurators Library (POSSIBLE)

```
src/
├── data_providers/              # All Providers
├── configurators/               # All Configurators
│   ├── ISceneConfigurator
│   ├── IEntityConfigurator
│   ├── StylesConfigurator
│   └── configurator_factory
└── ... other libraries
```

**Trade-offs:**
- ✅ Clear separation of providers and configurators
- ✅ Single place to find configurators
- ❌ Configurators now depend on many object types (Scene, Entity, UIElement)
- ❌ Changes to Scene require recompiling configurators library
- ❌ Less cohesive - configurators for different domains mixed together

### Recommended Organization: Option 1

**Providers:**
- Single library: `data_providers/`
- Construct and return native types
- No dependencies on game objects
- Used by configurators across codebase

**Configurators:**
- Live where they're used
- `configuration/` - Scene configurators
- `entity/` - Entity configurators
- `user_interface/` - UI configurators
- Each depends on relevant providers
- Co-located with objects they configure

**Benefits:**
1. **Clear Dependencies:**
   ```
   Configurators (Layer 2/3) → Providers (Layer 2) → Types (Layer 1)
   ```

2. **No Circular Dependencies:**
   - Providers don't know about Scene, Entity, UIElement
   - Configurators depend on providers (one direction)

3. **Easier Navigation:**
   - Need scene configuration? Look in `scenes/`
   - Need entity configuration? Look in `entity/`
   - Need data loading? Look in `data_providers/`

4. **Better Cohesion:**
   - Scene-related code stays together
   - Entity-related code stays together
   - Data loading code stays together

---

## Revised Implementation Plan

### Phase 0: Cleanup (Week 1, Days 1-2)

**Goal:** Remove unused code, clarify structure

#### 0.1 Remove context_data.fbs

**Files to delete:**
- `src/types/flatbuffers/configuration/context_data.fbs`
- `src/types/flatbuffers/generated/context_data_generated.h` (auto-regenerate will remove)

**Files to modify:**
- `src/data_providers/FlatbuffersDataLoader.h`
  - Remove `#include "context_data_generated.h"`
  - Remove `ProvideContextData()` method declaration

- `src/data_providers/FlatbuffersDataLoader.cpp`
  - Remove `ProvideContextData()` implementation

**Verification:**
```bash
# Should return nothing
grep -r "ProvideContextData\|context_data" src --exclude-dir=flatbuffers
```

#### 0.2 Update Documentation

**Update all references to "3 missing providers" → "2 missing providers":**
- BUILD_DEPENDENCY_ANALYSIS.md
- BUILD_STRATIFICATION_QUICK_REF.md
- DATA_PROVIDER_CONFIGURATOR_IMPLEMENTATION_PLAN.md

### Phase 1: Create Missing Providers (Week 1, Days 3-5)

**Goal:** Complete provider pattern with 2 new providers

#### 1.1 Create IUIStyleProvider ✅

... (rest of implementation plan continues as before)

---

## Summary of Changes

### Refined Definitions

**Provider:**
- Constructs new objects
- Converts data to native types
- Returns through I*Provider interface
- Lives in `data_providers/`

**Configurator:**
- Modifies existing objects
- Uses providers or FlatBuffers internally
- Returns success/failure
- Lives where the objects it configures live

### Current State Classification

**True Providers (7):**
- ✅ FlatbuffersEngineDataProvider
- ✅ FlatbuffersSceneDataProvider
- ⚠️ FlatbuffersAssetDataProvider (needs native type conversion)
- ✅ FlatbuffersFragmentDataProvider
- ⚠️ FlatbuffersSceneManagerDataProvider (needs native type conversion)
- ✅ FlatbuffersUserPreferencesProvider
- ⚠️ FlatbuffersSaveDataProvider (needs native type conversion)

**True Configurators (3):**
- ✅ FlatbuffersDefaultSceneConfigurator (needs provider injection)
- ✅ FlatbuffersEntityConfigurator (needs provider or PIMPL)
- ✅ StylesConfigurator (needs provider injection)

**Missing Providers (2):**
- ❌ IUIStyleProvider
- ❌ ILogicDataProvider

**To Remove:**
- ❌ context_data.fbs (unused)
- ❌ ProvideContextData() method

### Recommended Organization

- **Providers:** Single library (`data_providers/`)
- **Configurators:** Live where used (`scenes/`, `entity/`, `user_interface/`)
- **Clear dependencies:** Configurators → Providers → Types
- **No circular dependencies**

