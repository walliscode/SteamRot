# Native Object Creation Architecture - Executive Summary

## Purpose

This document provides an executive summary of the proposed generalized native object creation and configuration architecture for SteamRot.

## Problem Statement

The current architecture for creating and configuring native objects from external data has several limitations:

1. **Tight coupling**: Adding new data types requires modifying switch statements in `DataAccessFactory`
2. **Mixed abstractions**: Some interfaces use concrete types, others use agnostic types
3. **Limited reusability**: Configuration logic for shared objects is duplicated
4. **Unclear coupling points**: It's not obvious where data type coupling is allowed

## Proposed Solution

A **template-based configuration architecture** that:
- Uses compile-time type dispatch instead of runtime switching
- Confines coupling to explicit registry specializations
- Promotes code reuse through composition and free functions
- Maintains full type safety without type erasure

## Architecture Overview

### Core Components

1. **Generic Interfaces** (template-based)
   ```cpp
   template<typename DataType, typename ObjectType>
   class IConfigurator {
     virtual std::expected<std::monostate, FailInfo>
     Configure(ObjectType& object, const DataType& data) = 0;
   };
   ```

2. **Configurator Registry** (template specialization)
   ```cpp
   template<DataSourceType Source, typename Data, typename Object>
   struct ConfiguratorRegistry {
     static std::unique_ptr<IConfigurator<Data, Object>> 
     Create(EventHandler& eh, Factory& factory);
   };
   ```

3. **Configurator Factory** (caching and dependency injection)
   ```cpp
   template<DataSourceType SourceType>
   class ConfiguratorFactory {
     template<typename DataType, typename ObjectType>
     std::expected<IConfigurator<DataType, ObjectType>*, FailInfo>
     GetConfigurator();
   };
   ```

### Data Flow

```
User Code
   ↓
ConfiguratorFactory<DataSourceType::Flatbuffers>
   ↓
ConfiguratorRegistry<Flatbuffers, SceneDataFbs, Scene>  ← Only coupling point
   ↓
FlatbuffersSceneConfigurator (knows concrete data type)
   ↓
Scene (configured object)
```

## Key Design Decisions

### 1. Templates Instead of Switch Statements

**Current Approach:**
```cpp
switch (m_data_type) {
  case DataType::Flatbuffers:
    m_configurator = std::make_unique<FlatbuffersSceneConfigurator>();
    break;
  // Add case for each new type...
}
```

**Proposed Approach:**
```cpp
// Template specialization - no switch needed
template<>
struct ConfiguratorRegistry<DataSourceType::Flatbuffers, SceneDataFbs, Scene> {
  static auto Create(EventHandler& eh, Factory& f) {
    return std::make_unique<FlatbuffersSceneConfigurator>(eh, f);
  }
};
```

**Benefits:**
- Add new types by creating specializations
- Compile-time dispatch (faster, type-safe)
- No central switch to maintain

### 2. Composition for Nested Objects

When Object A contains Object B:

```cpp
class FlatbuffersSceneConfigurator {
  std::expected<std::monostate, FailInfo>
  Configure(Scene& scene, const SceneDataFbs& data) override {
    
    // Scene contains EntityMemoryPool
    // Get its configurator from factory
    auto entity_config = m_factory.GetConfigurator<
        EntityCollectionFbs, EntityMemoryPool>();
    
    // Use it to configure entities
    entity_config.value()->Configure(
        scene.GetEntityMemoryPool(),
        *data.entity_collection());
    
    return std::monostate{};
  }
};
```

**Benefits:**
- Entity configuration logic exists in one place
- Reusable across Scene, SaveFile, Level, etc.
- Testable (can mock factory)

### 3. Free Functions for Simple Nested Types

For simple types like SceneInfo, SceneState:

```cpp
// Free, stateless function
std::expected<std::monostate, FailInfo>
ConfigureSceneInfo(SceneInfo& info, const SceneInfoFbs* fb_info) {
  if (!fb_info) return std::unexpected(...);
  info.type = fb_info->type();
  return std::monostate{};
}

// Called from any configurator
class FlatbuffersSceneConfigurator {
  auto result = ConfigureSceneInfo(scene.GetSceneInfo(), data.scene_info());
};
```

**Benefits:**
- No interface overhead for simple types
- Callable from anywhere
- Easy to test in isolation

### 4. Explicit Coupling Points

Coupling confined to **registry specializations only**:

```cpp
// THIS is the only place that knows about:
// - FlatBuffers as data source
// - SceneDataFbs as data type
// - Scene as object type
// - FlatbuffersSceneConfigurator as implementation
template<>
struct ConfiguratorRegistry<DataSourceType::Flatbuffers, 
                           SceneDataFbs, Scene> {
  static auto CreateConfigurator(...) {
    return std::make_unique<FlatbuffersSceneConfigurator>(...);
  }
};
```

Everywhere else uses interfaces:
```cpp
IConfigurator<SceneDataFbs, Scene>* configurator = ...;
```

## Comparison with Current System

| Aspect | Current | Proposed |
|--------|---------|----------|
| Adding new data type | Modify `DataAccessFactory` switch | Add template specialization |
| Type dispatch | Runtime switch | Compile-time template |
| Coupling location | Scattered (factory + implementations) | Explicit (registry only) |
| Code reuse | Limited | High (composition + free functions) |
| Type safety | Mixed (some type erasure) | Full (no type erasure) |
| Extensibility | Manual | Automatic (templates) |

## Benefits

### 1. Easy to Add New Data Types

**To add JSON support:**

```cpp
// 1. Define data structure
struct JSONSceneData { /* ... */ };

// 2. Create configurator
class JSONSceneConfigurator : public IConfigurator<JSONSceneData, Scene> { /* ... */ };

// 3. Register (one specialization)
template<>
struct ConfiguratorRegistry<DataSourceType::JSON, JSONSceneData, Scene> {
  static auto Create(...) { return std::make_unique<JSONSceneConfigurator>(...); }
};

// 4. Use it (just change template parameter!)
ConfiguratorFactory<DataSourceType::JSON> factory(event_handler);
```

**No changes to existing code required!**

### 2. Clear Separation of Concerns

- **IDataProvider**: Loads data from files/memory
- **IConfigurator**: Configures objects from data
- **Free functions**: Configure simple nested types
- **Registry**: Maps data sources to implementations
- **Factory**: Manages lifecycle and dependencies

### 3. Testability

```cpp
// Mock factory for testing
class MockFactory : public ConfiguratorFactory<DataSourceType::Test> {
  template<typename Data, typename Object>
  auto GetConfigurator() {
    return std::make_unique<MockConfigurator<Data, Object>>();
  }
};

// Test configurator with mocked dependencies
FlatbuffersSceneConfigurator config(event_handler, mock_factory);
```

### 4. Performance

- **Compile-time dispatch**: No runtime overhead from switch statements
- **Caching**: Factory caches configurator instances
- **No type erasure**: Direct function calls, no virtual dispatch for data access

## Migration Strategy

The proposed architecture can be adopted incrementally:

### Phase 1: Add Template Infrastructure (Non-Breaking)
- Create new interfaces (`IConfigurator`, `IDataProvider`)
- Create factory and registry classes
- Add alongside existing code

### Phase 2: Migrate One Type (Parallel)
- Refactor Scene configurator to new pattern
- Keep old code for other types
- Test thoroughly

### Phase 3: Migrate Remaining Types (Incremental)
- Migrate EntityConfigurator
- Migrate UIElementConfigurator
- Keep both implementations until all tests pass

### Phase 4: Remove Old Code (Clean Up)
- Remove old `DataAccessFactory` switch implementation
- Remove old interface implementations
- Update documentation

**Risk Mitigation:**
- Each phase can be tested independently
- Old and new code can coexist
- Rollback is possible at each phase

## Answers to Specific Questions

### Q: "How to accept specific data types without type erasure?"

**A:** Template-based interfaces where data type is template parameter:
```cpp
IConfigurator<SceneDataFbs, Scene>  // No type erasure, knows SceneDataFbs
```

### Q: "How to avoid switch statements in factory?"

**A:** Template specialization in registry:
```cpp
template<>
struct ConfiguratorRegistry<Flatbuffers, SceneDataFbs, Scene> { /* ... */ };
```

### Q: "Should top-level objects source their own data?"

**A:** Yes, via paired `IDataProvider`:
```cpp
auto provider = factory.GetDataProvider<SceneDataFbs>();
auto data = provider->LoadData();
```

### Q: "Where is coupling allowed?"

**A:** Only in registry specializations:
```cpp
template<>
struct ConfiguratorRegistry<...> {  // Only here
  static auto Create(...) { 
    return std::make_unique<ConcreteImplementation>(...);
  }
};
```

### Q: "How does A's interface call B's interface when A contains B?"

**A:** Composition via factory:
```cpp
class SceneConfigurator {
  auto entity_config = m_factory.GetConfigurator<EntityData, Entities>();
  entity_config->Configure(scene.GetEntities(), data.entities());
};
```

### Q: "Should shared objects have interfaces?"

**A:** Yes, for reusability:
```cpp
// EntityMemoryPool configurator used by Scene, SaveFile, Level
auto entity_config = factory.GetConfigurator<EntityData, EntityMemoryPool>();
```

## Documentation Structure

Complete documentation is organized as follows:

1. **NATIVE_OBJECT_CREATION_ARCHITECTURE.md** (This document)
   - Overview of proposed architecture
   - Design patterns and rationale
   - Comparison with current system

2. **ADDRESSING_SPECIFIC_QUESTIONS.md**
   - Direct answers to problem statement questions
   - Detailed explanations with code examples

3. **examples/CONFIGURATOR_PATTERN_EXAMPLES.md**
   - Concrete, working code examples
   - Step-by-step implementation examples
   - Adding new data types walkthrough

4. **IMPLEMENTATION_GUIDE.md**
   - Step-by-step implementation instructions
   - File structure and organization
   - Migration strategy details

## Recommendations

### Immediate Actions
1. **Review** the proposed architecture with the team
2. **Prototype** the template infrastructure for Scene configurator
3. **Test** the pattern with one complete type migration
4. **Evaluate** performance and ergonomics

### Long-Term Plan
1. **Adopt** template-based pattern for all new configurators
2. **Migrate** existing configurators incrementally
3. **Document** the pattern as team standard
4. **Extend** to other data types (JSON, Binary, etc.) as needed

### Success Criteria
- ✅ Can add new data type with single template specialization
- ✅ No switch statements in factory code
- ✅ Clear, documented coupling points
- ✅ Improved code reuse (less duplication)
- ✅ Full compile-time type safety
- ✅ Existing functionality preserved

## Conclusion

The proposed template-based configuration architecture provides a general, extensible pattern for native object creation that:

- **Eliminates** tight coupling and switch statements
- **Enables** easy addition of new data types
- **Maintains** full type safety without type erasure
- **Promotes** code reuse through composition
- **Clarifies** where coupling is allowed
- **Supports** incremental migration from existing code

This approach addresses all concerns raised in the problem statement while providing a clear, consistent pattern for the entire codebase.
