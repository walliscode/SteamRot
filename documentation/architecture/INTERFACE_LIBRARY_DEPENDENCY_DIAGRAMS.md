# Interface Library Dependency Diagrams

## Visual Overview of Data Structure Dependencies

This document provides visual representations of dependencies for structures analyzed for interface library extraction.

---

## Current Dependency Graph (Problematic Circular Dependencies)

```
┌─────────────────────────────────────────────────────────────┐
│                    Current Architecture                       │
│              (Showing Circular Dependencies)                 │
└─────────────────────────────────────────────────────────────┘

                    ┌──────────────┐
                    │  components  │
                    └──────┬───────┘
                           │ uses
                           ↓
                    ┌──────────────┐
                    │    logger    │←──────────┐
                    │  (FailInfo)  │           │
                    └──────┬───────┘           │
                           │ uses              │ uses
                           ↓                   │
                    ┌──────────────┐          │
                    │    config    │          │
                    └──────────────┘          │
                                              │
     ┌──────────────┐         ┌──────────────┴───┐
     │    events    │────────→│      entity      │
     │ (EventHandler)│ uses    │ (EntityManager)  │
     └──────┬───────┘         └──────────┬───────┘
            │                            │
            │ uses                       │ uses
            ↓                            ↓
     ┌──────────────┐         ┌──────────────┐
     │    engine    │←────────│    logic     │
     │(EngineResources)│ uses  │  (Logic*)    │
     └──────────────┘         └──────────────┘
            ↓                            ↑
            │ uses                       │
            └────────────────────────────┘
                  CIRCULAR!
```

**Problem:** Circular dependencies prevent clean library separation and can cause CMake link errors.

---

## Proposed Architecture with Interface Libraries

```
┌─────────────────────────────────────────────────────────────────────┐
│                  Proposed Layered Architecture                       │
│           (Breaking Circular Dependencies with Interfaces)           │
└─────────────────────────────────────────────────────────────────────┘

Level 0: ┌─────────────────────────────────────┐
         │     interface_core (INTERFACE)      │
         │  - FailInfo, FailMode              │
         │  - EngineConfig (Display, UserPref) │
         │  - SceneConfig, SceneState          │
         │  - SceneManagerConfig, State        │
         │                                     │
         │  Dependencies: NONE (std only)      │
         └─────────────────┬───────────────────┘
                           │
                           │ depends on
                           ↓
Level 1: ┌─────────────────────────────────────┐
         │ interface_data_providers (INTERFACE)│
         │  - IAssetDataProvider              │
         │  - IFragmentDataProvider           │
         │  - ISceneManagerDataProvider       │
         │  - IEngineDataProvider             │
         │  - ISubscriberViewer               │
         │                                     │
         │  Dependencies: interface_core       │
         └─────────────────┬───────────────────┘
                           │
                           │ depends on
                           ↓
Level 2: ┌─────────────────────────────────────┐
         │   Implementation Libraries          │
         │  - logger (uses FailInfo)           │
         │  - data_handlers (uses interfaces)  │
         │  - data_providers (implements)      │
         │  - components                       │
         │  - events, entity, logic, etc.      │
         │                                     │
         │  Dependencies: interface_*, SFML    │
         └─────────────────────────────────────┘
```

**Key:** Dependency direction is ONE-WAY (top-down only). No circular dependencies.

---

## Dependency Levels by Structure Type

### Level 0: Core Types (Interface Library)

```
┌───────────────────────────────────────────────────────┐
│              interface_core Library                    │
│  (Zero external dependencies - only std library)       │
└───────────────────────────────────────────────────────┘

    FailInfo.h
    ├── FailMode (enum)
    └── FailInfo (struct)
        ├── mode: FailMode
        └── message: std::string

    EngineConfig.h
    ├── DisplayConfig (struct)
    │   ├── window_title: std::string
    │   ├── framerate_limit: uint32_t
    │   ├── fullscreen: bool
    │   └── vsync: bool
    ├── UserPreferencesConfig (struct)
    │   ├── master_volume: float
    │   ├── show_fps: bool
    │   └── preferred_language: std::string
    └── EngineConfig (struct)
        ├── display: DisplayConfig
        └── user_preferences: UserPreferencesConfig

    SceneConfig.h
    └── SceneConfig (struct) [empty, future expansion]

    SceneState.h
    └── SceneState (struct)
        └── active: bool

    SceneManagerConfig.h
    └── SceneManagerConfig (struct) [empty, future expansion]

    SceneManagerState.h
    └── SceneManagerState (struct) [empty, future expansion]

    SceneManagerResources.h
    └── SceneManagerResources (struct) [empty, future expansion]

┌───────────────────────────────────────────────────────┐
│ Dependency Score: 0 external, 0 internal              │
│ Risk: NONE - Pure data structures                     │
│ Benefit: Breaks circular deps with logger             │
└───────────────────────────────────────────────────────┘
```

---

### Level 1: Data Provider Interfaces

```
┌───────────────────────────────────────────────────────────┐
│         interface_data_providers Library                   │
│  (Depends on: interface_core)                              │
└───────────────────────────────────────────────────────────┘

    IAssetDataProvider.h
    └── IAssetDataProvider (interface)
        └── LoadAssets() → std::expected<AssetList, FailInfo>
                                                    ↑ from interface_core

    IFragmentDataProvider.h
    └── IFragmentDataProvider (interface)
        └── LoadFragments() → std::expected<Fragments, FailInfo>
                                                      ↑ from interface_core

    ISceneManagerDataProvider.h
    └── ISceneManagerDataProvider (interface)
        └── LoadSceneManagerState() → 
            std::expected<SceneManagerState, FailInfo>
                          ↑ from interface_core  ↑ from interface_core

    IEngineDataProvider.h
    ├── EngineResourcesConfigData (struct)
    │   ├── window_width: uint32_t
    │   ├── window_height: uint32_t
    │   ├── window_title: std::string
    │   └── framerate_limit: uint32_t
    └── IEngineDataProvider (interface)
        ├── LoadEngineResourcesConfig() → 
        │   std::expected<EngineResourcesConfigData, FailInfo>
        ├── LoadEngineConfig() → 
        │   std::expected<EngineConfig, FailInfo>
        │                 ↑ from interface_core
        └── LoadEngineState() → 
            std::expected<EngineState, FailInfo>
                          ↑ (blocked - depends on Subscriber)

    ISubscriberViewer.h
    └── ISubscriberViewer (interface)
        └── GetSubscribers() → 
            std::expected<vector<shared_ptr<Subscriber>>, FailInfo>
                                          ↑ (blocked - depends on EventPacket)

┌───────────────────────────────────────────────────────────┐
│ Dependency Score: 0 external, 1 internal (interface_core) │
│ Risk: LOW - Pure interfaces, well-tested pattern          │
│ Benefit: Clean data loading abstraction                   │
│ Blocked: IEngineDataProvider.LoadEngineState() needs      │
│          Subscriber (which depends on EventPacket)         │
└───────────────────────────────────────────────────────────┘
```

---

## Structures That Cannot Be Moved (Dependency Analysis)

### SFML-Dependent Structures

```
┌───────────────────────────────────────────────────────┐
│        SFML Dependency Chain (Cannot Extract)          │
└───────────────────────────────────────────────────────┘

    EventPacket.h
    ├── DEPENDS ON: SFML/Window/Keyboard.hpp
    ├── DEPENDS ON: SFML/Window/Mouse.hpp
    ├── DEPENDS ON: uuid.h (uuids::uuid)
    ├── DEPENDS ON: events_generated.h (FlatBuffers)
    └── USED BY: Subscriber, EventHandler, UIElement
                 └── BLOCKS: Event system extraction

    UserInputBitset.h
    ├── DEPENDS ON: SFML/Window/Event.hpp
    ├── DEPENDS ON: SFML/Window/Keyboard.hpp
    └── DEPENDS ON: SFML/Window/Mouse.hpp
        └── REASON: Wraps SFML input for bitset operations

    EngineResources.h
    ├── DEPENDS ON: SFML/Graphics/RenderWindow.hpp
    ├── DEPENDS ON: SFML/System/Vector2.hpp
    └── MEMBERS:
        ├── sf::RenderWindow game_window
        ├── EventHandler event_handler
        ├── sf::Vector2i mouse_position
        └── AssetManager asset_manager

    GameContext.h
    ├── DEPENDS ON: SFML/Graphics/RenderWindow.hpp
    ├── DEPENDS ON: SFML/System/Vector2.hpp
    └── MEMBERS (all references):
        ├── sf::RenderWindow &game_window
        ├── EventHandler &event_handler
        ├── sf::Vector2i &mouse_position
        └── AssetManager &asset_manager

    SceneContext.h
    ├── DEPENDS ON: SFML/Graphics/RenderTexture.hpp
    ├── DEPENDS ON: SFML/Graphics/RenderWindow.hpp
    ├── DEPENDS ON: SFML/System/Vector2.hpp
    └── MEMBERS (all references):
        ├── EntityMemoryPool &scene_entities
        ├── Archetype &archetypes
        ├── sf::RenderTexture &scene_texture
        ├── sf::RenderWindow &game_window
        ├── AssetManager &asset_manager
        ├── EventHandler &event_handler
        └── sf::Vector2i &mouse_position

    SceneResources.h
    ├── DEPENDS ON: SFML/Graphics/RenderTexture.hpp
    └── MEMBERS:
        ├── EntityManager entity_manager
        ├── LogicCollection logic_map
        ├── sf::RenderTexture scene_texture
        └── const GameContext &game_context

    UIElement.h
    ├── DEPENDS ON: SFML/Graphics.hpp
    ├── DEPENDS ON: user_interface_generated.h (FlatBuffers)
    └── MEMBERS:
        ├── sf::Vector2f position
        ├── sf::Vector2f size
        ├── subscription: shared_ptr<Subscriber>
        ├── response_event: optional<EventPacket>
        └── virtual DrawUIElement(sf::RenderTexture&)

    Style.h (and all *Style.h files)
    ├── DEPENDS ON: SFML/Graphics/Color.hpp
    ├── DEPENDS ON: SFML/Graphics/Font.hpp
    └── MEMBERS:
        ├── sf::Color background_color
        ├── sf::Color border_color
        ├── sf::Font* font
        └── ...

┌───────────────────────────────────────────────────────────┐
│ SFML Dependency Impact                                     │
├────────────────────────────────────────────────────────────┤
│ • SFML IS the rendering/input framework                   │
│ • Abstracting SFML requires wrappers for all types        │
│ • Performance cost: double conversion (wrapper ↔ SFML)    │
│ • Complexity cost: maintenance of parallel type system    │
│                                                            │
│ RECOMMENDATION: Accept SFML as foundational dependency    │
│                 Don't try to extract SFML-dependent types │
└────────────────────────────────────────────────────────────┘
```

---

### FlatBuffers-Dependent Structures

```
┌───────────────────────────────────────────────────────┐
│      FlatBuffers Dependency Chain (Format-Specific)    │
└───────────────────────────────────────────────────────┘

    *_generated.h files (auto-generated)
    ├── events_generated.h (EventType enum)
    ├── scene_types_generated.h (SceneType enum)
    ├── user_interface_generated.h (UI enums)
    ├── fragments_generated.h (Fragment enums)
    └── Used by: EventPacket, UIElement, Fragment, SceneInfo

    Flatbuffers* Implementation Classes
    ├── FlatbuffersDataLoader
    ├── FlatbuffersConfigurator
    ├── FlatbuffersEngineDataProvider
    ├── FlatbuffersSceneDataProvider
    ├── FlatbuffersAssetDataProvider
    └── FlatbuffersFragmentDataProvider
        └── PURPOSE: Implement I*Provider interfaces for FlatBuffers

┌───────────────────────────────────────────────────────────┐
│ FlatBuffers Dependency Impact                             │
├────────────────────────────────────────────────────────────┤
│ • FlatBuffers is ONE serialization format                │
│ • I*Provider interfaces already abstract this             │
│ • Generated enums could be replaced with native enums     │
│   (but requires refactoring)                              │
│                                                            │
│ RECOMMENDATION: Keep FlatBuffers in data_providers/       │
│                 Use I*Provider pattern for abstraction    │
│                 Consider replacing generated enums with   │
│                 native enums for interface extraction     │
└────────────────────────────────────────────────────────────┘
```

---

### UUID-Dependent Structures

```
┌───────────────────────────────────────────────────────┐
│        UUID Dependency Chain (Third-Party Lib)         │
└───────────────────────────────────────────────────────┘

    SceneInfo.h
    ├── DEPENDS ON: uuid.h (uuids::uuid)
    ├── DEPENDS ON: scene_types_generated.h (FlatBuffers)
    └── MEMBERS:
        ├── uuids::uuid id
        └── SceneType type

    EventPacket.h
    ├── DEPENDS ON: uuid.h (uuids::uuid)
    └── MEMBERS:
        ├── uuids::uuid event_id
        └── uuids::uuid source_id

    Scene, SceneManager, SceneFactory, etc.
    └── Use uuids::uuid for scene identification

┌───────────────────────────────────────────────────────────┐
│ UUID Dependency Impact                                     │
├────────────────────────────────────────────────────────────┤
│ • UUID is third-party library (stduuid)                   │
│ • Provides globally unique identifiers                    │
│ • Alternative: uint64_t with manual ID generation         │
│                                                            │
│ RECOMMENDATION: Evaluate if UUID is needed                │
│                 Could replace with simple uint64_t        │
│                 Would unblock SceneInfo, EventPacket      │
└────────────────────────────────────────────────────────────┘
```

---

## Dependency Cascade Visualization

### Example: Why Subscriber Cannot Be Extracted

```
┌──────────────────────────────────────────────────────────┐
│     Subscriber.h Dependency Cascade (Example)             │
└──────────────────────────────────────────────────────────┘

Subscriber.h
    ↓ includes
EventPacket.h
    ↓ includes
    ├── UserInputBitset.h
    │   ↓ includes
    │   ├── SFML/Window/Event.hpp ❌ EXTERNAL
    │   ├── SFML/Window/Keyboard.hpp ❌ EXTERNAL
    │   └── SFML/Window/Mouse.hpp ❌ EXTERNAL
    │
    ├── events_generated.h ❌ FLATBUFFERS
    ├── scene_types_generated.h ❌ FLATBUFFERS
    └── uuid.h ❌ EXTERNAL (uuids::uuid)

┌──────────────────────────────────────────────────────────┐
│ BLOCKED: Subscriber cannot be in interface library        │
│          because EventPacket pulls in SFML, FlatBuffers,  │
│          and UUID dependencies                            │
└──────────────────────────────────────────────────────────┘
```

### Solution: Refactor EventPacket

```
┌──────────────────────────────────────────────────────────┐
│  Proposed: Abstract EventPacket Dependencies              │
└──────────────────────────────────────────────────────────┘

Current:
    struct EventPacket {
        EventType m_event_type;        // FlatBuffers enum ❌
        EventData m_event_data;        // has SFML types ❌
        uuids::uuid event_id;          // UUID library ❌
        uuids::uuid source_id;         // UUID library ❌
        uint8_t event_lifetime;        // OK ✓
    };

Refactored:
    struct EventPacket {
        EventTypeNative m_event_type;  // Native enum ✓
        EventData m_event_data;        // Abstract types ✓
        uint64_t event_id;             // Simple ID ✓
        uint64_t source_id;            // Simple ID ✓
        uint8_t event_lifetime;        // OK ✓
    };

    // Conversion functions at boundaries
    EventPacketNative ConvertFromSFML(const SFMLEvent&);
    SFMLEvent ConvertToSFML(const EventPacketNative&);

┌──────────────────────────────────────────────────────────┐
│ COST: Significant refactoring (2-3 days)                  │
│ BENEFIT: Unlocks Subscriber for interface library         │
│ RECOMMENDATION: Only if event system interface extraction │
│                 is high priority                          │
└──────────────────────────────────────────────────────────┘
```

---

## Library Organization Chart

### Current Organization (Monolithic)

```
src/
├── components/        (depends on logger, engine, user_interface, SFML, FlatBuffers)
├── logger/            (depends on config)
├── config/            (generated, no deps)
├── events/            (depends on SFML, FlatBuffers, logger)
├── entity/            (depends on components, logger)
├── logic/             (depends on entity, engine, user_interface)
├── engine/            (depends on display, entity, events, logic, assets, config)
├── scenes/            (depends on display, entity, logic, context)
├── display/           (depends on SFML, scenes, logic)
├── user_interface/    (depends on SFML, FlatBuffers, events, engine)
├── assets/            (depends on SFML, FlatBuffers)
├── data_handlers/     (depends on FlatBuffers, logger, components)
├── data_providers/    (depends on data_handlers, components, FlatBuffers, events)
└── context/           (depends on assets, events, engine, entity)

Problems:
• Many circular dependencies
• Hard to build libraries independently
• CMake link order matters
```

### Proposed Organization (Layered with Interfaces)

```
src/
├── interface_core/           ← NEW: Level 0
│   ├── FailInfo.h
│   ├── EngineConfig.h
│   ├── SceneConfig.h
│   ├── SceneState.h
│   ├── SceneManagerConfig.h
│   ├── SceneManagerState.h
│   └── SceneManagerResources.h
│   Dependencies: NONE (INTERFACE library, std only)
│
├── interface_data_providers/ ← NEW: Level 1
│   ├── IAssetDataProvider.h
│   ├── IFragmentDataProvider.h
│   ├── ISceneManagerDataProvider.h
│   ├── IEngineDataProvider.h
│   └── ISubscriberViewer.h
│   Dependencies: interface_core
│
├── components/              ← Level 2
│   Dependencies: interface_core (for FailInfo)
│
├── logger/                  ← Level 2
│   Dependencies: interface_core (for FailInfo), config
│
├── data_handlers/           ← Level 2
│   Dependencies: interface_data_providers, interface_core, FlatBuffers
│
├── data_providers/          ← Level 2
│   Dependencies: interface_data_providers, data_handlers, FlatBuffers
│
├── events/                  ← Level 2
│   Dependencies: interface_core, SFML, FlatBuffers, logger
│
├── entity/                  ← Level 2
│   Dependencies: interface_core, components, events
│
├── user_interface/          ← Level 2
│   Dependencies: interface_core, SFML, FlatBuffers, events
│
├── assets/                  ← Level 2
│   Dependencies: interface_core, interface_data_providers, SFML
│
├── logic/                   ← Level 3
│   Dependencies: entity, engine, user_interface
│
├── scenes/                  ← Level 3
│   Dependencies: entity, logic, context
│
├── engine/                  ← Level 3
│   Dependencies: all Level 2 + scenes
│
├── display/                 ← Level 3
│   Dependencies: scenes, logic, SFML
│
└── context/                 ← Level 3
    Dependencies: assets, events, engine, entity

Benefits:
✓ No circular dependencies
✓ Clear dependency direction (level 0 → 1 → 2 → 3)
✓ Interface libraries can be built independently
✓ Easy to understand dependency flow
```

---

## CMake Configuration Comparison

### Before (Circular Dependencies)

```cmake
# components/CMakeLists.txt
add_library(components ...)
target_link_libraries(components PUBLIC
  logger          # logger depends on config
  engine          # engine depends on logic
  user_interface  # user_interface depends on events, engine
  # ... CIRCULAR!
)

# logger/CMakeLists.txt
add_library(logger ...)
target_link_libraries(logger PUBLIC
  config
)

# engine/CMakeLists.txt
add_library(engine ...)
target_link_libraries(engine PUBLIC
  logic          # logic depends on entity, engine → CIRCULAR!
  events
  entity
  # ...
)
```

### After (Clean Layered Dependencies)

```cmake
# interface_core/CMakeLists.txt (NEW)
add_library(interface_core INTERFACE)
target_include_directories(interface_core INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
# No target_link_libraries - only std library!

# interface_data_providers/CMakeLists.txt (NEW)
add_library(interface_data_providers INTERFACE)
target_include_directories(interface_data_providers INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(interface_data_providers INTERFACE
  interface_core  # Only interface_core!
)

# components/CMakeLists.txt (UPDATED)
add_library(components ...)
target_link_libraries(components PUBLIC
  interface_core  # Instead of logger!
  SFML::Graphics
  # ... other deps
)

# logger/CMakeLists.txt (UPDATED)
add_library(logger ...)
target_link_libraries(logger PUBLIC
  interface_core  # Instead of having FailInfo internally!
  config
)

# data_handlers/CMakeLists.txt (UPDATED)
add_library(data_handlers ...)
target_link_libraries(data_handlers PUBLIC
  interface_core
  interface_data_providers  # Uses interfaces!
  flatbuffers
)

# data_providers/CMakeLists.txt (UPDATED)
add_library(data_providers ...)
target_link_libraries(data_providers PUBLIC
  interface_data_providers  # Implements interfaces!
  data_handlers
  flatbuffers
)
```

---

## Migration Path Visualization

### Phase 1: Extract Core Types

```
Step 1: Create interface_core directory
src/interface_core/
├── CMakeLists.txt
├── FailInfo.h
├── EngineConfig.h
├── SceneConfig.h
├── SceneState.h
├── SceneManagerConfig.h
├── SceneManagerState.h
└── SceneManagerResources.h

Step 2: Update CMakeLists.txt
src/CMakeLists.txt:
  add_subdirectory(interface_core)  # Add FIRST!
  add_subdirectory(components)
  add_subdirectory(logger)
  # ...

Step 3: Update #include paths
Before: #include "FailInfo.h"
After:  #include "interface_core/FailInfo.h"

Or create forward headers:
logger/FailInfo.h:
  #pragma once
  #include "../interface_core/FailInfo.h"

Step 4: Update target_link_libraries
components/CMakeLists.txt:
  - target_link_libraries(components PUBLIC logger ...)
  + target_link_libraries(components PUBLIC interface_core ...)

logger/CMakeLists.txt:
  + target_link_libraries(logger PUBLIC interface_core ...)
```

### Phase 2: Extract Data Provider Interfaces

```
Step 1: Create interface_data_providers directory
src/interface_data_providers/
├── CMakeLists.txt
├── IAssetDataProvider.h
├── IFragmentDataProvider.h
├── ISceneManagerDataProvider.h
├── IEngineDataProvider.h
└── ISubscriberViewer.h

Step 2: Update CMakeLists.txt
src/CMakeLists.txt:
  add_subdirectory(interface_core)
  add_subdirectory(interface_data_providers)  # After interface_core!
  # ...

Step 3: Update #include paths and target_link_libraries
data_handlers/CMakeLists.txt:
  + target_link_libraries(data_handlers PUBLIC
  +   interface_data_providers
  +   ...
  + )

data_providers/CMakeLists.txt:
  + target_link_libraries(data_providers PUBLIC
  +   interface_data_providers
  +   ...
  + )
```

---

## Summary Diagrams

### Structures by Dependency Type

```
┌─────────────────────────────────────────────────────────────┐
│                  Dependency Classification                   │
└─────────────────────────────────────────────────────────────┘

                    ┌────────────┐
                    │  TOTAL: 52 │
                    │  Analyzed  │
                    └─────┬──────┘
                          │
         ┌────────────────┼────────────────┐
         │                │                │
         ↓                ↓                ↓
    ┌─────────┐    ┌──────────┐    ┌──────────┐
    │  GOOD   │    │ BLOCKED  │    │  IMPL    │
    │   13    │    │    25    │    │   14     │
    │   ✓     │    │    ❌    │    │    ⚠     │
    └────┬────┘    └─────┬────┘    └─────┬────┘
         │              │                │
         │              │                │
    ┌────▼─────┐   ┌────▼─────┐    ┌────▼─────┐
    │ 8 Config │   │ 15 SFML  │    │ Classes  │
    │ Structs  │   │  deps    │    │ Too      │
    ├──────────┤   ├──────────┤    │ Complex  │
    │ 5 Data   │   │ 5 UUID   │    │          │
    │ Provider │   │  deps    │    └──────────┘
    │ Ifaces   │   ├──────────┤
    └──────────┘   │ 5 FBuf   │
                   │  deps    │
                   └──────────┘

Good Candidates (13):     Can move to interface libraries
Blocked (25):             External dependencies prevent extraction
Implementation (14):       Too complex, appropriate in impl libraries
```

### Dependency Impact Matrix

```
┌──────────────────────────────────────────────────────────────┐
│           Dependency Impact on Interface Extraction          │
├───────────────┬────────┬──────────────────────────────────────┤
│ Dependency    │ Count  │ Impact                               │
├───────────────┼────────┼──────────────────────────────────────┤
│ None (std)    │   8    │ ✓ Can extract immediately            │
│ interface_*   │   5    │ ✓ Can extract to interface lib       │
│ SFML          │  25+   │ ❌ Cannot extract (is framework)     │
│ FlatBuffers   │  10+   │ ❌ Format-specific (use I*Provider)  │
│ UUID          │   5    │ ⚠ Consider replacing with uint64_t   │
│ Complex impl  │  14    │ ⚠ Appropriate in impl libraries      │
└───────────────┴────────┴──────────────────────────────────────┘
```

---

## Conclusion

**Extractable to Interface Libraries:**
- ✅ 8 config/state structs → interface_core
- ✅ 5 data provider interfaces → interface_data_providers

**Cannot Be Extracted:**
- ❌ 25+ SFML-dependent structures (framework is foundational)
- ❌ 10+ FlatBuffers-dependent (format-specific, already abstracted)
- ⚠️ 5 UUID-dependent (could refactor to uint64_t if needed)
- ⚠️ 14 implementation classes (appropriate complexity)

**Net Result:** Breaking 2 major circular dependency chains with minimal effort while accepting that SFML is a foundational dependency.

