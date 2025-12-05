# JSON to Binary Conversion System

## Overview

The SteamRot build system uses FlatBuffers to convert JSON configuration files into binary `.bin` files at compile time. This provides:
- Fast loading times
- Type safety
- Schema validation
- Automatic generation for both production and test directories

## File Naming Conventions

### Pattern: `<name>.<schema_type>.json`

JSON files must follow this naming pattern where the `<schema_type>` portion matches a custom extension registered in the CMake configuration.

### Examples

| Schema | Custom Extension | JSON Filename | Generated Binary |
|--------|-----------------|---------------|------------------|
| `user_preferences.fbs` | `.preferences.json` | `default.preferences.json` | `default.preferences.bin` |
| `scene_data.fbs` | `.scene_data.json` | `title.scene_data.json` | `title.scene_data.bin` |
| `engine_config.fbs` | `.engine_config.json` | `default.engine_config.json` | `default.engine_config.bin` |
| `engine_data.fbs` | `.engine_data.json` | `default.engine_data.json` | `default.engine_data.bin` |
| `scene_manager_data.fbs` | `.scene_manager_data.json` | `default.scene_manager_data.json` | `default.scene_manager_data.bin` |

### Special Cases

For directories containing only ONE JSON file, a generic `.json` extension can be used:
- `assets.fbs` + `asset_manager/asset_manager.json` → `asset_manager.bin`
- `context_data.fbs` + `context/context_data.json` → `context_data.bin`

**Warning**: If a directory contains multiple JSON files, they MUST use different custom extensions, otherwise all files will be compiled with the same schema.

## Directory Structure

### Production Data
```
data/defaults/
├── engine/
│   ├── default.engine_config.json
│   └── default.engine_data.json
├── scene_manager/
│   └── default.scene_manager_data.json
├── scenes/
│   ├── title.scene_data.json
│   └── crafting.scene_data.json
├── preferences/
│   └── default.preferences.json
└── ...
```

### Test Data
Binary files are automatically copied to `tests/data/defaults/` with the same structure:
```
tests/data/defaults/
├── engine/
│   ├── default.engine_config.bin
│   └── default.engine_data.bin
├── scene_manager/
│   └── default.scene_manager_data.bin
└── ...
```

## CMake Configuration

File: `src/flatbuffers_headers/convert_json_to_binary.cmake`

### Adding a New Data Type

1. **Create FlatBuffers schema**: `src/flatbuffers_headers/<name>.fbs`
2. **Choose custom extension**: `.your_type.json`
3. **Create JSON file**: `data/defaults/<subdirectory>/<name>.<your_type>.json`
4. **Add CMake line**:
```cmake
flatbuffers_generate_for_type(<schema_name> ".<your_type>.json" "<subdirectory>")
```

### Current Registered Types

```cmake
flatbuffers_generate_for_type(ui_style ".styles.json" "ui_styles")
flatbuffers_generate_for_type(scene_data ".scene_data.json" "scenes")
flatbuffers_generate_for_type(fragments ".fragment.json" "fragments")
flatbuffers_generate_for_type(assets ".json" "asset_manager")
flatbuffers_generate_for_type(scene_manager_data ".scene_manager_data.json" "scene_manager")
flatbuffers_generate_for_type(engine_config ".engine_config.json" "engine")
flatbuffers_generate_for_type(engine_data ".engine_data.json" "engine")
flatbuffers_generate_for_type(context_data ".json" "context")
flatbuffers_generate_for_type(user_preferences ".preferences.json" "preferences")
```

### Macro Parameters

```cmake
flatbuffers_generate_for_type(<schema_name> <json_ext> <subdir>)
```

- `<schema_name>`: Name of the `.fbs` file (without extension)
- `<json_ext>`: Custom extension pattern to match (e.g., `.engine_config.json`)
- `<subdir>`: Subdirectory under `data/defaults/` and `tests/data/defaults/`

## Build Process

1. **Configure**: CMake scans for JSON files matching registered extensions
2. **Generate**: For each match, runs `flatc --binary` to create `.bin` file
3. **Copy**: Copies production binaries to test directory
4. **Target**: `flatbuffers_generate_binaries` ensures all binaries are built

### Manual Build (for reference)

```bash
# From build directory
cmake --preset Debug
cmake --build --preset Debug --target flatbuffers_generate_binaries
```

## Common Issues

### Issue: JSON file not being converted

**Symptoms**: 
- File exists in `data/defaults/<dir>/` but no `.bin` generated
- Error: "file not found" when loading data

**Causes**:
1. **Wrong subdirectory in CMake**: Subdirectory parameter doesn't match actual directory name
2. **Wrong extension**: JSON file doesn't end with registered extension
3. **Multiple files with same extension**: Using `.json` for multiple files in same directory
4. **Wrong DIRECTORY_ROOTS**: CMake not searching in correct root directories (should be `data/defaults/` not `data/`)

**Solutions**:
1. Check CMake subdirectory matches actual directory: `"engine"` not `"engine_data"`
2. Rename file to use custom extension: `default.engine_config.json` not `engine_config.json`
3. Use unique extensions for each file type in the directory
4. Verify DIRECTORY_ROOTS in convert_json_to_binary.cmake points to `data/defaults/` and `tests/data/defaults/`

### Issue: Binary copied to wrong test location

**Cause**: Subdirectory parameter in CMake doesn't match actual directory structure

**Solution**: Update CMake macro call to use correct subdirectory name

## Best Practices

1. **Use descriptive prefixes**: `default.`, `title.`, `crafting.` help identify file purpose
2. **Match schema names**: Custom extension should relate to schema name (`.engine_config.json` for `engine_config.fbs`)
3. **One schema per extension**: Never reuse extension patterns for different schemas
4. **Document in comments**: Add comments in CMake for complex configurations
5. **Test after changes**: Verify binaries generate in both `data/` and `tests/data/`

## Related Files

- CMake configuration: `src/flatbuffers_headers/convert_json_to_binary.cmake`
- Schema generation: `src/flatbuffers_headers/generate_flatbuffers_headers.cmake`
- Data loading: `src/data_handlers/FlatbuffersDataLoader.cpp`
- Path configuration: `src/utilities/paths.h`

## Migration Notes

### Recent Changes (December 2024)

Fixed directory mismatches and file naming:
- **Fixed DIRECTORY_ROOTS**: Changed from `data/` and `tests/data/` to `data/defaults/` and `tests/data/defaults/` to match actual file locations
- Changed subdirectories to match actual structure (`"scenes"` not `"scene_data"`)
- Renamed engine files to use specific extensions:
  - `engine_config.json` → `default.engine_config.json`
  - `engine_data.json` → `default.engine_data.json`
- Renamed scene_manager file:
  - `scene_manager_data.json` → `default.scene_manager_data.json`
- Added separate CMake lines for `engine_config` and `engine_data`

These changes ensure proper conversion and avoid conflicts when multiple JSON files exist in the same directory.
