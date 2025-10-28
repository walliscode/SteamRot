# ContextDirector Integration Examples

This document provides complete examples of how to integrate ContextDirector into GameEngine and SceneFactory.

## Overview

ContextDirector provides centralized management of LogicContextBuilder instances, enabling:
- Single registration point during initialization
- Easy context retrieval when creating scenes
- Separation of builder configuration from context creation
- Simple scene switching

## Integration with GameEngine

### Example: GameEngine Initialization

This example shows how to load context configurations and register builders during GameEngine startup.

```cpp
// In GameEngine.h
class GameEngine {
private:
  // ... existing members ...
  
  std::expected<std::monostate, FailInfo> InitializeContextDirector();
  
public:
  // ... existing methods ...
};

// In GameEngine.cpp
std::expected<std::monostate, FailInfo> GameEngine::InitializeContextDirector() {
  // Load context configuration
  FlatbuffersDataLoader loader;
  auto context_data_result = loader.ProvideContextData();
  if (!context_data_result.has_value()) {
    return std::unexpected(context_data_result.error());
  }

  // Create configurator
  ContextConfigurator configurator(context_data_result.value());
  
  // Get scene configurations
  const auto* context_data = context_data_result.value();
  if (!context_data->scene_contexts()) {
    return std::unexpected(FailInfo{
      FailMode::MissingRequiredField,
      "No scene contexts in configuration"
    });
  }

  // Register builders for all configured scenes
  for (const auto* scene_config : *context_data->scene_contexts()) {
    // Create builder from configuration
    auto builder_result = configurator.CreateLogicContextBuilder(
        scene_config->scene_type());
    
    if (!builder_result.has_value()) {
      return std::unexpected(builder_result.error());
    }

    auto builder = builder_result.value();
    
    // Add runtime objects from GameEngine
    // These are the same for all scenes
    builder.SetGameWindow(m_window_ptr)
           .SetAssetManager(m_asset_manager_ptr)
           .SetEventHandler(m_event_handler_ptr)
           .SetMousePosition(m_mouse_position_ptr);
    
    // Register builder with ContextDirector
    ContextDirector::RegisterLogicContextBuilder(
        scene_config->scene_type(), builder);
  }

  return std::monostate{};
}

// Call during GameEngine startup
std::expected<std::monostate, FailInfo> GameEngine::Initialize() {
  // ... existing initialization ...
  
  // Initialize ContextDirector
  auto director_result = InitializeContextDirector();
  if (!director_result.has_value()) {
    return std::unexpected(director_result.error());
  }
  
  // ... rest of initialization ...
  
  return std::monostate{};
}
```

### Benefits

1. **Centralized Configuration**: All scene builders configured in one place
2. **Early Validation**: Configuration errors caught during startup
3. **Shared Resources**: Game-level resources (window, assets) set once
4. **Easy Debugging**: Single point to inspect registered builders

## Integration with SceneFactory

### Example: Scene Creation with ContextDirector

This example shows how SceneFactory can use ContextDirector to create contexts for new scenes.

```cpp
// In SceneFactory.h
class SceneFactory {
public:
  static std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateScene(SceneType type, const GameContext& game_context);

private:
  static std::expected<LogicContext, FailInfo>
  CreateLogicContextForScene(SceneType type,
                             EntityMemoryPool& entities,
                             const ArchetypeManager& archetypes,
                             sf::RenderTexture& scene_texture);
};

// In SceneFactory.cpp
std::expected<LogicContext, FailInfo>
SceneFactory::CreateLogicContextForScene(
    SceneType type,
    EntityMemoryPool& entities,
    const ArchetypeManager& archetypes,
    sf::RenderTexture& scene_texture) {
  
  // Get builder from ContextDirector
  auto builder_result = ContextDirector::GetLogicContextBuilder(type);
  if (!builder_result.has_value()) {
    return std::unexpected(builder_result.error());
  }

  auto builder = builder_result.value();
  
  // Add scene-specific resources
  // Game-level resources were already set during GameEngine initialization
  builder.SetSceneEntities(std::make_shared<EntityMemoryPool>(entities))
         .SetArchetypes(std::make_shared<const std::unordered_map<ArchetypeID, Archetype>>(
             archetypes.GetArchetypes()))
         .SetSceneTexture(std::make_shared<sf::RenderTexture>(scene_texture));
  
  // Build and return context
  return builder.Build();
}

std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateScene(SceneType type, const GameContext& game_context) {
  auto scene = std::make_unique<Scene>();
  
  // Load scene entities from configuration
  auto entities_result = LoadSceneEntities(type);
  if (!entities_result.has_value()) {
    return std::unexpected(entities_result.error());
  }
  
  // Create LogicContext using ContextDirector
  auto logic_context_result = CreateLogicContextForScene(
      type,
      scene->GetEntityPool(),
      scene->GetArchetypeManager(),
      scene->GetRenderTexture());
  
  if (!logic_context_result.has_value()) {
    return std::unexpected(logic_context_result.error());
  }

  // Create logic systems with context
  LogicFactory logic_factory(type, logic_context_result.value());
  auto logic_map_result = logic_factory.CreateLogicMap();
  
  if (!logic_map_result.has_value()) {
    return std::unexpected(logic_map_result.error());
  }

  scene->SetLogic(std::move(logic_map_result.value()));
  
  return scene;
}
```

### Benefits

1. **Simplified Scene Creation**: No need to manually construct LogicContext
2. **Consistent Configuration**: All scenes use registered builders
3. **Separation of Concerns**: Builder configuration separate from scene creation
4. **Error Handling**: Clear error propagation via std::expected

## Integration with Scene Switching

### Example: Switching Between Scenes

This example shows how to use ContextDirector when switching scenes.

```cpp
// In SceneManager.h
class SceneManager {
private:
  std::unique_ptr<Scene> m_current_scene;
  
  std::expected<std::monostate, FailInfo>
  LoadScene(SceneType type, const GameContext& game_context);

public:
  std::expected<std::monostate, FailInfo>
  ChangeScene(SceneType new_type, const GameContext& game_context);
};

// In SceneManager.cpp
std::expected<std::monostate, FailInfo>
SceneManager::LoadScene(SceneType type, const GameContext& game_context) {
  // Verify builder is registered before attempting to create scene
  if (!ContextDirector::HasBuilder(type)) {
    return std::unexpected(FailInfo{
      FailMode::NonExistentEnumValue,
      "No context builder registered for scene type: " + 
      std::to_string(static_cast<int>(type))
    });
  }

  // Create scene using SceneFactory (which uses ContextDirector)
  auto scene_result = SceneFactory::CreateScene(type, game_context);
  
  if (!scene_result.has_value()) {
    return std::unexpected(scene_result.error());
  }

  // Replace current scene
  m_current_scene = std::move(scene_result.value());
  
  return std::monostate{};
}

std::expected<std::monostate, FailInfo>
SceneManager::ChangeScene(SceneType new_type, const GameContext& game_context) {
  // Clean up current scene
  if (m_current_scene) {
    m_current_scene->Cleanup();
  }

  // Load new scene
  auto load_result = LoadScene(new_type, game_context);
  if (!load_result.has_value()) {
    return std::unexpected(load_result.error());
  }

  return std::monostate{};
}
```

### Benefits

1. **Pre-Registration Validation**: Check builder exists before creating scene
2. **Consistent Process**: Same workflow for all scene switches
3. **Clear Errors**: Specific error if builder not registered
4. **Simple API**: ContextDirector makes scene switching straightforward

## Complete Workflow Example

### Full Integration from Startup to Scene Change

```cpp
// 1. GameEngine Startup
int main() {
  GameEngine engine;
  
  // Initialize engine (includes ContextDirector registration)
  auto init_result = engine.Initialize();
  if (!init_result.has_value()) {
    // Handle error
    return -1;
  }
  
  // At this point, all scene builders are registered in ContextDirector
  
  // Start with title scene
  auto game_context = engine.GetGameContext();
  auto load_result = engine.GetSceneManager().LoadScene(
      SceneType::TITLE, game_context);
  
  if (!load_result.has_value()) {
    // Handle error
    return -1;
  }
  
  // Run game loop
  engine.Run();
  
  return 0;
}

// 2. During GameEngine::Initialize()
std::expected<std::monostate, FailInfo> GameEngine::Initialize() {
  // Create window, assets, event handler, etc.
  // ...
  
  // Load and register context builders
  FlatbuffersDataLoader loader;
  auto context_data = loader.ProvideContextData().value();
  ContextConfigurator configurator(context_data);
  
  for (const auto* scene_config : *context_data->scene_contexts()) {
    auto builder = configurator.CreateLogicContextBuilder(
        scene_config->scene_type()).value();
    
    builder.SetGameWindow(m_window_ptr)
           .SetAssetManager(m_asset_manager_ptr)
           .SetEventHandler(m_event_handler_ptr)
           .SetMousePosition(m_mouse_position_ptr);
    
    ContextDirector::RegisterLogicContextBuilder(
        scene_config->scene_type(), builder);
  }
  
  return std::monostate{};
}

// 3. When creating a scene (e.g., user clicks "Start Game")
void OnStartGameClicked() {
  auto change_result = m_scene_manager.ChangeScene(
      SceneType::CRAFTING, m_game_context);
  
  if (!change_result.has_value()) {
    // Display error to user
    ShowErrorDialog(change_result.error().message);
  }
}

// 4. Inside SceneFactory::CreateScene()
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateScene(SceneType type, const GameContext& game_context) {
  // Get builder from ContextDirector (already configured with game resources)
  auto builder = ContextDirector::GetLogicContextBuilder(type).value();
  
  // Add scene-specific resources
  // ...
  
  // Build context and create scene
  auto context = builder.Build().value();
  // ...
  
  return scene;
}
```

## Error Handling Patterns

### Checking Builder Registration

```cpp
// Before attempting to create a scene
if (!ContextDirector::HasBuilder(SceneType::CUSTOM_SCENE)) {
  // Builder not registered - this scene wasn't configured
  // Option 1: Return error
  return std::unexpected(FailInfo{
    FailMode::NonExistentEnumValue,
    "Scene not configured: CUSTOM_SCENE"
  });
  
  // Option 2: Register default builder on-the-fly
  auto default_builder = CreateDefaultBuilder();
  ContextDirector::RegisterLogicContextBuilder(
      SceneType::CUSTOM_SCENE, default_builder);
}

// Now safe to create scene
auto scene = SceneFactory::CreateScene(SceneType::CUSTOM_SCENE, context);
```

### Handling Configuration Load Failures

```cpp
std::expected<std::monostate, FailInfo> GameEngine::InitializeContextDirector() {
  FlatbuffersDataLoader loader;
  auto context_data_result = loader.ProvideContextData();
  
  if (!context_data_result.has_value()) {
    // Configuration file missing or corrupt
    
    // Option 1: Fatal error - can't continue
    return std::unexpected(FailInfo{
      FailMode::FileNotFound,
      "Cannot load context configuration: " + 
      context_data_result.error().message
    });
    
    // Option 2: Use fallback/default configuration
    auto default_builders = CreateDefaultBuilders();
    for (const auto& [type, builder] : default_builders) {
      ContextDirector::RegisterLogicContextBuilder(type, builder);
    }
    return std::monostate{};
  }
  
  // Continue with loaded configuration
  // ...
}
```

### Handling Build Failures

```cpp
std::expected<LogicContext, FailInfo>
SceneFactory::CreateLogicContextForScene(...) {
  auto builder = ContextDirector::GetLogicContextBuilder(type).value();
  
  // Add scene resources
  builder.SetSceneEntities(...)
         .SetArchetypes(...)
         .SetSceneTexture(...);
  
  // Build can fail if required fields are missing
  auto context_result = builder.Build();
  
  if (!context_result.has_value()) {
    // Builder was incomplete
    // Log which field was missing
    spdlog::error("Failed to build LogicContext for scene {}: {}",
                  static_cast<int>(type),
                  context_result.error().message);
    
    return std::unexpected(context_result.error());
  }
  
  return context_result.value();
}
```

## Testing Patterns

### Testing Scene Creation with ContextDirector

```cpp
TEST_CASE("Scene creation uses ContextDirector", "[integration][scene]") {
  // Setup
  steamrot::ContextDirector::ClearBuilders();
  
  // Load test configuration
  steamrot::PathProvider provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersDataLoader loader;
  auto context_data = loader.ProvideContextData().value();
  
  // Register builders
  steamrot::ContextConfigurator configurator(context_data);
  for (const auto* scene_config : *context_data->scene_contexts()) {
    auto builder = configurator.CreateLogicContextBuilder(
        scene_config->scene_type()).value();
    
    // Add test game resources
    builder.SetGameWindow(test_window)
           .SetAssetManager(test_assets)
           .SetEventHandler(test_handler)
           .SetMousePosition(test_mouse_pos);
    
    steamrot::ContextDirector::RegisterLogicContextBuilder(
        scene_config->scene_type(), builder);
  }
  
  // Test
  REQUIRE(steamrot::ContextDirector::HasBuilder(steamrot::SceneType::TEST));
  
  auto scene = steamrot::SceneFactory::CreateScene(
      steamrot::SceneType::TEST, test_game_context);
  
  REQUIRE(scene.has_value());
  
  // Cleanup
  steamrot::ContextDirector::ClearBuilders();
}
```

### Testing Builder Registration

```cpp
TEST_CASE("GameEngine registers all configured scenes", "[integration]") {
  steamrot::ContextDirector::ClearBuilders();
  
  steamrot::GameEngine engine;
  auto init_result = engine.Initialize();
  
  REQUIRE(init_result.has_value());
  
  // Verify all expected scenes are registered
  REQUIRE(steamrot::ContextDirector::HasBuilder(steamrot::SceneType::TITLE));
  REQUIRE(steamrot::ContextDirector::HasBuilder(steamrot::SceneType::CRAFTING));
  
  steamrot::ContextDirector::ClearBuilders();
}
```

## Performance Considerations

### Builder Registration Cost

- **One-time cost**: Registration happens during initialization
- **O(1) lookup**: Map-based storage provides fast retrieval
- **Memory overhead**: Minimal (one LogicContextBuilder per scene type)
- **Not on critical path**: No impact on game loop performance

### Builder Copy Cost

- **Lightweight**: Copying LogicContextBuilder is fast (shared_ptr copies)
- **Independent**: Modifications to copy don't affect registry
- **Safe**: Prevents accidental registry corruption

## Summary

ContextDirector integration provides:

1. **Centralized Management**: All builders registered in one place
2. **Simplified Scene Creation**: SceneFactory gets builders from director
3. **Configuration-Driven**: Builders created from configuration files
4. **Error Handling**: Clear error propagation at each step
5. **Testable**: Easy to mock and test with ClearBuilders()

The integration workflow:
```
GameEngine::Initialize()
  → Load configuration (FlatbuffersDataLoader)
  → Create builders (ContextConfigurator)
  → Register builders (ContextDirector)

SceneFactory::CreateScene()
  → Get builder (ContextDirector)
  → Add scene resources
  → Build context
  → Create scene
```

This pattern separates concerns and makes the codebase more maintainable while preserving type safety and error handling.
