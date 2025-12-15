# Context Interface Simplification Proposal

**Date**: December 15, 2025  
**Type**: Architectural Exploration  
**Status**: Under Review

---

## Problem Statement

Currently, we have three related structs that share significant overlap:

1. **EngineResources** - Owns the actual objects (EventHandler, AssetManager, RenderWindow)
2. **GameContext** - References all EngineResources members individually
3. **SceneContext** - References all EngineResources members + scene-specific objects

The user asks: Could we simplify this with interfaces? Is there potential to collapse some of this down?

---

## Current Structure Analysis

### EngineResources (Ownership Container)

```cpp
// src/types/core/EngineResources.h
struct EngineResources {
  sf::RenderWindow game_window;
  EventHandler event_handler;
  sf::Vector2i mouse_position;
  size_t loop_number;
  AssetManager asset_manager;
};
```

**Role**: Owns the actual objects, lives on Engine

### GameContext (Reference Wrapper)

```cpp
// src/context/GameContext.h
struct GameContext {
  EngineResources &engine_resources;  // Reference to owner
  sf::RenderWindow &game_window;      // Unpacked reference
  EventHandler &event_handler;        // Unpacked reference
  sf::Vector2i &mouse_position;       // Unpacked reference
  size_t &loop_number;                // Unpacked reference
  AssetManager &asset_manager;        // Unpacked reference
};
```

**Constructor:**
```cpp
GameContext::GameContext(EngineResources &resources)
    : engine_resources(resources),
      game_window(resources.game_window),
      event_handler(resources.event_handler),
      loop_number(resources.loop_number),
      mouse_position(resources.mouse_position),
      asset_manager(resources.asset_manager) {}
```

**Role**: Lightweight reference wrapper, cheap to pass around

### SceneContext (Extended Reference Wrapper)

```cpp
// src/context/SceneContext.h
struct SceneContext {
  EntityMemoryPool &scene_entities;         // From EntityManager
  const unordered_map<ArchetypeID, Archetype> &archetypes;  // From EntityManager
  sf::RenderTexture &scene_texture;         // Scene-specific
  sf::RenderWindow &game_window;            // From EngineResources
  AssetManager &asset_manager;              // From EngineResources
  EventHandler &event_handler;              // From EngineResources
  sf::Vector2i &mouse_position;             // From EngineResources
};
```

**Constructor:**
```cpp
SceneContext::SceneContext(sf::RenderTexture &scene_texture,
                           EngineResources &engine_resources,
                           EntityManager &entity_manager)
    : scene_entities(entity_manager.GetEntityMemoryPool()),
      archetypes(entity_manager.GetArchetypeManager().GetArchetypes()),
      scene_texture(scene_texture),
      game_window(engine_resources.game_window),
      asset_manager(engine_resources.asset_manager),
      event_handler(engine_resources.event_handler),
      mouse_position(engine_resources.mouse_position) {}
```

**Role**: SceneContext = GameContext fields + scene-specific fields

---

## Observations

### 1. Redundancy in GameContext

**Current:** GameContext stores BOTH:
- `EngineResources &engine_resources` (reference to owner)
- Unpacked individual references to each member

**Why?** This provides convenient direct access without needing to go through `engine_resources.field`.

**Redundancy Example:**
```cpp
// Can access either way:
context.game_window         // Direct
context.engine_resources.game_window  // Via owner
```

### 2. SceneContext Duplicates GameContext Fields

**SceneContext contains:**
- All the same EngineResources fields as GameContext (game_window, asset_manager, etc.)
- PLUS scene-specific fields (scene_entities, archetypes, scene_texture)

**Inheritance relationship:**
SceneContext could conceptually inherit from GameContext, but currently doesn't.

### 3. EngineResources ≈ GameContext (minus engine_resources field)

If we remove the `engine_resources` reference from GameContext, it's essentially the same as EngineResources but with references instead of ownership.

---

## Proposed Solutions

### Option 1: Interface-Based Approach

**Create IEngineResources interface in types/ (Layer 1):**

```cpp
// src/types/core/IEngineResources.h (Layer 1)
#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>

namespace steamrot {

// Forward declarations
class EventHandler;
class AssetManager;

/////////////////////////////////////////////////
/// @brief Interface for accessing engine-level resources
///
/// Provides read-only access to engine resources without
/// exposing ownership details. Implemented by both
/// EngineResources and GameContext.
/////////////////////////////////////////////////
class IEngineResources {
public:
  virtual ~IEngineResources() = default;

  virtual sf::RenderWindow& GetGameWindow() = 0;
  virtual EventHandler& GetEventHandler() = 0;
  virtual AssetManager& GetAssetManager() = 0;
  virtual sf::Vector2i& GetMousePosition() = 0;
  virtual size_t& GetLoopNumber() = 0;
};

} // namespace steamrot
```

**EngineResources implements the interface:**

```cpp
// src/resources/EngineResources.h (Layer 2)
#pragma once
#include "IEngineResources.h"
#include "EventHandler.h"
#include "AssetManager.h"

namespace steamrot {

struct EngineResources : public IEngineResources {
  // Ownership (concrete objects)
  sf::RenderWindow game_window;
  EventHandler event_handler;
  AssetManager asset_manager;
  sf::Vector2i mouse_position{0, 0};
  size_t loop_number{1};

  // Interface implementation
  sf::RenderWindow& GetGameWindow() override { return game_window; }
  EventHandler& GetEventHandler() override { return event_handler; }
  AssetManager& GetAssetManager() override { return asset_manager; }
  sf::Vector2i& GetMousePosition() override { return mouse_position; }
  size_t& GetLoopNumber() override { return loop_number; }
};

} // namespace steamrot
```

**GameContext eliminated - just use IEngineResources&**

Everywhere we currently use `GameContext`, we instead use `IEngineResources&`:

```cpp
// Before:
class SceneManager {
  const GameContext &m_game_context;
public:
  SceneManager(const GameContext &game_context);
};

// After:
class SceneManager {
  IEngineResources &m_engine_resources;
public:
  SceneManager(IEngineResources &engine_resources);
};
```

**SceneContext simplified:**

```cpp
// src/context/SceneContext.h
struct SceneContext {
  // Scene-specific
  EntityMemoryPool &scene_entities;
  const unordered_map<ArchetypeID, Archetype> &archetypes;
  sf::RenderTexture &scene_texture;
  
  // Engine resources via interface
  IEngineResources &engine_resources;
  
  SceneContext(sf::RenderTexture &scene_texture,
               IEngineResources &engine_resources,
               EntityManager &entity_manager);
};
```

**Pros:**
- ✅ Eliminates GameContext (one less type)
- ✅ EngineResources can stay in types/ (implements interface from types/)
- ✅ Interface in Layer 1, implementation in Layer 2 (clean separation)
- ✅ SceneContext simplified (no redundant individual references)
- ✅ Easy to mock IEngineResources for testing

**Cons:**
- ❌ Virtual function overhead (minimal but present)
- ❌ Breaks all existing code using GameContext (large refactor)
- ❌ Syntax changes from `context.game_window` to `context.engine_resources.GetGameWindow()`
- ❌ More verbose at call sites

---

### Option 2: Collapse GameContext into EngineResources

**Remove GameContext entirely, use EngineResources& everywhere:**

```cpp
// Before:
class SceneManager {
  const GameContext &m_game_context;
public:
  SceneManager(const GameContext &game_context);
};

// After:
class SceneManager {
  EngineResources &m_engine_resources;
public:
  SceneManager(EngineResources &engine_resources);
};
```

**SceneContext takes EngineResources directly:**

```cpp
struct SceneContext {
  // Scene-specific
  EntityMemoryPool &scene_entities;
  const unordered_map<ArchetypeID, Archetype> &archetypes;
  sf::RenderTexture &scene_texture;
  
  // Engine resources (unpacked for convenience)
  sf::RenderWindow &game_window;
  EventHandler &event_handler;
  AssetManager &asset_manager;
  sf::Vector2i &mouse_position;
  // Note: loop_number rarely used in SceneContext, can be dropped
  
  SceneContext(sf::RenderTexture &scene_texture,
               EngineResources &engine_resources,
               EntityManager &entity_manager);
};
```

**Pros:**
- ✅ Eliminates GameContext (one less type)
- ✅ No virtual function overhead
- ✅ Simpler mental model (EngineResources is THE owner)
- ✅ SceneContext keeps convenient unpacked references

**Cons:**
- ❌ EngineResources still needs to move to Layer 2 (not solving the layering issue)
- ❌ EngineResources directly used in Layer 3 (Engine, SceneManager, etc.)
- ❌ Large refactor (all GameContext usage)

---

### Option 3: Keep Current Model, Fix Layering Only

**Move EngineResources to Layer 2 as analyzed, keep GameContext and SceneContext:**

```cpp
// src/resources/EngineResources.h (Layer 2)
struct EngineResources {
  sf::RenderWindow game_window;
  EventHandler event_handler;
  AssetManager asset_manager;
  sf::Vector2i mouse_position;
  size_t loop_number;
};

// src/context/GameContext.h (Layer 3)
struct GameContext {
  EngineResources &engine_resources;
  // ... unpacked references for convenience
};

// src/context/SceneContext.h (Layer 3)
struct SceneContext {
  // Scene fields + engine fields
};
```

**Pros:**
- ✅ Minimal code changes (just move one file + update includes)
- ✅ Fixes layering violation
- ✅ Keeps existing API (no breaking changes)
- ✅ Proven pattern (already works well)

**Cons:**
- ❌ Doesn't reduce redundancy
- ❌ GameContext still "wraps" EngineResources

---

## Recommendation

### Short Term: Option 3 (Keep Current Model, Fix Layering)

**Rationale:**
1. **Lowest risk** - Minimal code changes, no API breaks
2. **Solves the immediate problem** - Fixes layering violation
3. **Preserves working system** - Current model is functional
4. **Quick turnaround** - 2-3 hours vs. days/weeks

**Trade-off accepted:**
- Some redundancy between EngineResources and GameContext
- GameContext serves as convenience wrapper (acceptable pattern)

### Long Term: Consider Option 1 (Interface-Based)

**When to revisit:**
- During major refactor
- When adding save/load system (different resource implementations)
- When testability becomes priority (mocking)

**Prerequisites for Option 1:**
1. Complete current layering migration
2. Analyze performance impact of virtual calls
3. Measure refactor scope (how many files affected?)
4. Consider incremental migration strategy

---

## Analysis of Redundancy

### Is GameContext Truly Redundant?

**Current Role of GameContext:**

1. **Convenience Wrapper**: Provides direct field access
   ```cpp
   // With GameContext
   logic.DoSomething(context.game_window);
   
   // Without GameContext (direct EngineResources)
   logic.DoSomething(engine_resources.game_window);
   ```

2. **Stable Interface**: If EngineResources changes, GameContext can shield clients
   
3. **Semantic Distinction**: "Context for game-level operations" vs "Resource ownership"

### Could We Just Use EngineResources&?

**Yes, technically**, but:

**Current pattern separates concerns:**
- EngineResources = "I OWN these objects"
- GameContext = "I provide ACCESS to these objects"

**Without GameContext:**
- Systems that just need access also see ownership semantics
- Less clear at API level what's owned vs. borrowed

### SceneContext Redundancy

**SceneContext = GameContext + Scene Fields**

This redundancy exists because:
1. SceneContext needs engine resources
2. SceneContext needs scene resources
3. C++ doesn't have cheap struct composition

**Could SceneContext inherit from GameContext?**

```cpp
struct SceneContext : public GameContext {
  EntityMemoryPool &scene_entities;
  const unordered_map<ArchetypeID, Archetype> &archetypes;
  sf::RenderTexture &scene_texture;
};
```

**Problem:** GameContext is a struct with references. Inheritance with reference members is tricky:
- Constructor chaining is awkward
- Can't easily initialize base class reference members

---

## Cost-Benefit Analysis

### Option 1: Interface-Based

| Aspect | Cost | Benefit |
|--------|------|---------|
| Refactor Effort | HIGH (100+ files) | Eliminates GameContext |
| Performance | Small (virtual calls) | Better testability |
| Complexity | Medium (interfaces) | Cleaner architecture |
| Risk | HIGH (breaks all clients) | Future flexibility |

**Verdict:** High cost, moderate benefit. **Not worth it now**.

### Option 2: Collapse GameContext

| Aspect | Cost | Benefit |
|--------|------|---------|
| Refactor Effort | HIGH (100+ files) | Eliminates GameContext |
| Performance | None | Simpler model |
| Complexity | None (simpler) | Less indirection |
| Risk | HIGH (breaks all clients) | N/A |

**Verdict:** High cost, low benefit. **Not worth it**.

### Option 3: Keep Current, Fix Layering

| Aspect | Cost | Benefit |
|--------|------|---------|
| Refactor Effort | LOW (mechanical) | Fixes layering |
| Performance | None | Keeps working system |
| Complexity | None | Proven pattern |
| Risk | LOW | Clean architecture |

**Verdict:** Low cost, high benefit. **Recommended**.

---

## Conclusion

### Short Answer

**GameContext and EngineResources are NOT redundant** - they serve different purposes:
- **EngineResources** = Ownership container
- **GameContext** = Access/convenience wrapper

**However**, the unpacking of references in GameContext does create some duplication. This is an acceptable trade-off for convenience.

### Recommended Path Forward

1. **Immediate**: Implement Option 3 (move EngineResources to Layer 2)
   - Fixes layering violation
   - Minimal changes
   - Low risk

2. **Future**: Monitor for pain points
   - If testing becomes difficult → consider Option 1 (interfaces)
   - If redundancy becomes costly → revisit
   - If performance matters → measure virtual call overhead

3. **Don't**: Refactor just for the sake of reducing types
   - Current pattern works well
   - Breaking changes are expensive
   - Focus on solving actual problems

---

## Alternative: Hybrid Approach

**If we want SOME simplification without full interfaces:**

### Keep GameContext as-is, simplify SceneContext

**Problem:** SceneContext duplicates all GameContext fields

**Solution:** SceneContext contains GameContext

```cpp
struct SceneContext {
  // Game-level (via reference to GameContext)
  const GameContext &game_context;
  
  // Scene-specific
  EntityMemoryPool &scene_entities;
  const unordered_map<ArchetypeID, Archetype> &archetypes;
  sf::RenderTexture &scene_texture;
  
  // Constructor
  SceneContext(const GameContext &game_context,
               sf::RenderTexture &scene_texture,
               EntityManager &entity_manager)
      : game_context(game_context),
        scene_entities(entity_manager.GetEntityMemoryPool()),
        archetypes(entity_manager.GetArchetypeManager().GetArchetypes()),
        scene_texture(scene_texture) {}
};
```

**Usage changes:**
```cpp
// Before:
logic.DoSomething(scene_context.game_window);

// After:
logic.DoSomething(scene_context.game_context.game_window);
```

**Pros:**
- ✅ Eliminates field duplication in SceneContext
- ✅ Clear relationship: SceneContext HAS-A GameContext
- ✅ Smaller refactor (only SceneContext clients)

**Cons:**
- ❌ Slightly more verbose at call sites
- ❌ Still requires refactoring all Logic classes

**Verdict:** Worth considering as middle ground.

---

## Summary Table

| Solution | Complexity | Breaking Changes | Layering Fix | Reduces Types |
|----------|-----------|------------------|--------------|---------------|
| **Option 1: Interfaces** | High | Yes (all clients) | Yes | Yes (drops GameContext) |
| **Option 2: Collapse** | Medium | Yes (all clients) | No | Yes (drops GameContext) |
| **Option 3: Keep Current** | Low | Minimal | Yes | No |
| **Hybrid: Simplify SceneContext** | Medium | Yes (Logic classes) | Yes | No |

**Recommendation:** **Option 3** for immediate fix, revisit **Hybrid** or **Option 1** in future.

---

**Document Version**: 1.0  
**Last Updated**: December 15, 2025  
**Status**: Analysis complete, awaiting decision
