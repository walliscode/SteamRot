# Alternative Data Handling Analysis - Direct Configuration

**Created**: December 12, 2025  
**Purpose**: Address concern about SceneData struct duplication and explore alternative patterns

---

## User Concern

> "I very specifically do not want to make an intermediate SceneData struct. I will end repeating heavy configuration logic twice. I might as just make a Scene directly"

**Valid concern**: Creating a `SceneData` intermediate struct could lead to:
1. Duplicate logic: Convert FlatBuffers → SceneData, then SceneData → Scene
2. Extra allocations and copies
3. More code to maintain

---

## Alternative Approaches

### Option 1: Pass FlatBuffers Directly to Configurator (Current-ish)

**Keep configurator taking FlatBuffers pointers directly:**

```cpp
class ISceneConfigurator {
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneDataFbs *data) = 0;
};

// Usage
FlatbuffersDataLoader loader;
auto scene_data = loader.ProvideDefaultSceneData(scene_type);

SceneFactory factory(game_context);
auto scene = factory.CreateSceneByType(scene_type);

ISceneConfigurator &configurator = GetSceneConfigurator();
configurator.ConfigureScene(*scene, scene_data.value());
```

**Pros**:
- ✅ No intermediate struct
- ✅ No duplicate conversion logic
- ✅ Direct access to FlatBuffers data

**Cons**:
- ❌ Configurator is coupled to FlatBuffers (cannot support XML/JSON)
- ❌ Need separate configurators per data format
- ❌ Testing requires FlatBuffers test data

---

### Option 2: Configurator Creates Scene Directly

**Have configurator create AND configure the Scene:**

```cpp
class ISceneConfigurator {
  virtual std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateConfiguredScene(SceneType type, const GameContext &context) = 0;
};

class FlatbuffersSceneConfigurator : public ISceneConfigurator {
private:
  FlatbuffersDataLoader m_loader;
  
public:
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateConfiguredScene(SceneType type, const GameContext &context) override {
    // Load data
    auto scene_data = m_loader.ProvideDefaultSceneData(type);
    if (!scene_data.has_value())
      return std::unexpected(scene_data.error());
    
    // Create scene
    std::unique_ptr<Scene> scene;
    switch (type) {
      case SceneType::SceneType_TITLE:
        scene = std::make_unique<TitleScene>(context);
        break;
      case SceneType::SceneType_CRAFTING:
        scene = std::make_unique<CraftingScene>(context);
        break;
      // ...
    }
    
    // Configure directly from FlatBuffers
    ConfigureSceneFromFlatBuffers(*scene, scene_data.value());
    
    return scene;
  }
};

// Usage
ISceneConfigurator &configurator = GetSceneConfigurator();
auto scene = configurator.CreateConfiguredScene(scene_type, game_context);
```

**Pros**:
- ✅ No intermediate struct
- ✅ One-stop shop for scene creation
- ✅ Simple usage

**Cons**:
- ❌ Configurator knows about scene types (Title, Crafting)
- ❌ Violates Single Responsibility Principle
- ❌ Need FlatbuffersSceneConfigurator, XMLSceneConfigurator, SaveSceneConfigurator
- ❌ Each configurator duplicates scene creation switch statement

---

### Option 3: Provider Returns FlatBuffers Pointer (Abstraction via Provider)

**Provider abstracts data loading, but returns FlatBuffers pointer:**

```cpp
class ISceneDataProvider {
  virtual std::expected<const SceneDataFbs*, FailInfo>
  LoadSceneData(SceneType type) const = 0;
};

class FlatbuffersDefaultSceneDataProvider : public ISceneDataProvider {
  std::expected<const SceneDataFbs*, FailInfo>
  LoadSceneData(SceneType type) const override {
    return m_loader.ProvideDefaultSceneData(type);
  }
};

// For XML, would return... what? XMLSceneData*? Different type!
class XMLSceneDataProvider : public ISceneDataProvider {
  std::expected<const SceneDataFbs*, FailInfo>  // ❌ Can't return XML data!
  LoadSceneData(SceneType type) const override;
};
```

**Pros**:
- ✅ Provider abstracts WHERE data comes from
- ✅ No intermediate struct conversion

**Cons**:
- ❌ Cannot support different data formats (XML would be a different pointer type)
- ❌ Interface still tied to FlatBuffers
- ❌ Doesn't solve the abstraction problem

---

### Option 4: Configurator Takes std::variant of Data Pointers

**Use variant to support multiple formats:**

```cpp
using SceneDataVariant = std::variant<
  const SceneDataFbs*,
  const XMLSceneData*,
  const JSONSceneData*
>;

class ISceneConfigurator {
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneDataVariant &data) = 0;
};

void ConfigureScene(Scene &scene, const SceneDataVariant &data) {
  std::visit([&](auto&& arg) {
    using T = std::decay_t<decltype(arg)>;
    if constexpr (std::is_same_v<T, const SceneDataFbs*>) {
      // Configure from FlatBuffers
    } else if constexpr (std::is_same_v<T, const XMLSceneData*>) {
      // Configure from XML
    }
  }, data);
}
```

**Pros**:
- ✅ No intermediate struct
- ✅ Supports multiple formats in one interface
- ✅ No duplicate conversion logic

**Cons**:
- ⚠️ Configurator still knows about all data formats
- ⚠️ Adding new format requires updating variant and configurator
- ⚠️ Complex visitor pattern in configurator

---

### Option 5: Keep Current Pattern, Just Pass Pointers

**Minimal change from current architecture:**

```cpp
// Current
class ISceneConfigurator {
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene &scene, const SceneType scene_type) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene &scene, const SceneType scene_type) = 0;
};

// Proposed: Just pass the data down!
class ISceneConfigurator {
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneDataFbs *data) = 0;
  
protected:
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene &scene, const SceneInfoFbs *info) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene &scene, const SceneResourcesFbs *resources) = 0;
};

// In SceneManager
auto scene_data = m_data_loader.ProvideDefaultSceneData(scene_type);

SceneFactory factory(m_game_context);
auto scene = factory.CreateSceneByType(scene_type);

ISceneConfigurator &configurator = GetSceneConfigurator();
configurator.ConfigureScene(*scene, scene_data.value());
```

**Pros**:
- ✅ No intermediate struct
- ✅ Minimal changes from current code
- ✅ Data loaded once, passed down
- ✅ Avoids redundant loading in each Configure method

**Cons**:
- ❌ Still coupled to FlatBuffers
- ❌ Cannot easily support XML/JSON

---

## Recommendation Based on User Concern

Given your constraint that you **specifically do not want an intermediate struct**, I recommend **Option 5** as the most pragmatic solution:

### Modified Recommendation: Pass FlatBuffers Pointers Through

**Goal**: Eliminate redundant data loading without creating intermediate structs.

**Key Changes**:

1. **SceneManager loads data once**:
```cpp
std::expected<std::monostate, FailInfo>
SceneManager::AddSceneFromDefault(SceneType type) {
  
  // Load data ONCE
  FlatbuffersDataLoader loader;
  auto scene_data = loader.ProvideDefaultSceneData(type);
  if (!scene_data.has_value())
    return std::unexpected(scene_data.error());
  
  // Create empty scene
  SceneFactory factory(m_game_context);
  auto scene = factory.CreateSceneByType(type);
  if (!scene.has_value())
    return std::unexpected(scene.error());
  
  // Configure with loaded data
  ISceneConfigurator &configurator = GetSceneConfigurator();
  auto config_result = configurator.ConfigureScene(*scene.value(), 
                                                    scene_data.value());
  if (!config_result.has_value())
    return std::unexpected(config_result.error());
  
  // Add to map
  m_scenes.emplace(scene.value()->GetSceneInfo().id, std::move(scene.value()));
  
  return std::monostate{};
}
```

2. **Configurator receives pointer, doesn't load**:
```cpp
class ISceneConfigurator {
public:
  // Main entry point - receives data
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneDataFbs *scene_data) {
    
    if (!scene_data)
      return std::unexpected(FailInfo{FailMode::NullPointer, "scene_data is null"});
    
    // Configure each aspect
    if (scene_data->scene_info()) {
      auto result = ConfigureSceneInfo(scene, scene_data->scene_info());
      if (!result.has_value())
        return std::unexpected(result.error());
    }
    
    if (scene_data->scene_resources()) {
      auto result = ConfigureSceneResources(scene, scene_data->scene_resources());
      if (!result.has_value())
        return std::unexpected(result.error());
    }
    
    if (scene_data->entity_collection()) {
      auto result = ConfigureEntities(scene, scene_data->entity_collection());
      if (!result.has_value())
        return std::unexpected(result.error());
    }
    
    auto result = ConfigureLogicMap(scene);
    if (!result.has_value())
      return std::unexpected(result.error());
    
    return std::monostate{};
  }

protected:
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene &scene, const SceneInfoFbs *info) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene &scene, const SceneResourcesFbs *resources) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEntities(Scene &scene, const EntityCollectionFbs *entities) = 0;
  
  std::expected<std::monostate, FailInfo>
  ConfigureLogicMap(Scene &scene);  // Non-virtual
};
```

3. **FlatbuffersDefaultSceneConfigurator implementation**:
```cpp
class FlatbuffersDefaultSceneConfigurator : public ISceneConfigurator {
protected:
  std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene &scene, const SceneInfoFbs *info) override {
    // Direct configuration from FlatBuffers
    scene.m_scene_info.type = info->scene_type();
    // ... other fields
    return std::monostate{};
  }
  
  std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene &scene, const SceneResourcesFbs *resources) override {
    // Direct configuration from FlatBuffers
    // ... configure resources
    return std::monostate{};
  }
  
  std::expected<std::monostate, FailInfo>
  ConfigureEntities(Scene &scene, const EntityCollectionFbs *entities) override {
    // Pass FlatBuffers pointer to entity configurator
    FlatbuffersEntityConfigurator entity_config(m_event_handler, *entities);
    return entity_config.ConfigureEntityMemoryPool(scene.GetEntityMemoryPool());
  }
};
```

### What This Achieves

**✅ Addresses Your Concerns**:
- No intermediate `SceneData` struct
- No duplicate conversion logic
- Configuration logic stays in configurators (where it belongs)
- Data loaded once, passed through

**✅ Improvements Over Current**:
- Eliminates redundant `ProvideDefaultSceneData()` calls
- Clear data flow: Manager loads → Factory creates → Configurator configures
- Configurator doesn't own `FlatbuffersDataLoader`

**⚠️ Trade-offs**:
- Still coupled to FlatBuffers (but you're okay with this)
- Cannot easily add XML/JSON support later (but that's not a current requirement)
- If you later need format abstraction, would need to revisit

### For Future XML/JSON Support

If/when you need to support other formats, you have two paths:

**Path A: Accept the intermediate struct at that time**
- When XML is actually needed, create the abstraction
- At that point, the cost is justified by the requirement

**Path B: Use variant/visitor pattern**
- Add `std::variant<const SceneDataFbs*, const XMLSceneData*>` 
- Configurator uses visitor pattern to handle both

---

## Summary

**Original Recommendation**: Create `ISceneDataProvider` with intermediate `SceneData` struct
- ✅ Full abstraction
- ❌ Duplicate conversion logic (your concern)

**Modified Recommendation**: Pass FlatBuffers pointers through, load once at top level
- ✅ No intermediate struct
- ✅ No duplicate logic
- ✅ Eliminates redundant loading
- ⚠️ Keeps FlatBuffers coupling (acceptable trade-off)

**Key Insight**: You're right that the intermediate struct creates duplication. The modified approach keeps the benefits of clear data flow while avoiding the conversion overhead. The abstraction can be added later **only if/when** multiple data formats are actually needed.

---

## Action Items

1. ✅ Update `ISceneConfigurator::ConfigureScene()` to take `const SceneDataFbs*`
2. ✅ Remove `FlatbuffersDataLoader` member from `FlatbuffersDefaultSceneConfigurator`
3. ✅ Update `SceneManager::AddSceneFromDefault()` to load data and pass to configurator
4. ✅ Update protected methods to take specific FlatBuffers pointers
5. ⏸️ Defer provider abstraction until XML/JSON support is actually needed

This approach respects your concern about duplication while still improving the architecture by eliminating redundant loading and clarifying responsibilities.
