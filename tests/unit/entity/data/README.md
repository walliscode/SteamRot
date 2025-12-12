# Entity Test Data

This directory contains test data for the entity subsystem tests.

## Entity Collection Files

Entity collection files use the `EntityCollectionFbs` schema and follow the naming pattern `entity_collection_*.json`. These are automatically compiled to `.bin` files during the build process.

### Available Entity Collections

- `entity_collection_empty.json` - Empty entity collection with pool size 10
- `entity_collection_size_25.json` - Empty entity collection with pool size 25
- `entity_collection_no_components.json` - Single entity with no components
- `entity_collection_ui_basic.json` - Single entity with CUserInterface (with root element)
- `entity_collection_ui_no_root.json` - Single entity with CUserInterface (without root element - for failure testing)
- `entity_collection_grimoire_basic.json` - Single entity with CGrimoireMachina
- `entity_collection_multi_component.json` - Multiple entities with different components

### Adding New Entity Collections

1. Create a new JSON file following the naming pattern `entity_collection_*.json`
2. Define your entity collection using the EntityCollectionFbs schema:
   ```json
   {
     "entity_memory_pool_size": 10,
     "entities": [
       {
         "index": 0,
         "c_user_interface": { ... },
         "c_grimoire_machina": { ... }
       }
     ]
   }
   ```
3. The CMake build system will automatically compile it to a `.bin` file
4. In your test, load it using:
   ```cpp
   auto collection_data = GetFreshEntityCollection("entity_collection_yourname.bin");
   ```

### Schema Location

The entity collection schema is defined in `src/types/flatbuffers/entities/entities.fbs`.

### Build Integration

Entity collection JSON files are compiled by `cmake/FlatbuffersCompilation/CompileEntityTestData.cmake`, which is included in the build process.

## Test Data Files

Test data files use the `TestDataConfig` schema and follow the naming pattern `*.test_data.json`. These are used for more complex data-driven testing scenarios.

See `documentation/testing/TEST_DATA_CONFIGURATION.md` for details on the test data system.
