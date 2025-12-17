# Factory Selection Strategies - Quick Reference

## Problem: Hard-Coded Provider Implementations

**Current Issue in SteamRot**:
```cpp
// provider_factory.cpp - HARD-CODED!
ISceneDataProvider& GetSceneDataProvider() {
  static FlatbuffersSceneDataProvider provider;  // ❌ Can't change
  return provider;
}
```

**Cannot Support**:
- ❌ Test data providers for testing
- ❌ Save file providers for gameplay
- ❌ JSON providers for prototyping
- ❌ Multiple formats simultaneously

---

## Decision Tree

```
Q: Do you need to change providers at runtime (after app start)?
├─ YES → Do you need plugin support (third-party providers)?
│   ├─ YES → Use REGISTRY PATTERN (#4)
│   └─ NO → Use DEPENDENCY INJECTION (#5)
└─ NO (only change at startup) → Is configuration file bootstrap a problem?
    ├─ YES (circular dependency) → Use ENVIRONMENT VARIABLE (#2) ⭐
    └─ NO → Do you need testing flexibility?
        ├─ YES → Use DEPENDENCY INJECTION (#5) + Config File (#1) ⭐⭐
        └─ NO → Use CONFIGURATION FILE (#1)
```

---

## Strategy Cheat Sheet

### 1️⃣ Configuration File

```cpp
// Load config at startup
InitializeProviderFactory(config);

// Factory uses config
ISceneDataProvider& provider = GetSceneDataProvider();
```

✅ **Use When**: Runtime config without recompiling, user-editable settings  
❌ **Avoid When**: Bootstrap/circular dependency, testing needs frequent changes  
⚡ **Complexity**: ★★★☆☆ (Medium)  
🧪 **Testability**: ★★☆☆☆ (Requires global state manipulation)

---

### 2️⃣ Environment Variable ⭐ Quick Win

```bash
export STEAMROT_SCENE_PROVIDER=test
export STEAMROT_ENGINE_PROVIDER=test
```

```cpp
ISceneDataProvider& GetSceneDataProvider() {
  const char* type = getenv("STEAMROT_SCENE_PROVIDER");
  // Create provider based on type
}
```

✅ **Use When**: CI/CD, testing, no bootstrap problem, quick fix  
❌ **Avoid When**: End-users need configuration, complex settings  
⚡ **Complexity**: ★★☆☆☆ (Low)  
🧪 **Testability**: ★★★★☆ (Easy - set in test fixtures)

---

### 3️⃣ Factory Method Parameter

```cpp
// Pass provider type as parameter
auto provider = CreateSceneDataProvider(ProviderType::Test);

// Or inject into constructor
SceneFactory factory(game_context, ProviderType::Test);
```

✅ **Use When**: Fine-grained control, different parts need different providers  
❌ **Avoid When**: Many call sites, want centralized configuration  
⚡ **Complexity**: ★★☆☆☆ (Low)  
🧪 **Testability**: ★★★★★ (Excellent - pass test type directly)

---

### 4️⃣ Registry Pattern

```cpp
// Register providers at startup
registry.RegisterSceneDataProvider("custom", []() {
  return std::make_unique<CustomProvider>();
});

// Create by name
auto provider = registry.CreateSceneDataProvider("custom");
```

✅ **Use When**: Plugin architecture, runtime provider discovery, DLLs  
❌ **Avoid When**: All providers known at compile-time, simple application  
⚡ **Complexity**: ★★★★☆ (High)  
🧪 **Testability**: ★★★☆☆ (Requires registry manipulation)

---

### 5️⃣ Dependency Injection ⭐⭐ Recommended Long-Term

```cpp
// Inject provider into constructor
class SceneFactory {
  ISceneDataProvider& m_provider;  // Injected
public:
  SceneFactory(GameContext& ctx, ISceneDataProvider& provider)
      : m_provider(provider) {}
};

// Usage
FlatbuffersSceneDataProvider prod_provider;
SceneFactory factory(game_context, prod_provider);

// Testing
TestSceneDataProvider test_provider;
SceneFactory test_factory(test_context, test_provider);
```

✅ **Use When**: Testing critical, SOLID principles, large codebase  
❌ **Avoid When**: Small app, cannot refactor extensively  
⚡ **Complexity**: ★★★★☆ (High initial refactoring)  
🧪 **Testability**: ★★★★★ (Excellent - inject mocks easily)

---

### 6️⃣ Abstract Factory

```cpp
// Factory interface
class IProviderFactory {
public:
  virtual std::unique_ptr<ISceneDataProvider> 
  CreateSceneDataProvider() = 0;
};

// Concrete factories
class FlatbuffersProviderFactory : public IProviderFactory { };
class TestProviderFactory : public IProviderFactory { };

// Use factory
auto factory = std::make_unique<TestProviderFactory>();
auto provider = factory->CreateSceneDataProvider();
```

✅ **Use When**: Provider families, consistent sets, Gang of Four patterns  
❌ **Avoid When**: Need to mix providers from different families  
⚡ **Complexity**: ★★★☆☆ (Medium)  
🧪 **Testability**: ★★★★★ (Excellent - use test factory)

---

## Comparison Matrix

|  | Config File | Env Var | Parameter | Registry | DI | Abstract Factory |
|---|:-----------:|:-------:|:---------:|:--------:|:--:|:----------------:|
| **Runtime Config** | ✅✅ | ✅✅ | ⚠️ | ✅✅ | ⚠️ | ⚠️ |
| **Testability** | ⚠️ | ✅ | ✅✅ | ✅ | ✅✅ | ✅✅ |
| **No Bootstrap Issue** | ❌ | ✅ | ✅ | ⚠️ | ✅ | ✅ |
| **Plugin Support** | ❌ | ❌ | ❌ | ✅✅ | ⚠️ | ⚠️ |
| **Simple Setup** | ⚠️ | ✅✅ | ✅✅ | ❌ | ❌ | ⚠️ |
| **Clear Dependencies** | ❌ | ❌ | ✅ | ⚠️ | ✅✅ | ✅ |

---

## SteamRot Current Problem

### Hard-Coded Providers

**`provider_factory.cpp`** (lines 20-41):
```cpp
IEngineDataProvider &GetEngineDataProvider() {
  static FlatbuffersEngineDataProvider provider;  // ❌ HARD-CODED
  return provider;
}

ISceneDataProvider &GetSceneDataProvider() {
  static FlatbuffersSceneDataProvider provider;  // ❌ HARD-CODED
  return provider;
}

IAssetDataProvider &GetAssetDataProvider() {
  static FlatbuffersAssetDataProvider provider;  // ❌ HARD-CODED
  return provider;
}
```

**`SceneFactory.cpp`** (line 54):
```cpp
ISceneConfigurator &GetSceneConfigurator() {
  static FlatbuffersSceneConfigurator configurator;  // ❌ HARD-CODED
  return configurator;
}
```

---

## Recommended Solution for SteamRot

### Phase 1: Environment Variables (Immediate - 1 day)

**Quick fix with minimal code change:**

```cpp
// provider_factory.cpp
ISceneDataProvider& GetSceneDataProvider() {
  static std::unique_ptr<ISceneDataProvider> provider;
  
  if (!provider) {
    const char* type_str = getenv("STEAMROT_SCENE_PROVIDER");
    std::string type = type_str ? type_str : "flatbuffers";
    
    if (type == "test") {
      provider = std::make_unique<TestSceneDataProvider>();
    } else if (type == "json") {
      provider = std::make_unique<JsonSceneDataProvider>();
    } else {
      provider = std::make_unique<FlatbuffersSceneDataProvider>();
    }
  }
  
  return *provider;
}
```

**Testing:**
```cpp
// In test setup
setenv("STEAMROT_SCENE_PROVIDER", "test", 1);
```

---

### Phase 2: Dependency Injection (Next sprint - 1-2 weeks)

**Refactor consumers to accept providers:**

```cpp
// SceneFactory.h
class SceneFactory {
private:
  ISceneDataProvider& m_scene_data_provider;  // Injected
  ISceneConfigurator& m_scene_configurator;   // Injected

public:
  SceneFactory(const GameContext& game_context,
               ISceneDataProvider& scene_data_provider,
               ISceneConfigurator& scene_configurator)
      : m_scene_data_provider(scene_data_provider),
        m_scene_configurator(scene_configurator) {}
  
  // No more GetSceneDataProvider() calls!
};
```

**Provider Container:**
```cpp
// ProviderContainer.h
class ProviderContainer {
  std::unique_ptr<ISceneDataProvider> m_scene_provider;
  std::unique_ptr<IEngineDataProvider> m_engine_provider;
  
public:
  static ProviderContainer CreateFromEnvironment();
  static ProviderContainer CreateForTesting();
  
  ISceneDataProvider& GetSceneDataProvider() { return *m_scene_provider; }
};
```

**Usage:**
```cpp
// main.cpp
auto providers = ProviderContainer::CreateFromEnvironment();
SceneFactory factory(game_context, 
                     providers.GetSceneDataProvider(),
                     providers.GetSceneConfigurator());

// test.cpp
auto providers = ProviderContainer::CreateForTesting();
SceneFactory factory(test_context,
                     providers.GetSceneDataProvider(),
                     providers.GetSceneConfigurator());
```

---

## Migration Checklist

### ✅ Phase 1: Environment Variables

- [ ] Add `ProviderType` enum
- [ ] Add `ParseProviderType()` helper
- [ ] Update `GetSceneDataProvider()` to read env var
- [ ] Update `GetEngineDataProvider()` to read env var
- [ ] Update `GetAssetDataProvider()` to read env var
- [ ] Update `GetSceneConfigurator()` to read env var
- [ ] Test with `export STEAMROT_SCENE_PROVIDER=test`

### ✅ Phase 2: Dependency Injection

- [ ] Create `ProviderContainer` class
- [ ] Add `CreateFromEnvironment()` factory method
- [ ] Add `CreateForTesting()` factory method
- [ ] Refactor `SceneFactory` to accept provider injection
- [ ] Refactor `AssetManager` to accept provider injection
- [ ] Update all call sites to inject providers
- [ ] Update tests to use `ProviderContainer::CreateForTesting()`
- [ ] Remove global factory functions (optional - keep for backward compat)

---

## Common Patterns

### Environment Variable Pattern

```cpp
// Helper function
static ProviderType GetProviderTypeFromEnv(
    const std::string& env_var,
    ProviderType default_type) {
  const char* value = std::getenv(env_var.c_str());
  if (!value) return default_type;
  return ParseProviderType(value);
}

// Usage in factory
ISceneDataProvider& GetSceneDataProvider() {
  static std::unique_ptr<ISceneDataProvider> provider;
  
  if (!provider) {
    ProviderType type = GetProviderTypeFromEnv(
        "STEAMROT_SCENE_PROVIDER",
        ProviderType::FlatBuffers);
    provider = CreateProviderImpl(type);
  }
  
  return *provider;
}
```

---

### Dependency Injection Pattern

```cpp
// Container owns providers
class ProviderContainer {
  std::unique_ptr<ISceneDataProvider> m_provider;
  
public:
  // Return reference (container owns, consumers borrow)
  ISceneDataProvider& GetProvider() { return *m_provider; }
};

// Consumer accepts reference
class SceneFactory {
  ISceneDataProvider& m_provider;  // Non-owning reference
  
public:
  SceneFactory(ISceneDataProvider& provider)
      : m_provider(provider) {}  // Borrow
};

// Usage: Container outlives consumers
ProviderContainer container;  // Owns providers
SceneFactory factory(container.GetProvider());  // Borrows
```

---

## Testing Examples

### Environment Variable Testing

```cpp
class ProviderFactoryTest : public ::testing::Test {
protected:
  void SetUp() override {
    setenv("STEAMROT_SCENE_PROVIDER", "test", 1);
  }
  
  void TearDown() override {
    unsetenv("STEAMROT_SCENE_PROVIDER");
  }
};

TEST_F(ProviderFactoryTest, UsesTestProvider) {
  ISceneDataProvider& provider = GetSceneDataProvider();
  // Provider is now TestSceneDataProvider
}
```

---

### Dependency Injection Testing

```cpp
TEST_CASE("Scene loads from test provider", "[SceneFactory]") {
  // Create test provider
  TestSceneDataProvider test_provider;
  TestSceneConfigurator test_configurator;
  
  // Inject into factory
  SceneFactory factory(test_context, test_provider, test_configurator);
  
  // Test uses injected test providers
  auto scene = factory.CreateSceneFromDefault(SceneType::TITLE);
  REQUIRE(scene.has_value());
}
```

---

## Common Pitfalls

### ❌ DON'T: Keep global static singletons

```cpp
// Bad: Static lifetime, can't change after first call
ISceneDataProvider& GetSceneDataProvider() {
  static FlatbuffersSceneDataProvider provider;  // Forever FlatBuffers!
  return provider;
}
```

### ✅ DO: Create on demand with configuration

```cpp
// Good: Checks configuration before creating
ISceneDataProvider& GetSceneDataProvider() {
  static std::unique_ptr<ISceneDataProvider> provider;
  if (!provider) {
    auto type = GetConfiguredProviderType();
    provider = CreateProviderImpl(type);
  }
  return *provider;
}
```

---

### ❌ DON'T: Hide dependencies

```cpp
// Bad: Hidden dependency on global factory
class SceneFactory {
public:
  void CreateScene() {
    auto& provider = GetSceneDataProvider();  // Hidden!
  }
};
```

### ✅ DO: Make dependencies explicit

```cpp
// Good: Dependency visible in constructor
class SceneFactory {
  ISceneDataProvider& m_provider;
public:
  SceneFactory(ISceneDataProvider& provider)  // Explicit!
      : m_provider(provider) {}
};
```

---

### ❌ DON'T: Circular configuration dependency

```cpp
// Bad: Config needs provider, provider needs config
ISceneDataProvider& GetProvider() {
  auto config = LoadConfig();  // Uses provider!
  return CreateProvider(config);  // Circular!
}
```

### ✅ DO: Use bootstrap-safe configuration

```cpp
// Good: Environment variable has no dependencies
ISceneDataProvider& GetProvider() {
  const char* type = getenv("STEAMROT_PROVIDER");  // No file I/O!
  return CreateProvider(ParseType(type));
}
```

---

## Quick Decision Guide

### I need to...

**Support test data in tests**  
→ Use **Environment Variable** (#2) or **Dependency Injection** (#5)

**Support save files at runtime**  
→ Use **Configuration File** (#1) or **Factory Parameter** (#3)

**Allow plugins to add providers**  
→ Use **Registry Pattern** (#4)

**Make testing easier**  
→ Use **Dependency Injection** (#5)

**Quick fix without refactoring**  
→ Use **Environment Variable** (#2)

**Long-term maintainable solution**  
→ Use **Dependency Injection** (#5) + **Configuration File** (#1)

---

## Full Analysis

For complete details, see:
- **[FACTORY_SELECTION_STRATEGIES_ANALYSIS.md](FACTORY_SELECTION_STRATEGIES_ANALYSIS.md)** - Full 49KB analysis with implementation examples

---

**Quick Reference Version**: 1.0  
**Last Updated**: 2025-12-17  
**For**: Provider/Configurator factory selection (not data type switching)
