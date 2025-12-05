# Data Loading Interface System - Architecture Diagrams

## Current Architecture (Before)

```
┌─────────────────────────────────────────────────────────────┐
│                      Game Code Layer                         │
│  ┌──────────┐  ┌────────────┐  ┌──────────────┐            │
│  │ Engine   │  │SceneFactory│  │ AssetManager │            │
│  └────┬─────┘  └──────┬─────┘  └──────┬───────┘            │
│       │                │                │                     │
└───────┼────────────────┼────────────────┼─────────────────────┘
        │                │                │
        │ Uses FlatBuffers types directly!
        │                │                │
        └────────┬───────┴────────────────┘
                 ↓
    ┌───────────────────────────────────┐
    │   FlatbuffersDataLoader           │
    │   (Concrete Class)                │
    ├───────────────────────────────────┤
    │ ProvideEngineData()               │
    │   → returns const EngineData*     │
    │      (FlatBuffers type!)          │
    │                                   │
    │ ProvideSceneData()                │
    │   → returns const SceneDataData*  │
    │      (FlatBuffers type!)          │
    │                                   │
    │ ProvideAssetData()                │
    │   → returns const AssetCollection*│
    │      (FlatBuffers type!)          │
    └────────────┬──────────────────────┘
                 │
                 ↓
    ┌───────────────────────────────────┐
    │   FlatBuffers Binary Files        │
    │   (.bin files)                    │
    └───────────────────────────────────┘

Problems:
❌ Game code depends on FlatBuffers types
❌ Can't swap data format
❌ Tight coupling to serialization
❌ Hard to mock in tests
```

## Proposed Architecture (After)

```
┌─────────────────────────────────────────────────────────────┐
│                      Game Code Layer                         │
│  ┌──────────┐  ┌────────────┐  ┌──────────────┐            │
│  │ Engine   │  │SceneFactory│  │ AssetManager │            │
│  └────┬─────┘  └──────┬─────┘  └──────┬───────┘            │
└───────┼────────────────┼────────────────┼─────────────────────┘
        │                │                │
        │ Uses native C++ structs only!
        │                │                │
        ↓                ↓                ↓
┌───────────────────────────────────────────────────────────────┐
│                  Interface Layer (Abstract)                    │
│                                                               │
│  ┌──────────────────────┐  ┌──────────────────────┐         │
│  │ IEngineDataProvider  │  │ ISceneDataProvider   │         │
│  ├──────────────────────┤  ├──────────────────────┤         │
│  │ LoadEngineCoreData() │  │ LoadSceneData()      │         │
│  │   → EngineCoreData   │  │   → SceneData        │         │
│  │   (native struct!)   │  │   (native struct!)   │         │
│  └──────────────────────┘  └──────────────────────┘         │
│                                                               │
│  ┌──────────────────────┐                                    │
│  │ IAssetDataProvider   │                                    │
│  ├──────────────────────┤                                    │
│  │ LoadAssetData()      │                                    │
│  │   → AssetData        │                                    │
│  │   (native struct!)   │                                    │
│  └──────────────────────┘                                    │
└───────────────────┬───────────────┬───────────────┬───────────┘
                    │               │               │
         ┌──────────┴──────┐  ┌────┴────┐  ┌──────┴──────┐
         ↓                 ↓  ↓         ↓  ↓             ↓
┌────────────────────────────────────────────────────────────────┐
│              Implementation Layer (Concrete)                    │
│                                                                │
│  ┌──────────────────────────────┐  ┌──────────────────────┐  │
│  │ FlatbuffersEngineDataProvider│  │ JsonEngineDataProvider│ │
│  ├──────────────────────────────┤  ├──────────────────────┤  │
│  │ LoadEngineCoreData()         │  │ LoadEngineCoreData() │  │
│  │   1. Load .bin file          │  │   1. Load .json file │  │
│  │   2. Parse FlatBuffers       │  │   2. Parse JSON      │  │
│  │   3. Convert to native struct│  │   3. Convert to      │  │
│  │   4. Return EngineCoreData   │  │      native struct   │  │
│  └──────────────────────────────┘  └──────────────────────┘  │
│                                                                │
│  ┌──────────────────────────────┐  ┌──────────────────────┐  │
│  │ FlatbuffersSceneDataProvider │  │ LuaSceneDataProvider │  │
│  │ FlatbuffersAssetDataProvider │  │ XmlAssetDataProvider │  │
│  │ etc...                        │  │ etc...               │  │
│  └──────────────────────────────┘  └──────────────────────┘  │
└────────────────────┬────────────────────┬───────────────┬─────┘
                     │                    │               │
                     ↓                    ↓               ↓
         ┌──────────────────┐  ┌──────────────┐  ┌──────────────┐
         │ FlatBuffers .bin │  │ JSON .json   │  │ Lua .lua     │
         │ files            │  │ files        │  │ files        │
         └──────────────────┘  └──────────────┘  └──────────────┘

Benefits:
✅ Game code uses native C++ structs
✅ Easy to swap formats
✅ Clean separation of concerns
✅ Easy to mock in tests
✅ Can add new formats without touching game code
```

## Data Flow: Loading Engine Data

### Current Flow
```
Engine::StartUp()
    │
    │ Creates concrete loader
    ↓
┌───────────────────────────┐
│ FlatbuffersDataLoader     │
│   loader;                 │
└───────────┬───────────────┘
            │
            │ loader.ProvideEngineCoreData()
            ↓
┌───────────────────────────┐
│ Returns:                  │
│ const EngineCoreData*     │
│   (FlatBuffers pointer!)  │
└───────────┬───────────────┘
            │
            │ Access via FlatBuffers API
            ↓
        fb_data->window_width()
        fb_data->window_title()->str()
            │
            ↓
┌───────────────────────────┐
│ core::ConfigureGameCore() │
│ (takes FlatBuffers ptr)   │
└───────────────────────────┘
```

### Proposed Flow
```
Engine::StartUp()
    │
    │ Gets interface reference
    ↓
┌───────────────────────────┐
│ IEngineDataProvider&      │
│   provider =              │
│   GetEngineDataProvider() │
└───────────┬───────────────┘
            │
            │ provider.LoadEngineCoreData()
            ↓
┌───────────────────────────┐
│ Returns:                  │
│ EngineCoreData            │
│   (Native struct!)        │
└───────────┬───────────────┘
            │
            │ Direct member access
            ↓
        data.window_width
        data.window_title
            │
            ↓
┌───────────────────────────┐
│ core::ConfigureGameCore() │
│ (takes native struct ref) │
└───────────────────────────┘
```

## Provider Implementation Pattern

```
┌─────────────────────────────────────────────────────────┐
│                  IMyDataProvider                        │
│                  (Abstract Interface)                    │
│                                                         │
│  + virtual LoadData() : expected<MyData, FailInfo>     │
└─────────────────────┬───────────────────────────────────┘
                      △
                      │ implements
          ┌───────────┴──────────────┐
          │                          │
┌─────────────────────┐   ┌──────────────────────┐
│ Flatbuffers         │   │ Json                 │
│ MyDataProvider      │   │ MyDataProvider       │
├─────────────────────┤   ├──────────────────────┤
│ LoadData():         │   │ LoadData():          │
│   1. Load .bin      │   │   1. Load .json      │
│   2. Parse FB       │   │   2. Parse JSON      │
│   3. Convert        │   │   3. Convert         │
│   4. Return struct  │   │   4. Return struct   │
└─────────────────────┘   └──────────────────────┘
```

## Type Conversion Flow

```
┌──────────────────────┐
│ FlatBuffers Type     │
│ EngineCoreDataFbs    │
└──────────┬───────────┘
           │
           │ Provider converts
           ↓
┌──────────────────────────────────────────┐
│ struct EngineCoreData {                  │
│   uint32_t window_width;                 │
│   uint32_t window_height;                │
│   string window_title;                   │
│   uint32_t framerate_limit;              │
│   string environment_type;               │
│ };                                       │
└──────────┬───────────────────────────────┘
           │
           │ Used by game code
           ↓
┌──────────────────────┐
│ Game Logic           │
│ - Engine             │
│ - SceneFactory       │
│ - Systems            │
└──────────────────────┘
```

## Configurator vs Provider Pattern

```
┌──────────────────────────────────────────────────────────────┐
│                    Load + Configure Flow                      │
└──────────────────────────────────────────────────────────────┘

Step 1: LOAD DATA
   │
   ↓
┌───────────────────────┐
│  IDataProvider        │
│  (Interface)          │
│                       │
│  LoadData()           │
│  → returns native     │
│     struct with data  │
└───────────┬───────────┘
            │
            │ Data only (no objects created yet)
            ↓
      ┌─────────────┐
      │  MyData     │
      │  struct     │
      └──────┬──────┘
             │
             │ Pass to configurator
             ↓
Step 2: CONFIGURE OBJECTS
   │
   ↓
┌───────────────────────┐
│  Configurator         │
│  (Class or Function)  │
│                       │
│  Configure(data)      │
│  → creates/modifies   │
│     runtime objects   │
└───────────┬───────────┘
            │
            │ Objects configured
            ↓
      ┌──────────┐
      │ Runtime  │
      │ Objects  │
      └──────────┘

Examples:

Provider:
  IEngineDataProvider::LoadEngineCoreData()
  → Returns EngineCoreData struct

Configurator:
  core::ConfigureGameCore(GameCore&, EngineCoreData)
  → Uses data to configure window, etc.
```

## Naming Hierarchy

```
Domain: "Engine"

┌─────────────────────────────────────────────┐
│ Game Struct (Simplest)                      │
│ EngineData                                  │
│ EngineCoreData                              │
│                                             │
│ Used most frequently by game code           │
└─────────────────────────────────────────────┘
                    △
                    │ provides
                    │
┌─────────────────────────────────────────────┐
│ Interface (Format-Agnostic)                 │
│ IEngineDataProvider                         │
│                                             │
│ Abstract contract for loading               │
└─────────────────────────────────────────────┘
                    △
                    │ implements
        ┌───────────┴──────────┐
        │                      │
┌───────────────────┐  ┌──────────────────┐
│ Implementation    │  │ Implementation   │
│ FlatbuffersEngine │  │ JsonEngine       │
│ DataProvider      │  │ DataProvider     │
│                   │  │                  │
│ Format-specific   │  │ Format-specific  │
└───────────────────┘  └──────────────────┘
        │                      │
        ↓                      ↓
┌───────────────────┐  ┌──────────────────┐
│ FlatBuffers Type  │  │ JSON parsing     │
│ EngineCoreDataFbs │  │ nlohmann::json   │
│                   │  │                  │
│ (internal only)   │  │ (internal only)  │
└───────────────────┘  └──────────────────┘
```

## Migration Timeline Visualization

```
Current State (T=0)
├── FlatbuffersDataLoader everywhere
├── Game code uses FlatBuffers types
└── Tight coupling

Phase 1: Create Interfaces (T=1-2 weeks)
├── Create native structs
├── Create provider interfaces
├── Implement FlatBuffers providers (delegate to old loader)
├── Write unit tests
└── NO changes to existing code yet

Phase 2: Migrate Integration Points (T=3-5 weeks)
├── Update Engine::StartUp()
├── Update SceneFactory
├── Update AssetManager
├── Update free function signatures
└── Both old and new code coexist

Phase 3: Cleanup (T=6 weeks)
├── Deprecate FlatbuffersDataLoader
├── Remove old loader
├── Update documentation
└── Only new system remains

Phase 4: Entity System (T=Future, separate project)
├── Create native component data structs
├── Abstract entity configurators
└── Complex, high risk - defer
```

## Testing Strategy

```
┌──────────────────────────────────────────────────────┐
│                   Test Pyramid                        │
└──────────────────────────────────────────────────────┘

      ┌──────────────────┐
      │  Integration     │  ← Engine loads via providers
      │  Tests           │    and configures correctly
      └────────┬─────────┘
               │
        ┌──────┴──────────────┐
        │  Unit Tests          │  ← Each provider works
        │  - Provider loads    │    correctly in isolation
        │  - Converts types    │
        │  - Returns structs   │
        └──────┬──────────────┘
               │
    ┌──────────┴─────────────────┐
    │  Mock Tests                 │  ← Game code can use
    │  - Mock providers           │    mock providers for
    │  - Return test data         │    isolated testing
    │  - Test game logic alone    │
    └─────────────────────────────┘

Mocking Example:
┌────────────────────────────────┐
│ class MockEngineDataProvider   │
│   : public IEngineDataProvider │
│ {                              │
│   LoadEngineCoreData() {       │
│     return test_data;          │
│   }                            │
│ };                             │
└────────────────────────────────┘
        │
        │ Inject into test
        ↓
┌────────────────────────────────┐
│ Engine engine(mock_provider);  │
│ // Test engine logic without   │
│ // loading real files!         │
└────────────────────────────────┘
```

## Comparison with Existing Good Examples

```
┌─────────────────────────────────────────────────────────────┐
│         ISaveDataProvider (Already Implemented!)            │
│                     ✅ Perfect Example                       │
└─────────────────────────────────────────────────────────────┘

1. Native Struct:
   struct SaveData {
     struct Metadata { ... } metadata;
     SceneType current_scene_type;
   };

2. Interface:
   class ISaveDataProvider {
     virtual expected<SaveData, FailInfo> LoadSave(...) = 0;
   };

3. Implementation:
   class FlatbuffersSaveDataProvider : public ISaveDataProvider {
     expected<SaveData, FailInfo> LoadSave(...) override;
   };

4. Usage:
   ISaveDataProvider& provider = GetSaveProvider();
   SaveData save = provider.LoadSave(slot).value();

This is EXACTLY the pattern we want for all data loading!
```

## Decision Tree Diagram

```
                    Need to work with external data?
                              │
                    ┌─────────┴─────────┐
                    │                   │
                   YES                  NO
                    │                   │
                    ↓                   └─→ Use data directly
          Data format might vary?
                    │
          ┌─────────┴─────────┐
          │                   │
         YES                  NO
          │                   │
          ↓                   └─→ Could use provider anyway
    CREATE PROVIDER                for consistency
          │
          ↓
    ┌─────────────────────────────┐
    │ 1. Define native struct     │
    │ 2. Create interface         │
    │ 3. Implement for format(s)  │
    │ 4. Create factory function  │
    └─────────────────────────────┘


              Need to transform data?
                      │
            ┌─────────┴─────────┐
            │                   │
           YES                  NO
            │                   │
            ↓                   └─→ Use data as-is
    Complex or needs deps?
            │
    ┌───────┴───────┐
    │               │
   YES              NO
    │               │
    ↓               ↓
CLASS          FREE FUNCTION
CONFIGURATOR   CONFIGURATOR
    │               │
    ↓               ↓
┌─────────────┐ ┌────────────┐
│ Has state   │ │ Stateless  │
│ Overloads   │ │ Pure       │
│ Dependencies│ │ Simple     │
└─────────────┘ └────────────┘
```

## Summary: Three Layers

```
┌─────────────────────────────────────────────────────────────┐
│                    LAYER 1: GAME CODE                        │
│  ┌──────────┐  ┌───────────┐  ┌───────────┐                │
│  │ Engine   │  │  Scene    │  │  Logic    │                │
│  │          │  │  Factory  │  │  Systems  │                │
│  └──────────┘  └───────────┘  └───────────┘                │
│                                                             │
│  Uses: Native C++ structs only (EngineCoreData, SceneData) │
│  Knows: Nothing about data formats                          │
└─────────────────────────────────────────────────────────────┘
                             │
                Uses interfaces and native structs
                             │
                             ↓
┌─────────────────────────────────────────────────────────────┐
│              LAYER 2: ABSTRACTION (INTERFACES)               │
│  ┌─────────────────┐  ┌─────────────────┐                  │
│  │ IEngineData     │  │ ISceneData      │                  │
│  │ Provider        │  │ Provider        │                  │
│  └─────────────────┘  └─────────────────┘                  │
│                                                             │
│  Defines: Contracts for loading data                        │
│  Returns: Native C++ structs                                │
│  Knows: Nothing about implementations                       │
└─────────────────────────────────────────────────────────────┘
                             │
                      Implemented by
                             │
                             ↓
┌─────────────────────────────────────────────────────────────┐
│           LAYER 3: IMPLEMENTATION (CONCRETE)                 │
│  ┌──────────────────┐  ┌──────────────┐  ┌──────────────┐ │
│  │  Flatbuffers     │  │    Json      │  │     Lua      │ │
│  │  Providers       │  │  Providers   │  │  Providers   │ │
│  └──────────────────┘  └──────────────┘  └──────────────┘ │
│                                                             │
│  Handles: Format-specific loading and parsing               │
│  Converts: Format types → Native structs                    │
│  Knows: Implementation details only                         │
└─────────────────────────────────────────────────────────────┘

Key Insight: Each layer only knows about the layer above it!
```
