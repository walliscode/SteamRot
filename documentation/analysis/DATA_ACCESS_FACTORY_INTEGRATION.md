# DataAccessFactory Integration into GameContext

## Overview

This document describes the architectural improvement made to integrate DataAccessFactory into the game's context system, replacing global provider_factory functions with a cleaner, dependency-injected approach.

**Date:** 2025-12-23  
**Issue:** Need to integrate DataAccessFactory without creating circular dependencies  
**Solution:** Add DataAccessFactory to Engine and pass through GameContext  

---

## Problem Statement

### Original Design

The codebase had a `provider_factory.h/cpp` with global functions that returned static provider instances:

```cpp
// provider_factory.h
IEngineDataProvider &GetEngineDataProvider();
ISceneManagerDataProvider &GetSceneManagerDataProvider();
IAssetDataProvider &GetAssetDataProvider();
ISceneDataProvider &GetSceneDataProvider();
```

These were called throughout the codebase:
- `Engine::StartUp()` called `GetEngineDataProvider()`
- `AssetManager::LoadDefaultAssets()` called `GetAssetDataProvider()`
- `SceneManager::StartUp()` called `GetSceneManagerDataProvider()`
- `SceneFactory::CreateSceneFromDefault()` called `GetSceneDataProvider()`

### Problems with Global Functions

1. **Global State:** Static instances hidden in implementation files
2. **Difficult Testing:** Cannot easily swap implementations for testing
3. **Tight Coupling:** Every component directly depends on provider_factory
4. **Unclear Ownership:** Who owns and manages the provider lifecycle?
5. **Not Extensible:** Cannot easily configure or customize data access per-engine-instance

### DataAccessFactory Already Implemented

The `DataAccessFactory` class was already implemented to manage providers in a centralized way, but it wasn't integrated into the system. The challenge was: **How do we pass DataAccessFactory around without creating circular dependencies?**

**Specifically:** If we add DataAccessFactory to GameContext, and AssetManager is also in GameContext (via EngineResources), how do we avoid AssetManager needing the GameContext it's part of?

---

## Solution: Dependency Injection via GameContext

### Design Principles

Following the pattern established by the `IFontProvider` decoupling (see `FONT_PROVIDER_DECOUPLING.md`), we use **dependency inversion** and **constructor injection**:

1. **DataAccessFactory lives in Engine** - Created early, before it's needed
2. **GameContext passes it as a reference** - Like AssetManager, EventHandler, etc.
3. **Components receive it via GameContext** - Clean dependency injection
4. **No circular dependencies** - DataAccessFactory doesn't depend on GameContext

### Architecture

```
┌─────────────────────────────────────────────┐
│ Engine                                      │
│                                             │
│ - m_data_access_factory (owned)            │
│ - m_engine_resources (owned)               │
│   ├─ game_window                           │
│   ├─ event_handler                         │
│   ├─ asset_manager                         │
│   └─ ...                                   │
│ - m_game_context (reference wrapper)       │
│   ├─ &game_window                          │
│   ├─ &event_handler                        │
│   ├─ &asset_manager                        │
│   └─ &data_access_factory ← NEW           │
└─────────────────────────────────────────────┘
          │
          │ passes GameContext to
          ▼
┌─────────────────────────────────────────────┐
│ SceneManager / SceneFactory                 │
│                                             │
│ - m_game_context                           │
│   └─ Uses data_access_factory.GetXxxProvider()│
└─────────────────────────────────────────────┘
```

### Key Changes

#### 1. Engine Owns DataAccessFactory

**Engine.h:**
```cpp
class Engine {
protected:
  // DataAccessFactory first - initialized before other members
  DataAccessFactory m_data_access_factory;
  
  EngineResources m_engine_resources;
  GameContext m_game_context;
  // ...
};
```

**Engine.cpp:**
```cpp
Engine::Engine()
    : m_data_access_factory(),              // First
      m_engine_resources(),                 // Second
      m_game_context(m_engine_resources, 
                     m_data_access_factory), // Pass both
      m_scene_manager(m_game_context) {}
```

#### 2. GameContext Holds Reference

**GameContext.h:**
```cpp
struct GameContext {
  GameContext(EngineResources &engine_resources,
              DataAccessFactory &data_access_factory);
              
  EngineResources &engine_resources;
  sf::RenderWindow &game_window;
  EventHandler &event_handler;
  AssetManager &asset_manager;
  DataAccessFactory &data_access_factory; // NEW
  // ...
};
```

#### 3. Components Use DataAccessFactory from Context

**Engine::StartUp()** - Before:
```cpp
IEngineDataProvider &data_provider = GetEngineDataProvider(); // Global
```

**Engine::StartUp()** - After:
```cpp
auto provider_result = m_data_access_factory.GetEngineDataProvider();
if (!provider_result.has_value()) {
  return std::unexpected(provider_result.error());
}
IEngineDataProvider &data_provider = *provider_result.value();
```

**SceneFactory::CreateSceneFromDefault()** - Before:
```cpp
ISceneDataProvider &provider = GetSceneDataProvider(); // Global
```

**SceneFactory::CreateSceneFromDefault()** - After:
```cpp
auto provider_result = m_game_context.data_access_factory.GetSceneDataProvider();
if (!provider_result.has_value()) {
  return std::unexpected(provider_result.error());
}
ISceneDataProvider &provider = *provider_result.value();
```

**AssetManager::LoadDefaultAssets()** - Before:
```cpp
std::expected<std::monostate, FailInfo> AssetManager::LoadDefaultAssets() {
  IAssetDataProvider &asset_provider = GetAssetDataProvider(); // Global
  // ...
}
```

**AssetManager::LoadDefaultAssets()** - After:
```cpp
std::expected<std::monostate, FailInfo>
AssetManager::LoadDefaultAssets(DataAccessFactory &data_access_factory) {
  auto asset_provider_result = data_access_factory.GetAssetDataProvider();
  if (!asset_provider_result.has_value()) {
    return std::unexpected(asset_provider_result.error());
  }
  IAssetDataProvider &asset_provider = *asset_provider_result.value();
  // ...
}
```

**SceneManager::AddSceneFromDefault()** - Before:
```cpp
auto load_asset_result = m_game_context.asset_manager.LoadSceneAssets(scene_type);
```

**SceneManager::AddSceneFromDefault()** - After:
```cpp
auto load_asset_result = m_game_context.asset_manager.LoadSceneAssets(
    m_game_context.data_access_factory, scene_type);
```

---

## Benefits

### 1. No Circular Dependencies

**Clear Dependency Flow:**
```
Engine
  ├─> DataAccessFactory (owns)
  ├─> EngineResources (owns)
  │    └─> AssetManager
  └─> GameContext (references both)
       ├─> DataAccessFactory& (reference)
       └─> AssetManager& (reference)
```

- Engine owns both DataAccessFactory and EngineResources
- GameContext just holds references
- No component needs to contain another component
- All dependencies flow downward

### 2. Clear Ownership

- **Engine** owns the DataAccessFactory
- **Lifetime** matches Engine lifetime
- **Destruction** automatic when Engine is destroyed
- **Single Instance** per Engine instance

### 3. Better Testability

**Test Fixture Pattern:**
```cpp
class TestFixture {
private:
  DataAccessFactory m_data_access_factory;  // Owned
  EngineResources m_engine_resources;       // Owned
  std::unique_ptr<GameContext> m_game_context; // Owns reference wrapper

public:
  void CreateContexts() {
    m_game_context = std::make_unique<GameContext>(
        m_engine_resources, 
        m_data_access_factory);
  }
};
```

Tests can now:
- Create their own DataAccessFactory instances
- Configure them as needed for testing
- Pass them through GameContext to components under test
- No global state to worry about

### 4. Consistent Pattern

This follows the same pattern as other GameContext members:

| Member | Type | Owned By | Passed Via |
|--------|------|----------|------------|
| `game_window` | `sf::RenderWindow&` | EngineResources | GameContext |
| `event_handler` | `EventHandler&` | EngineResources | GameContext |
| `asset_manager` | `AssetManager&` | EngineResources | GameContext |
| `data_access_factory` | `DataAccessFactory&` | Engine | GameContext |

All are references passed through GameContext for dependency injection.

### 5. Extensibility

Future improvements become easier:
- **Multiple Data Sources:** Different engines could use different DataAccessFactory configurations
- **Runtime Switching:** Could potentially switch data providers mid-game
- **Mocking:** Tests can inject mock DataAccessFactory instances
- **Configuration:** Each engine instance can be configured independently

---

## Files Changed

### Modified Files (13)

**Production Code (8):**

1. **`src/engine/Engine.h`**
   - Added `#include "DataAccessFactory.h"`
   - Added `DataAccessFactory m_data_access_factory;` member (first in list)
   - DataAccessFactory is first to ensure it's constructed before components that depend on it

2. **`src/engine/Engine.cpp`**
   - Updated constructor to initialize `m_data_access_factory` first
   - Updated `m_game_context` initialization to pass `m_data_access_factory`
   - Updated `StartUp()` to use `m_data_access_factory.GetEngineDataProvider()`

3. **`src/context/GameContext.h`**
   - Added `#include "DataAccessFactory.h"`
   - Updated constructor signature to accept `DataAccessFactory &data_access_factory`
   - Added `DataAccessFactory &data_access_factory;` member

4. **`src/context/GameContext.cpp`**
   - Updated constructor implementation to initialize `data_access_factory` reference

5. **`src/assets/AssetManager.h`**
   - Added forward declaration of `DataAccessFactory`
   - Updated `LoadDefaultAssets()` to accept `DataAccessFactory &` parameter
   - Updated `LoadSceneAssets()` to accept `DataAccessFactory &` parameter

6. **`src/assets/AssetManager.cpp`**
   - Added `#include "DataAccessFactory.h"`
   - Removed `#include "provider_factory.h"`
   - Updated both methods to use passed `DataAccessFactory` instead of global function

7. **`src/scenes/SceneManager.cpp`**
   - Removed `#include "provider_factory.h"`
   - Updated `StartUp()` to use `m_game_context.data_access_factory.GetSceneManagerDataProvider()`
   - Updated `AddSceneFromDefault()` to pass `m_game_context.data_access_factory` to AssetManager

8. **`src/scenes/SceneFactory.cpp`**
   - Removed `#include "provider_factory.h"`
   - Updated `CreateSceneFromDefault()` to use `m_game_context.data_access_factory.GetSceneDataProvider()`

**Test Code (5):**

9. **`tests/context/TestFixture.h`**
   - Added `#include "DataAccessFactory.h"`
   - Added `DataAccessFactory m_data_access_factory;` member
   - Positioned before `m_engine_resources` for proper initialization order

10. **`tests/context/TestFixture.cpp`**
    - Updated `CreateContexts()` to pass `m_data_access_factory` to GameContext constructor

11. **`tests/unit/assets/AssetManager.test.cpp`**
    - Added `#include "DataAccessFactory.h"`
    - Updated all test cases to create `DataAccessFactory` instance
    - Passed to `LoadDefaultAssets()` and `LoadSceneAssets()` calls

12. **`tests/unit/logic/logic_render.test.cpp`**
    - Added `#include "DataAccessFactory.h"`
    - Updated test case to create `DataAccessFactory` instance
    - Passed to `LoadDefaultAssets()` call

13. **`tests/unit/logic/logic_render_free_functions.test.cpp`**
    - Added `#include "DataAccessFactory.h"`
    - Updated all test cases (8 locations) to create `DataAccessFactory` instances
    - Passed to all `LoadDefaultAssets()` calls

### No Changes Required

- **`src/data_providers/provider_factory.h/cpp`** - Kept for now, may deprecate later
- **Other test files** - Only files directly calling AssetManager methods needed updates
- **Interface files** - No changes to `IAssetDataProvider`, `IEngineDataProvider`, etc.

---

## Comparison with IFontProvider Pattern

This change follows the same architectural principles as the `IFontProvider` decoupling:

### IFontProvider Pattern (Previous)

**Problem:** IUIStyleConfigurator had circular dependency with AssetManager

**Solution:** 
- Created `IFontProvider` interface
- AssetManager implements `IFontProvider`
- Configurator depends on `IFontProvider` (abstraction, not concrete class)

### DataAccessFactory Pattern (Current)

**Problem:** DataAccessFactory needs to be accessible without circular dependencies

**Solution:**
- DataAccessFactory owned by Engine
- Passed through GameContext as reference
- Components access via GameContext (dependency injection)

### Common Principles

Both solutions apply:
1. **Dependency Inversion Principle** - Depend on abstractions, not concretions
2. **Single Responsibility** - Clear separation of concerns
3. **Dependency Injection** - Pass dependencies explicitly
4. **No Circular Dependencies** - Clear one-way dependency flow

---

## Migration Guide

### For New Components

When creating new components that need data providers:

```cpp
class NewComponent {
private:
  const GameContext &m_game_context;
  
public:
  NewComponent(const GameContext &game_context) 
      : m_game_context(game_context) {}
  
  void DoSomething() {
    // Access provider through GameContext
    auto provider_result = 
        m_game_context.data_access_factory.GetEngineDataProvider();
    
    if (!provider_result.has_value()) {
      // Handle error
      return;
    }
    
    IEngineDataProvider &provider = *provider_result.value();
    // Use provider...
  }
};
```

### For Tests

When writing tests that need data access:

```cpp
TEST_CASE("My test", "[unit]") {
  // Option 1: Use TestFixture (recommended)
  steamrot::tests::TestFixture fixture;
  auto &game_context = fixture.GetGameContext();
  // game_context.data_access_factory is available
  
  // Option 2: Create manually
  steamrot::DataAccessFactory data_access_factory;
  steamrot::EngineResources engine_resources;
  steamrot::GameContext game_context(engine_resources, data_access_factory);
}
```

---

## Future Work

### Potential Improvements

1. **Deprecate provider_factory.h/cpp**
   - Remove global functions entirely
   - Update any remaining code that uses them
   - Clean up unused files

2. **Support Multiple DataTypes**
   - Currently hardcoded to Flatbuffers
   - Could support JSON, XML, custom formats
   - Configure via constructor parameter

3. **Runtime Provider Switching**
   - Allow changing providers at runtime
   - Useful for A/B testing, modding support
   - Requires careful state management

4. **Provider Configuration**
   - Add configuration options to DataAccessFactory
   - Cache settings, path overrides, etc.
   - Make providers more flexible

---

## Testing Strategy

### Unit Tests Updated

- ✅ `AssetManager.test.cpp` - Tests now create and pass DataAccessFactory
- ✅ `logic_render.test.cpp` - Rendering tests create DataAccessFactory
- ✅ `logic_render_free_functions.test.cpp` - All 8 test cases updated

### Test Infrastructure Updated

- ✅ `TestFixture` - Now owns DataAccessFactory and passes to GameContext
- ✅ All tests using TestFixture automatically have access to DataAccessFactory

### Integration Testing

Since building and testing is done locally by the user, the following should be verified:

1. **Compile Successfully** - All files should compile without errors
2. **Unit Tests Pass** - All updated test cases should pass
3. **Integration Tests Pass** - Full game should start and run
4. **No Regressions** - Existing functionality should work unchanged

---

## Design Principles Applied

### 1. Dependency Inversion Principle (DIP)

**Definition:** High-level modules should not depend on low-level modules. Both should depend on abstractions.

**Application:**
- Components depend on `DataAccessFactory` reference (passed via GameContext)
- DataAccessFactory provides abstract interfaces (`IEngineDataProvider`, etc.)
- Concrete implementations hidden behind interfaces

### 2. Single Responsibility Principle (SRP)

**Definition:** A class should have only one reason to change.

**Application:**
- **Engine:** Manages engine lifecycle and owns core objects
- **DataAccessFactory:** Manages data provider instances
- **GameContext:** Provides access to engine-level objects
- Each has a clear, single responsibility

### 3. Open/Closed Principle (OCP)

**Definition:** Software entities should be open for extension, closed for modification.

**Application:**
- New data provider types can be added without modifying existing code
- DataAccessFactory can support new provider interfaces
- Components work with any implementation of provider interfaces

### 4. Explicit Dependencies Principle

**Definition:** Methods should explicitly declare their dependencies.

**Application:**
- `AssetManager::LoadDefaultAssets(DataAccessFactory &)` explicitly declares dependency
- No hidden global state
- Dependencies visible in method signatures

---

## Lessons Learned

### What Worked Well

1. **Following Existing Patterns** - Using the IFontProvider pattern as a guide made design decisions clear
2. **Member Ordering** - Placing DataAccessFactory first in Engine ensured proper initialization
3. **Reference Semantics** - Using references in GameContext avoids ownership complexity
4. **Consistent Changes** - All components follow the same pattern for accessing providers

### What to Watch For

1. **Initialization Order** - DataAccessFactory must be first member in Engine
2. **Null Checking** - Always check `std::expected` results from GetXxxProvider()
3. **Test Updates** - Any test using AssetManager or providers needs DataAccessFactory
4. **Documentation** - Keep this doc updated as patterns evolve

---

## Summary

**Problem:** DataAccessFactory existed but wasn't integrated; global provider_factory functions created coupling

**Solution:** Add DataAccessFactory to Engine, pass through GameContext, inject into components

**Benefits:**
- ✅ No circular dependencies
- ✅ Clear ownership (Engine owns DataAccessFactory)
- ✅ Better testability (explicit dependencies)
- ✅ Consistent with existing patterns
- ✅ Extensible for future improvements

**Impact:** 
- 13 files modified (8 production, 5 test)
- Zero breaking changes to external interfaces
- Improved architecture quality

---

## Document Metadata

**Created:** 2025-12-23  
**Author:** GitHub Copilot Agent  
**Related Documents:**
- `FONT_PROVIDER_DECOUPLING.md` - Similar pattern for IFontProvider
- `USER_INTERFACE_DECOUPLING_ANALYSIS.md` - General decoupling principles

**Status:** Complete  
**Review:** Pending user verification that build and tests pass
