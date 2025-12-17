# FlatBuffers Layer Violations - Visual Diagram

**Date:** 2025-12-17

---

## Current State (BEFORE Migration)

```
┌─────────────────────────────────────────────────────────────────┐
│                         LAYER 1: Data/Types                     │
│                    (Should have ZERO dependencies)              │
└─────────────────────────────────────────────────────────────────┘

    ❌ VIOLATION: src/types/events/EventPacket.h
    │
    ├─ #include "events_generated.h"        ◄── FlatBuffers!
    │    └─ Uses: EventType enum
    │
    └─ #include "scene_types_generated.h"   ◄── FlatBuffers!
         └─ Uses: SceneType enum


    ❌ VIOLATION: src/types/events/Subscriber.h
    │
    └─ #include "events_generated.h"        ◄── FlatBuffers!
         └─ Uses: EventType enum


    ❌ VIOLATION: src/types/core/SceneInfo.h
    │
    └─ #include "scene_types_generated.h"   ◄── FlatBuffers!
         └─ Uses: SceneType enum


    ❌ VIOLATION: src/types/user_interface/DropDownListElement.h
    │
    └─ #include "user_interface_generated.h" ◄── FlatBuffers!
         └─ Uses: DataPopulateFunction enum


    ❌ VIOLATION: src/types/core/Fragment.h
    │
    └─ #include "fragments_generated.h"      ◄── FlatBuffers!
         └─ Uses: ViewDirection enum


    ✅ ACCEPTABLE: src/types/core/FbsSceneData.h
    │
    └─ #include "scene_data_generated.h"     ◄── OK (wrapper type)
         └─ Purpose: Temporary FlatBuffers pointer holder

┌─────────────────────────────────────────────────────────────────┐
│                    LAYER 2: Logic/Services                      │
│              (Contains FlatBuffers conversion logic)            │
└─────────────────────────────────────────────────────────────────┘

    Currently: No conversion functions exist for these enums
    
    ⚠️ Problem: Layer 1 directly uses FlatBuffers enums
                (no conversion step)

```

---

## Target State (AFTER Migration)

```
┌─────────────────────────────────────────────────────────────────┐
│                         LAYER 1: Data/Types                     │
│                      ✅ ZERO DEPENDENCIES                        │
└─────────────────────────────────────────────────────────────────┘

    ✅ CLEAN: src/types/events/EventPacket.h
    │
    ├─ #include "EventType.h"               ◄── Native enum!
    │    └─ Uses: EventType (enum class)
    │
    └─ #include "SceneType.h"               ◄── Native enum!
         └─ Uses: SceneType (enum class)


    ✅ CLEAN: src/types/events/Subscriber.h
    │
    └─ #include "EventType.h"               ◄── Native enum!
         └─ Uses: EventType (enum class)


    ✅ CLEAN: src/types/core/SceneInfo.h
    │
    └─ #include "SceneType.h"               ◄── Native enum!
         └─ Uses: SceneType (enum class)


    ✅ CLEAN: src/types/user_interface/DropDownListElement.h
    │
    └─ #include "DataPopulateFunction.h"    ◄── Native enum!
         └─ Uses: DataPopulateFunction (enum class)


    ✅ CLEAN: src/types/core/Fragment.h
    │
    └─ #include "ViewDirection.h"           ◄── Native enum!
         └─ Uses: ViewDirection (enum class)


    ✅ ACCEPTABLE: src/types/core/FbsSceneData.h
    │
    └─ #include "scene_data_generated.h"    ◄── OK (wrapper type)
         └─ Purpose: Temporary FlatBuffers pointer holder

         ┌──────────────────────────────────────────────────────┐
         │  New Native Enums Created:                           │
         ├──────────────────────────────────────────────────────┤
         │  src/types/events/EventType.h                        │
         │  src/types/core/SceneType.h                          │
         │  src/types/user_interface/DataPopulateFunction.h     │
         │  src/types/core/ViewDirection.h                      │
         └──────────────────────────────────────────────────────┘

                              ▼
                    (depends on Layer 1)
                              ▼

┌─────────────────────────────────────────────────────────────────┐
│                    LAYER 2: Logic/Services                      │
│              ✅ Contains FlatBuffers conversion logic           │
└─────────────────────────────────────────────────────────────────┘

    ✅ Conversion: FlatBuffers Data Providers / Configurators
    │
    ├─ ConvertEventType(uint64_t fbs_type)
    │    └─ EventTypeFbs → EventType
    │
    ├─ ConvertSceneType(int8_t fbs_type)
    │    └─ SceneTypeFbs → SceneType
    │
    ├─ ConvertDataPopulateFunction(int8_t fbs_func)
    │    └─ DataPopulateFunctionFbs → DataPopulateFunction
    │
    └─ ConvertViewDirection(uint8_t fbs_dir)
         └─ ViewDirectionFbs → ViewDirection

         ┌──────────────────────────────────────────────────────┐
         │  FlatBuffers Enums Renamed:                          │
         ├──────────────────────────────────────────────────────┤
         │  EventType → EventTypeFbs                            │
         │  SceneType → SceneTypeFbs                            │
         │  DataPopulateFunction → DataPopulateFunctionFbs      │
         │  ViewDirection → ViewDirectionFbs                    │
         └──────────────────────────────────────────────────────┘

                              ▼
                      (reads FlatBuffers)
                              ▼

┌─────────────────────────────────────────────────────────────────┐
│                  FlatBuffers Binary Data                        │
│                    (Serialized on disk)                         │
└─────────────────────────────────────────────────────────────────┘

    Binary files: .bin format
    │
    └─ Contains: EventTypeFbs, SceneTypeFbs, etc.
                 (FlatBuffers integer values)

```

---

## Data Flow (Target State)

```
┌──────────────────────────────────────────────────────────────────┐
│                      DATA LOADING FLOW                           │
└──────────────────────────────────────────────────────────────────┘

1. Binary File (.bin)
       │
       │ FlatBuffers load
       ▼
2. FlatBuffers Pointers (Layer 2)
       │
       │ Contains: EventTypeFbs (uint64_t value)
       │           SceneTypeFbs (int8_t value)
       │           DataPopulateFunctionFbs (int8_t value)
       │           ViewDirectionFbs (uint8_t value)
       ▼
3. Conversion Functions (Layer 2)
       │
       │ ConvertEventType(fbs_value) → EventType
       │ ConvertSceneType(fbs_value) → SceneType
       │ ConvertDataPopulateFunction(fbs_value) → DataPopulateFunction
       │ ConvertViewDirection(fbs_value) → ViewDirection
       ▼
4. Native C++ Enums (Layer 1)
       │
       │ Stored in: EventPacket, SceneInfo, DropDownListElement, Fragment
       │ Type-safe enum class values
       ▼
5. Runtime Usage (All Layers)
       │
       └─ Game logic uses native enums
          No FlatBuffers types in runtime code!

┌──────────────────────────────────────────────────────────────────┐
│                      KEY BENEFIT                                 │
├──────────────────────────────────────────────────────────────────┤
│  Runtime code NEVER sees FlatBuffers types!                      │
│  FlatBuffers only used during configuration/loading (Layer 2)    │
│  Clean separation of serialization from runtime types            │
└──────────────────────────────────────────────────────────────────┘
```

---

## Comparison: Layout Enum (Already Migrated)

### ✅ SUCCESS STORY: Layout enum follows the correct pattern

```
BEFORE UI Decoupling:
    Layer 1: UIElement.h
        └─ #include "user_interface_generated.h"  ❌
             └─ Uses: LayoutType enum

AFTER UI Decoupling (2025-12-16):
    Layer 1: UIElement.h
        └─ #include "Layout.h"                    ✅
             └─ Uses: Layout (native enum class)

    Layer 2: FlatbuffersUIElementConfigurator.cpp
        └─ ConvertLayout(int8_t fbs_layout)       ✅
             └─ LayoutFbs → Layout

    FlatBuffers Schema: user_interface.fbs
        └─ enum LayoutFbs: byte { ... }           ✅
             (renamed from LayoutType)
```

**This is the pattern to follow for the 4 remaining enums!**

---

## Migration Priority Map

```
   PRIORITY          ENUM NAME              COMPLEXITY          IMPACT
┌─────────────┬──────────────────────┬────────────────┬──────────────────┐
│             │                      │                │                  │
│  CRITICAL   │   SceneType          │  High          │  Widespread      │
│             │                      │  (many files)  │  (all systems)   │
│             │                      │                │                  │
├─────────────┼──────────────────────┼────────────────┼──────────────────┤
│             │                      │                │                  │
│    HIGH     │   EventType          │  High          │  Core system     │
│             │                      │  (bit flags)   │  (event routing) │
│             │                      │                │                  │
├─────────────┼──────────────────────┼────────────────┼──────────────────┤
│             │                      │                │                  │
│   MEDIUM    │ DataPopulateFunction │  Low           │  UI-specific     │
│             │                      │  (simple enum) │  (dropdowns)     │
│             │                      │                │                  │
├─────────────┼──────────────────────┼────────────────┼──────────────────┤
│             │                      │                │                  │
│    LOW      │   ViewDirection      │  Low           │  Limited scope   │
│             │                      │  (2 values)    │  (crafting only) │
│             │                      │                │                  │
└─────────────┴──────────────────────┴────────────────┴──────────────────┘

            Estimated Time: 12-18 hours total (1.5-2 days)
```

---

## Validation Command

After all migrations complete, run:

```bash
find src/types -name "*.h" -type f \
    ! -path "*/flatbuffers/*" \
    ! -name "FbsSceneData.h" \
    -exec grep -l "_generated\.h" {} \;
```

**Expected result:** Empty (no output)

**Meaning:** Zero FlatBuffers dependencies in Layer 1 ✅

---

## Quick Reference

| What | Where | Purpose |
|------|-------|---------|
| **Native Enums** | `src/types/*/*.h` | Layer 1 runtime types |
| **FlatBuffers Enums** | `src/types/flatbuffers/*/*.fbs` | Serialization schema |
| **Conversion Functions** | `src/*/Fbs*Configurator.cpp` | Layer 2 conversion logic |
| **Pattern Examples** | Layout, SpacingAndSizing | Already completed migrations |

---

**For detailed implementation steps, see:**
- `FLATBUFFERS_MIGRATION_TODO.md` - Step-by-step checklist
- `FLATBUFFERS_LAYER_ANALYSIS.md` - Complete analysis
- `FLATBUFFERS_SUMMARY.md` - Executive overview
