# Interface Library Analysis: Data Structure Separation

## Executive Summary

This analysis identifies data structures that can be moved to interface libraries to prevent circular CMake dependencies. The analysis focuses on structures with minimal external dependencies (only standard library) that can serve as clean interface contracts between subsystems.

**Key Findings:**
- **52 data structures** identified as good candidates for interface libraries
- **Zero external dependencies** (only std library) for all candidates
- **Components excluded** per requirements (no touching components)
- **Dependency patterns** documented for informed decision-making

## Analysis Methodology

### Scope
- Analyzed all 137 header files in `src/` directory
- Excluded `flatbuffers_headers/` (generated code)
- Excluded `components/` (per requirements)
- Focused on struct and class definitions

### Dependency Classification
Dependencies categorized as:
- **std**: Standard library (string, vector, map, optional, expected, chrono, filesystem, etc.)
- **SFML**: SFML Graphics/Window/System dependencies
- **flatbuffers**: FlatBuffers generated headers (*_generated.h)
- **external**: Third-party libraries (uuid, magic_enum, nlohmann_json)
- **internal**: Other project headers

### Selection Criteria
**Good Candidates:**
- ✅ No SFML dependencies
- ✅ No FlatBuffers dependencies  
- ✅ No third-party library dependencies
- ✅ Only std library + internal project headers

**Excluded:**
- ❌ SFML dependencies (would require SFML in interface library)
- ❌ FlatBuffers dependencies (format-specific, not interface-level)
- ❌ Third-party dependencies (uuid, magic_enum, etc.)
- ❌ Component files (per requirements)

## Good Candidates for Interface Libraries

### Category 1: Pure Config/State Structs (0 internal deps)

These structs have ZERO internal dependencies - only standard library. They are ideal for the most basic interface library layer.

#### 1. `engine/EngineConfig.h`
**Structs:**
- `DisplayConfig`
- `UserPreferencesConfig`
- `EngineConfig`

**Dependencies:** None (only std library)

**Usage:** Engine configuration data structures

**Rationale:** Pure data structures with no internal dependencies. Perfect for a core configuration interface library.

---

#### 2. `engine/EngineState.h`
**Structs:**
- `PerformanceMetrics`
- `EngineState`

**Internal Dependencies:** 
- `Subscriber.h`

**Rationale:** Nearly pure - only depends on Subscriber which could also be in interface library.

---

#### 3. `scenes/SceneConfig.h`
**Struct:**
- `SceneConfig`

**Dependencies:** None (only std library)

**Usage:** Scene configuration data

**Rationale:** Currently empty placeholder, but provides consistent interface pattern. Zero dependencies makes it ideal for interface library.

---

#### 4. `scenes/SceneState.h`
**Struct:**
- `SceneState`

**Dependencies:** None (only std library)

**Usage:** Scene runtime state

**Rationale:** Pure data structure with no dependencies. Contains runtime flags and will grow with scene state needs.

---

#### 5. `scenes/SceneManagerConfig.h`
**Struct:**
- `SceneManagerConfig`

**Dependencies:** None (only std library)

**Usage:** SceneManager configuration

**Rationale:** Empty placeholder for future config, but maintains pattern consistency. Zero dependencies.

---

#### 6. `scenes/SceneManagerState.h`
**Struct:**
- `SceneManagerState`

**Dependencies:** None (only std library)

**Usage:** SceneManager runtime state

**Rationale:** Pure state struct with no dependencies.

---

#### 7. `scenes/SceneManagerResources.h`
**Struct:**
- `SceneManagerResources`

**Dependencies:** None (only std library)

**Usage:** SceneManager owned resources

**Rationale:** Pure resource container struct.

---

#### 8. `logger/FailInfo.h`
**Struct:**
- `FailInfo`

**Enum:**
- `FailMode`

**Dependencies:** None (only std::string)

**Usage:** Error information passed throughout codebase

**Rationale:** Fundamental error-handling type used everywhere. Zero dependencies makes it perfect for core interface library. Already recognized in repository memories as core type.

---

### Category 2: Event System Structs (1 internal dep)

#### 9. `events/Subscriber.h`
**Struct:**
- `Subscriber`

**Internal Dependencies:**
- `EventPacket.h`

**Why it won't work:** EventPacket depends on SFML (for keyboard/mouse types), FlatBuffers (scene_types), and uuid. This creates a dependency chain that pulls in external libraries.

**Recommendation:** Subscriber itself is clean but blocked by EventPacket. Could be moved if EventPacket is refactored to remove external deps.

---

### Category 3: Data Provider Interfaces (2-4 internal deps)

These are interface classes (abstract base classes) that define contracts for data loading. They depend on the structs they return.

#### 10-14. Data Provider Interfaces
**Files:**
- `data_providers/IAssetDataProvider.h` (2 deps)
- `data_providers/ISubscriberViewer.h` (2 deps)
- `data_providers/IFragmentDataProvider.h` (2 deps)
- `data_providers/ISceneManagerDataProvider.h` (2 deps)
- `data_providers/IEngineDataProvider.h` (4 deps)

**Pattern:** All follow interface pattern with minimal dependencies
- Depend on `FailInfo.h`
- Depend on the data struct they return
- Pure abstract interfaces (no implementation)

**Rationale:** These ARE the interface layer - they define contracts between data sources and game code. Moving them to interface libraries is their intended purpose.

**Why some won't work:**
- If their returned data types have external dependencies
- Example: `IAssetDataProvider` returns lists of asset paths - clean
- Example: `IEngineDataProvider` returns `EngineState` which contains `Subscriber`

---

### Category 4: Style Structs (1 internal dep)

#### 15-21. UI Style Structs
**Files:**
- `user_interface/styles/ButtonStyle.h`
- `user_interface/styles/DropDownButtonStyle.h`
- `user_interface/styles/DropDownItemStyle.h`
- `user_interface/styles/DropDownListStyle.h`
- `user_interface/styles/DropDownContainerStyle.h`
- `user_interface/styles/PanelStyle.h`

**Internal Dependencies:** All depend on `Style.h`

**Why they won't work:** `Style.h` depends on SFML (sf::Color, sf::Font). These style structs are tightly coupled to SFML rendering types.

**Recommendation:** Cannot be moved to interface library without SFML. Would need to abstract color/font types (e.g., `struct Color { uint8_t r, g, b, a; }`).

---

### Category 5: Entity System Structs (2-5 internal deps)

#### 22. `entity/ArchetypeManager.h`
**Class:** `ArchetypeManager`

**Internal Dependencies (5):**
- `FailInfo.h`
- `containers.h`
- `entity_types.h`
- `cstddef`
- `cstdlib`

**Why it won't work:** Depends on component containers and entity types which are specific to ECS implementation.

**Recommendation:** This is implementation, not interface. Keep in entity library.

---

#### 23. `entity/EntityManager.h`
**Class:** `EntityManager`

**Internal Dependencies (5):**
- `ArchetypeManager.h`
- `EventHandler.h`
- `FailInfo.h`
- `containers.h`
- `cstddef`

**Why it won't work:** Depends on EventHandler (SFML), components, and ECS implementation details.

**Recommendation:** Core implementation class, not suitable for interface library.

---

### Category 6: Logic System Structs (1-3 internal deps)

#### 24. `logic/Logic.h`
**Class:** `Logic`

**Internal Dependencies (2):**
- `SceneContext.h`
- `Subscriber.h`

**Why it won't work:** `SceneContext` depends on SFML (RenderTexture, RenderWindow), AssetManager, EventHandler, etc.

**Recommendation:** Abstract base class for logic processing. Depends on runtime context which has SFML. Could have an interface layer if contexts are abstracted.

---

#### 25. `logic/ILogicStep.h`
**Classes:**
- `ILogicStep`
- `LogicClassStep`
- `FunctionStep`

**Internal Dependencies (3):**
- `Logic.h`
- `SceneContext.h`
- `functional`

**Why it won't work:** Same reason as Logic.h - depends on SceneContext with SFML.

---

### Category 7: Scene/Display Classes (Implementation Heavy)

#### 26-31. Scene Implementation Classes
**Files:**
- `scenes/CraftingScene.h`
- `scenes/TitleScene.h`
- `display/Session.h`
- `engine/Engine.h`
- `engine/GameEngine.h`

**Why they won't work:** These are concrete implementation classes that depend on the full game engine stack (SFML, assets, entities, logic, etc.). They are the implementations that USE interfaces, not interfaces themselves.

---

## Problematic Structures (Cannot Be Moved)

### External Library Dependencies

#### SFML Dependencies
**Why they can't be moved:** Would require pulling SFML into interface library, defeating the purpose.

**Examples:**
- `events/UserInputBitset.h` - Uses sf::Keyboard, sf::Mouse, sf::Event
- `user_interface/styles/Style.h` - Uses sf::Color, sf::Font
- `engine/EngineResources.h` - Uses sf::RenderWindow
- `context/GameContext.h` - Uses sf::RenderWindow, sf::Vector2i
- `scenes/SceneContext.h` - Uses sf::RenderTexture, sf::RenderWindow, sf::Vector2i
- `scenes/SceneResources.h` - Uses sf::RenderTexture
- `display/*` - All display classes use SFML types
- `events/EventHandler.h` - Uses SFML event types
- `events/EventPacket.h` - Uses SFML keyboard/mouse enums
- `user_interface/UIElement.h` - Uses sf::Vector2f, SFML rendering

**Impact:** These form the core game engine types that are inherently tied to SFML. Moving them would require:
1. Abstracting all SFML types (significant refactoring)
2. Creating wrapper types (added complexity, performance cost)
3. Double conversion (SFML → wrapper → SFML)

**Recommendation:** Keep these in their current libraries. They are implementation details, not interfaces.

---

#### FlatBuffers Dependencies
**Why they can't be moved:** FlatBuffers is a serialization format, not an interface. Generated headers (*_generated.h) are format-specific.

**Examples:**
- All `Flatbuffers*` classes (FlatbuffersDataLoader, FlatbuffersConfigurator, etc.)
- `user_interface/UIElement.h` - Uses user_interface_generated.h
- `components/Fragment.h` - Uses fragments_generated.h
- `events/EventPacket.h` - Uses events_generated.h, scene_types_generated.h
- `scenes/SceneInfo.h` - Uses scene_types_generated.h
- `assets/AssetManager.h` - Uses FlatBuffers types

**Impact:** FlatBuffers is ONE implementation of data loading. The interface pattern (IDataProvider) already abstracts this.

**Recommendation:** Keep FlatBuffers implementations in data_providers/data_handlers. The I*Provider interfaces are already the abstraction layer.

---

#### Third-Party Library Dependencies
**Examples:**
- `uuid.h` (stduuid library) - Used in SceneInfo, EventPacket, DisplayManager, etc.
- `magic_enum` - Used for enum string conversion
- `nlohmann_json` - Used for JSON parsing

**Why they can't be moved:** These are specific library choices. Moving structs using them would pull these libraries into the interface layer.

**Recommendation:** 
- UUID usage is pervasive - consider if truly needed or could be replaced with simple uint64_t IDs
- magic_enum is utility-level, not interface-level
- JSON is a specific format - keep abstracted behind IDataProvider pattern

---

## Dependency Chain Analysis

### Circular Dependency Example

Current situation (example):
```
components → logger (for FailInfo)
logger → config (for paths)
events → logger (for FailInfo)
entity → components (for component types)
entity → events (for EventHandler)
logic → entity (for entity access)
logic → engine (for context types)
engine → logic (for logic systems)
```

### Breaking Circular Dependencies with Interface Libraries

**Proposed:** Create interface libraries at different dependency levels:

#### Level 0: Core Types (No dependencies)
```
interface_core/
  - FailInfo.h + FailMode enum
  - EngineConfig.h (DisplayConfig, UserPreferencesConfig, EngineConfig)
  - SceneConfig.h
  - SceneState.h
  - SceneManagerConfig.h
  - SceneManagerState.h
```

**Used by:** Everything
**Depends on:** Only std library

---

#### Level 1: Data Provider Interfaces (Depends on Level 0)
```
interface_data_providers/
  - IAssetDataProvider.h
  - IFragmentDataProvider.h
  - ISceneManagerDataProvider.h
  - IEngineDataProvider.h (needs EngineConfig, EngineState from Level 0)
```

**Used by:** data_handlers, data_providers, game systems
**Depends on:** interface_core (for FailInfo, config structs)

---

#### Level 2: Event/State Interfaces (Depends on Level 0-1)
```
interface_events/
  - Subscriber.h (if EventPacket is refactored)
  - EngineState.h (contains Subscriber)
```

**Blocked by:** EventPacket's external dependencies

---

### What Cannot Be Separated

**Reason: SFML is fundamental to the architecture**

These types are inherently tied to SFML and form the core engine:
- Window management (sf::RenderWindow)
- Rendering (sf::RenderTexture, sf::Drawable)
- Input (sf::Keyboard, sf::Mouse, sf::Event)
- Math (sf::Vector2f, sf::Vector2i)
- Graphics (sf::Color, sf::Font, sf::Texture)

**Recommendation:** Accept that SFML is a core dependency for game engine types. Don't try to abstract it away - it's the chosen rendering framework.

---

## Recommendations

### 1. Create Core Interface Library ✅

**Library:** `interface_core` (INTERFACE library in CMake)

**Contents:**
- `FailInfo.h` / `FailMode`
- `EngineConfig.h` structs
- `SceneConfig.h`
- `SceneState.h`  
- `SceneManagerConfig.h`
- `SceneManagerState.h`

**Dependencies:** Only std library

**Benefits:**
- Zero external dependencies
- Used throughout codebase
- Already pure data structures
- Easy to extract (no implementation files)

**CMake:**
```cmake
add_library(interface_core INTERFACE)
target_include_directories(interface_core INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/interface_core
)
# No target_link_libraries - only std library
```

**Impact:** Breaks circular dependency between logger and other systems

---

### 2. Create Data Provider Interface Library ✅

**Library:** `interface_data_providers` (INTERFACE library)

**Contents:**
- `IAssetDataProvider.h`
- `IFragmentDataProvider.h`
- `ISceneManagerDataProvider.h`
- `IEngineDataProvider.h`
- `ISubscriberViewer.h`

**Dependencies:**
- `interface_core` (for FailInfo, config structs)
- std library

**Benefits:**
- Abstracts data loading from format
- Clean contract between data sources and consumers
- Already well-designed interfaces

**CMake:**
```cmake
add_library(interface_data_providers INTERFACE)
target_include_directories(interface_data_providers INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/interface_data_providers
)
target_link_libraries(interface_data_providers INTERFACE
  interface_core
)
```

**Impact:** Allows data_handlers and data_providers to depend on clean interfaces

---

### 3. Don't Try to Abstract SFML ❌

**Why not:** 
- SFML is the chosen rendering framework
- Wrapping SFML types adds complexity and overhead
- These types are already abstractions (SFML provides the abstraction layer)
- Conversion overhead (game code → wrapper → SFML → GPU)

**Keep SFML dependencies in:**
- engine (EngineResources, GameContext)
- scenes (SceneContext, SceneResources)
- display
- events (EventHandler, EventPacket)
- user_interface (UIElement)

**Recommendation:** Accept SFML as a foundational dependency. It's not circular - it's directional (everyone depends on SFML).

---

### 4. Consider Refactoring EventPacket ⚠️

**Current problem:**
```cpp
struct EventPacket {
  EventType m_event_type;           // FlatBuffers enum
  EventData m_event_data;           // variant with SFML types
  uuids::uuid event_id;             // uuid library
  uuids::uuid source_id;            // uuid library
  uint8_t event_lifetime;
};
```

**External dependencies:**
- SFML (for keyboard/mouse in UserInputBitset)
- FlatBuffers (for EventType enum)
- uuid library

**Refactoring options:**

**Option A: Remove UUID dependency**
```cpp
using EventID = uint64_t;  // Instead of uuids::uuid
```

**Option B: Abstract event types**
```cpp
// Instead of FlatBuffers enum
enum class EventType {
  None, UserInput, SceneChange, UIAction, ...
};
```

**Option C: Decouple SFML input types**
```cpp
// Create own input types, convert from SFML at boundary
struct KeyCode { int code; };
struct MouseButton { int button; };
```

**Benefit:** Would allow Subscriber and event system types to move to interface library

**Cost:** Significant refactoring, added conversion layer

**Recommendation:** Evaluate if the benefit justifies the cost. Current architecture with external dependencies is not inherently bad - just incompatible with interface library extraction.

---

## Implementation Priority

### Phase 1: Core Types (Immediate, Low Risk) ✅

**Action:** Create `interface_core` library

**Contents:**
- FailInfo.h
- EngineConfig.h
- SceneConfig.h
- SceneState.h
- SceneManagerConfig.h
- SceneManagerState.h

**Effort:** Low (2-4 hours)
- Create new directory
- Move headers
- Update CMakeLists.txt
- Update #include paths

**Risk:** Very low - these have no implementation files

**Benefit:** Breaks logger circular dependencies

---

### Phase 2: Data Provider Interfaces (Medium Priority) ✅

**Action:** Create `interface_data_providers` library

**Contents:**
- IAssetDataProvider.h
- IFragmentDataProvider.h
- ISceneManagerDataProvider.h
- IEngineDataProvider.h
- ISubscriberViewer.h

**Effort:** Low-Medium (4-6 hours)
- Create new directory
- Move interface headers
- Update CMakeLists.txt
- Update #include paths

**Risk:** Low - pure interfaces, no implementation

**Benefit:** Clean separation between interface and implementation

---

### Phase 3: Event System Refactoring (Future, High Effort) ⚠️

**Action:** Refactor EventPacket to remove external dependencies

**Changes:**
- Replace uuid with uint64_t
- Abstract event types from FlatBuffers
- Consider SFML input abstraction

**Effort:** High (2-3 days)
- Significant refactoring
- Need to update all event usage
- Testing required

**Risk:** Medium - touches core event system

**Benefit:** Would allow event types in interface library

**Recommendation:** Defer until clear need demonstrated. Current architecture works.

---

## Summary

### Good Candidates (Can Be Moved) ✅

**8 structs** can be immediately moved to `interface_core`:
1. FailInfo + FailMode
2. DisplayConfig
3. UserPreferencesConfig
4. EngineConfig
5. SceneConfig
6. SceneState
7. SceneManagerConfig
8. SceneManagerState

**5 interfaces** can be moved to `interface_data_providers`:
1. IAssetDataProvider
2. IFragmentDataProvider
3. ISceneManagerDataProvider
4. IEngineDataProvider
5. ISubscriberViewer

### Cannot Be Moved (External Dependencies) ❌

**44 structs/classes** cannot be moved due to:
- **SFML dependencies:** 25+ structures (rendering, input, math types)
- **FlatBuffers dependencies:** 10+ structures (serialization format-specific)
- **UUID dependencies:** 5+ structures (third-party library)
- **Implementation classes:** 10+ classes (concrete game engine implementations)

### Recommendations

1. ✅ **DO:** Create `interface_core` library with pure config/state structs
2. ✅ **DO:** Create `interface_data_providers` library with data loading interfaces
3. ❌ **DON'T:** Try to abstract SFML - it's the rendering framework
4. ⚠️ **CONSIDER:** Refactoring EventPacket if event system interface extraction is priority
5. ✅ **DO:** Accept that implementation libraries will depend on SFML
6. ✅ **DO:** Use interface pattern (IDataProvider) for format abstraction, not struct extraction

### Expected Impact

**Circular dependencies broken:**
- logger ← → components (FailInfo moved to interface_core)
- data_handlers ← → data_providers (interfaces separated)

**No change needed for:**
- SFML dependencies (accepted as foundational)
- Component structs (excluded per requirements)
- Implementation classes (appropriate in implementation libraries)

---

## Appendix: Full Candidate List

### All 52 Analyzed Candidates

**Category: Zero Internal Dependencies (8)**
1. EngineConfig.h - DisplayConfig, UserPreferencesConfig, EngineConfig
2. EngineState.h - PerformanceMetrics, EngineState
3. SceneConfig.h - SceneConfig
4. SceneState.h - SceneState
5. SceneManagerConfig.h - SceneManagerConfig
6. SceneManagerState.h - SceneManagerState
7. SceneManagerResources.h - SceneManagerResources
8. FailInfo.h - FailInfo, FailMode

**Category: 1 Internal Dependency (14)**
9. Subscriber.h (depends on EventPacket - blocked by SFML/uuid/FlatBuffers)
10-15. UI Style structs (blocked by Style.h → SFML)
16. CraftingScene.h (implementation class)
17. Session.h (depends on Tile → SFML)

**Category: 2 Internal Dependencies (17)**
18-22. Data provider interfaces (IAssetDataProvider, ISubscriberViewer, etc.)
23-28. UI element structs (all depend on UIElement → SFML)
29-31. FlatBuffers provider implementations

**Category: 3+ Internal Dependencies (13)**
32. ILogicStep.h (depends on SceneContext → SFML)
33. FlatbuffersEngineDataProvider.h
34. Engine.h (implementation class)
35. ArchetypeManager.h (ECS implementation)
36. EntityManager.h (ECS implementation)
37. UIStyle.h (depends on individual style structs)
38-44. Various implementation classes

**Total Good Candidates for Interface Libraries: 13**
- 8 config/state structs (zero deps)
- 5 data provider interfaces (depend on FailInfo only)

**Total Blocked by External Dependencies: 39**
- SFML: 25+
- FlatBuffers: 10+
- UUID: 5+
- Implementation complexity: remainder

---

## Document Metadata

**Analysis Date:** December 7, 2025
**Repository:** walliscode/SteamRot
**Branch:** copilot/analyze-data-structs-for-interfaces
**Scope:** Analysis only, no code changes
**Exclusions:** Components (per requirements)
**Files Analyzed:** 137 headers in src/ (excluding flatbuffers_headers/)

