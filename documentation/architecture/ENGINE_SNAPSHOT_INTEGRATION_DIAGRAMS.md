# EngineSnapshot Integration - Architecture Diagrams

This document provides visual representations of the EngineSnapshot integration architecture.

## High-Level Architecture

```
┌─────────────────────────────────────────────────────────────────────────┐
│                          EngineSnapshot                                  │
│                    (Central State Container)                             │
│  ┌──────────────────────────────────────────────────────────────────┐  │
│  │ • tick_number: optional<size_t>                                   │  │
│  │ • scene_collection_data: SceneCollectionData                      │  │
│  │ • global_event_bus: optional<EventBus>                            │  │
│  │ • engine_config: optional<EngineConfig>                           │  │
│  │ • engine_state: optional<EngineState>                             │  │
│  │ • engine_resources_config: optional<EngineResourcesConfig>        │  │
│  │ • loaded_assets: optional<AssetConfig>                            │  │
│  │ • scene_manager_data: optional<SceneManagerData>                  │  │
│  └──────────────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────────────┘
                                    │
                    ┌───────────────┼───────────────┐
                    │               │               │
                    ▼               ▼               ▼
        ┌───────────────┐  ┌──────────────┐  ┌──────────┐
        │  Save Files   │  │ Test Data    │  │ Network  │
        │  (.bin)       │  │ (TestData)   │  │ (future) │
        └───────────────┘  └──────────────┘  └──────────┘
```

## Interface Structure

```
╔═══════════════════════════════════════════════════════════════════════╗
║                      IEngineSnapshotProvider                           ║
║                      (Output Interface)                                ║
╠═══════════════════════════════════════════════════════════════════════╣
║  + CaptureSnapshot() const                                            ║
║      → std::expected<EngineSnapshot, FailInfo>                        ║
║                                                                        ║
║  + CapturePartialSnapshot(SnapshotFields fields) const                ║
║      → std::expected<EngineSnapshot, FailInfo>                        ║
╚═══════════════════════════════════════════════════════════════════════╝
                                  ▲
                                  │ implements
                                  │
╔═══════════════════════════════════════════════════════════════════════╗
║                           Engine                                       ║
║                    (Base Implementation)                               ║
╠═══════════════════════════════════════════════════════════════════════╣
║  # m_engine_resources: EngineResources                                ║
║  # m_game_context: GameContext                                        ║
║  # m_scene_manager: SceneManager                                      ║
║  # m_engine_config: EngineConfig                                      ║
║  # m_engine_state: EngineState                                        ║
╠═══════════════════════════════════════════════════════════════════════╣
║  + CaptureSnapshot() const : EngineSnapshot                           ║
║  + CapturePartialSnapshot(fields) const : EngineSnapshot              ║
║  + ConsumeSnapshot(snapshot) : expected<monostate, FailInfo>          ║
║  + ApplySnapshotUpdate(snapshot, mode) : expected<monostate, FailInfo>║
╚═══════════════════════════════════════════════════════════════════════╝
                                  │ implements
                                  ▼
╔═══════════════════════════════════════════════════════════════════════╗
║                      IEngineSnapshotConsumer                           ║
║                       (Input Interface)                                ║
╠═══════════════════════════════════════════════════════════════════════╣
║  + ConsumeSnapshot(const EngineSnapshot& snapshot)                    ║
║      → std::expected<std::monostate, FailInfo>                        ║
║                                                                        ║
║  + ApplySnapshotUpdate(const EngineSnapshot& snapshot,                ║
║                        SnapshotMode mode)                              ║
║      → std::expected<std::monostate, FailInfo>                        ║
╚═══════════════════════════════════════════════════════════════════════╝
```

## Data Flow: Current vs Proposed

### Current Flow (Without EngineSnapshot Integration)

```
┌──────────────────┐
│ JSON/Binary File │
└────────┬─────────┘
         │
         ▼
┌────────────────────────────┐
│ IEngineDataProvider        │
│ LoadEngineData()           │
└────────┬───────────────────┘
         │
         ▼
┌────────────────────────────┐
│ EngineData                 │
│ • EngineResourcesConfig    │
│ • EngineState              │
│ • EngineConfig             │
│ • AssetConfig              │
└────────┬───────────────────┘
         │
         ▼
┌────────────────────────────┐
│ Engine::StartUp()          │
│ Configures engine directly │
└────────────────────────────┘
```

### Proposed Flow (With EngineSnapshot Integration)

```
┌──────────────────┐
│ JSON/Binary File │
└────────┬─────────┘
         │
         ▼
┌─────────────────────────────┐
│ IEngineDataProvider         │
│ LoadEngineData()            │
└────────┬────────────────────┘
         │
         ▼
┌─────────────────────────────┐      ┌─────────────────────────┐
│ EngineData                  │──────│ ConvertEngineDataTo     │
│                             │      │ Snapshot()              │
└─────────────────────────────┘      └────────┬────────────────┘
                                              │
                                              ▼
                                     ┌──────────────────────┐
                                     │ EngineSnapshot       │
                                     │ (Unified Container)  │
                                     └────────┬─────────────┘
                                              │
                                              ▼
                                     ┌──────────────────────┐
                                     │ Engine::             │
                                     │ ConsumeSnapshot()    │
                                     └──────────────────────┘
```

## Snapshot Capture Flow (Export)

```
┌──────────────────────────────────────────────────────────────────┐
│                   User/System Request                             │
│                   engine.CaptureSnapshot()                        │
└────────────────────────────┬─────────────────────────────────────┘
                             │
                             ▼
┌──────────────────────────────────────────────────────────────────┐
│              Engine::CaptureSnapshot()                            │
│              Orchestrates snapshot creation                       │
└────┬───────────────┬───────────────┬──────────────┬──────────────┘
     │               │               │              │
     ▼               ▼               ▼              ▼
┌─────────┐  ┌──────────────┐  ┌────────────┐  ┌─────────────┐
│ Capture │  │ Capture      │  │ Capture    │  │ Capture     │
│ Config  │  │ State        │  │ Resources  │  │ Scenes      │
└────┬────┘  └──────┬───────┘  └─────┬──────┘  └──────┬──────┘
     │              │                 │                │
     └──────────────┴─────────────────┴────────────────┘
                             │
                             ▼
                ┌─────────────────────────┐
                │   EngineSnapshot        │
                │   (Complete State)      │
                └────────────┬────────────┘
                             │
                 ┌───────────┼───────────┐
                 │           │           │
                 ▼           ▼           ▼
         ┌────────────┐ ┌────────┐ ┌─────────┐
         │ Save File  │ │ Test   │ │ Network │
         └────────────┘ └────────┘ └─────────┘
```

## Snapshot Consumption Flow (Import)

```
┌──────────────────────────────────────────────────────────────────┐
│                     Snapshot Source                               │
│        (File, Test Data, Network, Manual Creation)                │
└────────────────────────────┬─────────────────────────────────────┘
                             │
                             ▼
                ┌─────────────────────────┐
                │   EngineSnapshot        │
                │   (May be partial)      │
                └────────────┬────────────┘
                             │
                             ▼
┌──────────────────────────────────────────────────────────────────┐
│              Engine::ConsumeSnapshot(snapshot)                    │
│              Validates and applies snapshot data                  │
└────┬───────────────┬───────────────┬──────────────┬──────────────┘
     │               │               │              │
     ▼               ▼               ▼              ▼
┌─────────────┐ ┌──────────────┐ ┌────────────┐ ┌──────────────┐
│ Configure   │ │ Configure    │ │ Configure  │ │ Configure    │
│ Resources   │ │ State        │ │ Config     │ │ Scenes       │
│ (if present)│ │ (if present) │ │(if present)│ │ (if present) │
└──────┬──────┘ └──────┬───────┘ └─────┬──────┘ └──────┬───────┘
       │               │               │              │
       └───────────────┴───────────────┴──────────────┘
                             │
                             ▼
                ┌─────────────────────────┐
                │   Engine Configured     │
                │   Ready to Run          │
                └─────────────────────────┘
```

## TestEngine Integration

```
┌────────────────────────────────────────────────────────────────────┐
│                          TestData                                   │
│  ┌──────────────────────────────────────────────────────────────┐ │
│  │ • meta_data: TestMetaData                                     │ │
│  │ • simulation_data: SimulationData                             │ │
│  │ • number_of_ticks: uint32_t                                   │ │
│  │ • starting_engine_snapshot: EngineSnapshot                    │ │
│  │ • expected_engine_snapshots: map<size_t, EngineSnapshot>      │ │
│  └──────────────────────────────────────────────────────────────┘ │
└────────────────────────────┬───────────────────────────────────────┘
                             │
                             ▼
┌────────────────────────────────────────────────────────────────────┐
│                   TestEngine(test_data)                             │
│                   Constructor                                       │
└────────────────────────────┬───────────────────────────────────────┘
                             │
                             ▼
┌────────────────────────────────────────────────────────────────────┐
│              ConsumeSnapshot(starting_engine_snapshot)              │
│              Initialize engine from test data                       │
└────────────────────────────┬───────────────────────────────────────┘
                             │
                             ▼
┌────────────────────────────────────────────────────────────────────┐
│                     RunGameLoop()                                   │
│          ┌────────────────────────────────────┐                    │
│          │  for each tick (1..number_of_ticks)│                    │
│          │  {                                  │                    │
│          │    ExecuteTick()                    │                    │
│          │    CaptureSnapshot(tick)            │──┐                 │
│          │  }                                  │  │                 │
│          └────────────────────────────────────┘  │                 │
└─────────────────────────────────────────────────┼─────────────────┘
                                                   │
                                                   ▼
┌────────────────────────────────────────────────────────────────────┐
│                        m_data_bank                                  │
│              map<size_t, EngineSnapshot>                            │
│  ┌──────────────────────────────────────────────────────────────┐ │
│  │ tick 1 → EngineSnapshot                                       │ │
│  │ tick 2 → EngineSnapshot                                       │ │
│  │ tick 3 → EngineSnapshot                                       │ │
│  │ ...                                                            │ │
│  └──────────────────────────────────────────────────────────────┘ │
└────────────────────────────┬───────────────────────────────────────┘
                             │
                             ▼
┌────────────────────────────────────────────────────────────────────┐
│                     Test Validation                                 │
│       Compare m_data_bank with expected_engine_snapshots            │
└────────────────────────────────────────────────────────────────────┘
```

## Save/Load Integration

```
┌────────────────────────────────────────────────────────────────────┐
│                         Save Flow                                   │
└────────────────────────────────────────────────────────────────────┘

    ┌─────────────┐
    │   Engine    │
    │  (running)  │
    └──────┬──────┘
           │
           │ CaptureSnapshot()
           ▼
    ┌─────────────────┐
    │ EngineSnapshot  │
    └──────┬──────────┘
           │
           │ ConvertSnapshotToSaveData()
           ▼
    ┌─────────────────┐      ┌──────────────────┐
    │   SaveData      │──────│ • SaveMetaData   │
    │                 │      │ • SceneManager   │
    │                 │      │   Data           │
    │                 │      │ • SceneCollection│
    └──────┬──────────┘      └──────────────────┘
           │
           │ Serialize via FlatBuffers
           ▼
    ┌─────────────────┐
    │  Binary File    │
    │  (.bin)         │
    └─────────────────┘

┌────────────────────────────────────────────────────────────────────┐
│                         Load Flow                                   │
└────────────────────────────────────────────────────────────────────┘

    ┌─────────────────┐
    │  Binary File    │
    │  (.bin)         │
    └──────┬──────────┘
           │
           │ Deserialize via FlatBuffers
           ▼
    ┌─────────────────┐
    │   SaveData      │
    └──────┬──────────┘
           │
           │ ConvertSaveDataToSnapshot()
           ▼
    ┌─────────────────┐
    │ EngineSnapshot  │
    └──────┬──────────┘
           │
           │ ConsumeSnapshot()
           ▼
    ┌─────────────┐
    │   Engine    │
    │ (restored)  │
    └─────────────┘
```

## Partial Snapshot Example

```
┌────────────────────────────────────────────────────────────────────┐
│              Use Case: Testing Scene Logic Only                     │
└────────────────────────────────────────────────────────────────────┘

Test needs:
  ✓ Scene entity data
  ✓ Tick number for context
  ✗ Engine config (use defaults)
  ✗ Resource config (use defaults)
  ✗ Event bus (not relevant)

CapturePartialSnapshot(SnapshotFields::SceneData | SnapshotFields::TickNumber)
                             │
                             ▼
                ┌──────────────────────────┐
                │   EngineSnapshot         │
                ├──────────────────────────┤
                │ tick_number: 5           │ ✓ Present
                │ scene_collection_data: […]│ ✓ Present
                │ global_event_bus: nullopt│ ✗ Absent
                │ engine_config: nullopt   │ ✗ Absent
                │ engine_state: nullopt    │ ✗ Absent
                │ ...                      │ ✗ Absent
                └──────────────────────────┘

Benefits:
  • Smaller snapshot size
  • Faster capture
  • Clearer test intent
  • Less test maintenance
```

## SnapshotMode Behaviors

```
┌────────────────────────────────────────────────────────────────────┐
│                    SnapshotMode::Initialize                         │
│                    (Cold Start)                                     │
└────────────────────────────────────────────────────────────────────┘

Before: Engine not configured
After:  Engine fully configured from snapshot
        Missing fields → defaults

┌────────────────────────────────────────────────────────────────────┐
│                    SnapshotMode::Restore                            │
│                    (Hot Restore)                                    │
└────────────────────────────────────────────────────────────────────┘

Before: Engine running with state A
After:  Engine state replaced with snapshot state B
        Missing fields → keep current values

┌────────────────────────────────────────────────────────────────────┐
│                    SnapshotMode::Merge                              │
│                    (Selective Update)                               │
└────────────────────────────────────────────────────────────────────┘

Before: Engine running with state A
After:  Engine state = merge(A, snapshot)
        Present fields → updated from snapshot
        Missing fields → keep current values
```

## Class Hierarchy

```
                    ┌──────────────────────┐
                    │ IEngineSnapshot      │
                    │ Provider             │
                    └──────────┬───────────┘
                               │
                    ┌──────────┴───────────┐
                    │                      │
                    ▼                      ▼
        ┌──────────────────┐  ┌──────────────────┐
        │  Engine          │  │ Future           │
        │  (base class)    │  │ Implementations  │
        └────────┬─────────┘  └──────────────────┘
                 │
         ┌───────┴────────┐
         │                │
         ▼                ▼
┌──────────────┐  ┌──────────────┐
│ GameEngine   │  │ TestEngine   │
└──────────────┘  └──────────────┘


                    ┌──────────────────────┐
                    │ IEngineSnapshot      │
                    │ Consumer             │
                    └──────────┬───────────┘
                               │
                    ┌──────────┴───────────┐
                    │                      │
                    ▼                      ▼
        ┌──────────────────┐  ┌──────────────────┐
        │  Engine          │  │ Future           │
        │  (base class)    │  │ Implementations  │
        └────────┬─────────┘  └──────────────────┘
                 │
         ┌───────┴────────┐
         │                │
         ▼                ▼
┌──────────────┐  ┌──────────────┐
│ GameEngine   │  │ TestEngine   │
└──────────────┘  └──────────────┘
```

## Conversion Utilities

```
┌────────────────────────────────────────────────────────────────────┐
│                   Conversion Utility Functions                      │
└────────────────────────────────────────────────────────────────────┘

EngineData ────────────────────────────────────────> EngineSnapshot
           ConvertEngineDataToSnapshot()
             • engine_config → engine_config
             • engine_state → engine_state
             • engine_resources_config → engine_resources_config
             • initial_asset_config → loaded_assets

EngineSnapshot ────────────────────────────────────> SaveData
                ConvertSnapshotToSaveData()
                  + SaveMetaData (user-provided)
                  • scene_collection_data → scene_collection_data
                  • scene_manager_data → scene_manager_data
                  • [meta added] → save_meta_data

SaveData ──────────────────────────────────────────> EngineSnapshot
          ConvertSaveDataToSnapshot()
            • scene_collection_data → scene_collection_data
            • scene_manager_data → scene_manager_data
            • [config fields need separate load]
```

## Summary Comparison

```
┌─────────────────────────────────────────────────────────────────────┐
│                  Before (Current)     │     After (Proposed)         │
├───────────────────────────────────────┼──────────────────────────────┤
│ Multiple data structures:             │ Single unified structure:    │
│  • EngineData                         │  • EngineSnapshot            │
│  • EngineSnapshot                     │                              │
│  • SaveData                           │ (SaveData uses snapshot)     │
│                                       │                              │
│ Separate initialization paths:        │ Unified initialization:      │
│  • IEngineDataProvider                │  • IEngineSnapshotConsumer   │
│  • Manual configuration               │                              │
│                                       │                              │
│ Limited testing support:               │ Rich testing support:        │
│  • TestEngine uses snapshots          │  • Full/partial snapshots    │
│  • But not for initialization         │  • Snapshot-based init       │
│                                       │  • Easy test data creation   │
│                                       │                              │
│ No save/load via snapshot:            │ Save/load via snapshot:      │
│  • SaveData separate                  │  • Snapshot ↔ SaveData       │
│  • No round-trip guarantee            │  • Round-trip guaranteed     │
└───────────────────────────────────────┴──────────────────────────────┘
```

## Legend

```
┌─────┐
│ Box │  - Container/Struct/Class
└─────┘

   │
   ▼     - Data Flow Direction

  ─────  - Relationship/Inheritance

╔═════╗
║ Box ║  - Interface (double lines)
╚═════╝

┌─────┐
│ Box │  - Implementation (single lines)
└─────┘

  →     - Transformation/Conversion

  ↔     - Bidirectional relationship
```
