# DataProvider Architecture

## Overview

DataProvider classes follow a **separation of concerns** pattern:
- **File Loading**: Handled by `FlatbuffersDataLoader` 
- **Data Conversion**: Handled by DataProvider classes

This allows flexibility in how data is provided to the system.

## Usage Patterns

### Pattern 1: Load from File (Traditional)

This is the original pattern and continues to work unchanged:

```cpp
// Create provider
steamrot::FlatbuffersSceneDataProvider provider;

// Load and convert in one step
auto result = provider.ProvideDefaultSceneData(steamrot::SceneType::SceneType_TITLE);

if (result.has_value()) {
    std::unique_ptr<steamrot::SceneData> scene_data = std::move(result.value());
    // Use scene_data...
}
```

### Pattern 2: Separate Loading and Conversion (New)

This pattern allows you to:
- Load data once, convert multiple times
- Pass pre-loaded data from different sources
- Test conversion logic independently

```cpp
// Step 1: Load FlatBuffers data
steamrot::FlatbuffersDataLoader loader;
auto load_result = loader.ProvideDefaultSceneData(steamrot::SceneType::SceneType_TITLE);

if (!load_result.has_value()) {
    // Handle loading error
    return;
}

const steamrot::SceneDataFbs* fb_scene_data = load_result.value();

// Step 2: Convert FlatBuffers to native type
steamrot::FlatbuffersSceneDataProvider provider;
auto convert_result = provider.ConvertSceneData(fb_scene_data);

if (convert_result.has_value()) {
    std::unique_ptr<steamrot::SceneData> scene_data = std::move(convert_result.value());
    // Use scene_data...
}
```

## Available Conversion Methods

All DataProvider interfaces now provide conversion methods:

### ISceneDataProvider

```cpp
// Load from file
std::expected<std::unique_ptr<SceneData>, FailInfo>
ProvideDefaultSceneData(const SceneType scene_type) const;

// Convert pre-loaded data
std::expected<std::unique_ptr<SceneData>, FailInfo>
ConvertSceneData(const SceneDataFbs* fb_scene_data) const;
```

### IEngineDataProvider

```cpp
// Load from file
std::expected<EngineData, FailInfo> LoadEngineData() const;

// Convert pre-loaded data
std::expected<EngineData, FailInfo>
ConvertEngineData(const EngineDataFbs* fb_engine_data) const;
```

### ISaveDataProvider

```cpp
// Load from file
std::expected<SaveData, FailInfo> ProvideSaveData() const;

// Convert pre-loaded data
std::expected<SaveData, FailInfo>
ConvertSaveData(const SaveDataFbs* fb_save_data) const;
```

### ISceneManagerDataProvider

```cpp
// Load from file
std::expected<SceneManagerData, FailInfo> ProvideSceneManagerData() const;

// Convert pre-loaded data
std::expected<SceneManagerData, FailInfo>
ConvertSceneManagerData(const SceneManagerDataFbs* fb_scene_manager_data) const;
```

### IUIStyleDataProvider

```cpp
// Load from file
std::expected<std::vector<UIStyle>, FailInfo> ProvideUIStyles();

// Convert pre-loaded data
std::expected<std::vector<UIStyle>, FailInfo>
ConvertUIStyles(const std::vector<const UIStyleData*>& fb_styles);
```

## Use Cases

### 1. Testing Conversion Logic

Test data conversion without file I/O:

```cpp
TEST_CASE("Test scene data conversion") {
    // Create test FlatBuffers data in memory
    flatbuffers::FlatBufferBuilder builder;
    // ... build test data ...
    const SceneDataFbs* test_data = GetSceneDataFbs(builder.GetBufferPointer());
    
    // Test conversion
    FlatbuffersSceneDataProvider provider;
    auto result = provider.ConvertSceneData(test_data);
    
    REQUIRE(result.has_value());
    // ... verify conversion ...
}
```

### 2. Caching Loaded Data

Load once, convert multiple times:

```cpp
class DataCache {
    std::unordered_map<SceneType, const SceneDataFbs*> cached_scenes;
    FlatbuffersDataLoader loader;
    
public:
    std::unique_ptr<SceneData> GetSceneData(SceneType type) {
        // Load and cache if not present
        if (cached_scenes.find(type) == cached_scenes.end()) {
            auto load_result = loader.ProvideDefaultSceneData(type);
            if (load_result.has_value()) {
                cached_scenes[type] = load_result.value();
            }
        }
        
        // Convert cached data
        FlatbuffersSceneDataProvider provider;
        auto convert_result = provider.ConvertSceneData(cached_scenes[type]);
        
        if (convert_result.has_value()) {
            return std::move(convert_result.value());
        }
        return nullptr;
    }
};
```

### 3. Alternative Data Sources

Load FlatBuffers data from network, database, or other sources:

```cpp
// Load from network instead of file
const SceneDataFbs* LoadFromNetwork(const std::string& url) {
    // ... custom loading logic ...
    return fb_data;
}

// Convert data regardless of source
FlatbuffersSceneDataProvider provider;
const SceneDataFbs* network_data = LoadFromNetwork("http://example.com/scene");
auto result = provider.ConvertSceneData(network_data);
```

## Benefits

1. **Separation of Concerns**: Loading and conversion are independent
2. **Testability**: Can test conversion without file I/O
3. **Flexibility**: Support different data sources (files, network, memory)
4. **Caching**: Load once, convert many times
5. **Backward Compatibility**: Existing code continues to work unchanged

## Implementation Notes

- All conversion methods validate input (null checks)
- Conversion methods are `const` (no side effects)
- Return types use `std::expected` for error handling
- FlatBuffers data lifetime is managed by caller
- The existing `Provide*` methods now internally use `Convert*` methods
