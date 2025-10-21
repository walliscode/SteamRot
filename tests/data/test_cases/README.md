# Test Cases Directory

This directory contains test data files for data-driven testing in the SteamRot game engine.

## File Structure

Each test case consists of:
- **JSON file** (`.json`): Human-readable test data definition
- **Binary file** (`.bin`): Compiled FlatBuffers binary (generated from JSON)

## File Naming Convention

Use descriptive names that indicate what is being tested:
- `entity_pool_basic.json` - Basic entity memory pool test
- `ui_collision_hover.json` - UI collision with hover state test
- `scene_transition_title_to_crafting.json` - Scene transition test

## Creating New Test Data

1. **Create JSON file** following the schema in `src/flatbuffers_headers/test_data.fbs`
2. **Convert to binary** using flatc:
   ```bash
   flatc --binary src/flatbuffers_headers/test_data.fbs tests/data/test_cases/your_test.json
   ```
3. **Use in tests** with `TestDataLoader`

## Example Structure

See `example_entity_pool.json` for a complete example.

Basic structure:
```json
{
  "version": 1,
  "test_case": {
    "metadata": {
      "test_name": "Test Name",
      "test_description": "What this test validates",
      "test_tags": ["unit", "component_type"],
      "expected_result": "PASS"
    },
    "before_state": { /* Initial state */ },
    "after_state": { /* Expected state after test */ }
  }
}
```

## Data Levels

Test data can represent different levels of game state:
- **EntityMemoryPoolData**: Entity pool with components
- **SceneData**: Scene-level data with multiple entity sets
- **EventBusData**: Event system state
- **ComponentData**: Specific component states

## Best Practices

1. **One test per file** - Each file should test one scenario
2. **Descriptive metadata** - Use clear test names and descriptions
3. **Minimal data** - Only include data relevant to the test
4. **Version control** - Commit both JSON and binary files
5. **Tags** - Use appropriate tags for test categorization

## Documentation

For detailed documentation on the test data system, see:
- `documentation/TEST_DATA_SYSTEM.md` - Complete system documentation
- `documentation/TESTING_IMPROVEMENT_PLAN.md` - Overall testing strategy

## Converting Files

To convert all JSON files to binary at once:
```bash
cd /home/runner/work/SteamRot/SteamRot
for f in tests/data/test_cases/*.json; do
  flatc --binary src/flatbuffers_headers/test_data.fbs "$f"
done
```
