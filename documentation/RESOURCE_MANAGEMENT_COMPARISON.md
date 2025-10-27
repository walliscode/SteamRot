# Resource Management System - Before & After Comparison

**Purpose**: Visual comparison showing improvements from current to planned system

## High-Level Comparison

| Aspect | Current System | Planned System |
|--------|---------------|----------------|
| **Resource Definition** | Hardcoded in C++ constructors | Data-driven via FlatBuffers JSON |
| **Test Resources** | Duplicate implementation in TestContext | Shared with production via presets |
| **Initialization** | All-or-nothing (full context always) | Flexible levels (minimal to full) |
| **Configuration** | Recompile to change window size, etc. | Edit JSON file |
| **Test Setup Time** | ~100ms for all tests | 1-154ms depending on needs |
| **Resource Management** | Implicit (member variables) | Explicit (ResourceManager) |
| **Lifecycle Control** | Automatic (constructor/destructor) | Explicit (Initialize/Shutdown) |

## Code Comparison

### Creating Test Environment

#### Current Approach
```cpp
// TestContext.h - hardcoded, always creates everything
class TestContext {
private:
  sf::RenderWindow render_window;        // Always created
  EventHandler event_handler;            // Always created
  AssetManager asset_manager;            // Always created
  EntityMemoryPool scene_entities;       // Always created
  ArchetypeManager archetype_manager;    // Always created
  sf::RenderTexture render_texture;      // Always created
  
public:
  // Constructor creates all resources
  TestContext(SceneType scene = SceneType::SceneType_TEST) {
    // Hardcoded initialization
    render_window.create(sf::VideoMode(800, 600), "Test");
    asset_manager.LoadDefaultAssets();
    // ... more initialization
  }
};

// Usage - no control over what's created
TEST_CASE("Simple component test", "[unit]") {
  TestContext ctx;  // Creates everything (100ms)
  auto& pool = ctx.scene_entities;
  // Only need EntityMemoryPool, but got everything
}
```

**Problems**:
- 🔴 Always creates RenderWindow (unnecessary for most tests)
- 🔴 Always loads assets (50ms overhead)
- 🔴 No way to customize initialization
- 🔴 Separate implementation from production
- 🔴 Hardcoded configuration values

#### Planned Approach
```cpp
// TestContext.h - flexible, preset-based
class TestContext {
private:
  ResourceCollection m_resources;  // Only requested resources
  
public:
  // Create from preset name
  explicit TestContext(const std::string& preset = "minimal_test") {
    m_resources = ResourceManager::BuildResourcesFromPreset(preset);
  }
};

// Usage - explicit control
TEST_CASE("Simple component test", "[unit]") {
  TestContext ctx("minimal_test");  // Just EntityMemoryPool (1ms)
  auto& pool = ctx.GetResources().Get<EntityMemoryPool>();
  // Only created what's needed
}
```

**Improvements**:
- ✅ Creates only requested resources
- ✅ 100x faster for simple tests
- ✅ Preset name documents requirements
- ✅ Shares configuration with production
- ✅ Data-driven from JSON

### Resource Configuration

#### Current Approach
```cpp
// GameEngine.cpp - hardcoded values
GameEngine::GameEngine(const EnvironmentType env_type)
    : m_window(sf::VideoMode(800, 600), "SteamRot"),  // Hardcoded
      m_event_handler(),
      m_asset_manager() {
  
  m_window.setFramerateLimit(60);  // Hardcoded
  
  // To change window size, must recompile
}
```

**Problems**:
- 🔴 Window size hardcoded
- 🔴 Window title hardcoded
- 🔴 Framerate limit hardcoded
- 🔴 Must recompile to change settings
- 🔴 No environment-specific configs

#### Planned Approach
```json
// data/resources/resource_manifest.json
{
  "presets": [
    {
      "name": "full_game",
      "resources": [
        {
          "resource_type": "RenderWindow",
          "render_window_config": {
            "width": 800,
            "height": 600,
            "title": "SteamRot",
            "framerate_limit": 60
          }
        }
      ]
    }
  ]
}
```

```cpp
// GameEngine.cpp - data-driven
auto manifest = loader.ProvideResourceManifest();
ResourceManager::Initialize(manifest);
auto resources = ResourceManager::BuildResourcesFromPreset("full_game");

// Window created from configuration
// To change settings, edit JSON and rebuild (no recompile)
```

**Improvements**:
- ✅ All settings in JSON
- ✅ No recompilation for config changes
- ✅ Version controlled configurations
- ✅ Environment-specific configs easy
- ✅ Self-documenting via JSON

### Test Preset Levels

#### Current Approach
```cpp
// Only one option - full context
TEST_CASE("Minimal test", "[unit]") {
  TestContext ctx;  // Gets:
                    // - RenderWindow (100ms)
                    // - AssetManager with defaults (50ms)
                    // - EventHandler
                    // - EntityMemoryPool
                    // - ArchetypeManager
                    // - RenderTexture
                    // Total: ~160ms setup
  
  auto& pool = ctx.scene_entities;
  // Use EntityMemoryPool only
}
```

**Problems**:
- 🔴 150ms overhead for resources not used
- 🔴 Memory waste (RenderWindow, textures, assets)
- 🔴 Can't express "just need X"
- 🔴 All tests pay same initialization cost

#### Planned Approach
```cpp
// Multiple preset options

// Option 1: Minimal (just EntityMemoryPool)
TEST_CASE("Minimal test", "[unit]") {
  TestContext ctx("minimal_test");  // ~1ms setup
  auto& pool = ctx.GetResources().Get<EntityMemoryPool>();
}

// Option 2: UI testing
TEST_CASE("UI test", "[unit][ui]") {
  TestContext ctx("ui_test");  // ~52ms setup
  auto& pool = ctx.GetResources().Get<EntityMemoryPool>();
  auto& texture = ctx.GetResources().Get<sf::RenderTexture>();
  auto& assets = ctx.GetResources().Get<AssetManager>();
}

// Option 3: Logic testing
TEST_CASE("Logic test", "[unit][logic]") {
  TestContext ctx("logic_test");  // ~54ms setup
  auto& logic_ctx = ctx.GetLogicContext();
  MyLogic logic(logic_ctx);
}

// Option 4: Full integration
TEST_CASE("Integration test", "[integration]") {
  TestContext ctx("full_scene_test");  // ~154ms setup
  // Complete scene infrastructure
}
```

**Improvements**:
- ✅ 160x faster for minimal tests (1ms vs 160ms)
- ✅ Memory usage matches actual needs
- ✅ Preset name expresses intent
- ✅ Choose appropriate level per test

## Performance Comparison

### Test Initialization Times

| Test Type | Current (All Resources) | Planned (Appropriate Preset) | Improvement |
|-----------|------------------------|------------------------------|-------------|
| Component unit test | 160ms | 1ms (minimal_test) | **160x faster** |
| UI rendering test | 160ms | 52ms (ui_test) | **3x faster** |
| Logic class test | 160ms | 54ms (logic_test) | **3x faster** |
| Scene integration test | 160ms | 154ms (full_scene_test) | **Similar** |

### Resource Breakdown

| Resource | Current (Always) | minimal_test | ui_test | logic_test | full_scene_test |
|----------|-----------------|--------------|---------|------------|-----------------|
| RenderWindow (100ms) | ✓ | ✗ | ✗ | ✗ | ✓ |
| AssetManager (50ms) | ✓ | ✗ | ✓ | ✓ | ✓ |
| EventHandler (<1ms) | ✓ | ✗ | ✗ | ✓ | ✓ |
| EntityMemoryPool (<1ms) | ✓ | ✓ | ✓ | ✓ | ✓ |
| ArchetypeManager (<1ms) | ✓ | ✗ | ✗ | ✓ | ✓ |
| RenderTexture (2ms) | ✓ | ✗ | ✓ | ✓ | ✓ |
| **Total Time** | **~160ms** | **~1ms** | **~52ms** | **~54ms** | **~154ms** |

## Configuration File Comparison

### Current: Hardcoded in C++

**GameEngine Constructor**:
```cpp
// src/systems/GameEngine.cpp (lines 15-25)
GameEngine::GameEngine(const EnvironmentType env_type)
    : m_window(sf::VideoMode(800, 600), "SteamRot"),
      m_event_handler(),
      m_asset_manager(),
      m_game_context(m_window, m_event_handler, m_loop_number, 
                     m_asset_manager, env_type),
      m_scene_manager(m_game_context),
      m_display_manager(m_game_context) {
  
  m_window.setFramerateLimit(60);
}
```

**Problems**:
- Values scattered across codebase
- Must grep to find all initializations
- No central configuration view
- Recompile required for changes

### Planned: Data-Driven in JSON

**Production Config**: `data/resources/resource_manifest.json`
```json
{
  "presets": [
    {
      "name": "full_game",
      "description": "Complete production resources",
      "resources": [
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
      ]
    }
  ]
}
```

**Test Config**: `tests/data/resources/test_resource_presets.json`
```json
{
  "presets": [
    {
      "name": "minimal_test",
      "description": "Just EntityMemoryPool for unit tests",
      "resources": [
        {
          "resource_type": "EntityMemoryPool",
          "entity_pool_config": {"pool_size": 10}
        }
      ]
    },
    {
      "name": "ui_test",
      "description": "UI testing resources",
      "resources": [
        {
          "resource_type": "EntityMemoryPool",
          "entity_pool_config": {"pool_size": 50}
        },
        {
          "resource_type": "RenderTexture",
          "render_texture_config": {"width": 800, "height": 600}
        },
        {
          "resource_type": "AssetManager",
          "asset_manager_config": {"load_default_assets": true}
        }
      ]
    }
  ]
}
```

**Benefits**:
- ✅ All configuration in one place
- ✅ Easy to review and modify
- ✅ Self-documenting with descriptions
- ✅ Version controlled
- ✅ No recompilation needed

## API Comparison

### GameEngine Creation

#### Current
```cpp
// Only one way - hardcoded initialization
int main() {
  GameEngine engine(EnvironmentType::Production);
  engine.RunGame();
}
```

#### Planned
```cpp
// Option 1: From preset (recommended)
int main() {
  auto manifest = LoadResourceManifest();
  ResourceManager::Initialize(manifest);
  
  auto engine = GameEngine::CreateFromPreset("full_game");
  engine.RunGame();
}

// Option 2: Custom resources
int main() {
  auto manifest = LoadResourceManifest();
  ResourceBuilder builder(manifest);
  
  auto resources = builder
    .WithRenderWindow()
    .WithEventHandler()
    .WithAssetManager()
    .Build()
    .value();
  
  GameEngine engine(resources);
  engine.RunGame();
}

// Option 3: Old way (backward compatible during migration)
int main() {
  GameEngine engine(EnvironmentType::Production);
  engine.RunGame();
}
```

### TestContext Creation

#### Current
```cpp
// Only one option
TEST_CASE("My test", "[unit]") {
  TestContext ctx;  // Always gets everything
  TestContext ctx(SceneType::SceneType_TITLE);  // Still gets everything
}
```

#### Planned
```cpp
// Many options

// Option 1: Simple preset
TEST_CASE("My test", "[unit]") {
  TestContext ctx("minimal_test");
}

// Option 2: Scene-specific
TEST_CASE("Scene test", "[integration]") {
  auto ctx = TestContext::ForScene(SceneType::SceneType_TITLE);
}

// Option 3: Custom builder
TEST_CASE("Custom test", "[unit]") {
  auto ctx = TestContextBuilder::WithPreset("minimal_test")
    .WithRenderTexture(640, 480)
    .WithEventHandler()
    .Build()
    .value();
}

// Option 4: Check availability
TEST_CASE("Conditional test", "[unit]") {
  TestContext ctx("ui_test");
  
  REQUIRE(ctx.HasResource<EntityMemoryPool>());
  REQUIRE(ctx.HasResource<sf::RenderTexture>());
  
  if (ctx.HasResource<AssetManager>()) {
    auto& assets = ctx.GetResources().Get<AssetManager>();
  }
}
```

## Documentation Comparison

### Current
- Scattered across source files
- Implementation = documentation
- Must read code to understand resources
- No central reference

### Planned
- **RESOURCE_MANAGEMENT_PLAN.md**: Complete design (1,350 lines)
- **RESOURCE_MANAGEMENT_QUICK_REF.md**: Quick reference (540 lines)
- **RESOURCE_MANAGEMENT_SUMMARY.md**: Executive summary
- **JSON configs**: Self-documenting with descriptions
- **Code examples**: Comprehensive usage patterns

## Migration Path

### Phase 1: Coexistence (Weeks 1-4)
```cpp
// Old way still works
GameEngine old_engine(EnvironmentType::Production);
TestContext old_test;

// New way available
auto new_engine = GameEngine::CreateFromPreset("full_game");
TestContext new_test("minimal_test");
```

### Phase 2: Gradual Migration (Weeks 5-7)
- Migrate tests file by file to presets
- Update production code to use ResourceManager
- Keep both paths working
- Comprehensive testing

### Phase 3: Cleanup (Week 8)
- Remove old constructors
- Update all documentation
- Final performance validation
- Production ready

## Key Improvements Summary

### 🚀 Performance
- **160x faster** for minimal unit tests
- **3x faster** for UI/logic tests  
- Memory usage matches actual needs

### 📝 Maintainability
- Single source of truth for configuration
- No resource duplication between prod/test
- Version controlled JSON configs
- Self-documenting preset names

### 🔧 Flexibility
- Choose initialization level per test
- Easy to add new resource types
- Composable via builder pattern
- Custom configurations when needed

### 🧪 Testing
- Clear intent via preset names
- Reusable configurations
- No "test-only" implementations
- Easy to write fast tests

### 🏗️ Architecture
- Separation of concerns (Resource/Context/Usage)
- Type-safe resource access
- Explicit lifecycle management
- Clear dependency declarations

---

**Conclusion**: The planned system addresses all current pain points while providing a foundation for future enhancements. The migration path ensures zero disruption during the transition.
