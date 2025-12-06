# Pattern Comparison: Engine vs Scene vs SceneManager

**Date**: December 6, 2025  
**Status**: Reference document  
**Purpose**: Show consistency of proposed pattern across all levels

---

## Side-by-Side Comparison

### Engine (Already Implemented) ✅

```cpp
class Engine {
protected:
  // ════════════════════════════════════════
  // RESOURCES: Long-lived, global resources
  // ════════════════════════════════════════
  EngineResources m_engine_resources;
  
  struct EngineResources {
    sf::RenderWindow game_window;      // Display
    EventHandler event_handler;         // Events
    AssetManager asset_manager;         // Assets
    sf::Vector2i mouse_position;        // Input
    size_t loop_number;                 // Timing
  };
  
  // ════════════════════════════════════════
  // CONFIG: Settings loaded from files
  // ════════════════════════════════════════
  EngineConfig m_engine_config;
  
  struct EngineConfig {
    DisplayConfig display;              // Window settings
    UserPreferencesConfig user_preferences; // User prefs
  };
  
  // ════════════════════════════════════════
  // STATE: Runtime operational data
  // ════════════════════════════════════════
  EngineState m_engine_state;
  
  struct EngineState {
    bool running{false};                // Engine running?
    bool paused{false};                 // Engine paused?
    std::vector<std::shared_ptr<Subscriber>> subscriptions;
    bool quit_requested{false};
    PerformanceMetrics performance;
  };
  
  // ════════════════════════════════════════
  // OTHER: References, managers, etc.
  // ════════════════════════════════════════
  GameContext m_game_context;           // Context reference
  SceneManager m_scene_manager;         // Scene management
};
```

### Scene (Proposed) 🔄

```cpp
class Scene {
protected:
  // ════════════════════════════════════════
  // RESOURCES: Long-lived, scene resources
  // ════════════════════════════════════════
  SceneResources m_scene_resources;
  
  struct SceneResources {
    EntityManager entity_manager;       // Entity system
    ActionManager action_manager;       // Action system
    LogicCollection logic_map;          // Logic systems
    SceneCore scene_core;               // Render texture
  };
  
  // ════════════════════════════════════════
  // CONFIG: Settings loaded from files
  // ════════════════════════════════════════
  SceneConfig m_scene_config;
  
  struct SceneConfig {
    // Configuration loaded from data files
    // Future: scene-specific settings
    // - render settings
    // - gameplay parameters
    
    // Note: Event handling done via Subscribers, not a config set
    // Note: May be empty initially, created for consistency
  };
  
  // ════════════════════════════════════════
  // STATE: Runtime operational data
  // ════════════════════════════════════════
  SceneState m_scene_state;
  
  struct SceneState {
    bool active{true};                  // Scene active?
    // Future: pause state, metrics
  };
  
  // ════════════════════════════════════════
  // OTHER: References, metadata, etc.
  // ════════════════════════════════════════
  const SceneInfo m_scene_info;         // Metadata
  const GameContext &m_game_context;    // Context reference
};
```

### SceneManager (Proposed) 🔄

```cpp
class SceneManager {
private:
  // ════════════════════════════════════════
  // RESOURCES: Long-lived manager resources
  // ════════════════════════════════════════
  SceneManagerResources m_scene_manager_resources;
  
  struct SceneManagerResources {
    // Currently empty
    // Future: scene factory, transition manager, etc.
    // Note: m_scenes is kept at top level per user request
  };
  
  // ════════════════════════════════════════
  // CONFIG: Settings loaded from files
  // ════════════════════════════════════════
  SceneManagerConfig m_scene_manager_config;
  
  struct SceneManagerConfig {
    // Future: scene manager settings
    // - max concurrent scenes
    // - transition settings
  };
  
  // ════════════════════════════════════════
  // STATE: Runtime operational data
  // ════════════════════════════════════════
  SceneManagerState m_scene_manager_state;
  
  struct SceneManagerState {
    std::unordered_map<EventType, std::shared_ptr<Subscriber>> subscriptions;
    // Future: active scene tracking, load state
  };
  
  // ════════════════════════════════════════
  // OTHER: References, scenes vector, etc.
  // ════════════════════════════════════════
  const GameContext &m_game_context;    // Context reference
  std::unordered_map<uuids::uuid, std::unique_ptr<Scene>> m_scenes; // Kept at top level!
};
```

---

## Pattern Consistency Matrix

| Aspect | Engine | Scene | SceneManager |
|--------|--------|-------|--------------|
| **Resources struct** | ✅ EngineResources | 🔄 SceneResources | 🔄 SceneManagerResources |
| **Config struct** | ✅ EngineConfig | 🔄 SceneConfig | 🔄 SceneManagerConfig |
| **State struct** | ✅ EngineState | 🔄 SceneState | 🔄 SceneManagerState |
| **Naming convention** | `m_engine_*` | `m_scene_*` | `m_scene_manager_*` |
| **Access pattern** | Direct members | Direct members | Direct members |
| **Lifetime semantics** | Clear | Clear | Clear |

Legend: ✅ = Implemented, 🔄 = Proposed

---

## Benefits of Consistency

### 1. Learning Curve

```
New Developer:
    │
    ├─> Learns Engine pattern
    │   "Oh, Resources/Config/State makes sense!"
    │
    ├─> Sees Scene
    │   "Hey, this is the same pattern! I already understand this!"
    │
    └─> Sees SceneManager
        "Same pattern again! This is easy!"
```

### 2. Intuitive Extensions

```
Adding new data to Scene?
    │
    ├─> "Is it a resource?" → SceneResources
    ├─> "Is it config from file?" → SceneConfig
    └─> "Is it runtime state?" → SceneState

No guesswork! The pattern tells me where it goes!
```

### 3. Code Navigation

```
Looking for Scene's entity manager?
    │
    ├─> "It's a manager, so it's a resource"
    ├─> Check m_scene_resources
    └─> Found: m_scene_resources.entity_manager

Pattern makes finding things predictable!
```

### 4. Maintenance

```
Refactoring Engine?
    │
    ├─> Pattern is already established
    ├─> Same approach for Scene
    └─> Same approach for SceneManager

Consistent patterns → Easier maintenance!
```

---

## Access Pattern Examples

### Engine (Current)

```cpp
// Direct access to resources
engine.m_engine_resources.game_window.clear();
engine.m_engine_resources.event_handler.ProcessEvents();

// Direct access to config
auto width = engine.m_engine_config.display.window_width;
auto volume = engine.m_engine_config.user_preferences.master_volume;

// Direct access to state
if (engine.m_engine_state.running) {
  // Process tick
}
```

### Scene (Proposed)

```cpp
// Direct access to resources
scene.m_scene_resources.entity_manager.Update();
scene.m_scene_resources.logic_map[LogicType::Render].RunLogic();

// Direct access to config
auto event_types = scene.m_scene_config.event_types;

// Direct access to state
if (scene.m_scene_state.active) {
  // Update scene
}
```

### SceneManager (Proposed)

```cpp
// Direct access to resources
// (empty initially)

// Direct access to config
// (future: scene manager settings)

// Direct access to state
auto& subscriptions = scene_manager.m_scene_manager_state.subscriptions;
```

**Consistency**: All use same `m_[category]_[type].[member]` pattern!

---

## Migration Path Visualization

### Before (Mixed)

```
Engine              Scene               SceneManager
├─ Mixed data       ├─ Mixed data       ├─ Mixed data
│  - resources      │  - resources      │  - resources
│  - config         │  - config         │  - config
│  - state          │  - state          │  - state
│  All flat         │  All flat         │  All flat
```

### After Phase 1 (Engine Done)

```
Engine              Scene               SceneManager
├─ Resources ✅     ├─ Mixed data       ├─ Mixed data
├─ Config ✅        │  - resources      │  - resources
├─ State ✅         │  - config         │  - config
│  Organized!       │  - state          │  - state
                    │  Still flat       │  Still flat
```

### After Phase 2 (Scene Done)

```
Engine              Scene               SceneManager
├─ Resources ✅     ├─ Resources ✅     ├─ Mixed data
├─ Config ✅        ├─ Config ✅        │  - resources
├─ State ✅         ├─ State ✅         │  - config
│  Organized!       │  Organized!       │  - state
                                        │  Still flat
```

### After Phase 3 (All Done)

```
Engine              Scene               SceneManager
├─ Resources ✅     ├─ Resources ✅     ├─ Resources ✅
├─ Config ✅        ├─ Config ✅        ├─ Config ✅
├─ State ✅         ├─ State ✅         ├─ State ✅
│  Organized!       │  Organized!       │  Organized!
```

**Result**: Consistent, predictable, maintainable!

---

## Decision Making: Same Across All Levels

### For Any New Data

```
┌─────────────────────────────────────────────┐
│ New data needs to be added to Engine?       │
│ New data needs to be added to Scene?        │
│ New data needs to be added to SceneManager? │
└────────────────┬────────────────────────────┘
                 │
                 ▼
    ┌────────────────────────────┐
    │ Is it a long-lived         │
    │ resource/manager?          │
    └────┬───────────────┬───────┘
         │ Yes           │ No
         ▼               ▼
    [Type]Resources  ┌──────────────────┐
                     │ Is it loaded     │
                     │ from data files? │
                     └────┬─────────┬───┘
                          │ Yes     │ No
                          ▼         ▼
                     [Type]Config [Type]State

Same decision tree for Engine, Scene, and SceneManager!
```

---

## Member Naming Consistency

### Pattern

```
m_[level]_[category].[member]
   │      │           │
   │      │           └─> Specific data member
   │      └─> resources / config / state
   └─> engine / scene / scene_manager
```

### Examples

```cpp
// Engine
engine.m_engine_resources.game_window
engine.m_engine_config.display
engine.m_engine_state.running

// Scene
scene.m_scene_resources.entity_manager
scene.m_scene_config.event_types
scene.m_scene_state.active

// SceneManager
scene_manager.m_scene_manager_resources.[future]
scene_manager.m_scene_manager_config.[future]
scene_manager.m_scene_manager_state.subscriptions
```

**Benefit**: Name tells you exactly where to find data!

---

## Testing Consistency

### Test Structure

```cpp
// Engine tests
TEST_CASE("EngineResources is default constructible")
TEST_CASE("EngineConfig loads from file")
TEST_CASE("EngineState tracks running flag")

// Scene tests (proposed)
TEST_CASE("SceneResources is default constructible")
TEST_CASE("SceneConfig loads from file")
TEST_CASE("SceneState tracks active flag")

// SceneManager tests (proposed)
TEST_CASE("SceneManagerResources is default constructible")
TEST_CASE("SceneManagerConfig loads from file")
TEST_CASE("SceneManagerState tracks subscriptions")
```

**Same test patterns across all levels!**

---

## Documentation Consistency

### Engine Documentation (Existing)

```
documentation/proposals/ENGINE_DATA_ORGANIZATION.md
├── Resources section
├── Config section
└── State section
```

### Scene Documentation (Proposed)

```
documentation/proposals/SCENE_DATA_ORGANIZATION.md
├── Resources section (same structure as Engine)
├── Config section (same structure as Engine)
└── State section (same structure as Engine)
```

**Same documentation structure!**

---

## Evolution Path

### Stage 1: Engine ✅
```
Engine organized with Resources/Config/State pattern
└─> Proves the pattern works
```

### Stage 2: Scene 🔄
```
Apply proven pattern to Scene
└─> Extends consistency to scene level
```

### Stage 3: SceneManager 🔄
```
Apply proven pattern to SceneManager
└─> Complete consistency across all levels
```

### Stage 4: Future Components 🔮
```
New major components (e.g., NetworkManager, AudioManager)
└─> Use same pattern automatically!
```

---

## Real-World Usage Examples

### Scenario: Developer Adds Scene-Level Audio Manager

**Before (Mixed Structure):**
```cpp
class Scene {
  EntityManager m_entity_manager;  // Where does AudioManager go?
  ActionManager m_action_manager;  // Here? After this?
  // ???
  AudioManager m_audio_manager;    // Just... stick it somewhere?
};
```
**Developer thinks**: "Where should I put this? 🤷"

**After (Organized Structure):**
```cpp
struct SceneResources {
  EntityManager entity_manager;
  ActionManager action_manager;
  LogicCollection logic_map;
  AudioManager audio_manager;     // Obviously goes in Resources!
  SceneCore scene_core;
};
```
**Developer thinks**: "It's a manager, so SceneResources!" ✅

---

### Scenario: Loading Scene Configuration

**Before:**
```cpp
// Where does this config data go?
scene.m_???  // Not clear!
```

**After:**
```cpp
// Config from file? Goes in SceneConfig!
scene.m_scene_config.audio_settings = loaded_data;
```

---

### Scenario: Tracking Scene State

**Before:**
```cpp
// Is scene paused?
scene.m_paused;      // Wait, is this flag at top level?
scene.m_is_paused;   // Or like this?
scene.m_state.paused; // Or nested?
```

**After:**
```cpp
// Runtime state? Check SceneState!
scene.m_scene_state.paused = true;
```

---

## Comparison with Other Potential Approaches

### Approach A: Keep Everything Flat (Current State)

```cpp
class Scene {
  EntityManager m_entity_manager;
  ActionManager m_action_manager;
  LogicCollection m_logic_map;
  bool m_active;
  std::unordered_set<EventType> m_scene_event_types;
  // ... where does new data go? 🤷
};
```

**Problems:**
- ❌ No clear organization
- ❌ Hard to extend
- ❌ Inconsistent with Engine

### Approach B: Use Getters/Setters

```cpp
class Scene {
private:
  SceneResources m_scene_resources;
  
public:
  EntityManager& GetEntityManager() { return m_scene_resources.entity_manager; }
  ActionManager& GetActionManager() { return m_scene_resources.action_manager; }
  // ... getters for everything
};
```

**Problems:**
- ❌ More boilerplate
- ❌ Doesn't match Engine pattern
- ❌ Hides the organization

### Approach C: Nested Structs (Proposed) ✅

```cpp
class Scene {
protected:
  SceneResources m_scene_resources;
  SceneConfig m_scene_config;
  SceneState m_scene_state;
};
```

**Benefits:**
- ✅ Clear organization
- ✅ Easy to extend
- ✅ Matches Engine pattern
- ✅ Direct access when needed
- ✅ Can add accessors if desired later

---

## Summary: Why This Pattern?

### Proven
```
Engine has used this pattern successfully
└─> It works in practice!
```

### Intuitive
```
Category names are self-explanatory
└─> Resources, Config, State are clear!
```

### Scalable
```
Easy to add new data
└─> Decision tree tells you where it goes!
```

### Consistent
```
Same pattern everywhere
└─> Learn once, apply everywhere!
```

### Maintainable
```
Organization makes navigation easy
└─> Find things quickly!
```

### Testable
```
Categories can be mocked/injected
└─> Better test isolation!
```

---

## Conclusion

By applying the same Resources/Config/State pattern from Engine to Scene and SceneManager, we achieve:

1. **Consistency**: Same pattern everywhere
2. **Clarity**: Names tell you what data is
3. **Maintainability**: Easy to navigate and extend
4. **Testability**: Clear boundaries for testing
5. **Scalability**: Pattern works for future additions

The proposed refactoring doesn't invent a new pattern—it extends the existing, proven Engine pattern to achieve consistency across the entire codebase.

---

**See Also:**
- REFACTORING_QUICK_REF.md - Quick overview
- REFACTORING_ANALYSIS.md - Detailed analysis
- REFACTORING_VISUALS.md - Visual diagrams
- REFACTORING_FILE_LIST.md - File-by-file changes
