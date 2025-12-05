# SteamRot Workflow Diagrams - Current State

**Generated**: December 5, 2025  
**Purpose**: Visual reference for how systems work NOW (not proposals)

---

## Table of Contents

1. [Data Loading Flow](#data-loading-flow)
2. [Test Execution Paths](#test-execution-paths)
3. [Provider Pattern Status](#provider-pattern-status)
4. [Testing Decision Flow](#testing-decision-flow)
5. [Entity Configuration Flow](#entity-configuration-flow)
6. [GameEngine vs TestEngine Tick](#gameengine-vs-testengine-tick)

---

## Data Loading Flow

### Current Implementation (Mixed Providers)

```
┌────────────────────────────────────────────────────────────────┐
│                    GAME CODE LAYER                             │
│                                                                │
│  Needs data for:                                               │
│  ├─ Engine configuration  ────────────────────┐                │
│  ├─ Scene configuration   ────────────────────┤                │
│  ├─ Asset loading         ────────────────────┤                │
│  ├─ Fragment templates    ────────────────────┤                │
│  ├─ Game configuration    ────────────────────┤                │
│  ├─ Entity/Component data ─────────────────┐  │                │
│  └─ UI styles            ─────────────────┐│  │                │
└────────────────────────────────────────────┼┼──┼────────────────┘
                                             ││  │
                   ┌─────────────────────────┘│  │
                   │                          │  │
                   │  ┌───────────────────────┘  │
                   │  │                          │
                   │  │  ┌───────────────────────┘
                   │  │  │
                   ▼  ▼  ▼
┌────────────────────────────────────────────────────────────────┐
│                 INTERFACE LAYER (where available)              │
│                                                                │
│  ✅ IEngineDataProvider → EngineCoreData (native struct)      │
│  ✅ ISceneDataProvider  → SceneCoreData  (native struct)      │
│  ✅ IAssetDataProvider  → AssetData      (native struct)      │
│  ✅ IFragmentDataProvider → Fragment     (native struct)      │
│  ✅ IGameConfigProvider → EngineData*    (FlatBuffers temp)   │
│                                                                │
│  ❌ No interface for entity/component configuration           │
│  ❌ No interface for UI style loading                         │
└────────────────────────────────────────────────────────────────┘
                           │
                           │ 5 providers use this
                           │
                           ▼
┌────────────────────────────────────────────────────────────────┐
│              IMPLEMENTATION LAYER                              │
│                                                                │
│  • FlatbuffersEngineDataProvider                              │
│  • FlatbuffersSceneDataProvider                               │
│  • FlatbuffersAssetDataProvider                               │
│  • FlatbuffersFragmentDataProvider                            │
│  • FlatbuffersGameConfigProvider                              │
│                                                                │
│  All wrap FlatbuffersDataLoader internally                    │
└────────────────────────────────────────────────────────────────┘
                           │
          ┌────────────────┼────────────────┐
          │                │                │
          ▼                ▼                ▼
┌──────────────────────────────────────────────────────────────┐
│              DATA LOADER LAYER                               │
│                                                              │
│  FlatbuffersDataLoader (internal implementation)            │
│  ├─ Used by: All 5 providers (wrapped) ✅                   │
│  ├─ Used by: FlatbuffersConfigurator (direct) ❌            │
│  └─ Used by: StylesConfigurator (direct) ❌                 │
│                                                              │
│  Methods:                                                    │
│  • LoadBinaryData(filename) → binary buffer                 │
│  • ProvideEngineData() → EngineData*                        │
│  • ProvideSceneManagerData() → SceneManagerData*            │
│  • ProvideDefaultSceneData(type) → DefaultSceneData*        │
│  • ProvideAssetData() → AssetCollection*                    │
│  • ProvideUIStylesData(names) → UIStyleCollection*          │
│  • ProvideFragment(name) → FragmentData*                    │
│  • ProvideContextData() → ContextData*                      │
│  • etc.                                                      │
└──────────────────────────────────────────────────────────────┘
                           │
                           │ Reads files
                           │
                           ▼
┌──────────────────────────────────────────────────────────────┐
│                    FILE SYSTEM                               │
│                                                              │
│  data/defaults/                                              │
│  ├─ engine/engine_data.bin                                  │
│  ├─ scene_manager/scene_manager_data.bin                    │
│  ├─ scenes/*.scene_data.bin                                 │
│  ├─ asset_manager/assets.bin                                │
│  └─ fragments/*.fragment.bin                                │
│                                                              │
│  tests/data/defaults/                                        │
│  └─ (same structure for test data)                          │
└──────────────────────────────────────────────────────────────┘
```

### Legend

- ✅ Uses provider pattern (good)
- ❌ Direct FlatbuffersDataLoader usage (inconsistent)
- ⚠️ Returns FlatBuffers types temporarily (compromise)

---

## Test Execution Paths

### Two Complementary Approaches

```
┌──────────────────────────────────────────────────────────────┐
│              WHEN TO USE WHICH APPROACH?                     │
└──────────────────────────────────────────────────────────────┘
                            │
                ┌───────────┴───────────┐
                │                       │
                ▼                       ▼
        ┌──────────────┐        ┌──────────────┐
        │ Unit Testing │        │ Integration  │
        │ Simple Setup │        │ Multi-Tick   │
        │ Fast (< 1ms) │        │ Data-Driven  │
        └──────────────┘        └──────────────┘
                │                       │
                ▼                       ▼
        ┌──────────────┐        ┌──────────────┐
        │ TestFixture  │        │ TestEngine + │
        │              │        │ Test Harness │
        └──────────────┘        └──────────────┘


╔════════════════════════════════════════════════════════════════╗
║                    PATH 1: TestFixture                         ║
║                    (Unit Testing)                              ║
╚════════════════════════════════════════════════════════════════╝

TEST_CASE("...", "[unit]") {
    │
    ▼
┌────────────────────────────┐
│ 1. Create TestFixture      │
│    TestFixture fixture;    │
│    fixture.Initialize();   │
└────────────────────────────┘
    │
    ▼
┌────────────────────────────┐
│ 2. Get Contexts            │
│    auto& game_ctx =        │
│      fixture.GetGameCtx(); │
│    auto& scene_ctx =       │
│      fixture.GetSceneCtx();│
└────────────────────────────┘
    │
    ▼
┌────────────────────────────┐
│ 3. Manual Setup            │
│    • Create entities       │
│    • Set component values  │
│    • Configure state       │
└────────────────────────────┘
    │
    ▼
┌────────────────────────────┐
│ 4. Create Subject          │
│    MyLogic logic(context); │
└────────────────────────────┘
    │
    ▼
┌────────────────────────────┐
│ 5. Execute                 │
│    logic.RunLogic();       │
└────────────────────────────┘
    │
    ▼
┌────────────────────────────┐
│ 6. Assert                  │
│    REQUIRE(x == y);        │
└────────────────────────────┘
    │
    ▼
   PASS/FAIL


╔════════════════════════════════════════════════════════════════╗
║                    PATH 2: TestEngine                          ║
║                    (Integration/Data-Driven)                   ║
╚════════════════════════════════════════════════════════════════╝

TEST_CASE("...", "[integration]") {
    │
    ▼
┌────────────────────────────────────┐
│ 1. Create test_data.json           │
│    • metadata (name, description)  │
│    • start_entity_collection       │
│    • simulation_data (logic list)  │
│    • tick_snapshots (expected)     │
└────────────────────────────────────┘
    │
    ▼
┌────────────────────────────────────┐
│ 2. Build project                   │
│    CMake generates .test_data.bin  │
└────────────────────────────────────┘
    │
    ▼
┌────────────────────────────────────┐
│ 3. Load Configurations             │
│    auto configs =                  │
│      load_test_data_configs();     │
└────────────────────────────────────┘
    │
    ▼
┌────────────────────────────────────┐
│ 4. Use Catch2 Generator            │
│    const auto* config =            │
│      GENERATE(from_range(configs));│
└────────────────────────────────────┘
    │
    ▼
┌────────────────────────────────────┐
│ 5. Run TestEngine                  │
│    auto result =                   │
│      RunTestEngineTest(config);    │
│                                    │
│    Internally:                     │
│    • Creates TestEngine            │
│    • Loads start_entity_collection │
│    • Runs N ticks                  │
│    • Executes simulation steps     │
│    • Captures state to data_bank   │
│    • Compares with tick_snapshots  │
└────────────────────────────────────┘
    │
    ▼
┌────────────────────────────────────┐
│ 6. Automatic Assertions            │
│    Uses matchers to compare:       │
│    • EntityMemoryPool state        │
│    • Component values              │
│    • Archetype membership          │
└────────────────────────────────────┘
    │
    ▼
   PASS/FAIL (with detailed diffs)
```

---

## Provider Pattern Status

### What's Done vs What's Not

```
┌──────────────────────────────────────────────────────────────┐
│                  PROVIDER PATTERN STATUS                     │
│                  As of December 2025                         │
└──────────────────────────────────────────────────────────────┘

Data Type          Interface?   Implementation?   Native Struct?
────────────────────────────────────────────────────────────────
Engine Core        ✅ Yes       ✅ FlatBuffers    ✅ EngineCoreData
Scene Core         ✅ Yes       ✅ FlatBuffers    ✅ SceneCoreData
Assets             ✅ Yes       ✅ FlatBuffers    ✅ AssetData
Fragments          ✅ Yes       ✅ FlatBuffers    ✅ Fragment
Game Config        ✅ Yes       ✅ FlatBuffers    ⚠️  Still FlatBuffers*
Entities           ❌ No        ❌ Direct         ❌ Still FlatBuffers*
UI Styles          ❌ No        ❌ Direct         ❌ Still FlatBuffers*
────────────────────────────────────────────────────────────────

Status:
✅ = Fully implemented
⚠️  = Temporary (returns FlatBuffers, will convert to native later)
❌ = Not implemented (intentional - see deferred section)

* These still use FlatBuffers types in game code


┌──────────────────────────────────────────────────────────────┐
│              WHERE CODE GETS DATA TODAY                      │
└──────────────────────────────────────────────────────────────┘

Engine.cpp
    │
    ├─ Engine core config
    │  └─► IEngineDataProvider ✅
    │      └─► EngineCoreData (native)
    │
    └─ User preferences
       └─► IUserPreferencesProvider ✅
           └─► UserPreferences (native)

GameEngine.cpp
    │
    └─ Game configuration
       └─► IGameConfigProvider ✅
           └─► EngineData* (FlatBuffers ⚠️)

SceneFactory.cpp
    │
    └─ Scene core config
       └─► ISceneDataProvider ✅
           └─► SceneCoreData (native)

AssetManager.cpp
    │
    └─ Asset definitions
       └─► IAssetDataProvider ✅
           └─► AssetData (native)

FlatbuffersConfigurator.cpp
    │
    └─ Entity/Component data
       └─► FlatbuffersDataLoader ❌ (direct!)
           └─► EntityCollection* (FlatBuffers)

StylesConfigurator.cpp
    │
    └─ UI styles
       └─► FlatbuffersDataLoader ❌ (direct!)
           └─► UIStyleCollection* (FlatBuffers)
```

### Why This Mixed State?

```
┌──────────────────────────────────────────────────────────────┐
│                 DESIGN DECISIONS                             │
└──────────────────────────────────────────────────────────────┘

Decision 1: Complete 5 core providers first
  Reason: 80/20 rule - biggest value for least effort
  Result: ✅ Done (Dec 2024)

Decision 2: Leave entity config as FlatBuffers direct
  Reason: Complex (polymorphic UIElement hierarchy), high risk
  Effort: 4-6 weeks estimated
  Result: 🚫 Deferred indefinitely

Decision 3: Leave UI styles as FlatBuffers direct
  Reason: Low priority, works fine, ~8 hours not worth it
  Result: 🚫 Deferred indefinitely

Decision 4: IGameConfigProvider returns FlatBuffers temporarily
  Reason: Complex types (subscriptions, event bus)
  Plan: Convert to native in Phase 4 (maybe)
  Result: ⚠️ Working compromise
```

---

## Testing Decision Flow

### How to Choose Test Approach

```
                    START: Need to test something
                                │
                                ▼
                    ┌────────────────────────┐
                    │ What are you testing?  │
                    └────────────────────────┘
                                │
                 ┌──────────────┴──────────────┐
                 │                             │
                 ▼                             ▼
        ┌─────────────────┐         ┌──────────────────┐
        │ Single class or │         │ Multiple classes │
        │ function        │         │ or systems       │
        └─────────────────┘         └──────────────────┘
                 │                             │
                 ▼                             ▼
        ┌─────────────────┐         ┌──────────────────┐
        │ Fast execution? │         │ State changes    │
        │ (< 1ms)         │         │ over time?       │
        └─────────────────┘         └──────────────────┘
                 │                             │
         ┌───────┴───────┐           ┌─────────┴─────────┐
         │               │           │                   │
         ▼               ▼           ▼                   ▼
    ┌────────┐      ┌────────┐  ┌────────┐         ┌────────┐
    │  YES   │      │  NO    │  │  YES   │         │  NO    │
    │        │      │ Complex│  │ (3+    │         │ Simple │
    │        │      │ Setup  │  │ ticks) │         │ (1-2   │
    │        │      │        │  │        │         │ ticks) │
    └────────┘      └────────┘  └────────┘         └────────┘
         │               │           │                   │
         │               │           │                   │
         └───────┬───────┘           └─────────┬─────────┘
                 │                             │
                 ▼                             ▼
        ┌─────────────────┐         ┌──────────────────┐
        │                 │         │                  │
        │  USE            │         │  USE             │
        │  TestFixture    │         │  TestEngine      │
        │                 │         │  + Harness       │
        │  • Unit test    │         │                  │
        │  • Fast         │         │  • Integration   │
        │  • Simple       │         │  • Multi-tick    │
        │  • C++ only     │         │  • Data-driven   │
        │  • Direct       │         │  • JSON config   │
        │    assertions   │         │  • Snapshots     │
        │                 │         │                  │
        └─────────────────┘         └──────────────────┘


┌──────────────────────────────────────────────────────────────┐
│                  EXAMPLES BY USE CASE                        │
└──────────────────────────────────────────────────────────────┘

Use TestFixture when:
  ├─ Testing Logic class construction
  ├─ Testing Logic class single method
  ├─ Testing free function with simple inputs
  ├─ Testing component default values
  ├─ Testing helper function correctness
  └─ Need fast feedback (<1ms per test)

Use TestEngine when:
  ├─ Testing UI interaction sequence (click → state change)
  ├─ Testing entity state changes over 5+ ticks
  ├─ Testing event sequences (fire event → wait → check result)
  ├─ Testing data-driven scenarios (same test, many configs)
  ├─ Testing tick-by-tick state evolution
  └─ Need snapshot comparison at specific ticks


┌──────────────────────────────────────────────────────────────┐
│                  MIGRATION PATH                              │
└──────────────────────────────────────────────────────────────┘

Start with TestFixture
        │
        ▼
    Write test
        │
        ▼
    Runs fast? ───YES──► Keep TestFixture
        │
        NO
        │
        ▼
    Need multiple ticks? ───NO──► Simplify test, keep TestFixture
        │
        YES
        │
        ▼
    Extract to JSON
        │
        ▼
    Create test_data.json
        │
        ▼
    Convert to TestEngine
        │
        ▼
    Validate results match
```

---

## Entity Configuration Flow

### Current Implementation (Direct FlatBuffers)

```
┌──────────────────────────────────────────────────────────────┐
│               ENTITY CONFIGURATION FLOW                      │
│               (Current - No Provider)                        │
└──────────────────────────────────────────────────────────────┘

Scene needs entities
    │
    ▼
┌──────────────────────────────────────┐
│ SceneFactory::CreateDefaultScene()   │
│ or TestEngine::StartUp()             │
└──────────────────────────────────────┘
    │
    ▼
┌──────────────────────────────────────┐
│ FlatbuffersConfigurator              │
│ .ConfigureEntitiesFromDefaultData()  │
│                                      │
│ OR                                   │
│                                      │
│ .ConfigureEntitiesFromCollection()   │
│   (for testing)                      │
└──────────────────────────────────────┘
    │
    │ Direct usage!
    │
    ▼
┌──────────────────────────────────────┐
│ FlatbuffersDataLoader                │
│ .ProvideDefaultSceneData(scene_type) │
│                                      │
│ Returns: DefaultSceneData*           │
│   └─ entity_collection: EntityColl*  │
│       └─ entities: vector<Entity*>   │
└──────────────────────────────────────┘
    │
    │ All FlatBuffers types
    │
    ▼
┌──────────────────────────────────────┐
│ For each entity in collection:       │
│                                      │
│ ConfigureComponent<CMeta>()          │
│ ConfigureComponent<CUserInterface>() │
│ ConfigureComponent<CGrimoireMachina>│
│ ConfigureComponent<CUIState>()       │
│ etc.                                 │
│                                      │
│ All work with FlatBuffers types      │
└──────────────────────────────────────┘
    │
    ▼
┌──────────────────────────────────────┐
│ EntityMemoryPool populated           │
│ ├─ Entities created                  │
│ ├─ Components configured             │
│ └─ Archetypes updated                │
└──────────────────────────────────────┘


┌──────────────────────────────────────────────────────────────┐
│                   WHY NO PROVIDER HERE?                      │
└──────────────────────────────────────────────────────────────┘

Problem 1: Complex Type Hierarchy
  • CUserInterface contains std::unique_ptr<UIElement>
  • UIElement has 7 derived types (Button, Panel, TextBox, etc.)
  • Polymorphic hierarchy needs deep copying
  • Native struct would be massive

Problem 2: High Migration Cost
  • Need to convert all UIElement types
  • Need to implement Clone() pattern
  • Need to rewrite FlatbuffersConfigurator
  • Estimated: 4-6 weeks (HIGH RISK)

Problem 3: Low Incremental Value
  • Can test with TestDataConfig.start_entity_collection
  • Can inject test data without provider
  • FlatBuffers binary works fine
  • 80% of provider benefits already achieved

Decision: DEFERRED INDEFINITELY
```

---

## GameEngine vs TestEngine Tick

### Execution Comparison

```
╔══════════════════════════════════════════════════════════════╗
║                    GAMEENGINE TICK                           ║
║                    (Production)                              ║
╚══════════════════════════════════════════════════════════════╝

while (window.isOpen()) {
    │
    ▼
┌────────────────────────────────────┐
│ ExecuteSystemsTick()               │
└────────────────────────────────────┘
    │
    ├─► EventHandler::ExecuteEventHandlerLevelLogic()
    │   ├─ PreloadEvents(window)
    │   ├─ ProcessWaitingRoomEventBus()
    │   └─ UpdateSubscribersFromGlobalEventBus()
    │
    ├─► ExecuteEngineLevelLogic()
    │   └─ ProcessSubscriptions()
    │       └─ Check EVENT_QUIT_GAME
    │
    ├─► SceneManager::ExecuteSceneManagerLevelLogic()
    │   └─ ProcessSubscriptions()
    │       └─ Check EVENT_CHANGE_SCENE
    │
    ├─► ExecuteSceneLevelLogic()
    │   └─ SceneManager::UpdateScenes()
    │       └─ For each active scene:
    │           ├─ sAction()    (Action Logic)
    │           ├─ sCollision() (Collision Logic)
    │           └─ sRender()    (Render Logic)
    │
    └─► ExecuteDisplayManagerTick()
        └─ DisplayManager::CallRenderCycle()
            ├─ Clear window
            ├─ Draw all scene textures
            └─ Display window
    │
    ▼
┌────────────────────────────────────┐
│ loop_number++                      │
└────────────────────────────────────┘
    │
    └──► Continue loop


╔══════════════════════════════════════════════════════════════╗
║                    TESTENGINE TICK                           ║
║                    (Testing)                                 ║
╚══════════════════════════════════════════════════════════════╝

for (tick = 1; tick <= target_ticks; tick++) {
    │
    ▼
┌────────────────────────────────────┐
│ ExecuteSystemsTick()               │
│ (Same as GameEngine)               │
└────────────────────────────────────┘
    │
    ├─► EventHandler::ExecuteEventHandlerLevelLogic()
    │   ├─ ProcessWaitingRoomEventBus()
    │   └─ UpdateSubscribersFromGlobalEventBus()
    │
    ├─► ExecuteEngineLevelLogic()
    │   └─ ProcessSubscriptions() [no-op for tests]
    │
    ├─► SceneManager::ExecuteSceneManagerLevelLogic()
    │   └─ ProcessSubscriptions()
    │
    ├─► ExecuteSceneLevelLogic()
    │   └─ ⚠️ DIFFERENT: SimulationRunner
    │       └─ RunSimulationStep(simulation_data, tick)
    │           │
    │           ├─ ExecuteInputEventsForTick()
    │           ├─ ExecuteEventsForTick()
    │           └─ ExecuteSimulation()
    │               └─ Run configured logic
    │                   (FullEngine / SceneManager /
    │                    SceneLogic / Custom)
    │
    └─► ExecuteDisplayManagerTick() [no-op for tests]
    │
    ▼
┌────────────────────────────────────┐
│ AddToDataBank(tick)                │
│ • Capture EntityMemoryPool state   │
│ • Store in m_data_bank[tick]       │
└────────────────────────────────────┘
    │
    ▼
┌────────────────────────────────────┐
│ current_tick++                     │
└────────────────────────────────────┘
    │
    └──► Continue loop (until target_ticks)


┌──────────────────────────────────────────────────────────────┐
│                   KEY DIFFERENCES                            │
└──────────────────────────────────────────────────────────────┘

Aspect              GameEngine           TestEngine
──────────────────────────────────────────────────────────────
Loop Control        window.isOpen()      Fixed tick count
Input Source        SFML events          JSON config
Scene Logic         SceneManager         SimulationRunner
Logic Execution     Fixed pipeline       Configurable
Display Output      Window rendered      No display
State Capture       None                 Data bank
Comparison          None                 Tick snapshots
Purpose             Play game            Validate behavior


┌──────────────────────────────────────────────────────────────┐
│                   EVENT ORDERING ISSUE                       │
└──────────────────────────────────────────────────────────────┘

Problem: Different execution order in SceneLevelLogic

GameEngine:
  ├─ EventHandler logic BEFORE scene logic
  ├─ SceneManager updates
  └─ Scene sAction/sCollision/sRender

TestEngine (via SimulationRunner):
  ├─ ExecuteInputEventsForTick() FIRST
  ├─ ExecuteEventsForTick() SECOND
  ├─ ProcessWaitingRoom THIRD
  ├─ UpdateSubscribers FOURTH
  └─ ExecuteSimulation() LAST

Consequence: Tests might pass while game fails (or vice versa)

Proposal: UNIFIED_TICK_ARCHITECTURE.md addresses this
Status: Proposal stage, deferred (large effort, system works)
```

---

## Summary

These diagrams show **how the system works today**, including:

- ✅ What's implemented correctly (providers)
- ⚠️ What's in a mixed state (entity config, styles)
- ❌ What's intentionally not done (deferred items)

**Key Takeaway**: System is functional and well-designed, with intentional
compromises documented.

---

**See Also**:
- [Current State Analysis](analysis/CURRENT_STATE_ANALYSIS_2025.md) - Full details
- [Quick Action Plan](QUICK_ACTION_PLAN.md) - What to do next
- [Architecture Current State](architecture/ARCHITECTURE_CURRENT_STATE.md) - Reference doc
- [Active Proposals](proposals/ACTIVE.md) - What's being considered

---

Generated: December 5, 2025  
Status: Current as of this date
