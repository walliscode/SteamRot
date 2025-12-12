# Entity Test Data

This directory contains test data for the entity subsystem tests.

## entity_test_data.json

This JSON file contains simple test configuration data that can be referenced when adding new test scenarios. The test code uses helper functions to build FlatBuffers entity collections in-memory based on these configurations.

### Helper Functions

The test file provides helper functions for creating common entity collection scenarios:

- `CreateEmptyCollection(pool_size)` - Empty entity collection
- `CreateUIEntity(ui_name, visible, with_root, pool_size)` - Entity with CUserInterface
- `CreateGrimoireEntity(pool_size)` - Entity with CGrimoireMachina  
- `CreateEntityNoComponents(pool_size)` - Entity with no components
- `CreateMultiComponentCollection()` - Multiple entities with different components

### Adding New Test Scenarios

To add a new test scenario:

1. Add an entry to `entity_test_data.json` for documentation purposes
2. Create a new helper function in the test file if needed, or use existing helpers
3. Use the helper function in your test case

This approach keeps the tests simple and focused on unit testing without the complexity of external binary file compilation.

## Test Data Files

Other test data files (`.test_data.json`) use the TestDataConfig schema for more complex data-driven testing scenarios.

See `documentation/testing/TEST_DATA_CONFIGURATION.md` for details on the test data system.
