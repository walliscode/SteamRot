# Nested Data Pattern Diagrams

## Current vs. Proposed Architecture

### Current Architecture (Clunky)

```
┌─────────────────────────────────────────────────────────┐
│                   DataAccessFactory                      │
└────────┬────────────────────────────────────────────────┘
         │
         ├─► GetEngineDataProvider()
         │   │
         │   └─► IEngineDataProvider
         │       ├─► LoadEngineResourcesConfig() ──► EngineResourcesConfig
         │       ├─► LoadEngineConfig() ──────────► EngineConfig
         │       ├─► LoadEngineState() ───────────► EngineState
         │       └─► GetAssetConfigViewer()
         │           │
         │           └─► IAssetConfigViewer
         │               └─► ProvideAssetConfig() ──► AssetConfig
         │
         └─► GetSceneDataProvider()
             │
             └─► ISceneDataProvider
                 ├─► ProvideDefaultSceneData() ──► SceneData
                 └─► GetAssetConfigViewer() ──────► [Not Supported]

Problem: Multiple method calls required to get nested data!
```

### Proposed Architecture (Clean)

```
┌─────────────────────────────────────────────────────────┐
│                   DataAccessFactory                      │
└────────┬────────────────────────────────────────────────┘
         │
         ├─► GetEngineDataProvider()
         │   │
         │   └─► IEngineDataProvider
         │       └─► LoadEngineData() ──► EngineData
         │                                  ├─► resources_config
         │                                  ├─► engine_config
         │                                  ├─► engine_state
         │                                  └─► asset_config (nested!)
         │
         └─► GetSceneDataProvider()
             │
             └─► ISceneDataProvider
                 └─► ProvideDefaultSceneData() ──► SceneData
                                                     ├─► scene_info
                                                     └─► asset_config (nested!)

Solution: Single method call returns complete structure with nested data!
```

## Data Structure Comparison

### Before: Split Data Structures

```
┌──────────────────────────┐     ┌──────────────────────────┐
│  EngineResourcesConfig   │     │      EngineConfig        │
├──────────────────────────┤     ├──────────────────────────┤
│ - window_width           │     │ - display                │
│ - window_height          │     │ - user_preferences       │
│ - window_title           │     └──────────────────────────┘
│ - framerate_limit        │
└──────────────────────────┘     ┌──────────────────────────┐
                                  │      EngineState         │
┌──────────────────────────┐     ├──────────────────────────┤
│      AssetConfig         │     │ - running                │
├──────────────────────────┤     │ - paused                 │
│ - fonts                  │     │ - subscriptions          │
│ - ui_styles              │     │ - quit_requested         │
└──────────────────────────┘     └──────────────────────────┘

Separate structures loaded via different methods
Access pattern: provider->LoadX(), provider->GetViewer()->ProvideY()
```

### After: Unified Data Wrapper

```
┌─────────────────────────────────────────────────────────┐
│                      EngineData                          │
├─────────────────────────────────────────────────────────┤
│  EngineResourcesConfig resources_config                  │
│  ├─► window_width, window_height                         │
│  └─► window_title, framerate_limit                       │
│                                                           │
│  EngineConfig engine_config                              │
│  ├─► DisplayConfig display                               │
│  └─► UserPreferencesConfig user_preferences              │
│                                                           │
│  EngineState engine_state                                │
│  ├─► running, paused, quit_requested                     │
│  └─► vector<Subscriber> subscriptions                    │
│                                                           │
│  AssetConfig asset_config         ◄── Nested data!       │
│  ├─► vector<FontData> fonts                              │
│  └─► vector<string> ui_styles                            │
│                                                           │
│  Accessor Methods:                                        │
│  + GetAssetConfig() const                                │
│  + GetDisplayConfig() const                              │
│  + GetUserPreferences() const                            │
└─────────────────────────────────────────────────────────┘

Unified wrapper with all related data nested inside
Access pattern: provider->LoadEngineData() returns everything
```

## Call Flow Comparison

### Before: Multiple Calls Required

```
Client Code                Provider                     Viewers
    │                         │                            │
    ├──LoadResourcesConfig()─►│                            │
    │◄──EngineResourcesConfig─┤                            │
    │                         │                            │
    ├──LoadEngineConfig()────►│                            │
    │◄──EngineConfig──────────┤                            │
    │                         │                            │
    ├──LoadEngineState()─────►│                            │
    │◄──EngineState───────────┤                            │
    │                         │                            │
    ├──GetAssetConfigViewer()►│                            │
    │◄──IAssetConfigViewer────┤                            │
    │                         │                            │
    ├──ProvideAssetConfig()──────────────────────────────►│
    │◄──AssetConfig────────────────────────────────────────┤
    │                         │                            │

Result: 5 separate calls to get all engine data!
```

### After: Single Call Returns Everything

```
Client Code                Provider            Internal
    │                         │                   │
    ├──LoadEngineData()──────►│                   │
    │                         ├──(internal)───────►
    │                         │  Load all configs │
    │                         │  Load asset data  │
    │                         │  Build wrapper    │
    │                         │  Cache result     │
    │                         ◄──────────────────┤
    │◄──EngineData────────────┤                   │
    │  (with all nested data) │                   │
    │                         │                   │

Result: 1 call gets everything, nested data included!
```

## Provider Implementation Pattern

### Provider with Caching

```
┌─────────────────────────────────────────────────────────┐
│           FlatbuffersEngineDataProvider                  │
├─────────────────────────────────────────────────────────┤
│  Private:                                                │
│    - FlatbuffersDataLoader m_loader                      │
│    - optional<EngineData> m_cached_data  ◄── Cache!      │
│                                                           │
│  Public:                                                 │
│    + LoadEngineData() const                              │
│      │                                                    │
│      ├─► Check cache                                     │
│      │   └─► If cached: return m_cached_data            │
│      │                                                    │
│      ├─► If not cached:                                  │
│      │   ├─► Load EngineResourcesConfig                  │
│      │   ├─► Load EngineConfig                           │
│      │   ├─► Load EngineState                            │
│      │   ├─► Load AssetConfig (nested!)                  │
│      │   ├─► Build EngineData wrapper                    │
│      │   ├─► Store in m_cached_data                      │
│      │   └─► Return EngineData                           │
│      │                                                    │
│    + LoadEngineResourcesConfig() const                   │
│    + LoadEngineConfig() const                            │
│    + LoadEngineState() const                             │
│    + GetAssetConfigViewer() const                        │
│      (Old methods kept for backward compatibility)       │
└─────────────────────────────────────────────────────────┘

Pattern: Check cache first, load once, return quickly thereafter
```

## Nested Data Access Patterns

### Direct Member Access

```
EngineData engine_data = provider->LoadEngineData().value();

// Direct member access
engine_data.resources_config.window_width = 1920;
engine_data.engine_config.display.fullscreen = true;
engine_data.asset_config.fonts.push_back(font_data);
```

### Accessor Method Access

```
EngineData engine_data = provider->LoadEngineData().value();

// Using accessor methods (recommended for external access)
const AssetConfig& assets = engine_data.GetAssetConfig();
const DisplayConfig& display = engine_data.GetDisplayConfig();

// Accessors provide clean abstraction
for (const auto& font : assets.fonts) {
  LoadFont(font);
}
```

### Optional Nested Data

```
┌──────────────────────────────┐
│       GameSessionData        │
├──────────────────────────────┤
│ PlayerData player            │◄── Always present
│ optional<MultiplayerConfig>  │◄── May be absent
│                              │
│ + HasMultiplayer() const     │
│ + GetMultiplayer() const     │
└──────────────────────────────┘

Usage:
if (game_session.HasMultiplayer()) {
  const auto& mp = game_session.GetMultiplayer();
  // Use multiplayer config
}
```

## Migration Path

### Phase 1: Current State
```
[Existing Code]
├─► Multiple load methods
├─► Viewer pattern for nested data  
└─► No unified wrapper
```

### Phase 2: Add Wrappers (Backward Compatible)
```
[Existing Code] ◄── Still works!
├─► Multiple load methods
├─► Viewer pattern
└─► No unified wrapper

[New Code]
├─► Unified wrapper structs
├─► LoadEngineData() method (NEW)
└─► Caching support
```

### Phase 3: Migrate Callers
```
[Legacy Code] ◄── Still supported
├─► Some callers use old methods
└─► Marked as legacy

[Modern Code] ◄── Recommended
├─► Most callers use LoadEngineData()
└─► Clean, simple access
```

### Phase 4: Complete (Future)
```
[Modern Code Only]
└─► All callers use unified pattern
    ├─► Single load method
    ├─► Wrapper structs
    └─► Easy nested access
```

## Real-World Usage Scenarios

### Scenario 1: Engine Startup

```
Before:
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│   Engine    │────►│  Provider   │────►│   Viewers   │
│  StartUp()  │     │ 4 methods   │     │  1 method   │
└─────────────┘     └─────────────┘     └─────────────┘
     5 calls total to get all data

After:
┌─────────────┐     ┌─────────────┐
│   Engine    │────►│  Provider   │
│  StartUp()  │     │  1 method   │
└─────────────┘     └─────────────┘
     1 call gets everything
```

### Scenario 2: Scene Configuration

```
Before:
Scene needs assets ──► Get SceneDataProvider
                      ├─► ProvideDefaultSceneData()
                      └─► GetAssetConfigViewer() (not supported!)

After:
Scene needs assets ──► Get SceneDataProvider
                      └─► ProvideDefaultSceneData()
                          └─► Returns SceneData with asset_config nested
```

## Key Takeaways

1. **Unified Wrappers** = One structure contains everything
2. **Single Load Method** = One call gets all data
3. **Nested Directly** = No separate viewer calls needed
4. **Cached** = Load once, access many times
5. **Backward Compatible** = Old code still works
6. **Future Proof** = Easy to add more nested data

See full documentation for implementation details and examples.
