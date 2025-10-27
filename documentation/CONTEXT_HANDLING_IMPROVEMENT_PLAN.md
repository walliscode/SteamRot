# Context Handling System Improvement Plan

## Executive Summary

This document presents a comprehensive analysis of the current context handling
system in SteamRot and proposes improvements to make it more extensible,
data-driven, and maintainable. The analysis covers both the main game context
handling and test infrastructure.

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

**Instantiation:** Created once in `GameEngine` constructor and passed by const
reference throughout the system.

**Issues:**

1. **Tight Coupling**: All components are required at construction time
2. **Non-extensible**: Adding new context data requires changing the struct and
   all instantiation sites
3. **Not Data-Driven**: Configuration is hardcoded in C++
4. **Limited Construction Flexibility**: References must be initialized in
   constructor, limiting validation and error handling
5. **Single Point of Failure**: All context must be valid at GameEngine creation

**Note on Reference-Based Design:** The current system uses references (not
pointers) which provides important safety guarantees:

- **Compile-time lifetime safety**: References must be valid when created
- **No null state**: References cannot be null, preventing a class of runtime
  errors
- **Clear ownership**: References signal that the context doesn't own the
  resources
- **Strict ordering**: Forces proper initialization order, making bugs more
  visible

The proposed builder pattern preserves these benefits using
`std::optional<std::reference_wrapper<T>>` instead of pointers.

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

**Instantiation:** Created in `Scene::GetLogicContext()` method, called during
scene creation in `SceneFactory::CreateDefaultScene()`.

**Issues:**

1. **Duplication**: Overlaps with GameContext (game_window, asset_manager,
   event_handler, mouse_position)
2. **Tight Coupling**: Requires both Scene and GameContext to be fully
   initialized
3. **Creation Timing**: Created during scene initialization, limiting when logic
   can be created
4. **Not Reusable**: New LogicContext created each time GetLogicContext() is
   called
5. **No Lazy Initialization**: All scene resources must exist before any logic
   can be created

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

1. **Scene-Specific Logic Contexts**: Separate unique_ptr for each scene type is
   not scalable
2. **Eager Initialization**: Creates all objects in constructor based on scene
   type
3. **Hardcoded Scenes**: Adding new scenes requires modifying TestContext
4. **Duplicated Logic**: Scene configuration logic duplicated 3 times
5. **Heavy Construction**: Every test pays the cost of full initialization even
   if not needed

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

### Key Design Decisions

Based on review feedback, the following architectural decisions guide the
implementation:

1. **Smart Pointers in Builders, References in Contexts**
   - Builders use `std::shared_ptr<T>` internally for flexibility
   - Final contexts use references (matching existing design philosophy)
   - Avoids undefined behavior while maintaining strict lifetime guarantees

2. **LogicContext Pulls Individual Fields (Not GameContext)**
   - LogicContext takes individual resources (window, asset_manager, etc.)
   - Does not store a reference to GameContext itself
   - More explicit dependencies and better testability

3. **No Singleton Pattern - Static Registry Instead**
   - ContextDirector uses static map and methods (not singleton)
   - Simpler, more testable, not time-critical
   - Easy to clear for testing

4. **Non-Templated, Concrete Builders**
   - GameContextBuilder and LogicContextBuilder are concrete classes
   - No template base class needed
   - Builders don't store the context - just build and return it

5. **FlatBuffers Integration**
   - Root tables: `ContextData` for main game, `TestContextData` for tests
   - Use existing `FlatbuffersDataLoader` with new methods
   - Consistent with existing data loading patterns

6. **Test Infrastructure - Director Pattern**
   - TestContextDirector with static registry (like ContextDirector)
   - TestResources struct stores permanent objects
   - TestContext is a simple wrapper for convenience
   - Data-driven via TestContextData

7. **No Extensibility System**
   - Keeping it simple - no extension framework needed at this stage
   - Can add specific features directly to contexts as needed

### Phase 1: Introduce Context Builder Pattern

**Design Philosophy: Smart Pointers in Builders, References in Contexts**

The builder pattern uses smart pointers internally for flexibility during
construction, but produces contexts with references:

- **Builder Internal State**: Uses smart pointers (`std::shared_ptr<T>`) to
  store objects during construction
  - Allows builders to own temporary objects if needed
  - Explicit lifetime management during building phase
  - No undefined behavior from uninitialized references

- **Final Context**: Built contexts use references (matching current design)
  - Builders pass through references from their smart pointers when calling
    `Build()`
  - No risk of dangling pointers in production code
  - Clear ownership semantics
  - Compiler-enforced lifetime guarantees

- **Simplified Design**: Builders don't need to be templated or store the final
  Context
  - Each builder is a concrete class for its specific context type
  - Build() returns the context directly, not a stored copy
  - Easier to understand and maintain

This approach provides builder pattern flexibility while preserving the safety
and clarity of reference-based design for the final contexts.

#### 1.1 Create GameContextBuilder

**New File:** `src/context/GameContextBuilder.h`

```cpp
namespace steamrot {

class GameContextBuilder {
private:
  std::shared_ptr<sf::RenderWindow> m_window;
  std::shared_ptr<EventHandler> m_event_handler;
  std::shared_ptr<AssetManager> m_asset_manager;
  std::shared_ptr<const size_t> m_loop_number;
  EnvironmentType m_env_type{EnvironmentType::None};

public:
  GameContextBuilder() = default;

  GameContextBuilder& SetWindow(std::shared_ptr<sf::RenderWindow> window);
  GameContextBuilder& SetEventHandler(std::shared_ptr<EventHandler> handler);
  GameContextBuilder& SetAssetManager(std::shared_ptr<AssetManager> manager);
  GameContextBuilder& SetLoopNumber(std::shared_ptr<const size_t> loop_num);
  GameContextBuilder& SetEnvironmentType(EnvironmentType env_type);

  // Build returns GameContext with references extracted from smart pointers
  std::expected<GameContext, FailInfo> Build() const;
};

} // namespace steamrot
```

**Benefits:**

- Smart pointers in builder allow flexible ownership during construction
- Build() extracts references from smart pointers for the final context
- No templating needed - concrete class for GameContext
- Clear validation before building

#### 1.2 Create LogicContextBuilder

**New File:** `src/logic/LogicContextBuilder.h`

```cpp
namespace steamrot {

class LogicContextBuilder {
private:
  std::shared_ptr<EntityMemoryPool> m_scene_entities;
  std::shared_ptr<const std::unordered_map<ArchetypeID, Archetype>> m_archetypes;
  std::shared_ptr<sf::RenderTexture> m_scene_texture;
  std::shared_ptr<sf::RenderWindow> m_game_window;
  std::shared_ptr<const AssetManager> m_asset_manager;
  std::shared_ptr<EventHandler> m_event_handler;
  std::shared_ptr<const sf::Vector2i> m_mouse_position;

public:
  LogicContextBuilder() = default;

  LogicContextBuilder& SetSceneEntities(std::shared_ptr<EntityMemoryPool> entities);
  LogicContextBuilder& SetArchetypes(std::shared_ptr<const std::unordered_map<ArchetypeID, Archetype>> archetypes);
  LogicContextBuilder& SetSceneTexture(std::shared_ptr<sf::RenderTexture> texture);
  LogicContextBuilder& SetGameWindow(std::shared_ptr<sf::RenderWindow> window);
  LogicContextBuilder& SetAssetManager(std::shared_ptr<const AssetManager> manager);
  LogicContextBuilder& SetEventHandler(std::shared_ptr<EventHandler> handler);
  LogicContextBuilder& SetMousePosition(std::shared_ptr<const sf::Vector2i> mouse_pos);

  // Build returns LogicContext with references extracted from smart pointers
  std::expected<LogicContext, FailInfo> Build() const;
};

} // namespace steamrot
```

**Design Note: LogicContext and GameContext**

LogicContext pulls individual fields from GameContext rather than storing a
reference to GameContext itself:

- **Explicit dependencies**: Each Logic class sees exactly what it needs
- **Better testability**: Can mock individual resources without full GameContext
- **Clearer API**: SetGameWindow(), SetAssetManager() etc. are more explicit
  than SetGameContext()
- **Flexibility**: Can mix scene-specific and game-global resources freely

**Benefits:**

- Smart pointers in builder allow flexible ownership during construction
- Build() extracts references for the final context
- No dependency on GameContext object - pulls what's needed
- Explicit about which resources come from game vs scene

### Phase 2: Data-Driven Context Configuration

#### 2.1 Create Context Configuration Schema

**New File:** `src/flatbuffers_headers/context_data.fbs`

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
}

// Root table for context configuration
table ContextData {
  game_context: GameContextConfig;
  scene_contexts: [SceneContextConfig];
}

root_type ContextData;
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

#### 2.2 Context Data Loading with FlatbuffersDataLoader

**Modified:** `src/data_handlers/FlatbuffersDataLoader.h`

Add method to existing class:

```cpp
/////////////////////////////////////////////////
/// @brief Provides ContextData from binary file
/////////////////////////////////////////////////
std::expected<const ContextData *, FailInfo> ProvideContextData() const;
```

**New File:** `src/context/ContextConfigurator.h`

```cpp
namespace steamrot {

class ContextConfigurator {
private:
  const ContextData* m_config_data{nullptr};

  std::expected<EnvironmentType, FailInfo>
  ParseEnvironmentType(const std::string& type_str) const;

public:
  explicit ContextConfigurator(const ContextData* config);

  // Create builder from configuration data
  std::expected<GameContextBuilder, FailInfo>
  CreateGameContextBuilder() const;

  std::expected<LogicContextBuilder, FailInfo>
  CreateLogicContextBuilder(const SceneType& scene_type) const;
};

} // namespace steamrot
```

**Usage Pattern:**

```cpp
// In GameEngine::StartUp() or similar
FlatbuffersDataLoader data_loader;
auto context_data_result = data_loader.ProvideContextData();
if (!context_data_result.has_value()) {
  // Handle error
}

ContextConfigurator configurator(context_data_result.value());
auto builder_result = configurator.CreateGameContextBuilder();
// ... use builder
```

**Benefits:**

- Game configuration externalized to JSON
- No code changes needed for common adjustments
- Easy to have different configs for different environments
- Version control friendly

### Phase 3: Context Management with Static Registry

#### 3.1 Create ContextDirector (Static Registry Pattern)

**New File:** `src/context/ContextDirector.h`

```cpp
namespace steamrot {

class ContextDirector {
private:
  // Static map to register builders by scene type
  static std::unordered_map<SceneType, LogicContextBuilder> s_logic_context_builders;

  // No singleton - just static methods and data
  ContextDirector() = delete;

public:
  // Register a builder for a specific scene type
  static void RegisterLogicContextBuilder(SceneType type, LogicContextBuilder builder);

  // Get builder for a scene type (returns copy for further configuration)
  static std::expected<LogicContextBuilder, FailInfo>
  GetLogicContextBuilder(SceneType scene_type);

  // Build and return a LogicContext directly
  static std::expected<LogicContext, FailInfo>
  BuildLogicContext(SceneType scene_type);

  // Clear all registered builders (useful for testing)
  static void ClearBuilders();

  // Check if a builder is registered for a scene type
  static bool HasBuilder(SceneType scene_type);
};

} // namespace steamrot
```

**Benefits:**

- **No singleton pattern** - uses static methods and data instead
- **Not time-critical** - builders can be registered during initialization
- **Simple registry** - static map stores builders by scene type
- **Testable** - ClearBuilders() allows clean test setup
- **Explicit control** - callers decide when to build contexts

### Phase 4: Improve Test Infrastructure

#### 4.1 Data-Driven Test Context Configuration

**New File:** `src/flatbuffers_headers/test_context_data.fbs`

```fbs
namespace steamrot;

// Configuration for test contexts
table TestContextConfig {
  scene_type: string;
  entity_pool_size: uint32 = 100;
  ui_entity_count: uint32 = 0;
  grimoire_entity_count: uint32 = 0;
}

// Root table for test context configuration
table TestContextData {
  configs: [TestContextConfig];
}

root_type TestContextData;
```

**Modified:** `src/data_handlers/FlatbuffersDataLoader.h`

Add method to existing class:

```cpp
/////////////////////////////////////////////////
/// @brief Provides TestContextData from binary file
/////////////////////////////////////////////////
std::expected<const TestContextData *, FailInfo> ProvideTestContextData() const;
```

#### 4.2 TestContextDirector - Static Registry for Test Contexts

**New File:** `tests/context/TestContextDirector.h`

```cpp
namespace steamrot::tests {

// Stores the permanent test objects that contexts reference
struct TestResources {
  sf::RenderWindow render_window;
  EventHandler event_handler;
  AssetManager asset_manager;
  size_t loop_number{0};
  EnvironmentType env_type{EnvironmentType::Test};

  TestResources();
};

class TestContextDirector {
private:
  // Static storage for permanent test resources
  static std::unique_ptr<TestResources> s_resources;

  // Static map to cache built contexts by scene type
  static std::unordered_map<SceneType, GameContext> s_game_contexts;
  static std::unordered_map<SceneType, LogicContext> s_logic_contexts;

  TestContextDirector() = delete;

public:
  // Initialize resources (call once at test start)
  static void Initialize();

  // Get or build GameContext
  static std::expected<const GameContext&, FailInfo> GetGameContext();

  // Get or build LogicContext for a scene type
  static std::expected<const LogicContext&, FailInfo>
  GetLogicContext(SceneType scene_type);

  // Clear all cached contexts (for test isolation)
  static void ClearContexts();

  // Reset everything (for test cleanup)
  static void Reset();
};

} // namespace steamrot::tests
```

**Design Rationale:**

- **TestContextDirector**: Static registry pattern (like ContextDirector)
- **TestResources**: Permanent storage for objects that contexts reference
- **No Factory needed**: Director handles both storage and context creation
- **Data-driven**: Can load configurations from TestContextData

#### 4.3 Simplified TestContext as Convenience Wrapper

**Modified:** `tests/context/TestContext.h`

```cpp
namespace steamrot::tests {

// Simple convenience wrapper around TestContextDirector
class TestContext {
private:
  SceneType m_scene_type{SceneType::SceneType_TEST};

public:
  TestContext(SceneType scene_type = SceneType::SceneType_TEST);

  // Get contexts via director
  const GameContext& GetGameContext() const;
  const LogicContext& GetLogicContext() const;

  // Configure scene type
  TestContext& WithSceneType(SceneType scene_type);
};

} // namespace steamrot::tests
```

**Usage Example:**

```cpp
// Simple usage
TEST_CASE("Test with default context", "[unit]") {
  steamrot::tests::TestContext context;
  auto& game_ctx = context.GetGameContext();
  // Test using game_ctx
}

// With specific scene
TEST_CASE("Test title scene", "[unit]") {
  steamrot::tests::TestContext context(SceneType::SceneType_TITLE);
  auto& logic_ctx = context.GetLogicContext();
  // Test using logic_ctx
}

// Using director directly (for advanced cases)
TEST_CASE("Direct director usage", "[unit]") {
  TestContextDirector::Initialize();
  auto& game_ctx = TestContextDirector::GetGameContext().value();
  auto& logic_ctx = TestContextDirector::GetLogicContext(SceneType::SceneType_TITLE).value();
  // Test using contexts
  TestContextDirector::ClearContexts();
}
```

**Benefits:**

- TestResources stores permanent objects
- TestContextDirector manages context lifecycle
- TestContext provides simple wrapper for common cases
- Data-driven via TestContextData FlatBuffers
- Static pattern (no singleton)

## Implementation Roadmap

### Stage 1: Foundation (Week 1-2)

- [ ] Create GameContextBuilder
- [ ] Create LogicContextBuilder
- [ ] Add unit tests for builders
- [ ] Update GameEngine to use GameContextBuilder
- [ ] Update Scene to use LogicContextBuilder

### Stage 2: Data-Driven Configuration (Week 3-4)

- [ ] Create context_data.fbs schema
- [ ] Add ProvideContextData() to FlatbuffersDataLoader
- [ ] Implement ContextConfigurator
- [ ] Create default configuration JSON files
- [ ] Add configuration loading to GameEngine startup
- [ ] Add configuration loading tests
- [ ] Document configuration format

### Stage 3: Context Management (Week 5-6)

- [ ] Implement ContextDirector with static registry
- [ ] Integrate ContextDirector into GameEngine
- [ ] Update SceneFactory to use ContextDirector
- [ ] Add context lifecycle tests

### Stage 4: Test Infrastructure (Week 7-8)

- [ ] Create test_context_data.fbs schema
- [ ] Add ProvideTestContextData() to FlatbuffersDataLoader
- [ ] Implement TestResources struct
- [ ] Implement TestContextDirector with static registry
- [ ] Create TestContext wrapper class
- [ ] Update existing tests to use new TestContext API
- [ ] Add test context configuration files
- [ ] Document new test patterns

### Stage 5: Migration and Cleanup (Week 9-10)

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
TEST_CASE("ContextDirector registers and builds contexts", "[integration][context]") {
  ContextDirector::ClearBuilders();

  FlatbuffersDataLoader data_loader;
  auto context_data_result = data_loader.ProvideContextData();
  REQUIRE(context_data_result.has_value());

  ContextConfigurator configurator(context_data_result.value());
  auto builder_result = configurator.CreateLogicContextBuilder(SceneType::SceneType_TITLE);
  REQUIRE(builder_result.has_value());

  ContextDirector::RegisterLogicContextBuilder(SceneType::SceneType_TITLE, builder_result.value());
  REQUIRE(ContextDirector::HasBuilder(SceneType::SceneType_TITLE));

  auto logic_ctx_result = ContextDirector::BuildLogicContext(SceneType::SceneType_TITLE);
  REQUIRE(logic_ctx_result.has_value());
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

1. **Easier Testing**: Simplified test context creation with TestContextDirector
2. **Better Separation of Concerns**: Context building separate from usage
3. **Clearer Dependencies**: Builder pattern makes dependencies explicit
4. **Simple Design**: No complex extension system - just core functionality

### For Maintainability

1. **Data-Driven**: Configuration changes don't require recompilation
2. **Static Registry**: ContextDirector and TestContextDirector use simple
   static maps
3. **Validation**: Built-in validation before context creation
4. **Documentation**: Configuration files serve as documentation

### For Testing

1. **Isolation**: Easy to test components with minimal context
2. **Flexibility**: TestContextDirector manages all test resources
3. **Data-Driven**: TestContextData allows configurable test scenarios
4. **Simple API**: TestContext wrapper provides convenient interface

### For Future Features

1. **Serialization**: Context configuration can be saved/loaded
2. **Multi-Environment**: Easy to maintain different configs for dev/test/prod
3. **Incremental Enhancement**: Can add features to contexts directly as needed

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
2. Week 3-4: Update GameEngine to use builders and ContextDirector
3. Week 5-6: Update Scene/SceneFactory to use builders
4. Week 7-8: Update test infrastructure with TestContextDirector
5. Week 9-10: Migrate all scenes and logic classes, remove old constructors,
   cleanup

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

This improvement plan addresses the current limitations of the context handling
system by introducing:

1. **Builder Pattern** for flexible, validated context construction
2. **Data-Driven Configuration** to externalize settings via FlatBuffers
3. **Static Registry Pattern** (ContextDirector, TestContextDirector) for simple
   lifecycle control
4. **Improved Test Infrastructure** with TestResources and data-driven
   configuration
5. **Simplified Design** - no complex extensibility framework, just core
   functionality

The phased approach allows for gradual migration (10 weeks) while maintaining
backward compatibility, reducing risk while delivering continuous value.

## Appendix A: File Structure After Implementation

```
src/
├── context/
│   ├── GameContext.h/cpp           [Unchanged]
│   ├── GameContextBuilder.h/cpp    [New]
│   ├── ContextConfigurator.h/cpp   [New]
│   ├── ContextDirector.h/cpp       [New]
│   └── CMakeLists.txt              [Modified]
├── logic/
│   ├── LogicContext.h              [Modified: Pull fields from GameContext individually]
│   ├── LogicContextBuilder.h/cpp   [New]
│   └── CMakeLists.txt              [Modified]
├── data_handlers/
│   └── FlatbuffersDataLoader.h/cpp [Modified: Add ProvideContextData(), ProvideTestContextData()]
├── flatbuffers_headers/
│   ├── context_data.fbs            [New]
│   └── test_context_data.fbs       [New]
└── ...

data/
├── context/
│   └── context_data.bin            [New: Compiled FlatBuffers]
└── ...

tests/
├── context/
│   ├── TestContext.h/cpp           [Modified: Simplified wrapper]
│   ├── TestContextDirector.h/cpp   [New]
│   └── CMakeLists.txt              [Modified]
├── data/
│   └── test_context_data.bin       [New: Compiled FlatBuffers]
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
FlatbuffersDataLoader data_loader;
auto context_data_result = data_loader.ProvideContextData();
if (!context_data_result.has_value()) {
  // Handle error
}

ContextConfigurator configurator(context_data_result.value());
auto builder_result = configurator.CreateGameContextBuilder();
if (!builder_result.has_value()) {
  // Handle error
}

// Configure builder with actual objects from GameEngine
auto builder = builder_result.value();
builder.SetWindow(std::make_shared<sf::RenderWindow>(m_window))
       .SetEventHandler(std::make_shared<EventHandler>(m_event_handler))
       .SetAssetManager(std::make_shared<AssetManager>(m_asset_manager))
       .SetLoopNumber(std::make_shared<const size_t>(m_loop_number));

auto context_result = builder.Build();
if (!context_result.has_value()) {
  // Handle error
}

m_game_context = context_result.value();
```

### Creating Scene-Specific Logic Context

```cpp
// In SceneFactory::CreateDefaultScene()

// Create builder and configure with scene resources
LogicContextBuilder builder;
builder.SetSceneEntities(std::make_shared<EntityMemoryPool>(scene_ptr->m_entity_manager.GetEntityMemoryPool()))
       .SetArchetypes(std::make_shared<const std::unordered_map<ArchetypeID, Archetype>>(
           scene_ptr->m_entity_manager.GetArchetypeManager().GetArchetypes()))
       .SetSceneTexture(std::make_shared<sf::RenderTexture>(scene_ptr->m_render_texture))
       .SetGameWindow(std::make_shared<sf::RenderWindow>(game_context.game_window))
       .SetAssetManager(std::make_shared<const AssetManager>(game_context.asset_manager))
       .SetEventHandler(std::make_shared<EventHandler>(game_context.event_handler))
       .SetMousePosition(std::make_shared<const sf::Vector2i>(game_context.mouse_position));

// Register builder with ContextDirector for later use
ContextDirector::RegisterLogicContextBuilder(scene_type, builder);

// Or build immediately
auto logic_ctx_result = builder.Build();
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
