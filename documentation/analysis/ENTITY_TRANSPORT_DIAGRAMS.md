# Entity Transport Architecture Diagrams

This document provides visual representations of the entity transport architecture.

**Key Constraint**: Must support lazy loading to avoid copying large entity data (50k-100k entities predicted).

---

## Current Architecture Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                    Current Architecture                         │
│                   (3 Abstractions)                              │
└─────────────────────────────────────────────────────────────────┘

┌──────────────────────┐
│  IEntityImporter     │  Interface (runtime polymorphism)
│  ──────────────      │
│  + ImportEntities()  │  Purpose: Format-agnostic import
│                      │  Issue: Only 1 implementation exists
└──────────────────────┘
         ▲
         │ implements
         │
┌─────────────────────────────┐
│ FlatbuffersEntityImporter   │  Thin wrapper class
│ ───────────────────────     │
│ - m_event_handler           │  Issue: Just delegates to configurator
│ - m_entity_collection       │
│ + ImportEntities()          │
└─────────────────────────────┘
         │
         │ wraps
         │
┌─────────────────────────────┐
│ EntityCollectionFbs         │  FlatBuffers data
│ (FlatBuffers generated)     │
└─────────────────────────────┘


┌──────────────────────┐
│  IEntityExporter     │  Interface (unused!)
│  ──────────────      │
│  + ExportEntities()  │  Purpose: Serialize entities
│                      │  Issue: Zero implementations
└──────────────────────┘
         │
         ✗ (no implementations)


┌─────────────────────────────────────────────┐
│  EntityTransportVariant                     │  Runtime variant (4 states)
│  ─────────────────────                      │
│  std::variant<                              │  Issue: Mixing compile-time
│    std::monostate,                          │        scenarios at runtime
│    std::unique_ptr<IEntityImporter>,        │
│    std::shared_ptr<EntityMemoryPool>,       │
│    EntityMemoryPool                         │
│  >                                          │
└─────────────────────────────────────────────┘
```

---

## Data Flow: Production Scene Loading (Current)

```
┌─────────────────────┐
│  scene_data.json    │  JSON file on disk
└──────────┬──────────┘
           │
           ▼
┌─────────────────────────────┐
│  FlatbuffersDataLoader      │  Layer 1: File loading
└──────────┬──────────────────┘
           │
           ▼
┌─────────────────────────────┐
│  EntityCollectionFbs        │  Layer 2: FlatBuffers data
└──────────┬──────────────────┘
           │
           ▼
┌─────────────────────────────┐
│ FlatbuffersEntityImporter   │  Layer 3: Interface wrapper
│ (wraps EntityCollectionFbs) │          (unnecessary!)
└──────────┬──────────────────┘
           │
           ▼
┌─────────────────────────────┐
│  EntityTransportVariant     │  Layer 4: Variant storage
│  (holds unique_ptr)         │
└──────────┬──────────────────┘
           │
           ▼
┌─────────────────────────────┐
│  SceneFactory               │  Layer 5: Extraction + validation
│  ::ImportEntities()         │          (type checking, null checking)
└──────────┬──────────────────┘
           │
           ▼
┌──────────────────────────────┐
│ FlatbuffersEntityConfigurator│ Layer 6: Actual work happens here!
│ ::ConfigureEntityMemoryPool()│         (this does the real job)
└──────────┬───────────────────┘
           │
           ▼
┌─────────────────────────────┐
│   EntityMemoryPool          │  Final result
│   (Scene entities)          │
└─────────────────────────────┘

Total: 6 layers for what should be 3 layers
```

---

## Data Flow: Scene State Capture (Current)

```
┌─────────────────────────────┐
│   EntityMemoryPool          │  Scene's current state
│   (in Scene)                │
└──────────┬──────────────────┘
           │
           │ copy + wrap in shared_ptr
           │
           ▼
┌─────────────────────────────┐
│  EntityTransportVariant     │  Wrapped in variant
│  (std::shared_ptr<EMP>)     │
└──────────┬──────────────────┘
           │
           ▼
┌─────────────────────────────┐
│  SceneData                  │  Stored for later
└─────────────────────────────┘

Issue: Ownership unclear - why shared_ptr?
```

---

## Proposed Architecture: Option B (Simplified Variant - Recommended)

```
┌─────────────────────────────────────────────────────────────────┐
│                    Proposed Architecture                        │
│         (1 Abstraction - Variant with Lazy Loading)             │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────┐
│  EntityLazyLoadData (struct)    │  Plain data (no polymorphism)
│  ────────────                   │
│  - flatbuffers_data: ptr        │  Only FlatBuffers pointer
└─────────────────────────────────┘  (preserves lazy loading!)
                                     EventHandler from scene context

┌─────────────────────────────────────────────┐
│  EntitySource (variant)                     │
│  ─────────────                              │
│  std::variant<                              │
│    std::monostate,        // No entities    │
│    EntityMemoryPool,      // Direct pool    │
│    EntityLazyLoadData     // Lazy load      │
│  >                                          │
└─────────────────────────────────────────────┘
           │
           │ used in
           ▼
┌─────────────────────────────┐
│  SceneData                  │
│  ─────────                  │
│  - entity_source: variant   │
└─────────────────────────────┘

Usage:
  std::visit with compile-time dispatch
  (no virtual functions, preserves lazy loading!)
  EventHandler provided at usage point from scene context

Use cases mapped to variant states:
   ┌────────────┬─────────────┬──────────────┐
   │            │             │              │
Production    Testing      Testing      Serialization
(lazy load)   (empty)   (pre-config)     (snapshot)
   │             │            │              │
   ▼             ▼            ▼              ▼
EntityLazy    monostate   EntityPool    EntityPool
LoadData                  (test_pool)   (copy)

Key: Lazy loading preserved for production (no copying!)
     EventHandler not in variant (from scene context)
```

---

```
┌─────────────────────────────────────────────────────────────────┐
│                    Proposed Architecture                        │
│                   (Zero Abstractions!)                          │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────┐
│  SceneData                  │
│  ─────────                  │
│  - scene_info               │
│  - scene_resources_config   │
│  - scene_asset_config       │
│  - entity_pool: optional    │  Simple! May or may not exist
└─────────────────────────────┘
           ▲
           │
   ┌───────┴────────┬────────────────┬─────────────┐
   │                │                │             │
   │                │                │             │
Production      Testing         Testing       Serialization
(file)          (empty)      (pre-config)     (snapshot)
   │                │                │             │
   ▼                ▼                ▼             ▼
entity_pool      nullopt         test_pool      scene_pool
= pool                           = pool          = snapshot
```

---

## Data Flow: Option A (Simplified)

### Production Scene Loading

```
┌─────────────────────┐
│  scene_data.json    │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────────────┐
│  FlatbuffersDataLoader      │  Layer 1: File loading
└──────────┬──────────────────┘
           │
           ▼
┌─────────────────────────────┐
│  EntityCollectionFbs        │  Layer 2: FlatBuffers data
└──────────┬──────────────────┘
           │
           ▼
┌──────────────────────────────┐
│ FlatbuffersEntityConfigurator│ Layer 3: Configure directly!
│ ::ConfigureEntityMemoryPool()│         (no wrappers needed)
└──────────┬───────────────────┘
           │
           ▼
┌─────────────────────────────┐
│   EntityMemoryPool          │  
│   ↓                         │
│   stored in                 │
│   scene_data.entity_pool    │
└─────────────────────────────┘

Total: 3 layers (exactly what's needed)
Reduction: 6 → 3 layers (50% simpler!)
```

### Testing

```
┌─────────────────────────────┐
│  Test Setup                 │
│  ──────────                 │
│  EntityMemoryPool pool;     │
│  // configure for test      │
└──────────┬──────────────────┘
           │
           │ direct assignment
           │
           ▼
┌─────────────────────────────┐
│  scene_data.entity_pool     │
│  = std::move(pool)          │
└─────────────────────────────┘

Simple! No variant, no interfaces, no wrappers
```

### Scene Capture

```
┌─────────────────────────────┐
│   Scene::GetEntityMemory()  │
└──────────┬──────────────────┘
           │
           │ direct copy
           │
           ▼
┌─────────────────────────────┐
│  scene_data.entity_pool     │
│  = scene.GetEntityMemory()  │
└─────────────────────────────┘

Clear ownership: value semantics
```

---

## Proposed Architecture: Option B (Simplified Variant)

```
┌─────────────────────────────────────────────────────────────────┐
│                Alternative: Option B                            │
│                (1 Abstraction - Variant)                        │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────┐
│  EntityData (struct)            │  Plain data (no polymorphism)
│  ────────────                   │
│  - flatbuffers_data: ptr        │  Only FlatBuffers pointer
└─────────────────────────────────┘  EventHandler from scene context

┌─────────────────────────────────────────────┐
│  EntitySource (variant)                     │
│  ─────────────                              │
│  std::variant<                              │
│    std::monostate,        // No entities    │
│    EntityMemoryPool,      // Direct pool    │
│    EntityData             // Lazy load      │
│  >                                          │
└─────────────────────────────────────────────┘
           │
           │ used in
           ▼
┌─────────────────────────────┐
│  SceneData                  │
│  ─────────                  │
│  - entity_source: variant   │
└─────────────────────────────┘

Usage:
  std::visit with compile-time dispatch
  (no virtual functions!)
```

---

## Comparison: Current vs. Option A vs. Option B

```
┌──────────────────┬─────────────────┬──────────────┬──────────────┐
│                  │    CURRENT      │   OPTION A   │   OPTION B   │
├──────────────────┼─────────────────┼──────────────┼──────────────┤
│ Abstractions     │       3         │      0       │      1       │
│                  │ (Importer,      │   (none!)    │  (variant)   │
│                  │  Exporter,      │              │              │
│                  │  Variant)       │              │              │
├──────────────────┼─────────────────┼──────────────┼──────────────┤
│ Polymorphism     │  Virtual funcs  │     None     │  std::visit  │
│                  │  (runtime)      │              │ (compile-time│
├──────────────────┼─────────────────┼──────────────┼──────────────┤
│ Heap Allocs      │     High        │     Low      │     Low      │
│                  │ (unique_ptr)    │              │              │
├──────────────────┼─────────────────┼──────────────┼──────────────┤
│ Indirection      │   6 layers      │   3 layers   │  3-4 layers  │
│                  │                 │   (50% less!)│              │
├──────────────────┼─────────────────┼──────────────┼──────────────┤
│ Code Clarity     │      Low        │    High      │   Medium     │
│                  │ (complex)       │  (simple!)   │              │
├──────────────────┼─────────────────┼──────────────┼──────────────┤
│ Test Complexity  │     High        │     Low      │   Medium     │
│                  │ (variant +      │  (optional   │  (variant    │
│                  │  interface)     │   compare)   │   compare)   │
└──────────────────┴─────────────────┴──────────────┴──────────────┘
```

---

## Ownership Models Comparison

### Current (Mixed Ownership)

```
EntityTransportVariant
    │
    ├─ std::monostate              // No ownership
    ├─ unique_ptr<IEntityImporter> // Exclusive ownership (lazy loader)
    ├─ shared_ptr<EntityMemoryPool>// Shared ownership (why?)
    └─ EntityMemoryPool            // Value ownership (expensive copy)

Problem: 4 different ownership models!
```

### Option A (Clear Ownership)

```
std::optional<EntityMemoryPool>
    │
    ├─ nullopt        // No entities
    └─ value          // Owned value

Simple: Either you have it or you don't!
```

### Option B (Clear Ownership)

```
std::variant<std::monostate, EntityMemoryPool, EntityData>
    │
    ├─ monostate      // No entities
    ├─ EntityMemoryPool // Owned value
    └─ EntityData      // Non-owning references (lazy)

Clear: 3 distinct states with clear semantics
```

---

## Performance Impact

### Current Performance

```
Scene Loading Timeline:
┌────┬────┬────┬────┬────┬────┐
│ 1  │ 2  │ 3  │ 4  │ 5  │ 6  │  6 layers
└────┴────┴────┴────┴────┴────┘
  │    │    │    │    │    │
  │    │    │    │    │    └─ Configure (actual work)
  │    │    │    │    └────── Extract/validate
  │    │    │    └─────────── Variant wrapper
  │    │    └──────────────── Interface wrapper (virtual call)
  │    └───────────────────── FlatBuffers data
  └────────────────────────── File load

Overhead: Layers 3, 4, 5 are unnecessary!
```

### Option A Performance

```
Scene Loading Timeline:
┌────┬────┬────┐
│ 1  │ 2  │ 3  │  3 layers
└────┴────┴────┘
  │    │    │
  │    │    └─ Configure (actual work)
  │    └────── FlatBuffers data
  └─────────── File load

Improvement: 50% fewer layers
Estimated: 3-5% faster (overhead removed)
```

---

## Testing Complexity

### Current Test Assertion

```cpp
// Complex variant + interface handling
TEST_CASE("Test entity transport") {
  // Setup
  EntityTransportVariant expected = /* ... */;
  EntityTransportVariant actual = /* ... */;
  
  // Must handle:
  // 1. Variant type checking
  if (expected.index() != actual.index()) FAIL();
  
  // 2. Handle each variant state
  if (holds_alternative<unique_ptr<IEntityImporter>>(expected)) {
    // Can't compare interfaces!
    // Must accept always different
  }
  else if (holds_alternative<shared_ptr<EntityMemoryPool>>(expected)) {
    // Check pointer validity
    // Compare pointed-to values
  }
  // ... 4 different branches!
}
```

### Option A Test Assertion

```cpp
// Simple optional comparison
TEST_CASE("Test entity pool") {
  // Setup
  std::optional<EntityMemoryPool> expected = /* ... */;
  std::optional<EntityMemoryPool> actual = /* ... */;
  
  // One assertion!
  REQUIRE(actual == expected);
}
```

**Reduction**: ~20 lines → 1 line

---

## Migration Path Visualization

```
┌─────────────────────────────────────────────────────────────┐
│  Phase 1: Remove IEntityExporter (Immediate) ✓              │
│  Risk: Zero │ Impact: Zero │ Effort: Minimal                │
└─────────────────────────────────────────────────────────────┘
                    │
                    ▼
┌─────────────────────────────────────────────────────────────┐
│  Phase 2: Remove IEntityImporter (Short-term)               │
│  Risk: Low │ Impact: Medium │ Effort: Low                   │
│                                                              │
│  - Delete FlatbuffersEntityImporter                         │
│  - Use FlatbuffersEntityConfigurator directly               │
│  - Simplify EntityTransportVariant (4 states → 3 states)    │
└─────────────────────────────────────────────────────────────┘
                    │
                    ▼
┌─────────────────────────────────────────────────────────────┐
│  Phase 3: Simplify Variant (Medium-term)                    │
│  Risk: Medium │ Impact: High │ Effort: Medium               │
│                                                              │
│  - Evaluate: Is variant still needed?                       │
│  - Consider: std::optional<EntityMemoryPool>                │
│  - Update: All variant handling code                        │
└─────────────────────────────────────────────────────────────┘
                    │
                    ▼
┌─────────────────────────────────────────────────────────────┐
│  Phase 4: Lazy Loading Evaluation (Long-term)               │
│  Risk: Low │ Impact: TBD │ Effort: TBD                      │
│                                                              │
│  - Measure: Is eager loading a problem?                     │
│  - If yes: Add std::function wrapper                        │
│  - If no: Keep simple design                                │
└─────────────────────────────────────────────────────────────┘
```

---

## Recommended Reading Order

1. **Start here**: [Quick Reference](./ENTITY_TRANSPORT_QUICK_REFERENCE.md)
2. **This document**: Visual overview and diagrams
3. **Deep dive**: [Full Analysis](./ENTITY_TRANSPORT_ARCHITECTURE_ANALYSIS.md)

---

**Document Version**: 1.0  
**Date**: 2026-01-29  
**Format**: ASCII diagrams for version control friendly diffs
