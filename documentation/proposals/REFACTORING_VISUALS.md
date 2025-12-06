# Refactoring Visuals: Data Organization

**Date**: December 6, 2025  
**Status**: Visual aids for discussion  
**Related**: REFACTORING_ANALYSIS.md

---

## Current vs Proposed: Scene Class Organization

### Current Structure (Flat, Mixed)
```
┌─────────────────────────────────────────────────────────────┐
│                         Scene                                │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  m_scene_info            [Metadata]                          │
│  m_entity_manager        [Resource]                          │
│  m_action_manager        [Resource]                          │
│  m_game_context          [Reference]                         │
│  m_logic_map             [Resource]                          │
│  m_scene_core            [Resource]                          │
│  m_active                [State]                             │
│  m_scene_event_types     [Config/State]                      │
│                                                              │
│  Problem: No clear categorization!                           │
└─────────────────────────────────────────────────────────────┘
```

### Proposed Structure (Organized, Clear)
```
┌─────────────────────────────────────────────────────────────┐
│                         Scene                                │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌─────────────────────────────────────────────────────┐    │
│  │ Metadata (unchanged)                                │    │
│  │  - m_scene_info                                     │    │
│  │  - m_game_context (reference)                       │    │
│  └─────────────────────────────────────────────────────┘    │
│                                                              │
│  ┌─────────────────────────────────────────────────────┐    │
│  │ m_scene_resources (SceneResources)                  │    │
│  │  Long-lived, owned resources                        │    │
│  │  - entity_manager                                   │    │
│  │  - action_manager                                   │    │
│  │  - logic_map                                        │    │
│  │  - scene_core (render texture)                      │    │
│  └─────────────────────────────────────────────────────┘    │
│                                                              │
│  ┌─────────────────────────────────────────────────────┐    │
│  │ m_scene_config (SceneConfig)                        │    │
│  │  Configuration loaded from data                     │    │
│  │  - event_types                                      │    │
│  │  - [future: scene settings]                         │    │
│  └─────────────────────────────────────────────────────┘    │
│                                                              │
│  ┌─────────────────────────────────────────────────────┐    │
│  │ m_scene_state (SceneState)                          │    │
│  │  Runtime operational state                          │    │
│  │  - active                                           │    │
│  │  - [future: pause state, metrics]                   │    │
│  └─────────────────────────────────────────────────────┘    │
│                                                              │
│  Benefit: Clear categorization, easy to extend!              │
└─────────────────────────────────────────────────────────────┘
```

---

## Engine → Scene → SceneManager Pattern

### Consistent Organization Across Levels

```
┌───────────────────────────────────────────────────────────────┐
│                         Engine                                 │
├───────────────────────────────────────────────────────────────┤
│  ✓ EngineResources                                            │
│  ✓ EngineConfig                                               │
│  ✓ EngineState                                                │
│                                                               │
│  Pattern established! ────────┐                               │
└───────────────────────────────┼───────────────────────────────┘
                                │
                ┌───────────────┴───────────────┐
                │  Apply same pattern to:       │
                └───────────────┬───────────────┘
                                │
        ┌───────────────────────┴──────────────────────┐
        │                                               │
        ▼                                               ▼
┌────────────────────────────┐         ┌────────────────────────────┐
│         Scene              │         │      SceneManager          │
├────────────────────────────┤         ├────────────────────────────┤
│  ⟹ SceneResources         │         │  m_scenes (top level)      │
│  ⟹ SceneConfig            │         │  ⟹ SceneManagerResources  │
│  ⟹ SceneState             │         │  ⟹ SceneManagerConfig     │
└────────────────────────────┘         │  ⟹ SceneManagerState      │
                                       └────────────────────────────┘
```

### Benefits of Consistent Pattern

```
┌──────────────────────────────────────────────────────────────┐
│                    Consistency Benefits                       │
├──────────────────────────────────────────────────────────────┤
│                                                               │
│  1. Intuitive Understanding                                   │
│     "If I learned Engine, I know Scene!"                      │
│                                                               │
│  2. Predictable Extension                                     │
│     "New data? Which category: Resources/Config/State?"       │
│                                                               │
│  3. Better Testability                                        │
│     "Mock just the category I need!"                          │
│                                                               │
│  4. Clear Documentation                                       │
│     "Same pattern everywhere = easier to document"            │
│                                                               │
│  5. Maintainability                                           │
│     "Consistent structure = easier to maintain"               │
│                                                               │
└──────────────────────────────────────────────────────────────┘
```

---

## Flatbuffers Reorganization

### Current Structure (Flat)

```
src/flatbuffers_headers/
│
├── All 28 .fbs files mixed together
│   (Hard to find related schemas!)
│
├── types.fbs                    ← Where is this?
├── events.fbs                   ← Where is this?
├── scene_data.fbs               ← Where is this?
├── test_data.fbs                ← Where is this?
├── entities.fbs                 ← Where is this?
├── ...and 23 more files...      ← Lost in the noise
│
└── engine/                      ← Only organized subdirectory
    ├── engine_config.fbs
    ├── engine_state.fbs
    └── engine_resources_config.fbs
```

**Problem**: "Where do I put a new schema?" 🤷

### Proposed Structure (Organized)

```
src/flatbuffers_headers/
│
├── core/                        ← Core types everyone uses
│   ├── types.fbs
│   └── scene_types.fbs
│
├── engine/                      ← Engine-level (already exists!)
│   ├── engine_config.fbs
│   ├── engine_state.fbs
│   └── engine_resources_config.fbs
│
├── scenes/                      ← Scene-related schemas
│   ├── scene_data.fbs
│   ├── scene_manager_data.fbs
│   └── fragments.fbs
│
├── entities/                    ← Entity & component schemas
│   ├── entities.fbs
│   ├── user_interface.fbs
│   ├── grimoire_machina.fbs
│   ├── ui_state.fbs
│   ├── ui_style.fbs
│   └── joints.fbs
│
├── events/                      ← Event system schemas
│   ├── events.fbs
│   ├── event_packet_data.fbs
│   ├── event_bus_data.fbs
│   ├── subscriber_data.fbs
│   └── user_input.fbs
│
├── logic/                       ← Logic-related schemas
│   └── logic_data.fbs
│
├── assets/                      ← Asset management
│   └── assets.fbs
│
├── configuration/               ← Configuration schemas
│   ├── context_data.fbs
│   ├── user_preferences.fbs
│   └── save_data.fbs
│
└── testing/                     ← Test infrastructure
    ├── test_data.fbs
    ├── simulation.fbs
    ├── event_test_data.fbs
    └── input_test_data.fbs
```

**Benefit**: "New event schema? Obviously goes in events/!" ✓

---

## Data Loading Hierarchy

### Three-Tier Cascade System

```
┌─────────────────────────────────────────────────────────────┐
│               Data Loading Hierarchy                         │
└─────────────────────────────────────────────────────────────┘

                    User Starts Game
                           │
                           ▼
              ┌────────────────────────┐
              │  1. Load Defaults      │
              │  (data/defaults/)      │
              │  ✓ Always loaded       │
              │  ✓ Version controlled  │
              │  ✓ Provides baseline   │
              └────────┬───────────────┘
                       │
                       ▼
              ┌────────────────────────┐
              │  2. Load User Prefs    │
              │  (data/user/preferences/)│
              │  ✓ Overrides defaults  │
              │  ✓ User-specific       │
              │  ✓ Not committed       │
              └────────┬───────────────┘
                       │
                       ▼
              ┌────────────────────────┐
              │  3. Load Save Data     │
              │  (data/user/saves/)    │
              │  ✓ Per-save overrides  │
              │  ✓ Scene-specific      │
              │  ✓ If save active      │
              └────────┬───────────────┘
                       │
                       ▼
                 Final Configuration
```

### Per-Component Loading Rules

```
┌──────────────────────────────────────────────────────────────┐
│                  Component Loading Matrix                     │
├──────────────┬────────────┬──────────────┬──────────────────┤
│ Component    │  Defaults  │  User Prefs  │  Save Data       │
├──────────────┼────────────┼──────────────┼──────────────────┤
│ Engine       │     ✓      │      ✓       │       ✗          │
│ TitleScene   │     ✓      │      ✗       │       ✗          │
│ GameScenes   │     ✓      │      ✓       │       ✓          │
│ SceneManager │     ✓      │      ?       │       ?          │
└──────────────┴────────────┴──────────────┴──────────────────┘

Legend:
  ✓ = Always loaded
  ✗ = Never loaded
  ? = To be decided
```

#### Rationale: TitleScene Defaults Only

```
┌─────────────────────────────────────────────────────────────┐
│              Why TitleScene Uses Defaults Only               │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  User 1's Computer        User 2's Computer                 │
│  ┌─────────────┐          ┌─────────────┐                  │
│  │ TitleScene  │          │ TitleScene  │                  │
│  │ (defaults)  │  ═════   │ (defaults)  │                  │
│  └─────────────┘          └─────────────┘                  │
│       Same!                    Same!                         │
│                                                              │
│  Benefits:                                                   │
│  ✓ Consistent first impression                              │
│  ✓ Easier to support (all users see same thing)             │
│  ✓ Simpler to test                                          │
│  ✓ No user customization bugs                               │
│                                                              │
│  Gameplay Scenes (CraftingScene, etc.)                       │
│  ┌─────────────┐          ┌─────────────┐                  │
│  │ Crafting    │          │ Crafting    │                  │
│  │ (saves!)    │  ≠≠≠≠≠   │ (saves!)    │                  │
│  └─────────────┘          └─────────────┘                  │
│    Different!                Different!                      │
│                                                              │
│  Benefits:                                                   │
│  ✓ Per-player progression                                   │
│  ✓ Saved state                                              │
│  ✓ User preferences applied                                 │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## Decision Flow: Where Does New Data Go?

### For Scene Data

```
                Is this scene-level data?
                          │
                    Yes   │   No
                ┌─────────┴─────────┐
                │                   │
                ▼                   ▼
    ┌────────────────────┐    (Not for Scene,
    │ Scene Data         │     check Engine or
    │ Decision Tree      │     SceneManager)
    └─────────┬──────────┘
              │
              ▼
    ┌──────────────────────────────┐
    │ Is it a long-lived resource? │
    │ (Manager, system, etc.)      │
    └────┬──────────────────┬──────┘
         │ Yes              │ No
         ▼                  ▼
    SceneResources    ┌─────────────────────────┐
                      │ Is it loaded from file? │
                      └────┬──────────────┬─────┘
                           │ Yes          │ No
                           ▼              ▼
                      SceneConfig   SceneState
```

### For SceneManager Data

```
                Is this SceneManager-level data?
                          │
                    Yes   │   No
                ┌─────────┴─────────┐
                │                   │
                ▼                   ▼
    ┌────────────────────┐    (Not for SceneManager,
    │ SceneManager Data  │     check Engine or
    │ Decision Tree      │     Scene)
    └─────────┬──────────┘
              │
              ▼
    Note: m_scenes stays at top level!
    (User request)
              │
              ▼
    ┌──────────────────────────────┐
    │ Is it a long-lived resource? │
    │ (Not including m_scenes)     │
    └────┬──────────────────┬──────┘
         │ Yes              │ No
         ▼                  ▼
    SceneManager-     ┌─────────────────────────┐
    Resources         │ Is it loaded from file? │
                      └────┬──────────────┬─────┘
                           │ Yes          │ No
                           ▼              ▼
                      SceneManager-  SceneManager-
                      Config         State
```

---

## Migration Strategy Comparison

### Option A: Big Bang

```
Current State
      │
      │ [ONE BIG CHANGE]
      │
      │  ┌─────────────────────────────────┐
      │  │ • Scene reorganization          │
      │  │ • SceneManager reorganization   │
      │  │ • Flatbuffers reorganization    │
      └──│ • Data loading hierarchy        │
         │ • All includes updated          │
         │ • All tests updated             │
         └────────┬────────────────────────┘
                  │
                  ▼
           Final State

Timeline: Fast (6-8 days)
Risk: HIGH ⚠️⚠️⚠️
Review: Difficult
Testing: Only at end
Rollback: All or nothing
```

### Option B: Incremental

```
Current State
      │
      ├───► [Phase 1: Scene]
      │           │
      │           ├─ Test ✓
      │           │
      ├───► [Phase 2: SceneManager]
      │           │
      │           ├─ Test ✓
      │           │
      ├───► [Phase 3: Flatbuffers]
      │           │
      │           ├─ Test ✓
      │           │
      └───► [Phase 4: Data Loading]
                  │
                  ├─ Test ✓
                  │
                  ▼
           Final State

Timeline: Slower (8-12 days)
Risk: LOW ✓✓✓
Review: Easy (one phase at a time)
Testing: After each phase
Rollback: Per-phase
```

### Option C: Hybrid

```
Current State
      │
      ├───► [Group 1: Scene + SceneManager]
      │           │
      │           ├─ Test ✓
      │           │
      ├───► [Group 2: Flatbuffers]
      │           │
      │           ├─ Test ✓
      │           │
      └───► [Group 3: Data Loading]
                  │
                  ├─ Test ✓
                  │
                  ▼
           Final State

Timeline: Medium (7-10 days)
Risk: MEDIUM ✓✓
Review: Moderate
Testing: After each group
Rollback: Per-group
```

### Recommendation Matrix

```
┌─────────────────┬──────────┬──────────┬──────────────┐
│ Consider This   │ Big Bang │ Incremental│   Hybrid    │
├─────────────────┼──────────┼──────────┼──────────────┤
│ Time pressure   │    ✓     │    ✗     │      ~       │
│ Risk averse     │    ✗     │    ✓     │      ~       │
│ Easy review     │    ✗     │    ✓     │      ~       │
│ Quick delivery  │    ✓     │    ✗     │      ~       │
│ Safe rollback   │    ✗     │    ✓     │      ~       │
│ Team learning   │    ✗     │    ✓     │      ✓       │
└─────────────────┴──────────┴──────────┴──────────────┘

Legend: ✓ = Good fit, ✗ = Poor fit, ~ = Neutral
```

---

## Include Path Impact

### Current Includes (Flat Structure)

```cpp
// In any source file:
#include "events_generated.h"           // Easy!
#include "scene_data_generated.h"       // Simple!
#include "entities_generated.h"         // Clear!
```

### Option 1: Move Generated Headers Too

```cpp
// If we move generated headers to subdirs:
#include "events/events_generated.h"          // More typing
#include "scenes/scene_data_generated.h"      // Longer paths
#include "entities/entities_generated.h"      // More folders

// Impact: ALL ~94 includes need updating! 😱
```

### Option 2: Keep Generated Headers in Root (Recommended)

```cpp
// If we keep generated headers in root:
#include "events_generated.h"           // No change! ✓
#include "scene_data_generated.h"       // No change! ✓
#include "entities_generated.h"         // No change! ✓

// Impact: ZERO includes need updating! 🎉

// Schema files (.fbs) move to subdirs
// Generated headers (*_generated.h) stay in root
// CMake generates headers to root directory
```

### Visual: Recommended Approach

```
src/flatbuffers_headers/
│
├── events/
│   └── events.fbs                  ← Schema moves to subdir
│
├── scenes/
│   └── scene_data.fbs              ← Schema moves to subdir
│
├── entities/
│   └── entities.fbs                ← Schema moves to subdir
│
└── [Root directory]
    ├── events_generated.h          ← Generated header stays
    ├── scene_data_generated.h      ← Generated header stays
    └── entities_generated.h        ← Generated header stays

Benefits:
✓ Organized schema files (.fbs)
✓ No include path changes
✓ Minimal disruption
✓ Easy to find schemas
✓ Backwards compatible
```

---

## Timeline Visualization

### Incremental Approach Timeline

```
Week 1
├── Mon-Tue: Phase 1 (Scene organization)
│   ├── Create SceneResources, SceneConfig, SceneState
│   ├── Update Scene.h/cpp
│   ├── Update derived scenes
│   └── Test ✓
│
├── Wed: Phase 2 (SceneManager organization)
│   ├── Create SceneManager structs
│   ├── Update SceneManager.h/cpp
│   └── Test ✓
│
└── Thu-Fri: Phase 3 (Flatbuffers) - Part 1
    ├── Create directories
    ├── Move files
    └── Update CMake

Week 2
├── Mon-Tue: Phase 3 (Flatbuffers) - Part 2
│   ├── Test builds
│   ├── Fix any issues
│   └── Test ✓
│
├── Wed-Thu: Phase 4 (Data loading)
│   ├── Implement cascade
│   ├── Update providers
│   └── Test ✓
│
└── Fri: Final verification
    ├── Full test suite
    ├── Documentation
    └── PR ready!

Total: 8-12 days
```

---

## Success Metrics

### Quantitative Metrics

```
Before Refactoring          After Refactoring
┌─────────────────┐         ┌─────────────────┐
│ Time to Find    │         │ Time to Find    │
│ Related Schema  │         │ Related Schema  │
│   ~5 minutes    │   VS    │   ~30 seconds   │
└─────────────────┘         └─────────────────┘

┌─────────────────┐         ┌─────────────────┐
│ Time to Add     │         │ Time to Add     │
│ Scene Data      │         │ Scene Data      │
│   ~30 minutes   │   VS    │   ~10 minutes   │
│ (confusion)     │         │ (clear path)    │
└─────────────────┘         └─────────────────┘

┌─────────────────┐         ┌─────────────────┐
│ Cognitive Load  │         │ Cognitive Load  │
│ Understanding   │         │ Understanding   │
│ Data Org        │         │ Data Org        │
│   HIGH ⚠️       │   VS    │   LOW ✓         │
└─────────────────┘         └─────────────────┘
```

### Qualitative Improvements

```
┌──────────────────────────────────────────────────────────────┐
│                     Developer Experience                      │
├──────────────────────────────────────────────────────────────┤
│                                                               │
│  Before: "Where do I put this scene data?"                    │
│          "Is this a resource or state?"                       │
│          "Where's the scene_data.fbs file?"                   │
│          "What pattern should I follow?"                      │
│                                                               │
│  After:  "SceneConfig - it's configuration!"                  │
│          "Look in scenes/ directory!"                         │
│          "Same pattern as Engine!"                            │
│          "This makes sense!"                                  │
│                                                               │
└──────────────────────────────────────────────────────────────┘
```

---

## Risk Mitigation

### High-Risk Areas

```
┌──────────────────────────────────────────────────────────────┐
│                        Risk Areas                             │
├─────────────────┬────────────────┬───────────────────────────┤
│ Risk            │ Impact         │ Mitigation                │
├─────────────────┼────────────────┼───────────────────────────┤
│ Breaking builds │ HIGH ⚠️⚠️⚠️   │ Test after each phase     │
│ Include errors  │ MEDIUM ⚠️⚠️    │ Keep generated in root    │
│ Lost changes    │ HIGH ⚠️⚠️⚠️   │ Incremental commits       │
│ Test failures   │ MEDIUM ⚠️⚠️    │ Run tests frequently      │
│ Merge conflicts │ MEDIUM ⚠️⚠️    │ Communicate with team     │
│ CMake issues    │ HIGH ⚠️⚠️⚠️   │ Test build system early   │
└─────────────────┴────────────────┴───────────────────────────┘
```

### Testing Checkpoints

```
After Each Phase:
    │
    ├──► Clean build
    │    └─ cmake --preset Debug
    │
    ├──► Compile check
    │    └─ cmake --build --preset Debug
    │
    ├──► Run tests
    │    └─ ctest --preset Debug
    │
    ├──► Manual verification
    │    └─ Run game, check basic functionality
    │
    └──► Commit point
         └─ Safe to commit if all above pass ✓
```

---

## Summary: Key Decisions Needed

```
┌──────────────────────────────────────────────────────────────┐
│              Decisions Required for Approval                  │
├──────────────────────────────────────────────────────────────┤
│                                                               │
│  1. Migration Strategy                                        │
│     [ ] Big Bang      [ ] Incremental      [ ] Hybrid        │
│                                                               │
│  2. Generated Headers Location                                │
│     [ ] Move to subdirs    [ ] Keep in root (recommended)    │
│                                                               │
│  3. SceneManagerResources                                     │
│     [ ] Create it now (consistency)                           │
│     [ ] Wait for actual resources                             │
│                                                               │
│  4. TitleScene Data                                           │
│     [ ] Defaults only (recommended)                           │
│     [ ] Allow user preferences                                │
│                                                               │
│  5. m_scenes Location                                         │
│     [✓] Top level (user confirmed)                            │
│     [ ] Reconsider?                                           │
│                                                               │
│  6. Flatbuffers Directory Granularity                         │
│     [ ] Proposed structure OK                                 │
│     [ ] Suggest modifications: _____________                  │
│                                                               │
│  7. Priority                                                  │
│     [ ] High - Start immediately                              │
│     [ ] Medium - After other work                             │
│     [ ] Low - Nice to have                                    │
│                                                               │
└──────────────────────────────────────────────────────────────┘
```

---

## Next Steps Flowchart

```
        This Document Reviewed
                │
                ▼
        ┌───────────────────┐
        │ Discuss & Decide  │
        │ Key Questions     │
        └────────┬──────────┘
                 │
                 ├─ Approved? ────── No ──► Revise Plan
                 │
                 │ Yes
                 ▼
        ┌───────────────────┐
        │ Choose Migration  │
        │ Strategy          │
        └────────┬──────────┘
                 │
                 ▼
        ┌───────────────────┐
        │ Begin Phase 1     │
        │ Implementation    │
        └────────┬──────────┘
                 │
                 ▼
        ┌───────────────────┐
        │ Test & Verify     │
        └────────┬──────────┘
                 │
                 ├─ Pass? ────── No ──► Fix Issues
                 │
                 │ Yes
                 ▼
        Continue to Next Phase...
```

---

**End of Visual Documentation**

For detailed analysis and descriptions, see `REFACTORING_ANALYSIS.md`
