# Refactoring: Detailed File Change List

**Date**: December 6, 2025  
**Status**: Planning document (APPROVED with checkpoints)  
**Related**: REFACTORING_ANALYSIS.md, REFACTORING_VISUALS.md, REFACTORING_QUICK_REF.md

This document lists all files that will need to be created or modified for the refactoring.

**Migration Approach**: Incremental with review checkpoints in single PR

---

## Phase 0: Cleanup (Before Main Refactoring)

### Files to Modify

#### 1. Remove EntityMemoryPool from SceneInfo
**File**: `src/scenes/SceneInfo.h`
- Remove `EntityMemoryPool entity_memory_pool;` member
- Update documentation to clarify SceneInfo is just metadata (ID and type)
- Note: EntityMemoryPool lives in EntityManager, not SceneInfo

#### 2. Remove m_scene_event_types from Scene
**File**: `src/scenes/Scene.h`
- Remove `std::unordered_set<EventType> m_scene_event_types;` member
- Remove documentation comment about event types
- Note: Event handling is done via Subscribers, not this unused set

#### 3. Update Tests (if any reference these)
Check and update:
- `tests/unit/scenes/Scene.test.cpp` (if exists)
- `tests/unit/scenes/TitleScene.test.cpp`
- `tests/unit/scenes/CraftingScene.test.cpp`
- `tests/unit/scenes/SceneManager.test.cpp`

#### 4. Verify No Usage
Run searches to confirm:
```bash
grep -r "entity_memory_pool" src/ tests/
grep -r "scene_event_types" src/ tests/
```
Both should only show the declarations being removed.

### Estimated Impact
- **Modified files**: 2-4
- **Lines changed**: ~10-15 (removals)
- **Risk**: Very low (removing unused code)
- **Effort**: 0.5 days

### Checkpoint
**User reviews Phase 0 changes before proceeding to Phase 1**

---

## Phase 1: Scene Organization

### New Files to Create

1. `src/scenes/SceneResources.h`
2. `src/scenes/SceneConfig.h`
3. `src/scenes/SceneState.h`

### Files to Modify

#### Core Scene Files
1. `src/scenes/Scene.h`
   - Add includes for new structs
   - Replace individual members with struct members
   - Update member access patterns

2. `src/scenes/Scene.cpp`
   - Update constructor
   - Update all member accesses (m_entity_manager → m_scene_resources.entity_manager)
   - Update GetEntityMemoryPool(), GetRenderTexture(), etc.

3. `src/scenes/TitleScene.h`
   - Update if needed for new structure

4. `src/scenes/TitleScene.cpp`
   - Update member accesses
   - Update sMovement(), sCollision(), sAction(), sRender()

5. `src/scenes/CraftingScene.h`
   - Update if needed for new structure

6. `src/scenes/CraftingScene.cpp`
   - Update member accesses
   - Update sMovement(), sCollision(), sAction(), sRender()

#### Scene Support Files
7. `src/scenes/SceneContext.h`
   - Update constructor signature if needed
   - Update to work with new Scene structure

8. `src/scenes/SceneContext.cpp`
   - Update GetSceneContext() implementation
   - Update member access patterns

9. `src/scenes/SceneFactory.h`
   - Update if needed for new Scene structure

10. `src/scenes/SceneFactory.cpp`
    - Update scene creation logic
    - Update member initialization

11. `src/scenes/SceneInfo.h`
    - Review if changes needed

#### Test Files
12. `tests/unit/scenes/Scene.test.cpp` (if exists)
    - Update tests for new structure

13. `tests/unit/scenes/TitleScene.test.cpp`
    - Update tests for member access

14. `tests/unit/scenes/CraftingScene.test.cpp`
    - Update tests for member access

15. `tests/unit/scenes/SceneFactory.test.cpp`
    - Update tests for scene creation

### Estimated Impact
- **New files**: 3
- **Modified files**: 11-15
- **Lines changed**: ~200-300
- **Effort**: 2-3 days

---

## Phase 2: SceneManager Organization

### New Files to Create

1. `src/scenes/SceneManagerResources.h`
2. `src/scenes/SceneManagerConfig.h`
3. `src/scenes/SceneManagerState.h`

### Files to Modify

#### Core SceneManager Files
1. `src/scenes/SceneManager.h`
   - Add includes for new structs
   - Replace individual members with struct members
   - Keep m_scenes at top level (user request)
   - Update member access patterns

2. `src/scenes/SceneManager.cpp`
   - Update constructor
   - Update all member accesses (m_subscriptions → m_scene_manager_state.subscriptions)
   - Update ConfigureSceneManagerFromData()
   - Update RegisterSubscriber()
   - Update ProcessSubscriptions()
   - Update LoadTitleScene(), LoadCraftingScene()
   - Update AddSceneFromDefault()
   - Update UpdateScenes()

#### Engine Integration
3. `src/engine/Engine.h`
   - Review if SceneManager usage changes

4. `src/engine/Engine.cpp`
   - Update if SceneManager access patterns change

5. `src/engine/GameEngine.h`
   - Review SceneManager integration

6. `src/engine/GameEngine.cpp`
   - Update SceneManager usage if needed

#### Test Files
7. `tests/unit/scenes/SceneManager.test.cpp`
   - Update tests for new structure
   - Update tests for member access

8. `tests/harness/TestEngine.h`
   - Update if SceneManager usage changes

9. `tests/harness/TestEngine.cpp`
   - Update SceneManager integration

### Estimated Impact
- **New files**: 3
- **Modified files**: 7-9
- **Lines changed**: ~150-200
- **Effort**: 1-2 days

---

## Phase 3: Flatbuffers Reorganization

### New Directories to Create

1. `src/flatbuffers_headers/core/`
2. `src/flatbuffers_headers/scenes/`
3. `src/flatbuffers_headers/entities/`
4. `src/flatbuffers_headers/events/`
5. `src/flatbuffers_headers/logic/`
6. `src/flatbuffers_headers/assets/`
7. `src/flatbuffers_headers/configuration/`
8. `src/flatbuffers_headers/testing/`

### Files to Move

#### To core/
1. `types.fbs`
2. `scene_types.fbs`

#### To scenes/
3. `scene_data.fbs`
4. `scene_manager_data.fbs`
5. `fragments.fbs`

#### To entities/
6. `entities.fbs`
7. `user_interface.fbs`
8. `grimoire_machina.fbs`
9. `ui_state.fbs`
10. `ui_style.fbs`
11. `joints.fbs`

#### To events/
12. `events.fbs`
13. `event_packet_data.fbs`
14. `event_bus_data.fbs`
15. `subscriber_data.fbs`
16. `user_input.fbs`

#### To logic/
17. `logic_data.fbs`

#### To assets/
18. `assets.fbs`

#### To configuration/
19. `context_data.fbs`
20. `user_preferences.fbs`
21. `save_data.fbs`

#### To testing/
22. `test_data.fbs`
23. `simulation.fbs`
24. `event_test_data.fbs`
25. `input_test_data.fbs`

#### Already in engine/ (no move needed)
26. `engine/engine_config.fbs`
27. `engine/engine_state.fbs`
28. `engine/engine_resources_config.fbs`

### CMake Files to Modify

1. `src/flatbuffers_headers/generate_flatbuffers_headers.cmake`
   - Update schema_files list with new paths
   - Update output directory logic
   - Ensure generated headers stay in root

2. `src/flatbuffers_headers/convert_json_to_binary.cmake`
   - Update schema paths in macro calls
   - Verify subdirectory schemas work

3. `src/flatbuffers_headers/CMakeLists.txt`
   - Review if changes needed

### Include Path Updates (~94 files)

**Note**: If we keep generated headers in root (recommended), no include changes needed!

If we move generated headers to subdirs, need to update:

#### Events System (~8 files)
1. `src/events/EventHandler.h`
2. `src/events/EventHandler.cpp`
3. `src/events/event_factory.h`
4. `src/events/event_factory.cpp`
5. `src/events/event_bus_conversion.h`
6. `src/events/SubscriberFactory.h`
7. `src/events/SubscriberFactory.cpp`
8. `src/events/EventPacket.h`

#### Logic System (~5 files)
9. `src/logic/logic_render.cpp`
10. `src/logic/LogicFactory.h`
11. `src/logic/LogicFactory.cpp`
12. `src/logic/CraftingRenderLogic.cpp`
13. And other logic files...

#### Data Handlers (~2 files)
14. `src/data_handlers/FlatbuffersDataLoader.h`
15. `src/data_handlers/FlatbuffersDataLoader.cpp`

#### Data Providers (~10 files)
16. `src/data_providers/FlatbuffersAssetDataProvider.h`
17. `src/data_providers/FlatbuffersAssetDataProvider.cpp`
18. `src/data_providers/FlatbuffersEngineDataProvider.h`
19. `src/data_providers/FlatbuffersEngineDataProvider.cpp`
20. `src/data_providers/FlatbuffersFragmentDataProvider.h`
21. `src/data_providers/FlatbuffersFragmentDataProvider.cpp`
22. `src/data_providers/FlatbuffersGameConfigProvider.h`
23. `src/data_providers/FlatbuffersGameConfigProvider.cpp`
24. `src/data_providers/FlatbuffersSceneDataProvider.h`
25. `src/data_providers/FlatbuffersSceneDataProvider.cpp`
26. `src/data_providers/ISceneDataProvider.h`

#### Configuration (~3 files)
27. `src/configuration/ISaveDataProvider.h`
28. And other configuration files...

#### Scenes (~5 files)
29. `src/scenes/SceneManager.h`
30. `src/scenes/SceneFactory.cpp`
31. And other scene files...

#### Entity System
32. `src/entity/*` (various files)

#### Components
33. `src/components/*` (various files)

#### User Interface
34. `src/user_interface/*` (various files)

#### Context
35. `src/context/*` (various files)

#### Core
36. `src/core/*` (various files)

#### Test Files (~30+ files)
37. `tests/unit/engine/Engine.test.cpp`
38. `tests/unit/engine/GameEngine.test.cpp`
39. `tests/unit/scenes/CraftingScene.test.cpp`
40. `tests/unit/scenes/TitleScene.test.cpp`
41. `tests/unit/scenes/SceneFactory.test.cpp`
42. `tests/unit/scenes/SceneManager.test.cpp`
43. `tests/unit/data_providers/FlatbuffersEngineDataProvider.test.cpp`
44. `tests/unit/data_providers/FlatbuffersSceneDataProvider.test.cpp`
45. `tests/harness/TestEngine.h`
46. `tests/harness/TestEngine.cpp`
47. And 20+ more test files...

### Estimated Impact
- **New directories**: 8
- **Moved files**: 25 .fbs files
- **Modified CMake**: 2-3 files
- **Include updates**: 0 (if keep generated in root) or ~94 (if move generated)
- **Lines changed**: ~50 (CMake) + 0-200 (includes)
- **Effort**: 3-4 days

---

## Phase 4: Data Loading Hierarchy

### New Files to Create

1. `src/data_providers/ConfigurationMerger.h` (optional utility)
2. `src/data_providers/ConfigurationMerger.cpp` (optional utility)
3. `documentation/architecture/DATA_LOADING_HIERARCHY.md`

### Files to Modify

#### Data Provider Interfaces
1. `src/data_providers/IEngineDataProvider.h`
   - Add methods for cascading load
   - Document hierarchy behavior

2. `src/data_providers/ISceneDataProvider.h`
   - Add methods for cascading load
   - Document per-scene rules

3. `src/data_providers/IGameConfigProvider.h`
   - Review hierarchy support

#### Data Provider Implementations
4. `src/data_providers/FlatbuffersEngineDataProvider.h`
   - Implement cascading load

5. `src/data_providers/FlatbuffersEngineDataProvider.cpp`
   - Load defaults → user prefs cascade

6. `src/data_providers/FlatbuffersSceneDataProvider.h`
   - Implement cascading load

7. `src/data_providers/FlatbuffersSceneDataProvider.cpp`
   - Load defaults → user prefs → save data cascade
   - Implement per-scene rules (TitleScene defaults only)

8. `src/data_providers/FlatbuffersGameConfigProvider.h`
   - Review hierarchy support

9. `src/data_providers/FlatbuffersGameConfigProvider.cpp`
   - Implement if needed

#### Data Handlers
10. `src/data_handlers/FlatbuffersDataLoader.h`
    - Add hierarchy support methods

11. `src/data_handlers/FlatbuffersDataLoader.cpp`
    - Implement cascading load logic
    - Handle defaults/prefs/saves directories

#### Engine Integration
12. `src/engine/Engine.h`
    - Document data loading behavior

13. `src/engine/Engine.cpp`
    - Update StartUp() to use cascade

14. `src/engine/GameEngine.cpp`
    - Ensure user prefs loaded

#### Scene Integration
15. `src/scenes/Scene.h`
    - Document data loading behavior

16. `src/scenes/Scene.cpp`
    - Update ConfigureFromDefault()

17. `src/scenes/SceneManager.cpp`
    - Update scene loading logic

#### Configuration Support
18. `src/configuration/ISaveDataProvider.h`
    - Ensure save data integration

19. `src/configuration/*` (other files as needed)

#### Test Files
20. `tests/unit/data_providers/FlatbuffersEngineDataProvider.test.cpp`
    - Test cascade behavior

21. `tests/unit/data_providers/FlatbuffersSceneDataProvider.test.cpp`
    - Test cascade behavior
    - Test per-scene rules

22. `tests/integration/data_loading/` (new directory)
    - Test full cascade integration

#### Data Files
23. Review `data/defaults/` structure
24. Review `data/user/preferences/` structure
25. Review `data/user/saves/` structure
26. Create example save files if needed

### Estimated Impact
- **New files**: 2-3 (+ integration tests)
- **Modified files**: 15-20
- **Lines changed**: ~300-400
- **Effort**: 2-3 days

---

## Documentation Updates (All Phases)

### New Documentation
1. `documentation/proposals/SCENE_DATA_ORGANIZATION.md`
   - Document Scene/SceneManager organization
   - Similar to ENGINE_DATA_ORGANIZATION.md

2. `documentation/architecture/DATA_LOADING_HIERARCHY.md`
   - Document cascade system
   - Explain per-component rules

3. `documentation/architecture/FLATBUFFERS_ORGANIZATION.md`
   - Document directory structure
   - Explain schema categorization

### Documentation to Update
4. `documentation/proposals/ENGINE_DATA_ORGANIZATION.md`
   - Add references to Scene/SceneManager

5. `documentation/proposals/ENGINE_DATA_ORGANIZATION_QUICK_REF.md`
   - Extend to cover Scene/SceneManager

6. `.github/GITHUB_COPILOT_INSTRUCTIONS.md`
   - Update with new patterns
   - Document data organization rules

7. `README.md`
   - Update architecture overview if needed
   - Update project structure section

### Estimated Impact
- **New docs**: 3
- **Updated docs**: 4
- **Lines added**: ~1000-1500
- **Effort**: Concurrent with implementation

---

## Summary: Total Impact

### Files to Create
- **Struct headers**: 6 (Scene + SceneManager)
- **Documentation**: 3 new docs
- **Utilities**: 0-2 (configuration merger)
- **Total new files**: 9-11

### Files to Modify
- **Phase 1 (Scene)**: 11-15 files
- **Phase 2 (SceneManager)**: 7-9 files
- **Phase 3 (Flatbuffers)**: 2-3 CMake + 0-94 includes
- **Phase 4 (Data loading)**: 15-20 files
- **Documentation**: 4 files
- **Total modified files**: 39-45 (excluding includes) or 133-139 (with includes)

### Files to Move
- **Flatbuffers schemas**: 25 .fbs files

### Directories to Create
- **Flatbuffers**: 8 subdirectories

### Effort Estimate
- **Phase 1**: 2-3 days
- **Phase 2**: 1-2 days
- **Phase 3**: 3-4 days
- **Phase 4**: 2-3 days
- **Total**: 8-12 days (incremental)

### Risk Assessment
- **Build breaks**: Medium (mitigated by testing after each phase)
- **Test failures**: Medium (existing tests will catch issues)
- **Include errors**: Low (if keep generated in root)
- **CMake issues**: Medium (test early and often)
- **Merge conflicts**: Medium (communicate with team)

---

## Verification Checklist (Per Phase)

After each phase:

```
[ ] Clean build succeeds
    cmake --preset Debug

[ ] Full compile succeeds
    cmake --build --preset Debug

[ ] All tests pass
    ctest --preset Debug

[ ] Manual smoke test
    Run game, verify basic functionality

[ ] Code review ready
    Clean commits, good messages

[ ] Documentation updated
    Relevant docs reflect changes

[ ] No compiler warnings
    Check build output

[ ] No new static analysis issues
    Run any linters/analyzers
```

---

## Rollback Strategy

### Per-Phase Rollback
Each phase is a separate commit/PR, can be reverted independently:

```
Phase 1 ──> [Test failed] ──> Revert Phase 1 ──> Fix ──> Retry
Phase 2 ──> [Test passed] ──> Keep Phase 2
Phase 3 ──> [Test passed] ──> Keep Phase 3
Phase 4 ──> [Test failed] ──> Revert Phase 4 ──> Fix ──> Retry
```

### Big Bang Rollback
If doing all at once:

```
All Changes ──> [Test failed] ──> Revert everything ──> Debug ──> Retry
```

Much harder to isolate issues!

---

## Communication Plan

### Before Starting
- [ ] Get approval on plan from maintainers
- [ ] Announce refactoring to team
- [ ] Set expectations on timeline
- [ ] Coordinate with ongoing work

### During Implementation
- [ ] Daily updates on progress
- [ ] Flag any blockers immediately
- [ ] Share learnings with team
- [ ] Request reviews promptly

### After Completion
- [ ] Announce completion
- [ ] Share documentation
- [ ] Conduct team walkthrough
- [ ] Collect feedback for future refactorings

---

**End of Detailed File Change List**

This document provides a comprehensive view of all files that will be affected by the refactoring. Use this to:
- Estimate effort and timeline
- Identify potential conflicts
- Plan testing strategy
- Coordinate with team members
- Track progress during implementation
