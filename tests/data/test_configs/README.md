# Test Configuration Data

## Overview

Test configuration files use the existing FlatBuffers EntityData schema to define test scenarios. This approach leverages the production data loading infrastructure (FlatbuffersConfigurator, FlatbuffersDataLoader) without duplication.

## Structure

Test configuration files follow the same structure as scene files:
- JSON source files (`.test_config.json`)
- Compiled to binary FlatBuffers (`.test_config.bin`) via CMake
- Loaded using FlatbuffersDataLoader
- Configured using FlatbuffersConfigurator

## File Naming Convention

Use descriptive names that indicate what is being tested:
- `component_type_scenario.test_config.json`
- Examples:
  - `grimoire_default.test_config.json` - Default CGrimoireMachina configuration
  - `ui_simple_panel.test_config.json` - Simple UI panel test
  - `multiple_entities.test_config.json` - Multiple entity test

## Schema

Test files use the existing `entities.fbs` schema:

```fbs
table EntityData {
  index: uint32;
  c_user_interface: UserInterfaceData;
  c_grimoire_machina: GrimoireMachinaData;
  c_ui_state: UIStateData;
}

table EntityCollection {
  entities: [EntityData] (required);
  entity_memory_pool_size: int;
}
```

## Example Test Configuration

### Simple Component Test

```json
{
  "entity_memory_pool_size": 10,
  "entities": [
    {
      "index": 0,
      "c_grimoire_machina": {
        "fragments": ["fragment1", "fragment2"],
        "joints": ["joint1"]
      }
    }
  ]
}
```

### Multiple Component Test

```json
{
  "entity_memory_pool_size": 10,
  "entities": [
    {
      "index": 0,
      "c_user_interface": {
        "ui_name": "TestUI",
        "start_visible": true,
        "root_ui_element": {
          "base_data": {
            "position": { "x": 0.0, "y": 0.0 },
            "size": { "x": 100.0, "y": 100.0 },
            "children_active": false,
            "children": [],
            "layout": "None",
            "spacing_strategy": "None"
          }
        }
      },
      "c_grimoire_machina": {
        "fragments": ["frag1"],
        "joints": ["joint1"]
      }
    }
  ]
}
```

## Building Test Data

Test configuration files are automatically compiled to binary FlatBuffers during CMake build:

```bash
cmake --preset Debug
cmake --build --preset Debug
```

The CMake build system:
1. Finds all `.test_config.json` files in `tests/data/test_configs/`
2. Compiles them to `.test_config.bin` using flatc
3. Uses the `entities.fbs` schema

## Using Test Data in Tests

### Loading Test Configuration

```cpp
#include "FlatbuffersDataLoader.h"
#include "FlatbuffersConfigurator.h"
#include "PathProvider.h"

TEST_CASE("Load test configuration", "[unit][test_data]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersDataLoader loader;
  
  // Load test configuration
  auto result = loader.LoadEntityData("grimoire_default.test_config.bin");
  REQUIRE(result.has_value());
  
  const auto* entity_collection = result.value();
  REQUIRE(entity_collection->entities()->size() == 1);
}
```

### Configuring Entities from Test Data

```cpp
TEST_CASE("Configure entity from test data", "[unit][configurator]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersDataLoader loader;
  steamrot::FlatbuffersConfigurator configurator;
  
  EntityMemoryPool entity_pool;
  entity_pool.AllocateEntityMemory(10);
  
  // Load and configure
  auto result = configurator.ConfigureEntitiesFromDefaultData(
      entity_pool, "grimoire_default.test_config.bin");
  
  REQUIRE(result.has_value());
  
  // Verify configuration
  auto& grimoire = emp_helpers::GetComponent<CGrimoireMachina>(0, entity_pool);
  REQUIRE(grimoire.m_active == true);
  REQUIRE(grimoire.m_all_fragments.size() == 3);
}
```

## Benefits

1. **No Duplication**: Uses existing FlatBuffers schema and loading infrastructure
2. **Type Safety**: FlatBuffers provides compile-time type checking
3. **Binary Efficiency**: Compiled binary format is fast to load
4. **Schema Evolution**: FlatBuffers schema evolution handles versioning
5. **Production Parity**: Test data uses same format as production data
6. **Reusable**: Existing configurator logic works with test data

## Test Scenarios Covered

### Component Tests
- `grimoire_default.test_config.json` - CGrimoireMachina with fragments and joints
- `grimoire_empty.test_config.json` - CGrimoireMachina with empty collections
- `ui_simple_panel.test_config.json` - CUserInterface with simple panel
- `ui_button.test_config.json` - CUserInterface with button element

### Integration Tests
- `multiple_entities.test_config.json` - Multiple entities with different components
- `combined_components.test_config.json` - Single entity with multiple components

## Adding New Test Configurations

1. Create JSON file: `tests/data/test_configs/your_test.test_config.json`
2. Follow EntityCollection schema structure
3. Build project - CMake will compile to binary
4. Load in tests using FlatbuffersDataLoader
5. Configure using FlatbuffersConfigurator

## Validation

FlatBuffers compiler validates JSON files against the schema during build:
```bash
cmake --build --preset Debug
```

If there are schema errors, the build will fail with descriptive messages.

## Related Files

- Schema: `src/flatbuffers_headers/entities.fbs`
- Data Loader: `src/data_handlers/FlatbuffersDataLoader.h/cpp`
- Configurator: `src/entity/FlatbuffersConfigurator.h/cpp`
- Build Script: `src/flatbuffers_headers/convert_json_to_binary.cmake`
