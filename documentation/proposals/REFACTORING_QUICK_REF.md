# Refactoring Quick Reference

**Date**: December 6, 2025  
**Status**: Quick reference for discussion  
**See Also**: REFACTORING_ANALYSIS.md (details), REFACTORING_VISUALS.md (diagrams)

---

## TL;DR

We're reorganizing Scene/SceneManager to match Engine's clean structure and organizing flatbuffers into logical directories.

---

## Three-Struct Pattern (Like Engine)

```
Engine:             Scene:              SceneManager:
├─ Resources        ├─ Resources        ├─ Resources
├─ Config           ├─ Config           ├─ Config
└─ State            └─ State            └─ State
```

**Benefits**: Consistent, predictable, maintainable

---

## What Goes Where?

### Scene

**SceneResources** → Long-lived stuff
- EntityManager, ActionManager, LogicMap, SceneCore

**SceneConfig** → Loaded from files
- Scene-specific settings (future: render settings, gameplay parameters)

**SceneState** → Runtime flags
- Active flag, pause state

### SceneManager

**SceneManagerResources** → Long-lived stuff
- (Empty initially, m_scenes stays at top level)

**SceneManagerConfig** → Loaded from files
- Scene manager settings

**SceneManagerState** → Runtime flags
- Subscriptions map

---

## Flatbuffers Reorganization

### Before (Flat)
```
src/flatbuffers_headers/
└── 28 .fbs files mixed together
```

### After (Organized)
```
src/flatbuffers_headers/
├── core/           # types, scene_types
├── engine/         # Already exists!
├── scenes/         # scene_data, scene_manager_data, fragments
├── entities/       # entities, UI, components
├── events/         # events, subscriber, user_input
├── logic/          # logic_data
├── assets/         # assets
├── configuration/  # context, preferences, save_data
└── testing/        # test_data, simulation
```

**Generated headers**: Stay in root (no include changes!)

---

## Data Loading Cascade

```
1. Defaults (always)
   ↓
2. User Preferences (if applicable)
   ↓
3. Save Data (if active save + gameplay scene)
   ↓
Final Config
```

**Rules**:
- Engine: Defaults + User Prefs
- TitleScene: Defaults ONLY
- GameScenes: All three tiers

---

## Impact Summary

| Category | Count | Effort |
|----------|-------|--------|
| New struct headers | 6-8 | Low |
| Modified files | 30-40 | Medium |
| Include updates | 94 | Medium |
| CMake changes | 2-3 | Medium |
| Timeline | 8-12 days | - |

---

## Migration Options

### Option A: Big Bang
- All at once
- Fast (6-8 days)
- HIGH RISK ⚠️

### Option B: Incremental  
- One phase at a time
- Slow (8-12 days)
- LOW RISK ✓ **RECOMMENDED**

### Option C: Hybrid
- Related phases grouped
- Medium (7-10 days)
- MEDIUM RISK

---

## Key Decisions (APPROVED ✅)

1. **Migration Strategy**: ✅ Incremental (phased approach with review checkpoints)
2. **Generated Headers**: ✅ Keep in root (no include changes)
3. **SceneManagerResources**: ✅ Create now (even if empty, for consistency)
4. **Scene Vector Location**: ✅ Keep at top level (not nested in resources)
5. **TitleScene Data**: ✅ Defaults only for now (can revisit later)
6. **Access Pattern**: ✅ Direct access (not getters/setters)

## Cleanup Items (APPROVED ✅)

- ✅ Remove EntityMemoryPool from SceneInfo (lives in EntityManager)
- ✅ Remove m_scene_event_types from Scene (unused, superseded by Subscribers)

---

## Phase Order (Incremental with Review Checkpoints)

**Phase 0: Cleanup** (0.5 days)
- Remove EntityMemoryPool from SceneInfo
- Remove m_scene_event_types from Scene
- Update tests, verify no functionality loss
- **CHECKPOINT**: Review & approve before Phase 1

**Phase 1: Scene organization** (2-3 days)
- Create SceneResources, SceneConfig, SceneState structs
- Update Scene class, derived scenes
- Update all Scene-related code
- **CHECKPOINT**: Review & approve before Phase 2

**Phase 2: SceneManager organization** (1-2 days)
- Create SceneManager structs
- Update SceneManager class
- Keep m_scenes at top level
- **CHECKPOINT**: Review & approve before Phase 3

**Phase 3: Flatbuffers reorganization** (3-4 days)
- Move files, update CMake
- Test builds, verify generation
- **CHECKPOINT**: Review & approve before Phase 4

**Phase 4: Data loading hierarchy** (2-3 days)
- Implement cascade, update providers
- Document per-component rules
- **CHECKPOINT**: Final review

---

## Success Criteria

### Must Have
- ✓ All tests pass
- ✓ Project builds
- ✓ No functionality regression
- ✓ Clear organization

### Should Have
- ✓ Improved clarity
- ✓ Easier to extend
- ✓ Better testability

---

## Risk Mitigation

| Risk | Mitigation |
|------|------------|
| Breaking builds | Test after each phase |
| Include errors | Keep generated headers in root |
| Lost changes | Frequent commits |
| CMake issues | Test build system early |

---

## Quick Navigation

- **Full Details**: See REFACTORING_ANALYSIS.md
- **Visuals/Diagrams**: See REFACTORING_VISUALS.md
- **Checklist**: See PR description
- **Questions**: Discuss in PR comments

---

## Current State vs Proposed

### Scene (Before)
```cpp
class Scene {
  EntityManager m_entity_manager;      // Mixed
  ActionManager m_action_manager;      // together
  LogicMap m_logic_map;                // no
  SceneCore m_scene_core;              // clear
  bool m_active;                       // structure
  std::unordered_set m_scene_event_types;
};
```

### Scene (After)
```cpp
class Scene {
  SceneResources m_scene_resources;   // Clear
  SceneConfig m_scene_config;         // separation
  SceneState m_scene_state;           // of concerns
};
```

**Benefit**: "Where do I add X?" → "Which category: Resources/Config/State?"

---

## Examples

### Adding New Scene Data

**Before**: "Uh, where does this go?" 🤷

**After**: 
- Resource (EntityManager)? → `SceneResources`
- Config (from file)? → `SceneConfig`
- Runtime flag? → `SceneState`

### Finding a Schema

**Before**: Scroll through 28 files in root 😓

**After**: "Event schema? Check `events/` directory!" 🎯

### Loading Scene Data

**Before**: Just load the file 🤔

**After**:
```cpp
// Clear cascade
config = LoadDefaults(scene);
if (AllowsUserPrefs(scene)) 
  config = Merge(config, LoadUserPrefs());
if (AllowsSaveData(scene)) 
  config = Merge(config, LoadSaveData());
```

---

## Testing Checklist (Per Phase)

```
[ ] Clean build
[ ] Compile succeeds
[ ] All tests pass
[ ] Manual smoke test
[ ] Documentation updated
[ ] Commit
```

---

## Recommendation

**Go Incremental**:
1. Safer (test after each phase)
2. Easier review (smaller PRs)
3. Better learning (understand each change)
4. Can pause between phases
5. Only ~2 days slower than big bang

**Start with**: Phase 1 (Scene organization)

---

## Open Questions

1. Should SceneManagerResources be created now (consistency) or later (when needed)?
2. Should TitleScene ever allow user preferences, or always use defaults?
3. Is 8 subdirectories for flatbuffers too granular or just right?
4. Should we batch some phases together (hybrid approach)?

---

## Getting Started

Once approved:

1. Review REFACTORING_ANALYSIS.md for details
2. Decide on migration strategy
3. Create Phase 1 branch
4. Implement Scene organization
5. Test thoroughly
6. PR for review
7. Repeat for next phase

---

**Ready to discuss!** Please provide feedback on the approach and key decisions.
