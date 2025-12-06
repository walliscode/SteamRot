# Refactoring Analysis: Current vs Proposed Structure

**Date**: December 6, 2025  
**Status**: Analysis for Discussion  
**Related Plan**: See PR description for full checklist

---

## Current State: Scene Class

### Current Structure
```cpp
class Scene {
protected:
  // Mixed data - no clear organization
  const SceneInfo m_scene_info;          // Metadata
  EntityManager m_entity_manager;         // Resource/Manager
  ActionManager m_action_manager;         // Resource/Manager
  const GameContext &m_game_context;      // Reference to engine
  std::unordered_map<LogicType, std::vector<std::unique_ptr<Logic>>> m_logic_map; // Resource
  SceneCore m_scene_core;                 // Resource (contains render texture)
  bool m_active;                          // State
  std::unordered_set<EventType> m_scene_event_types; // State/Config
};
```

### Problems
- **No categorization**: All members at same level
- **Unclear ownership**: What's a resource vs state vs config?
- **Hard to extend**: Where to add new scene data?
- **Testing difficulty**: Can't mock/inject categories independently

---

## Proposed State: Scene Class

### Proposed Structure
```cpp
class Scene {
protected:
  // Clear categorization following Engine pattern
  
  // Metadata (unchanged)
  const SceneInfo m_scene_info;
  const GameContext &m_game_context;
  
  // Category 1: Scene Resources (long-lived, owned resources)
  SceneResources m_scene_resources;
  
  // Category 2: Scene Configuration (loaded from data)
  SceneConfig m_scene_config;
  
  // Category 3: Scene State (runtime operational data)
  SceneState m_scene_state;
};
```

### SceneResources (new struct)
```cpp
struct SceneResources {
  // Long-lived scene resources
  EntityManager entity_manager;
  ActionManager action_manager;
  LogicCollection logic_map;  // Map of Logic instances
  SceneCore scene_core;       // Contains render texture
};
```

**What goes here**: 
- Managers (EntityManager, ActionManager)
- Logic systems
- Render texture (via SceneCore)
- Any other long-lived scene resources

**Lifetime**: Created at scene construction, destroyed at scene destruction

### SceneConfig (new struct)
```cpp
struct SceneConfig {
  // Configuration loaded from data files
  std::unordered_set<EventType> event_types;  // Events this scene handles
  // Future: scene-specific settings
  // - render settings
  // - gameplay parameters
  // - AI difficulty
};
```

**What goes here**:
- Settings loaded from scene data files
- Scene-specific configuration
- Immutable or rarely changing data

**Lifetime**: Loaded at scene creation from data files

### SceneState (new struct)
```cpp
struct SceneState {
  // Runtime operational state
  bool active{true};
  // Future: scene state tracking
  // - pause state
  // - transition state
  // - performance metrics
};
```

**What goes here**:
- Runtime flags and state
- Mutable operational data
- Performance tracking

**Lifetime**: Changes during scene execution

---

## Current State: SceneManager Class

### Current Structure
```cpp
class SceneManager {
private:
  // Mixed data - no clear organization
  const GameContext &m_game_context;                            // Reference
  std::unordered_map<uuids::uuid, std::unique_ptr<Scene>> m_scenes; // Resources
  std::unordered_map<EventType, std::shared_ptr<Subscriber>> m_subscriptions; // State
};
```

### Problems
- Same issues as Scene: no categorization, unclear ownership

---

## Proposed State: SceneManager Class

### Proposed Structure
```cpp
class SceneManager {
private:
  // Clear categorization
  
  // Reference to engine (unchanged)
  const GameContext &m_game_context;
  
  // Keep scenes vector at top level as requested
  std::unordered_map<uuids::uuid, std::unique_ptr<Scene>> m_scenes;
  
  // Category 1: SceneManager Resources (if any)
  SceneManagerResources m_scene_manager_resources;
  
  // Category 2: SceneManager Configuration
  SceneManagerConfig m_scene_manager_config;
  
  // Category 3: SceneManager State
  SceneManagerState m_scene_manager_state;
};
```

### SceneManagerResources (new struct)
```cpp
struct SceneManagerResources {
  // Currently: No clear resources besides m_scenes
  // m_scenes is kept at top level in SceneManager
  
  // Future: SceneManager-specific resources
  // - Scene factory?
  // - Scene transition manager?
};
```

**Note**: User requested keeping `m_scenes` at top level, not nested in resources.

### SceneManagerConfig (new struct)
```cpp
struct SceneManagerConfig {
  // Configuration loaded from data files
  // Currently: loaded via ConfigureSceneManagerFromData
  
  // Future: scene manager settings
  // - max concurrent scenes
  // - transition settings
  // - scene loading strategy
};
```

### SceneManagerState (new struct)
```cpp
struct SceneManagerState {
  // Runtime operational state
  std::unordered_map<EventType, std::shared_ptr<Subscriber>> subscriptions;
  
  // Future: runtime tracking
  // - active scene count
  // - scene load state
  // - transition state
};
```

---

## Flatbuffers Organization

### Current Structure
```
src/flatbuffers_headers/
├── All 28 .fbs files in root (flat)
├── engine/ (only subdirectory)
│   ├── engine_config.fbs
│   ├── engine_state.fbs
│   └── engine_resources_config.fbs
└── All generated *_generated.h files in root
```

**Problems**:
- Hard to find related schemas
- No logical grouping
- Poor discoverability
- Scales poorly as schemas grow

### Proposed Structure
```
src/flatbuffers_headers/
├── core/              # Core types (types.fbs, scene_types.fbs)
├── engine/            # Engine schemas (ALREADY EXISTS)
├── scenes/            # Scene schemas (scene_data, scene_manager_data, fragments)
├── entities/          # Entity/component schemas (entities, UI, grimoire, etc.)
├── events/            # Event system (events, event_packet, subscriber, user_input)
├── logic/             # Logic schemas (logic_data)
├── assets/            # Asset management (assets)
├── configuration/     # Configuration (context, preferences, save_data)
└── testing/           # Test infrastructure (test_data, simulation, test data types)
```

**Benefits**:
- ✅ Logical grouping by domain
- ✅ Easy to find related schemas
- ✅ Scales well as project grows
- ✅ Matches Engine organization (engine/ subdir)
- ✅ Clear separation of concerns

**Generated Headers**: Stay in root of flatbuffers_headers/ for include simplicity
- Alternative: Move to subdirs (requires more include path changes)

---

## Data Loading Hierarchy

### Current State
```
data/
├── defaults/          # Default configuration
│   ├── engine/
│   ├── scenes/
│   ├── preferences/
│   └── ...
└── user/              # User-specific data
    ├── preferences/   # User preference overrides
    └── saves/         # Game saves
```

**Current behavior**: 
- Engine loads defaults, then user preferences
- Not fully implemented for all data types
- No clear hierarchy for scene data

### Proposed Hierarchy
```
1. Shipped Defaults (data/defaults/)
   ├── Always loaded first
   ├── Provides baseline configuration
   └── Version controlled

2. User Preferences (data/user/preferences/)
   ├── Loaded second
   ├── Overrides defaults
   └── User-specific, not version controlled

3. Save Data (data/user/saves/)
   ├── Loaded last (if applicable)
   ├── Per-save overrides
   └── Scene-specific saved state
```

### Loading Strategy by Component

#### Engine
- ✅ Load defaults
- ✅ Load user preferences
- ❌ No save data (engine-level)

#### TitleScene
- ✅ Load defaults ONLY
- ❌ No user preferences (consistent experience)
- ❌ No save data (static scene)

**Rationale**: Title screen should be consistent across users

#### CraftingScene (and other gameplay scenes)
- ✅ Load defaults
- ✅ Load user preferences
- ✅ Load save data (if save file active)

**Rationale**: Gameplay scenes need saved state

#### SceneManager
- ✅ Load defaults
- Possibly: User preferences for scene management settings
- Possibly: Save data for which scenes to load

### Implementation Approach

```cpp
// Pseudo-code for cascading configuration
ConfigData LoadConfiguration(ConfigType type, SceneType scene) {
  ConfigData config;
  
  // 1. Load defaults (always)
  config = LoadDefaults(type, scene);
  
  // 2. Apply user preferences (if applicable)
  if (AllowsUserPreferences(scene)) {
    auto user_prefs = LoadUserPreferences(type);
    config = MergeConfiguration(config, user_prefs);
  }
  
  // 3. Apply save data (if applicable)
  if (AllowsSaveData(scene) && SaveFileActive()) {
    auto save_data = LoadSaveData(type, scene);
    config = MergeConfiguration(config, save_data);
  }
  
  return config;
}
```

---

## Impact Analysis

### Files to Create (New)
1. `src/scenes/SceneResources.h`
2. `src/scenes/SceneConfig.h`
3. `src/scenes/SceneState.h`
4. `src/scenes/SceneManagerResources.h`
5. `src/scenes/SceneManagerConfig.h`
6. `src/scenes/SceneManagerState.h`
7. `documentation/proposals/SCENE_DATA_ORGANIZATION.md`
8. `documentation/architecture/DATA_LOADING_HIERARCHY.md`

### Files to Modify (Significant)
1. `src/scenes/Scene.h` - Update member organization
2. `src/scenes/Scene.cpp` - Update member access
3. `src/scenes/TitleScene.h/cpp` - Update for new structure
4. `src/scenes/CraftingScene.h/cpp` - Update for new structure
5. `src/scenes/SceneManager.h` - Update member organization
6. `src/scenes/SceneManager.cpp` - Update member access
7. `src/scenes/SceneFactory.h/cpp` - Update scene creation
8. `src/scenes/SceneContext.h/cpp` - Update context creation
9. `src/flatbuffers_headers/generate_flatbuffers_headers.cmake` - Update paths
10. `src/flatbuffers_headers/convert_json_to_binary.cmake` - Update paths

### Files to Update (Include paths) - ~94 files
All files with `#include "*_generated.h"` including:
- `src/events/` - ~8 files
- `src/logic/` - ~5 files
- `src/data_handlers/` - ~2 files
- `src/data_providers/` - ~10 files
- `src/configuration/` - ~3 files
- `src/scenes/` - ~5 files
- `tests/` - ~30+ files
- Many others across the codebase

### CMake Build System
- Update `generate_flatbuffers_headers.cmake` for new directory structure
- Update schema file paths in generation commands
- Ensure generated headers output to correct locations
- Test that all JSON→binary conversions work

---

## Migration Strategy Options

### Option A: Big Bang (All at Once)
**Approach**: Implement all phases in one PR

**Pros**:
- ✅ Complete and consistent when done
- ✅ No intermediate inconsistent state
- ✅ Easier to reason about final state

**Cons**:
- ❌ High risk if something breaks
- ❌ Difficult to review
- ❌ Hard to roll back partially
- ❌ Long time before any testing possible

### Option B: Incremental (Phase by Phase)
**Approach**: One phase at a time, each independently testable

**Phase Order**:
1. Scene data organization (test independently)
2. SceneManager data organization (test independently)
3. Flatbuffers reorganization (test independently)
4. Data loading hierarchy (test independently)

**Pros**:
- ✅ Lower risk per change
- ✅ Easier to review
- ✅ Can test after each phase
- ✅ Can roll back individual phases
- ✅ Can pause between phases

**Cons**:
- ❌ More PRs to manage
- ❌ Temporary inconsistency between phases
- ❌ Longer overall timeline

### Option C: Hybrid (Group Related Phases)
**Approach**: Group related changes together

**Groups**:
1. Scene + SceneManager organization (closely related)
2. Flatbuffers reorganization (independent)
3. Data loading hierarchy (builds on 1 & 2)

**Pros**:
- ✅ Balances risk and progress
- ✅ Related changes together
- ✅ Fewer PRs than full incremental

**Cons**:
- ❌ Still some complexity per PR
- ❌ Some temporary inconsistency

### Recommendation
**Option B (Incremental)** or **Option C (Hybrid)** depending on time constraints and risk tolerance.

---

## Testing Strategy

### Per-Phase Testing
Each phase should include:
1. **Unit tests** - Test new structs and classes
2. **Integration tests** - Test interaction between components
3. **Build verification** - Ensure project builds
4. **Existing test pass** - All existing tests should still pass

### Phase-Specific Tests

#### Phase 1: Scene Organization
- Test Scene construction with new structs
- Test Scene member access patterns
- Test TitleScene and CraftingScene still work
- Test SceneContext creation

#### Phase 2: SceneManager Organization
- Test SceneManager construction with new structs
- Test SceneManager operations (add/remove scenes)
- Test scene loading and management
- Test subscription system

#### Phase 3: Flatbuffers Reorganization
- Test header generation from new locations
- Test JSON→binary conversion still works
- Test all includes resolve correctly
- Build full project and run all tests

#### Phase 4: Data Loading Hierarchy
- Test default data loading
- Test user preference overrides
- Test save data loading
- Test cascading configuration
- Test per-scene data source rules

---

## Open Questions for Discussion

### 1. Struct Member Access
Should we provide accessor methods or direct member access?

```cpp
// Option A: Direct access (like EngineResources)
scene.m_scene_resources.entity_manager.DoSomething();

// Option B: Accessor methods
scene.GetEntityManager().DoSomething();
```

**Recommendation**: Direct access (matches EngineResources pattern)

### 2. SceneManagerResources
SceneManager doesn't have many resources besides `m_scenes`. Should we:
- Create the struct anyway (for consistency)?
- Wait until we have actual resources to put in it?
- Skip it entirely?

**Recommendation**: Create it for consistency, even if mostly empty initially

### 3. Generated Header Locations
Should generated headers:
- Stay in root of flatbuffers_headers/? (less disruption)
- Move to subdirs matching .fbs files? (more organized)

**Recommendation**: Stay in root (simpler includes, less churn)

### 4. Scene Vector Location
User requested keeping `m_scenes` at top level. Should we:
- Keep it directly in SceneManager? (as requested)
- Reconsider if SceneManagerResources is created?

**Recommendation**: Keep at top level as explicitly requested

### 5. TitleScene Data Loading
Should TitleScene allow user preferences?
- Current plan: defaults only
- Alternative: allow user preferences for UI customization

**Recommendation**: Start with defaults only, can add preferences later

---

## Timeline Estimation

### Per Phase (assuming incremental approach)

**Phase 1: Scene Organization** - 2-3 days
- Create structs
- Update Scene classes
- Update all references
- Update tests
- Documentation

**Phase 2: SceneManager Organization** - 1-2 days
- Create structs
- Update SceneManager
- Update tests
- Documentation

**Phase 3: Flatbuffers Reorganization** - 3-4 days
- Create directories
- Move files
- Update CMake
- Update all includes (~94 files)
- Test builds
- Documentation

**Phase 4: Data Loading Hierarchy** - 2-3 days
- Implement cascading config
- Update data providers
- Update scene loading
- Tests
- Documentation

**Total: 8-12 days** for complete implementation

**Big Bang**: Potentially faster (6-8 days) but much higher risk

---

## Success Criteria

### Must Have
- ✅ All tests pass
- ✅ Project builds successfully
- ✅ No functionality regression
- ✅ Clear, consistent data organization
- ✅ Documentation updated

### Should Have
- ✅ Improved code clarity
- ✅ Easier to extend in future
- ✅ Better testing isolation
- ✅ Clear data loading hierarchy

### Nice to Have
- ✅ Performance improvements
- ✅ Reduced compile times
- ✅ Better error messages

---

## Next Steps

1. **Review this analysis** with team/maintainers
2. **Make key decisions** on open questions
3. **Choose migration strategy** (incremental vs big bang)
4. **Prioritize phases** if doing incremental
5. **Begin implementation** of first phase
6. **Iterate and adjust** based on learnings

---

## Appendix: Example File Moves

### Flatbuffers Reorganization Examples

#### Before
```
src/flatbuffers_headers/
├── events.fbs
├── scene_data.fbs
├── entities.fbs
└── test_data.fbs
```

#### After
```
src/flatbuffers_headers/
├── events/
│   └── events.fbs
├── scenes/
│   └── scene_data.fbs
├── entities/
│   └── entities.fbs
└── testing/
    └── test_data.fbs
```

#### Include Changes

Before:
```cpp
#include "events_generated.h"
#include "scene_data_generated.h"
```

After (if we move generated headers):
```cpp
#include "events/events_generated.h"
#include "scenes/scene_data_generated.h"
```

Or (if we keep generated headers in root):
```cpp
#include "events_generated.h"  // No change!
#include "scene_data_generated.h"  // No change!
```

**Recommendation**: Keep generated headers in root to minimize include changes
