# DataProvider Refactoring: Solution Summary

## Original Problem

From the issue:
> - for a DataProvider class such as the ISceneDataProvider (i also want to solve this problem generally)
> - currently we are providing the native struct by loading from file within in the concrete class.
> - however i want to be able to pass the concrete data type to it and get the native object out
> - e.g. with the FlatbuffersSceneDataProvider I would like to be able to pass SceneDataFbs* and get out a SceneData
> - how do i do this without breaking the interface approach?

## Solution Approach

The solution maintains the **interface approach** while enabling both use cases through **method overloading**:

### Before (Single Responsibility - Combined)

```cpp
class ISceneDataProvider {
public:
    // Only one method - loads from file AND converts
    virtual std::expected<std::unique_ptr<SceneData>, FailInfo>
    ProvideDefaultSceneData(const SceneType scene_type) const = 0;
};
```

**Problem:** Couldn't pass pre-loaded FlatBuffers data for conversion.

### After (Separated Responsibilities)

```cpp
class ISceneDataProvider {
public:
    // Method 1: Load from file AND convert (original - unchanged)
    virtual std::expected<std::unique_ptr<SceneData>, FailInfo>
    ProvideDefaultSceneData(const SceneType scene_type) const = 0;

    // Method 2: Convert pre-loaded data (NEW)
    virtual std::expected<std::unique_ptr<SceneData>, FailInfo>
    ConvertSceneData(const SceneDataFbs *fb_scene_data) const = 0;
};
```

**Benefits:**
- ✅ Can still load from file (backward compatible)
- ✅ Can now pass FlatBuffers data directly
- ✅ Interface approach preserved
- ✅ No breaking changes to existing code

## Implementation Pattern

Applied to all DataProvider interfaces:

1. **ISceneDataProvider** → Added `ConvertSceneData(const SceneDataFbs*)`
2. **IEngineDataProvider** → Added `ConvertEngineData(const EngineDataFbs*)`
3. **ISaveDataProvider** → Added `ConvertSaveData(const SaveDataFbs*)`
4. **ISceneManagerDataProvider** → Added `ConvertSceneManagerData(const SceneManagerDataFbs*)`
5. **IUIStyleDataProvider** → Added `ConvertUIStyles(const vector<UIStyleData*>&)`

## How It Works

### Original Implementation (Combined)

```cpp
std::expected<std::unique_ptr<SceneData>, FailInfo>
FlatbuffersSceneDataProvider::ProvideDefaultSceneData(
    const SceneType scene_type) const {
    
    // Step 1: Load from file
    FlatbuffersDataLoader data_loader;
    auto fb_data = data_loader.ProvideDefaultSceneData(scene_type).value();
    
    // Step 2: Convert (mixed with loading logic)
    FbsSceneData scene_data;
    // ... conversion code ...
    
    return std::make_unique<FbsSceneData>(scene_data);
}
```

### Refactored Implementation (Separated)

```cpp
// New method: Pure conversion (no file I/O)
std::expected<std::unique_ptr<SceneData>, FailInfo>
FlatbuffersSceneDataProvider::ConvertSceneData(
    const SceneDataFbs *fb_scene_data) const {
    
    // Validate input
    if (!fb_scene_data) {
        return std::unexpected(FailInfo{...});
    }
    
    // Pure conversion logic
    FbsSceneData scene_data;
    // ... conversion code ...
    
    return std::make_unique<FbsSceneData>(scene_data);
}

// Original method: Now uses the new convert method
std::expected<std::unique_ptr<SceneData>, FailInfo>
FlatbuffersSceneDataProvider::ProvideDefaultSceneData(
    const SceneType scene_type) const {
    
    // Step 1: Load from file
    FlatbuffersDataLoader data_loader;
    auto load_result = data_loader.ProvideDefaultSceneData(scene_type);
    if (!load_result.has_value()) {
        return std::unexpected(load_result.error());
    }
    
    // Step 2: Delegate to conversion method
    return ConvertSceneData(load_result.value());
}
```

## Usage Examples

### Use Case 1: Traditional (Backward Compatible)

```cpp
// Still works exactly as before
FlatbuffersSceneDataProvider provider;
auto result = provider.ProvideDefaultSceneData(SceneType::SceneType_TITLE);
```

### Use Case 2: Pass Pre-loaded Data (NEW)

```cpp
// Load FlatBuffers data from any source
FlatbuffersDataLoader loader;
const SceneDataFbs* fb_data = loader.ProvideDefaultSceneData(
    SceneType::SceneType_TITLE).value();

// Convert the FlatBuffers data
FlatbuffersSceneDataProvider provider;
auto result = provider.ConvertSceneData(fb_data);
```

### Use Case 3: Load Once, Convert Multiple Times

```cpp
// Load once
FlatbuffersDataLoader loader;
const SceneDataFbs* fb_data = loader.ProvideDefaultSceneData(
    SceneType::SceneType_TITLE).value();

// Convert multiple times (e.g., for different instances)
FlatbuffersSceneDataProvider provider;
auto scene1 = provider.ConvertSceneData(fb_data);
auto scene2 = provider.ConvertSceneData(fb_data);
auto scene3 = provider.ConvertSceneData(fb_data);
```

### Use Case 4: Alternative Data Sources

```cpp
// Load from network, database, cache, etc.
const SceneDataFbs* fb_data = LoadFromNetwork("http://example.com/scene");

// Convert regardless of source
FlatbuffersSceneDataProvider provider;
auto result = provider.ConvertSceneData(fb_data);
```

## Key Design Decisions

### 1. Method Overloading (Not Replacement)

**Decision:** Add new methods alongside existing ones
**Reason:** Maintains backward compatibility

### 2. Interface-Level Changes

**Decision:** Add new methods to interfaces, not just implementations
**Reason:** Enforces consistent pattern across all providers

### 3. Extraction Pattern

**Decision:** Extract conversion logic into new method, have old method call it
**Reason:** Avoids code duplication, maintains single source of truth

### 4. Null Checking

**Decision:** All conversion methods validate input parameters
**Reason:** Safety and clear error messages

## Benefits Achieved

1. **Separation of Concerns**: File loading and data conversion are independent
2. **Flexibility**: Support any data source (files, network, memory, cache)
3. **Testability**: Can test conversion logic without file I/O
4. **Reusability**: Load once, convert many times
5. **Backward Compatibility**: All existing code continues to work
6. **Type Safety**: FlatBuffers types are explicit in method signatures
7. **Interface Integrity**: Pattern applied consistently across all providers

## Testing Strategy

All new conversion methods include tests for:
- Null pointer handling
- Successful conversion with valid data
- Backward compatibility (existing tests still pass)

Example test:
```cpp
TEST_CASE("FlatbuffersSceneDataProvider::ConvertSceneData converts "
          "FlatBuffers data to native SceneData") {
    FlatbuffersSceneDataProvider provider;
    FlatbuffersDataLoader loader;
    
    // Load FlatBuffers data
    auto fb_data = loader.ProvideDefaultSceneData(
        SceneType::SceneType_TITLE).value();
    
    // Convert using new method
    auto result = provider.ConvertSceneData(fb_data);
    
    REQUIRE(result.has_value());
    // ... verify conversion ...
}
```

## Answer to Original Question

> "how do i do this without breaking the interface approach?"

**Answer:** By adding new methods to the interface alongside existing ones:

- ✅ Interface approach maintained (all providers implement same interface)
- ✅ No breaking changes (existing methods still work)
- ✅ New capability added (can pass FlatBuffers data directly)
- ✅ Consistent pattern applied across all DataProvider types
- ✅ Separation of concerns achieved (loading vs. conversion)

The key insight is that **interface approach doesn't mean single method** - it means consistent contract across implementations. We extended the contract without breaking it.
