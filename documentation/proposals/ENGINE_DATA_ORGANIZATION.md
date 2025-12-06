# Engine Data Organization Proposal

**Date**: December 6, 2025  
**Status**: Proposal  
**Type**: Architecture - Data Organization  
**Related**: ENGINE_ARCHITECTURE_IMPROVEMENTS.md  
**Visual Companion**: [ENGINE_DATA_ORGANIZATION_VISUALS.md](ENGINE_DATA_ORGANIZATION_VISUALS.md)

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Problem Statement](#problem-statement)
3. [Current State Analysis](#current-state-analysis)
4. [Proposed Organization Strategy](#proposed-organization-strategy)
5. [Detailed Categories](#detailed-categories)
6. [Implementation Patterns](#implementation-patterns)
7. [Migration Guidance](#migration-guidance)
8. [Future Extensibility](#future-extensibility)
9. [Examples and Code Patterns](#examples-and-code-patterns)

> **Note**: For visual diagrams, flowcharts, and illustrations, see the companion document [ENGINE_DATA_ORGANIZATION_VISUALS.md](ENGINE_DATA_ORGANIZATION_VISUALS.md)

---

## Executive Summary

This document proposes a clear, intuitive organization strategy for data and resources in the Engine class hierarchy. The current structure mixes global resources, configuration data, and engine-specific state without clear categorization, making it difficult to:

- Understand what data belongs where
- Add new resources without confusion
- Determine ownership and lifetime semantics
- Test components in isolation

**Proposed Solution**: Organize Engine data into **three distinct categories** with clear naming conventions:

1. **Core Resources** (`GameCore`) - Global, long-lived resources shared across the engine
2. **Configuration** (`EngineConfig`, `UserPreferences`) - Settings and preferences loaded at startup
3. **Engine State** (`EngineState`) - Engine-specific runtime state and operational data

This organization provides:
- **Clear ownership semantics** - Resources vs. configuration vs. state
- **Intuitive naming** - Category indicates purpose and lifetime
- **Easy extensibility** - New data has an obvious home
- **Better testability** - Categories can be mocked/injected independently

---

## Problem Statement

### Current Issues

The Engine class currently contains a mix of different types of data without clear organization:

```cpp
class Engine {
protected:
  GameCore m_game_core;              // Global resources
  GameContext m_game_context;        // References to core
  SceneManager m_scene_manager;      // Scene management subsystem
  UserPreferences m_user_preferences; // Configuration data
  bool m_running;                    // Engine state
  std::vector<std::shared_ptr<Subscriber>> m_subscriptions; // Engine state
};
```

**Problems**:

1. **Unclear categorization** - What is a "resource" vs. "configuration" vs. "state"?
2. **Non-intuitive naming** - `m_game_core` doesn't clearly indicate it holds global resources
3. **Mixed lifetimes** - Some data is initialized once, some changes every frame
4. **Difficult to extend** - Where should new global resources go? New config options? New engine state?
5. **Testing complexity** - Hard to mock or inject specific categories without affecting others

### User Feedback

> "i'm not quite happy with the division of data in the Engine or that it is a robust system going forward. I also dont think the naming is intuitive."

The user identifies three types of data:
1. **Global resources** - RenderWindow, EventHandler, AssetManager, MousePosition, etc.
2. **Configuration/miscellaneous data** - Settings, preferences, startup parameters
3. **Engine member data** - Subscribers and other engine-specific operational state

---

## Current State Analysis

### Current Data Distribution

#### In Engine Base Class
```cpp
class Engine {
protected:
  // Category: Core Resources (but not clearly named as such)
  GameCore m_game_core;              
  GameContext m_game_context;        
  SceneManager m_scene_manager;      
  
  // Category: Configuration (but mixed with state)
  UserPreferences m_user_preferences;
  
  // Category: Engine State (but not grouped)
  bool m_running;
  std::vector<std::shared_ptr<Subscriber>> m_subscriptions;
};
```

#### In GameCore
```cpp
struct GameCore {
  // All resources stored flat without categorization
  sf::RenderWindow game_window;
  EventHandler event_handler;
  sf::Vector2i mouse_position;
  size_t loop_number;
  AssetManager asset_manager;
};
```

#### In GameEngine
```cpp
class GameEngine : public Engine {
private:
  DisplayManager m_display_manager;  // Rendering subsystem
};
```

### Current Strengths

1. **GameCore exists** - Already groups some resources together
2. **GameContext provides references** - Convenient access pattern established
3. **Subsystems are separate** - SceneManager, DisplayManager are not mixed into core
4. **Base Engine pattern works** - Inheritance structure is sound

### Current Weaknesses

1. **No clear categorization** - Resources, config, and state are mixed
2. **Inconsistent naming** - No pattern to indicate category
3. **Flat structure** - Everything at same level, no hierarchy
4. **Poor discoverability** - Hard to find where to add new data
5. **Coupling** - Changes to one category can affect others

---

## Proposed Organization Strategy

### Three-Category System

Organize all Engine data into **three clear categories** with distinct purposes, lifetimes, and naming conventions:

```
Engine Data
├── 1. Core Resources (GameCore)
│   ├── Long-lived, global scope
│   ├── Shared across entire engine
│   └── Initialized once at startup
│
├── 2. Configuration (Config/Preferences)
│   ├── Loaded from files/user settings
│   ├── Mostly immutable during runtime
│   └── Can be saved/reloaded
│
└── 3. Engine State (EngineState)
    ├── Runtime operational data
    ├── Changes during execution
    └── Engine-specific bookkeeping
```

### Design Principles

1. **Semantic Naming** - Name clearly indicates category and purpose
2. **Single Responsibility** - Each category has one clear purpose
3. **Lifetime Clarity** - Category indicates when data is created/destroyed
4. **Access Patterns** - Category determines how data is accessed
5. **Extensibility** - New data has obvious categorization

### Category Decision Tree

When adding new data to Engine, ask:

```
┌─────────────────────────────────────┐
│ Is this a long-lived, shared        │
│ resource (window, manager, etc.)?   │
└────────┬───────────────┬────────────┘
         │ YES           │ NO
         ▼               ▼
    GameCore        ┌────────────────────────────┐
                    │ Is this loaded from files   │
                    │ and configures behavior?    │
                    └───────┬──────────┬─────────┘
                            │ YES      │ NO
                            ▼          ▼
                    Configuration  EngineState
```

---

## Detailed Categories

### Category 1: Core Resources (GameCore)

**Purpose**: Long-lived, global resources shared across the entire engine lifetime.

**Characteristics**:
- Created once during Engine construction
- Destroyed when Engine is destroyed
- Accessed via references throughout the codebase
- Provides core infrastructure (windowing, events, assets)

**Current Implementation**: `GameCore`

**Suggested Enhancement**: Organize into logical subcategories within GameCore

```cpp
struct GameCore {
  // Rendering & Display
  sf::RenderWindow game_window;
  
  // Event System
  EventHandler event_handler;
  sf::Vector2i mouse_position;
  
  // Asset Management
  AssetManager asset_manager;
  
  // Timing & Loop Control
  size_t loop_number;
};
```

**What Belongs Here**:
- ✅ RenderWindow - Core display resource
- ✅ EventHandler - Global event system
- ✅ AssetManager - Asset loading and caching
- ✅ Mouse position - Current input state
- ✅ Loop number - Frame counter
- ❌ User preferences - This is configuration, not a resource
- ❌ Subscriptions - This is engine state, not a core resource
- ❌ Scene-specific data - Belongs in SceneManager

**Naming Convention**: `GameCore` (already good), internal members use descriptive names

**Access Pattern**: 
```cpp
// Direct access via reference from GameContext
context.game_window.clear();
context.event_handler.RegisterSubscriber(sub);
context.asset_manager.GetFont("main");
```

### Category 2: Configuration (Config/Preferences)

**Purpose**: Settings and preferences loaded from files that configure engine behavior.

**Characteristics**:
- Loaded during StartUp() from data files
- Mostly immutable during runtime (read-only)
- Can be hot-reloaded in development
- User can override defaults
- Persists between sessions

**Current Implementation**: 
- `EngineCoreData` (loaded, then used to configure GameCore)
- `UserPreferences` (stored on Engine)
- `EngineConfig` (struct definition, not clearly used)

**Suggested Organization**: Group all configuration into clear structs

```cpp
// Window and display settings
struct DisplayConfig {
  uint32_t window_width{800};
  uint32_t window_height{600};
  std::string window_title{"SteamRot"};
  uint32_t framerate_limit{60};
  bool fullscreen{false};
  bool vsync{true};
};

// User preferences (gameplay, audio, etc.)
struct UserPreferences {
  float master_volume{1.0f};
  bool show_fps{false};
  std::string preferred_language{"en"};
  // ... other user settings
};

// Engine behavior configuration
struct EngineConfig {
  DisplayConfig display;
  // Future: Add other config categories
  // - InputConfig
  // - AudioConfig
  // - NetworkConfig
};
```

**What Belongs Here**:
- ✅ Window dimensions, title, framerate
- ✅ Fullscreen, vsync settings
- ✅ User volume, language preferences
- ✅ Keybindings, control schemes
- ✅ Debug/development flags
- ❌ Current mouse position - This is runtime state
- ❌ Loop number - This is runtime state
- ❌ Subscriptions - This is engine state

**Naming Convention**: `*Config` for engine settings, `*Preferences` for user settings

**Access Pattern**:
```cpp
// Loaded once at startup
auto config = LoadEngineConfig();
ConfigureFromConfig(m_game_core, config.display);

// Read-only access during runtime
if (m_engine_config.display.vsync) { /* ... */ }
```

### Category 3: Engine State (EngineState)

**Purpose**: Runtime operational data that changes during engine execution.

**Characteristics**:
- Created and modified during engine operation
- Changes every frame or in response to events
- Engine-specific bookkeeping
- Not persisted between sessions (unless explicitly saved)
- Distinct from game/scene state

**Current Implementation**: Scattered across Engine members

```cpp
class Engine {
protected:
  bool m_running;  // Runtime flag
  std::vector<std::shared_ptr<Subscriber>> m_subscriptions;  // Operational state
};
```

**Suggested Organization**: Group into `EngineState` struct

```cpp
struct EngineState {
  // Execution control
  bool running{false};
  bool paused{false};
  
  // Subscription management
  std::vector<std::shared_ptr<Subscriber>> subscriptions;
  
  // Diagnostics & debugging
  size_t ticks_executed{0};
  std::optional<std::chrono::time_point<std::chrono::steady_clock>> last_tick_time;
  
  // Engine-level flags
  bool quit_requested{false};
  bool scene_change_requested{false};
  std::optional<SceneType> requested_scene_type;
};
```

**What Belongs Here**:
- ✅ Running/paused flags
- ✅ Subscriptions (engine-level event handlers)
- ✅ Quit/scene change requests
- ✅ Tick counters, timing info
- ✅ Engine-level error state
- ❌ Scene entities - Belongs in Scene
- ❌ Window object - This is a core resource
- ❌ Configuration data - Belongs in Config

**Naming Convention**: `EngineState`, `*Status`, `*Flags`

**Access Pattern**:
```cpp
// Modified during execution
m_engine_state.running = true;
m_engine_state.subscriptions.push_back(sub);

// Checked in game loop
if (m_engine_state.quit_requested) {
  Shutdown();
}
```

---

## Implementation Patterns

### Pattern 1: Struct-Based Organization

**Use for**: Core Resources, Configuration

Group related data into cohesive structs with clear names:

```cpp
// GOOD: Clear category and contents
struct GameCore {
  sf::RenderWindow game_window;
  EventHandler event_handler;
  AssetManager asset_manager;
  // ...
};

// GOOD: Configuration grouped logically
struct DisplayConfig {
  uint32_t width;
  uint32_t height;
  std::string title;
};

// BAD: Flat, unclear categorization
class Engine {
  sf::RenderWindow window;
  uint32_t width;
  EventHandler handler;
  bool running;
  // Everything mixed together
};
```

### Pattern 2: Reference-Based Access

**Use for**: Accessing core resources from subsystems

Provide lightweight reference wrappers for convenient access:

```cpp
// Already implemented well
struct GameContext {
  GameCore& game_core;
  sf::RenderWindow& game_window;
  EventHandler& event_handler;
  // ... references to core resources
};

// Usage
void SomeSystem(const GameContext& ctx) {
  ctx.game_window.draw(sprite);  // Clear, convenient
}
```

### Pattern 3: Manager Pattern for Subsystems

**Use for**: Large, complex subsystems

Keep subsystems as separate managers rather than embedding in categories:

```cpp
// GOOD: Subsystem remains separate
class Engine {
protected:
  GameCore m_game_core;
  SceneManager m_scene_manager;
  // SceneManager is complex enough to be its own manager
};

// BAD: Embedding complex logic in data struct
struct GameCore {
  // Don't put SceneManager here - it's too complex
};
```

**Rule of Thumb**: 
- Simple resources → Go in GameCore
- Complex subsystems with logic → Separate manager classes

### Pattern 4: Configuration Cascade

**Use for**: Default → User overrides pattern

```cpp
// Load defaults
auto default_config = LoadDefaultConfig();

// Load user overrides (if they exist)
if (auto user_config = LoadUserConfig(); user_config.has_value()) {
  // Merge: user settings override defaults
  default_config.Merge(user_config.value());
}

// Use final configuration
m_engine_config = default_config;
```

### Pattern 5: State Machine for Engine State

**Use for**: Complex state transitions

```cpp
enum class EnginePhase {
  Initializing,
  Running,
  Paused,
  ShuttingDown
};

struct EngineState {
  EnginePhase phase{EnginePhase::Initializing};
  
  void Transition(EnginePhase new_phase) {
    // Validate transitions
    phase = new_phase;
  }
};
```

---

## Migration Guidance

### Step 1: Create New Category Structs (No Breaking Changes)

Add new structs alongside existing members:

```cpp
class Engine {
protected:
  // Existing (keep for now)
  GameCore m_game_core;
  GameContext m_game_context;
  SceneManager m_scene_manager;
  UserPreferences m_user_preferences;
  bool m_running;
  std::vector<std::shared_ptr<Subscriber>> m_subscriptions;
  
  // NEW: Add category structs
  EngineConfig m_engine_config;  // Will hold display config, etc.
  EngineState m_engine_state;    // Will hold running, subscriptions, etc.
};
```

### Step 2: Migrate Data Member by Member

Move members into appropriate categories gradually:

```cpp
// Before
class Engine {
  bool m_running;
  std::vector<std::shared_ptr<Subscriber>> m_subscriptions;
};

// After (Step 1: Add to new struct)
struct EngineState {
  bool running{false};
  std::vector<std::shared_ptr<Subscriber>> subscriptions;
};

class Engine {
  bool m_running;  // Keep temporarily
  std::vector<std::shared_ptr<Subscriber>> m_subscriptions;  // Keep temporarily
  EngineState m_engine_state;  // New
};

// After (Step 2: Update one usage site at a time)
// Change: m_running → m_engine_state.running

// After (Step 3: Remove old member once all usages updated)
class Engine {
  EngineState m_engine_state;
};
```

### Step 3: Update Access Patterns

Provide compatibility accessors during migration:

```cpp
class Engine {
private:
  EngineState m_engine_state;
  
public:
  // Temporary compatibility: Delegate to new location
  [[deprecated("Use GetEngineState().subscriptions")]]
  std::vector<std::shared_ptr<Subscriber>>& GetSubscriptions() {
    return m_engine_state.subscriptions;
  }
  
  // New access pattern
  EngineState& GetEngineState() { return m_engine_state; }
  const EngineState& GetEngineState() const { return m_engine_state; }
};
```

### Step 4: Update GameContext if Needed

If state needs to be accessed in subsystems, add to GameContext:

```cpp
struct GameContext {
  GameCore& game_core;
  EngineState& engine_state;  // NEW: Add reference to state
  
  // Convenience references (existing)
  sf::RenderWindow& game_window;
  EventHandler& event_handler;
  // ...
};
```

### Step 5: Document New Patterns

Update documentation to reflect new organization:

```cpp
/// @brief Engine execution state (flags, subscriptions, timing).
///
/// This struct contains runtime state that changes during engine operation.
/// It is distinct from:
/// - GameCore: Long-lived global resources
/// - EngineConfig: Configuration loaded at startup
struct EngineState {
  // ...
};
```

---

## Future Extensibility

### Adding New Global Resources

**Question**: Where should a new global resource go?

**Decision Tree**:
1. Is it a long-lived, global infrastructure component?
   - **YES** → Add to `GameCore`
   - **NO** → Continue to next question

2. Is it a scene-specific resource?
   - **YES** → Add to `Scene` or `SceneManager`
   - **NO** → Continue to next question

3. Is it a subsystem with complex logic?
   - **YES** → Create separate manager class
   - **NO** → Reconsider if it's actually a resource

**Example**: Adding a networking system

```cpp
// Option A: Simple socket → Add to GameCore
struct GameCore {
  sf::TcpSocket network_socket;
};

// Option B: Complex networking → Separate manager
class Engine {
private:
  GameCore m_game_core;
  NetworkManager m_network_manager;  // Complex, gets its own manager
};
```

### Adding New Configuration

**Question**: Where should a new configuration option go?

**Answer**: Add to `EngineConfig` or appropriate subcategory:

```cpp
// New config category
struct AudioConfig {
  float master_volume{1.0f};
  float music_volume{0.8f};
  float sfx_volume{1.0f};
};

// Add to EngineConfig
struct EngineConfig {
  DisplayConfig display;
  AudioConfig audio;  // NEW
};
```

**Loading Pattern**:
```cpp
// Load from file
auto audio_config = LoadAudioConfig();
m_engine_config.audio = audio_config;

// Use in engine
m_game_core.asset_manager.SetMasterVolume(m_engine_config.audio.master_volume);
```

### Adding New Engine State

**Question**: Where should new runtime state go?

**Answer**: Add to `EngineState`:

```cpp
struct EngineState {
  bool running{false};
  std::vector<std::shared_ptr<Subscriber>> subscriptions;
  
  // NEW: Performance tracking
  struct PerformanceStats {
    float avg_frame_time_ms{0.0f};
    size_t total_ticks{0};
  } performance;
};
```

### Future Categories

As the engine grows, consider additional categories:

```cpp
// Potential future organization
class Engine {
protected:
  // Core: Long-lived global resources
  GameCore m_game_core;
  
  // Configuration: Loaded settings
  EngineConfig m_engine_config;
  UserPreferences m_user_preferences;
  
  // State: Runtime operational data
  EngineState m_engine_state;
  
  // Subsystems: Complex managers
  SceneManager m_scene_manager;
  NetworkManager m_network_manager;  // Future
  
  // Diagnostics: Profiling & debugging (optional, development only)
  #ifdef DEBUG
  DiagnosticsState m_diagnostics;
  #endif
};
```

---

## Examples and Code Patterns

### Example 1: Current vs. Proposed - Window Configuration

**Current (Mixed)**:
```cpp
// Configuration scattered across multiple places
class Engine {
  GameCore m_game_core;  // Contains window
  UserPreferences m_user_preferences;  // No window settings here
};

struct GameCore {
  sf::RenderWindow game_window;  // Resource
  // Window size not stored anywhere accessible
};

// Loading during StartUp
auto engine_core_data = provider.LoadEngineCoreData();
core::ConfigureGameCore(m_game_core, engine_core_data);  // What's in here?
```

**Proposed (Clear Categories)**:
```cpp
// Configuration clearly separated
struct DisplayConfig {
  uint32_t window_width{800};
  uint32_t window_height{600};
  std::string window_title{"SteamRot"};
  uint32_t framerate_limit{60};
  bool fullscreen{false};
  bool vsync{true};
};

struct EngineConfig {
  DisplayConfig display;
};

class Engine {
  GameCore m_game_core;           // Resources
  EngineConfig m_engine_config;   // Configuration
};

// Loading during StartUp
auto config = LoadEngineConfig();
m_engine_config = config;
ConfigureWindow(m_game_core.game_window, m_engine_config.display);

// Later access
if (m_engine_config.display.fullscreen) {
  m_game_core.game_window.create(/* fullscreen mode */);
}
```

### Example 2: Current vs. Proposed - Subscriptions

**Current (Unclear Ownership)**:
```cpp
class Engine {
protected:
  std::vector<std::shared_ptr<Subscriber>> m_subscriptions;  // What is this?
};

// Usage - hard to find
for (const auto& subscriber : m_subscriptions) {
  if (subscriber->m_active) {
    // Process
  }
}
```

**Proposed (Clear State Management)**:
```cpp
struct EngineState {
  /// @brief Engine-level event subscriptions (quit game, etc.)
  std::vector<std::shared_ptr<Subscriber>> subscriptions;
  
  /// @brief Flag indicating if quit was requested
  bool quit_requested{false};
};

class Engine {
protected:
  EngineState m_engine_state;
};

// Usage - clear purpose
for (const auto& subscriber : m_engine_state.subscriptions) {
  if (subscriber->m_active) {
    if (subscriber->m_trigger_event_type == EventType::EVENT_QUIT_GAME) {
      m_engine_state.quit_requested = true;
    }
  }
}
```

### Example 3: Adding New Resource - Audio Manager

**Question**: We need to add an AudioManager. Where does it go?

**Decision Process**:
1. Is it long-lived and global? **YES**
2. Is it a simple resource or complex subsystem? **Complex** (has logic for mixing, streaming, etc.)
3. **Decision**: Create separate manager class (like SceneManager, DisplayManager)

**Implementation**:
```cpp
class AudioManager {
private:
  float m_master_volume;
  std::unordered_map<std::string, sf::SoundBuffer> m_sound_buffers;
  
public:
  void PlaySound(const std::string& sound_name);
  void SetMasterVolume(float volume);
};

class Engine {
protected:
  GameCore m_game_core;
  SceneManager m_scene_manager;
  AudioManager m_audio_manager;  // NEW: Complex subsystem
};

// If AudioManager needs to be accessed by scenes
struct GameContext {
  GameCore& game_core;
  AudioManager& audio_manager;  // NEW: Add reference
  // ...
};
```

### Example 4: Adding Configuration - Input Mapping

**Question**: We need configurable input mappings. Where do they go?

**Decision Process**:
1. Is it configuration loaded from files? **YES**
2. Is it user-configurable? **YES**
3. **Decision**: Add to `EngineConfig` or `UserPreferences`

**Implementation**:
```cpp
struct InputConfig {
  std::unordered_map<std::string, sf::Keyboard::Key> key_bindings{
    {"jump", sf::Keyboard::Space},
    {"attack", sf::Keyboard::X},
    // ...
  };
};

struct EngineConfig {
  DisplayConfig display;
  InputConfig input;  // NEW
};

// Usage in event processing
void ProcessKeyPress(sf::Keyboard::Key key) {
  if (key == m_engine_config.input.key_bindings["jump"]) {
    // Handle jump
  }
}
```

### Example 5: Adding State - Performance Monitoring

**Question**: We want to track frame times and FPS. Where does this go?

**Decision Process**:
1. Is it runtime state that changes each frame? **YES**
2. Is it engine-level or scene-level? **Engine-level** (tracks overall performance)
3. **Decision**: Add to `EngineState`

**Implementation**:
```cpp
struct PerformanceMetrics {
  float frame_time_ms{0.0f};
  float fps{0.0f};
  size_t total_frames{0};
  std::chrono::time_point<std::chrono::steady_clock> last_frame_time;
};

struct EngineState {
  bool running{false};
  std::vector<std::shared_ptr<Subscriber>> subscriptions;
  PerformanceMetrics performance;  // NEW
};

// Update each frame
void Engine::OnTickEnd() {
  auto now = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
    now - m_engine_state.performance.last_frame_time
  );
  
  m_engine_state.performance.frame_time_ms = duration.count();
  m_engine_state.performance.fps = 1000.0f / duration.count();
  m_engine_state.performance.total_frames++;
  m_engine_state.performance.last_frame_time = now;
}
```

---

## Summary

### Key Takeaways

1. **Three Categories**: Core Resources, Configuration, Engine State
2. **Clear Naming**: Category name indicates purpose and lifetime
3. **Decision Tree**: Easy process for categorizing new data
4. **Gradual Migration**: Can be adopted incrementally without breaking existing code
5. **Future-Proof**: Pattern scales to accommodate new subsystems

### Benefits

- ✅ **Intuitive** - Category immediately indicates purpose
- ✅ **Organized** - Related data grouped together
- ✅ **Extensible** - Clear place for new data
- ✅ **Maintainable** - Easy to find and modify data
- ✅ **Testable** - Categories can be mocked independently

### Naming Conventions Summary

| Category | Struct Name | Member Prefix | Example |
|----------|-------------|---------------|---------|
| Core Resources | `GameCore` | `m_game_core` | `m_game_core.game_window` |
| Configuration | `*Config`, `*Preferences` | `m_engine_config` | `m_engine_config.display.width` |
| Engine State | `*State`, `*Status` | `m_engine_state` | `m_engine_state.running` |
| Subsystems | `*Manager` | `m_*_manager` | `m_scene_manager` |

### Next Steps

1. Review this proposal with team
2. Decide on adoption strategy (gradual vs. big refactor)
3. Update existing documentation to reflect new organization
4. Create migration guide for each category
5. Update coding guidelines with new patterns

---

## References

- **Related Proposals**:
  - `ENGINE_ARCHITECTURE_IMPROVEMENTS.md` - Tick pipeline improvements
  - `DATA_LOADING_INTERFACE_SYSTEM.md` - Data provider pattern

- **Current Implementation**:
  - `src/engine/Engine.h` - Base engine class
  - `src/core/GameCore.h` - Core resources struct
  - `src/context/GameContext.h` - Reference wrapper
  - `src/configuration/EngineConfig.h` - Configuration struct (underutilized)

- **Future Work**:
  - Implement `EngineState` struct
  - Enhance `EngineConfig` with subcategories
  - Add accessors and compatibility layer for migration
