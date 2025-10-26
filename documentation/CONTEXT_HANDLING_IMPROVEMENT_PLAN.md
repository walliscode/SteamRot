# Context Handling System Improvement Plan

## Executive Summary

This document presents a comprehensive analysis of the current context handling system in SteamRot and proposes improvements to make it more extensible, data-driven, and maintainable. The analysis covers both the main game context handling and test infrastructure.

## Current State Analysis

### 1. GameContext (Main Game)

**Location:** `src/context/GameContext.h/cpp`

**Current Structure:**
```cpp
struct GameContext {
  sf::RenderWindow &game_window;
  EventHandler &event_handler;
  sf::Vector2i mouse_position{0, 0};
  const size_t &loop_number;
  AssetManager &asset_manager;
  const EnvironmentType env_type;
};
```

**Instantiation:** Created once in `GameEngine` constructor and passed by const reference throughout the system.

**Issues:**
1. **Tight Coupling**: All components are required at construction time
2. **Non-extensible**: Adding new context data requires changing the struct and all instantiation sites
3. **Not Data-Driven**: Configuration is hardcoded in C++
4. **Limited Construction Flexibility**: References must be initialized in constructor, limiting validation and error handling
5. **Single Point of Failure**: All context must be valid at GameEngine creation

**Note on Reference-Based Design:**
The current system uses references (not pointers) which provides important safety guarantees:
- **Compile-time lifetime safety**: References must be valid when created
- **No null state**: References cannot be null, preventing a class of runtime errors
- **Clear ownership**: References signal that the context doesn't own the resources
- **Strict ordering**: Forces proper initialization order, making bugs more visible

The proposed builder pattern preserves these benefits using `std::optional<std::reference_wrapper<T>>` instead of pointers.

### 2. LogicContext (Scene-Specific)

**Location:** `src/logic/LogicContext.h`

**Current Structure:**
```cpp
struct LogicContext {
  EntityMemoryPool &scene_entities;
  const std::unordered_map<ArchetypeID, Archetype> &archetypes;
  sf::RenderTexture &scene_texture;
  sf::RenderWindow &game_window;
  const AssetManager &asset_manager;
  EventHandler &event_handler;
  const sf::Vector2i &mouse_position;
};
```

**Instantiation:** Created in `Scene::GetLogicContext()` method, called during scene creation in `SceneFactory::CreateDefaultScene()`.

**Issues:**
1. **Duplication**: Overlaps with GameContext (game_window, asset_manager, event_handler, mouse_position)
2. **Tight Coupling**: Requires both Scene and GameContext to be fully initialized
3. **Creation Timing**: Created during scene initialization, limiting when logic can be created
4. **Not Reusable**: New LogicContext created each time GetLogicContext() is called
5. **No Lazy Initialization**: All scene resources must exist before any logic can be created

### 3. TestContext (Test Infrastructure)

**Location:** `tests/context/TestContext.h/cpp`

**Current Structure:**
```cpp
class TestContext {
private:
  sf::RenderWindow render_window;
  EventHandler event_handler;
  AssetManager asset_manager;
  const size_t loop_number{0};
  const EnvironmentType env_type{EnvironmentType::Test};
  EntityMemoryPool scene_entities;
  ArchetypeManager archetype_manager;
  sf::RenderTexture render_texture;
  std::unique_ptr<GameContext> game_context_ptr;
  std::unique_ptr<LogicContext> logic_context_for_test_scene;
  std::unique_ptr<LogicContext> logic_context_for_title_scene;
  std::unique_ptr<LogicContext> logic_context_for_crafting_scene;
};
```

**Issues:**
1. **Scene-Specific Logic Contexts**: Separate unique_ptr for each scene type is not scalable
2. **Eager Initialization**: Creates all objects in constructor based on scene type
3. **Hardcoded Scenes**: Adding new scenes requires modifying TestContext
4. **Duplicated Logic**: Scene configuration logic duplicated 3 times
5. **Heavy Construction**: Every test pays the cost of full initialization even if not needed

### 4. Context Flow Architecture

**Current Flow:**
```
GameEngine Constructor
  └─> Create GameContext (with all dependencies)
      └─> Pass to SceneManager
          └─> Pass to Scene
              └─> Scene::GetLogicContext() creates LogicContext
                  └─> Pass to LogicFactory
                      └─> Pass to each Logic instance (stored as copy)
```

**Problems:**
- Long dependency chain with no lazy evaluation
- No separation between construction and initialization
- Difficult to add optional context data
- Hard to test components in isolation

## Proposed Improvements

### Phase 1: Introduce Context Builder Pattern

**Design Philosophy: References Over Pointers**

The builder pattern implementation maintains the existing codebase's preference for references over pointers:

- **Builder Internal State**: Uses `std::optional<std::reference_wrapper<T>>` to store references during construction
  - Allows checking if a reference has been set (via `has_value()`)
  - Maintains reference semantics (no null pointers)
  - Enforces strict lifetime requirements
  - Forces errors at build time if references become invalid

- **Final Context**: Continues to use references (not pointers) matching the current design
  - No risk of dangling pointers in production code
  - Clear ownership semantics
  - Compiler-enforced lifetime guarantees

This approach provides builder pattern flexibility while preserving the safety and clarity of reference-based design.

#### 1.1 Create ContextBuilder Base Class

**New File:** `src/context/ContextBuilder.h`

```cpp
namespace steamrot {

template<typename ContextT>
class ContextBuilder {
protected:
  std::optional<ContextT> m_context;
  
public:
  virtual ~ContextBuilder() = default;
  
  // Build and validate context
  virtual std::expected<ContextT, FailInfo> Build() = 0;
  
  // Check if context is ready
  virtual bool IsValid() const = 0;
  
  // Reset builder state
  virtual void Reset() = 0;
};

} // namespace steamrot
```

#### 1.2 Create GameContextBuilder

**New File:** `src/context/GameContextBuilder.h`

```cpp
namespace steamrot {

class GameContextBuilder : public ContextBuilder<GameContext> {
private:
  std::optional<std::reference_wrapper<sf::RenderWindow>> m_window;
  std::optional<std::reference_wrapper<EventHandler>> m_event_handler;
  std::optional<std::reference_wrapper<AssetManager>> m_asset_manager;
  std::optional<std::reference_wrapper<const size_t>> m_loop_number;
  std::optional<EnvironmentType> m_env_type;
  
public:
  GameContextBuilder& SetWindow(sf::RenderWindow& window);
  GameContextBuilder& SetEventHandler(EventHandler& handler);
  GameContextBuilder& SetAssetManager(AssetManager& manager);
  GameContextBuilder& SetLoopNumber(const size_t& loop_num);
  GameContextBuilder& SetEnvironmentType(EnvironmentType env_type);
  
  std::expected<GameContext, FailInfo> Build() override;
  bool IsValid() const override;
  void Reset() override;
};

} // namespace steamrot
```

**Benefits:**
- Flexible construction order
- Easy validation before building
- Clear error messages for missing dependencies
- Testable in isolation
- **Uses references (via std::reference_wrapper) to maintain strict lifetime guarantees**
- **Forces errors at build time if references become invalid**

#### 1.3 Create LogicContextBuilder

**New File:** `src/logic/LogicContextBuilder.h`

```cpp
namespace steamrot {

class LogicContextBuilder : public ContextBuilder<LogicContext> {
private:
  std::optional<std::reference_wrapper<EntityMemoryPool>> m_scene_entities;
  std::optional<std::reference_wrapper<const std::unordered_map<ArchetypeID, Archetype>>> m_archetypes;
  std::optional<std::reference_wrapper<sf::RenderTexture>> m_scene_texture;
  std::optional<std::reference_wrapper<const GameContext>> m_game_context;
  
public:
  LogicContextBuilder& SetSceneEntities(EntityMemoryPool& entities);
  LogicContextBuilder& SetArchetypes(const ArchetypeManager& manager);
  LogicContextBuilder& SetSceneTexture(sf::RenderTexture& texture);
  LogicContextBuilder& SetGameContext(const GameContext& context);
  
  std::expected<LogicContext, FailInfo> Build() override;
  bool IsValid() const override;
  void Reset() override;
};

} // namespace steamrot
```

**Benefits:**
- Reuse GameContext data instead of duplicating
- Clear dependency on GameContext
- Can be configured incrementally
- Easy to extend with new scene-specific data
- **Uses references (via std::reference_wrapper) to enforce strict lifetime management**
- **Prevents dangling pointer issues by requiring valid references**

### Phase 2: Data-Driven Context Configuration

#### 2.1 Create Context Configuration Schema

**New File:** `src/flatbuffers_headers/context_config.fbs`

```fbs
namespace steamrot;

// Configuration for GameContext initialization
table GameContextConfig {
  window_width: uint32 = 800;
  window_height: uint32 = 600;
  window_title: string;
  framerate_limit: uint32 = 60;
  environment_type: string; // "Development", "Production", "Test"
}

// Configuration for scene-specific context
table SceneContextConfig {
  scene_type: string;
  entity_pool_size: uint32 = 100;
  render_texture_width: uint32 = 800;
  render_texture_height: uint32 = 600;
  // Future: add configurable context extensions
  extensions: [ContextExtension];
}

// Extensible context data
table ContextExtension {
  key: string;
  value_type: string; // "int", "float", "string", "bool"
  value_data: [ubyte]; // Serialized value
}

table ContextConfiguration {
  game_context: GameContextConfig;
  scene_contexts: [SceneContextConfig];
}
```

**Example JSON Configuration:**
```json
{
  "game_context": {
    "window_width": 800,
    "window_height": 600,
    "window_title": "SteamRot",
    "framerate_limit": 60,
    "environment_type": "Development"
  },
  "scene_contexts": [
    {
      "scene_type": "TITLE",
      "entity_pool_size": 50,
      "render_texture_width": 800,
      "render_texture_height": 600
    },
    {
      "scene_type": "CRAFTING",
      "entity_pool_size": 200,
      "render_texture_width": 800,
      "render_texture_height": 600
    }
  ]
}
```

#### 2.2 Create ContextConfigurator

**New File:** `src/context/ContextConfigurator.h`

```cpp
namespace steamrot {

class ContextConfigurator {
private:
  const ContextConfiguration* m_config_data{nullptr};
  
  std::expected<EnvironmentType, FailInfo> 
  ParseEnvironmentType(const std::string& type_str);
  
public:
  explicit ContextConfigurator(const ContextConfiguration* config);
  
  std::expected<GameContextBuilder, FailInfo> 
  CreateGameContextBuilder() const;
  
  std::expected<LogicContextBuilder, FailInfo>
  CreateLogicContextBuilder(const SceneType& scene_type) const;
  
  std::expected<std::monostate, FailInfo>
  LoadFromFile(const std::string& config_path);
};

} // namespace steamrot
```

**Benefits:**
- Game configuration externalized to JSON
- No code changes needed for common adjustments
- Easy to have different configs for different environments
- Version control friendly

### Phase 3: Lazy Context Initialization

#### 3.1 Create ContextManager (Singleton)

**New File:** `src/context/ContextManager.h`

```cpp
namespace steamrot {

class ContextManager {
private:
  static std::unique_ptr<ContextManager> s_instance;
  
  std::optional<GameContext> m_game_context;
  std::unordered_map<SceneType, LogicContextBuilder> m_logic_context_builders;
  std::unique_ptr<ContextConfigurator> m_configurator;
  
  ContextManager() = default;
  
public:
  static ContextManager& GetInstance();
  
  // Initialize from configuration file
  std::expected<std::monostate, FailInfo>
  Initialize(const std::string& config_path);
  
  // Get or create GameContext
  std::expected<const GameContext&, FailInfo> GetGameContext();
  
  // Register scene-specific context builder
  void RegisterSceneContextBuilder(SceneType type, LogicContextBuilder builder);
  
  // Get or build LogicContext for a scene
  std::expected<LogicContext, FailInfo> 
  GetLogicContext(SceneType scene_type);
  
  // Clear all contexts (useful for scene transitions)
  void ClearSceneContexts();
  
  // For testing: reset singleton
  static void ResetForTesting();
};

} // namespace steamrot
```

**Benefits:**
- Centralized context management
- Lazy initialization (contexts created when first needed)
- Easy to reset for testing
- Single source of truth for context state

### Phase 4: Improve Test Infrastructure

#### 4.1 Refactor TestContext to Use Builders

**Modified:** `tests/context/TestContext.h`

```cpp
namespace steamrot::tests {

class TestContext {
private:
  // Core test resources
  sf::RenderWindow m_render_window;
  EventHandler m_event_handler;
  AssetManager m_asset_manager;
  const size_t m_loop_number{0};
  const EnvironmentType m_env_type{EnvironmentType::Test};
  
  // Builders for different contexts
  std::unique_ptr<GameContextBuilder> m_game_context_builder;
  std::unordered_map<SceneType, std::unique_ptr<LogicContextBuilder>> 
    m_logic_context_builders;
  
  // Cached contexts (built on demand)
  mutable std::optional<GameContext> m_game_context_cache;
  mutable std::unordered_map<SceneType, LogicContext> m_logic_context_cache;
  
  // Internal helpers
  void InitializeGameContextBuilder();
  void InitializeLogicContextBuilder(SceneType scene_type);
  
public:
  // Simplified constructor - doesn't require scene type upfront
  TestContext();
  
  // Get contexts (built lazily)
  const GameContext& GetGameContext() const;
  const LogicContext& GetLogicContext(SceneType scene_type) const;
  
  // Configure for specific test scenarios
  TestContext& WithSceneType(SceneType scene_type);
  TestContext& WithEntityCount(size_t count);
  TestContext& WithCustomConfig(const std::string& config_path);
  
  // Clear cached contexts (for multi-scene tests)
  void ClearContextCache();
};

} // namespace steamrot::tests
```

**Usage Example:**
```cpp
// Old way - forced to pick scene in constructor
TEST_CASE("Some test", "[unit]") {
  steamrot::tests::TestContext context{SceneType::SceneType_TITLE};
  auto& game_ctx = context.GetGameContext();
  auto& logic_ctx = context.GetLogicContextForTitleScene();
}

// New way - flexible and lazy
TEST_CASE("Some test", "[unit]") {
  steamrot::tests::TestContext context;
  context.WithSceneType(SceneType::SceneType_TITLE)
         .WithEntityCount(50);
  
  auto& game_ctx = context.GetGameContext();
  auto& logic_ctx = context.GetLogicContext(SceneType::SceneType_TITLE);
}

// Even better - only get what you need
TEST_CASE("Simple test", "[unit]") {
  steamrot::tests::TestContext context;
  auto& game_ctx = context.GetGameContext(); // No scene needed!
}
```

#### 4.2 Create TestContextFactory

**New File:** `tests/context/TestContextFactory.h`

```cpp
namespace steamrot::tests {

class TestContextFactory {
public:
  // Preset configurations for common test scenarios
  static TestContext CreateMinimal();
  static TestContext CreateWithUI(size_t ui_entity_count = 10);
  static TestContext CreateWithGrimoire(size_t grimoire_count = 5);
  static TestContext CreateForScene(SceneType scene_type);
  static TestContext CreateFromConfig(const std::string& test_config_path);
  
  // For data-driven tests
  static std::vector<TestContext> CreateAllSceneContexts();
};

} // namespace steamrot::tests
```

**Benefits:**
- Reduced test boilerplate
- Consistent test setup
- Easy to create specialized test contexts
- Supports data-driven testing

### Phase 5: Extensibility Improvements

#### 5.1 Context Extension System

**New File:** `src/context/ContextExtensions.h`

```cpp
namespace steamrot {

// Base class for context extensions
class IContextExtension {
public:
  virtual ~IContextExtension() = default;
  virtual const std::string& GetKey() const = 0;
  virtual std::expected<std::monostate, FailInfo> 
    LoadFromData(const ContextExtension* data) = 0;
};

// Manager for dynamic context extensions
class ContextExtensionManager {
private:
  std::unordered_map<std::string, std::unique_ptr<IContextExtension>> 
    m_extensions;
  
public:
  template<typename T>
  void RegisterExtension(const std::string& key, std::unique_ptr<T> extension);
  
  template<typename T>
  T* GetExtension(const std::string& key);
  
  std::expected<std::monostate, FailInfo>
  LoadExtensionsFromData(const ContextConfiguration* config);
};

// Example extension for debug info
class DebugContextExtension : public IContextExtension {
private:
  bool m_show_fps{false};
  bool m_show_entity_count{false};
  
public:
  const std::string& GetKey() const override { 
    static const std::string key = "debug";
    return key; 
  }
  
  std::expected<std::monostate, FailInfo> 
  LoadFromData(const ContextExtension* data) override;
  
  bool ShouldShowFPS() const { return m_show_fps; }
  bool ShouldShowEntityCount() const { return m_show_entity_count; }
};

} // namespace steamrot
```

**Benefits:**
- Add new context data without modifying core structs
- Plugin-style extensibility
- Can be loaded from data files
- Easy to add domain-specific context

#### 5.2 Modified Context Structs to Support Extensions

**Modified:** `src/context/GameContext.h`

```cpp
namespace steamrot {

struct GameContext {
  // Core required fields (unchanged)
  sf::RenderWindow &game_window;
  EventHandler &event_handler;
  sf::Vector2i mouse_position{0, 0};
  const size_t &loop_number;
  AssetManager &asset_manager;
  const EnvironmentType env_type;
  
  // NEW: Extension support
  std::shared_ptr<ContextExtensionManager> extensions;
  
  // Helper to get extensions safely
  template<typename T>
  T* GetExtension(const std::string& key) {
    if (extensions) {
      return extensions->GetExtension<T>(key);
    }
    return nullptr;
  }
};

} // namespace steamrot
```

## Implementation Roadmap

### Stage 1: Foundation (Week 1-2)
- [ ] Create ContextBuilder base class and implementations
- [ ] Create GameContextBuilder
- [ ] Create LogicContextBuilder
- [ ] Add unit tests for builders
- [ ] Update GameEngine to use GameContextBuilder
- [ ] Update Scene to use LogicContextBuilder

### Stage 2: Data-Driven Configuration (Week 3-4)
- [ ] Create context_config.fbs schema
- [ ] Implement ContextConfigurator
- [ ] Create default configuration JSON files
- [ ] Add configuration loading to GameEngine startup
- [ ] Add configuration loading tests
- [ ] Document configuration format

### Stage 3: Context Management (Week 5-6)
- [ ] Implement ContextManager singleton
- [ ] Integrate ContextManager into GameEngine
- [ ] Update SceneFactory to use ContextManager
- [ ] Add context caching and lazy initialization
- [ ] Add context lifecycle tests

### Stage 4: Test Infrastructure (Week 7-8)
- [ ] Refactor TestContext to use builders
- [ ] Create TestContextFactory
- [ ] Update existing tests to use new TestContext API
- [ ] Add test context configuration files
- [ ] Document new test patterns

### Stage 5: Extension System (Week 9-10)
- [ ] Implement IContextExtension interface
- [ ] Create ContextExtensionManager
- [ ] Add extension support to context structs
- [ ] Create example extensions (Debug, Metrics)
- [ ] Add extension loading from data
- [ ] Document extension creation guide

### Stage 6: Migration and Cleanup (Week 11-12)
- [ ] Migrate all scenes to new context system
- [ ] Migrate all logic classes to new context system
- [ ] Remove deprecated context creation methods
- [ ] Update all documentation
- [ ] Performance testing and optimization
- [ ] Final code review and cleanup

## Testing Strategy

### Unit Tests
```cpp
// Builder validation
TEST_CASE("GameContextBuilder validates required fields", "[unit][context]") {
  GameContextBuilder builder;
  auto result = builder.Build();
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == FailMode::MissingRequiredField);
}

// Builder incremental construction
TEST_CASE("GameContextBuilder builds successfully", "[unit][context]") {
  GameContextBuilder builder;
  sf::RenderWindow window;
  EventHandler handler;
  AssetManager assets;
  size_t loop_num = 0;
  
  auto result = builder
    .SetWindow(window)
    .SetEventHandler(handler)
    .SetAssetManager(assets)
    .SetLoopNumber(loop_num)
    .SetEnvironmentType(EnvironmentType::Test)
    .Build();
    
  REQUIRE(result.has_value());
}
```

### Integration Tests
```cpp
// Context configuration loading
TEST_CASE("ContextManager loads from config file", "[integration][context]") {
  ContextManager::GetInstance().ResetForTesting();
  
  auto init_result = ContextManager::GetInstance()
    .Initialize("test_configs/context_config.json");
  
  REQUIRE(init_result.has_value());
  
  auto game_ctx_result = ContextManager::GetInstance().GetGameContext();
  REQUIRE(game_ctx_result.has_value());
}
```

### Data-Driven Tests
```cpp
// Test all scene configurations
TEST_CASE("All scene contexts load correctly", "[integration][data-driven]") {
  auto configs = LoadAllSceneConfigs();
  
  for (const auto& config : configs) {
    auto context = TestContextFactory::CreateFromConfig(config);
    auto logic_ctx = context.GetLogicContext(config.scene_type);
    
    REQUIRE(logic_ctx.scene_entities.size() >= config.min_entity_count);
  }
}
```

## Benefits Summary

### For Developers
1. **Easier Testing**: Simplified test context creation with less boilerplate
2. **Better Separation of Concerns**: Context building separate from usage
3. **Clearer Dependencies**: Builder pattern makes dependencies explicit
4. **Extensibility**: Easy to add new context data without breaking existing code

### For Maintainability
1. **Data-Driven**: Configuration changes don't require recompilation
2. **Centralized Management**: Single source of truth for context state
3. **Validation**: Built-in validation before context creation
4. **Documentation**: Configuration files serve as documentation

### For Testing
1. **Isolation**: Easy to test components with minimal context
2. **Flexibility**: Create exactly the context needed for each test
3. **Reusability**: TestContextFactory provides common configurations
4. **Performance**: Lazy initialization reduces test setup overhead

### For Future Features
1. **Plugin System**: Extensions allow for domain-specific context data
2. **Serialization**: Context configuration can be saved/loaded
3. **Hot Reload**: Configuration changes can be applied at runtime
4. **Multi-Environment**: Easy to maintain different configs for dev/test/prod

## Migration Path

### Backward Compatibility
During migration, both old and new systems will coexist:

```cpp
// Old way still works
GameContext old_ctx(window, handler, loop_num, assets, env_type);

// New way is available
GameContextBuilder builder;
auto new_ctx = builder
  .SetWindow(window)
  .SetEventHandler(handler)
  .SetLoopNumber(loop_num)
  .SetAssetManager(assets)
  .SetEnvironmentType(env_type)
  .Build();
```

### Gradual Migration
1. Week 1-2: Add new builders alongside existing constructors
2. Week 3-4: Update GameEngine to use builders
3. Week 5-6: Update Scene/SceneFactory to use builders
4. Week 7-8: Update test infrastructure
5. Week 9-10: Migrate all Logic classes
6. Week 11-12: Remove old constructors, cleanup

## Risk Mitigation

### Performance Concerns
- **Risk**: Builder pattern adds overhead
- **Mitigation**: Contexts built once and cached; negligible runtime impact
- **Validation**: Performance benchmarks before/after

### Complexity Concerns
- **Risk**: More classes/files to maintain
- **Mitigation**: Clear documentation, examples, and migration guide
- **Validation**: Code review focus on simplicity

### Breaking Changes
- **Risk**: Existing code stops working
- **Mitigation**: Gradual migration with backward compatibility period
- **Validation**: Comprehensive test suite validates all changes

## Conclusion

This improvement plan addresses the current limitations of the context handling system by introducing:

1. **Builder Pattern** for flexible, validated context construction
2. **Data-Driven Configuration** to externalize settings
3. **Centralized Management** for better lifecycle control
4. **Lazy Initialization** to improve performance
5. **Extension System** for future-proof extensibility
6. **Improved Test Infrastructure** for better developer experience

The phased approach allows for gradual migration while maintaining backward compatibility, reducing risk while delivering continuous value.

## Appendix A: File Structure After Implementation

```
src/
├── context/
│   ├── GameContext.h/cpp           [Modified: Add extensions]
│   ├── GameContextBuilder.h/cpp    [New]
│   ├── ContextBuilder.h            [New: Template base]
│   ├── ContextConfigurator.h/cpp   [New]
│   ├── ContextManager.h/cpp        [New]
│   ├── ContextExtensions.h/cpp     [New]
│   └── CMakeLists.txt              [Modified]
├── logic/
│   ├── LogicContext.h              [Modified: Add extensions]
│   ├── LogicContextBuilder.h/cpp   [New]
│   └── CMakeLists.txt              [Modified]
├── flatbuffers_headers/
│   └── context_config.fbs          [New]
└── ...

data/
├── context/
│   ├── game_context.json           [New]
│   └── scene_contexts.json         [New]
└── ...

tests/
├── context/
│   ├── TestContext.h/cpp           [Modified: Use builders]
│   ├── TestContextFactory.h/cpp    [New]
│   └── CMakeLists.txt              [Modified]
├── data/
│   └── test_context_configs/       [New]
│       ├── minimal.json
│       ├── ui_heavy.json
│       └── grimoire.json
└── ...
```

## Appendix B: Configuration File Examples

### Game Context Configuration
**File:** `data/context/game_context.json`
```json
{
  "window_width": 800,
  "window_height": 600,
  "window_title": "SteamRot - Development",
  "framerate_limit": 60,
  "environment_type": "Development",
  "extensions": [
    {
      "key": "debug",
      "value_type": "json",
      "value_data": {
        "show_fps": true,
        "show_entity_count": true,
        "show_memory_usage": false
      }
    }
  ]
}
```

### Scene Context Configuration
**File:** `data/context/scene_contexts.json`
```json
{
  "scenes": [
    {
      "scene_type": "TITLE",
      "entity_pool_size": 50,
      "render_texture_width": 800,
      "render_texture_height": 600,
      "extensions": []
    },
    {
      "scene_type": "CRAFTING",
      "entity_pool_size": 200,
      "render_texture_width": 800,
      "render_texture_height": 600,
      "extensions": [
        {
          "key": "crafting_config",
          "value_type": "json",
          "value_data": {
            "max_ingredients": 10,
            "recipe_slots": 4
          }
        }
      ]
    }
  ]
}
```

### Test Context Configuration
**File:** `tests/data/test_context_configs/ui_heavy.json`
```json
{
  "scene_type": "TEST",
  "entity_pool_size": 100,
  "render_texture_width": 800,
  "render_texture_height": 600,
  "preload_entities": {
    "ui_entities": 50,
    "grimoire_entities": 0
  }
}
```

## Appendix C: Example Usage Patterns

### Creating Game Context in GameEngine
```cpp
// In GameEngine::StartUp()
ContextConfigurator configurator;
auto load_result = configurator.LoadFromFile("data/context/game_context.json");
if (!load_result.has_value()) {
  // Handle error
}

auto builder_result = configurator.CreateGameContextBuilder();
if (!builder_result.has_value()) {
  // Handle error
}

auto builder = builder_result.value();
builder.SetWindow(m_window)
       .SetEventHandler(m_event_handler)
       .SetAssetManager(m_asset_manager)
       .SetLoopNumber(m_loop_number);

auto context_result = builder.Build();
if (!context_result.has_value()) {
  // Handle error
}

m_game_context = context_result.value();
```

### Creating Scene-Specific Logic Context
```cpp
// In SceneFactory::CreateDefaultScene()
auto& context_mgr = ContextManager::GetInstance();

// Register scene-specific builder
LogicContextBuilder builder;
builder.SetSceneEntities(scene_ptr->m_entity_manager.GetEntityMemoryPool())
       .SetArchetypes(scene_ptr->m_entity_manager.GetArchetypeManager())
       .SetSceneTexture(scene_ptr->m_render_texture)
       .SetGameContext(game_context);

context_mgr.RegisterSceneContextBuilder(scene_type, builder);

// Get LogicContext when needed
auto logic_ctx_result = context_mgr.GetLogicContext(scene_type);
if (logic_ctx_result.has_value()) {
  LogicFactory logic_factory(scene_type, logic_ctx_result.value());
  // ... create logic
}
```

### Using Context Extensions
```cpp
// In a Logic class
void MyLogic::ProcessLogic() {
  // Try to get debug extension
  if (auto* debug_ext = m_logic_context.GetExtension<DebugContextExtension>("debug")) {
    if (debug_ext->ShouldShowFPS()) {
      // Draw FPS counter
    }
  }
  
  // Normal logic continues...
}
```

### Test Context Usage
```cpp
// Minimal test - just need GameContext
TEST_CASE("Test EventHandler", "[unit]") {
  auto context = TestContextFactory::CreateMinimal();
  auto& game_ctx = context.GetGameContext();
  
  REQUIRE(game_ctx.event_handler.GetGlobalEventBus().empty());
}

// UI-heavy test
TEST_CASE("Test UI Rendering", "[unit]") {
  auto context = TestContextFactory::CreateWithUI(50);
  auto& logic_ctx = context.GetLogicContext(SceneType::SceneType_TEST);
  
  UIRenderLogic render_logic(logic_ctx);
  render_logic.RunLogic();
  
  // Verify rendering...
}

// Custom configuration test
TEST_CASE("Test Custom Scenario", "[integration]") {
  auto context = TestContextFactory::CreateFromConfig(
    "tests/data/test_context_configs/custom_scenario.json"
  );
  
  // Test with custom configuration...
}
```
