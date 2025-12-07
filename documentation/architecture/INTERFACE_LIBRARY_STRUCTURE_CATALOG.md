# Interface Library Structure Catalog

Complete catalog of all 52 analyzed structures with dependency details.

## Legend

**Dependency Types:**
- **std**: Standard library only
- **SFML**: SFML Graphics/Window/System
- **FlatBuffers**: Generated FlatBuffers headers
- **UUID**: stduuid library
- **Internal**: Other project headers

**Extractability:**
- ✅ **YES**: Can move to interface library
- ❌ **NO**: Blocked by external dependencies
- ⚠️ **MAYBE**: Possible with refactoring

---

## Extractable Structures (13)

### interface_core Candidates (8 structures)

| # | File | Structure | Type | Internal Deps | External Deps | Target Library |
|---|------|-----------|------|---------------|---------------|----------------|
| 1 | `logger/FailInfo.h` | `FailMode` | enum | 0 | std only | interface_core |
| 2 | `logger/FailInfo.h` | `FailInfo` | struct | 0 | std only | interface_core |
| 3 | `engine/EngineConfig.h` | `DisplayConfig` | struct | 0 | std only | interface_core |
| 4 | `engine/EngineConfig.h` | `UserPreferencesConfig` | struct | 0 | std only | interface_core |
| 5 | `engine/EngineConfig.h` | `EngineConfig` | struct | 0 | std only | interface_core |
| 6 | `scenes/SceneConfig.h` | `SceneConfig` | struct | 0 | std only | interface_core |
| 7 | `scenes/SceneState.h` | `SceneState` | struct | 0 | std only | interface_core |
| 8 | `scenes/SceneManagerConfig.h` | `SceneManagerConfig` | struct | 0 | std only | interface_core |

**Notes:**
- Zero external dependencies
- Pure data structures
- Currently empty structs (SceneConfig, etc.) are placeholders for future expansion
- FailInfo is fundamental error type used throughout codebase

---

### interface_data_providers Candidates (5 interfaces)

| # | File | Structure | Type | Internal Deps | External Deps | Target Library |
|---|------|-----------|------|---------------|---------------|----------------|
| 9 | `data_providers/IAssetDataProvider.h` | `IAssetDataProvider` | interface | FailInfo.h | std only | interface_data_providers |
| 10 | `data_providers/IFragmentDataProvider.h` | `IFragmentDataProvider` | interface | FailInfo.h, Fragment.h | std only | interface_data_providers |
| 11 | `data_providers/ISceneManagerDataProvider.h` | `ISceneManagerDataProvider` | interface | FailInfo.h, SceneManagerState.h | std only | interface_data_providers |
| 12 | `data_providers/IEngineDataProvider.h` | `EngineResourcesConfigData` | struct | 0 | std only | interface_data_providers |
| 13 | `data_providers/IEngineDataProvider.h` | `IEngineDataProvider` | interface | FailInfo.h, EngineConfig.h, EngineState.h | std only | interface_data_providers |

**Notes:**
- Pure abstract interfaces
- Define contracts for data loading
- Depend only on interface_core types
- Already follow good interface design pattern
- Note: IEngineDataProvider.LoadEngineState() returns EngineState which depends on Subscriber (which has EventPacket dependency chain)

**Special Case:**
- `ISubscriberViewer` interface could be moved but depends on `Subscriber` which depends on `EventPacket` (SFML + UUID + FlatBuffers)

---

## Non-Extractable Structures (39+)

### SFML-Dependent Structures (25+)

| File | Structure | Type | Reason | SFML Deps |
|------|-----------|------|--------|-----------|
| `events/UserInputBitset.h` | `UserInputBitset` | struct | ❌ SFML input types | sf::Keyboard, sf::Mouse, sf::Event |
| `events/EventPacket.h` | `EventPacket` | struct | ❌ SFML + UUID + FlatBuffers | sf::Keyboard, sf::Mouse + uuid + events_generated.h |
| `events/EventHandler.h` | `EventHandler` | class | ❌ SFML + FlatBuffers | sf::Event + events_generated.h |
| `engine/EngineResources.h` | `EngineResources` | struct | ❌ SFML types | sf::RenderWindow, sf::Vector2i |
| `context/GameContext.h` | `GameContext` | struct | ❌ SFML types | sf::RenderWindow, sf::Vector2i |
| `scenes/SceneContext.h` | `SceneContext` | struct | ❌ SFML rendering | sf::RenderTexture, sf::RenderWindow, sf::Vector2i |
| `scenes/SceneResources.h` | `SceneResources` | struct | ❌ SFML rendering | sf::RenderTexture |
| `user_interface/UIElement.h` | `UIElement` | struct | ❌ SFML + FlatBuffers | sf::Vector2f, sf::RenderTexture + user_interface_generated.h |
| `user_interface/styles/Style.h` | `Style` | struct | ❌ SFML graphics | sf::Color, sf::Font |
| `user_interface/styles/ButtonStyle.h` | `ButtonStyle` | struct | ❌ Depends on Style.h | (via Style.h) |
| `user_interface/styles/PanelStyle.h` | `PanelStyle` | struct | ❌ Depends on Style.h | (via Style.h) |
| `user_interface/styles/DropDownButtonStyle.h` | `DropDownButtonStyle` | struct | ❌ Depends on Style.h | (via Style.h) |
| `user_interface/styles/DropDownItemStyle.h` | `DropDownItemStyle` | struct | ❌ Depends on Style.h | (via Style.h) |
| `user_interface/styles/DropDownListStyle.h` | `DropDownListStyle` | struct | ❌ Depends on Style.h | (via Style.h) |
| `user_interface/styles/DropDownContainerStyle.h` | `DropDownContainerStyle` | struct | ❌ Depends on Style.h | (via Style.h) |
| `components/Fragment.h` | `Fragment` | struct | ❌ SFML + FlatBuffers | sf::Transform, sf::Vector2f, sf::VertexArray + fragments_generated.h |
| `components/Joint.h` | `Joint` | struct | ❌ SFML graphics | sf::Vector2f, sf::Transform, sf::VertexArray |
| `display/Tile.h` | `Tile` | class | ❌ SFML + UUID | sf::Drawable + uuid |
| `display/Session.h` | `Session` | class | ❌ Depends on Tile.h | (via Tile.h) |
| `display/DisplayManager.h` | `DisplayManager` | class | ❌ SFML | sf::RenderWindow, sf::RenderTexture |
| `assets/AssetManager.h` | `AssetManager` | class | ❌ SFML + FlatBuffers | sf::Font, sf::Texture + asset_list_generated.h |
| `logic/Logic.h` | `Logic` | class | ❌ Depends on SceneContext | (via SceneContext → SFML) |
| `logic/ILogicStep.h` | `ILogicStep` | interface | ❌ Depends on SceneContext | (via SceneContext → SFML) |
| `scenes/Scene.h` | `Scene` | class | ❌ SFML + UUID | sf::RenderTexture + uuid |
| `scenes/SceneFactory.h` | `SceneFactory` | class | ❌ SFML + UUID | (via Scene.h) |
| `scenes/SceneManager.h` | `SceneManager` | class | ❌ SFML + UUID | (via Scene.h) |

**Analysis:**
- SFML is the rendering framework - it's foundational, not an implementation detail
- Abstracting SFML would require wrappers for all types (Vector2, Color, RenderWindow, etc.)
- Performance cost: double conversion (game code → wrapper → SFML → GPU)
- Complexity cost: maintaining parallel type system
- **Recommendation:** Accept SFML as foundational dependency

---

### FlatBuffers-Dependent Structures (10+)

| File | Structure | Type | Reason | FlatBuffers Deps |
|------|-----------|------|--------|------------------|
| `data_handlers/FlatbuffersDataLoader.h` | `FlatbuffersDataLoader` | class | ❌ Format-specific | flatbuffers library |
| `entity/FlatbuffersConfigurator.h` | `FlatbuffersConfigurator` | class | ❌ Format-specific | entities_generated.h |
| `data_providers/FlatbuffersEngineDataProvider.h` | `FlatbuffersEngineDataProvider` | class | ❌ Implements IEngineDataProvider | engine_data_generated.h |
| `data_providers/FlatbuffersSceneDataProvider.h` | `FlatbuffersSceneDataProvider` | class | ❌ Implements ISceneDataProvider | scene_data_generated.h |
| `data_providers/FlatbuffersAssetDataProvider.h` | `FlatbuffersAssetDataProvider` | class | ❌ Implements IAssetDataProvider | asset_list_generated.h |
| `data_providers/FlatbuffersFragmentDataProvider.h` | `FlatbuffersFragmentDataProvider` | class | ❌ Implements IFragmentDataProvider | fragments_generated.h |
| `data_providers/FlatbuffersSceneManagerDataProvider.h` | `FlatbuffersSceneManagerDataProvider` | class | ❌ Implements ISceneManagerDataProvider | scene_manager_generated.h |
| `data_providers/FlatbuffersSubscriberViewer.h` | `FlatbuffersSubscriberViewer` | class | ❌ Implements ISubscriberViewer | subscriber_generated.h |
| `configuration/FlatbuffersSaveDataProvider.h` | `FlatbuffersSaveDataProvider` | class | ❌ Format-specific | save_data_generated.h |
| `configuration/FlatbuffersUserPreferencesProvider.h` | `FlatbuffersUserPreferencesProvider` | class | ❌ Format-specific | preferences_generated.h |

**Analysis:**
- FlatBuffers is ONE serialization format
- I*Provider interfaces already provide the abstraction layer
- These are implementations of the interfaces (appropriate in implementation libraries)
- **Recommendation:** Keep FlatBuffers implementations in data_providers/data_handlers

---

### UUID-Dependent Structures (5)

| File | Structure | Type | Reason | UUID Usage |
|------|-----------|------|--------|------------|
| `scenes/SceneInfo.h` | `SceneInfo` | struct | ⚠️ UUID + FlatBuffers | uuids::uuid id + SceneType (FlatBuffers enum) |
| `events/EventPacket.h` | `EventPacket` | struct | ⚠️ UUID + SFML + FlatBuffers | uuids::uuid event_id, source_id |
| `events/Subscriber.h` | `Subscriber` | struct | ⚠️ Depends on EventPacket | (via EventPacket → UUID) |
| `scenes/Scene.h` | `Scene` | class | ⚠️ UUID + SFML | uuids::uuid id |
| `scenes/SceneManager.h` | `SceneManager` | class | ⚠️ UUID + SFML | Uses uuids::uuid for scene tracking |

**Analysis:**
- UUID is third-party library (stduuid)
- Provides globally unique identifiers
- **Alternative:** Replace with uint64_t + manual ID generation
- **Impact:** Would unblock SceneInfo and EventPacket (if combined with other refactorings)
- **Recommendation:** Consider if UUID is truly needed, or if simple uint64_t IDs suffice

---

### Complex Implementation Classes (14)

| File | Structure | Type | Reason |
|------|-----------|------|--------|
| `engine/Engine.h` | `Engine` | class | ⚠️ Complex core implementation |
| `engine/GameEngine.h` | `GameEngine` | class | ⚠️ Top-level game engine |
| `entity/EntityManager.h` | `EntityManager` | class | ⚠️ Complex ECS implementation |
| `entity/ArchetypeManager.h` | `ArchetypeManager` | class | ⚠️ Complex ECS implementation |
| `logic/UIRenderLogic.h` | `UIRenderLogic` | class | ⚠️ Concrete logic implementation |
| `logic/UICollisionLogic.h` | `UICollisionLogic` | class | ⚠️ Concrete logic implementation |
| `logic/UIActionLogic.h` | `UIActionLogic` | class | ⚠️ Concrete logic implementation |
| `logic/UIStateLogic.h` | `UIStateLogic` | class | ⚠️ Concrete logic implementation |
| `logic/CraftingRenderLogic.h` | `CraftingRenderLogic` | class | ⚠️ Concrete logic implementation |
| `logic/LogicFactory.h` | `LogicFactory` | class | ⚠️ Factory implementation |
| `scenes/CraftingScene.h` | `CraftingScene` | class | ⚠️ Concrete scene implementation |
| `scenes/TitleScene.h` | `TitleScene` | class | ⚠️ Concrete scene implementation |
| `user_interface/UIElementFactory.h` | `UIElementFactory` | class | ⚠️ Factory implementation |
| `user_interface/*Element.h` | Various UI elements | struct | ⚠️ Concrete element implementations |

**Analysis:**
- These are concrete implementations, not interfaces
- Appropriate complexity for implementation libraries
- Have dependencies on full game engine stack
- **Recommendation:** Keep in implementation libraries where they belong

---

## Summary Statistics

| Category | Count | Extractable? | Target Library |
|----------|-------|--------------|----------------|
| **Config/State Structs** | 8 | ✅ YES | interface_core |
| **Data Provider Interfaces** | 5 | ✅ YES | interface_data_providers |
| **SFML-Dependent** | 25+ | ❌ NO | Keep in impl libraries |
| **FlatBuffers-Dependent** | 10+ | ❌ NO | Keep in data_providers |
| **UUID-Dependent** | 5 | ⚠️ MAYBE | Consider refactoring |
| **Complex Implementations** | 14 | ❌ NO | Keep in impl libraries |
| **TOTAL ANALYZED** | 52+ | 13 extractable | - |

---

## Dependency Chain Examples

### Example 1: Clean Extraction (FailInfo)

```
FailInfo.h
└── std::string ✓ (standard library)

Result: ✅ Can move to interface_core
```

---

### Example 2: Blocked by SFML (EventPacket)

```
EventPacket.h
├── UserInputBitset.h
│   ├── SFML/Window/Event.hpp ❌
│   ├── SFML/Window/Keyboard.hpp ❌
│   └── SFML/Window/Mouse.hpp ❌
├── events_generated.h ❌ (FlatBuffers)
├── scene_types_generated.h ❌ (FlatBuffers)
└── uuid.h ❌ (third-party)

Result: ❌ Cannot move (3 external dependencies)
```

---

### Example 3: Blocked by Dependency Chain (Subscriber)

```
Subscriber.h
└── EventPacket.h
    └── (see Example 2 - multiple external deps)

Result: ❌ Blocked by EventPacket's dependencies
```

---

### Example 4: Interface with Clean Deps (IEngineDataProvider)

```
IEngineDataProvider.h
├── FailInfo.h → interface_core ✓
├── EngineConfig.h → interface_core ✓
└── EngineState.h
    └── Subscriber.h → EventPacket → ❌ (see Example 2)

Result: ⚠️ Mostly clean, but LoadEngineState() method returns EngineState which has Subscriber dependency
```

---

## Implementation Notes

### Phase 1: interface_core

**What to move:**
1. FailInfo.h (entire file)
2. EngineConfig.h (entire file)
3. SceneConfig.h (entire file)
4. SceneState.h (entire file)
5. SceneManagerConfig.h (entire file)
6. SceneManagerState.h (entire file)

**CMake:**
```cmake
add_library(interface_core INTERFACE)
target_include_directories(interface_core INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
# No target_link_libraries - only std library
```

**Migration effort:** 2-4 hours
**Risk level:** Very low

---

### Phase 2: interface_data_providers

**What to move:**
1. IAssetDataProvider.h
2. IFragmentDataProvider.h
3. ISceneManagerDataProvider.h
4. IEngineDataProvider.h
5. ISubscriberViewer.h

**CMake:**
```cmake
add_library(interface_data_providers INTERFACE)
target_include_directories(interface_data_providers INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(interface_data_providers INTERFACE
  interface_core
)
```

**Migration effort:** 4-6 hours
**Risk level:** Low

---

## Future Considerations

### Refactoring EventPacket (if needed)

**Current blockers:**
- SFML input types
- FlatBuffers enums
- UUID library

**Possible refactoring:**
```cpp
// Before
struct EventPacket {
  EventType m_event_type;        // FlatBuffers enum
  EventData m_event_data;        // Contains SFML types
  uuids::uuid event_id;          // UUID library
  uuids::uuid source_id;         // UUID library
  uint8_t event_lifetime;
};

// After (hypothetical)
struct EventPacket {
  EventTypeNative m_event_type;  // Native enum
  EventDataNative m_event_data;  // Abstracted types
  uint64_t event_id;             // Simple ID
  uint64_t source_id;            // Simple ID
  uint8_t event_lifetime;
};
```

**Effort:** 2-3 days significant refactoring
**Benefit:** Would unblock Subscriber, EngineState for interface extraction
**Recommendation:** Only if event system interface extraction is high priority

---

## Related Documents

- [Interface Library Analysis](INTERFACE_LIBRARY_ANALYSIS.md) - Full analysis
- [Interface Library Dependency Diagrams](INTERFACE_LIBRARY_DEPENDENCY_DIAGRAMS.md) - Visual guides
- [Interface Library Quick Reference](INTERFACE_LIBRARY_QUICK_REF.md) - Quick start guide

---

**Document Version:** 1.0  
**Last Updated:** December 7, 2025  
**Analysis Scope:** 52 structures across src/ (excluding flatbuffers_headers/)

