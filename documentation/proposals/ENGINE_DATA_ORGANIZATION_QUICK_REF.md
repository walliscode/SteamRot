# Engine Data Organization - Quick Reference

**Main Document**: [ENGINE_DATA_ORGANIZATION.md](ENGINE_DATA_ORGANIZATION.md)  
**Visual Guide**: [ENGINE_DATA_ORGANIZATION_VISUALS.md](ENGINE_DATA_ORGANIZATION_VISUALS.md)

This is a quick reference guide for developers working with Engine data organization.

---

## The Three Categories

### 1. Core Resources (`EngineResources`)

**What**: Long-lived, global infrastructure resources

**Examples**: RenderWindow, EventHandler, AssetManager

**Lifetime**: Created at Engine construction, destroyed at Engine destruction

**Access**: Via `GameContext` references

**Add when**: Resource is global, long-lived, and infrastructure-related

---

### 2. Configuration (`EngineConfig`)

**What**: Settings loaded from files that configure behavior (includes user preferences)

**Examples**: Window size, framerate, vsync, user volume

**Lifetime**: Loaded at startup, mostly immutable during runtime

**Access**: Direct member access (read-only)

**Add when**: Data is loaded from files and configures engine behavior

---

### 3. Engine State (`EngineState`)

**What**: Runtime operational state that changes during execution

**Examples**: Running flag, subscriptions, quit request, performance metrics

**Lifetime**: Created and modified during engine operation

**Access**: Direct member access (mutable)

**Add when**: Data changes during runtime and is engine-specific bookkeeping

---

## Quick Decision Guide

### Where Should My New Data Go?

```
┌─ Is it long-lived and global?
│  ├─ YES → Is it complex with logic?
│  │        ├─ YES → Separate Manager (SceneManager-style)
│  │        └─ NO  → EngineResources
│  └─ NO  → Is it loaded from files?
│           ├─ YES → EngineConfig
│           └─ NO  → EngineState
```

---

## Common Patterns

### Pattern: Adding a Simple Resource to EngineResources

```cpp
struct EngineResources {
  sf::RenderWindow game_window;
  EventHandler event_handler;
  AssetManager asset_manager;
  sf::TcpSocket network_socket;  // NEW: Add here
};

// Add reference to GameContext
struct GameContext {
  sf::TcpSocket& network_socket;  // NEW: Add reference
};
```

### Pattern: Adding Configuration

```cpp
struct NewConfig {
  // Configuration fields
  uint32_t some_setting{100};
};

struct EngineConfig {
  DisplayConfig display;
  NewConfig new_category;  // NEW: Add subcategory
};

// Load in StartUp()
auto config = LoadEngineConfig();
m_engine_config.new_category = config.new_category;
```

### Pattern: Adding Engine State

```cpp
struct EngineState {
  bool running{false};
  std::vector<std::shared_ptr<Subscriber>> subscriptions;
  bool my_new_flag{false};  // NEW: Add state
};

// Use in logic
if (m_engine_state.my_new_flag) {
  // Handle
}
```

### Pattern: Adding a Complex Subsystem

```cpp
class Engine {
protected:
  EngineResources m_engine_resources;
  SceneManager m_scene_manager;
  AudioManager m_audio_manager;  // NEW: Complex subsystem
};

// Add reference if needed by scenes
struct GameContext {
  AudioManager& audio_manager;  // NEW: Add reference
};
```

---

## Naming Conventions

| Category | Struct Name | Member Prefix | Example Access |
|----------|-------------|---------------|----------------|
| Core Resources | `EngineResources` | `m_engine_resources` | `m_engine_resources.game_window` |
| Configuration | `*Config` | `m_engine_config` | `m_engine_config.display.width` |
| User Preferences | `UserPreferencesConfig` | (inside config) | `m_engine_config.user_preferences.volume` |
| Engine State | `*State` | `m_engine_state` | `m_engine_state.running` |
| Subsystems | `*Manager` | `m_*_manager` | `m_scene_manager` |

---

## What Belongs Where?

### ✅ EngineResources (Core Resources)

- ✅ RenderWindow
- ✅ EventHandler
- ✅ AssetManager
- ✅ Mouse position
- ✅ Loop number
- ✅ Simple sockets/connections
- ❌ Configuration data (→ EngineConfig)
- ❌ Runtime flags (→ EngineState)
- ❌ Complex subsystems (→ Manager)

### ✅ EngineConfig (Configuration)

- ✅ Window dimensions, title
- ✅ Framerate limit
- ✅ Fullscreen, vsync flags
- ✅ User volume, language
- ✅ Key bindings
- ✅ Debug flags
- ❌ Current mouse position (→ EngineResources, it's state)
- ❌ Loop number (→ EngineResources, it's a counter resource)
- ❌ Running flag (→ EngineState)

### ✅ EngineState (Engine State)

- ✅ Running/paused flags
- ✅ Subscriptions
- ✅ Quit requested flag
- ✅ Scene change requests
- ✅ Performance metrics (FPS, frame time)
- ✅ Engine-level error state
- ❌ Scene entities (→ Scene)
- ❌ Window object (→ EngineResources)
- ❌ Configuration (→ EngineConfig)

### ✅ Separate Manager (Subsystems)

- ✅ SceneManager (complex scene logic)
- ✅ DisplayManager (complex rendering)
- ✅ AudioManager (complex audio mixing)
- ✅ NetworkManager (complex networking)
- ❌ Simple resources (→ EngineResources)
- ❌ Configuration data (→ EngineConfig)

---

## Migration Checklist

When migrating existing data to new organization:

- [ ] Identify current location and usage
- [ ] Determine correct category using decision tree
- [ ] Add member to new category struct
- [ ] Create compatibility accessor (deprecated)
- [ ] Update usage sites one by one
- [ ] Remove old member once all sites updated
- [ ] Update GameContext if needed
- [ ] Update documentation

---

## Common Scenarios

### Scenario: Adding Audio Volume Control

**What**: User-configurable audio settings

**Category**: Configuration (user preferences)

**Why**: Loaded from files, user-configurable, mostly read-only

**Location**: `EngineConfig.user_preferences` (as subcategory)

```cpp
struct UserPreferencesConfig {
  float master_volume{1.0f};
  // ... other user preferences
};

struct AudioConfig {
  float music_volume{0.8f};
  float sfx_volume{1.0f};
};

struct EngineConfig {
  DisplayConfig display;
  UserPreferencesConfig user_preferences;  // Add here
  AudioConfig audio;
};
```

### Scenario: Adding FPS Counter

**What**: Runtime performance tracking

**Category**: Engine State

**Why**: Changes every frame, runtime state, not persisted

**Location**: `EngineState`

```cpp
struct PerformanceMetrics {
  float fps{0.0f};
  float frame_time_ms{0.0f};
};

struct EngineState {
  bool running{false};
  PerformanceMetrics performance;  // Add here
};
```

### Scenario: Adding Network Connection

**What**: TCP socket for multiplayer

**Category**: Core Resource (if simple) OR Subsystem (if complex)

**Decision**: If just a socket → EngineResources. If managing connections, sessions, protocols → NetworkManager

```cpp
// Simple case
struct EngineResources {
  sf::TcpSocket network_socket;  // Add here
};

// Complex case
class NetworkManager {
  // Connection pooling, protocol handling, etc.
};

class Engine {
  NetworkManager m_network_manager;  // Add as subsystem
};
```

---

## Anti-Patterns to Avoid

### ❌ Don't Mix Categories in One Struct

```cpp
// BAD: Mixing resources and state
struct EngineResources {
  sf::RenderWindow window;  // Resource
  bool running;             // State - doesn't belong here!
};

// GOOD: Separate by category
struct EngineResources {
  sf::RenderWindow window;
};
struct EngineState {
  bool running;
};
```

### ❌ Don't Put Complex Logic in Data Structs

```cpp
// BAD: Logic in data struct
struct EngineResources {
  void UpdateAllSystems() { /* complex logic */ }
};

// GOOD: Separate manager
class SystemManager {
  void UpdateAllSystems() { /* complex logic */ }
};
```

### ❌ Don't Create Ambiguous Names

```cpp
// BAD: Unclear category
struct EngineData {  // What kind of data?
  // Mix of everything
};

// GOOD: Clear category
struct EngineConfig {  // Clearly configuration
  // Only config data
};
struct EngineState {  // Clearly state
  // Only runtime state
};
```

---

## References

- **Full Proposal**: [ENGINE_DATA_ORGANIZATION.md](ENGINE_DATA_ORGANIZATION.md)
- **Visual Diagrams**: [ENGINE_DATA_ORGANIZATION_VISUALS.md](ENGINE_DATA_ORGANIZATION_VISUALS.md)
- **Related**: [ENGINE_ARCHITECTURE_IMPROVEMENTS.md](ENGINE_ARCHITECTURE_IMPROVEMENTS.md)
- **Implementation**: See migration guidance in main document

---

## Questions?

- **Q: Where does X go?**  
  A: Use the decision tree in this guide or the main document

- **Q: Can I add to multiple categories?**  
  A: If data serves multiple purposes, split it into logical parts per category

- **Q: What if I'm unsure?**  
  A: Default to the most specific category. If still unsure, ask for review

- **Q: How do I migrate existing data?**  
  A: Follow the migration checklist and gradual approach in the main document

- **Q: Do I need to refactor everything now?**  
  A: No! Use the new pattern for new data. Migrate old data gradually
