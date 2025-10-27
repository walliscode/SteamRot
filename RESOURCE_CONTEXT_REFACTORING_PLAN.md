# Resource and Context Refactoring Plan

## Overview

This document outlines the plan to refactor SteamRot's context and resource handling system to improve separation of concerns, eliminate builder pattern complexity, and enable data-driven resource configuration.

## Problem Statement Requirements

Based on the original requirements:

1. Create Resource structs for objects like RenderWindows, mouse positions, RenderTexture
2. Resources should be one-off assets at different levels (GameEngine, Scene, etc.)
3. Create Context objects as lightweight reference containers to be passed around
4. Rename LogicContext to SceneContext and move it appropriately
5. Resource structs should NOT contain references/pointers to other Resource members (no base Resource class needed)
6. Resources should be data-driven from FlatBuffers where applicable
7. Create a ResourceConfigurator to handle configuration for all Resource types
8. Create a TestFixture to generate Resource structs and pass Context objects for tests
9. Move away from builders - accept heavier test cost

## Proposed Architecture

### Two-Layer Architecture

**Layer 1: Resources (Ownership)**
- Own the actual resource objects
- No cross-references between resources
- Data-driven configuration from FlatBuffers
- Live at appropriate scope levels (game, scene)

**Layer 2: Contexts (References)**
- Lightweight reference containers
- Passed around the codebase
- Cheap to copy (just references)
- Provide convenient access to resources

### Key Design Principles

1. **Clear Ownership**: Resources own objects, contexts reference them
2. **No Circular Dependencies**: Resources are standalone, contexts reference resources
3. **Data-Driven**: Use FlatBuffers for configuration where applicable
4. **No Builders**: Direct construction from resources
5. **Type Safety**: Compile-time checking of reference validity

## Detailed Design

### 1. GameResources Struct

**Purpose**: Own game-level resources

**Location**: `src/resources/GameResources.h`

**Structure**:
```cpp
namespace steamrot {

struct GameResources {
  // Game window
  sf::RenderWindow game_window;
  
  // Global event handler
  EventHandler event_handler;
  
  // Current mouse position (updated each frame)
  sf::Vector2i mouse_position{0, 0};
  
  // Loop number
  size_t loop_number{0};
  
  // Asset manager
  AssetManager asset_manager;
  
  // Environment type
  EnvironmentType env_type{EnvironmentType::None};
};

} // namespace steamrot
```

**Key Points**:
- All members are concrete objects (no references/pointers)
- Default-constructible
- Lives in GameEngine

### 2. SceneResources Struct

**Purpose**: Own scene-level resources

**Location**: `src/resources/SceneResources.h`

**Structure**:
```cpp
namespace steamrot {

struct SceneResources {
  // Entity memory pool
  EntityMemoryPool scene_entities;
  
  // Archetype manager
  ArchetypeManager archetype_manager;
  
  // Render texture
  sf::RenderTexture scene_texture;
  
  // Constructor to properly initialize archetype_manager
  SceneResources() : archetype_manager(scene_entities) {}
};

} // namespace steamrot
```

**Key Points**:
- All members are concrete objects
- archetype_manager needs scene_entities reference (handled in constructor)
- Lives in Scene instances

### 3. GameContext Struct

**Purpose**: Lightweight reference container for game-level resources

**Location**: `src/context/GameContext.h` (update existing)

**Structure**:
```cpp
namespace steamrot {

struct GameContext {
  // Constructor takes reference to GameResources
  explicit GameContext(GameResources &resources);
  
  // References to game resources
  sf::RenderWindow &game_window;
  EventHandler &event_handler;
  sf::Vector2i &mouse_position;
  size_t &loop_number;
  AssetManager &asset_manager;
  EnvironmentType &env_type;
};

} // namespace steamrot
```

**Implementation**:
```cpp
GameContext::GameContext(GameResources &resources)
    : game_window(resources.game_window),
      event_handler(resources.event_handler),
      mouse_position(resources.mouse_position),
      loop_number(resources.loop_number),
      asset_manager(resources.asset_manager),
      env_type(resources.env_type) {}
```

**Changes from Current**:
- Remove individual parameter constructor
- Take single GameResources reference
- Simplify initialization

### 4. SceneContext Struct

**Purpose**: Lightweight reference container for scene and game resources

**Location**: `src/scenes/SceneContext.h` (new file, replaces LogicContext)

**Structure**:
```cpp
namespace steamrot {

struct SceneContext {
  // Constructor takes both resource containers
  SceneContext(SceneResources &scene_res, GameResources &game_res);
  
  // Scene resource references
  EntityMemoryPool &scene_entities;
  const std::unordered_map<ArchetypeID, Archetype> &archetypes;
  sf::RenderTexture &scene_texture;
  
  // Game resource references
  sf::RenderWindow &game_window;
  AssetManager &asset_manager;
  EventHandler &event_handler;
  sf::Vector2i &mouse_position;
};

} // namespace steamrot
```

**Implementation**:
```cpp
SceneContext::SceneContext(SceneResources &scene_res, GameResources &game_res)
    : scene_entities(scene_res.scene_entities),
      archetypes(scene_res.archetype_manager.GetArchetypes()),
      scene_texture(scene_res.scene_texture),
      game_window(game_res.game_window),
      asset_manager(game_res.asset_manager),
      event_handler(game_res.event_handler),
      mouse_position(game_res.mouse_position) {}
```

**Migration from LogicContext**:
- Rename LogicContext -> SceneContext
- Move from `src/logic/` to `src/scenes/`
- Change constructor to take resource references

### 5. ResourceConfigurator Class

**Purpose**: Configure resources from FlatBuffers data

**Location**: `src/resources/ResourceConfigurator.h/cpp`

**Interface**:
```cpp
namespace steamrot {

class ResourceConfigurator {
private:
  const ContextData *m_config_data{nullptr};
  
  std::expected<EnvironmentType, FailInfo>
  ParseEnvironmentType(const std::string &type_str) const;

public:
  explicit ResourceConfigurator(const ContextData *config);
  
  // Configure GameResources from FlatBuffers
  std::expected<std::monostate, FailInfo>
  ConfigureGameResources(GameResources &resources) const;
  
  // Configure SceneResources from FlatBuffers
  std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(SceneResources &resources,
                          const SceneType &scene_type) const;
};

} // namespace steamrot
```

**Responsibilities**:
- Parse environment types from strings
- Configure window settings (size, title, framerate)
- Configure entity pool sizes
- Configure render texture dimensions
- Use existing context_data.fbs schema

### 6. TestFixture Class

**Purpose**: Test infrastructure for generating resources and contexts

**Location**: `tests/context/TestFixture.h/cpp`

**Interface**:
```cpp
namespace steamrot::tests {

class TestFixture {
private:
  GameResources m_game_resources;
  SceneResources m_scene_resources;
  std::unique_ptr<GameContext> m_game_context;
  std::unique_ptr<SceneContext> m_scene_context;
  
  void ConfigureGameResources();
  void ConfigureSceneResources(const SceneType &scene_type);

public:
  explicit TestFixture(const SceneType &scene_type = SceneType::SceneType_TEST);
  
  GameResources &GetGameResources();
  SceneResources &GetSceneResources();
  GameContext &GetGameContext();
  SceneContext &GetSceneContext();
};

} // namespace steamrot::tests
```

**Usage**:
```cpp
// In tests
TestFixture fixture(SceneType::SceneType_TEST);
SceneContext &ctx = fixture.GetSceneContext();
// Use ctx for testing
```

**Advantages**:
- Owns resources directly (no builders)
- Provides contexts on demand
- Simple, straightforward API
- Can access resources if needed

## Implementation Steps

### Phase 1: Create New Files

**Step 1.1**: Create GameResources
- [ ] Create `src/resources/GameResources.h`
- [ ] Define struct with all game-level resources
- [ ] Create `src/resources/CMakeLists.txt`
- [ ] Update `src/CMakeLists.txt` to add resources subdirectory

**Step 1.2**: Create SceneResources
- [ ] Create `src/resources/SceneResources.h`
- [ ] Define struct with all scene-level resources
- [ ] Add proper constructor for archetype_manager initialization

**Step 1.3**: Create ResourceConfigurator
- [ ] Create `src/resources/ResourceConfigurator.h`
- [ ] Create `src/resources/ResourceConfigurator.cpp`
- [ ] Implement ConfigureGameResources()
- [ ] Implement ConfigureSceneResources()
- [ ] Reuse existing context_data.fbs schema

**Step 1.4**: Create SceneContext
- [ ] Create `src/scenes/SceneContext.h`
- [ ] Create `src/scenes/SceneContext.cpp`
- [ ] Implement constructor taking both resource references
- [ ] Update `src/scenes/CMakeLists.txt`

**Step 1.5**: Create TestFixture
- [ ] Create `tests/context/TestFixture.h`
- [ ] Create `tests/context/TestFixture.cpp`
- [ ] Implement resource ownership
- [ ] Implement context generation
- [ ] Update `tests/context/CMakeLists.txt`

### Phase 2: Update Existing Code

**Step 2.1**: Update GameContext
- [ ] Modify `src/context/GameContext.h` constructor
- [ ] Modify `src/context/GameContext.cpp` implementation
- [ ] Change to take GameResources reference

**Step 2.2**: Update Logic System
- [ ] Update `src/logic/Logic.h` to use SceneContext
- [ ] Update `src/logic/Logic.cpp` implementation
- [ ] Rename `m_logic_context` to `m_scene_context`
- [ ] Update `src/logic/LogicFactory.h` to use SceneContext
- [ ] Update `src/logic/LogicFactory.cpp` to pass SceneContext

**Step 2.3**: Update All Logic Implementations
- [ ] Update `UIActionLogic.h/cpp` to use SceneContext
- [ ] Update `UIRenderLogic.h/cpp` to use SceneContext
- [ ] Update `UICollisionLogic.h/cpp` to use SceneContext
- [ ] Update `UIStateLogic.h/cpp` to use SceneContext
- [ ] Update `CraftingRenderLogic.h/cpp` to use SceneContext
- [ ] Replace all references to LogicContext with SceneContext
- [ ] Update all member variable names

**Step 2.4**: Update Scene System
- [ ] Update `src/scenes/Scene.h` GetLogicContext -> GetSceneContext
- [ ] Update `src/scenes/Scene.cpp` to create SceneContext
- [ ] Update `src/scenes/SceneFactory.cpp` to use SceneContext

**Step 2.5**: Update ContextConfigurator
- [ ] Simplify `src/context/ContextConfigurator.h`
- [ ] Update `src/context/ContextConfigurator.cpp` to wrap ResourceConfigurator
- [ ] Remove builder creation methods
- [ ] Add resource configuration methods

### Phase 3: Remove Builders

**Step 3.1**: Remove Builder Files
- [ ] Delete `src/context/GameContextBuilder.h`
- [ ] Delete `src/context/GameContextBuilder.cpp`
- [ ] Delete `src/logic/LogicContextBuilder.h`
- [ ] Delete `src/logic/LogicContextBuilder.cpp`
- [ ] Update `src/context/CMakeLists.txt`
- [ ] Update `src/logic/CMakeLists.txt`

**Step 3.2**: Remove LogicContext
- [ ] Delete `src/logic/LogicContext.h`
- [ ] Verify all references updated to SceneContext

**Step 3.3**: Remove Builder Tests
- [ ] Delete `tests/unit/context/GameContextBuilder.test.cpp`
- [ ] Delete `tests/unit/context/LogicContextBuilder.test.cpp`
- [ ] Update `tests/unit/context/CMakeLists.txt`

### Phase 4: Update Tests

**Step 4.1**: Update Unit Tests
- [ ] Update `tests/unit/context/ContextConfigurator.test.cpp`
- [ ] Update all Logic test files to use SceneContext
- [ ] Update `tests/context/TestContext.h/cpp` to use SceneContext
- [ ] Update test helper files

**Step 4.2**: Update Integration Tests
- [ ] Update `tests/integration/context_configuration/context_configuration.integration.test.cpp`
- [ ] Replace builder usage with resource configuration

**Step 4.3**: Update Scene Tests
- [ ] Update all scene test files to use SceneContext
- [ ] Update test expectations

### Phase 5: Integration

**Step 5.1**: Future GameEngine Integration (Optional)
- [ ] Consider updating GameEngine to own GameResources
- [ ] Update GameEngine constructor
- [ ] Update initialization flow

**Step 5.2**: Future Scene Integration (Optional)
- [ ] Consider updating Scene to own SceneResources
- [ ] Update Scene initialization
- [ ] Update resource lifecycle management

## Testing Strategy

### Unit Tests

**GameResources/SceneResources**:
- Test default construction
- Test member initialization
- Verify no cross-references

**ResourceConfigurator**:
- Test GameResources configuration
- Test SceneResources configuration
- Test environment type parsing
- Test error handling (null config, missing fields)

**GameContext/SceneContext**:
- Test construction from resources
- Test reference validity
- Test that changes to resources reflect in contexts

**TestFixture**:
- Test resource generation
- Test context generation
- Test different scene types

### Integration Tests

**Resource Configuration**:
- Test loading from FlatBuffers
- Test all configured scene types
- Test configuration values

**Logic System**:
- Test Logic classes with SceneContext
- Test LogicFactory with SceneContext
- Verify logic execution

**Scene System**:
- Test Scene creation with new architecture
- Test SceneContext generation
- Verify scene functionality

## Migration Guide

### For Existing Code Using GameContext

**Before**:
```cpp
GameContext context(window, event_handler, loop_number, 
                   asset_manager, env_type);
```

**After**:
```cpp
GameResources resources;
// Configure resources (manually or via ResourceConfigurator)
resources.env_type = env_type;
GameContext context(resources);
```

### For Existing Code Using LogicContext

**Before**:
```cpp
LogicContext logic_context{scene_entities, archetypes, scene_texture,
                           game_window, asset_manager, event_handler,
                           mouse_position};
```

**After**:
```cpp
SceneContext scene_context(scene_resources, game_resources);
```

### For Tests

**Before (with builders)**:
```cpp
auto builder = GameContextBuilder()
    .SetWindow(window_ptr)
    .SetEventHandler(handler_ptr)
    .Build();
```

**After (with TestFixture)**:
```cpp
TestFixture fixture(SceneType::SceneType_TEST);
GameContext &context = fixture.GetGameContext();
```

## Benefits

1. **Clearer Ownership**: Resources own objects at appropriate level
2. **Simpler API**: No builder pattern complexity
3. **Data-Driven**: Resources configured from FlatBuffers
4. **Better Testing**: TestFixture provides clean infrastructure
5. **Type Safety**: Compile-time reference checking
6. **Explicit Dependencies**: Clear what each component needs
7. **Reduced Code**: Eliminates ~1000+ lines of builder code

## Risks and Mitigations

### Risk: Breaking Existing Code
**Mitigation**: 
- Implement in phases
- Update systematically (all Logic classes together)
- Comprehensive testing at each phase
- Keep TestContext working during transition

### Risk: Reference Lifetime Issues
**Mitigation**:
- Resources must outlive contexts (enforced by design)
- Clear ownership documented
- Runtime checks in Debug builds if needed

### Risk: Test Infrastructure Complexity
**Mitigation**:
- TestFixture is simpler than builders
- Owns resources directly
- No smart pointer management needed

## Success Criteria

1. All builder files removed
2. All code uses Resource/Context architecture
3. All tests pass
4. ResourceConfigurator works with FlatBuffers
5. TestFixture provides clean test infrastructure
6. No performance regression
7. Code is simpler and more maintainable

## Timeline Estimate

- Phase 1 (Create new files): 2-3 hours
- Phase 2 (Update existing code): 4-5 hours
- Phase 3 (Remove builders): 1-2 hours
- Phase 4 (Update tests): 3-4 hours
- Phase 5 (Integration): 1-2 hours
- Testing and validation: 2-3 hours

**Total**: ~13-19 hours of focused development work

## Conclusion

This refactoring will significantly improve the codebase by:
- Separating resource ownership from access
- Eliminating builder pattern complexity
- Enabling data-driven configuration
- Providing better test infrastructure

The two-layer architecture (Resources for ownership, Contexts for access) is cleaner, more maintainable, and aligns with modern C++ best practices.
