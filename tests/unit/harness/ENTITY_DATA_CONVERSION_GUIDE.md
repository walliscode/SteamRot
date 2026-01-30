# TestEngine Entity Data Conversion Guide

## Problem

When `TestData` is loaded from FlatBuffers (via `FlatbuffersTestDataProvider`), the `entity_transport` variant in `SceneData` holds `const EntityCollectionFbs*` pointers. However, when `TestEngine` captures snapshots during execution, it converts these to `EntityMemoryPool` instances (see `TestEngine::StoreEngineSnapShot()` lines 109-128).

When comparing snapshots using `EqualsEngineSnapshot`, the `EntityTransportEqualsMatcher` only does shallow comparison for `EntityCollectionFbs*` (checking pool size and entity count, not deep entity data). This means tests that compare FlatBuffers data with EntityMemoryPool data will fail due to variant type mismatch.

## Solution

Use the `ConvertAllSceneEntityData()` helper function to convert FlatBuffers entity data to `EntityMemoryPool` format before comparison.

## When to Use Conversion

### ✅ Convert when:
1. **TestData loaded from FlatBuffers** - The `entity_transport` contains `const EntityCollectionFbs*`
2. **Comparing with TestEngine snapshots** - TestEngine converts to `EntityMemoryPool` in its data bank
3. **Deep entity data comparison needed** - You're actually comparing entity component data

### ❌ Don't convert when:
1. **Entity transport is monostate** - No entities to compare
2. **Manually created test data** - Already uses `EntityMemoryPool` or monostate
3. **Testing scene loading only** - Not comparing entity data

## Usage Pattern

### Pattern 1: Single Test with Entity Data

```cpp
TEST_CASE("TestEngine zero position matches config with entities", "[unit][TestEngine]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 2;
  steamrot::EventHandler mock_event_handler;

  // Create scene with FlatBuffers entity configurator
  steamrot::SceneData scene_data;
  scene_data.scene_info.type = steamrot::SceneType::TITLE;
  scene_data.scene_resources_config.texture_width = 512;
  scene_data.scene_resources_config.texture_height = 648;
  scene_data.entity_configurator =
      std::make_unique<steamrot::FlatbuffersEntityConfigurator>(mock_event_handler);
  // Assume entity_transport has EntityCollectionFbs* with actual entity data
  
  test_data.starting_engine_snapshot.scene_collection_data.push_back(
      std::move(scene_data));

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto startup_result = engine.StartUp();
  REQUIRE(startup_result.has_value());

  // Convert ALL scene entity data before comparison
  auto convert_result = steamrot::tests::ConvertAllSceneEntityData(
      test_data.starting_engine_snapshot);
  if (!convert_result.has_value()) {
    FAIL("ConvertAllSceneEntityData failed: " + convert_result.error().message);
  }

  // Assert - Now comparison will work
  const auto &data_bank = engine.GetDataBank();
  REQUIRE_THAT(data_bank.at(0), 
               steamrot::tests::EqualsEngineSnapshot(test_data.starting_engine_snapshot));
}
```

### Pattern 2: Loaded from FlatBuffers

```cpp
TEST_CASE("TestEngine runs simulation from JSON test data", "[unit][TestEngine][integration]") {
  // Arrange - Load from JSON
  std::filesystem::path obj_dir_path = std::filesystem::path(__FILE__).parent_path();
  FlatbuffersTestDataProvider provider(obj_dir_path);
  
  auto test_data_result = provider.ProvideAllTestData();
  REQUIRE(test_data_result.has_value());
  
  auto& test_data_vec = test_data_result.value();
  REQUIRE(!test_data_vec.empty());
  
  steamrot::TestData& test_data = test_data_vec[0];

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto startup_result = engine.StartUp();
  REQUIRE(startup_result.has_value());
  
  auto run_result = engine.RunGame();
  REQUIRE(run_result.has_value());

  // Convert entity data for comparison
  auto convert_result = steamrot::tests::ConvertAllSceneEntityData(
      test_data.starting_engine_snapshot);
  REQUIRE(convert_result.has_value());
  
  // Also convert expected snapshots if present
  for (auto& [tick, snapshot] : test_data.expected_engine_snapshots) {
    auto convert_expected = steamrot::tests::ConvertAllSceneEntityData(snapshot);
    REQUIRE(convert_expected.has_value());
  }

  // Assert - Compare snapshots
  const auto &data_bank = engine.GetDataBank();
  REQUIRE_THAT(data_bank.at(0), 
               steamrot::tests::EqualsEngineSnapshot(test_data.starting_engine_snapshot));
}
```

### Pattern 3: Testing Without Entities (No Conversion Needed)

```cpp
TEST_CASE("TestEngine handles scenes without entities", "[unit][TestEngine]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 1;

  steamrot::SceneData scene_data;
  scene_data.scene_info.type = steamrot::SceneType::TITLE;
  scene_data.scene_resources_config.texture_width = 256;
  scene_data.scene_resources_config.texture_height = 256;
  // entity_transport is monostate (default) - no entities

  test_data.starting_engine_snapshot.scene_collection_data.push_back(
      std::move(scene_data));

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto startup_result = engine.StartUp();
  REQUIRE(startup_result.has_value());

  // NO CONVERSION NEEDED - no entities to compare
  
  // Assert
  const auto &scene_manager = engine.GetSceneManager();
  REQUIRE(scene_manager.GetScenes().size() == 1);
}
```

## Helper Function Details

### `ConvertAllSceneEntityData(EngineSnapshot& snapshot)`

**Purpose**: Converts entity data in all scenes within an `EngineSnapshot` from FlatBuffers format to `EntityMemoryPool` format for comparison.

**Behavior**:
- Iterates through all `SceneData` in `snapshot.scene_collection_data`
- Skips scenes with `monostate` (no entities)
- Skips scenes already converted to `EntityMemoryPool`
- For `const EntityCollectionFbs*`:
  - Calls `ConvertEMPData()` which uses the `entity_configurator` to populate an `EntityMemoryPool`
  - Replaces the variant with the populated `EntityMemoryPool`
- For `shared_ptr<EntityMemoryPool>`:
  - Dereferences to get `EntityMemoryPool` copy
  - Replaces the variant with the copy

**Returns**: `std::expected<std::monostate, FailInfo>`

**Error Conditions**:
- `entity_configurator` is null (need configurator to convert FlatBuffers)
- Configuration fails during conversion

## Implementation Details

### Why Conversion is Needed

1. **TestEngine.cpp Lines 109-128**: `StoreEngineSnapShot()` explicitly converts `shared_ptr<EntityMemoryPool>` to `EntityMemoryPool` (copy-constructed) when storing in data bank
   
2. **EntityTransportEqualsMatcher.cpp Lines 108-188**: When comparing `const EntityCollectionFbs*`, only does shallow comparison (entity count, pool size) with comment: "Deep comparison of entity data is not implemented here as it would be complex and is better handled by converting to EntityMemoryPool"

3. **Variant Type Mismatch**: If expected data has `EntityCollectionFbs*` but actual has `EntityMemoryPool`, matcher fails at line 34-46 due to different variant indices

### What ConvertEMPData Does

Located in `tests/harness/harness_runner.cpp` lines 83-108:

```cpp
std::expected<std::monostate, FailInfo> ConvertEMPData(SceneData &scene_data) {
  // 1. Check configurator exists
  if (scene_data.entity_configurator == nullptr) {
    return std::unexpected(FailInfo{...});
  }

  // 2. Create fresh EntityMemoryPool
  EntityMemoryPool emp;
  
  // 3. Configure from entity_transport (extracts EntityCollectionFbs* and populates EMP)
  auto configure_result =
      scene_data.entity_configurator->ConfigureEntityMemoryPoolFromSource(
          emp, scene_data.entity_transport);
  
  if (!configure_result) {
    return std::unexpected(configure_result.error());
  }

  // 4. Replace variant with populated EntityMemoryPool
  scene_data.entity_transport = std::move(emp);

  return std::monostate{};
}
```

## Testing Checklist

When writing TestEngine tests:

- [ ] Does test use `FlatbuffersTestDataProvider`? → **Need conversion**
- [ ] Does `SceneData` have `entity_configurator` set? → **Probably has entity data, need conversion**
- [ ] Does test compare `EngineSnapshot` with entities? → **Need conversion**
- [ ] Is `entity_transport` just `monostate`? → **No conversion needed**
- [ ] Only testing scene loading, not entity data? → **No conversion needed**

## Common Mistakes

### ❌ Wrong: Comparing without conversion

```cpp
// This will fail if entity_transport has EntityCollectionFbs*
REQUIRE_THAT(data_bank.at(0), EqualsEngineSnapshot(test_data.starting_engine_snapshot));
```

### ✅ Right: Convert before comparison

```cpp
// Convert first
auto convert_result = steamrot::tests::ConvertAllSceneEntityData(
    test_data.starting_engine_snapshot);
REQUIRE(convert_result.has_value());

// Now comparison works
REQUIRE_THAT(data_bank.at(0), EqualsEngineSnapshot(test_data.starting_engine_snapshot));
```

### ❌ Wrong: Converting each scene individually

```cpp
// Verbose and error-prone
for (auto& scene : test_data.starting_engine_snapshot.scene_collection_data) {
  auto result = ConvertEMPData(scene);
  REQUIRE(result.has_value());
}
```

### ✅ Right: Use helper for all scenes

```cpp
// Clean and handles edge cases
auto convert_result = steamrot::tests::ConvertAllSceneEntityData(
    test_data.starting_engine_snapshot);
REQUIRE(convert_result.has_value());
```

## Future Improvements

Consider these enhancements:

1. **Automatic conversion in matcher**: `EntityTransportEqualsMatcher` could auto-convert FlatBuffers when comparing with EntityMemoryPool
2. **TestData post-load hook**: Automatically convert after loading from FlatBuffers
3. **Builder pattern**: TestData builder that handles conversion internally

## See Also

- `tests/harness/harness_runner.h` - `ConvertAllSceneEntityData()` declaration
- `tests/harness/harness_runner.cpp` - Implementation
- `tests/unit/harness/TestEngine.test.cpp` - Example usage (lines 386-426)
- `tests/matchers/EntityTransportEqualsMatcher.cpp` - Why conversion is needed
- `tests/harness/TestEngine.cpp` - How TestEngine stores snapshots
