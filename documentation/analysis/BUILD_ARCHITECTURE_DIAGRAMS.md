# Build Architecture Diagrams

**Purpose:** Visual representations of current and proposed build architecture  
**Date:** December 11, 2025

---

## Table of Contents

1. [Current Architecture](#current-architecture)
2. [Circular Dependency Problems](#circular-dependency-problems)
3. [Proposed Three-Layer Architecture](#proposed-three-layer-architecture)
4. [Data Flow Architecture](#data-flow-architecture)
5. [Component Independence](#component-independence)
6. [Provider Pattern](#provider-pattern)

---

## Current Architecture

### Current Library Dependency Graph

```
                    ┌──────────┐
                    │  engine  │ (Top-level orchestrator)
                    └────┬─────┘
                         │
        ┌────────────────┼────────────────┐
        │                │                │
    ┌───▼────┐     ┌────▼────┐     ┌────▼────┐
    │display │◄───►│ scenes  │     │ logic   │
    └───┬────┘     └────┬────┘     └────┬────┘
        │               │               │
        └───────┬───────┴───────┬───────┘
                │               │
        ┌───────▼───────┐   ┌───▼──────────┐
        │   context     │   │ user_interface│
        └───────┬───────┘   └───┬───────────┘
                │               │
                └───────┬───────┘
                        │
            ┌───────────▼────────────┐
            │    components          │
            │    entity              │
            │    events              │
            │    data_providers      │
            │    assets              │
            └───────────┬────────────┘
                        │
            ┌───────────▼────────────┐
            │   data_handlers        │
            │   flatbuffers_headers  │
            └───────────┬────────────┘
                        │
            ┌───────────▼────────────┐
            │   logger               │
            │   config               │
            └────────────────────────┘

Legend:
  →   Depends on
  ◄─► Circular dependency
```

### Problem: Circular Dependencies

```
┌──────────┐                    ┌──────────┐
│ display  │◄──────────────────►│  scenes  │
└──────────┘                    └──────────┘
     │                               │
     │ depends on                    │ depends on
     │                               │
     ▼                               ▼
[Problem: Each needs the other at compile time]


┌──────────┐                    ┌──────────┐
│  engine  │────────────────────►│ context  │
└──────────┘                    └────┬─────┘
     ▲                               │
     │                               │ depends on
     │ depends on                    │
     └───────────────────────────────┘
[Problem: Circular reference through headers]


┌───────────┐      ┌──────────────┐      ┌─────────┐
│components │─────►│user_interface│─────►│ engine  │
└───────────┘      └──────────────┘      └────┬────┘
     ▲                                        │
     │                                        │
     └────────────────────────────────────────┘
[Problem: Long circular chain through many libraries]
```

---

## Proposed Three-Layer Architecture

### Layer Structure

```
┌───────────────────────────────────────────────────────────────┐
│                    Layer 3: Orchestration                      │
│                                                                │
│  ┌──────────────┐  ┌───────────────┐  ┌──────────────┐      │
│  │scene_        │  │display_       │  │engine_       │      │
│  │management    │  │management     │  │core          │      │
│  └──────────────┘  └───────────────┘  └──────────────┘      │
│                                                                │
│  • Coordinates lower layers                                   │
│  • Factory and builder patterns                               │
│  • Application entry points                                   │
└───────────────────────────────────────────────────────────────┘
                           │
                  Can depend on ↓
┌───────────────────────────────────────────────────────────────┐
│                   Layer 2: Implementation                      │
│                                                                │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐       │
│  │data_     │ │entity_   │ │event_    │ │logic_    │       │
│  │providers │ │system    │ │system    │ │system    │       │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘       │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐                     │
│  │ui_system │ │asset_    │ │context_  │                     │
│  │          │ │system    │ │impl      │                     │
│  └──────────┘ └──────────┘ └──────────┘                     │
│                                                                │
│  • Concrete implementations                                   │
│  • Business logic                                             │
│  • FlatBuffers hidden in .cpp                                 │
└───────────────────────────────────────────────────────────────┘
                           │
                  Can depend on ↓
┌───────────────────────────────────────────────────────────────┐
│                  Layer 1: Data & Interfaces                    │
│                                                                │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐       │
│  │types     │ │data_     │ │interfaces│ │components│       │
│  │          │ │structures│ │          │ │          │       │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘       │
│  ┌──────────┐ ┌──────────┐                                   │
│  │logger    │ │config    │                                   │
│  └──────────┘ └──────────┘                                   │
│                                                                │
│  • Pure data structures                                       │
│  • Abstract interfaces                                        │
│  • NO circular dependencies                                   │
└───────────────────────────────────────────────────────────────┘
```

### Dependency Rules

```
┌─────────────────────────────────────────────────┐
│ Allowed Dependencies                            │
└─────────────────────────────────────────────────┘

Layer 3 → Layer 2 ✅
Layer 3 → Layer 1 ✅
Layer 2 → Layer 1 ✅

Layer 1 → Layer 2 ❌ FORBIDDEN
Layer 1 → Layer 3 ❌ FORBIDDEN
Layer 2 → Layer 3 ❌ FORBIDDEN

Circular dependencies: ❌ FORBIDDEN AT ALL LAYERS
```

---

## Data Flow Architecture

### Current: FlatBuffers Everywhere

```
┌──────────────────────────────────────────────────────────┐
│                 FlatBuffers Schemas                      │
│                     (32 .fbs files)                      │
└────────────────────┬─────────────────────────────────────┘
                     │ flatc compiler
                     ▼
┌──────────────────────────────────────────────────────────┐
│              Generated Headers                           │
│              (32 *_generated.h files)                    │
└────────────────────┬─────────────────────────────────────┘
                     │
         ┌───────────┼───────────┐
         │           │           │
         ▼           ▼           ▼
    ┌────────┐  ┌────────┐  ┌────────┐
    │Library │  │Library │  │Library │  (14 libraries)
    │   A    │  │   B    │  │   C    │
    └────────┘  └────────┘  └────────┘

Problem: FlatBuffers headers included in 14+ libraries
Impact: Changes to .fbs files cause widespread recompilation
```

### Proposed: Provider Pattern

```
┌──────────────────────────────────────────────────────────┐
│                 FlatBuffers Schemas                      │
└────────────────────┬─────────────────────────────────────┘
                     │ flatc compiler
                     ▼
┌──────────────────────────────────────────────────────────┐
│              Generated Headers                           │
└────────────────────┬─────────────────────────────────────┘
                     │ ONLY included in
                     ▼
┌──────────────────────────────────────────────────────────┐
│         Data Provider Implementations (.cpp)             │
│         (10 provider files in Layer 2)                   │
└────────────────────┬─────────────────────────────────────┘
                     │ Returns native C++ structs
                     ▼
┌──────────────────────────────────────────────────────────┐
│         Native Data Structures (Layer 1)                 │
│         (pure C++ structs, no FlatBuffers)               │
└────────────────────┬─────────────────────────────────────┘
                     │ Used by
                     ▼
┌──────────────────────────────────────────────────────────┐
│              Application Code                            │
│              (Layer 2 & 3)                               │
└──────────────────────────────────────────────────────────┘

Benefit: FlatBuffers changes only affect 10 provider .cpp files
Impact: 90% reduction in recompilation
```

---

## Component Independence

### Breaking CUserInterface → UIElement Dependency

```
Current Problem:
┌─────────────────────────────────────────────────────────┐
│ components/CUserInterface.h includes UIElement.h        │
│ → UIElement.h includes SceneContext.h                   │
│ → SceneContext.h includes Engine headers                │
│ → Creates massive circular dependency                   │
└─────────────────────────────────────────────────────────┘

Proposed Solution:
┌─────────────────────────────────────────────────────────┐
│ components/CUserInterface.h:                            │
│   class UIElement;  // Forward declaration              │
│   std::unique_ptr<UIElement> root;                      │
│                                                          │
│ components/CUserInterface.cpp:                          │
│   #include "UIElement.h"  // Full definition in .cpp    │
└─────────────────────────────────────────────────────────┘

Result: Breaks circular dependency, faster compilation
```

---

## Provider Pattern

### Interface + Implementation

```
Layer 1: Interface
┌──────────────────────────────────────┐
│ interfaces/IEngineDataProvider.h     │
│                                      │
│ class IEngineDataProvider {         │
│   virtual LoadEngineConfig() = 0;   │
│ };                                   │
│                                      │
│ Uses: Native C++ structs            │
└──────────────────────────────────────┘
         │
         │ Implemented by
         ▼
Layer 2: Implementation
┌──────────────────────────────────────┐
│ FlatbuffersEngineDataProvider.cpp    │
│                                      │
│ #include "engine_config_generated.h"│
│                                      │
│ LoadEngineConfig() {                │
│   // Load FlatBuffers               │
│   // Convert to native struct       │
│   return native_data;               │
│ }                                    │
└──────────────────────────────────────┘

Result: FlatBuffers isolated, clean interface
```

---

## Build Time Comparison

### Before Stratification
```
Change Component.h → Rebuild 80-90% of codebase (2-5 min)
Change FlatBuffers → Rebuild 90% of codebase (3-6 min)
```

### After Stratification
```
Change Component.h → Rebuild 30-40% of codebase (30-90 sec)
Change FlatBuffers → Rebuild 10% of codebase (15-30 sec)
Change Layer 2 impl → Rebuild 20-30% of codebase (20-60 sec)
```

**Expected Improvement:** 30-40% faster clean builds, 50-60% faster incremental

---

## References

- [BUILD_DEPENDENCY_ANALYSIS.md](./BUILD_DEPENDENCY_ANALYSIS.md)
- [BUILD_STRATIFICATION_QUICK_REF.md](./BUILD_STRATIFICATION_QUICK_REF.md)
- [DATA_PROVIDER_SYSTEM.md](../DATA_PROVIDER_SYSTEM.md)
