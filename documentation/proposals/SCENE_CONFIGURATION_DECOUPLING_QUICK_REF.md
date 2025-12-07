# Scene Configuration Decoupling - Quick Reference

**Quick lookup guide for implementing the scene configuration decoupling architecture.**

---

## TL;DR

**Problem**: Scene/SceneFactory hardcoded to "default" data, can't load from saves.

**Solution**: Abstract configurators + SceneManager controls data sources.

**Key Changes**:
1. Create `IEntityConfigurator` interface
2. Remove `ConfigureFromDefault()` from Scene
3. Add `Scene::Configure(IEntityConfigurator&, const void*)`
4. SceneManager fetches data, passes to scene factory
5. EntityMemoryPool configured by reference (no copies)

---

## Quick Patterns

### Pattern 1: Creating Scene from Default Data

```cpp
// SceneManager.cpp
std::expected<uuids::uuid, FailInfo>
SceneManager::AddSceneFromDefault(const SceneType &scene_type) {
    // 1. Get data from provider
    ISceneDataProvider& provider = GetSceneDataProvider();
    auto scene_data = provider.LoadSceneData(scene_type);
    if (!scene_data.has_value())
        return std::unexpected(scene_data.error());
    
    // 2. Load FlatBuffers entity data
    FlatbuffersDataLoader loader;
    auto fb_data = loader.ProvideDefaultSceneData(scene_type);
    if (!fb_data.has_value())
        return std::unexpected(fb_data.error());
    
    // 3. Create configurator
    FlatbuffersConfigurator configurator(m_game_context.event_handler);
    
    // 4. Call scene factory with configurator and data
    auto scene_result = scene_factory::CreateScene(
        scene_type,
        m_game_context,
        configurator,
        fb_data.value()->entity_collection());
    
    if (!scene_result.has_value())
        return std::unexpected(scene_result.error());
    
    // 5. Add to scenes map
    auto uuid = scene_result.value()->GetSceneInfo().id;
    m_scenes.emplace(uuid, std::move(scene_result.value()));
    
    return uuid;
}
```

### Pattern 2: Creating Scene from Save Data (Future)

```cpp
// SceneManager.cpp
std::expected<uuids::uuid, FailInfo>
SceneManager::AddSceneFromSave(uint32_t slot_index) {
    // 1. Get save data from provider
    ISaveDataProvider& provider = GetSaveDataProvider();
    auto save_data = provider.LoadSave(slot_index);
    if (!save_data.has_value())
        return std::unexpected(save_data.error());
    
    // 2. Create configurator for save data
    SaveDataConfigurator configurator(m_game_context.event_handler);
    
    // 3. Call scene factory with configurator and data
    auto scene_result = scene_factory::CreateScene(
        save_data.value().current_scene_type,
        m_game_context,
        configurator,
        &save_data.value());
    
    if (!scene_result.has_value())
        return std::unexpected(scene_result.error());
    
    // 4. Add to scenes map
    auto uuid = scene_result.value()->GetSceneInfo().id;
    m_scenes.emplace(uuid, std::move(scene_result.value()));
    
    return uuid;
}
```

### Pattern 3: Implementing IEntityConfigurator

```cpp
// MyDataConfigurator.h
class MyDataConfigurator : public EntityConfigurator {
public:
    MyDataConfigurator(EventHandler &event_handler);
    
    std::expected<std::monostate, FailInfo>
    ConfigureEntities(EntityMemoryPool &entity_memory_pool,
                     const void *data_source) override;
};

// MyDataConfigurator.cpp
std::expected<std::monostate, FailInfo>
MyDataConfigurator::ConfigureEntities(
    EntityMemoryPool &entity_memory_pool,
    const void *data_source) {
    
    // 1. Validate input
    if (!data_source) {
        return std::unexpected(FailInfo{
            FailMode::NullPointer,
            "Data source is null"
        });
    }
    
    // 2. Cast to expected type
    const MyDataFormat* my_data = 
        static_cast<const MyDataFormat*>(data_source);
    
    // 3. Configure entity memory pool directly (by reference)
    // Resize pool
    size_t pool_size = my_data->GetEntityCount();
    std::apply([pool_size](auto &...component_vector) {
        (component_vector.resize(pool_size), ...);
    }, entity_memory_pool);
    
    // 4. Configure each entity
    for (size_t i = 0; i < pool_size; ++i) {
        // Get components for this entity
        auto& ui_component = 
            entity::memory::GetComponent<CUserInterface>(i, entity_memory_pool);
        
        // Configure from data
        ui_component.m_name = my_data->GetEntityName(i);
        ui_component.m_active = true;
        // ... etc
    }
    
    return std::monostate{};
}
```

### Pattern 4: Scene Factory Free Function

```cpp
// SceneFactory.h
namespace steamrot::scene_factory {

std::expected<std::unique_ptr<Scene>, FailInfo>
CreateScene(const SceneType &scene_type,
           const GameContext &game_context,
           IEntityConfigurator &configurator,
           const void *data_source);

} // namespace steamrot::scene_factory

// SceneFactory.cpp
namespace steamrot::scene_factory {

std::expected<std::unique_ptr<Scene>, FailInfo>
CreateScene(const SceneType &scene_type,
           const GameContext &game_context,
           IEntityConfigurator &configurator,
           const void *data_source) {
    
    // 1. Generate UUID
    uuids::uuid scene_uuid = CreateUUID();
    
    // 2. Create scene based on type
    std::unique_ptr<Scene> scene_ptr{nullptr};
    
    switch (scene_type) {
    case SceneType::SceneType_TITLE:
        scene_ptr = std::make_unique<TitleScene>(scene_uuid, game_context);
        break;
    case SceneType::SceneType_CRAFTING:
        scene_ptr = std::make_unique<CraftingScene>(scene_uuid, game_context);
        break;
    default:
        return std::unexpected(FailInfo{
            FailMode::NonExistentEnumValue,
            "Unknown scene type"
        });
    }
    
    // 3. Configure render texture (from ISceneDataProvider)
    ISceneDataProvider& data_provider = GetSceneDataProvider();
    auto scene_data = data_provider.LoadSceneData(scene_type);
    if (!scene_data.has_value())
        return std::unexpected(scene_data.error());
    
    sf::Vector2u texture_size(
        scene_data.value().render_texture_width,
        scene_data.value().render_texture_height);
    scene_ptr->m_scene_resources.scene_texture = sf::RenderTexture(texture_size);
    
    // 4. Configure entities using provided configurator
    auto configure_result = scene_ptr->Configure(configurator, data_source);
    if (!configure_result.has_value())
        return std::unexpected(configure_result.error());
    
    // 5. Generate archetypes
    auto archetype_result = 
        scene_ptr->m_scene_resources.entity_manager.GenerateAllArchetypes();
    if (!archetype_result.has_value())
        return std::unexpected(archetype_result.error());
    
    // 6. Configure logic (separate from entity data)
    LogicFactory logic_factory(scene_type, scene_ptr->GetSceneContext());
    auto logic_map = logic_factory.CreateLogicMap(nullptr);
    if (!logic_map.has_value())
        return std::unexpected(logic_map.error());
    scene_ptr->SetLogicMap(std::move(logic_map.value()));
    
    return scene_ptr;
}

} // namespace steamrot::scene_factory
```

### Pattern 5: Scene Configuration Method

```cpp
// Scene.h
class Scene {
public:
    std::expected<std::monostate, FailInfo>
    Configure(IEntityConfigurator &configurator, const void *data_source);
};

// Scene.cpp
std::expected<std::monostate, FailInfo>
Scene::Configure(IEntityConfigurator &configurator,
                const void *data_source) {
    
    // Delegate to EntityManager
    auto result = m_scene_resources.entity_manager.Configure(
        configurator, data_source);
    
    if (!result.has_value())
        return std::unexpected(result.error());
    
    return std::monostate{};
}
```

### Pattern 6: EntityManager Configuration

```cpp
// EntityManager.h
class EntityManager {
public:
    std::expected<std::monostate, FailInfo>
    Configure(IEntityConfigurator &configurator, const void *data_source);
};

// EntityManager.cpp
std::expected<std::monostate, FailInfo>
EntityManager::Configure(IEntityConfigurator &configurator,
                        const void *data_source) {
    
    // Call configurator to configure our entity memory pool
    auto result = configurator.ConfigureEntities(
        m_entity_memory_pool, data_source);
    
    if (!result.has_value())
        return std::unexpected(result.error());
    
    return std::monostate{};
}
```

---

## Decision Trees

### When to Use Which Configurator?

```
Need to configure entities?
│
├─> From FlatBuffers default data?
│   └─> Use FlatbuffersConfigurator
│
├─> From SaveData struct?
│   └─> Use SaveDataConfigurator (future)
│
├─> From JSON file?
│   └─> Implement JsonConfigurator
│
├─> From network?
│   └─> Implement NetworkConfigurator
│
└─> From test data?
    └─> Implement MockConfigurator
```

### Where to Fetch Data?

```
Who needs the data?
│
├─> SceneManager creating scene?
│   └─> SceneManager fetches data, passes to scene factory
│
├─> Testing scene configuration?
│   └─> Test fetches data, passes to scene factory or Scene::Configure
│
└─> Scene factory needs render texture size?
    └─> Scene factory calls ISceneDataProvider directly
```

---

## Common Mistakes to Avoid

### ❌ Don't: Fetch Data Inside Scene/SceneFactory

```cpp
// BAD - Scene fetches its own data
std::expected<std::monostate, FailInfo>
Scene::ConfigureFromDefault() {
    FlatbuffersDataLoader loader;
    auto data = loader.ProvideDefaultSceneData(m_scene_info.type);
    // Scene knows about data source!
}
```

### ✅ Do: Pass Data from Outside

```cpp
// GOOD - Scene receives data from caller
std::expected<std::monostate, FailInfo>
Scene::Configure(IEntityConfigurator &configurator, const void *data) {
    return m_scene_resources.entity_manager.Configure(configurator, data);
}
```

---

### ❌ Don't: Expose Format-Specific Types in Public APIs

```cpp
// BAD - FlatBuffers type in public API
std::expected<std::unique_ptr<Scene>, FailInfo>
CreateScene(SceneType type, GameContext context, 
           const EntityCollection *fb_data);  // FlatBuffers type!
```

### ✅ Do: Use Abstract Interface and Opaque Pointer

```cpp
// GOOD - Abstract interface, opaque data
std::expected<std::unique_ptr<Scene>, FailInfo>
CreateScene(SceneType type, GameContext context,
           IEntityConfigurator &configurator,  // Abstract!
           const void *data_source);           // Opaque!
```

---

### ❌ Don't: Copy Entity Data Through Intermediate Structures

```cpp
// BAD - Copying entity data
std::vector<EntityData> intermediate_data = ExtractEntities(fb_data);
for (const auto& entity_data : intermediate_data) {
    CopyToPool(entity_data, entity_memory_pool);  // Copy!
}
```

### ✅ Do: Configure EntityMemoryPool Directly by Reference

```cpp
// GOOD - Direct configuration
std::expected<std::monostate, FailInfo>
ConfigureEntities(EntityMemoryPool &pool,  // By reference!
                 const void *data) {
    // Configure pool in-place
    auto& component = entity::memory::GetComponent<CUserInterface>(i, pool);
    component.m_name = ExtractName(data, i);
}
```

---

### ❌ Don't: Make Scene Methods Data-Source Specific

```cpp
// BAD - Scene knows about "default" vs "save"
class Scene {
    std::expected<std::monostate, FailInfo> ConfigureFromDefault();
    std::expected<std::monostate, FailInfo> ConfigureFromSave(SaveData);
};
```

### ✅ Do: Single Generic Configuration Method

```cpp
// GOOD - Scene is data-source agnostic
class Scene {
    std::expected<std::monostate, FailInfo>
    Configure(IEntityConfigurator &configurator, const void *data);
};
```

---

## Type Safety with `const void*`

### Validating Casts

```cpp
std::expected<std::monostate, FailInfo>
FlatbuffersConfigurator::ConfigureEntities(
    EntityMemoryPool &pool, const void *data) {
    
    // 1. Null check
    if (!data) {
        return std::unexpected(FailInfo{
            FailMode::NullPointer,
            "Data source is null"
        });
    }
    
    // 2. Cast to expected type
    const EntityCollection* entity_data = 
        static_cast<const EntityCollection*>(data);
    
    // 3. Validate FlatBuffers table structure
    if (!entity_data->entity_memory_pool_size()) {
        return std::unexpected(FailInfo{
            FailMode::FlatbuffersDataNotFound,
            "Entity memory pool size not found in data"
        });
    }
    
    if (!entity_data->entities()) {
        return std::unexpected(FailInfo{
            FailMode::FlatbuffersDataNotFound,
            "Entities vector not found in data"
        });
    }
    
    // 4. Safe to use
    size_t pool_size = entity_data->entity_memory_pool_size();
    // ... configure pool
}
```

### Documenting Expected Types

```cpp
/////////////////////////////////////////////////
/// @class FlatbuffersConfigurator
/// @brief Configures entities from FlatBuffers EntityCollection data.
///
/// Expected data_source type: `const EntityCollection*`
/// The data_source parameter in ConfigureEntities must point to a valid
/// FlatBuffers EntityCollection table.
/////////////////////////////////////////////////
class FlatbuffersConfigurator : public EntityConfigurator {
    // ...
};
```

---

## Testing Patterns

### Pattern 1: Mock Configurator for Testing

```cpp
// In test file
class MockConfigurator : public EntityConfigurator {
private:
    size_t m_configure_call_count = 0;
    
public:
    MockConfigurator(EventHandler &handler) 
        : EntityConfigurator(handler) {}
    
    std::expected<std::monostate, FailInfo>
    ConfigureEntities(EntityMemoryPool &pool, 
                     const void *data) override {
        m_configure_call_count++;
        
        // Minimal test setup
        std::apply([](auto &...vec) { 
            (vec.resize(5), ...); 
        }, pool);
        
        return std::monostate{};
    }
    
    size_t GetConfigureCallCount() const { 
        return m_configure_call_count; 
    }
};

TEST_CASE("Scene::Configure calls configurator") {
    TestContext test_context;
    MockConfigurator mock_config(test_context.GetEventHandler());
    
    TitleScene scene(CreateUUID(), test_context.GetGameContext());
    
    auto result = scene.Configure(mock_config, nullptr);
    
    REQUIRE(result.has_value());
    REQUIRE(mock_config.GetConfigureCallCount() == 1);
}
```

### Pattern 2: Testing Scene Factory

```cpp
TEST_CASE("scene_factory::CreateScene with configurator") {
    TestContext test_context;
    FlatbuffersConfigurator configurator(test_context.GetEventHandler());
    
    // Load test data
    FlatbuffersDataLoader loader;
    auto fb_data = loader.ProvideDefaultSceneData(
        SceneType::SceneType_TITLE);
    REQUIRE(fb_data.has_value());
    
    // Create scene
    auto scene = scene_factory::CreateScene(
        SceneType::SceneType_TITLE,
        test_context.GetGameContext(),
        configurator,
        fb_data.value()->entity_collection());
    
    REQUIRE(scene.has_value());
    REQUIRE(scene.value()->GetSceneInfo().type == SceneType::SceneType_TITLE);
}
```

### Pattern 3: Testing SceneManager Data Source Control

```cpp
TEST_CASE("SceneManager::AddSceneFromDefault uses ISceneDataProvider") {
    TestContext test_context;
    SceneManager scene_manager(test_context.GetGameContext());
    
    // SceneManager should use ISceneDataProvider internally
    auto uuid = scene_manager.AddSceneFromDefault(
        SceneType::SceneType_TITLE);
    
    REQUIRE(uuid.has_value());
    
    // Verify scene was created
    const auto& scenes = scene_manager.GetScenes();
    REQUIRE(scenes.size() == 1);
    REQUIRE(scenes.contains(uuid.value()));
}
```

---

## Migration Checklist

### Phase 1: IEntityConfigurator Interface
- [ ] Create `src/entity/IEntityConfigurator.h`
- [ ] Add pure virtual `ConfigureEntities(EntityMemoryPool&, const void*)`
- [ ] Update `EntityConfigurator` to inherit from `IEntityConfigurator`
- [ ] Update CMakeLists.txt
- [ ] Run tests

### Phase 2: FlatbuffersConfigurator
- [ ] Add `ConfigureEntities` override to `FlatbuffersConfigurator`
- [ ] Keep `ConfigureEntitiesFromDefaultData` for backward compatibility
- [ ] Update internal implementation to use new method
- [ ] Update tests
- [ ] Run tests

### Phase 3: Scene Configuration
- [ ] Add `Scene::Configure(IEntityConfigurator&, const void*)`
- [ ] Update `EntityManager` to accept external configurator
- [ ] Deprecate `Scene::ConfigureFromDefault()` (add comment)
- [ ] Update Scene tests
- [ ] Run tests

### Phase 4: SceneFactory
- [ ] Convert `SceneFactory` to `scene_factory` namespace
- [ ] Create `CreateScene(type, context, configurator, data)`
- [ ] Update all scene type creation
- [ ] Update SceneFactory tests
- [ ] Run tests

### Phase 5: SceneManager
- [ ] Update `SceneManager::AddSceneFromDefault` to fetch data
- [ ] Add `SceneManager::AddSceneFromSave` stub
- [ ] Update convenience methods
- [ ] Update SceneManager tests
- [ ] Run full test suite

### Phase 6: Cleanup
- [ ] Remove `ConfigureFromDefault()` completely
- [ ] Remove `DataType` enum if unused
- [ ] Remove old test code
- [ ] Update documentation
- [ ] Final test run

---

## Quick Commands

### Build and Test After Changes

```bash
# Build
cmake --preset Debug
cmake --build --preset Debug

# Test specific areas
ctest --preset Debug -R Scene
ctest --preset Debug -R Entity
ctest --preset Debug -R SceneFactory
ctest --preset Debug -R SceneManager

# Full test suite
ctest --preset Debug
```

### Check for FlatBuffers Leakage

```bash
# Search for FlatBuffers types in public headers
grep -r "EntityCollection\|SceneDataData\|SceneDataFbs" src/scenes/*.h
grep -r "EntityCollection\|SceneDataData\|SceneDataFbs" src/entity/*.h

# Should only appear in:
# - FlatbuffersConfigurator.cpp (implementation)
# - FlatbuffersDataLoader.h/cpp
# - Generated headers
```

---

## Related Documentation

- [Full Proposal](SCENE_CONFIGURATION_DECOUPLING.md) - Complete design document
- [Architecture Diagrams](SCENE_CONFIGURATION_DECOUPLING_DIAGRAMS.md) - Visual representations
- [ISceneDataProvider](../../src/data_providers/ISceneDataProvider.h) - Scene data interface
- [ISaveDataProvider](../../src/configuration/ISaveDataProvider.h) - Save data interface
