# Scene Factory & Configurator Quick Reference

**Quick access guide for Scene configuration architecture**

---

## TL;DR

Use **Strategy Pattern** with configurators:
- `ISceneConfigurator` interface (strategy)
- `DefaultSceneConfigurator` for default scene data
- `SavedSceneConfigurator` for save data
- SceneFactory receives configurator in constructor
- No intermediate native structs - configure directly from FlatBuffers

---

## Architecture Pattern

```
SceneManager → Creates Configurator → Passes to SceneFactory
                     ↓
            ISceneConfigurator (Strategy)
                     ↓
      ┌──────────────┴──────────────┐
      ↓                             ↓
DefaultSceneConfigurator    SavedSceneConfigurator
      ↓                             ↓
  GetSceneData() → SceneDataFbs*
```

---

## Interface Definitions

### ISceneConfigurator

```cpp
class ISceneConfigurator {
public:
  virtual ~ISceneConfigurator() = default;
  
  // Get FlatBuffers scene data
  virtual const SceneDataFbs* GetSceneData() const = 0;
  
  // Create entity configurator
  virtual std::unique_ptr<IEntityConfigurator> 
  CreateEntityConfigurator(EventHandler &event_handler) const = 0;
  
  // Get scene type
  virtual SceneType GetSceneType() const = 0;
};
```

### DefaultSceneConfigurator

```cpp
class DefaultSceneConfigurator : public ISceneConfigurator {
private:
  SceneType m_scene_type;
  ISceneDataProvider &m_scene_data_provider;
  mutable const SceneDataFbs *m_cached_scene_data{nullptr};

public:
  DefaultSceneConfigurator(SceneType scene_type,
                          ISceneDataProvider &provider);
  
  const SceneDataFbs* GetSceneData() const override;
  std::unique_ptr<IEntityConfigurator> 
  CreateEntityConfigurator(EventHandler &) const override;
  SceneType GetSceneType() const override;
};
```

### SavedSceneConfigurator

```cpp
class SavedSceneConfigurator : public ISceneConfigurator {
private:
  uint32_t m_save_slot_index;
  ISaveDataProvider &m_save_data_provider;
  mutable const SceneDataFbs *m_cached_scene_data{nullptr};

public:
  SavedSceneConfigurator(uint32_t slot_index,
                        ISaveDataProvider &provider);
  
  const SceneDataFbs* GetSceneData() const override;
  std::unique_ptr<IEntityConfigurator> 
  CreateEntityConfigurator(EventHandler &) const override;
  SceneType GetSceneType() const override;
};
```

---

## Usage Patterns

### Pattern 1: Loading Default Scene

```cpp
// In SceneManager
std::expected<std::monostate, FailInfo>
SceneManager::LoadSceneFromDefault(SceneType scene_type) {
  // 1. Get data provider (singleton)
  ISceneDataProvider &provider = GetSceneDataProvider();
  
  // 2. Create configurator
  DefaultSceneConfigurator configurator(scene_type, provider);
  
  // 3. Create factory with configurator
  FlatbuffersSceneFactory factory(m_game_context, configurator);
  
  // 4. Create and configure scene
  auto scene_result = factory.CreateScene();
  if (!scene_result.has_value()) {
    return std::unexpected(scene_result.error());
  }
  
  // 5. Store scene
  auto scene_id = scene_result.value()->GetSceneInfo().id;
  m_scenes.emplace(scene_id, std::move(scene_result.value()));
  
  return std::monostate{};
}
```

### Pattern 2: Loading Saved Scene

```cpp
// In SceneManager
std::expected<std::monostate, FailInfo>
SceneManager::LoadSceneFromSave(uint32_t save_slot_index) {
  // 1. Get save provider (singleton)
  ISaveDataProvider &provider = GetSaveDataProvider();
  
  // 2. Create configurator
  SavedSceneConfigurator configurator(save_slot_index, provider);
  
  // 3. Create factory with configurator
  FlatbuffersSceneFactory factory(m_game_context, configurator);
  
  // 4. Create and configure scene
  auto scene_result = factory.CreateScene();
  if (!scene_result.has_value()) {
    return std::unexpected(scene_result.error());
  }
  
  // 5. Store scene
  auto scene_id = scene_result.value()->GetSceneInfo().id;
  m_scenes.emplace(scene_id, std::move(scene_result.value()));
  
  return std::monostate{};
}
```

### Pattern 3: SceneFactory Usage

```cpp
// In ISceneFactory::CreateScene()
std::expected<std::unique_ptr<Scene>, FailInfo> 
ISceneFactory::CreateScene() {
  // 1. Get scene data from configurator
  const SceneDataFbs *scene_data = m_scene_configurator.GetSceneData();
  if (!scene_data) {
    return std::unexpected(FailInfo{FailMode::NullPointer, 
                                    "SceneData is null"});
  }
  
  // 2. Create entity configurator
  m_entity_configurator = 
      m_scene_configurator.CreateEntityConfigurator(
          m_game_context.event_handler);
  
  // 3. Create Scene by type
  auto scene_result = CreateSceneByType();
  if (!scene_result.has_value()) {
    return std::unexpected(scene_result.error());
  }
  
  auto scene_ptr = std::move(scene_result.value());
  
  // 4. Configure Scene (uses scene_data directly)
  if (auto result = ConfigureSceneInfo(*scene_ptr); !result) {
    return std::unexpected(result.error());
  }
  
  if (auto result = ConfigureSceneResources(*scene_ptr); !result) {
    return std::unexpected(result.error());
  }
  
  return scene_ptr;
}
```

---

## Key Principles

### ✅ DO

1. **Create configurator in SceneManager** - Manager orchestrates data source selection
2. **Pass configurator by const reference** - Factory doesn't own configurator
3. **Cache FlatBuffers data** - Load once per configurator lifetime
4. **Use std::expected for errors** - Propagate failures up the chain
5. **Let configurator create entity configurator** - Consistent data source

### ❌ DON'T

1. **Don't store raw FlatBuffers pointers in Factory** - Use configurator.GetSceneData()
2. **Don't create intermediate native structs** - Configure directly from FlatBuffers
3. **Don't put conditionals in Factory** - Use strategy pattern instead
4. **Don't expose FlatBuffers types to game code** - Keep encapsulated in configurators
5. **Don't reuse configurators** - One configurator per Scene

---

## Configurator Responsibilities

### ISceneConfigurator

| Method | Responsibility | Returns |
|--------|----------------|---------|
| `GetSceneData()` | Provide FlatBuffers scene data | `const SceneDataFbs*` |
| `CreateEntityConfigurator()` | Create matching entity configurator | `unique_ptr<IEntityConfigurator>` |
| `GetSceneType()` | Provide scene type enum | `SceneType` |

### DefaultSceneConfigurator

| Responsibility | Implementation |
|----------------|----------------|
| Data Source | `ISceneDataProvider` |
| Load Data | `provider.LoadSceneData(scene_type)` |
| Extract FlatBuffers | From provider's FlatBuffers data |
| Entity Configurator | `FlatbuffersEntityConfigurator` with entity collection |

### SavedSceneConfigurator

| Responsibility | Implementation |
|----------------|----------------|
| Data Source | `ISaveDataProvider` |
| Load Data | `provider.LoadSave(slot_index)` |
| Extract FlatBuffers | Extract `SceneDataFbs` from `SaveDataFbs` |
| Entity Configurator | `FlatbuffersEntityConfigurator` with entity collection |

---

## SceneFactory Responsibilities

### Updated Constructor

```cpp
// Before (❌ Direct coupling)
FlatbuffersSceneFactory(const GameContext &game_context,
                        const SceneDataFbs *scene_data_fbs);

// After (✅ Strategy pattern)
FlatbuffersSceneFactory(const GameContext &game_context,
                        const ISceneConfigurator &scene_configurator);
```

### Configuration Flow

```cpp
// Inside FlatbuffersSceneFactory::ConfigureSceneResources()
std::expected<std::monostate, FailInfo>
FlatbuffersSceneFactory::ConfigureSceneResources(Scene &scene) {
  // 1. Get data from configurator
  const SceneDataFbs *scene_data = m_scene_configurator.GetSceneData();
  if (!scene_data) {
    return std::unexpected(FailInfo{FailMode::NullPointer, 
                                    "SceneData is null"});
  }
  
  // 2. Configure render texture
  if (scene_data->scene_resources()) {
    auto resources = scene_data->scene_resources();
    // Use resources to configure scene
  }
  
  // 3. Configure entity memory pool
  auto configure_result = 
      m_entity_configurator->ConfigureEntityMemoryPool(
          scene.GetEntityManager().GetEntityMemoryPool());
  
  if (!configure_result.has_value()) {
    return std::unexpected(configure_result.error());
  }
  
  return std::monostate{};
}
```

---

## Data Flow Comparison

### Current (❌ Coupled)

```
SceneManager
    → Creates SceneDataFbs* directly
    → Passes to FlatbuffersSceneFactory
    → Factory stores FlatBuffers pointer
    → Direct coupling to FlatBuffers
```

### Proposed (✅ Decoupled)

```
SceneManager
    → Creates ISceneConfigurator (strategy)
    → Passes to SceneFactory
    → Factory calls configurator.GetSceneData()
    → No direct FlatBuffers dependency in Factory
```

---

## Error Handling

### Configurator Errors

```cpp
const SceneDataFbs* DefaultSceneConfigurator::GetSceneData() const {
  if (m_cached_scene_data) {
    return m_cached_scene_data;  // Return cached data
  }
  
  // Load from provider
  auto result = m_scene_data_provider.LoadSceneData(m_scene_type);
  if (!result.has_value()) {
    // Log error but return nullptr (Factory will handle)
    return nullptr;
  }
  
  // Cache and return
  m_cached_scene_data = GetSceneDataFbs(result.value());
  return m_cached_scene_data;
}
```

### Factory Error Handling

```cpp
std::expected<std::unique_ptr<Scene>, FailInfo> 
ISceneFactory::CreateScene() {
  const SceneDataFbs *scene_data = m_scene_configurator.GetSceneData();
  if (!scene_data) {
    return std::unexpected(FailInfo{
        FailMode::NullPointer,
        "Failed to get SceneData from configurator"
    });
  }
  
  // Continue with configuration...
}
```

---

## Testing Strategy

### Unit Tests

```cpp
TEST_CASE("DefaultSceneConfigurator loads and caches data", 
          "[unit][DefaultSceneConfigurator]") {
  // Mock provider
  MockSceneDataProvider provider;
  
  DefaultSceneConfigurator configurator(
      SceneType::SceneType_TITLE, provider);
  
  // First call loads
  const SceneDataFbs *data1 = configurator.GetSceneData();
  REQUIRE(data1 != nullptr);
  REQUIRE(provider.GetLoadCount() == 1);
  
  // Second call returns cached data
  const SceneDataFbs *data2 = configurator.GetSceneData();
  REQUIRE(data2 == data1);  // Same pointer
  REQUIRE(provider.GetLoadCount() == 1);  // No additional load
}
```

### Integration Tests

```cpp
TEST_CASE("SceneFactory with DefaultSceneConfigurator", 
          "[integration][SceneFactory]") {
  GameContext game_context = CreateTestGameContext();
  ISceneDataProvider &provider = GetSceneDataProvider();
  
  DefaultSceneConfigurator configurator(
      SceneType::SceneType_TITLE, provider);
  
  FlatbuffersSceneFactory factory(game_context, configurator);
  
  auto scene_result = factory.CreateScene();
  REQUIRE(scene_result.has_value());
  
  auto scene = std::move(scene_result.value());
  REQUIRE(scene->GetSceneInfo().scene_type == SceneType::SceneType_TITLE);
}
```

---

## Files to Create/Modify

### New Files

```
src/scenes/ISceneConfigurator.h
src/scenes/DefaultSceneConfigurator.h
src/scenes/DefaultSceneConfigurator.cpp
src/scenes/SavedSceneConfigurator.h
src/scenes/SavedSceneConfigurator.cpp
```

### Modified Files

```
src/scenes/ISceneFactory.h              (constructor signature)
src/scenes/ISceneFactory.cpp            (use configurator)
src/scenes/FlatbuffersSceneFactory.h    (constructor signature)
src/scenes/FlatbuffersSceneFactory.cpp  (use configurator)
src/scenes/SceneManager.h               (new load methods)
src/scenes/SceneManager.cpp             (create configurators)
```

---

## Common Pitfalls

### ❌ Pitfall 1: Storing FlatBuffers Pointer in Factory

```cpp
// DON'T DO THIS
class FlatbuffersSceneFactory : public ISceneFactory {
private:
  const SceneDataFbs *m_scene_data_fbs;  // ❌ Direct coupling
};
```

**Solution**: Use configurator to get data on-demand

```cpp
// DO THIS
class FlatbuffersSceneFactory : public ISceneFactory {
  // No FlatBuffers member! Use m_scene_configurator.GetSceneData()
};
```

### ❌ Pitfall 2: Creating Native Intermediate Struct

```cpp
// DON'T DO THIS
struct SceneData {
  std::string scene_id;
  EntityCollection entities;  // Complex nested structure
  LogicCollection logic;
  AssetCollection assets;
};

SceneData ConvertFromFlatBuffers(const SceneDataFbs *fbs);  // ❌ Duplication
```

**Solution**: Configure directly from FlatBuffers

```cpp
// DO THIS
void ConfigureScene(Scene &scene, const SceneDataFbs *scene_data_fbs) {
  // Use FlatBuffers data directly
  if (scene_data_fbs->scene_info()) {
    scene.m_scene_info.scene_id = scene_data_fbs->scene_info()->scene_id()->str();
  }
}
```

### ❌ Pitfall 3: Conditional Logic in Factory

```cpp
// DON'T DO THIS
class SceneFactory {
  void LoadData() {
    if (m_is_from_save) {
      // Load from save
    } else {
      // Load from default
    }
  }
};
```

**Solution**: Use strategy pattern with configurators

```cpp
// DO THIS
class SceneFactory {
  SceneFactory(const ISceneConfigurator &configurator)  // Strategy
      : m_scene_configurator(configurator) {}
  
  void LoadData() {
    const SceneDataFbs *data = m_scene_configurator.GetSceneData();
    // No conditionals needed!
  }
};
```

---

## Decision Flow: Where Does Configuration Happen?

### Question: Should Scene have ConfigureFromDefault() method?

**Answer**: ❌ NO

**Reason**: Scene is a structural container. Configuration logic belongs in configurators and factories.

```cpp
// ❌ DON'T DO THIS
class Scene {
  void ConfigureFromDefault(const SceneDataFbs *data);
};

// ✅ DO THIS
class SceneFactory {
  std::expected<std::monostate, FailInfo> 
  ConfigureSceneResources(Scene &scene) {
    // Configuration happens here, using data from configurator
  }
};
```

### Question: Should Factory know about data providers?

**Answer**: ❌ NO

**Reason**: Factory should be agnostic to data source. Configurator handles data access.

```cpp
// ❌ DON'T DO THIS
class SceneFactory {
  ISceneDataProvider &m_provider;
};

// ✅ DO THIS
class SceneFactory {
  const ISceneConfigurator &m_configurator;  // Strategy
};
```

### Question: Should SceneManager create configurators?

**Answer**: ✅ YES

**Reason**: SceneManager orchestrates scene loading and decides which data source to use.

```cpp
// ✅ DO THIS
class SceneManager {
  std::expected<std::monostate, FailInfo>
  LoadSceneFromDefault(SceneType scene_type) {
    DefaultSceneConfigurator configurator(...);  // Manager creates
    FlatbuffersSceneFactory factory(m_game_context, configurator);
    // ...
  }
};
```

---

## Related Documents

- [Full Analysis](SCENE_FACTORY_AND_CONFIGURATOR_ANALYSIS.md)
- [Visual Diagrams](SCENE_FACTORY_VISUALS.md)
- `src/data_providers/ISceneDataProvider.h`
- `src/configuration/ISaveDataProvider.h`
