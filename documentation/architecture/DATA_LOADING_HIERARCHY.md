# Data Loading Hierarchy

## Overview

The SteamRot engine uses a cascading configuration system that loads data from multiple sources in a defined priority order. This allows for shipped defaults, user customization, and per-save game state.

## Hierarchy Levels

The data loading hierarchy consists of three tiers:

```
1. Shipped Defaults (data/defaults/)
   └─> Always loaded first, provides baseline configuration
       
2. User Preferences (data/user/preferences/)
   └─> Overrides defaults with user-specific settings
       
3. Save Data (data/user/saves/)
   └─> Overrides both defaults and preferences with save-specific state
```

## Directory Structure

```
data/
├── defaults/              # Shipped with game (read-only in production)
│   ├── engine/           # Engine configuration
│   ├── context/          # Game context settings
│   ├── preferences/      # Default user preferences
│   ├── scenes/           # Scene data
│   ├── scene_manager/    # Scene manager configuration
│   └── asset_manager/    # Asset management settings
│
├── user/                 # User-specific data (read/write)
│   ├── preferences/      # User preference overrides
│   └── saves/            # Per-save game state
│
├── assets/               # Game assets (fonts, textures, etc.)
└── ui_styles/            # UI style definitions
```

## Loading Rules by Component

### Engine

**Tiers Used**: Defaults → User Preferences

**Rationale**: Engine configuration should be customizable per-user but not per-save.

**Examples**:
- Window size and position
- Graphics settings
- Audio settings
- Input mappings

**Loading Process**:
1. Load `data/defaults/engine/*.json`
2. If `data/user/preferences/engine/*.json` exists, merge/override
3. Result: Engine configured with user's preferred settings

### TitleScene (Static Scenes)

**Tiers Used**: Defaults ONLY

**Rationale**: Static scenes (title, main menu) should provide consistent experience across all users and saves.

**Examples**:
- Title screen layout
- Main menu structure
- Credits screen

**Loading Process**:
1. Load `data/defaults/scenes/title.scene_data.json`
2. No user preference or save data loading
3. Result: Consistent title screen for all users

### Gameplay Scenes (Dynamic Scenes)

**Tiers Used**: Defaults → User Preferences → Save Data

**Rationale**: Gameplay scenes need both user customization and per-save state.

**Examples**:
- Crafting scene with saved inventory
- Battle scene with saved party state
- World map with explored regions

**Loading Process**:
1. Load `data/defaults/scenes/crafting.scene_data.json`
2. If `data/user/preferences/scenes/crafting.*.json` exists, merge/override
3. If active save has `saves/<save_id>/scenes/crafting.*.json`, merge/override
4. Result: Scene with user preferences and current save state

### SceneManager

**Tiers Used**: Defaults → User Preferences

**Rationale**: Scene management settings are user-specific but not save-specific.

**Examples**:
- Scene transition preferences
- Scene cache settings

**Loading Process**:
1. Load `data/defaults/scene_manager/*.json`
2. If `data/user/preferences/scene_manager/*.json` exists, merge/override
3. Result: Scene manager configured with user preferences

### Game Context

**Tiers Used**: Defaults → User Preferences

**Rationale**: Context settings define the game environment and should be customizable.

**Examples**:
- Framerate limits
- Entity pool sizes
- Render texture dimensions

**Loading Process**:
1. Load `data/defaults/context/*.json`
2. If `data/user/preferences/context/*.json` exists, merge/override
3. Result: Game context with user-preferred settings

## Merge Strategy

### Simple Override
For scalar values (integers, booleans, strings), later tiers completely override earlier tiers:

```
Defaults:     window_width = 1920
User Prefs:   window_width = 1280
Result:       window_width = 1280
```

### Collection Merge
For collections (arrays, maps), later tiers can:
- **Add** new items
- **Override** existing items by key/ID
- **Remove** items (if explicitly marked for removal)

```
Defaults:     entities = [entity_1, entity_2]
Save Data:    entities = [entity_1_modified, entity_3]
Result:       entities = [entity_1_modified, entity_2, entity_3]
```

### Nested Object Merge
For nested objects, merge recursively:

```
Defaults:     player = { health: 100, mana: 50 }
Save Data:    player = { health: 75 }
Result:       player = { health: 75, mana: 50 }
```

## Implementation Status

### Current Implementation

The data loading hierarchy is **already implemented** in the existing codebase:

1. **Directory Structure**: Already established with `data/defaults/`, `data/user/preferences/`, and `data/user/saves/`

2. **FlatBuffers Support**: Schemas support loading from multiple sources via JSON

3. **Data Providers**: Existing data provider interfaces (IEngineDataProvider, ISceneDataProvider) handle data loading

### Phase 4 Documentation

Phase 4 of the refactoring **documents** this existing hierarchy and establishes clear rules:

- ✅ Document the three-tier hierarchy
- ✅ Establish per-component loading rules
- ✅ Define merge strategies
- ✅ Provide examples for each component
- ✅ Document directory structure

**Note**: The infrastructure already exists. This document formalizes the approach and provides guidance for future development.

## Usage Guidelines

### For Engine Developers

When adding new configuration:

1. **Determine the appropriate tier(s)**:
   - Does it need to be user-customizable? → Add User Preferences support
   - Does it need to be per-save? → Add Save Data support
   - Should it be static for all users? → Defaults only

2. **Place default files** in `data/defaults/<category>/`

3. **Document the loading behavior** in code comments

4. **Follow the established pattern** for the component type

### For Content Creators

When creating game content:

1. **Start with defaults**: All content should have shipped defaults

2. **Test user preferences**: If applicable, test with user preference overrides

3. **Test save data**: If applicable, test with save-specific state

4. **Validate merge behavior**: Ensure overrides work correctly

### For Players

Players can customize the game by:

1. **User Preferences**: Modify files in `data/user/preferences/`
   - Affects all saves and game sessions
   - Persists across game restarts

2. **Save Data**: Game automatically saves state in `data/user/saves/<save_id>/`
   - Specific to each save file
   - Preserves game progress

## Future Enhancements

### Potential Improvements

1. **UI for User Preferences**: Add in-game settings UI to modify user preferences
   - Options menu for engine settings
   - Scene-specific preference screens

2. **Save Data Compression**: Implement compression for save files
   - Reduce disk usage
   - Faster save/load times

3. **Cloud Sync**: Support syncing user preferences and save data
   - Cross-device progress
   - Backup and restore

4. **Mod Support**: Allow mod data to override defaults
   - Additional tier: Defaults → Mods → User Prefs → Save Data
   - Load mod content before user preferences

5. **Configuration Validation**: Add schema validation
   - Validate JSON against FlatBuffers schemas
   - Provide helpful error messages

## Related Documentation

- **Data Provider System**: `documentation/DATA_PROVIDER_SYSTEM.md`
- **FlatBuffers Schemas**: `src/flatbuffers_headers/`
- **Configuration Schemas**: `src/flatbuffers_headers/configuration/`
- **Engine Organization**: `documentation/proposals/ENGINE_DATA_ORGANIZATION.md`
- **Scene Organization**: This refactoring (Phases 0-3)

## Refactoring Context

This documentation was created as part of **Phase 4** of the Scene/SceneManager data organization refactoring:

- **Phase 0**: Cleanup - Removed unused code
- **Phase 1**: Scene organization - Applied Resources/Config/State pattern
- **Phase 2**: SceneManager organization - Applied same pattern
- **Phase 3**: Flatbuffers reorganization - Organized schemas into subdirectories
- **Phase 4**: Data Loading Hierarchy - **This document** (documentation-focused)

The hierarchy described here leverages the existing infrastructure and is now formally documented for consistency and future development.
