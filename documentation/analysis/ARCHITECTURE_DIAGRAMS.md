# SteamRot Architecture Diagrams

**Date**: December 3, 2025  
**Purpose**: Visual representation of current codebase architecture

---

## Table of Contents

1. [Engine Class Hierarchy](#engine-class-hierarchy)
2. [Context System](#context-system)
3. [Logic System Flow](#logic-system-flow)
4. [Test Infrastructure](#test-infrastructure)
5. [Game Loop vs Test Loop](#game-loop-vs-test-loop)
6. [Component-Entity-Archetype System](#component-entity-archetype-system)
7. [Data Flow](#data-flow)

---

## Engine Class Hierarchy

```
┌─────────────────────────────────────────────────────────────────┐
│                       Engine (Abstract Base)                     │
│                                                                   │
│  Protected Members:                                               │
│  ├─ GameCore m_game_core                                         │
│  ├─ GameContext m_game_context                                   │
│  ├─ SceneManager m_scene_manager                                 │
│  ├─ UserPreferences m_user_preferences                           │
│  ├─ std::vector<Subscriber> m_subscriptions                      │
│  └─ bool m_running                                               │
│                                                                   │
│  Virtual Methods:                                                 │
│  ├─ ConfigureEngineStateFromData() = 0                          │
│  ├─ ExecuteSceneLevelLogic() = 0                                │
│  ├─ ExecuteDisplayManagerTick() = 0                             │
│  ├─ RunGameLoop() = 0                                            │
│  ├─ ProcessSubscriptions() = 0                                  │
│  └─ StartUp() [virtual, has base implementation]                │
│                                                                   │
│  Concrete Methods:                                                │
│  ├─ RunGame() - calls StartUp() then RunGameLoop()              │
│  ├─ ExecuteSystemsTick() - common tick logic                    │
│  └─ ExecuteEngineLevelLogic() - engine-level processing         │
└─────────────────────────────────────────────────────────────────┘
                    │                            │
                    │                            │
        ┌───────────▼─────────┐      ┌──────────▼───────────┐
        │    GameEngine       │      │    TestEngine        │
        │                     │      │                      │
        │  Additional:        │      │  Additional:         │
        │  - DisplayManager   │      │  - TestDataConfig*   │
        │                     │      │  - SimulationData*   │
        │  Implements:        │      │  - TickLevel         │
        │  - Loads from files │      │  - DataBank (map)    │
        │  - SFML game loop   │      │  - target_ticks      │
        │  - Real rendering   │      │  - current_tick      │
        │  - User input       │      │                      │
        │                     │      │  Implements:         │
        │  StartUp():         │      │  - Loads from config │
        │  1. Base::StartUp() │      │  - Simulation loop   │
        │  2. Load saved      │      │  - Tick snapshots    │
        │     preferences     │      │  - No rendering      │
        │  3. LoadTitleScene()│      │  - Simulated input   │
        └─────────────────────┘      └──────────────────────┘
                    │                            │
                    │                            │
                    ▼                            ▼
            Production Game                Test Execution
            (Real gameplay)             (Validation & Testing)
```

---

## Context System

```
┌────────────────────────────────────────────────────────────────────┐
│                           GameCore                                 │
│                                                                     │
│  Owns (unique_ptr):                                                │
│  ├─ sf::RenderWindow window                                        │
│  ├─ EventHandler event_handler                                     │
│  ├─ AssetManager asset_manager                                     │
│  ├─ sf::Vector2i mouse_position                                    │
│  └─ size_t loop_number                                             │
│                                                                     │
│  Created during: Engine::StartUp()                                 │
│  Lifetime: Entire game/test run                                    │
└────────────────────────────────────────────────────────────────────┘
                            │
                            │ Creates (by reference)
                            ▼
        ┌───────────────────────────────────────┐
        │          GameContext                  │
        │      (Lightweight wrapper)            │
        │                                       │
        │  References:                          │
        │  ├─ GameCore& game_core               │
        │  ├─ sf::RenderWindow& game_window     │
        │  ├─ EventHandler& event_handler       │
        │  ├─ sf::Vector2i& mouse_position      │
        │  ├─ size_t& loop_number               │
        │  └─ AssetManager& asset_manager       │
        │                                       │
        │  Usage: Passed to DisplayManager      │
        └───────────────────────────────────────┘
                            │
                            │ Combined with
                            │
        ┌───────────────────┴──────────────────────┐
        │                                          │
        ▼                                          ▼
┌───────────────────┐                  ┌────────────────────┐
│    SceneCore      │                  │  EntityManager     │
│                   │                  │                    │
│  Owns:            │                  │  Owns:             │
│  - RenderTexture  │                  │  - EntityMemoryPool│
│                   │                  │  - ArchetypeManager│
│  Per-scene object │                  │                    │
└───────────────────┘                  └────────────────────┘
        │                                          │
        └───────────────────┬──────────────────────┘
                            │ Creates (by reference)
                            ▼
        ┌───────────────────────────────────────────────┐
        │              SceneContext                     │
        │          (Lightweight wrapper)                │
        │                                               │
        │  References:                                  │
        │  ├─ EntityMemoryPool& scene_entities          │
        │  ├─ unordered_map<ArchetypeID, Archetype>&    │
        │  │  archetypes                                │
        │  ├─ sf::RenderTexture& scene_texture          │
        │  ├─ sf::RenderWindow& game_window             │
        │  ├─ AssetManager& asset_manager               │
        │  ├─ EventHandler& event_handler               │
        │  └─ sf::Vector2i& mouse_position              │
        │                                               │
        │  Usage: Passed to Logic classes               │
        └───────────────────────────────────────────────┘
                            │
                            │ Passed to
                            ▼
                ┌───────────────────────┐
                │   Logic Classes       │
                │                       │
                │  - UIActionLogic      │
                │  - UICollisionLogic   │
                │  - UIRenderLogic      │
                │  - etc.               │
                └───────────────────────┘
```

**Key Principles**:
1. **Core objects** own the actual data (window, assets, entities)
2. **Context objects** provide lightweight reference access
3. **Contexts are cheap to copy** (only references)
4. **Clear ownership hierarchy** prevents dangling references

---

## Logic System Flow

```
┌──────────────────────────────────────────────────────────────────┐
│                        SceneManager                              │
│                                                                   │
│  For each Scene:                                                  │
│  ├─ EntityManager (entities + archetypes)                        │
│  ├─ SceneCore (render texture)                                   │
│  └─ LogicCollection (created by LogicFactory)                    │
└──────────────────────────────────────────────────────────────────┘
                            │
                            │ Creates Logic for each scene
                            ▼
        ┌───────────────────────────────────────┐
        │         LogicFactory                  │
        │                                       │
        │  CreateLogicMap(SceneType)            │
        │  ├─ CreateActionLogics()              │
        │  ├─ CreateCollisionLogics()           │
        │  ├─ CreateRenderLogics()              │
        │  └─ CreateMovementLogics()            │
        │                                       │
        │  Returns: LogicCollection             │
        └───────────────────────────────────────┘
                            │
                            │ Creates
                            ▼
┌───────────────────────────────────────────────────────────────────┐
│                      LogicCollection                              │
│   std::unordered_map<LogicType, std::vector<unique_ptr<Logic>>>  │
│                                                                    │
│   LogicType::Action                                               │
│   └─▶ [UIActionLogic, ...]                                        │
│                                                                    │
│   LogicType::Collision                                            │
│   └─▶ [UICollisionLogic, ...]                                     │
│                                                                    │
│   LogicType::Render                                               │
│   └─▶ [UIRenderLogic, CraftingRenderLogic, ...]                  │
│                                                                    │
│   LogicType::Movement                                             │
│   └─▶ [PlayerMovementLogic, ...]                                  │
└───────────────────────────────────────────────────────────────────┘
                            │
                            │ Each vector contains
                            ▼
        ┌───────────────────────────────────────┐
        │      Logic (Abstract Base)            │
        │                                       │
        │  Protected:                           │
        │  ├─ ProcessLogic() = 0 [pure virtual]│
        │  └─ SceneContext m_scene_context      │
        │                                       │
        │  Public:                              │
        │  ├─ RunLogic() [calls ProcessLogic()] │
        │  └─ m_subscribers vector              │
        └───────────────────────────────────────┘
                            │
        ┌───────────────────┼───────────────────┐
        │                   │                   │
        ▼                   ▼                   ▼
┌──────────────┐  ┌─────────────────┐  ┌──────────────────┐
│UIActionLogic │  │UICollisionLogic │  │ UIRenderLogic    │
│              │  │                 │  │                  │
│ProcessLogic():│  │ProcessLogic(): │  │ProcessLogic():   │
│  Calls:      │  │  Calls:         │  │  Calls:          │
│  logic::     │  │  logic::        │  │  draw_ui_        │
│  action::    │  │  collision::    │  │  elements::      │
│  Process...  │  │  Process...     │  │  Draw...         │
└──────────────┘  └─────────────────┘  └──────────────────┘
        │                   │                   │
        └───────────────────┼───────────────────┘
                            │
                            ▼
        ┌───────────────────────────────────────┐
        │       Free Function Modules           │
        │                                       │
        │  namespace steamrot::logic::action {  │
        │    ProcessUIActionsAndEvents(...)     │
        │    ProcessNestedUIActionsAndEvents()  │
        │    ProcessButtonElementActions()      │
        │    ProcessDropDownListElementActions()│
        │  }                                    │
        │                                       │
        │  namespace steamrot::logic::collision │
        │    ProcessUICollisions(...)           │
        │    CheckUICollision(...)              │
        │  }                                    │
        │                                       │
        │  namespace draw_ui_elements {         │
        │    DrawUserInterfaceElements(...)     │
        │    DrawUIElement(...)                 │
        │  }                                    │
        └───────────────────────────────────────┘
```

**Execution Flow**:
1. SceneManager calls UpdateScenes()
2. For each scene: sAction() → sCollision() → sRender()
3. Each s*() method iterates LogicCollection for that LogicType
4. Each Logic::RunLogic() calls ProcessLogic()
5. ProcessLogic() may call free functions or implement directly

---

## Test Infrastructure

```
┌─────────────────────────────────────────────────────────────────┐
│                    Test Infrastructure                          │
└─────────────────────────────────────────────────────────────────┘
                            │
        ┌───────────────────┼───────────────────┐
        │                   │                   │
        ▼                   ▼                   ▼
┌──────────────┐  ┌──────────────────┐  ┌─────────────────┐
│ TestFixture  │  │   TestEngine     │  │  Test Matchers  │
│              │  │                  │  │                 │
│ Purpose:     │  │ Purpose:         │  │ Purpose:        │
│ - Unit tests │  │ - Integration    │  │ - Component     │
│ - Mock       │  │ - Simulation     │  │   comparison    │
│   contexts   │  │ - Data-driven    │  │ - Entity pool   │
│              │  │                  │  │   comparison    │
└──────────────┘  └──────────────────┘  └─────────────────┘
        │                   │                   │
        │                   │                   │
        ▼                   ▼                   ▼
┌──────────────┐  ┌──────────────────┐  ┌─────────────────┐
│Creates:      │  │Extends:          │  │Provides:        │
│- GameCore    │  │- Engine base     │  │- EntityMemory   │
│- SceneCore   │  │                  │  │  PoolEquals     │
│- EntityMgr   │  │Has:              │  │- Component      │
│              │  │- TestDataConfig  │  │  Equals         │
│Returns:      │  │- SimulationData  │  │                 │
│- GameContext │  │- DataBank        │  │Used in:         │
│- SceneContext│  │  (snapshots)     │  │- Assertions     │
│              │  │                  │  │- Validations    │
│              │  │Supports:         │  │                 │
│Used by:      │  │- Tick execution  │  │                 │
│- All unit    │  │- Input sim       │  │                 │
│  tests       │  │- Event sim       │  │                 │
└──────────────┘  │- Tick snapshots  │  └─────────────────┘
                  └──────────────────┘
```

### TestFixture Usage Pattern

```cpp
TEST_CASE("Test something", "[unit]") {
  // Setup
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  
  // Get context
  auto& scene_context = fixture.GetSceneContext();
  auto& game_context = fixture.GetGameContext();
  
  // Create object under test
  MyClass obj(scene_context);
  
  // Act
  obj.DoSomething();
  
  // Assert
  REQUIRE(expected_result);
}
```

### TestEngine Usage Pattern

```cpp
TEST_CASE("Data-driven test", "[integration]") {
  // Load test data
  auto configs = load_test_data_configs();
  const auto* config = configs[0];
  
  // Create and run test engine
  TestEngine engine(config);
  engine.RunGame();
  
  // Get results
  auto& data_bank = engine.GetDataBank();
  
  // Compare against expected
  auto expected = config->expected_entity_collection();
  REQUIRE(data_bank.at(final_tick) == expected);
}
```

---

## Game Loop vs Test Loop

### GameEngine Loop

```
┌─────────────────────────────────────────────────┐
│         GameEngine::RunGameLoop()               │
│                                                 │
│  while (window.isOpen()) {                      │
│    ┌─────────────────────────────────────┐    │
│    │  UpdateSystems()                     │    │
│    │  ├─ UpdateGameResources()            │    │
│    │  │  └─ Update mouse_position         │    │
│    │  │                                   │    │
│    │  ├─ EventHandler::PreloadEvents()    │    │
│    │  │  └─ Poll SFML events              │    │
│    │  │                                   │    │
│    │  ├─ EventHandler::ProcessWaitingRoom()│   │
│    │  │  └─ Move events to main bus       │    │
│    │  │                                   │    │
│    │  ├─ EventHandler::UpdateSubscribers()│    │
│    │  │  └─ Notify all subscribers        │    │
│    │  │                                   │    │
│    │  ├─ GameEngine::ProcessSubscriptions()│   │
│    │  │  └─ Execute engine-level logic    │    │
│    │  │                                   │    │
│    │  ├─ SceneManager::UpdateSceneManager()│   │
│    │  │  ├─ ProcessSubscriptions()        │    │
│    │  │  └─ UpdateScenes()                │    │
│    │  │     ├─ sAction()                  │    │
│    │  │     ├─ sCollision()               │    │
│    │  │     └─ sRender()                  │    │
│    │  │                                   │    │
│    │  ├─ DisplayManager::CallRenderCycle()│    │
│    │  │  └─ Render to window              │    │
│    │  │                                   │    │
│    │  └─ EventHandler::TickGlobalEventBus()│   │
│    │     └─ Clear processed events        │    │
│    └─────────────────────────────────────┘    │
│    loop_number++                              │
│  }                                            │
└─────────────────────────────────────────────────┘
```

### TestEngine Loop

```
┌─────────────────────────────────────────────────┐
│         TestEngine::RunGameLoop()               │
│                                                 │
│  for (tick = 1; tick <= target_ticks; tick++) {│
│    ┌─────────────────────────────────────┐    │
│    │  ExecuteInputEventsForTick()        │    │
│    │  └─ Inject simulated input          │    │
│    │                                     │    │
│    │  ExecuteEventsForTick()             │    │
│    │  └─ Inject simulated events         │    │
│    │                                     │    │
│    │  ProcessWaitingRoom()               │    │
│    │  └─ Move events to main bus         │    │
│    │                                     │    │
│    │  UpdateSubscribers()                │    │
│    │  └─ Notify all subscribers          │    │
│    │                                     │    │
│    │  ExecuteSceneLevelLogic()           │    │
│    │  └─ Based on TickLevel:             │    │
│    │     ├─ FullEngine: UpdateSceneManager│   │
│    │     ├─ SceneManager: UpdateScenes()  │   │
│    │     ├─ SceneLogic: sAction/Collision/│   │
│    │     │   Render                       │    │
│    │     └─ Custom: simulation_data       │    │
│    │                                     │    │
│    │  AddToDataBank(tick)                │    │
│    │  └─ Snapshot current state          │    │
│    │                                     │    │
│    │  TickGlobalEventBus()               │    │
│    │  └─ Clear processed events          │    │
│    └─────────────────────────────────────┘    │
│    current_tick++                             │
│  }                                            │
└─────────────────────────────────────────────────┘
```

### Key Differences

| Aspect | GameEngine | TestEngine |
|--------|-----------|------------|
| Loop Control | while (window.isOpen()) | for (tick = 1; tick <= target_ticks) |
| Input | SFML events | Simulated input |
| Rendering | DisplayManager renders | No rendering |
| Logic Execution | Fixed (UpdateScenes) | Configurable (TickLevel) |
| Snapshots | None | DataBank per tick |
| Purpose | Gameplay | Testing/Validation |

---

## Component-Entity-Archetype System

```
┌──────────────────────────────────────────────────────────────┐
│                   Component System                           │
│                                                               │
│  Components are pure data containers:                         │
│  - No methods (except GetComponentRegisterIndex())           │
│  - Inherit from Component base                               │
│  - Default constructible                                     │
│  - m_ prefix for member variables                            │
└──────────────────────────────────────────────────────────────┘
                            │
                            │ Stored in
                            ▼
┌──────────────────────────────────────────────────────────────┐
│               EntityMemoryPool                               │
│                                                               │
│  std::array of component tuples:                             │
│  [0]: std::tuple<CMeta, CUserInterface, CGrimoire, ...>      │
│  [1]: std::tuple<CMeta, CUserInterface, CGrimoire, ...>      │
│  [2]: std::tuple<CMeta, CUserInterface, CGrimoire, ...>      │
│  ...                                                          │
│  [N]: std::tuple<CMeta, CUserInterface, CGrimoire, ...>      │
│                                                               │
│  Each entity has all components, but only some are active    │
└──────────────────────────────────────────────────────────────┘
                            │
                            │ Indexed by
                            ▼
┌──────────────────────────────────────────────────────────────┐
│              ArchetypeManager                                │
│                                                               │
│  Groups entities by active component combinations:           │
│                                                               │
│  ArchetypeID (bitset): 0b0000...0101                         │
│  Archetype (vector):   [0, 5, 12, 23]  ← entity indices     │
│                                                               │
│  Example archetypes:                                         │
│  ├─ CMeta + CUserInterface                                   │
│  │  └─ Entities: [0, 1, 2] (UI entities)                    │
│  │                                                           │
│  ├─ CMeta + CGrimoire                                        │
│  │  └─ Entities: [10, 11] (grimoire entities)              │
│  │                                                           │
│  └─ CMeta + CUserInterface + CGrimoire                       │
│     └─ Entities: [20] (UI with grimoire data)               │
└──────────────────────────────────────────────────────────────┘
                            │
                            │ Used by
                            ▼
┌──────────────────────────────────────────────────────────────┐
│                  Logic Classes                               │
│                                                               │
│  Logic iterates by archetype:                                │
│                                                               │
│  void MyLogic::ProcessLogic() {                              │
│    // Generate archetype for needed components              │
│    auto archetype_id =                                       │
│      GenerateArchetypeIDfromTypes<CUserInterface>();        │
│                                                               │
│    // Find entities with this archetype                      │
│    auto it = m_scene_context.archetypes.find(archetype_id); │
│    if (it != archetypes.end()) {                            │
│      const Archetype& archetype = it->second;               │
│                                                               │
│      // Process each entity                                  │
│      for (size_t entity_id : archetype) {                   │
│        auto& component = GetComponent<CUserInterface>(      │
│          entity_id, m_scene_context.scene_entities);        │
│        // ... process component ...                          │
│      }                                                        │
│    }                                                          │
│  }                                                            │
└──────────────────────────────────────────────────────────────┘
```

### Advantages of This System

1. **Cache Friendly**: Components stored contiguously
2. **Fast Iteration**: Logic only processes entities with required components
3. **Type Safe**: Compile-time component checks
4. **Flexible**: Easy to add/remove components from entities
5. **Efficient**: No dynamic allocation during gameplay

---

## Data Flow

### Startup Data Flow

```
┌─────────────┐
│ main.cpp    │
└──────┬──────┘
       │
       ▼
┌─────────────────────────────────────────┐
│ Engine::StartUp()                       │
│ ├─ Load engine_data.json                │
│ │  └─ EngineCoreData (window config)    │
│ ├─ Load default.preferences.bin         │
│ │  └─ UserPreferences                   │
│ └─ ConfigureEngineStateFromData()       │
│    └─ [virtual - implemented by derived]│
└─────────────────────────────────────────┘
       │
       ├─────────────────────┬─────────────────────┐
       ▼                     ▼                     ▼
┌──────────────────┐  ┌──────────────────┐  ┌──────────────────┐
│ GameEngine       │  │ TestEngine       │  │ Common           │
│ StartUp()        │  │ StartUp()        │  │ Configuration    │
│                  │  │                  │  │                  │
│ ├─ Base::StartUp │  │ ├─ Base::StartUp │  │ ├─ Create        │
│ ├─ Load saved    │  │ │   only          │  │ │  GameCore      │
│ │  preferences   │  │ │                 │  │ ├─ Create        │
│ └─ LoadTitleScene│  │ └─ Use test_data │  │ │  GameContext   │
│                  │  │    config         │  │ └─ Configure     │
│ Loads from:      │  │                  │  │    subscribers   │
│ - engine_data.   │  │ Loads from:      │  │                  │
│   json           │  │ - test_data.json │  │                  │
│ - scene_data/    │  │ - test_data.bin  │  │                  │
└──────────────────┘  └──────────────────┘  └──────────────────┘
```

### Runtime Data Flow

```
┌──────────────┐
│ User Input   │ ──┐
└──────────────┘   │
                   │
┌──────────────┐   │
│ SFML Events  │ ──┤
└──────────────┘   │
                   ├──▶ ┌─────────────────┐
┌──────────────┐   │    │ EventHandler    │
│ Game Events  │ ──┤    │                 │
└──────────────┘   │    │ ├─ Waiting Room │
                   │    │ ├─ Global Bus   │
┌──────────────┐   │    │ └─ Subscribers  │
│ Logic Events │ ──┘    └─────────────────┘
└──────────────┘             │
                             │
                    ┌────────┴────────┐
                    │                 │
                    ▼                 ▼
         ┌──────────────────┐  ┌──────────────────┐
         │ Engine-level     │  │ Scene-level      │
         │ Subscriptions    │  │ Subscriptions    │
         └──────────────────┘  └──────────────────┘
                    │                 │
                    └────────┬────────┘
                             │
                             ▼
                    ┌─────────────────┐
                    │ Logic Classes   │
                    │                 │
                    │ Process by      │
                    │ archetype       │
                    └─────────────────┘
                             │
                             ▼
                    ┌─────────────────┐
                    │ Update          │
                    │ Components      │
                    └─────────────────┘
                             │
                             ▼
                    ┌─────────────────┐
                    │ Render          │
                    │ (DisplayManager)│
                    └─────────────────┘
                             │
                             ▼
                    ┌─────────────────┐
                    │ Display         │
                    │ (sf::Window)    │
                    └─────────────────┘
```

### Test Data Flow

```
┌──────────────────┐
│ test_data.json   │
│                  │
│ ├─ metadata      │
│ ├─ start_data    │
│ ├─ simulation    │
│ └─ expected_data │
└──────────────────┘
         │
         │ Compiled to
         ▼
┌──────────────────┐
│ test_data.bin    │
│ (FlatBuffers)    │
└──────────────────┘
         │
         │ Loaded by
         ▼
┌──────────────────┐
│ Test Harness     │
│                  │
│ load_test_data_  │
│ configs()        │
└──────────────────┘
         │
         │ Returns
         ▼
┌──────────────────┐
│ TestDataConfig*  │
└──────────────────┘
         │
         │ Passed to
         ▼
┌──────────────────┐
│ TestEngine       │
│                  │
│ Constructor      │
└──────────────────┘
         │
         │ RunGame()
         ▼
┌──────────────────┐
│ Execute ticks    │
│ Take snapshots   │
│ Store in DataBank│
└──────────────────┘
         │
         │ Compare
         ▼
┌──────────────────┐
│ expected_data    │
│ vs               │
│ DataBank         │
└──────────────────┘
```

---

## Summary

These diagrams illustrate the current architecture of SteamRot:

1. **Engine Hierarchy**: Clear separation between game and test execution
2. **Context System**: Lightweight reference wrappers with clear ownership
3. **Logic System**: Flexible architecture with free functions and classes
4. **Test Infrastructure**: Dual approach for unit and integration testing
5. **Game/Test Loops**: Similar structure with key differences for testing
6. **ECS System**: Efficient archetype-based entity processing
7. **Data Flow**: Clear paths from input to rendering/validation

The architecture is generally well-designed with:
- ✓ Clear separation of concerns
- ✓ Efficient entity-component system
- ✓ Flexible testing infrastructure
- ✓ Good use of polymorphism and composition

Areas for improvement (see main analysis):
- Update documentation to match current implementation
- Add missing unit tests
- Improve namespace consistency
- Enhance test infrastructure reusability
