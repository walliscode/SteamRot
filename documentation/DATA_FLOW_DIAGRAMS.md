# Data Flow Diagrams

This document provides visual representations of data flow in the SteamRot engine for both production save/load and testing workflows.

## Current Architecture Overview

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         STEAMROT ENGINE ARCHITECTURE                     │
└─────────────────────────────────────────────────────────────────────────┘

┌───────────────────────────────┐      ┌──────────────────────────────────┐
│     PRODUCTION WORKFLOW       │      │      TESTING WORKFLOW            │
│         (GameEngine)          │      │       (TestEngine)               │
└───────────────────────────────┘      └──────────────────────────────────┘
```

## 1. Production Save/Load Data Flow

### Current Implementation

```
┌─────────────────────────────────────────────────────────────────────────┐
│                          SAVE GAME WORKFLOW                              │
└─────────────────────────────────────────────────────────────────────────┘

  GameEngine State                 SaveData                    File
  ┌──────────────┐              ┌──────────┐              ┌──────────┐
  │ SceneManager │              │          │              │          │
  │ - Scenes     │────Export───▶│ Metadata │────Write────▶│ .save    │
  │ - Entities   │              │ Scenes   │              │ (FBS)    │
  │              │              │ Entities │              │          │
  └──────────────┘              └──────────┘              └──────────┘
       ▲                             ▲
       │                             │
       │         Currently Missing:  │
       │         - EngineState       │
       │         - EngineConfig      │
       │         - EventBus          │
       │                             │

┌─────────────────────────────────────────────────────────────────────────┐
│                          LOAD GAME WORKFLOW                              │
└─────────────────────────────────────────────────────────────────────────┘

  File                         SaveData                  GameEngine State
  ┌──────────┐              ┌──────────┐              ┌──────────────┐
  │          │              │          │              │ SceneManager │
  │ .save    │────Read─────▶│ Metadata │────Import───▶│ - Scenes     │
  │ (FBS)    │              │ Scenes   │              │ - Entities   │
  │          │              │ Entities │              │              │
  └──────────┘              └──────────┘              └──────────────┘
                                 │
                                 │
                    Uses: IEntityImporter
                    - FlatbuffersEntityImporter
                    - SceneCollectionData
```

### Recommended Implementation

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    ENHANCED SAVE GAME WORKFLOW                           │
└─────────────────────────────────────────────────────────────────────────┘

  GameEngine State                  SaveData                    File
  ┌──────────────┐              ┌─────────────┐            ┌──────────┐
  │ EngineState  │              │             │            │          │
  │ SceneManager │───Export────▶│ Metadata    │───Write───▶│ .save    │
  │ - Scenes     │              │ State       │            │ (FBS)    │
  │ - Entities   │              │ Scenes      │            │          │
  │ EventBus     │              │ Entities    │            │          │
  │              │              │ Events      │            │          │
  └──────────────┘              └─────────────┘            └──────────┘
       │                              ▲
       │                              │
       └──────────Uses─────────────────┘
           engine::export utilities
           - ExportActiveScenes()
           - ExportEventBus()
           - ExportEngineState()
           
  Note: EngineConfig (user preferences, display settings) NOT saved here.
  These are global user settings managed separately via the preference system
  (default.preferences.bin and user-specific preference files).
  User prefs are loaded at engine startup, not per-save-game.
```

## 2. Testing Data Flow

### Current Implementation

```
┌─────────────────────────────────────────────────────────────────────────┐
│                       TESTING WORKFLOW (Current)                         │
└─────────────────────────────────────────────────────────────────────────┘

  Test Data (JSON)              TestData               TestEngine Execution
  ┌──────────────┐           ┌──────────┐            ┌──────────────────┐
  │ Metadata     │           │ Metadata │            │ Tick 0           │
  │ NumTicks     │──Parse───▶│ NumTicks │──Feed─────▶│ - Execute Logic  │
  │ Simulation   │           │ Snapshot │            │ - Capture State  │
  │ StartState   │           │ Expected │            ├──────────────────┤
  │ ExpectedMap  │           │          │            │ Tick 1           │
  └──────────────┘           └──────────┘            │ - Execute Logic  │
       │                          │                  │ - Capture State  │
       │                          │                  ├──────────────────┤
       └──────FlatBuffers─────────┘                  │ Tick N           │
            .test_data.bin                           │ - Execute Logic  │
                                                     │ - Capture State  │
                                                     └──────────────────┘
                                                            │
                                                            ▼
  Validation                                         Data Bank
  ┌──────────────┐                                ┌──────────────┐
  │ Compare      │◀──────────────────────────────│ Tick 0: State│
  │ Expected vs  │                                │ Tick 1: State│
  │ Actual per   │                                │ Tick N: State│
  │ Tick         │                                └──────────────┘
  └──────────────┘
```

### TestEngine Snapshot Capture

```
┌─────────────────────────────────────────────────────────────────────────┐
│                   TestEngine::CaptureSnapShot(tick)                      │
└─────────────────────────────────────────────────────────────────────────┘

  Active Scenes                 EngineSnapshot              Data Bank
  ┌──────────────┐           ┌────────────────┐         ┌──────────────┐
  │ Scene UUID 1 │           │ tick_number    │         │              │
  │ - Entities   │──Copy────▶│ scene_snapshots│────────▶│ [tick] = {   │
  │ Scene UUID 2 │           │ - UUID 1: EMP  │         │   snapshot   │
  │ - Entities   │           │ - UUID 2: EMP  │         │ }            │
  └──────────────┘           │ event_bus      │         └──────────────┘
                             └────────────────┘
  EventHandler                      ▲
  ┌──────────────┐                  │
  │ GlobalBus    │──────Copy────────┘
  │ - Events     │
  └──────────────┘

  EMP = EntityMemoryPool (deep copy)
```

## 3. Shared Components

### Entity Import/Export (Shared)

```
┌─────────────────────────────────────────────────────────────────────────┐
│               SHARED ENTITY IMPORT/EXPORT MECHANISM                      │
└─────────────────────────────────────────────────────────────────────────┘

  Production Path                    Testing Path
  ┌──────────────┐                ┌──────────────┐
  │ SaveData     │                │ TestData     │
  │ - Scenes     │                │ - Snapshot   │
  └──────┬───────┘                └──────┬───────┘
         │                               │
         ▼                               ▼
  ┌─────────────────────────────────────────────┐
  │      SceneCollectionData                    │
  │      Vector<SceneLoadData>                  │
  │      - SceneData                            │
  │      - unique_ptr<IEntityImporter>          │
  └─────────────┬───────────────────────────────┘
                │
                ▼
  ┌─────────────────────────────────────────────┐
  │      IEntityImporter (Interface)            │
  │      - ImportEntities(EntityMemoryPool&)    │
  └─────────────┬───────────────────────────────┘
                │
                ▼
  ┌─────────────────────────────────────────────┐
  │    FlatbuffersEntityImporter                │
  │    - Wraps EntityCollectionFbs reference    │
  │    - Uses FlatbuffersEntityConfigurator     │
  └─────────────────────────────────────────────┘
                │
                ▼
  ┌─────────────────────────────────────────────┐
  │       EntityMemoryPool                      │
  │       (Configured with entities)            │
  └─────────────────────────────────────────────┘

  ✅ This pattern is SHARED and works well!
  ✅ No changes needed here
```

## 4. Recommended Shared Export Utilities

```
┌─────────────────────────────────────────────────────────────────────────┐
│                 PROPOSED: engine::export NAMESPACE                       │
└─────────────────────────────────────────────────────────────────────────┘

  Engine State                  Export Utilities           Output
  ┌──────────────┐           ┌──────────────────┐      ┌──────────────┐
  │ SceneManager │──────────▶│ ExportActive     │─────▶│ SceneColl    │
  │              │           │ Scenes()         │      │ ectionData   │
  └──────────────┘           └──────────────────┘      └──────────────┘

  ┌──────────────┐           ┌──────────────────┐      ┌──────────────┐
  │ EventHandler │──────────▶│ ExportEventBus() │─────▶│ EventBus     │
  └──────────────┘           └──────────────────┘      └──────────────┘

  ┌──────────────┐           ┌──────────────────┐      ┌──────────────┐
  │ EngineState  │──────────▶│ ExportEngine     │─────▶│ EngineState  │
  │              │           │ State()          │      │              │
  └──────────────┘           └──────────────────┘      └──────────────┘

  ┌──────────────┐           ┌──────────────────┐      ┌──────────────┐
  │ Engine       │──────────▶│ ExportEngine     │─────▶│ Engine       │
  │              │           │ Snapshot()       │      │ Snapshot     │
  └──────────────┘           └──────────────────┘      └──────────────┘
                                    ▲
                                    │
                 Used by:           │
                 - GameEngine::SaveGame()
                 - TestEngine::CaptureSnapShot()
                 
  Note: ExportEngineConfig() NOT needed for SaveData.
  User preferences and display settings are global user settings,
  not per-save-game data. They're managed separately via the
  preference system (default.preferences.bin and user files).
```

## 5. SaveData vs EngineSnapshot Relationship

### Current State

```
┌─────────────────────────────────────────────────────────────────────────┐
│                 SaveData vs EngineSnapshot (Current)                     │
└─────────────────────────────────────────────────────────────────────────┘

  SaveData                          EngineSnapshot
  ┌──────────────────────┐         ┌──────────────────────┐
  │ SaveMetaData         │         │ optional<tick_number>│
  │ SceneManagerData     │         │ SceneCollectionData  │
  │ SceneCollectionData  │         │ optional<EventBus>   │
  │                      │         │                      │
  │ ❌ NO EngineState    │         │ ❌ NO EngineState    │
  │ ❌ NO EngineConfig   │         │ ❌ NO EngineConfig   │
  │ ❌ NO EventBus       │         │ ❌ NO SceneManager   │
  └──────────────────────┘         └──────────────────────┘
          │                                 │
          └────────Partial Overlap─────────┘
               (SceneCollectionData)
```

### Recommended State

```
┌─────────────────────────────────────────────────────────────────────────┐
│              SaveData vs EngineSnapshot (Recommended)                    │
└─────────────────────────────────────────────────────────────────────────┘

  SaveData                          EngineSnapshot
  ┌──────────────────────┐         ┌─────────────────────────┐
  │ SaveMetaData         │         │ optional<tick_number>   │
  │ EngineState          │         │ optional<EngineState>   │
  │ SceneManagerData     │         │ optional<SceneMgrData>  │
  │ SceneCollectionData  │         │ SceneCollectionData     │
  │ optional<EventBus>   │         │ optional<EventBus>      │
  └──────────────────────┘         └─────────────────────────┘
          │                                 │
          └────────Complete Overlap────────┘
                  ▲
                  │
     EngineSnapshot is superset with optional fields
     SaveData has required fields (non-optional)
     
  Note: EngineConfig NOT in either - user preferences and display
  settings are global user settings, not per-save or per-snapshot.
  Managed separately via preference system.
```

## 6. SaveData to TestData Conversion

### Proposed Workflow

```
┌─────────────────────────────────────────────────────────────────────────┐
│                SaveData → TestData Conversion Utility                    │
└─────────────────────────────────────────────────────────────────────────┘

  Gameplay Save                Converter                  Test Data
  ┌──────────────┐         ┌────────────┐            ┌──────────────┐
  │ SaveData     │         │ Convert    │            │ TestData     │
  │ - Metadata   │────────▶│ SaveDataTo │───────────▶│ - TestMeta   │
  │ - Config     │         │ TestData() │            │ - Starting   │
  │ - State      │         └────────────┘            │   Snapshot   │
  │ - Scenes     │                │                  │ - NumTicks=1 │
  │ - Entities   │                │                  └──────────────┘
  │ - Events     │                │                         │
  └──────────────┘                │                         │
                                  │                         ▼
                         Maps:    │                  Use in Tests
                         SaveMetaData → TestMetaData      ┌──────────┐
                         SceneCollectionData → Snapshot   │ Validate │
                         EventBus → Snapshot              │ Load/Save│
                                                          │ Workflow │
                                                          └──────────┘

  Use Cases:
  - Generate regression tests from real gameplay
  - Debug problematic save files
  - Validate save/load round-trip correctness
```

## 7. Complete Data Flow (Recommended)

```
┌─────────────────────────────────────────────────────────────────────────┐
│              COMPLETE ENGINE DATA FLOW (Recommended)                     │
└─────────────────────────────────────────────────────────────────────────┘

                         ┌───────────────┐
                         │  GameEngine   │
                         │  (Running)    │
                         └───────┬───────┘
                                 │
                    ┌────────────┴────────────┐
                    │                         │
                    ▼                         ▼
            ┌───────────────┐       ┌─────────────────┐
            │  Save Game    │       │ Generate Test   │
            │  (Production) │       │ (Development)   │
            └───────┬───────┘       └────────┬────────┘
                    │                        │
                    │ engine::export         │ engine::export
                    │ utilities              │ utilities
                    ▼                        ▼
            ┌───────────────┐       ┌─────────────────┐
            │   SaveData    │       │  EngineSnapshot │
            │   - Metadata  │       │  - Tick         │
            │   - State     │       │  - State        │
            │   - Scenes    │       │  - Scenes       │
            │   - Events    │       │  - Events       │
            └───────┬───────┘       └────────┬────────┘
                    │                        │
                    │ Note: EngineConfig     │ Note: EngineConfig
                    │ NOT in SaveData -      │ NOT needed for
                    │ user prefs are global  │ per-tick snapshots
                    │                        │
                    │ FlatBuffers            │ Convert
                    │ Serialize              │
                    ▼                        ▼
            ┌───────────────┐       ┌─────────────────┐
            │  .save file   │       │    TestData     │
            │  (Binary)     │       │    - TestMeta   │
            └───────┬───────┘       │    - Simulation │
                    │               │    - Snapshots  │
                    │               └────────┬────────┘
                    │ Load Game              │
                    ▼                        │ FlatBuffers
            ┌───────────────┐               │ Serialize
            │  GameEngine   │               ▼
            │  (Restored)   │       ┌─────────────────┐
            └───────────────┘       │ .test_data.bin  │
                                    │ (Binary)        │
                                    └────────┬────────┘
                                             │ Load Test
                                             ▼
                                    ┌─────────────────┐
                                    │   TestEngine    │
                                    │   (Validation)  │
                                    └─────────────────┘
```

## 8. Implementation Timeline

```
┌─────────────────────────────────────────────────────────────────────────┐
│                      IMPLEMENTATION ROADMAP                              │
└─────────────────────────────────────────────────────────────────────────┘

Phase 1: Core State Capture (High Priority)
┌────────────────────────────────────────────────────────────────┐
│ 1. Extend SaveData struct with EngineState, EventBus          │
│    (NOT EngineConfig - that's global user settings)           │
│ 2. Update save_data.fbs FlatBuffers schema                     │
│ 3. Update FlatbuffersSaveDataProvider                          │
│ 4. Implement GameEngine::SaveGame() method                     │
│ 5. Implement GameEngine::LoadGame() method                     │
└────────────────────────────────────────────────────────────────┘
         │
         ▼
Phase 2: Code Reuse (Medium Priority)
┌────────────────────────────────────────────────────────────────┐
│ 1. Create engine::export namespace with utility functions      │
│ 2. Implement ExportActiveScenes(), ExportEventBus(), etc.      │
│ 3. Refactor TestEngine::CaptureSnapShot() to use utilities     │
│ 4. Refactor GameEngine::SaveGame() to use utilities            │
│ 5. Add tests for export utilities                              │
└────────────────────────────────────────────────────────────────┘
         │
         ▼
Phase 3: Testing Integration (Low Priority)
┌────────────────────────────────────────────────────────────────┐
│ 1. Extend EngineSnapshot with optional<EngineState>,          │
│    optional<SceneManagerData> (NOT EngineConfig - global)     │
│ 2. Implement ConvertSaveDataToTestData() utility               │
│ 3. Add tools for test generation from gameplay                 │
│ 4. Update TestEngine to use extended EngineSnapshot            │
│ 5. Add documentation for test generation workflow              │
└────────────────────────────────────────────────────────────────┘
```

## Summary

**Key Insights:**
1. Entity import/export is already well-shared via `IEntityImporter` ✅
2. SaveData is incomplete - missing EngineState and EventBus ❌
3. EngineConfig should NOT be in SaveData - it's global user settings ✅
4. TestEngine and SaveData have good separation of concerns ✅
4. State export logic is duplicated (opportunity for utilities) 📝

**Recommended Changes:**
1. Extend SaveData to capture complete per-save state (EngineState, EventBus)
2. Create shared `engine::export` utilities
3. Align EngineSnapshot as superset of SaveData
4. Enable SaveData → TestData conversion

**Note:** EngineConfig (user preferences, display settings) should NOT be
included in SaveData - these are global user settings managed separately
via the preference system (default.preferences.bin and user files).

**Benefits:**
- Complete save/load functionality
- Code reuse between GameEngine and TestEngine
- Test generation from real gameplay
- Clear architectural boundaries
