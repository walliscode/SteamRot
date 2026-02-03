# Entity Test Data

This directory contains test data for the entity subsystem tests.

## entity_test_data.json

This JSON file contains a single `EntityCollectionFbs` with multiple test entities covering various test scenarios. The file is compiled to `entity_test_data.bin` during the build process.

### Entity Index Reference

- **Entity 0**: No components (empty entity)
- **Entity 1**: CUserInterface (ui_name: "simple_ui", visible: true, simple panel)
- **Entity 2**: CUserInterface (ui_name: "hidden_ui", visible: false, simple panel)
- **Entity 3**: CGrimoireMachina (empty fragments/joints)
- **Entity 4**: CGrimoireMachina (with fragments: ["fragment_a", "fragment_b"], joints: ["joint_x", "joint_y"])
- **Entity 5**: CUserInterface (ui_name: "menu_ui", visible: true, simple panel)
- **Entity 6**: CUserInterface (ui_name: "settings_ui", visible: false, simple panel)
- **Entity 7**: CUIState (state_key: "main_menu", ui_names_on: ["simple_ui", "menu_ui"], ui_names_off: ["hidden_ui"])
- **Entity 8**: CUIState (state_key: "settings_screen", ui_names_on: ["settings_ui"], ui_names_off: ["simple_ui", "menu_ui"])
- **Entity 9**: CUIState (state_key: "game_running", ui_names_on: ["simple_ui"], with subscriber for Q key press)
- **Entity 10**: CUIState (multiple state keys: "multiple_states_a" and "multiple_states_b")

Pool size: 20

### Using in Tests

All tests share the same entity collection loaded via `LoadEntityTestData()`:

```cpp
// Load shared entity test data
auto test_data = LoadEntityTestData();
const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

// Create configurator
steamrot::FlatbuffersEntityConfigurator configurator(
    game_context.event_handler, *entity_collection);

// Create fresh EMP for test
steamrot::entity::memory::ResizeEntityMemoryPool(scene_context.scene_entities, 100);

// Test with entity at index 1 (UI with root)
auto &ui_component = steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
    1, scene_context.scene_entities);
```

### Adding New Test Scenarios

To add a new test scenario:

1. Add a new entity to `entity_test_data.json`
2. Update the entity index reference above
3. The JSON will be automatically compiled to binary during build
4. Use the new entity index in your test

## Test Data Files

Other test data files (`.test_data.json`) use the TestDataConfig schema for more complex data-driven testing scenarios.

See `documentation/testing/TEST_DATA_CONFIGURATION.md` for details on the test data system.
