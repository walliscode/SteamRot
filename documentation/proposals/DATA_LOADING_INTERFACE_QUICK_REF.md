# Data Loading Interface System - Quick Reference

## TL;DR

Replace FlatBuffers-specific loaders with format-agnostic interfaces that return native C++ structs.

**Before**:
```cpp
FlatbuffersDataLoader loader;
const EngineCoreData* fb_data = loader.ProvideEngineCoreData().value();  // FlatBuffers type!
```

**After**:
```cpp
IEngineDataProvider& provider = GetEngineDataProvider();
EngineCoreData data = provider.LoadEngineCoreData().value();  // Native struct!
```

---

## Naming Conventions

### Game-Facing Structs (Simplest Names)
```cpp
struct EngineData { ... };      // NOT: EngineCoreDataFlatbuffers
struct SceneData { ... };       // NOT: SceneDataData
struct AssetData { ... };       // NOT: AssetCollection
```

### Interfaces (Format-Agnostic)
```cpp
class IEngineDataProvider { ... };    // NOT: FlatbuffersEngineDataLoader
class ISceneDataProvider { ... };     // NOT: ISceneDataFlatbuffersProvider
class IAssetDataProvider { ... };
```

### Implementations (Format-Specific)
```cpp
class FlatbuffersEngineDataProvider : public IEngineDataProvider { ... };
class JsonSceneDataProvider : public ISceneDataProvider { ... };
class LuaConfigProvider : public IEngineDataProvider { ... };
```

---

## Pattern: Provider vs Configurator

### Use Provider (Interface) For:
- ✅ Loading external data (files, network)
- ✅ Format might vary (FlatBuffers, JSON, XML, Lua)
- ✅ Returns complete data structures
- ✅ Stateless/pure operations

**Example**: `IEngineDataProvider`, `ISceneDataProvider`, `IAssetDataProvider`

### Use Configurator (Class) For:
- ✅ Complex transformation logic
- ✅ Needs dependencies (EventHandler, AssetManager)
- ✅ Modifying existing objects
- ✅ Requires internal state

**Example**: `EntityConfigurator`, `StylesConfigurator`

### Use Free Function For:
- ✅ Simple data copying
- ✅ No state needed
- ✅ Pure operation

**Example**: `core::ConfigureGameCore()`, `core::ConfigureSceneCore()`

---

## Migration Phases

### Phase 1: Create Interfaces (1-2 weeks, Low Risk)
- Create native structs
- Create interfaces
- Implement FlatBuffers providers
- No changes to existing code

### Phase 2: Update Integration Points (2-3 weeks, Moderate Risk)
- Update Engine, SceneFactory, AssetManager
- Update free functions to take native structs
- Switch from FlatbuffersDataLoader to providers

### Phase 3: Cleanup (1 week, Low Risk)
- Deprecate FlatbuffersDataLoader
- Remove old code
- Update documentation

### Phase 4: Entity System (4-6 weeks, High Risk)
- **DEFER TO FUTURE**
- Entity/component system deeply integrated with FlatBuffers
- Separate project if needed

---

## Interface Template

```cpp
// 1. Define native struct
struct MyData {
  std::string name;
  int value{0};
};

// 2. Define interface
class IMyDataProvider {
public:
  virtual ~IMyDataProvider() = default;
  virtual std::expected<MyData, FailInfo> LoadData() const = 0;
};

// 3. Implement for FlatBuffers
class FlatbuffersMyDataProvider : public IMyDataProvider {
private:
  FlatbuffersDataLoader m_loader;
  
public:
  std::expected<MyData, FailInfo> LoadData() const override {
    auto fb_result = m_loader.LoadMyFlatbuffersData();
    if (!fb_result) return std::unexpected(fb_result.error());
    
    // Convert FlatBuffers to native
    MyData data;
    data.name = fb_result.value()->name()->str();
    data.value = fb_result.value()->value();
    return data;
  }
};

// 4. Create factory function
IMyDataProvider& GetMyDataProvider() {
  static FlatbuffersMyDataProvider provider;
  return provider;
}

// 5. Use in game code
auto data = GetMyDataProvider().LoadData().value();
```

---

## Good Examples Already in Codebase

### ISaveDataProvider ✅
```cpp
struct SaveData {
  struct Metadata {
    std::string save_name;
    uint32_t slot_index{0};
  } metadata;
  SceneType current_scene_type;
};

class ISaveDataProvider {
  virtual std::expected<SaveData, FailInfo> LoadSave(uint32_t slot) const = 0;
};

class FlatbuffersSaveDataProvider : public ISaveDataProvider { ... };
```

**Why it's good**:
- Native C++ struct, no FlatBuffers exposure
- Interface is format-agnostic
- Implementation handles FlatBuffers details

### IUserPreferencesProvider ✅
```cpp
struct UserPreferences {
  struct Display {
    bool fullscreen{false};
    bool vsync{true};
  } display;
  struct Audio {
    float master_volume{1.0f};
  } audio;
};

class IUserPreferencesProvider {
  virtual std::expected<UserPreferences, FailInfo> LoadPreferences() const = 0;
};
```

**Why it's good**:
- Clean nested structs
- No serialization format leaked
- Easy to test and mock

---

## Bad Examples to Avoid

### FlatbuffersDataLoader ❌
```cpp
class FlatbuffersDataLoader {
  // Returns FlatBuffers pointer!
  std::expected<const EngineData *, FailInfo> ProvideEngineData() const;
  
  // Format-specific name!
  std::expected<const SceneDataData *, FailInfo> ProvideDefaultSceneData(...) const;
};
```

**Problems**:
- Exposes FlatBuffers types to game code
- Can't swap implementations
- Format in class name

---

## Common Patterns

### Loading Engine Data
```cpp
// Provider pattern
IEngineDataProvider& provider = GetEngineDataProvider();
auto result = provider.LoadEngineCoreData();
if (result.has_value()) {
  const EngineCoreData& data = result.value();
  // Use native struct
}
```

### Configuring from Data
```cpp
// Load via provider
auto data = GetSceneDataProvider().LoadSceneCoreData(scene_type);

// Configure via free function or configurator
core::ConfigureSceneCore(scene_core, data.value());
```

### Error Handling
```cpp
auto result = provider.LoadData();
if (!result.has_value()) {
  // Handle error
  const FailInfo& error = result.error();
  return std::unexpected(error);
}
// Use result.value()
```

---

## Integration Points

| Current | Needs Update | New Pattern |
|---------|--------------|-------------|
| `Engine::StartUp()` | Use FlatbuffersDataLoader | Use IEngineDataProvider |
| `SceneFactory::CreateDefaultScene()` | Use FlatbuffersDataLoader | Use ISceneDataProvider |
| `AssetManager` | Use FlatbuffersDataLoader | Use IAssetDataProvider |
| `StylesConfigurator` | Takes FlatBuffers types | Takes native structs or merge into provider |
| `core::ConfigureGameCore()` | Takes FlatBuffers pointer | Takes native struct reference |

---

## Testing

### Unit Test Provider
```cpp
TEST_CASE("Provider loads native struct", "[unit][providers]") {
  MyDataProvider provider;
  
  auto result = provider.LoadData();
  REQUIRE(result.has_value());
  
  const auto& data = result.value();
  REQUIRE(!data.name.empty());
  REQUIRE(data.value > 0);
}
```

### Mock Provider for Testing
```cpp
class MockDataProvider : public IMyDataProvider {
  std::expected<MyData, FailInfo> LoadData() const override {
    MyData test_data;
    test_data.name = "Test";
    test_data.value = 42;
    return test_data;
  }
};

// Use in tests
MockDataProvider mock_provider;
auto data = mock_provider.LoadData().value();
```

---

## Directory Structure

```
src/
├── data_providers/           # NEW
│   ├── IEngineDataProvider.h
│   ├── FlatbuffersEngineDataProvider.h/cpp
│   ├── ISceneDataProvider.h
│   ├── FlatbuffersSceneDataProvider.h/cpp
│   ├── IAssetDataProvider.h
│   ├── FlatbuffersAssetDataProvider.h/cpp
│   ├── provider_factory.h/cpp
│   └── CMakeLists.txt
├── data_handlers/            # EXISTING (deprecate later)
│   ├── DataLoader.h
│   └── FlatbuffersDataLoader.h/cpp
└── configuration/            # EXISTING (good examples)
    ├── ISaveDataProvider.h   ✅
    ├── IUserPreferencesProvider.h  ✅
    └── ...
```

---

## Decision Tree

**Need to load external data?**
- YES → Create Provider Interface
  - **Data format might vary?**
    - YES → Provider is the right choice
    - NO → Could use free function, but Provider is still good
    
- NO → Not a Provider use case

**Need to transform data into runtime objects?**
- YES → Create Configurator
  - **Needs dependencies?** (EventHandler, AssetManager, etc.)
    - YES → Class-based Configurator
    - NO → Free function Configurator
    
- NO → Just use the data directly

**Just copying data into struct?**
- YES → Free function is perfect
- NO → Need Configurator or Provider

---

## Key Takeaways

1. **Native structs everywhere** - Game code never sees FlatBuffers types
2. **Interfaces abstract formats** - Easy to add JSON, XML, Lua later
3. **Good examples exist** - `ISaveDataProvider` and `IUserPreferencesProvider` show the way
4. **Incremental migration** - Can implement alongside existing code
5. **Entity system deferred** - Too complex for initial phases
6. **Testing is easier** - Mock providers in tests
7. **Clean separation** - Loading (Provider) vs Configuration (Configurator) vs Usage (Game Code)

---

## Next Steps

1. Review full analysis: `DATA_LOADING_INTERFACE_SYSTEM.md`
2. Get feedback on approach
3. Start Phase 1: Create interfaces
4. Implement providers one at a time
5. Test thoroughly before moving integration points
6. Update documentation as you go

---

## Resources

- **Full Analysis**: `documentation/proposals/DATA_LOADING_INTERFACE_SYSTEM.md`
- **Good Examples**: 
  - `src/configuration/ISaveDataProvider.h`
  - `src/configuration/IUserPreferencesProvider.h`
- **Current Loader**: `src/data_handlers/FlatbuffersDataLoader.h`
- **Current Configurators**:
  - `src/entity/FlatbuffersConfigurator.h`
  - `src/core/core_configuration.h`
  - `src/user_interface/styles/StylesConfigurator.h`
