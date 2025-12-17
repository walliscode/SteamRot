# Factory Selection Strategies for Data Providers and Configurators

## Executive Summary

This document analyzes strategies for selecting concrete implementations of data providers and configurators at runtime. Unlike switching on data types after loading (covered in SWITCHING_STRATEGIES_ANALYSIS.md), this addresses the factory-level problem: **how to choose between different provider implementations** (FlatBuffers, JSON, XML, etc.) **before data is loaded**.

**Target Audience**: Architects and developers implementing data loading systems, plugin architectures, and configurable data pipelines.

**Reading Time**: 20-25 minutes

**Key Problem**: Current SteamRot implementation hard-codes FlatBuffers providers in `provider_factory.cpp` and configurators in `SceneFactory.cpp`. This prevents supporting multiple data formats (save files, test data, JSON configs) without code modification.

**Key Findings**:
- **Six viable strategies** identified: Configuration File, Environment Variable, Factory Method Parameter, Registry Pattern, Strategy Pattern with Injection, Abstract Factory Pattern
- **Current approach is brittle**: Hard-coded static instances in factory functions
- **Recommendation**: Use Configuration File strategy for production + Strategy Pattern with Injection for testing
- **Migration path**: Refactor `provider_factory.cpp` to read configuration, inject providers into consumers

---

## Table of Contents

1. [Problem Statement](#problem-statement)
2. [Current Architecture Issues](#current-architecture-issues)
3. [Strategy 1: Configuration File](#strategy-1-configuration-file)
4. [Strategy 2: Environment Variable](#strategy-2-environment-variable)
5. [Strategy 3: Factory Method Parameter](#strategy-3-factory-method-parameter)
6. [Strategy 4: Registry Pattern](#strategy-4-registry-pattern)
7. [Strategy 5: Strategy Pattern with Dependency Injection](#strategy-5-strategy-pattern-with-dependency-injection)
8. [Strategy 6: Abstract Factory Pattern](#strategy-6-abstract-factory-pattern)
9. [Comparative Analysis](#comparative-analysis)
10. [Recommendations for SteamRot](#recommendations-for-steamrot)
11. [Migration Path](#migration-path)
12. [Implementation Examples](#implementation-examples)

---

## Problem Statement

### Current Hard-Coded Approach

**File**: `src/data_providers/provider_factory.cpp`

```cpp
IEngineDataProvider &GetEngineDataProvider() {
  static FlatbuffersEngineDataProvider provider;  // Hard-coded!
  return provider;
}

ISceneDataProvider &GetSceneDataProvider() {
  static FlatbuffersSceneDataProvider provider;  // Hard-coded!
  return provider;
}
```

**File**: `src/scenes/SceneFactory.cpp`

```cpp
ISceneConfigurator &GetSceneConfigurator() {
  static FlatbuffersSceneConfigurator configurator;  // Hard-coded!
  return configurator;
}
```

### Why This Is Problematic

1. **Cannot support multiple data formats**: Want to load from JSON for prototyping, FlatBuffers for production, save files for gameplay
2. **Testing is difficult**: Cannot inject mock providers without modifying production code
3. **Violates Open/Closed Principle**: Adding new format requires editing factory source code
4. **No runtime flexibility**: Cannot switch formats based on configuration or user preference
5. **Tight coupling**: Consumers directly depend on concrete implementations

### Real Use Cases in SteamRot

#### Use Case 1: Save Files vs Default Data
```cpp
// Want to load from save file if it exists, otherwise from defaults
ISceneDataProvider &provider = 
    save_file_exists ? GetSaveFileProvider() : GetDefaultDataProvider();
    
std::unique_ptr<SceneData> data = provider.ProvideDefaultSceneData(type);
```

#### Use Case 2: Test Data Isolation
```cpp
// Tests should use test data, not production FlatBuffers
ISceneDataProvider &provider = GetTestDataProvider();  // Need this!
```

#### Use Case 3: Format Migration
```cpp
// Support both old JSON format and new FlatBuffers during migration
ISceneDataProvider &provider = 
    config.use_legacy_format ? GetJsonProvider() : GetFlatBuffersProvider();
```

#### Use Case 4: Modding Support
```cpp
// Allow mods to provide custom data formats
ISceneDataProvider &provider = GetModDataProvider(mod_id);
```

### Requirements

A good factory selection strategy should:

1. ✅ **Decoupling**: Consumers don't depend on concrete provider types
2. ✅ **Runtime Configuration**: Select implementation without recompiling
3. ✅ **Testability**: Easy to inject test/mock implementations
4. ✅ **Extensibility**: Add new formats without modifying existing code
5. ✅ **Type Safety**: Compile-time checking where possible
6. ✅ **Clear Ownership**: Lifetime management is explicit
7. ✅ **Performance**: Minimal overhead for selection logic

---

## Current Architecture Issues

### Issue 1: Static Lifetime Trap

```cpp
ISceneDataProvider &GetSceneDataProvider() {
  static FlatbuffersSceneDataProvider provider;  // Lives forever!
  return provider;
}
```

**Problems**:
- Cannot change implementation after first call
- Cannot have multiple instances with different configurations
- Destruction order is undefined (potential crashes on exit)
- Cannot reset state for testing

### Issue 2: Hidden Dependencies

```cpp
// In SceneFactory.cpp
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateSceneFromDefault(SceneType type) {
  ISceneDataProvider &provider = GetSceneDataProvider();  // Hidden dependency!
  // ...
}
```

**Problems**:
- SceneFactory depends on global factory function
- Cannot see what providers are needed from class interface
- Hard to test - requires global state manipulation
- Violates Dependency Inversion Principle

### Issue 3: Circular Configuration Problem

**File**: `src/data_providers/provider_factory.cpp`

```cpp
// How do we know which provider to return?
ISceneDataProvider &GetSceneDataProvider() {
  // Need configuration to decide, but where does config come from?
  // If config itself uses a provider, we have a circular dependency!
  
  static FlatbuffersSceneDataProvider provider;  // Just hard-code it :(
  return provider;
}
```

---

## Strategy 1: Configuration File

### Description

Read a configuration file at startup to determine which concrete implementations to instantiate. Store the configuration in a global or application-level object accessible to factory functions.

### Implementation Pattern

**Configuration File** (`data/engine/provider_config.json`):
```json
{
  "providers": {
    "scene_data": "flatbuffers",
    "engine_data": "flatbuffers",
    "asset_data": "flatbuffers",
    "save_data": "json"
  },
  "fallback_provider": "flatbuffers"
}
```

**Configuration Struct**:
```cpp
// src/types/core/ProviderConfig.h
namespace steamrot {

enum class ProviderType {
  FlatBuffers,
  JSON,
  XML,
  SaveFile,
  Test
};

struct ProviderConfig {
  ProviderType scene_data_provider = ProviderType::FlatBuffers;
  ProviderType engine_data_provider = ProviderType::FlatBuffers;
  ProviderType asset_data_provider = ProviderType::FlatBuffers;
  ProviderType save_data_provider = ProviderType::SaveFile;
  
  // Load from file
  static std::expected<ProviderConfig, FailInfo> LoadFromFile(
      const std::string& path);
};

} // namespace steamrot
```

**Factory Implementation**:
```cpp
// src/data_providers/provider_factory.cpp
namespace steamrot {

// Global configuration (loaded at startup)
static ProviderConfig g_provider_config;

// Initialize configuration (called once at startup)
void InitializeProviderFactory(const ProviderConfig& config) {
  g_provider_config = config;
}

ISceneDataProvider& GetSceneDataProvider() {
  static std::unique_ptr<ISceneDataProvider> provider;
  
  if (!provider) {
    switch (g_provider_config.scene_data_provider) {
    case ProviderType::FlatBuffers:
      provider = std::make_unique<FlatbuffersSceneDataProvider>();
      break;
    case ProviderType::JSON:
      provider = std::make_unique<JsonSceneDataProvider>();
      break;
    case ProviderType::SaveFile:
      provider = std::make_unique<SaveFileSceneDataProvider>();
      break;
    case ProviderType::Test:
      provider = std::make_unique<TestSceneDataProvider>();
      break;
    default:
      // Fallback
      provider = std::make_unique<FlatbuffersSceneDataProvider>();
    }
  }
  
  return *provider;
}

} // namespace steamrot
```

**Usage in main()**:
```cpp
int main() {
  // Load provider configuration early
  auto config_result = ProviderConfig::LoadFromFile("data/engine/provider_config.json");
  if (!config_result.has_value()) {
    // Handle error - use defaults
    InitializeProviderFactory(ProviderConfig{});
  } else {
    InitializeProviderFactory(config_result.value());
  }
  
  // Now all factory functions use configured providers
  ISceneDataProvider& provider = GetSceneDataProvider();
  // ...
}
```

### Pros

✅ **Runtime Configuration**: Change providers without recompiling  
✅ **User Control**: Can be edited by users for modding  
✅ **Clear Configuration**: All provider choices in one place  
✅ **Minimal Code Changes**: Factory functions stay mostly the same  
✅ **Environment-Specific**: Different configs for dev/test/prod  

### Cons

❌ **Bootstrap Problem**: How to load config file itself? (circular dependency)  
❌ **Global State**: Configuration is global mutable state  
❌ **Initialization Order**: Must initialize before any factory calls  
❌ **Error Handling**: What if config file is missing or invalid?  
❌ **Testing Complexity**: Must reset global state between tests  

### When to Use

- ✅ Want users/devs to configure data formats without coding
- ✅ Different environments need different providers (dev/test/prod)
- ✅ Application has clear initialization phase before factory usage
- ✅ Configuration is loaded from simple, early-available source

### When NOT to Use

- ❌ Need to change providers during runtime (not just at startup)
- ❌ Testing requires frequent provider switching
- ❌ Configuration itself requires a provider (circular dependency)

---

## Strategy 2: Environment Variable

### Description

Use environment variables to select concrete implementations. Similar to configuration file but uses OS environment instead of file.

### Implementation Pattern

**Set Environment Variable**:
```bash
# Development
export STEAMROT_SCENE_PROVIDER=flatbuffers
export STEAMROT_ENGINE_PROVIDER=flatbuffers

# Testing
export STEAMROT_SCENE_PROVIDER=test
export STEAMROT_ENGINE_PROVIDER=test

# Production
export STEAMROT_SCENE_PROVIDER=flatbuffers
export STEAMROT_ENGINE_PROVIDER=flatbuffers
```

**Factory Implementation**:
```cpp
// src/data_providers/provider_factory.cpp
namespace steamrot {

ProviderType GetProviderTypeFromEnv(const std::string& env_var, 
                                    ProviderType default_type) {
  const char* value = std::getenv(env_var.c_str());
  if (!value) return default_type;
  
  std::string str_value(value);
  if (str_value == "flatbuffers") return ProviderType::FlatBuffers;
  if (str_value == "json") return ProviderType::JSON;
  if (str_value == "xml") return ProviderType::XML;
  if (str_value == "save") return ProviderType::SaveFile;
  if (str_value == "test") return ProviderType::Test;
  
  return default_type;
}

ISceneDataProvider& GetSceneDataProvider() {
  static std::unique_ptr<ISceneDataProvider> provider;
  
  if (!provider) {
    ProviderType type = GetProviderTypeFromEnv(
        "STEAMROT_SCENE_PROVIDER", 
        ProviderType::FlatBuffers);
    
    switch (type) {
    case ProviderType::FlatBuffers:
      provider = std::make_unique<FlatbuffersSceneDataProvider>();
      break;
    case ProviderType::JSON:
      provider = std::make_unique<JsonSceneDataProvider>();
      break;
    case ProviderType::Test:
      provider = std::make_unique<TestSceneDataProvider>();
      break;
    default:
      provider = std::make_unique<FlatbuffersSceneDataProvider>();
    }
  }
  
  return *provider;
}

} // namespace steamrot
```

**Testing Usage**:
```cpp
// In test setup
void SetUp() override {
  setenv("STEAMROT_SCENE_PROVIDER", "test", 1);
  setenv("STEAMROT_ENGINE_PROVIDER", "test", 1);
}

void TearDown() override {
  unsetenv("STEAMROT_SCENE_PROVIDER");
  unsetenv("STEAMROT_ENGINE_PROVIDER");
}
```

### Pros

✅ **No File I/O**: No bootstrap problem  
✅ **Platform Standard**: Environment variables are OS-native  
✅ **CI/CD Friendly**: Easy to set in build scripts  
✅ **Per-Process Configuration**: Each process can have different settings  
✅ **Testing Support**: Can override in test fixtures  

### Cons

❌ **Limited Data Types**: Only strings (no complex config)  
❌ **Not User-Friendly**: Requires command-line knowledge  
❌ **Platform Differences**: Windows vs Unix environment variables  
❌ **Debugging Difficulty**: Hard to see what's configured  
❌ **No Validation**: Invalid values silently fall back to default  

### When to Use

- ✅ CI/CD environments with automated testing
- ✅ Developer-focused configuration (not end-users)
- ✅ Simple binary choices (FlatBuffers vs Test)
- ✅ Container/cloud deployments

### When NOT to Use

- ❌ End-user configuration (not technical users)
- ❌ Complex configuration with multiple parameters
- ❌ Need runtime changes after process start

---

## Strategy 3: Factory Method Parameter

### Description

Pass the desired provider type as a parameter to factory functions. Moves selection responsibility to the caller.

### Implementation Pattern

**Factory Interface**:
```cpp
// src/data_providers/provider_factory.h
namespace steamrot {

enum class ProviderType {
  FlatBuffers,
  JSON,
  XML,
  SaveFile,
  Test
};

// Factory functions take provider type as parameter
std::unique_ptr<ISceneDataProvider> 
CreateSceneDataProvider(ProviderType type);

std::unique_ptr<IEngineDataProvider> 
CreateEngineDataProvider(ProviderType type);

std::unique_ptr<IAssetDataProvider> 
CreateAssetDataProvider(ProviderType type);

// Convenience: Get default providers (for backward compatibility)
ISceneDataProvider& GetDefaultSceneDataProvider();

} // namespace steamrot
```

**Factory Implementation**:
```cpp
// src/data_providers/provider_factory.cpp
namespace steamrot {

std::unique_ptr<ISceneDataProvider> 
CreateSceneDataProvider(ProviderType type) {
  switch (type) {
  case ProviderType::FlatBuffers:
    return std::make_unique<FlatbuffersSceneDataProvider>();
  case ProviderType::JSON:
    return std::make_unique<JsonSceneDataProvider>();
  case ProviderType::XML:
    return std::make_unique<XmlSceneDataProvider>();
  case ProviderType::SaveFile:
    return std::make_unique<SaveFileSceneDataProvider>();
  case ProviderType::Test:
    return std::make_unique<TestSceneDataProvider>();
  default:
    return std::make_unique<FlatbuffersSceneDataProvider>();
  }
}

// Default provider for convenience
ISceneDataProvider& GetDefaultSceneDataProvider() {
  static auto provider = CreateSceneDataProvider(ProviderType::FlatBuffers);
  return *provider;
}

} // namespace steamrot
```

**Usage in SceneFactory**:
```cpp
// src/scenes/SceneFactory.h
class SceneFactory {
private:
  const GameContext& m_game_context;
  ProviderType m_provider_type;  // Store provider type

public:
  SceneFactory(const GameContext& game_context, 
               ProviderType provider_type = ProviderType::FlatBuffers)
      : m_game_context(game_context), m_provider_type(provider_type) {}
      
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateSceneFromDefault(SceneType type) {
    // Create provider with specified type
    auto provider = CreateSceneDataProvider(m_provider_type);
    
    // Use provider
    std::unique_ptr<SceneData> data = provider->ProvideDefaultSceneData(type);
    // ...
  }
};
```

**Usage in main()**:
```cpp
int main() {
  // Determine provider type from config, environment, etc.
  ProviderType provider_type = DetermineProviderType();
  
  // Pass provider type to factory
  SceneFactory scene_factory(game_context, provider_type);
  
  auto scene = scene_factory.CreateSceneFromDefault(SceneType::TITLE);
  // ...
}
```

**Testing Usage**:
```cpp
TEST_CASE("Scene loads from test data", "[SceneFactory]") {
  // Create factory with test provider
  SceneFactory factory(test_context, ProviderType::Test);
  
  auto scene = factory.CreateSceneFromDefault(SceneType::TITLE);
  REQUIRE(scene.has_value());
}
```

### Pros

✅ **Explicit Control**: Caller decides which provider to use  
✅ **No Global State**: No static configuration to manage  
✅ **Easy Testing**: Pass test provider type directly  
✅ **Flexible**: Different factories can use different providers  
✅ **Clear Dependencies**: Provider type is visible in function signature  

### Cons

❌ **Parameter Proliferation**: Must pass type through call chain  
❌ **Backward Compatibility**: Changes existing function signatures  
❌ **Burden on Caller**: Every caller must make provider decision  
❌ **Duplication**: Provider type repeated at every call site  

### When to Use

- ✅ Different parts of application need different providers simultaneously
- ✅ Testing requires fine-grained provider control
- ✅ Small number of factory call sites
- ✅ Providers change frequently during execution

### When NOT to Use

- ❌ Large codebase with many factory call sites
- ❌ Provider type rarely changes
- ❌ Want centralized configuration

---

## Strategy 4: Registry Pattern

### Description

Maintain a registry of available providers and look them up by name or type at runtime. Supports runtime registration of new provider types (useful for plugins).

### Implementation Pattern

**Registry Interface**:
```cpp
// src/data_providers/ProviderRegistry.h
namespace steamrot {

class ProviderRegistry {
private:
  // Factory function type for creating providers
  using SceneDataProviderFactory = 
      std::function<std::unique_ptr<ISceneDataProvider>()>;
  
  // Map of provider name to factory function
  std::unordered_map<std::string, SceneDataProviderFactory> 
      m_scene_data_providers;
  
  // Singleton instance
  static ProviderRegistry& GetInstance();
  
public:
  // Register a provider factory
  void RegisterSceneDataProvider(
      const std::string& name,
      SceneDataProviderFactory factory);
  
  // Create provider by name
  std::unique_ptr<ISceneDataProvider> 
  CreateSceneDataProvider(const std::string& name);
  
  // Check if provider is registered
  bool HasSceneDataProvider(const std::string& name) const;
  
  // Get list of registered providers
  std::vector<std::string> GetRegisteredSceneDataProviders() const;
};

} // namespace steamrot
```

**Registry Implementation**:
```cpp
// src/data_providers/ProviderRegistry.cpp
namespace steamrot {

ProviderRegistry& ProviderRegistry::GetInstance() {
  static ProviderRegistry instance;
  return instance;
}

void ProviderRegistry::RegisterSceneDataProvider(
    const std::string& name,
    SceneDataProviderFactory factory) {
  m_scene_data_providers[name] = factory;
}

std::unique_ptr<ISceneDataProvider> 
ProviderRegistry::CreateSceneDataProvider(const std::string& name) {
  auto it = m_scene_data_providers.find(name);
  if (it == m_scene_data_providers.end()) {
    // Provider not found - return default
    return std::make_unique<FlatbuffersSceneDataProvider>();
  }
  return it->second();  // Call factory function
}

bool ProviderRegistry::HasSceneDataProvider(const std::string& name) const {
  return m_scene_data_providers.find(name) != m_scene_data_providers.end();
}

std::vector<std::string> 
ProviderRegistry::GetRegisteredSceneDataProviders() const {
  std::vector<std::string> names;
  for (const auto& [name, _] : m_scene_data_providers) {
    names.push_back(name);
  }
  return names;
}

} // namespace steamrot
```

**Registration at Startup**:
```cpp
// src/data_providers/register_providers.cpp
namespace steamrot {

void RegisterBuiltInProviders() {
  auto& registry = ProviderRegistry::GetInstance();
  
  // Register FlatBuffers provider
  registry.RegisterSceneDataProvider("flatbuffers", []() {
    return std::make_unique<FlatbuffersSceneDataProvider>();
  });
  
  // Register JSON provider
  registry.RegisterSceneDataProvider("json", []() {
    return std::make_unique<JsonSceneDataProvider>();
  });
  
  // Register XML provider
  registry.RegisterSceneDataProvider("xml", []() {
    return std::make_unique<XmlSceneDataProvider>();
  });
  
  // Register Save File provider
  registry.RegisterSceneDataProvider("save", []() {
    return std::make_unique<SaveFileSceneDataProvider>();
  });
  
  // Register Test provider
  registry.RegisterSceneDataProvider("test", []() {
    return std::make_unique<TestSceneDataProvider>();
  });
}

} // namespace steamrot
```

**Usage in main()**:
```cpp
int main() {
  // Register all built-in providers
  RegisterBuiltInProviders();
  
  // Load configuration (which provider to use)
  std::string provider_name = LoadProviderConfig();  // e.g., "flatbuffers"
  
  // Create provider from registry
  auto& registry = ProviderRegistry::GetInstance();
  auto provider = registry.CreateSceneDataProvider(provider_name);
  
  // ...
}
```

**Plugin Support**:
```cpp
// In plugin DLL
extern "C" void RegisterCustomProvider() {
  auto& registry = ProviderRegistry::GetInstance();
  
  registry.RegisterSceneDataProvider("custom_format", []() {
    return std::make_unique<CustomFormatSceneDataProvider>();
  });
}
```

### Pros

✅ **Runtime Extensibility**: Register new providers without recompiling  
✅ **Plugin Support**: Plugins can register their own providers  
✅ **Discoverable**: Can query available providers at runtime  
✅ **Centralized**: All provider registration in one place  
✅ **Flexible**: String-based lookup supports config files  

### Cons

❌ **Complexity**: More infrastructure code required  
❌ **Runtime Errors**: Invalid provider names fail at runtime  
❌ **Performance**: String lookup + function call overhead  
❌ **Global State**: Registry is typically a singleton  
❌ **Initialization Order**: Must register before usage  

### When to Use

- ✅ Plugin architecture (third-party provider implementations)
- ✅ Need to discover available providers at runtime
- ✅ Dynamic loading of modules/DLLs
- ✅ User can select provider from UI dropdown

### When NOT to Use

- ❌ All providers known at compile time
- ❌ Performance-critical code (string lookup overhead)
- ❌ Simple application without plugins

---

## Strategy 5: Strategy Pattern with Dependency Injection

### Description

Use constructor injection to pass provider instances to consumers. Inversion of Control (IoC) container can manage provider lifetimes. This is the most "clean architecture" approach.

### Implementation Pattern

**Modified SceneFactory** (accepts provider as dependency):
```cpp
// src/scenes/SceneFactory.h
namespace steamrot {

class SceneFactory {
private:
  const GameContext& m_game_context;
  ISceneDataProvider& m_scene_data_provider;  // Injected dependency
  ISceneConfigurator& m_scene_configurator;   // Injected dependency

public:
  // Dependency injection through constructor
  SceneFactory(const GameContext& game_context,
               ISceneDataProvider& scene_data_provider,
               ISceneConfigurator& scene_configurator)
      : m_game_context(game_context),
        m_scene_data_provider(scene_data_provider),
        m_scene_configurator(scene_configurator) {}
  
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateSceneFromDefault(SceneType type) {
    // Use injected provider (no factory function call!)
    std::unique_ptr<SceneData> data = 
        m_scene_data_provider.ProvideDefaultSceneData(type);
    
    if (!data) return nullptr;
    
    std::unique_ptr<Scene> scene = CreateEmptyScene(type).value();
    
    // Use injected configurator
    auto config_result = m_scene_configurator.ConfigureScene(*scene, data.get());
    if (!config_result.has_value()) return nullptr;
    
    return std::move(scene);
  }
};

} // namespace steamrot
```

**Provider Container** (manages provider lifetimes):
```cpp
// src/data_providers/ProviderContainer.h
namespace steamrot {

class ProviderContainer {
private:
  std::unique_ptr<ISceneDataProvider> m_scene_data_provider;
  std::unique_ptr<IEngineDataProvider> m_engine_data_provider;
  std::unique_ptr<IAssetDataProvider> m_asset_data_provider;
  std::unique_ptr<ISceneConfigurator> m_scene_configurator;

public:
  // Create container with default providers
  static ProviderContainer CreateDefault();
  
  // Create container with test providers
  static ProviderContainer CreateForTesting();
  
  // Create container with custom providers
  static ProviderContainer CreateCustom(
      std::unique_ptr<ISceneDataProvider> scene_provider,
      std::unique_ptr<IEngineDataProvider> engine_provider,
      std::unique_ptr<IAssetDataProvider> asset_provider,
      std::unique_ptr<ISceneConfigurator> scene_configurator);
  
  // Getters (return references to owned providers)
  ISceneDataProvider& GetSceneDataProvider() { return *m_scene_data_provider; }
  IEngineDataProvider& GetEngineDataProvider() { return *m_engine_data_provider; }
  IAssetDataProvider& GetAssetDataProvider() { return *m_asset_data_provider; }
  ISceneConfigurator& GetSceneConfigurator() { return *m_scene_configurator; }
};

} // namespace steamrot
```

**Container Implementation**:
```cpp
// src/data_providers/ProviderContainer.cpp
namespace steamrot {

ProviderContainer ProviderContainer::CreateDefault() {
  ProviderContainer container;
  container.m_scene_data_provider = 
      std::make_unique<FlatbuffersSceneDataProvider>();
  container.m_engine_data_provider = 
      std::make_unique<FlatbuffersEngineDataProvider>();
  container.m_asset_data_provider = 
      std::make_unique<FlatbuffersAssetDataProvider>();
  container.m_scene_configurator = 
      std::make_unique<FlatbuffersSceneConfigurator>();
  return container;
}

ProviderContainer ProviderContainer::CreateForTesting() {
  ProviderContainer container;
  container.m_scene_data_provider = 
      std::make_unique<TestSceneDataProvider>();
  container.m_engine_data_provider = 
      std::make_unique<TestEngineDataProvider>();
  container.m_asset_data_provider = 
      std::make_unique<TestAssetDataProvider>();
  container.m_scene_configurator = 
      std::make_unique<TestSceneConfigurator>();
  return container;
}

ProviderContainer ProviderContainer::CreateCustom(
    std::unique_ptr<ISceneDataProvider> scene_provider,
    std::unique_ptr<IEngineDataProvider> engine_provider,
    std::unique_ptr<IAssetDataProvider> asset_provider,
    std::unique_ptr<ISceneConfigurator> scene_configurator) {
  ProviderContainer container;
  container.m_scene_data_provider = std::move(scene_provider);
  container.m_engine_data_provider = std::move(engine_provider);
  container.m_asset_data_provider = std::move(asset_provider);
  container.m_scene_configurator = std::move(scene_configurator);
  return container;
}

} // namespace steamrot
```

**Usage in main()**:
```cpp
int main() {
  // Create provider container (determines which implementations to use)
  auto providers = ProviderContainer::CreateDefault();
  
  // Inject providers into consumers
  GameContext game_context(/* ... */);
  SceneFactory scene_factory(
      game_context,
      providers.GetSceneDataProvider(),
      providers.GetSceneConfigurator());
  
  // SceneFactory uses injected providers
  auto scene = scene_factory.CreateSceneFromDefault(SceneType::TITLE);
  // ...
}
```

**Testing Usage**:
```cpp
TEST_CASE("Scene loads from test data", "[SceneFactory]") {
  // Create test providers
  auto providers = ProviderContainer::CreateForTesting();
  
  // Inject test providers
  TestGameContext test_context;
  SceneFactory factory(
      test_context,
      providers.GetSceneDataProvider(),
      providers.GetSceneConfigurator());
  
  auto scene = factory.CreateSceneFromDefault(SceneType::TITLE);
  REQUIRE(scene.has_value());
}
```

### Pros

✅ **Testability**: Easy to inject mocks and test implementations  
✅ **Clear Dependencies**: Dependencies visible in constructor  
✅ **Inversion of Control**: Consumers don't create dependencies  
✅ **No Global State**: No singletons or global factory functions  
✅ **Lifetime Control**: Container manages provider lifetimes  
✅ **SOLID Principles**: Follows Dependency Inversion Principle  

### Cons

❌ **Refactoring Required**: Must change all consumer constructors  
❌ **Boilerplate**: More code for container management  
❌ **Reference Lifetime**: Must ensure providers outlive consumers  
❌ **Initialization Complexity**: Must wire up all dependencies  

### When to Use

- ✅ Large application with many dependencies (benefits from IoC)
- ✅ Testing is critical (unit tests for all components)
- ✅ Following SOLID principles strictly
- ✅ Multiple provider configurations needed simultaneously

### When NOT to Use

- ❌ Small application (overhead not worth it)
- ❌ Cannot refactor existing codebase extensively
- ❌ Team unfamiliar with dependency injection

---

## Strategy 6: Abstract Factory Pattern

### Description

Create a factory interface that produces related families of providers. Different factory implementations create different sets of providers (e.g., FlatBuffers family, JSON family, Test family).

### Implementation Pattern

**Abstract Factory Interface**:
```cpp
// src/data_providers/IProviderFactory.h
namespace steamrot {

class IProviderFactory {
public:
  virtual ~IProviderFactory() = default;
  
  // Create methods for each provider type
  virtual std::unique_ptr<ISceneDataProvider> 
  CreateSceneDataProvider() const = 0;
  
  virtual std::unique_ptr<IEngineDataProvider> 
  CreateEngineDataProvider() const = 0;
  
  virtual std::unique_ptr<IAssetDataProvider> 
  CreateAssetDataProvider() const = 0;
  
  virtual std::unique_ptr<ISceneConfigurator> 
  CreateSceneConfigurator() const = 0;
};

} // namespace steamrot
```

**Concrete Factory: FlatBuffers Family**:
```cpp
// src/data_providers/FlatbuffersProviderFactory.h
namespace steamrot {

class FlatbuffersProviderFactory : public IProviderFactory {
public:
  std::unique_ptr<ISceneDataProvider> 
  CreateSceneDataProvider() const override {
    return std::make_unique<FlatbuffersSceneDataProvider>();
  }
  
  std::unique_ptr<IEngineDataProvider> 
  CreateEngineDataProvider() const override {
    return std::make_unique<FlatbuffersEngineDataProvider>();
  }
  
  std::unique_ptr<IAssetDataProvider> 
  CreateAssetDataProvider() const override {
    return std::make_unique<FlatbuffersAssetDataProvider>();
  }
  
  std::unique_ptr<ISceneConfigurator> 
  CreateSceneConfigurator() const override {
    return std::make_unique<FlatbuffersSceneConfigurator>();
  }
};

} // namespace steamrot
```

**Concrete Factory: Test Family**:
```cpp
// src/data_providers/TestProviderFactory.h
namespace steamrot {

class TestProviderFactory : public IProviderFactory {
public:
  std::unique_ptr<ISceneDataProvider> 
  CreateSceneDataProvider() const override {
    return std::make_unique<TestSceneDataProvider>();
  }
  
  std::unique_ptr<IEngineDataProvider> 
  CreateEngineDataProvider() const override {
    return std::make_unique<TestEngineDataProvider>();
  }
  
  std::unique_ptr<IAssetDataProvider> 
  CreateAssetDataProvider() const override {
    return std::make_unique<TestAssetDataProvider>();
  }
  
  std::unique_ptr<ISceneConfigurator> 
  CreateSceneConfigurator() const override {
    return std::make_unique<TestSceneConfigurator>();
  }
};

} // namespace steamrot
```

**Factory Selection**:
```cpp
// src/data_providers/provider_factory_selector.cpp
namespace steamrot {

std::unique_ptr<IProviderFactory> 
CreateProviderFactory(ProviderFamilyType family) {
  switch (family) {
  case ProviderFamilyType::FlatBuffers:
    return std::make_unique<FlatbuffersProviderFactory>();
  case ProviderFamilyType::JSON:
    return std::make_unique<JsonProviderFactory>();
  case ProviderFamilyType::Test:
    return std::make_unique<TestProviderFactory>();
  default:
    return std::make_unique<FlatbuffersProviderFactory>();
  }
}

} // namespace steamrot
```

**Usage in main()**:
```cpp
int main() {
  // Select factory based on configuration
  ProviderFamilyType family = LoadProviderFamilyConfig();
  auto factory = CreateProviderFactory(family);
  
  // Create all providers from same family
  auto scene_provider = factory->CreateSceneDataProvider();
  auto engine_provider = factory->CreateEngineDataProvider();
  auto asset_provider = factory->CreateAssetDataProvider();
  auto configurator = factory->CreateSceneConfigurator();
  
  // Use providers
  // ...
}
```

**Testing Usage**:
```cpp
TEST_CASE("Application works with test providers", "[Integration]") {
  // Create test factory
  auto factory = std::make_unique<TestProviderFactory>();
  
  // All providers are test implementations
  auto scene_provider = factory->CreateSceneDataProvider();
  auto engine_provider = factory->CreateEngineDataProvider();
  // ...
  
  // Run application logic
}
```

### Pros

✅ **Consistency**: Ensures related providers are from same family  
✅ **Encapsulation**: Provider creation logic is encapsulated  
✅ **Easy Swapping**: Change entire family by changing factory  
✅ **Type Safety**: Compiler ensures all factory methods are implemented  
✅ **Open/Closed**: Add new families without modifying existing code  

### Cons

❌ **Rigid Grouping**: All providers must be from same family (can't mix)  
❌ **Interface Bloat**: Factory interface grows with each provider type  
❌ **Boilerplate**: Must implement all methods for each concrete factory  
❌ **Not Granular**: Cannot select providers individually  

### When to Use

- ✅ Providers naturally group into families (FlatBuffers, JSON, XML)
- ✅ Want consistency (all providers from same format)
- ✅ Switching entire provider ecosystem at once
- ✅ Following Gang of Four design patterns

### When NOT to Use

- ❌ Need to mix providers from different families
- ❌ Provider types change frequently (interface must change)
- ❌ Only one or two provider types (overhead not worth it)

---

## Comparative Analysis

### Summary Table

| Strategy | Decoupling | Testability | Extensibility | Runtime Config | Complexity | Bootstrap Safe |
|----------|:---------:|:-----------:|:-------------:|:--------------:|:----------:|:--------------:|
| **Configuration File** | ⚠️ Medium | ⚠️ Medium | ✅ Good | ✅ Excellent | ⚠️ Medium | ❌ Circular |
| **Environment Variable** | ⚠️ Medium | ✅ Good | ⚠️ Medium | ✅ Excellent | ✅ Low | ✅ Yes |
| **Factory Parameter** | ✅ Good | ✅✅ Excellent | ✅ Good | ⚠️ Medium | ✅ Low | ✅ Yes |
| **Registry Pattern** | ✅ Good | ✅ Good | ✅✅ Excellent | ✅ Excellent | ❌ High | ⚠️ Init Order |
| **Dependency Injection** | ✅✅ Excellent | ✅✅ Excellent | ✅ Good | ⚠️ Medium | ❌ High | ✅ Yes |
| **Abstract Factory** | ✅ Good | ✅✅ Excellent | ✅ Good | ⚠️ Medium | ⚠️ Medium | ✅ Yes |

### Use Case Recommendations

| Use Case | Best Strategy | Alternative |
|----------|---------------|-------------|
| **Production Runtime Config** | Configuration File | Environment Variable |
| **Automated Testing** | Dependency Injection | Factory Parameter |
| **Plugin Architecture** | Registry Pattern | Abstract Factory |
| **Simple Applications** | Environment Variable | Factory Parameter |
| **Large Codebases** | Dependency Injection | Abstract Factory |
| **Bootstrap/Early Init** | Environment Variable | Factory Parameter |
| **Mixing Providers** | Dependency Injection | Factory Parameter |
| **Consistent Families** | Abstract Factory | Configuration File |

---

## Recommendations for SteamRot

### Current State Assessment

**Problems**:
1. ❌ Hard-coded FlatBuffers in `provider_factory.cpp`
2. ❌ Hard-coded configurator in `SceneFactory.cpp`
3. ❌ Testing requires production providers
4. ❌ Cannot support save files without code changes
5. ❌ Global static singletons with unclear lifetimes

### Recommended Approach: Hybrid Strategy

Use **Environment Variable** for simple cases + **Dependency Injection** for testing.

#### Phase 1: Quick Win - Environment Variables (1-2 days)

Replace hard-coded providers with environment variable selection:

```cpp
// src/data_providers/provider_factory.cpp
ISceneDataProvider& GetSceneDataProvider() {
  static std::unique_ptr<ISceneDataProvider> provider;
  
  if (!provider) {
    const char* provider_type = std::getenv("STEAMROT_SCENE_PROVIDER");
    std::string type_str = provider_type ? provider_type : "flatbuffers";
    
    if (type_str == "test") {
      provider = std::make_unique<TestSceneDataProvider>();
    } else if (type_str == "json") {
      provider = std::make_unique<JsonSceneDataProvider>();
    } else {
      provider = std::make_unique<FlatbuffersSceneDataProvider>();
    }
  }
  
  return *provider;
}
```

**Benefits**:
- ✅ Minimal code change
- ✅ Testing can set environment variables
- ✅ No bootstrap problem
- ✅ Works immediately

#### Phase 2: Dependency Injection for Testing (1-2 weeks)

Refactor consumers to accept providers via constructor:

**Before**:
```cpp
class SceneFactory {
public:
  SceneFactory(const GameContext& game_context);
  // Uses GetSceneDataProvider() internally
};
```

**After**:
```cpp
class SceneFactory {
private:
  ISceneDataProvider& m_scene_data_provider;
  ISceneConfigurator& m_scene_configurator;
  
public:
  // Dependency injection
  SceneFactory(const GameContext& game_context,
               ISceneDataProvider& scene_data_provider,
               ISceneConfigurator& scene_configurator);
};
```

**Benefits**:
- ✅ Clear dependencies
- ✅ Easy testing (inject mocks)
- ✅ No global state in tests
- ✅ Follows SOLID principles

#### Phase 3: Configuration File (Optional, 1 week)

Add JSON configuration for production deployments:

```json
{
  "providers": {
    "scene_data": "flatbuffers",
    "engine_data": "flatbuffers",
    "asset_data": "flatbuffers"
  }
}
```

**Benefits**:
- ✅ User-configurable
- ✅ Environment-specific configs
- ✅ No recompilation needed

### Migration Priority

1. **High Priority**: Fix hard-coded providers in `provider_factory.cpp`
2. **High Priority**: Fix hard-coded configurator in `SceneFactory.cpp`
3. **Medium Priority**: Refactor SceneFactory to use dependency injection
4. **Medium Priority**: Refactor AssetManager to use dependency injection
5. **Low Priority**: Add configuration file support
6. **Future**: Add registry pattern for plugin support

---

## Migration Path

### Step 1: Add Provider Type Enum

```cpp
// src/types/core/ProviderType.h
namespace steamrot {

enum class ProviderType {
  FlatBuffers,
  JSON,
  XML,
  SaveFile,
  Test
};

// Helper function to parse from string
ProviderType ParseProviderType(const std::string& str);

} // namespace steamrot
```

### Step 2: Update Factory Functions

```cpp
// src/data_providers/provider_factory.cpp

// Helper: Get provider type from environment
static ProviderType GetProviderTypeFromEnv(const std::string& env_var,
                                           ProviderType default_type) {
  const char* value = std::getenv(env_var.c_str());
  if (!value) return default_type;
  return ParseProviderType(value);
}

// Updated factory function
ISceneDataProvider& GetSceneDataProvider() {
  static std::unique_ptr<ISceneDataProvider> provider;
  
  if (!provider) {
    ProviderType type = GetProviderTypeFromEnv(
        "STEAMROT_SCENE_PROVIDER",
        ProviderType::FlatBuffers);
    
    provider = CreateSceneDataProviderImpl(type);
  }
  
  return *provider;
}

// Create helper (internal)
static std::unique_ptr<ISceneDataProvider>
CreateSceneDataProviderImpl(ProviderType type) {
  switch (type) {
  case ProviderType::FlatBuffers:
    return std::make_unique<FlatbuffersSceneDataProvider>();
  case ProviderType::Test:
    return std::make_unique<TestSceneDataProvider>();
  // Add more cases as implementations become available
  default:
    return std::make_unique<FlatbuffersSceneDataProvider>();
  }
}
```

### Step 3: Update Tests

```cpp
// tests/scenes/SceneFactory.test.cpp

class SceneFactoryTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Set environment variable to use test providers
    setenv("STEAMROT_SCENE_PROVIDER", "test", 1);
    setenv("STEAMROT_ENGINE_PROVIDER", "test", 1);
  }
  
  void TearDown() override {
    unsetenv("STEAMROT_SCENE_PROVIDER");
    unsetenv("STEAMROT_ENGINE_PROVIDER");
  }
};

TEST_F(SceneFactoryTest, LoadsFromTestProvider) {
  // Factory will use test provider due to environment variable
  SceneFactory factory(test_context);
  auto scene = factory.CreateSceneFromDefault(SceneType::TITLE);
  EXPECT_TRUE(scene.has_value());
}
```

### Step 4: Refactor SceneFactory (Dependency Injection)

```cpp
// src/scenes/SceneFactory.h
class SceneFactory {
private:
  const GameContext& m_game_context;
  ISceneDataProvider* m_scene_data_provider;     // Owned externally
  ISceneConfigurator* m_scene_configurator;      // Owned externally

public:
  // Constructor with optional provider injection
  SceneFactory(const GameContext& game_context,
               ISceneDataProvider* scene_data_provider = nullptr,
               ISceneConfigurator* scene_configurator = nullptr);
  
  // If providers not injected, uses global factory functions (backward compat)
};
```

```cpp
// src/scenes/SceneFactory.cpp
SceneFactory::SceneFactory(const GameContext& game_context,
                           ISceneDataProvider* scene_data_provider,
                           ISceneConfigurator* scene_configurator)
    : m_game_context(game_context),
      m_scene_data_provider(scene_data_provider),
      m_scene_configurator(scene_configurator) {}

std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateSceneFromDefault(SceneType type) {
  // Use injected provider if available, otherwise use global factory
  ISceneDataProvider& provider = m_scene_data_provider 
      ? *m_scene_data_provider 
      : GetSceneDataProvider();
  
  ISceneConfigurator& configurator = m_scene_configurator
      ? *m_scene_configurator
      : GetSceneConfigurator();
  
  // Rest of implementation...
}
```

### Step 5: Update Tests to Use Injection

```cpp
TEST_CASE("Scene loads from injected test provider", "[SceneFactory]") {
  // Create test providers
  TestSceneDataProvider test_provider;
  TestSceneConfigurator test_configurator;
  
  // Inject into factory
  SceneFactory factory(test_context, &test_provider, &test_configurator);
  
  auto scene = factory.CreateSceneFromDefault(SceneType::TITLE);
  REQUIRE(scene.has_value());
}
```

---

## Implementation Examples

### Complete Example: Provider Container

```cpp
// src/data_providers/ProviderContainer.h
namespace steamrot {

class ProviderContainer {
private:
  std::unique_ptr<ISceneDataProvider> m_scene_data_provider;
  std::unique_ptr<IEngineDataProvider> m_engine_data_provider;
  std::unique_ptr<IAssetDataProvider> m_asset_data_provider;
  std::unique_ptr<ISceneConfigurator> m_scene_configurator;

public:
  ProviderContainer() = default;
  
  // Move-only (owns providers)
  ProviderContainer(ProviderContainer&&) = default;
  ProviderContainer& operator=(ProviderContainer&&) = default;
  ProviderContainer(const ProviderContainer&) = delete;
  ProviderContainer& operator=(const ProviderContainer&) = delete;
  
  // Factory methods
  static ProviderContainer CreateFromEnvironment();
  static ProviderContainer CreateForTesting();
  static ProviderContainer CreateDefault();
  
  // Setters (for custom configuration)
  void SetSceneDataProvider(std::unique_ptr<ISceneDataProvider> provider) {
    m_scene_data_provider = std::move(provider);
  }
  
  void SetEngineDataProvider(std::unique_ptr<IEngineDataProvider> provider) {
    m_engine_data_provider = std::move(provider);
  }
  
  void SetAssetDataProvider(std::unique_ptr<IAssetDataProvider> provider) {
    m_asset_data_provider = std::move(provider);
  }
  
  void SetSceneConfigurator(std::unique_ptr<ISceneConfigurator> configurator) {
    m_scene_configurator = std::move(configurator);
  }
  
  // Getters (return references to owned providers)
  ISceneDataProvider& GetSceneDataProvider() { 
    assert(m_scene_data_provider);
    return *m_scene_data_provider; 
  }
  
  IEngineDataProvider& GetEngineDataProvider() { 
    assert(m_engine_data_provider);
    return *m_engine_data_provider; 
  }
  
  IAssetDataProvider& GetAssetDataProvider() { 
    assert(m_asset_data_provider);
    return *m_asset_data_provider; 
  }
  
  ISceneConfigurator& GetSceneConfigurator() { 
    assert(m_scene_configurator);
    return *m_scene_configurator; 
  }
};

} // namespace steamrot
```

```cpp
// src/data_providers/ProviderContainer.cpp
namespace steamrot {

ProviderContainer ProviderContainer::CreateFromEnvironment() {
  ProviderContainer container;
  
  // Read environment variables
  auto scene_type = GetProviderTypeFromEnv("STEAMROT_SCENE_PROVIDER", 
                                           ProviderType::FlatBuffers);
  auto engine_type = GetProviderTypeFromEnv("STEAMROT_ENGINE_PROVIDER", 
                                            ProviderType::FlatBuffers);
  auto asset_type = GetProviderTypeFromEnv("STEAMROT_ASSET_PROVIDER", 
                                           ProviderType::FlatBuffers);
  
  // Create providers based on environment
  container.m_scene_data_provider = CreateSceneDataProviderImpl(scene_type);
  container.m_engine_data_provider = CreateEngineDataProviderImpl(engine_type);
  container.m_asset_data_provider = CreateAssetDataProviderImpl(asset_type);
  container.m_scene_configurator = CreateSceneConfiguratorImpl(scene_type);
  
  return container;
}

ProviderContainer ProviderContainer::CreateForTesting() {
  ProviderContainer container;
  container.m_scene_data_provider = std::make_unique<TestSceneDataProvider>();
  container.m_engine_data_provider = std::make_unique<TestEngineDataProvider>();
  container.m_asset_data_provider = std::make_unique<TestAssetDataProvider>();
  container.m_scene_configurator = std::make_unique<TestSceneConfigurator>();
  return container;
}

ProviderContainer ProviderContainer::CreateDefault() {
  ProviderContainer container;
  container.m_scene_data_provider = 
      std::make_unique<FlatbuffersSceneDataProvider>();
  container.m_engine_data_provider = 
      std::make_unique<FlatbuffersEngineDataProvider>();
  container.m_asset_data_provider = 
      std::make_unique<FlatbuffersAssetDataProvider>();
  container.m_scene_configurator = 
      std::make_unique<FlatbuffersSceneConfigurator>();
  return container;
}

} // namespace steamrot
```

**Usage**:
```cpp
// In main()
int main() {
  // Create provider container from environment
  auto providers = ProviderContainer::CreateFromEnvironment();
  
  // Create game context
  GameContext game_context(/* ... */);
  
  // Inject providers into factories
  SceneFactory scene_factory(
      game_context,
      &providers.GetSceneDataProvider(),
      &providers.GetSceneConfigurator());
  
  // Providers are used transparently
  auto scene = scene_factory.CreateSceneFromDefault(SceneType::TITLE);
}

// In tests
TEST_CASE("Testing with test providers") {
  auto providers = ProviderContainer::CreateForTesting();
  
  TestGameContext test_context;
  SceneFactory factory(
      test_context,
      &providers.GetSceneDataProvider(),
      &providers.GetSceneConfigurator());
  
  auto scene = factory.CreateSceneFromDefault(SceneType::TITLE);
  REQUIRE(scene.has_value());
}
```

---

## Conclusion

Selecting concrete provider and configurator implementations is fundamentally different from switching on data types after loading. The key insights:

1. **Factory Selection is Architectural**: It's about wiring up the application, not processing data
2. **Use Different Strategies for Different Contexts**:
   - **Production**: Configuration File or Environment Variable
   - **Testing**: Dependency Injection or Factory Parameter
   - **Plugins**: Registry Pattern or Abstract Factory
3. **Hybrid Approach Works Best**: Environment variables for quick wins, dependency injection for long-term maintainability
4. **Avoid Global Hard-Coding**: Current approach is the worst option - brittle and untestable

### Recommended Migration for SteamRot

**Immediate** (this week):
- Replace hard-coded providers with environment variable selection
- Enables testing without code modification

**Short-term** (next sprint):
- Refactor SceneFactory and AssetManager to accept provider injection
- Update tests to inject test providers

**Long-term** (future):
- Add configuration file support for production
- Consider registry pattern if plugin architecture is needed

The goal is not perfect architecture, but pragmatic improvement: making the codebase more testable and extensible without massive refactoring.

---

## References

### SteamRot Source Files
- `src/data_providers/provider_factory.cpp` - Hard-coded providers (current problem)
- `src/scenes/SceneFactory.cpp` - Hard-coded configurator (line 54)
- `src/interfaces/ISceneDataProvider.h` - Provider interface
- `src/interfaces/ISceneConfigurator.h` - Configurator interface

### Design Patterns
- [Factory Method Pattern](https://refactoring.guru/design-patterns/factory-method)
- [Abstract Factory Pattern](https://refactoring.guru/design-patterns/abstract-factory)
- [Strategy Pattern](https://refactoring.guru/design-patterns/strategy)
- [Registry Pattern](https://martinfowler.com/eaaCatalog/registry.html)
- [Dependency Injection](https://martinfowler.com/articles/injection.html)

### Related Documents
- [SWITCHING_STRATEGIES_ANALYSIS.md](SWITCHING_STRATEGIES_ANALYSIS.md) - For switching on data types after loading

---

**Document Version**: 1.0  
**Last Updated**: 2025-12-17  
**Author**: SteamRot Architecture Team  
**Status**: Analysis Complete - Implementation Pending
