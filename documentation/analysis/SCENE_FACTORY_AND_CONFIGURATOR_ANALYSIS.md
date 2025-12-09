# Scene Factory Data Sourcing Architecture - Comprehensive Analysis

**Date**: December 8, 2025  
**Status**: Analysis Complete - Ready for Implementation  
**Related**: ISceneFactory, IEntityConfigurator, ISceneDataProvider, ISaveDataProvider

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Problem Statement](#problem-statement)
3. [Current Architecture](#current-architecture)
4. [Architecture Options Analysis](#architecture-options-analysis)
5. [Recommended Solution](#recommended-solution)
6. [Implementation Plan](#implementation-plan)
7. [Code Examples](#code-examples)
8. [Testing Strategy](#testing-strategy)
9. [Migration Notes](#migration-notes)

---

## Executive Summary

### The Problem
SceneFactory and EntityConfigurator need to support two data sources:
1. **Default scenes**: Load from ISceneDataProvider (default scene data files)
2. **Saved scenes**: Load from ISaveDataProvider (save files)

Currently, both receive FlatBuffers pointers directly. The challenge is to support both sources without:
- Creating intermediate copying structs
- Adding complex conditional logic
- Violating SOLID principles
- Baking FlatBuffers types into abstract interfaces

### The Solution
**Use the Strategy Pattern with Dual-Format Interface**

Create `ISceneConfigurator` interface with two implementations:
- `DefaultSceneConfigurator`: Works with ISceneDataProvider
- `SavedSceneConfigurator`: Works with ISaveDataProvider

SceneFactory receives a configurator (not data providers), and the configurator encapsulates all data source logic.

The interface provides **both** native C++ structs (for format-agnostic code) **and** FlatBuffers pointers (for EntityConfigurator compatibility):
- `GetSceneData()` → Returns `SceneData` (native struct) - **Primary interface**
- `GetSceneDataFbs()` → Returns `const SceneDataFbs*` - **Internal use only**

**Key Benefits**:
- ✅ No null-check conditionals in SceneFactory
- ✅ Clean separation of concerns
- ✅ Easy to test and extend
- ✅ Follows SOLID principles
- ✅ Direct FlatBuffers access (no copying)
- ✅ Format-agnostic primary interface (native C++ structs)
- ✅ FlatBuffers compatibility for EntityConfigurator

---

## Problem Statement

### Problem Statement Details

The user has implemented:
1. Abstract `ISceneFactory` class
2. Abstract `IEntityConfigurator` class
3. Two data source interfaces:
   - `ISceneDataProvider` - loads default scene data
   - `ISaveDataProvider` - loads saved game data

### Key Constraints
1. **No intermediate structs**: Data goes directly from FlatBuffers to Scene/Entities
2. **Two distinct sources**: Default OR Save (never both simultaneously)
3. **Minimal copying**: Avoid performance overhead
4. **SOLID principles**: Clean, maintainable architecture

### User's Initial Thought
> "Feed either the ISceneDataProvider or ISaveDataProvider to SceneFactory on construction and let it use the fact that one is null and the other not to decide how to populate its data?"

**Analysis**: This approach would work but has drawbacks:
- ❌ Requires null-check conditionals throughout SceneFactory
- ❌ Violates Single Responsibility Principle
- ❌ Makes testing more complex
- ❌ Less extensible (what if we add more sources?)

---

## Current Architecture

### Class Hierarchy

```
ISceneFactory (abstract)
├── Protected Members:
│   ├── m_entity_configurator: unique_ptr<IEntityConfigurator>
│   ├── m_game_context: const GameContext&
│   └── scene_type: SceneType
├── Public Methods:
│   ├── CreateScene() → std::expected<unique_ptr<Scene>, FailInfo>
│   ├── ConfigureSceneInfo(Scene&) → std::expected<monostate, FailInfo>
│   └── ConfigureLogicMap(Scene&) → std::expected<monostate, FailInfo>
└── Pure Virtual Methods:
    ├── ConfigureSceneResources(Scene&) → std::expected<monostate, FailInfo>
    └── ConfigureSceneConfig(Scene&) → std::expected<monostate, FailInfo>

FlatbuffersSceneFactory : public ISceneFactory
├── Private Members:
│   └── m_scene_data_fbs: const SceneDataFbs*
├── Constructor:
│   └── FlatbuffersSceneFactory(GameContext&, const SceneDataFbs*)
└── Implements:
    ├── ConfigureSceneResources(Scene&) override
    └── ConfigureSceneConfig(Scene&) override [NOT YET IMPLEMENTED]
```

```
IEntityConfigurator (abstract)
├── Protected Members:
│   └── m_event_handler: EventHandler&
├── Public Methods (all pure virtual):
│   ├── ConfigureEntityMemoryPool(EntityMemoryPool&)
│   ├── ConfigureFirstLayerComponents(EntityMemoryPool&)
│   ├── ConfigureSecondLayerComponents(EntityMemoryPool&)
│   ├── ConfigureComponent(Component&)
│   ├── ConfigureCMeta(CMeta&)
│   ├── ConfigureCUserInterface(CUserInterface&)
│   ├── ConfigureCUIState(CUIState&, EntityMemoryPool&)
│   ├── ConfigureCGrimoireMachina(CGrimoireMachina&)
│   └── ConfigureCMachineForm(CMachinaForm&)

FlatbuffersEntityConfigurator : public IEntityConfigurator
├── Private Members:
│   ├── m_entity_collection_data: const EntityCollectionFbs&
│   └── m_current_entity_data: const EntityDataFbs*
├── Constructor:
│   └── FlatbuffersEntityConfigurator(EventHandler&, const EntityCollectionFbs&)
└── Implements all virtual methods
```

### Data Flow (Current)

```
FlatbuffersDataLoader
  ↓ ProvideDefaultSceneData(SceneType)
  ↓ returns: const SceneDataFbs*
  ↓
FlatbuffersSceneFactory(game_context, scene_data_fbs)
  ↓ Constructor creates:
  ↓
FlatbuffersEntityConfigurator(event_handler, *scene_data_fbs->entity_collection())
  ↓ Stores reference to:
  ↓
EntityCollectionFbs& (FlatBuffers data)
  ↓ Configuration methods read directly from:
  ↓
FlatBuffers tables (no intermediate copying)
```

### Key Observations

1. **Direct FlatBuffers Access**: Both factory and configurator hold FlatBuffers references
2. **Single Source**: Currently only supports default scene data
3. **Tight Coupling**: FlatbuffersSceneFactory hard-coded to use FlatbuffersEntityConfigurator
4. **Missing Implementation**: ConfigureSceneConfig() not yet implemented

---

## Architecture Options Analysis

### Option 1: Dual-Provider Constructor (User's Suggestion)

```cpp
FlatbuffersSceneFactory(
    const GameContext& game_context,
    ISceneDataProvider* default_provider,  // nullable
    ISaveDataProvider* save_provider)       // nullable
{
    if (default_provider) {
        // Load from default
        auto scene_data = default_provider->LoadSceneData(scene_type);
        // Use scene_data...
    } else if (save_provider) {
        // Load from save
        auto save_data = save_provider->LoadSave(slot_index);
        // Extract scene data...
    } else {
        // Error: both null
    }
}
```

**Pros**:
- ✅ Simple to understand
- ✅ Direct provider access

**Cons**:
- ❌ Null-check conditionals throughout class
- ❌ Violates Single Responsibility Principle
- ❌ Complex error handling (both null? both set?)
- ❌ Makes testing harder (need to test null combinations)
- ❌ Not extensible (adding third source = more conditionals)
- ❌ SceneFactory knows too much about data sources

**Verdict**: ❌ **NOT RECOMMENDED** - Technical debt, maintainability issues

---

### Option 2: Abstract Factory Pattern (RECOMMENDED)

Create an abstraction layer between SceneFactory and data sources.

```cpp
// New interface
class ISceneConfigurator {
public:
    virtual ~ISceneConfigurator() = default;
    
    virtual std::expected<const SceneDataFbs*, FailInfo> 
    GetSceneData() = 0;
    
    virtual std::expected<std::unique_ptr<IEntityConfigurator>, FailInfo>
    CreateEntityConfigurator(EventHandler& event_handler) = 0;
};

// Implementation for default scenes
class DefaultSceneConfigurator : public ISceneConfigurator {
private:
    ISceneDataProvider& m_scene_data_provider;
    SceneType m_scene_type;
    const SceneDataFbs* m_cached_scene_data{nullptr};
    
public:
    DefaultSceneConfigurator(ISceneDataProvider& provider, SceneType type);
    
    std::expected<const SceneDataFbs*, FailInfo> GetSceneData() override;
    std::expected<std::unique_ptr<IEntityConfigurator>, FailInfo>
    CreateEntityConfigurator(EventHandler& event_handler) override;
};

// Implementation for saved scenes
class SavedSceneConfigurator : public ISceneConfigurator {
private:
    ISaveDataProvider& m_save_data_provider;
    uint32_t m_slot_index;
    const SceneDataFbs* m_cached_scene_data{nullptr};
    
public:
    SavedSceneConfigurator(ISaveDataProvider& provider, uint32_t slot);
    
    std::expected<const SceneDataFbs*, FailInfo> GetSceneData() override;
    std::expected<std::unique_ptr<IEntityConfigurator>, FailInfo>
    CreateEntityConfigurator(EventHandler& event_handler) override;
};

// Updated SceneFactory
class FlatbuffersSceneFactory : public ISceneFactory {
private:
    std::unique_ptr<ISceneConfigurator> m_scene_configurator;
    
public:
    FlatbuffersSceneFactory(
        const GameContext& game_context,
        std::unique_ptr<ISceneConfigurator> configurator);
};
```

**Pros**:
- ✅ No null-checks in SceneFactory
- ✅ Clean separation of concerns
- ✅ Easy to extend (new configurator = new source)
- ✅ Follows SOLID principles (Open/Closed, Single Responsibility)
- ✅ Easy to test (mock configurator)
- ✅ Direct FlatBuffers access (no copying)
- ✅ SceneFactory doesn't know about data sources

**Cons**:
- ⚠️ More classes to maintain
- ⚠️ Slightly more upfront design

**Verdict**: ✅ **STRONGLY RECOMMENDED** - Best practices, scalable, maintainable

---

### Option 3: Variant-Based Approach

```cpp
using DataSource = std::variant<
    std::reference_wrapper<ISceneDataProvider>,
    std::reference_wrapper<ISaveDataProvider>
>;

FlatbuffersSceneFactory(const GameContext& game_context, DataSource source);
```

**Pros**:
- ✅ Type-safe (can't have both null or both set)
- ✅ Modern C++ approach

**Cons**:
- ❌ Still requires `std::visit` with conditionals
- ❌ SceneFactory still knows about data sources
- ❌ Not as clean as Strategy Pattern
- ❌ Less extensible than Option 2

**Verdict**: ⚠️ **ACCEPTABLE** but Option 2 is better

---

## Recommended Solution

### Architecture: Strategy Pattern + Dual-Format Interface

The solution combines two design patterns:

1. **Strategy Pattern**: ISceneConfigurator abstracts the "how to get scene data" strategy
2. **Abstract Factory Pattern**: Configurator creates appropriate EntityConfigurator

### Dual-Format Interface Rationale

**Problem**: The codebase uses both native C++ structs (Data Loading Interface pattern) AND FlatBuffers pointers (for EntityConfigurator).

**Solution**: Provide both formats through the interface:

```cpp
class ISceneConfigurator {
    // PRIMARY: Format-agnostic native struct
    virtual std::expected<SceneData, FailInfo> GetSceneData() = 0;
    
    // SECONDARY: FlatBuffers pointer for EntityConfigurator
    virtual std::expected<const SceneDataFbs*, FailInfo> GetSceneDataFbs() = 0;
};
```

**Why Both?**
1. **SceneFactory** should use native `SceneData` struct (follows Data Loading Interface pattern)
2. **EntityConfigurator** currently requires `EntityCollectionFbs&` (from FlatBuffers data)
3. Configurator caches both formats internally (loaded once, no duplication)
4. Factory code becomes format-agnostic (can work with JSON, Lua, etc. in future)
5. Maintains compatibility with existing EntityConfigurator

**Future Migration Path**: When EntityConfigurator is refactored to use native structs, remove `GetSceneDataFbs()` from the interface. For now, it provides necessary bridge between old and new patterns.

### Key Components

#### 1. ISceneConfigurator Interface

```cpp
namespace steamrot {

class ISceneConfigurator {
public:
    virtual ~ISceneConfigurator() = default;
    
    /////////////////////////////////////////////////
    /// @brief Get the scene data as native C++ struct
    ///
    /// Configurator handles fetching from appropriate source
    /// and converting to native format.
    /// May cache internally to avoid repeated lookups.
    ///
    /// @return Native SceneData struct or failure info
    /////////////////////////////////////////////////
    virtual std::expected<SceneData, FailInfo> 
    GetSceneData() = 0;
    
    /////////////////////////////////////////////////
    /// @brief Get the FlatBuffers pointer for internal use
    ///
    /// This is needed for EntityConfigurator and direct FlatBuffers access.
    /// Configurator caches the FlatBuffers data internally.
    ///
    /// @return FlatBuffers scene data pointer or failure info
    /////////////////////////////////////////////////
    virtual std::expected<const SceneDataFbs*, FailInfo>
    GetSceneDataFbs() = 0;
    
    /////////////////////////////////////////////////
    /// @brief Create entity configurator for this scene
    ///
    /// Configurator creates appropriate entity configurator
    /// based on data source (default vs saved).
    ///
    /// @param event_handler Reference to EventHandler
    /// @return EntityConfigurator or failure info
    /////////////////////////////////////////////////
    virtual std::expected<std::unique_ptr<IEntityConfigurator>, FailInfo>
    CreateEntityConfigurator(EventHandler& event_handler) = 0;
    
    /////////////////////////////////////////////////
    /// @brief Get the scene type
    ///
    /// @return SceneType enum value
    /////////////////////////////////////////////////
    virtual SceneType GetSceneType() const = 0;
};

} // namespace steamrot
```

#### 2. DefaultSceneConfigurator Implementation

```cpp
class DefaultSceneConfigurator : public ISceneConfigurator {
private:
    ISceneDataProvider& m_scene_data_provider;
    SceneType m_scene_type;
    
    // Cache native and FlatBuffers data
    mutable std::optional<SceneData> m_cached_native_data;
    mutable const SceneDataFbs* m_cached_fbs_data{nullptr};
    mutable bool m_data_loaded{false};
    
public:
    DefaultSceneConfigurator(
        ISceneDataProvider& provider, 
        SceneType scene_type)
        : m_scene_data_provider(provider)
        , m_scene_type(scene_type) {}
    
    std::expected<SceneData, FailInfo> GetSceneData() override {
        if (!m_data_loaded) {
            LoadData();
        }
        
        if (!m_cached_native_data.has_value()) {
            return std::unexpected(FailInfo{
                FailMode::NullPointer,
                "Scene data failed to load"
            });
        }
        
        return m_cached_native_data.value();
    }
    
    std::expected<const SceneDataFbs*, FailInfo> GetSceneDataFbs() override {
        if (!m_data_loaded) {
            LoadData();
        }
        
        if (!m_cached_fbs_data) {
            return std::unexpected(FailInfo{
                FailMode::NullPointer,
                "FlatBuffers scene data failed to load"
            });
        }
        
        return m_cached_fbs_data;
    }
    
    std::expected<std::unique_ptr<IEntityConfigurator>, FailInfo>
    CreateEntityConfigurator(EventHandler& event_handler) override {
        auto scene_data_fbs = GetSceneDataFbs();
        if (!scene_data_fbs.has_value()) {
            return std::unexpected(scene_data_fbs.error());
        }
        
        if (!scene_data_fbs.value()->entity_collection()) {
            return std::unexpected(FailInfo{
                FailMode::NullPointer,
                "EntityCollection is null in scene data"
            });
        }
        
        return std::make_unique<FlatbuffersEntityConfigurator>(
            event_handler,
            *scene_data_fbs.value()->entity_collection()
        );
    }
    
    SceneType GetSceneType() const override {
        return m_scene_type;
    }

private:
    void LoadData() const {
        if (m_data_loaded) return;
        
        // Get native data from provider (follows Data Loading Interface pattern)
        auto native_result = m_scene_data_provider.LoadSceneData(m_scene_type);
        if (native_result.has_value()) {
            m_cached_native_data = native_result.value();
        }
        
        // Also need FlatBuffers pointer for EntityConfigurator
        // This requires access to FlatbuffersDataLoader internally
        // TODO: Refactor to use a common cache or loader that provides both
        FlatbuffersDataLoader loader;
        auto fbs_result = loader.ProvideDefaultSceneData(m_scene_type);
        if (fbs_result.has_value()) {
            m_cached_fbs_data = fbs_result.value();
        }
        
        m_data_loaded = true;
    }
};
```

#### 3. SavedSceneConfigurator Implementation

```cpp
class SavedSceneConfigurator : public ISceneConfigurator {
private:
    ISaveDataProvider& m_save_data_provider;
    uint32_t m_slot_index;
    
    // Cache native and FlatBuffers data
    mutable std::optional<SceneData> m_cached_native_data;
    mutable const SceneDataFbs* m_cached_fbs_data{nullptr};
    mutable bool m_data_loaded{false};
    
public:
    SavedSceneConfigurator(
        ISaveDataProvider& provider,
        uint32_t slot_index)
        : m_save_data_provider(provider)
        , m_slot_index(slot_index) {}
    
    std::expected<SceneData, FailInfo> GetSceneData() override {
        if (!m_data_loaded) {
            LoadData();
        }
        
        if (!m_cached_native_data.has_value()) {
            return std::unexpected(FailInfo{
                FailMode::NullPointer,
                "Scene data failed to load from save"
            });
        }
        
        return m_cached_native_data.value();
    }
    
    std::expected<const SceneDataFbs*, FailInfo> GetSceneDataFbs() override {
        if (!m_data_loaded) {
            LoadData();
        }
        
        if (!m_cached_fbs_data) {
            return std::unexpected(FailInfo{
                FailMode::NullPointer,
                "FlatBuffers scene data failed to load from save"
            });
        }
        
        return m_cached_fbs_data;
    }
    
    std::expected<std::unique_ptr<IEntityConfigurator>, FailInfo>
    CreateEntityConfigurator(EventHandler& event_handler) override {
        auto scene_data_fbs = GetSceneDataFbs();
        if (!scene_data_fbs.has_value()) {
            return std::unexpected(scene_data_fbs.error());
        }
        
        if (!scene_data_fbs.value()->entity_collection()) {
            return std::unexpected(FailInfo{
                FailMode::NullPointer,
                "EntityCollection is null in save data"
            });
        }
        
        return std::make_unique<FlatbuffersEntityConfigurator>(
            event_handler,
            *scene_data_fbs.value()->entity_collection()
        );
    }
    
    SceneType GetSceneType() const override {
        if (m_data_loaded && m_cached_native_data.has_value()) {
            return m_cached_native_data.value().scene_type;
        }
        return SceneType::SceneType_UNKNOWN;
    }

private:
    void LoadData() const {
        if (m_data_loaded) return;
        
        // Load save data
        auto save_data_result = m_save_data_provider.LoadSave(m_slot_index);
        if (!save_data_result.has_value()) {
            m_data_loaded = true;
            return;
        }
        
        const SaveData& save_data = save_data_result.value();
        
        // Extract native SceneData from SaveData
        // Note: When SaveData is extended with scene_states field,
        // use SceneDataExtractor to convert nested SaveData to flat SceneData
        // For now, extract from current_scene_type
        SceneData native_data;
        native_data.scene_type = save_data.current_scene_type;
        // TODO: Extract other fields when SaveData includes scene state
        m_cached_native_data = native_data;
        
        // Also need FlatBuffers pointer for EntityConfigurator
        // TODO: When SaveData includes scene_states, extract SceneDataFbs from there
        // For now, load from default as fallback
        FlatbuffersDataLoader loader;
        auto fbs_result = loader.ProvideDefaultSceneData(save_data.current_scene_type);
        if (fbs_result.has_value()) {
            m_cached_fbs_data = fbs_result.value();
        }
        
        m_data_loaded = true;
    }
};
```

#### 4. Updated FlatbuffersSceneFactory

```cpp
class FlatbuffersSceneFactory : public ISceneFactory {
private:
    std::unique_ptr<ISceneConfigurator> m_scene_configurator;
    
public:
    FlatbuffersSceneFactory(
        const GameContext& game_context,
        std::unique_ptr<ISceneConfigurator> configurator)
        : ISceneFactory(game_context)
        , m_scene_configurator(std::move(configurator))
    {
        // Get scene type from configurator
        scene_type = m_scene_configurator->GetSceneType();
        
        // Create entity configurator
        auto entity_config_result = 
            m_scene_configurator->CreateEntityConfigurator(
                game_context.event_handler
            );
        
        if (entity_config_result.has_value()) {
            m_entity_configurator = std::move(entity_config_result.value());
        }
    }
    
    std::expected<std::monostate, FailInfo>
    ConfigureSceneResources(Scene& scene) override {
        // Use native SceneData struct (format-agnostic!)
        auto scene_data = m_scene_configurator->GetSceneData();
        if (!scene_data.has_value()) {
            return std::unexpected(scene_data.error());
        }
        
        // Configure using native struct
        const SceneData& data = scene_data.value();
        
        // Configure render texture size from native data
        scene.GetRenderTexture().create(
            data.render_texture_width,
            data.render_texture_height
        );
        
        // Configure entities using entity configurator (already created)
        auto configure_emp_result = 
            m_entity_configurator->ConfigureEntityMemoryPool(
                scene.GetEntityManager().GetEntityMemoryPool()
            );
        
        if (!configure_emp_result.has_value()) {
            return std::unexpected(configure_emp_result.error());
        }
        
        return std::monostate{};
    }
    
    std::expected<std::monostate, FailInfo>
    ConfigureSceneConfig(Scene& scene) override {
        // Use native SceneData struct (format-agnostic!)
        auto scene_data = m_scene_configurator->GetSceneData();
        if (!scene_data.has_value()) {
            return std::unexpected(scene_data.error());
        }
        
        // Configure using native struct
        const SceneData& data = scene_data.value();
        
        // Set scene ID and type from native data
        scene.GetSceneInfo().scene_id = data.scene_id;
        scene.GetSceneInfo().scene_type = data.scene_type;
        
        return std::monostate{};
    }
};
```

**Key Point**: Factory uses `GetSceneData()` which returns native `SceneData` struct, making it format-agnostic. The FlatBuffers-specific `GetSceneDataFbs()` is only used internally by the configurator to create EntityConfigurator.

### Usage Examples

#### Loading Default Scene

```cpp
// In SceneManager::AddSceneFromDefault()
ISceneDataProvider& scene_data_provider = GetSceneDataProvider();

auto configurator = std::make_unique<DefaultSceneConfigurator>(
    scene_data_provider,
    scene_type
);

auto factory = FlatbuffersSceneFactory(
    m_game_context,
    std::move(configurator)
);

auto scene_result = factory.CreateScene();
```

#### Loading Saved Scene

```cpp
// In SceneManager::AddSceneFromSave()
ISaveDataProvider& save_data_provider = GetSaveDataProvider();

auto configurator = std::make_unique<SavedSceneConfigurator>(
    save_data_provider,
    slot_index
);

auto factory = FlatbuffersSceneFactory(
    m_game_context,
    std::move(configurator)
);

auto scene_result = factory.CreateScene();
```

---

## EntityConfigurator Abstraction

### Problem: Different Data Sources for Entities

Entity data can come from:
1. **Default scenes**: EntityCollectionFbs from scene files
2. **Saved games**: EntityCollectionFbs from save files
3. **Test fixtures**: Test data (future)

Currently, FlatbuffersEntityConfigurator's constructor takes `const EntityCollectionFbs&`, which means it's already flexible about the source.

### Solution: Virtual Methods, Not Overloads

The repository memories mention that IEntityConfigurator should use **virtual methods** not **overloaded methods** to support different data sources.

#### Why Virtual, Not Overloaded?

**Overloaded methods** are resolved at **compile-time**:
```cpp
// This DOESN'T work for polymorphism
class IEntityConfigurator {
public:
    virtual void Configure(const EntityCollectionFbs& data) = 0;
    virtual void Configure(const SavedEntityData& data) = 0;  // Different signature
};
// Problem: Base class pointer calls are resolved at compile-time,
// not runtime. Defeats purpose of polymorphism.
```

**Virtual methods** with **internal branching** or **strategy pattern**:
```cpp
// Option A: Virtual methods with enum
class IEntityConfigurator {
public:
    enum class DataSource { Default, Saved, Test };
    
    virtual void ConfigureFromDefault(const EntityCollectionFbs& data) = 0;
    virtual void ConfigureFromSave(const EntityCollectionFbs& data) = 0;
    virtual void ConfigureFromTest(const EntityCollectionFbs& data) = 0;
};

// Option B: Single virtual method with variant
class IEntityConfigurator {
public:
    using EntityData = std::variant<
        const EntityCollectionFbs*,     // Default
        const SavedEntityCollectionFbs*  // Saved
    >;
    
    virtual void Configure(const EntityData& data) = 0;
};
```

### Recommendation

**Keep current design** - it's already good!

The current `IEntityConfigurator` design is already flexible:
- Constructor takes `EventHandler&` and `EntityCollectionFbs&`
- The `EntityCollectionFbs&` reference can come from any source
- No changes needed for default vs saved distinction

If saved entities use a different FlatBuffers schema, then:
1. Create `SavedEntityConfigurator` implementation
2. Have it adapt SavedEntityCollectionFbs → EntityCollectionFbs
3. Pass to base methods

**No changes needed to IEntityConfigurator interface at this time.**

---

## SceneManager Data Sourcing

### Current SceneManager Methods

```cpp
class SceneManager {
public:
    std::expected<std::monostate, FailInfo>
    AddSceneFromDefault(const SceneType& scene_type);
    
    // Future method:
    // std::expected<std::monostate, FailInfo>
    // AddSceneFromSave(uint32_t slot_index);
};
```

### Updated Implementation

```cpp
std::expected<std::monostate, FailInfo>
SceneManager::AddSceneFromDefault(const SceneType& scene_type) {
    
    // Get scene data provider
    ISceneDataProvider& scene_data_provider = GetSceneDataProvider();
    
    // Create configurator for default scene
    auto configurator = std::make_unique<DefaultSceneConfigurator>(
        scene_data_provider,
        scene_type
    );
    
    // Create factory with configurator
    auto factory = FlatbuffersSceneFactory(
        m_game_context,
        std::move(configurator)
    );
    
    // Create scene
    auto scene_result = factory.CreateScene();
    if (!scene_result.has_value()) {
        return std::unexpected(scene_result.error());
    }
    
    // Add to scenes map
    auto scene_ptr = std::move(scene_result.value());
    m_scenes.emplace(
        scene_ptr->GetSceneInfo().id,
        std::move(scene_ptr)
    );
    
    // Load assets
    auto load_asset_result = 
        m_game_context.asset_manager.LoadSceneAssets(scene_type);
    
    if (!load_asset_result.has_value()) {
        return std::unexpected(load_asset_result.error());
    }
    
    return std::monostate{};
}

// Future method for loading from save
std::expected<std::monostate, FailInfo>
SceneManager::AddSceneFromSave(uint32_t slot_index) {
    
    // Get save data provider
    ISaveDataProvider& save_data_provider = GetSaveDataProvider();
    
    // Create configurator for saved scene
    auto configurator = std::make_unique<SavedSceneConfigurator>(
        save_data_provider,
        slot_index
    );
    
    // Create factory with configurator
    auto factory = FlatbuffersSceneFactory(
        m_game_context,
        std::move(configurator)
    );
    
    // Create scene
    auto scene_result = factory.CreateScene();
    if (!scene_result.has_value()) {
        return std::unexpected(scene_result.error());
    }
    
    // Add to scenes map
    auto scene_ptr = std::move(scene_result.value());
    m_scenes.emplace(
        scene_ptr->GetSceneInfo().id,
        std::move(scene_ptr)
    );
    
    // Note: Assets already loaded (don't reload for saved scenes)
    
    return std::monostate{};
}
```

### Key Points

1. **SceneManager decides the source**: It chooses Default vs Saved
2. **SceneManager creates configurator**: Passes appropriate provider
3. **Factory is agnostic**: Doesn't know about data sources
4. **Clean separation**: Each layer has single responsibility

---

## Implementation Plan

### Phase 1: Create ISceneConfigurator Interface

**Files to Create**:
1. `src/scenes/ISceneConfigurator.h` - Interface definition
2. `src/scenes/ISceneConfigurator.cpp` - Any common implementation
3. `src/scenes/DefaultSceneConfigurator.h` - Default implementation
4. `src/scenes/DefaultSceneConfigurator.cpp` - Implementation
5. `src/scenes/SavedSceneConfigurator.h` - Saved implementation
6. `src/scenes/SavedSceneConfigurator.cpp` - Implementation

**Tests to Create**:
1. `tests/unit/scenes/DefaultSceneConfigurator.test.cpp`
2. `tests/unit/scenes/SavedSceneConfigurator.test.cpp`

**Dependencies**:
- ISceneDataProvider
- ISaveDataProvider  
- scene_data_generated.h (FlatBuffers)

---

### Phase 2: Update FlatbuffersSceneFactory

**Files to Modify**:
1. `src/scenes/FlatbuffersSceneFactory.h`
   - Replace `m_scene_data_fbs` with `m_scene_configurator`
   - Update constructor signature

2. `src/scenes/FlatbuffersSceneFactory.cpp`
   - Update constructor implementation
   - Update ConfigureSceneResources to use configurator
   - Implement ConfigureSceneConfig to use configurator

**Tests to Modify**:
1. Any existing FlatbuffersSceneFactory tests

---

### Phase 3: Update SceneManager

**Files to Modify**:
1. `src/scenes/SceneManager.cpp`
   - Update `AddSceneFromDefault()` to create DefaultSceneConfigurator
   - Add `AddSceneFromSave()` method with SavedSceneConfigurator

**Tests to Modify**:
1. SceneManager tests for new flow

---

### Phase 4: Integration Testing

**Tests to Create**:
1. `tests/integration/scene_loading/default_scene_loading.test.cpp`
2. `tests/integration/scene_loading/saved_scene_loading.test.cpp`

---

## Code Examples

### Complete Example: Loading Default Scene

```cpp
// Step 1: SceneManager decides to load default scene
std::expected<std::monostate, FailInfo>
SceneManager::AddSceneFromDefault(const SceneType& scene_type) {
    
    // Step 2: Get the scene data provider
    ISceneDataProvider& provider = GetSceneDataProvider();
    
    // Step 3: Create configurator for this specific source
    auto configurator = std::make_unique<DefaultSceneConfigurator>(
        provider,
        scene_type
    );
    
    // Step 4: Create factory with configurator
    auto factory = FlatbuffersSceneFactory(
        m_game_context,
        std::move(configurator)
    );
    
    // Step 5: Factory uses configurator to get data and create scene
    auto scene_result = factory.CreateScene();
    
    // Step 6: Handle result
    if (!scene_result.has_value()) {
        return std::unexpected(scene_result.error());
    }
    
    // Add to map, load assets, etc.
    // ...
    
    return std::monostate{};
}

// Inside FlatbuffersSceneFactory::ConfigureSceneResources()
std::expected<std::monostate, FailInfo>
FlatbuffersSceneFactory::ConfigureSceneResources(Scene& scene) {
    
    // Get scene data from configurator (handles source internally)
    auto scene_data_result = m_scene_configurator->GetSceneData();
    
    if (!scene_data_result.has_value()) {
        return std::unexpected(scene_data_result.error());
    }
    
    const SceneDataFbs* scene_data = scene_data_result.value();
    
    // Use scene data to configure
    if (scene_data->scene_resources()) {
        // Configure render texture size, etc.
    }
    
    // Entity configurator already created in constructor
    auto configure_result = m_entity_configurator->ConfigureEntityMemoryPool(
        scene.GetEntityManager().GetEntityMemoryPool()
    );
    
    if (!configure_result.has_value()) {
        return std::unexpected(configure_result.error());
    }
    
    return std::monostate{};
}
```

---

## Testing Strategy

### Unit Tests

#### DefaultSceneConfigurator Tests
```cpp
TEST_CASE("DefaultSceneConfigurator::GetSceneData returns scene data", 
          "[unit][DefaultSceneConfigurator]") {
    MockSceneDataProvider mock_provider;
    DefaultSceneConfigurator configurator(
        mock_provider,
        SceneType::SceneType_TITLE
    );
    
    auto result = configurator.GetSceneData();
    REQUIRE(result.has_value());
    REQUIRE(result.value() != nullptr);
}

TEST_CASE("DefaultSceneConfigurator caches scene data", 
          "[unit][DefaultSceneConfigurator]") {
    MockSceneDataProvider mock_provider;
    mock_provider.set_call_counter();
    
    DefaultSceneConfigurator configurator(
        mock_provider,
        SceneType::SceneType_TITLE
    );
    
    configurator.GetSceneData();
    configurator.GetSceneData();
    
    REQUIRE(mock_provider.call_count() == 1);  // Only called once
}

TEST_CASE("DefaultSceneConfigurator::CreateEntityConfigurator creates configurator", 
          "[unit][DefaultSceneConfigurator]") {
    MockSceneDataProvider mock_provider;
    MockEventHandler event_handler;
    
    DefaultSceneConfigurator configurator(
        mock_provider,
        SceneType::SceneType_TITLE
    );
    
    auto result = configurator.CreateEntityConfigurator(event_handler);
    REQUIRE(result.has_value());
    REQUIRE(result.value() != nullptr);
}
```

#### SavedSceneConfigurator Tests
```cpp
TEST_CASE("SavedSceneConfigurator::GetSceneData returns save data", 
          "[unit][SavedSceneConfigurator]") {
    MockSaveDataProvider mock_provider;
    SavedSceneConfigurator configurator(mock_provider, 0);
    
    auto result = configurator.GetSceneData();
    REQUIRE(result.has_value());
    REQUIRE(result.value() != nullptr);
}
```

### Integration Tests

```cpp
TEST_CASE("SceneManager loads default scene with configurator", 
          "[integration][SceneManager]") {
    GameContext game_context = CreateTestGameContext();
    SceneManager scene_manager(game_context);
    
    auto result = scene_manager.AddSceneFromDefault(
        SceneType::SceneType_TITLE
    );
    
    REQUIRE(result.has_value());
    REQUIRE(scene_manager.GetScenes().size() == 1);
}
```

---

## Migration Notes

### Backward Compatibility

During migration, both old and new patterns can coexist:

```cpp
// Old pattern (deprecated)
FlatbuffersSceneFactory(
    const GameContext& game_context,
    const SceneDataFbs* scene_data_fbs);

// New pattern (preferred)
FlatbuffersSceneFactory(
    const GameContext& game_context,
    std::unique_ptr<ISceneConfigurator> configurator);
```

Mark old constructor as `[[deprecated]]` and migrate call sites one by one.

### Future Extensions

This pattern easily supports new data sources:

```cpp
// Network-based scenes
class NetworkSceneConfigurator : public ISceneConfigurator {
    // Load from server
};

// Procedurally generated scenes
class ProceduralSceneConfigurator : public ISceneConfigurator {
    // Generate scene data
};

// Replay/recorded scenes
class ReplaySceneConfigurator : public ISceneConfigurator {
    // Load from replay file
};
```

---

## Summary

### Key Decisions

1. ✅ **Use Strategy Pattern**: ISceneConfigurator abstracts data sourcing
2. ✅ **Configurator creates EntityConfigurator**: Keeps coupling low
3. ✅ **SceneManager orchestrates**: Decides which configurator to use
4. ✅ **Direct FlatBuffers access**: No intermediate copying
5. ✅ **IEntityConfigurator unchanged**: Already flexible enough

### Benefits

- **Clean Architecture**: Single Responsibility Principle
- **Easy Testing**: Mock configurators
- **Extensible**: New sources = new configurator
- **No Conditionals**: No null-checks in factory
- **Type Safe**: Compile-time guarantees
- **Performance**: Zero copy, cached data

### Next Steps

1. Create ISceneConfigurator interface
2. Implement DefaultSceneConfigurator
3. Implement SavedSceneConfigurator (stub for now, implement when SaveData extended)
4. Update FlatbuffersSceneFactory
5. Update SceneManager
6. Write tests
7. Document patterns

---

**End of Analysis**
