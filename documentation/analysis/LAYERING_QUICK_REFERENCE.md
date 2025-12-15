# SteamRot Layering Quick Reference

**Date**: December 15, 2025  
**Purpose**: Quick lookup guide for layering rules and package organization

---

## TL;DR

**Problem**: EngineResources lives in types/ (Layer 1) but contains EventHandler/AssetManager (Layer 2)

**Solution**: Move EngineResources to new resources/ package (Layer 2)

**Result**: Clean three-layer architecture with no violations

---

## Three-Layer Architecture

```
Layer 1: Data + Interfaces (types, interfaces, components)
   ↑ depends on
Layer 2: Logic + Services (events, assets, logic, entity, resources)
   ↑ depends on
Layer 3: Orchestration (context, scenes, display, engine)
```

---

## Package Layers

### Layer 1: Data + Interfaces (Zero Dependencies)

| Package | Purpose | Can Include |
|---------|---------|-------------|
| **types/core/** | Config structs, enums | Only POD types, no complex objects |
| **types/events/** | Event data structures | Only data, no logic |
| **types/user_interface/** | UIElement data | Data only (no drawing methods) |
| **interfaces/** | Abstract interfaces | Pure virtual, no implementation |
| **components/** | Component data | Data + GetComponentRegisterIndex() only |

**Rule**: NO includes from Layer 2 or 3

**Examples**:
- ✅ `EngineResourcesConfig` - Pure data struct
- ✅ `IEventHandler` - Pure interface
- ✅ `SceneConfig` - Configuration data
- ❌ `EngineResources` - Contains Level 2 objects (moving to resources/)

### Layer 2: Logic + Services

| Package | Purpose | Can Include |
|---------|---------|-------------|
| **events/** | EventHandler implementation | Layer 1 only |
| **assets/** | AssetManager implementation | Layer 1 only |
| **data_providers/** | Data loading | Layer 1 + interfaces |
| **entity/** | Entity/archetype management | Layer 1 + components |
| **logic/** | Game logic classes | Layer 1 + entity |
| **resources/** | Resource ownership (NEW) | Layer 1 + events + assets |

**Rule**: Depends ONLY on Layer 1

**Examples**:
- ✅ `EventHandler` implements `IEventHandler`
- ✅ `AssetManager` implements `IAssetManager`
- ✅ `EngineResources` (MOVING HERE) - Owns EventHandler/AssetManager
- ✅ `FlatbuffersEngineDataProvider` - Implements provider interface

### Layer 3: Orchestration

| Package | Purpose | Can Include |
|---------|---------|-------------|
| **context/** | Reference wrappers | Layer 1 + 2 |
| **scenes/** | Scene management | Layer 1 + 2 + context |
| **display/** | Display management | Layer 1 + 2 + scenes |
| **engine/** | Top-level orchestration | Everything |

**Rule**: Depends on Layer 1 and 2

**Examples**:
- ✅ `GameContext` - References EngineResources
- ✅ `SceneManager` - Orchestrates scenes
- ✅ `Engine` - Top-level, constructs everything

---

## The EngineResources Problem

### Current State (WRONG)

```
src/types/core/EngineResources.h  (Layer 1)
  ├── includes EventHandler (Layer 2) ❌ VIOLATION
  └── includes AssetManager (Layer 2) ❌ VIOLATION
```

### Fixed State (CORRECT)

```
src/resources/EngineResources.h    (Layer 2)
  ├── includes EventHandler (Layer 2) ✅ OK
  └── includes AssetManager (Layer 2) ✅ OK
```

---

## Migration Checklist

### Phase 1: Create Resources Package

- [ ] Create `src/resources/` directory
- [ ] Create `src/resources/CMakeLists.txt`
- [ ] Move `EngineResources.h` from `types/core/` to `resources/`
- [ ] Update all `#include "EngineResources.h"` paths
- [ ] Update CMakeLists.txt dependencies
- [ ] Build and verify

### Phase 2: Update Documentation

- [ ] Update architecture diagrams
- [ ] Update README.md
- [ ] Update RESOURCE_CONTEXT_ARCHITECTURE.md
- [ ] Create/update layering guides

### Phase 3: Verify No Regressions

- [ ] Run full test suite
- [ ] Check for include path errors
- [ ] Verify contexts still work
- [ ] Test GameEngine and TestEngine

---

## Common Patterns

### Pattern 1: Config in Layer 1, Runtime in Layer 2

**Config Data** (Layer 1):
```cpp
// types/core/EngineResourcesConfig.h
struct EngineResourcesConfig {
  uint32_t window_width;
  std::string window_title;
  // Only POD types
};
```

**Runtime Container** (Layer 2):
```cpp
// resources/EngineResources.h
struct EngineResources {
  sf::RenderWindow game_window;
  EventHandler event_handler;     // Complex object - OK in Layer 2
  AssetManager asset_manager;     // Complex object - OK in Layer 2
};
```

### Pattern 2: Interface in Layer 1, Implementation in Layer 2

**Interface** (Layer 1):
```cpp
// interfaces/IEventHandler.h
class IEventHandler {
public:
  virtual void AddEvent(const EventPacket &event) = 0;
  virtual ~IEventHandler() = default;
};
```

**Implementation** (Layer 2):
```cpp
// events/EventHandler.h
class EventHandler : public IEventHandler {
public:
  void AddEvent(const EventPacket &event) override;
private:
  EventBus m_global_event_bus;  // Complex internals
};
```

### Pattern 3: Ownership in Layer 2, References in Layer 3

**Owner** (Layer 2):
```cpp
// resources/EngineResources.h
struct EngineResources {
  EventHandler event_handler;  // OWNS the object
};
```

**Reference Wrapper** (Layer 3):
```cpp
// context/GameContext.h
struct GameContext {
  GameContext(EngineResources &resources)
    : event_handler(resources.event_handler) {}  // REFERENCES the object
  
  EventHandler &event_handler;  // Reference, not ownership
};
```

---

## Dependency Rules

### ✅ ALLOWED Dependencies

| From Layer | To Layer | Example |
|-----------|----------|---------|
| Layer 2 | Layer 1 | EventHandler includes EventPacket |
| Layer 3 | Layer 2 | GameContext includes EngineResources |
| Layer 3 | Layer 1 | Engine includes EngineResourcesConfig |

### ❌ FORBIDDEN Dependencies

| From Layer | To Layer | Example | Fix |
|-----------|----------|---------|-----|
| Layer 1 | Layer 2 | EngineResources includes EventHandler | Move EngineResources to Layer 2 |
| Layer 1 | Layer 3 | Component includes Scene | Never do this |
| Layer 2 | Layer 3 | Logic includes Engine | Pass via constructor |

---

## File Organization

```
src/
├── types/              # Layer 1: Data structures
│   ├── core/           # POD structs, enums, config
│   ├── events/         # Event data
│   └── user_interface/ # UI data structures
│
├── interfaces/         # Layer 1: Abstract interfaces
│   ├── IEventHandler.h
│   └── IAssetManager.h
│
├── components/         # Layer 1: Component data
│   ├── CMeta.h
│   └── CUserInterface.h
│
├── events/             # Layer 2: Event system
│   └── EventHandler.h/cpp
│
├── assets/             # Layer 2: Asset management
│   └── AssetManager.h/cpp
│
├── resources/          # Layer 2: Resource ownership (NEW)
│   └── EngineResources.h
│
├── entity/             # Layer 2: Entity management
│   └── EntityManager.h/cpp
│
├── logic/              # Layer 2: Game logic
│   └── Logic classes
│
├── data_providers/     # Layer 2: Data loading
│   └── Provider implementations
│
├── context/            # Layer 3: Reference wrappers
│   ├── GameContext.h/cpp
│   └── SceneContext.h/cpp
│
├── scenes/             # Layer 3: Scene management
│   └── SceneManager.h/cpp
│
├── display/            # Layer 3: Display
│   └── DisplayManager.h/cpp
│
└── engine/             # Layer 3: Top orchestration
    ├── Engine.h/cpp
    ├── GameEngine.h/cpp
    └── TestEngine.h/cpp
```

---

## When Adding New Code

### Adding a New Data Structure

**Question**: Where does it go?

**Answer**: 
- If it's POD (config, enums, simple structs) → `types/core/`
- If it owns complex objects → New package in Layer 2 (like resources/)
- If it's pure interface → `interfaces/`

### Adding a New Service Class

**Question**: Where does it go?

**Answer**:
- Create new package in Layer 2 (events/, assets/, etc.)
- Implement an interface from `interfaces/`
- Only depend on Layer 1

### Adding a New System

**Question**: Where does it go?

**Answer**:
- Create new package in Layer 3 (scenes/, display/, etc.)
- Can depend on Layer 1 and 2
- Receives services via constructor injection

---

## Testing By Layer

### Layer 1: Zero Mocking

```cpp
TEST_CASE("EngineResourcesConfig is default constructible") {
  steamrot::EngineResourcesConfig config;  // No mocks needed
  REQUIRE(config.window_width == 800);
}
```

### Layer 2: Interface Mocking

```cpp
TEST_CASE("EventHandler processes events") {
  MockEventHandler handler;  // Mock IEventHandler
  handler.AddEvent(event);
  REQUIRE(handler.GetEventCount() == 1);
}
```

### Layer 3: Service Mocking

```cpp
TEST_CASE("Engine starts up successfully") {
  MockEngineResources resources;  // Mock Layer 2
  Engine engine(resources);
  REQUIRE_NOTHROW(engine.StartUp());
}
```

---

## Red Flags 🚩

**Watch out for these anti-patterns**:

### 🚩 Red Flag 1: Layer 1 includes Layer 2

```cpp
// types/core/MyConfig.h
#include "EventHandler.h"  // ❌ WRONG - Layer 2 in Layer 1
```

**Fix**: Move MyConfig to Layer 2, or make EventHandler an interface

### 🚩 Red Flag 2: Circular dependencies

```cpp
// Package A depends on Package B
// Package B depends on Package A  // ❌ WRONG
```

**Fix**: Extract common types to Layer 1, or use interfaces

### 🚩 Red Flag 3: Lower layer constructs upper layer

```cpp
// In Layer 2:
Scene scene;  // ❌ WRONG - Scene is Layer 3
```

**Fix**: Receive via constructor, let Layer 3 construct

---

## Questions?

### Q: Can Layer 2 packages depend on each other?

**A**: Yes, but be careful. Prefer unidirectional dependencies. If A and B both need something, extract it to Layer 1.

### Q: What if I need a complex type in Layer 1?

**A**: You probably don't. If you truly own it, move to Layer 2. If you just reference it, use an interface.

### Q: Can I have sub-layers (2.5, etc.)?

**A**: Informally yes, but officially no. Keep it simple: 1, 2, or 3.

### Q: What about external dependencies (SFML, etc.)?

**A**: External deps can be used anywhere. The layer rules apply only to internal packages.

---

## Summary

| Concept | Key Rule |
|---------|----------|
| **Layer 1** | Zero dependencies on other layers |
| **Layer 2** | Depends only on Layer 1 |
| **Layer 3** | Depends on Layer 1 + 2 |
| **EngineResources** | Moving from types/ to resources/ (Layer 2) |
| **Ownership** | Layer 2+ owns, Layer 3 references |
| **Construction** | Top-down (Engine constructs everything) |
| **Testing** | Layer 1 needs no mocks, Layer 2+ uses interfaces |

---

**Document Version**: 1.0  
**Last Updated**: December 15, 2025
