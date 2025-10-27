# Resource Management System - Design Plan

**Status**: 📋 PLANNING PHASE  
**Date**: 2025-10-27  
**Related Plans**: 
- CONTEXT_HANDLING_IMPROVEMENT_PLAN.md (Stages 1-2 completed, Stages 3-4 planned)
- TESTING_IMPROVEMENT_PLAN.md (Stage 3.1 completed)

## Executive Summary

This document outlines a comprehensive plan for improving resource management in SteamRot. The goal is to create a data-driven, flexible resource initialization system that:

- **Unifies resource management** across production and test environments
- **Eliminates duplicate test infrastructure** by making production resources reusable
- **Provides flexible initialization** levels based on what's actually needed
- **Leverages existing patterns** (FlatBuffers, builders, context system)
- **Reduces test overhead** by allowing minimal resource setup

## Problem Statement

### Current Issues

#### 1. Resource Duplication Between Production and Tests

**Production Resources** (in GameEngine):
- `sf::RenderWindow m_window`
- `EventHandler m_event_handler`
- `AssetManager m_asset_manager`
- `GameContext m_game_context`
- `SceneManager m_scene_manager`
- `DisplayManager m_display_manager`

**Test Resources** (in TestContext):
- `sf::RenderWindow render_window` (duplicate)
- `EventHandler event_handler` (duplicate)
- `AssetManager asset_manager` (duplicate)
- `EntityMemoryPool scene_entities` (test-specific)
- `ArchetypeManager archetype_manager` (test-specific)
- `sf::RenderTexture render_texture` (test-specific)

**Problems:**
- Different initialization logic in production vs tests
- Hard to keep test resources consistent with production
- Tests can't easily reuse production resource configurations
- No shared resource initialization patterns

#### 2. Rigid Initialization in Tests

Current TestContext always creates:
- Full RenderWindow (expensive, unnecessary for unit tests)
- Complete AssetManager with all assets loaded
- Full EventHandler system
- Scene-specific contexts for TITLE, CRAFTING, TEST scenes

**Problems:**
- Every test pays full initialization cost
- Unit tests that only need EntityMemoryPool still get full context
- Can't easily create minimal test scenarios
- Slow test execution due to over-initialization

#### 3. Hardcoded Resource Configuration

Resources are configured in C++ constructors:
```cpp
GameEngine::GameEngine(const EnvironmentType env_type)
    : m_window(sf::VideoMode(800, 600), "SteamRot"),
      m_event_handler(),
      m_asset_manager(),
      // ...
```

**Problems:**
- Window size hardcoded
- Can't easily test different configurations
- Scene-specific resources hardcoded
- No data-driven resource specification

#### 4. No Clear Resource Lifecycle Management

Resources are created and destroyed implicitly:
- GameEngine owns everything
- TestContext owns its own copies
- No explicit resource lifecycle hooks
- Difficult to control when resources are initialized/cleaned up

**Problems:**
- Can't defer resource creation until needed
- Can't share expensive resources between tests
- Memory footprint of tests is unnecessarily high
- No lazy initialization support

## Proposed Solution: Resource Management System

### Key Design Principles

1. **Single Source of Truth**: Production resource configurations are reused in tests
2. **Composable Initialization**: Build only what you need, when you need it
3. **Data-Driven Configuration**: Resource specifications in FlatBuffers/JSON
4. **Clear Lifecycle Management**: Explicit control over resource initialization/cleanup
5. **Backward Compatible**: Existing code continues to work during migration

### Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                   Resource Configuration                     │
│                   (FlatBuffers Schema)                       │
│  - ResourceManifest (what resources are available)          │
│  - ResourceConfig (how to initialize each resource)         │
│  - ResourcePreset (common combinations for scenes/tests)    │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                    ResourceBuilder                           │
│  - Reads configuration data                                 │
│  - Creates ResourceSpecification                            │
│  - Returns builder for constructing resources               │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                  ResourceManager                             │
│  - Central registry for all resources                       │
│  - Lazy initialization support                              │
│  - Lifetime management                                      │
│  - Shared resource pool                                     │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│              Production / Test Usage                         │
│  GameEngine: Full resource set via manifest                │
│  TestContext: Minimal subset via preset                    │
│  Custom Tests: Specific resources as needed                │
└─────────────────────────────────────────────────────────────┘
```

## Detailed Design

### Phase 1: Resource Configuration Schema

#### 1.1 FlatBuffers Schema

**File**: `src/flatbuffers_headers/resource_management.fbs`

```fbs
namespace steamrot;

// Enumeration of all resource types
enum ResourceType : byte {
  RenderWindow = 0,
  EventHandler = 1,
  AssetManager = 2,
  EntityMemoryPool = 3,
  ArchetypeManager = 4,
  RenderTexture = 5,
  SceneManager = 6,
  DisplayManager = 7
}

// Configuration for RenderWindow resource
table RenderWindowConfig {
  width: uint32 = 800;
  height: uint32 = 600;
  title: string;
  framerate_limit: uint32 = 60;
  vsync_enabled: bool = false;
}

// Configuration for EntityMemoryPool resource
table EntityMemoryPoolConfig {
  pool_size: uint32 = 100;
  // Future: pre-allocation strategies, growth policies
}

// Configuration for AssetManager resource
table AssetManagerConfig {
  load_default_assets: bool = true;
  scene_assets_to_load: [SceneType];
  // Future: specific asset lists, lazy loading config
}

// Configuration for RenderTexture resource
table RenderTextureConfig {
  width: uint32 = 800;
  height: uint32 = 600;
}

// Generic resource configuration
table ResourceConfig {
  resource_type: ResourceType;
  
  // Type-specific configurations (only one should be set)
  render_window_config: RenderWindowConfig;
  entity_pool_config: EntityMemoryPoolConfig;
  asset_manager_config: AssetManagerConfig;
  render_texture_config: RenderTextureConfig;
}

// A named preset combining multiple resources
table ResourcePreset {
  name: string (required);
  description: string;
  resources: [ResourceConfig];
}

// Root resource manifest
table ResourceManifest {
  // Individual resource configurations
  resource_configs: [ResourceConfig];
  
  // Named presets for common scenarios
  presets: [ResourcePreset];
}

root_type ResourceManifest;
```

#### 1.2 Example Configuration Files

**Production**: `data/resources/resource_manifest.json`

```json
{
  "resource_configs": [
    {
      "resource_type": "RenderWindow",
      "render_window_config": {
        "width": 800,
        "height": 600,
        "title": "SteamRot",
        "framerate_limit": 60,
        "vsync_enabled": false
      }
    },
    {
      "resource_type": "AssetManager",
      "asset_manager_config": {
        "load_default_assets": true,
        "scene_assets_to_load": ["TITLE", "CRAFTING"]
      }
    }
  ],
  "presets": [
    {
      "name": "full_game",
      "description": "Complete game resources for production",
      "resources": [
        {
          "resource_type": "RenderWindow",
          "render_window_config": {
            "width": 800,
            "height": 600,
            "title": "SteamRot",
            "framerate_limit": 60
          }
        },
        {
          "resource_type": "EventHandler"
        },
        {
          "resource_type": "AssetManager",
          "asset_manager_config": {
            "load_default_assets": true
          }
        },
        {
          "resource_type": "SceneManager"
        },
        {
          "resource_type": "DisplayManager"
        }
      ]
    },
    {
      "name": "title_scene",
      "description": "Resources for title scene",
      "resources": [
        {
          "resource_type": "EntityMemoryPool",
          "entity_pool_config": {
            "pool_size": 50
          }
        },
        {
          "resource_type": "RenderTexture",
          "render_texture_config": {
            "width": 800,
            "height": 600
          }
        }
      ]
    }
  ]
}
```

**Test**: `tests/data/resources/test_resource_presets.json`

```json
{
  "presets": [
    {
      "name": "minimal_test",
      "description": "Minimal resources for unit tests",
      "resources": [
        {
          "resource_type": "EntityMemoryPool",
          "entity_pool_config": {
            "pool_size": 10
          }
        }
      ]
    },
    {
      "name": "ui_test",
      "description": "Resources for UI testing",
      "resources": [
        {
          "resource_type": "EntityMemoryPool",
          "entity_pool_config": {
            "pool_size": 50
          }
        },
        {
          "resource_type": "RenderTexture",
          "render_texture_config": {
            "width": 800,
            "height": 600
          }
        },
        {
          "resource_type": "AssetManager",
          "asset_manager_config": {
            "load_default_assets": true
          }
        }
      ]
    },
    {
      "name": "logic_test",
      "description": "Resources for Logic class testing",
      "resources": [
        {
          "resource_type": "EntityMemoryPool",
          "entity_pool_config": {
            "pool_size": 100
          }
        },
        {
          "resource_type": "ArchetypeManager"
        },
        {
          "resource_type": "RenderTexture",
          "render_texture_config": {
            "width": 800,
            "height": 600
          }
        },
        {
          "resource_type": "EventHandler"
        },
        {
          "resource_type": "AssetManager",
          "asset_manager_config": {
            "load_default_assets": false
          }
        }
      ]
    },
    {
      "name": "full_scene_test",
      "description": "Full scene resources for integration tests",
      "resources": [
        {
          "resource_type": "RenderWindow",
          "render_window_config": {
            "width": 640,
            "height": 480,
            "title": "Test Window",
            "framerate_limit": 60
          }
        },
        {
          "resource_type": "EventHandler"
        },
        {
          "resource_type": "AssetManager",
          "asset_manager_config": {
            "load_default_assets": true,
            "scene_assets_to_load": ["TEST"]
          }
        },
        {
          "resource_type": "EntityMemoryPool",
          "entity_pool_config": {
            "pool_size": 100
          }
        },
        {
          "resource_type": "ArchetypeManager"
        },
        {
          "resource_type": "RenderTexture",
          "render_texture_config": {
            "width": 640,
            "height": 480
          }
        }
      ]
    }
  ]
}
```

### Phase 2: Resource Management Core Classes

#### 2.1 ResourceHandle - Type-Safe Resource Access

**File**: `src/resources/ResourceHandle.h`

```cpp
namespace steamrot {

// Type-safe handle to a resource
template <typename T>
class ResourceHandle {
private:
  std::shared_ptr<T> m_resource{nullptr};
  
public:
  ResourceHandle() = default;
  explicit ResourceHandle(std::shared_ptr<T> resource) 
    : m_resource(std::move(resource)) {}
  
  // Check if resource is initialized
  bool IsValid() const { return m_resource != nullptr; }
  
  // Get reference to resource (throws if not valid)
  T& Get() {
    if (!m_resource) {
      throw std::runtime_error("Resource not initialized");
    }
    return *m_resource;
  }
  
  const T& Get() const {
    if (!m_resource) {
      throw std::runtime_error("Resource not initialized");
    }
    return *m_resource;
  }
  
  // Get shared_ptr (for passing to other systems)
  std::shared_ptr<T> GetShared() const { return m_resource; }
  
  // Reset resource
  void Reset() { m_resource.reset(); }
};

} // namespace steamrot
```

#### 2.2 ResourceCollection - Container for All Resources

**File**: `src/resources/ResourceCollection.h`

```cpp
namespace steamrot {

// Collection of all resources for a context
struct ResourceCollection {
  // Core game resources
  ResourceHandle<sf::RenderWindow> render_window;
  ResourceHandle<EventHandler> event_handler;
  ResourceHandle<AssetManager> asset_manager;
  ResourceHandle<DisplayManager> display_manager;
  ResourceHandle<SceneManager> scene_manager;
  
  // Scene-specific resources
  ResourceHandle<EntityMemoryPool> entity_pool;
  ResourceHandle<ArchetypeManager> archetype_manager;
  ResourceHandle<sf::RenderTexture> render_texture;
  
  // Additional runtime data
  ResourceHandle<size_t> loop_number;
  EnvironmentType env_type{EnvironmentType::None};
  
  // Check if specific resource is available
  template <typename T>
  bool Has() const {
    if constexpr (std::is_same_v<T, sf::RenderWindow>) {
      return render_window.IsValid();
    } else if constexpr (std::is_same_v<T, EventHandler>) {
      return event_handler.IsValid();
    } else if constexpr (std::is_same_v<T, AssetManager>) {
      return asset_manager.IsValid();
    } else if constexpr (std::is_same_v<T, EntityMemoryPool>) {
      return entity_pool.IsValid();
    } else if constexpr (std::is_same_v<T, ArchetypeManager>) {
      return archetype_manager.IsValid();
    } else if constexpr (std::is_same_v<T, sf::RenderTexture>) {
      return render_texture.IsValid();
    } else {
      static_assert(always_false<T>, "Unsupported resource type");
    }
  }
  
  // Get specific resource (throws if not available)
  template <typename T>
  T& Get() {
    if constexpr (std::is_same_v<T, sf::RenderWindow>) {
      return render_window.Get();
    } else if constexpr (std::is_same_v<T, EventHandler>) {
      return event_handler.Get();
    } else if constexpr (std::is_same_v<T, AssetManager>) {
      return asset_manager.Get();
    } else if constexpr (std::is_same_v<T, EntityMemoryPool>) {
      return entity_pool.Get();
    } else if constexpr (std::is_same_v<T, ArchetypeManager>) {
      return archetype_manager.Get();
    } else if constexpr (std::is_same_v<T, sf::RenderTexture>) {
      return render_texture.Get();
    } else {
      static_assert(always_false<T>, "Unsupported resource type");
    }
  }
  
  // Clear all resources
  void Clear() {
    render_window.Reset();
    event_handler.Reset();
    asset_manager.Reset();
    display_manager.Reset();
    scene_manager.Reset();
    entity_pool.Reset();
    archetype_manager.Reset();
    render_texture.Reset();
    loop_number.Reset();
  }
  
private:
  template<typename> static constexpr bool always_false = false;
};

} // namespace steamrot
```

#### 2.3 ResourceBuilder - Constructs Resources from Configuration

**File**: `src/resources/ResourceBuilder.h`

```cpp
namespace steamrot {

class ResourceBuilder {
private:
  const ResourceManifest* m_manifest{nullptr};
  ResourceCollection m_collection;
  
  // Create individual resources based on config
  std::expected<std::monostate, FailInfo>
  CreateRenderWindow(const RenderWindowConfig* config);
  
  std::expected<std::monostate, FailInfo>
  CreateEventHandler();
  
  std::expected<std::monostate, FailInfo>
  CreateAssetManager(const AssetManagerConfig* config);
  
  std::expected<std::monostate, FailInfo>
  CreateEntityMemoryPool(const EntityMemoryPoolConfig* config);
  
  std::expected<std::monostate, FailInfo>
  CreateArchetypeManager();
  
  std::expected<std::monostate, FailInfo>
  CreateRenderTexture(const RenderTextureConfig* config);
  
public:
  explicit ResourceBuilder(const ResourceManifest* manifest);
  
  // Build resources from a named preset
  std::expected<ResourceCollection, FailInfo>
  BuildFromPreset(const std::string& preset_name);
  
  // Build specific resources by type
  ResourceBuilder& WithRenderWindow(const RenderWindowConfig* config = nullptr);
  ResourceBuilder& WithEventHandler();
  ResourceBuilder& WithAssetManager(const AssetManagerConfig* config = nullptr);
  ResourceBuilder& WithEntityMemoryPool(const EntityMemoryPoolConfig* config = nullptr);
  ResourceBuilder& WithArchetypeManager();
  ResourceBuilder& WithRenderTexture(const RenderTextureConfig* config = nullptr);
  
  // Build and return collection
  std::expected<ResourceCollection, FailInfo> Build();
};

} // namespace steamrot
```

#### 2.4 ResourceManager - Central Resource Registry

**File**: `src/resources/ResourceManager.h`

```cpp
namespace steamrot {

class ResourceManager {
private:
  // Static registry pattern (similar to ContextDirector)
  static std::unique_ptr<ResourceCollection> s_production_resources;
  static std::unique_ptr<ResourceCollection> s_test_resources;
  static const ResourceManifest* s_production_manifest;
  static const ResourceManifest* s_test_manifest;
  
  ResourceManager() = delete;
  
public:
  // Initialize resource manager with manifests
  static std::expected<std::monostate, FailInfo>
  Initialize(const ResourceManifest* production_manifest,
             const ResourceManifest* test_manifest = nullptr);
  
  // Get or create production resources
  static std::expected<ResourceCollection&, FailInfo>
  GetProductionResources();
  
  // Get or create test resources from preset
  static std::expected<ResourceCollection, FailInfo>
  CreateTestResources(const std::string& preset_name);
  
  // Build resources from preset (doesn't cache)
  static std::expected<ResourceCollection, FailInfo>
  BuildResourcesFromPreset(const std::string& preset_name,
                          EnvironmentType env = EnvironmentType::Test);
  
  // Check if initialized
  static bool IsInitialized();
  
  // Clear all cached resources
  static void ClearResources();
  
  // Shutdown resource manager
  static void Shutdown();
};

} // namespace steamrot
```

### Phase 3: Integration with Existing Systems

#### 3.1 Update GameEngine to Use ResourceManager

**Modified**: `src/systems/GameEngine.h`

```cpp
class GameEngine {
private:
  // NEW: Resource collection instead of individual members
  ResourceCollection m_resources;
  
  // Keep context (now points into resource collection)
  GameContext m_game_context;
  
  // ... existing methods ...
  
public:
  // Modified constructor - accepts resource collection
  GameEngine(ResourceCollection resources,
             const EnvironmentType env_type = EnvironmentType::None);
  
  // NEW: Static factory method using ResourceManager
  static std::expected<GameEngine, FailInfo>
  CreateFromPreset(const std::string& preset_name = "full_game");
  
  // ... existing methods ...
};
```

**Usage in main.cpp**:

```cpp
// Old way (still supported during migration)
GameEngine game_engine(EnvironmentType::Production);

// New way (using ResourceManager)
auto manifest = LoadResourceManifest(); // from FlatbuffersDataLoader
ResourceManager::Initialize(manifest);
auto engine = GameEngine::CreateFromPreset("full_game");
```

#### 3.2 Modernize TestContext to Use ResourceManager

**Modified**: `tests/context/TestContext.h`

```cpp
namespace steamrot::tests {

class TestContext {
private:
  ResourceCollection m_resources;
  std::unique_ptr<GameContext> m_game_context;
  std::unique_ptr<LogicContext> m_logic_context;
  
public:
  // Create from preset name
  explicit TestContext(const std::string& preset_name = "minimal_test");
  
  // Create for specific scene type (uses scene preset)
  static TestContext ForScene(SceneType scene_type);
  
  // Builder-style interface
  static TestContextBuilder WithPreset(const std::string& preset_name);
  
  // Get contexts (creates on demand)
  GameContext& GetGameContext();
  LogicContext& GetLogicContext();
  
  // Direct resource access for advanced cases
  ResourceCollection& GetResources() { return m_resources; }
  
  // Check what's available
  template <typename T>
  bool HasResource() const { return m_resources.Has<T>(); }
};

// Builder for custom test resource configurations
class TestContextBuilder {
private:
  ResourceBuilder m_builder;
  
public:
  explicit TestContextBuilder(const ResourceManifest* manifest);
  
  TestContextBuilder& WithEntityPool(size_t size = 100);
  TestContextBuilder& WithRenderTexture(uint32_t width = 800, uint32_t height = 600);
  TestContextBuilder& WithAssetManager(bool load_defaults = false);
  TestContextBuilder& WithEventHandler();
  
  std::expected<TestContext, FailInfo> Build();
};

} // namespace steamrot::tests
```

**New Usage Examples**:

```cpp
// Minimal unit test - just entity pool
TEST_CASE("Minimal test", "[unit]") {
  steamrot::tests::TestContext context("minimal_test");
  
  REQUIRE(context.HasResource<EntityMemoryPool>());
  REQUIRE_FALSE(context.HasResource<sf::RenderWindow>());
  
  auto& pool = context.GetResources().Get<EntityMemoryPool>();
  // Use pool...
}

// UI test - needs more resources
TEST_CASE("UI rendering test", "[unit][ui]") {
  steamrot::tests::TestContext context("ui_test");
  
  REQUIRE(context.HasResource<EntityMemoryPool>());
  REQUIRE(context.HasResource<sf::RenderTexture>());
  REQUIRE(context.HasResource<AssetManager>());
  
  auto& logic_ctx = context.GetLogicContext();
  // Test UI logic...
}

// Scene-specific test
TEST_CASE("Title scene test", "[integration]") {
  auto context = steamrot::tests::TestContext::ForScene(SceneType::SceneType_TITLE);
  
  auto& logic_ctx = context.GetLogicContext();
  // Test title scene...
}

// Custom test with builder
TEST_CASE("Custom resource test", "[unit]") {
  auto context = steamrot::tests::TestContextBuilder::WithPreset("minimal_test")
    .WithEntityPool(50)
    .WithEventHandler()
    .Build()
    .value();
  
  // Test with custom configuration...
}
```

### Phase 4: Data Loading Integration

#### 4.1 Extend FlatbuffersDataLoader

**Modified**: `src/data_handlers/FlatbuffersDataLoader.h`

```cpp
class FlatbuffersDataLoader {
  // ... existing methods ...
  
public:
  /////////////////////////////////////////////////
  /// @brief Provides ResourceManifest from binary file
  ///
  /// @param env Environment type (Test or Production)
  /////////////////////////////////////////////////
  std::expected<const ResourceManifest*, FailInfo>
  ProvideResourceManifest(EnvironmentType env = EnvironmentType::Production) const;
};
```

**Implementation**: `src/data_handlers/FlatbuffersDataLoader.cpp`

```cpp
std::expected<const ResourceManifest*, FailInfo>
FlatbuffersDataLoader::ProvideResourceManifest(EnvironmentType env) const {
  std::string file_path;
  
  if (env == EnvironmentType::Test) {
    file_path = m_path_provider.GetAbsolutePath(
      "resources/test_resource_presets.bin", DataDirectories::TestData);
  } else {
    file_path = m_path_provider.GetAbsolutePath(
      "resources/resource_manifest.bin", DataDirectories::Data);
  }
  
  // Load and verify...
  // Return ResourceManifest*
}
```

## Implementation Roadmap

### Stage 1: Foundation (Week 1)

**Goal**: Create core resource management infrastructure

- [ ] Create `resource_management.fbs` schema
- [ ] Implement `ResourceHandle<T>` template class
- [ ] Implement `ResourceCollection` struct
- [ ] Add unit tests for ResourceHandle and ResourceCollection
- [ ] Create example configuration files
- [ ] Integrate schema into build system

**Deliverables**:
- Schema compiled and headers generated
- ResourceHandle and ResourceCollection tested
- Example JSON files for reference

### Stage 2: Resource Builder (Week 2)

**Goal**: Implement resource construction from configuration

- [ ] Implement `ResourceBuilder` class
- [ ] Implement individual resource creation methods
- [ ] Add preset-based building
- [ ] Add fluent builder interface
- [ ] Comprehensive unit tests for builder
- [ ] Integration tests with real configurations

**Deliverables**:
- ResourceBuilder fully functional
- Can build resources from presets
- Can build custom resource combinations

### Stage 3: Resource Manager (Week 3)

**Goal**: Central resource registry and lifecycle management

- [ ] Implement `ResourceManager` static registry
- [ ] Implement initialization and shutdown
- [ ] Add production/test resource caching
- [ ] Integrate with FlatbuffersDataLoader
- [ ] Add lifecycle tests
- [ ] Document resource manager patterns

**Deliverables**:
- ResourceManager operational
- Production and test manifests loadable
- Cached resources accessible

### Stage 4: GameEngine Integration (Week 4)

**Goal**: Migrate GameEngine to use ResourceManager

- [ ] Add ResourceCollection member to GameEngine
- [ ] Implement factory method CreateFromPreset()
- [ ] Update GameContext to reference resources from collection
- [ ] Maintain backward compatibility with old constructor
- [ ] Update GameEngine tests
- [ ] Update documentation

**Deliverables**:
- GameEngine works with ResourceManager
- Both old and new initialization paths work
- Tests passing

### Stage 5: Test Infrastructure Migration (Week 5)

**Goal**: Modernize TestContext to use ResourceManager

- [ ] Implement new TestContext with preset support
- [ ] Implement TestContextBuilder
- [ ] Create test resource presets
- [ ] Add TestContext::ForScene() factory
- [ ] Migrate example tests to new API
- [ ] Document new test patterns

**Deliverables**:
- TestContext uses ResourceManager
- Preset-based test creation works
- Migration guide for tests
- Example tests migrated

### Stage 6: Full Migration (Week 6-7)

**Goal**: Migrate all code to new resource system

- [ ] Migrate all test files to use presets
- [ ] Remove old TestContext implementation
- [ ] Remove GameEngine old constructor
- [ ] Update Scene/SceneFactory resource handling
- [ ] Full integration testing
- [ ] Performance benchmarking

**Deliverables**:
- All code uses ResourceManager
- Old code paths removed
- Performance acceptable
- All tests passing

### Stage 7: Documentation and Polish (Week 8)

**Goal**: Complete documentation and examples

- [ ] Write comprehensive usage guide
- [ ] Create migration guide from old to new
- [ ] Add troubleshooting section
- [ ] Document all preset configurations
- [ ] Add cookbook of common patterns
- [ ] Final code review

**Deliverables**:
- Complete documentation
- Migration guide
- All patterns documented
- Ready for production use

## Benefits

### For Testing

1. **Faster Tests**: Only initialize what's needed
   - Unit test with just EntityMemoryPool: ~1ms setup
   - Full scene test: ~50ms setup
   - Old way: ~100ms setup for everything

2. **Clearer Test Intent**: Preset names document requirements
   ```cpp
   TestContext context("minimal_test");  // Obvious: minimal resources
   TestContext context("ui_test");       // Obvious: UI resources needed
   ```

3. **Easier Test Setup**: No manual resource configuration
   ```cpp
   // Old way
   TestContext ctx;
   auto& pool = ctx.scene_entities;
   auto& manager = ctx.archetype_manager;
   // ... lots of setup
   
   // New way
   auto ctx = TestContext("logic_test");  // Everything ready
   ```

4. **Reusable Configurations**: Share presets across tests
   - All UI tests use "ui_test" preset
   - All logic tests use "logic_test" preset
   - Integration tests use "full_scene_test"

### For Production

1. **Data-Driven Resource Setup**: Configure without recompiling
   - Window size in JSON
   - Asset loading in JSON
   - Scene configurations in JSON

2. **Flexible Initialization**: Load only what's needed
   - Menu scene: minimal resources
   - Crafting scene: full resources
   - Title scene: moderate resources

3. **Consistent Resource Management**: Same patterns everywhere
   - Production uses ResourceManager
   - Tests use ResourceManager
   - Same configuration format

4. **Better Resource Control**: Explicit lifecycle management
   - Know when resources are created
   - Control resource sharing
   - Easy cleanup

### For Maintainability

1. **Single Source of Truth**: No duplicate configurations
   - Production manifest defines resources
   - Test presets reference same configs
   - Changes propagate automatically

2. **Type-Safe Resource Access**: Compile-time checking
   ```cpp
   if (resources.Has<EntityMemoryPool>()) {
     auto& pool = resources.Get<EntityMemoryPool>();  // Type-safe
   }
   ```

3. **Clear Dependencies**: Preset lists required resources
   ```json
   {
     "name": "ui_test",
     "resources": ["EntityMemoryPool", "RenderTexture", "AssetManager"]
   }
   ```

4. **Easier Refactoring**: Add/remove resources in one place
   - New resource type: Add to schema
   - New preset: Add to JSON
   - Update ResourceCollection

## Migration Strategy

### Backward Compatibility

During migration, both systems coexist:

```cpp
// Old way (Phase 1-4)
GameEngine old_engine(EnvironmentType::Production);
TestContext old_test;

// New way (Phase 5+)
auto new_engine = GameEngine::CreateFromPreset("full_game");
TestContext new_test("minimal_test");
```

### Gradual Migration Path

1. **Week 1-2**: Infrastructure exists, nothing uses it yet
2. **Week 3-4**: GameEngine supports both old and new
3. **Week 5**: TestContext supports both old and new
4. **Week 6-7**: Migrate all code to new system
5. **Week 8**: Remove old system

### Testing During Migration

- Keep all existing tests running with old TestContext
- Add new tests using new TestContext in parallel
- Gradually migrate tests file by file
- Remove old TestContext only when all tests migrated

## Risk Mitigation

### Performance Risk

**Risk**: Resource system adds overhead

**Mitigation**:
- Benchmarking before/after migration
- ResourceHandle is lightweight wrapper around shared_ptr
- Lazy initialization only creates what's needed
- Caching in ResourceManager prevents repeated creation

**Validation**: Performance tests show <5% overhead acceptable

### Complexity Risk

**Risk**: System too complex to use

**Mitigation**:
- Presets provide simple "one-liner" setup
- Builder provides escape hatch for custom cases
- Comprehensive examples and documentation
- Migration guide with patterns

**Validation**: New test setup is simpler than old

### Breaking Changes Risk

**Risk**: Existing code stops working

**Mitigation**:
- Maintain backward compatibility during migration
- Deprecation warnings before removal
- Gradual migration over weeks
- Comprehensive test suite validates changes

**Validation**: All tests pass at each stage

### Configuration Complexity Risk

**Risk**: JSON configurations become unwieldy

**Mitigation**:
- Presets handle 90% of cases
- Default values for all fields
- Clear examples and documentation
- Validation errors point to specific issues

**Validation**: Most tests use simple preset names

## Future Enhancements

### Phase 2 Extensions (Post-Initial Implementation)

1. **Resource Dependencies**
   - Declare dependencies between resources
   - Automatic dependency resolution
   - Validation of configuration completeness

2. **Resource Pools**
   - Shared resource pools for tests
   - Resource reuse across test cases
   - Memory-efficient test suites

3. **Hot Reload**
   - Reload resource configurations at runtime
   - Useful for development/debugging
   - No recompilation needed

4. **Resource Profiling**
   - Track resource usage and lifetime
   - Identify resource leaks
   - Performance profiling

5. **Advanced Presets**
   - Preset inheritance/composition
   - Conditional resource inclusion
   - Platform-specific presets

6. **Validation and Schema Evolution**
   - Configuration validation
   - Schema versioning
   - Migration between schema versions

## Appendix A: Complete File Structure

```
src/
├── resources/                      [NEW]
│   ├── ResourceHandle.h           [NEW]
│   ├── ResourceCollection.h       [NEW]
│   ├── ResourceCollection.cpp     [NEW]
│   ├── ResourceBuilder.h          [NEW]
│   ├── ResourceBuilder.cpp        [NEW]
│   ├── ResourceManager.h          [NEW]
│   ├── ResourceManager.cpp        [NEW]
│   └── CMakeLists.txt             [NEW]
├── flatbuffers_headers/
│   ├── resource_management.fbs    [NEW]
│   └── ...
├── systems/
│   ├── GameEngine.h               [MODIFIED]
│   ├── GameEngine.cpp             [MODIFIED]
│   └── ...
├── data_handlers/
│   ├── FlatbuffersDataLoader.h    [MODIFIED]
│   ├── FlatbuffersDataLoader.cpp  [MODIFIED]
│   └── ...
└── ...

data/
├── resources/                     [NEW]
│   ├── resource_manifest.json     [NEW]
│   └── resource_manifest.bin      [NEW - generated]
└── ...

tests/
├── context/
│   ├── TestContext.h              [MODIFIED]
│   ├── TestContext.cpp            [MODIFIED]
│   ├── TestContextBuilder.h       [NEW]
│   ├── TestContextBuilder.cpp     [NEW]
│   └── ...
├── data/
│   └── resources/                 [NEW]
│       ├── test_resource_presets.json  [NEW]
│       └── test_resource_presets.bin   [NEW - generated]
├── unit/
│   └── resources/                 [NEW]
│       ├── ResourceHandle.test.cpp     [NEW]
│       ├── ResourceCollection.test.cpp [NEW]
│       ├── ResourceBuilder.test.cpp    [NEW]
│       ├── ResourceManager.test.cpp    [NEW]
│       └── CMakeLists.txt             [NEW]
└── ...

documentation/
├── RESOURCE_MANAGEMENT_PLAN.md    [NEW - this document]
├── RESOURCE_MANAGEMENT_GUIDE.md   [NEW - usage guide]
└── RESOURCE_MIGRATION_GUIDE.md    [NEW - migration guide]
```

## Appendix B: Integration with Context System

The resource management system complements the existing context system:

### Relationship Between Systems

```
ResourceManager (owns resources)
       │
       ├─> ResourceCollection (storage)
       │
       ▼
ContextBuilder (references resources)
       │
       ├─> GameContextBuilder
       ├─> LogicContextBuilder
       │
       ▼
Context Objects (use resources)
       │
       ├─> GameContext
       ├─> LogicContext
       │
       ▼
Logic/Systems (consume contexts)
```

### Unified Workflow

```cpp
// 1. Load resource manifest
auto manifest = data_loader.ProvideResourceManifest();

// 2. Initialize resource manager
ResourceManager::Initialize(manifest);

// 3. Build resources from preset
auto resources = ResourceManager::BuildResourcesFromPreset("full_game");

// 4. Create contexts from resources
GameContextBuilder ctx_builder;
ctx_builder.SetWindow(resources.render_window.GetShared())
           .SetEventHandler(resources.event_handler.GetShared())
           .SetAssetManager(resources.asset_manager.GetShared())
           .SetLoopNumber(resources.loop_number.GetShared())
           .SetEnvironmentType(resources.env_type);

auto game_context = ctx_builder.Build();

// 5. Use context in engine
GameEngine engine(resources, game_context);
```

### Benefits of Separation

1. **ResourceManager**: Handles resource **lifetime**
2. **ContextBuilder**: Handles context **configuration**
3. **Context**: Provides **access** to resources

Each layer has clear responsibility and can evolve independently.

## Appendix C: Comparison with Current System

### Current System

```cpp
// TestContext.h - duplicates production resources
class TestContext {
private:
  sf::RenderWindow render_window;         // Duplicate
  EventHandler event_handler;             // Duplicate
  AssetManager asset_manager;             // Duplicate
  EntityMemoryPool scene_entities;        // Test-specific
  // ... hardcoded initialization
  
public:
  TestContext(SceneType scene = SceneType::SceneType_TEST) {
    // Always creates everything
    // Always same configuration
    // Can't customize
  }
};

// Usage - no control over resources
TEST_CASE("My test", "[unit]") {
  TestContext ctx;  // Gets everything, no choice
  // Use ctx...
}
```

### New System

```cpp
// TestContext.h - uses shared resource system
class TestContext {
private:
  ResourceCollection m_resources;  // From ResourceManager
  
public:
  // Create from preset (data-driven)
  explicit TestContext(const std::string& preset = "minimal_test") {
    m_resources = ResourceManager::BuildResourcesFromPreset(preset);
  }
};

// Usage - explicit control
TEST_CASE("Minimal test", "[unit]") {
  TestContext ctx("minimal_test");  // Only EntityMemoryPool
}

TEST_CASE("UI test", "[unit][ui]") {
  TestContext ctx("ui_test");  // EntityPool + RenderTexture + Assets
}

TEST_CASE("Custom test", "[unit]") {
  auto ctx = TestContextBuilder::WithPreset("minimal_test")
    .WithRenderTexture()
    .Build();
}
```

### Key Improvements

1. **Flexibility**: Choose resources via preset name
2. **Performance**: Only create what's needed
3. **Clarity**: Preset name documents requirements
4. **Reusability**: Share presets across tests
5. **Data-Driven**: Configure in JSON, not C++

## Conclusion

The Resource Management System provides:

1. **Unified resource handling** across production and test environments
2. **Data-driven configuration** for flexible resource initialization
3. **Composable resource setup** with presets and builder patterns
4. **Clear lifecycle management** with explicit initialization/cleanup
5. **Better test performance** by loading only required resources
6. **Maintainable codebase** with single source of truth

The phased implementation (8 weeks) allows gradual migration while maintaining backward compatibility, reducing risk while delivering continuous value.

This plan integrates seamlessly with existing systems (Context handling, FlatBuffers data loading) and follows established patterns (builders, static registries, data-driven configuration).

**Next Step**: Review and approval, then begin Stage 1 implementation.
