# User Preferences Configuration

## Overview

The user preferences system provides a way to persist user-level settings
separately from gameplay data (saves) and engine configuration (static defaults).

## Architecture

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         CONFIGURATION HIERARCHY                              │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  ┌───────────────────────┐                                                  │
│  │   Engine Defaults     │  Static, compile-time configuration              │
│  │   (context_data.json) │  Window size, framerate, scene pools             │
│  └───────────────────────┘                                                  │
│              │                                                               │
│              ▼                                                               │
│  ┌───────────────────────┐                                                  │
│  │   User Preferences    │  User-adjustable settings                        │
│  │   (user_preferences)  │  Audio, display, accessibility                   │
│  └───────────────────────┘                                                  │
│              │                                                               │
│              │   Overrides engine defaults where applicable                  │
│              ▼                                                               │
│  ┌───────────────────────┐                                                  │
│  │   Runtime Settings    │  Final merged settings used by engine            │
│  └───────────────────────┘                                                  │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

## User Preferences Categories

### Display Preferences

| Setting | Type | Default | Description |
|---------|------|---------|-------------|
| `window_width` | uint32 | 0 | Window width (0 = use engine default) |
| `window_height` | uint32 | 0 | Window height (0 = use engine default) |
| `fullscreen` | bool | false | Fullscreen mode |
| `vsync` | bool | true | Vertical sync enabled |
| `target_framerate` | uint32 | 60 | Target frames per second |

### Audio Preferences

| Setting | Type | Default | Description |
|---------|------|---------|-------------|
| `master_volume` | float | 1.0 | Master volume (0.0 - 1.0) |
| `music_volume` | float | 0.8 | Music volume (0.0 - 1.0) |
| `sfx_volume` | float | 1.0 | Sound effects volume (0.0 - 1.0) |
| `muted` | bool | false | Master mute state |

### Accessibility Preferences

| Setting | Type | Default | Description |
|---------|------|---------|-------------|
| `ui_scale` | float | 1.0 | UI scaling factor |
| `preferred_font` | string | "" | Preferred font name (empty = default) |

## File Locations

- **Default preferences**: Built into application code
- **User preferences**: `{data_dir}/preferences/user_preferences.bin`

## Usage

### Loading Preferences

```cpp
#include "FlatbuffersUserPreferencesProvider.h"

steamrot::FlatbuffersUserPreferencesProvider provider;

// Load preferences (returns defaults if no user file)
auto result = provider.LoadPreferences();
if (result.has_value()) {
    const auto& prefs = result.value();
    
    // Apply display settings
    if (prefs.display.window_width > 0) {
        // Use user's preferred window size
    }
    
    // Apply audio settings
    SetMasterVolume(prefs.audio.master_volume);
}
```

### Saving Preferences

```cpp
steamrot::FlatbuffersUserPreferencesProvider provider;

steamrot::UserPreferences prefs;
prefs.audio.master_volume = 0.75f;
prefs.display.fullscreen = true;

auto result = provider.SavePreferences(prefs);
if (!result.has_value()) {
    // Handle error
}
```

### Checking for User Preferences

```cpp
steamrot::FlatbuffersUserPreferencesProvider provider;

if (provider.HasUserPreferences()) {
    // User has saved preferences
} else {
    // Using defaults
}
```

## Interface Design

The system uses an interface `IUserPreferencesProvider` to allow for different
implementations:

```cpp
class IUserPreferencesProvider {
public:
    virtual std::expected<UserPreferences, FailInfo> LoadPreferences() const = 0;
    virtual std::expected<std::monostate, FailInfo> SavePreferences(const UserPreferences&) = 0;
    virtual bool HasUserPreferences() const = 0;
    virtual UserPreferences GetDefaultPreferences() const = 0;
};
```

This allows:
- `FlatbuffersUserPreferencesProvider` for file-based storage
- Mock implementations for testing
- Future cloud-based storage implementations

## FlatBuffers Schema

See `src/flatbuffers_headers/user_preferences.fbs` for the complete schema.

## Relationship to Other Systems

### vs. Context Configuration (context_data.json)

- **Context data**: Engine-level static configuration (scene pool sizes, etc.)
- **User preferences**: User-adjustable settings that may override context data

### vs. Save Data

- **User preferences**: Settings (audio, display) - persist across all games
- **Save data**: Gameplay progress - specific to a save slot

## Best Practices

1. **Always provide defaults**: Ensure the engine works without any user files
2. **Graceful degradation**: If loading fails, fall back to defaults
3. **Version migration**: Use the `version` field for future schema updates
4. **Validate values**: Check ranges (e.g., volume 0.0-1.0) when loading

## Future Enhancements

- Controls/keybind preferences
- Language/localization preferences
- Cloud sync support
- Preferences UI in-game
