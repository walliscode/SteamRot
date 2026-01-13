# Quick Reference: Native Object Creation Pattern

This is a quick reference guide for the proposed template-based configurator pattern.

## TL;DR

**Problem:** Adding new data types requires modifying switch statements in `DataAccessFactory`

**Solution:** Template-based interfaces + registry specialization = compile-time dispatch, no switches

**Benefit:** Add new data type with single template specialization, no changes to existing code

## Core Pattern (Copy This)

### 1. Generic Interface

```cpp
template<typename DataType, typename ObjectType>
class IConfigurator {
public:
  virtual ~IConfigurator() = default;
  virtual std::expected<std::monostate, FailInfo>
  Configure(ObjectType& object, const DataType& data) = 0;
};
```

### 2. Concrete Implementation

```cpp
class FlatbuffersSceneConfigurator 
    : public IConfigurator<SceneDataFbs, Scene> {
public:
  std::expected<std::monostate, FailInfo>
  Configure(Scene& scene, const SceneDataFbs& data) override {
    // Type-safe access to SceneDataFbs
    // No casting, no type erasure
    return std::monostate{};
  }
};
```

### 3. Registry Specialization (ONLY coupling point)

```cpp
template<DataSourceType Source, typename Data, typename Object>
struct ConfiguratorRegistry;  // No default

template<>
struct ConfiguratorRegistry<DataSourceType::Flatbuffers, 
                           SceneDataFbs, Scene> {
  static auto Create(EventHandler& eh, Factory& factory) {
    return std::make_unique<FlatbuffersSceneConfigurator>(eh, factory);
  }
};
```

### 4. Factory

```cpp
template<DataSourceType SourceType>
class ConfiguratorFactory {
public:
  template<typename DataType, typename ObjectType>
  std::expected<IConfigurator<DataType, ObjectType>*, FailInfo>
  GetConfigurator() {
    // Uses registry - no switch statement
    auto instance = ConfiguratorRegistry<SourceType, DataType, ObjectType>
        ::Create(m_event_handler, *this);
    // Cache and return
    return instance.get();
  }
};
```

### 5. Usage

```cpp
ConfiguratorFactory<DataSourceType::Flatbuffers> factory(event_handler);
auto configurator = factory.GetConfigurator<SceneDataFbs, Scene>();
configurator.value()->Configure(scene, data);
```

## Adding New Data Type (4 Steps)

### Step 1: Define Data Structure
```cpp
struct JSONSceneData {
  std::string scene_name;
  // ... fields
};
```

### Step 2: Create Configurator
```cpp
class JSONSceneConfigurator : public IConfigurator<JSONSceneData, Scene> {
  std::expected<std::monostate, FailInfo>
  Configure(Scene& scene, const JSONSceneData& data) override {
    scene.SetName(data.scene_name);
    return std::monostate{};
  }
};
```

### Step 3: Register
```cpp
template<>
struct ConfiguratorRegistry<DataSourceType::JSON, JSONSceneData, Scene> {
  static auto Create(...) { 
    return std::make_unique<JSONSceneConfigurator>(...);
  }
};
```

### Step 4: Use It
```cpp
ConfiguratorFactory<DataSourceType::JSON> factory(event_handler);
auto configurator = factory.GetConfigurator<JSONSceneData, Scene>();
// Done! No other code changes needed
```

## Common Patterns

### Pattern 1: Composition (A contains B)

```cpp
class SceneConfigurator {
private:
  ConfiguratorFactory<DataSourceType>& m_factory;

public:
  std::expected<std::monostate, FailInfo>
  Configure(Scene& scene, const SceneDataFbs& data) override {
    
    // Get configurator for nested object B
    auto entity_config = m_factory.GetConfigurator<
        EntityCollectionFbs, EntityMemoryPool>();
    
    // Use it
    entity_config.value()->Configure(
        scene.GetEntityMemoryPool(),
        *data.entity_collection());
  }
};
```

### Pattern 2: Free Functions for Simple Types

```cpp
// Free, stateless function
std::expected<std::monostate, FailInfo>
ConfigureSceneInfo(SceneInfo& info, const SceneInfoFbs* fb_info) {
  if (!fb_info) return std::unexpected(...);
  info.type = fb_info->type();
  return std::monostate{};
}

// Call from any configurator
auto result = ConfigureSceneInfo(scene.GetSceneInfo(), data.scene_info());
```

### Pattern 3: Shared Object Configurator

```cpp
// EntityMemoryPool configurator registered once
template<>
struct ConfiguratorRegistry<Flatbuffers, EntityCollectionFbs, EntityMemoryPool> {
  static auto Create(...) { return std::make_unique<EntityConfigurator>(...); }
};

// Used by Scene, SaveFile, Level - no duplication
class SceneConfigurator {
  auto entity_config = m_factory.GetConfigurator<EntityData, EntityMemoryPool>();
  entity_config->Configure(scene.GetEntities(), data.entities());
};

class SaveFileConfigurator {
  auto entity_config = m_factory.GetConfigurator<EntityData, EntityMemoryPool>();
  entity_config->Configure(save.GetEntities(), data.entities());
};
```

## Key Decisions

| Decision | Reason |
|----------|--------|
| Templates for interfaces | Avoid type erasure, compile-time type safety |
| Registry specialization | Confine coupling to one place |
| Factory caching | Performance, manage lifecycles |
| Free functions for simple types | No interface overhead |
| Composition for nested objects | Code reuse, single responsibility |

## When to Use Each Approach

### Use IConfigurator Interface When:
- ✅ Object is standalone (Scene, Entity, UIStyle)
- ✅ Object is reused by multiple parents
- ✅ Configuration logic is complex
- ✅ Need to swap implementations (testing, data source)

### Use Free Functions When:
- ✅ Type is nested/dependent (SceneInfo, SceneState)
- ✅ Configuration is simple
- ✅ No runtime polymorphism needed
- ✅ Want to avoid interface overhead

## Comparison

| Approach | Current | Proposed |
|----------|---------|----------|
| Add data type | Modify switch in factory | Add registry specialization |
| Coupling | Scattered | Explicit (registry only) |
| Type safety | Mixed | Full (no type erasure) |
| Dispatch | Runtime switch | Compile-time template |
| Code reuse | Limited | High (composition) |

## Benefits Summary

1. ✅ **No switch statements** - compile-time dispatch
2. ✅ **No type erasure** - full type information preserved
3. ✅ **Easy to extend** - add specialization, done
4. ✅ **Clear coupling** - registry only
5. ✅ **Testable** - can mock factory
6. ✅ **Reusable** - composition + free functions
7. ✅ **Type safe** - compile-time checking
8. ✅ **Fast** - no runtime overhead

## Complete Documentation

For detailed information, see:

- **[README.md](README.md)** - Documentation navigation
- **[ARCHITECTURE_SUMMARY.md](ARCHITECTURE_SUMMARY.md)** - Executive summary
- **[NATIVE_OBJECT_CREATION_ARCHITECTURE.md](NATIVE_OBJECT_CREATION_ARCHITECTURE.md)** - Detailed design
- **[ADDRESSING_SPECIFIC_QUESTIONS.md](ADDRESSING_SPECIFIC_QUESTIONS.md)** - Q&A format
- **[IMPLEMENTATION_GUIDE.md](IMPLEMENTATION_GUIDE.md)** - Step-by-step instructions
- **[examples/CONFIGURATOR_PATTERN_EXAMPLES.md](examples/CONFIGURATOR_PATTERN_EXAMPLES.md)** - Working examples

## Cheat Sheet

```cpp
// 1. Interface
template<typename Data, typename Object>
class IConfigurator { virtual Configure(Object&, const Data&) = 0; };

// 2. Implementation
class ConcreteConfig : public IConfigurator<DataFbs, Object> { /* ... */ };

// 3. Registry (ONLY coupling point)
template<>
struct ConfiguratorRegistry<Flatbuffers, DataFbs, Object> {
  static auto Create(...) { return std::make_unique<ConcreteConfig>(...); }
};

// 4. Factory
ConfiguratorFactory<DataSourceType::Flatbuffers> factory(event_handler);

// 5. Usage
auto config = factory.GetConfigurator<DataFbs, Object>();
config->Configure(object, data);
```

## Migration Checklist

- [ ] Create template infrastructure (IConfigurator, Registry, Factory)
- [ ] Migrate one type (e.g., Scene) to test pattern
- [ ] Verify all tests pass
- [ ] Migrate remaining types incrementally
- [ ] Remove old switch-based factory
- [ ] Update team documentation

## Questions?

See full documentation in `/documentation/` directory.
