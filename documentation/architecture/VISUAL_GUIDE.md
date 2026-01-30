# Entity Configurator Architecture - Visual Guide

This document provides visual representations of the EntityConfigurator design.

---

## High-Level Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                          SceneData                              │
│  (Complete Configuration Package - DTO Pattern)                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────────┐  ┌──────────────────┐  ┌──────────────┐  │
│  │   SceneInfo     │  │ SceneResources   │  │ AssetConfig  │  │
│  │   (metadata)    │  │   Config         │  │              │  │
│  └─────────────────┘  └──────────────────┘  └──────────────┘  │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │        EntityTransportVariant (Type-Safe Union)         │   │
│  ├─────────────────────────────────────────────────────────┤   │
│  │  • std::monostate (empty)                              │   │
│  │  • EntityMemoryPool (direct - testing)                 │   │
│  │  • shared_ptr<EntityMemoryPool> (shared)               │   │
│  │  • const EntityCollectionFbs* (FlatBuffers - prod)     │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │   std::unique_ptr<IEntityConfigurator>                  │   │
│  │   (Strategy Pattern - knows how to process the data)    │   │
│  │                                                          │   │
│  │   ┌─────────────────────────────────────────────┐       │   │
│  │   │   FlatbuffersEntityConfigurator             │       │   │
│  │   │   • Reads EntityCollectionFbs*              │       │   │
│  │   │   • Populates EntityMemoryPool              │       │   │
│  │   │   • Registers event subscribers             │       │   │
│  │   │                                             │       │   │
│  │   │   Requires: EventHandler& (injected)        │       │   │
│  │   └─────────────────────────────────────────────┘       │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## Scene Creation Data Flow

```
┌──────────────────────────────────────────────────────────────────┐
│ Step 1: Scene Data Provider Creates SceneData                   │
└──────────────────────────────────────────────────────────────────┘
                              │
                              ▼
        ┌─────────────────────────────────────────┐
        │ FlatbuffersSceneDataProvider            │
        │  .CreateSceneData(scene_type)           │
        └─────────────────────────────────────────┘
                              │
                              │ Loads FlatBuffers from disk
                              ▼
        ┌─────────────────────────────────────────┐
        │ Creates SceneData:                      │
        │  • entity_transport = FlatBuffers ptr   │
        │  • entity_configurator = new            │
        │    FlatbuffersEntityConfigurator(       │
        │      event_handler)                     │
        └─────────────────────────────────────────┘
                              │
                              ▼
┌──────────────────────────────────────────────────────────────────┐
│ Step 2: Scene Factory Uses SceneData                            │
└──────────────────────────────────────────────────────────────────┘
                              │
                              ▼
        ┌─────────────────────────────────────────┐
        │ SceneFactory                            │
        │  .CreateSceneFromSceneData(scene_data)  │
        └─────────────────────────────────────────┘
                              │
                              ▼
        ┌─────────────────────────────────────────┐
        │ scene_data.entity_configurator->        │
        │   ConfigureEntityMemoryPoolFromSource(  │
        │     scene.GetEntityMemoryPool(),        │
        │     scene_data.entity_transport)        │
        └─────────────────────────────────────────┘
                              │
                              ▼
┌──────────────────────────────────────────────────────────────────┐
│ Step 3: Configurator Processes Data                             │
└──────────────────────────────────────────────────────────────────┘
                              │
                              ▼
        ┌─────────────────────────────────────────┐
        │ Phase 1: Configure First-Layer          │
        │  • CMeta, CUserInterface,               │
        │    CGrimoireMachina, CMachinaForm       │
        │  • Create UI elements                   │
        │  • Register event subscribers ──────────┼──> EventHandler
        └─────────────────────────────────────────┘
                              │
                              ▼
        ┌─────────────────────────────────────────┐
        │ Phase 2: Configure Second-Layer         │
        │  • CUIState (references entities)       │
        │  • Create state transition subscribers ─┼──> EventHandler
        └─────────────────────────────────────────┘
                              │
                              ▼
        ┌─────────────────────────────────────────┐
        │ Result: Fully configured scene          │
        │  • Entities populated                   │
        │  • Components configured                │
        │  • Event subscriptions active           │
        └─────────────────────────────────────────┘
```

---

## EventHandler Dependency Flow

```
┌──────────────────────────────────────────────────────────────────┐
│                         GameContext                              │
│                    (Lives for entire game)                       │
├──────────────────────────────────────────────────────────────────┤
│                                                                  │
│  ┌────────────────────────────────────────────┐                 │
│  │           EventHandler                     │                 │
│  │  (Manages event pub-sub system)            │                 │
│  │                                            │                 │
│  │  Subscribers registered:                   │                 │
│  │  • UI button click handlers                │◄────┐           │
│  │  • UI state transition handlers            │     │           │
│  │  • Mouse hover handlers                    │     │           │
│  │  • Custom event handlers                   │     │           │
│  └────────────────────────────────────────────┘     │           │
│                                                      │           │
└──────────────────────────────────────────────────────┼───────────┘
                                                       │
                                                       │ Reference injected
                                                       │
┌──────────────────────────────────────────────────────┼───────────┐
│                     SceneDataProvider                │           │
│                 (Creates SceneData)                  │           │
├──────────────────────────────────────────────────────┼───────────┤
│                                                      │           │
│  Creates:                                            │           │
│  ┌───────────────────────────────────────┐          │           │
│  │   IEntityConfigurator                 │          │           │
│  │   ┌─────────────────────────────────┐ │          │           │
│  │   │ EventHandler &m_event_handler ──┼─┼──────────┘           │
│  │   └─────────────────────────────────┘ │   (Reference stored) │
│  │                                       │                       │
│  │   During configuration:               │                       │
│  │   • Creates UI elements               │                       │
│  │   • Creates Subscriber objects        │                       │
│  │   • Calls m_event_handler.            │                       │
│  │     RegisterSubscriber() ──────────────┼────> Registers with  │
│  │                                       │       EventHandler    │
│  └───────────────────────────────────────┘                       │
│                                                                  │
│  (Configurator destroyed after scene creation)                  │
│  (Subscribers remain in EventHandler)                           │
└──────────────────────────────────────────────────────────────────┘

Key Points:
1. EventHandler lives in GameContext (outlives scenes)
2. Configurator is temporary (destroyed after scene creation)
3. Subscribers created during config remain in EventHandler
4. This enables atomic configuration (entity + events together)
```

---

## Strategy Pattern - Different Configurators

```
                ┌─────────────────────────────┐
                │   IEntityConfigurator       │
                │   (Abstract Interface)      │
                │                             │
                │   + ConfigureEMP(...)       │
                │   # EventHandler& handler   │
                └──────────────┬──────────────┘
                               │
                               │ Implements
                ┌──────────────┴──────────────┐
                │                             │
                ▼                             ▼
┌───────────────────────────┐   ┌─────────────────────────────┐
│ FlatbuffersEntity         │   │ TestEntityConfigurator      │
│ Configurator              │   │                             │
│                           │   │                             │
│ • Reads FlatBuffers data  │   │ • Reads in-memory data      │
│ • Production use          │   │ • Testing use               │
│ • Parses JSON-derived     │   │ • Direct EntityMemoryPool   │
│   binary format           │   │   injection                 │
└───────────────────────────┘   └─────────────────────────────┘
         Used with:                      Used with:
         EntityCollectionFbs*            EntityMemoryPool

┌─────────────────────────────────────────────────────────────────┐
│ Future extensibility:                                           │
│ • JSONEntityConfigurator (parse JSON directly)                  │
│ • NetworkEntityConfigurator (load from network)                 │
│ • Any new format can be added without changing existing code    │
└─────────────────────────────────────────────────────────────────┘
```

---

## EntityTransportVariant - Compile-Time Type Safety

```
┌──────────────────────────────────────────────────────────────────┐
│              EntityTransportVariant                              │
│  std::variant<monostate, EntityMemoryPool, shared_ptr<EMP>,      │
│                const EntityCollectionFbs*>                       │
└──────────────────────────────────────────────────────────────────┘
                              │
                              │ Holds exactly one of:
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
        ▼                     ▼                     ▼
┌───────────────┐  ┌──────────────────┐  ┌─────────────────────┐
│ std::monostate│  │ EntityMemoryPool │  │ const               │
│               │  │ or shared_ptr    │  │ EntityCollectionFbs*│
│               │  │                  │  │                     │
│ Empty state   │  │ Direct data      │  │ FlatBuffers pointer │
│ (no entities) │  │ (testing)        │  │ (production)        │
└───────────────┘  └──────────────────┘  └─────────────────────┘

Benefits:
┌──────────────────────────────────────────────────────────────────┐
│ ✅ Compile-time type safety                                      │
│    • std::holds_alternative<T> checks type                       │
│    • std::get<T> extracts with runtime check                     │
│    • All types known at compile time                             │
│                                                                  │
│ ✅ Zero-cost abstraction                                         │
│    • No virtual dispatch                                         │
│    • No runtime type information overhead                        │
│    • Compiler optimizes variant access                           │
│                                                                  │
│ ✅ Extensible                                                    │
│    • New types can be added to variant                           │
│    • Existing code continues to work                             │
│    • Type safety maintained                                      │
└──────────────────────────────────────────────────────────────────┘
```

---

## Two-Phase Configuration (Component Dependencies)

```
┌──────────────────────────────────────────────────────────────────┐
│ Phase 1: ConfigureFirstLayerComponents                          │
│ (Independent components - no dependencies on other entities)     │
└──────────────────────────────────────────────────────────────────┘
                              │
                              ▼
        ┌─────────────────────────────────────────┐
        │ For each entity in EntityCollectionFbs: │
        │                                         │
        │ • CMeta (entity metadata)               │
        │ • CUserInterface (UI tree structure)    │
        │   - Creates UIElement hierarchy         │
        │   - Registers event subscribers ────────┼──> EventHandler
        │ • CGrimoireMachina (grimoire data)      │
        │ • CMachinaForm (form data)              │
        └─────────────────────────────────────────┘
                              │
                              ▼
┌──────────────────────────────────────────────────────────────────┐
│ Phase 2: ConfigureSecondLayerComponents                         │
│ (Dependent components - reference entities configured in phase 1)│
└──────────────────────────────────────────────────────────────────┘
                              │
                              ▼
        ┌─────────────────────────────────────────┐
        │ For each entity in EntityCollectionFbs: │
        │                                         │
        │ • CUIState (state machine)              │
        │   - References entity IDs               │
        │   - Needs CUserInterface to exist first │
        │   - Creates state transition subscribers│
        │   - Registers subscribers ──────────────┼──> EventHandler
        └─────────────────────────────────────────┘

Why Two Phases?
┌──────────────────────────────────────────────────────────────────┐
│ CUIState needs to reference entities that must already exist:    │
│                                                                  │
│ Example:                                                         │
│   Entity 5 has CUIState that references entities 1, 2, 3        │
│   → Phase 1 configures entities 1, 2, 3 (with CUserInterface)   │
│   → Phase 2 configures entity 5's CUIState (can now reference)  │
│                                                                  │
│ Without two phases: Order-dependent bugs, forward references     │
│ With two phases: Clean separation, all references valid          │
└──────────────────────────────────────────────────────────────────┘
```

---

## Design Patterns Summary

```
┌──────────────────────────────────────────────────────────────────┐
│                    Design Patterns Used                          │
├──────────────────────────────────────────────────────────────────┤
│                                                                  │
│ 1. Strategy Pattern                                              │
│    • IEntityConfigurator = Strategy interface                    │
│    • FlatbuffersEntityConfigurator = Concrete strategy           │
│    • SceneData = Context (holds strategy)                        │
│                                                                  │
│ 2. Dependency Injection                                          │
│    • EventHandler injected via constructor                       │
│    • Dependencies explicit and testable                          │
│    • Constructor injection best practice                         │
│                                                                  │
│ 3. Data Transfer Object (DTO)                                    │
│    • SceneData packages all configuration data                   │
│    • Transfers data from provider to factory                     │
│    • Includes strategy appropriate for data                      │
│                                                                  │
│ 4. Template Method                                               │
│    • ConfigureEntityMemoryPoolFromSource orchestrates            │
│    • Phase 1: ConfigureFirstLayerComponents                      │
│    • Phase 2: ConfigureSecondLayerComponents                     │
│                                                                  │
│ 5. Type-Safe Union (std::variant)                                │
│    • EntityTransportVariant holds different data formats         │
│    • Compile-time type checking                                  │
│    • Zero-cost abstraction                                       │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘
```

---

## Key Takeaways

1. **EventHandler dependency is necessary**
   - Required for subscriber creation during configuration
   - Enables atomic setup (entity + events together)
   - Constructor injection makes dependency explicit

2. **Embedding configurator in SceneData is appropriate**
   - SceneData is a complete configuration package
   - Configurator is part of the configuration strategy
   - Keeps API simple and testable

3. **EntityTransportVariant is excellent choice**
   - Compile-time type safety
   - Zero-cost abstraction
   - Extensible with new formats
   - Perfect for "all types known at compile time"

4. **Design follows best practices**
   - Strategy pattern for configurators
   - Dependency injection for EventHandler
   - Single responsibility principle
   - Open/closed principle (open for extension)
   - Data-driven design

5. **No changes needed**
   - Current architecture is optimal
   - Alternative designs are inferior
   - Only documentation added

---

For complete analysis, see:
- `ENTITY_CONFIGURATOR_DESIGN_ANALYSIS.md` - Full 18KB analysis
- `ANALYSIS_SUMMARY.md` - Executive summary
