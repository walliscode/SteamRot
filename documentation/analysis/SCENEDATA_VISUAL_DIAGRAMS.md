# SceneData Architecture: Visual Diagrams

This document provides visual representations of the current architecture and proposed solutions.

## Current Architecture: Import Flow

```
┌─────────────────────────────────────────────────────────────────────┐
│                        IMPORT DATA FLOW                              │
└─────────────────────────────────────────────────────────────────────┘

Step 1: Load FlatBuffers Data
┌──────────────────────┐
│ FlatBuffers Binary   │
│  (scene_data.bin)    │
└──────────┬───────────┘
           │
           ▼
┌──────────────────────┐
│ FlatbuffersDataLoader│
│   .LoadFile()        │
└──────────┬───────────┘
           │
           ▼
┌──────────────────────────────┐
│ SceneDataFbs (FlatBuffers)   │
│ ├─ scene_info                │
│ ├─ scene_resources_config    │
│ └─ entities: EntityCollection│
└──────────┬───────────────────┘
           │
           ▼

Step 2: Create SceneData with Pointer
┌────────────────────────────────────────┐
│ FlatbuffersSceneDataProvider           │
│   .ProvideDefaultSceneData()           │
│                                        │
│   Creates FbsSceneData:                │
│   ┌──────────────────────────────┐    │
│   │ FbsSceneData                 │    │
│   │ ├─ scene_info                │    │
│   │ ├─ scene_resources_config    │    │
│   │ ├─ scene_asset_config        │    │
│   │ └─ entity_collection: *──────┼────┼──> EntityCollectionFbs
│   └──────────────────────────────┘    │    (pointer to FlatBuffers)
│                                        │
│   return unique_ptr<SceneData>        │
└────────────┬───────────────────────────┘
             │
             ▼

Step 3: Configure Scene (Downcasting Required)
┌────────────────────────────────────────────────┐
│ FlatbuffersSceneConfigurator                   │
│   .ConfigureEntities(scene, scene_data)        │
│                                                │
│   const SceneData* scene_data  (base pointer) │
│            ↓                                   │
│   dynamic_cast + const_cast                    │
│            ↓                                   │
│   FbsSceneData* fbs_scene_data (derived)      │
│            ↓                                   │
│   Access: fbs_scene_data->entity_collection    │
└────────────┬───────────────────────────────────┘
             │
             ▼

Step 4: Configure Entities
┌────────────────────────────────────────────┐
│ FlatbuffersEntityConfigurator              │
│   (event_handler, entity_collection)       │
│                                            │
│   .ConfigureEntityMemoryPool(emp)          │
│                                            │
│   For each entity in entity_collection:    │
│   ├─ Read FlatBuffers EntityDataFbs        │
│   ├─ Configure CUserInterface              │
│   ├─ Configure CGrimoireMachina            │
│   └─ ... (other components)                │
└────────────┬───────────────────────────────┘
             │
             ▼
┌────────────────────────┐
│ EntityMemoryPool       │
│ (Runtime Entities)     │
│                        │
│ Ready for game logic   │
└────────────────────────┘
```

## Problem: Export Direction (Missing)

```
┌─────────────────────────────────────────────────────────────────────┐
│                    EXPORT DATA FLOW (MISSING)                        │
└─────────────────────────────────────────────────────────────────────┘

Step 1: What Data Structure to Use?
┌────────────────────────┐
│ EntityMemoryPool       │
│ (Runtime Entities)     │
│                        │
│ Need to save this...   │
└────────────┬───────────┘
             │
             ▼
             ???  ← NO CLEAR PATTERN
             │
             ▼
┌────────────────────────────────────────────┐
│ Question: What goes here?                  │
│                                            │
│ Option A: NativeSceneData?                 │
│   ├─ scene_info                            │
│   ├─ scene_resources_config                │
│   ├─ scene_asset_config                    │
│   └─ entity_memory_pool: * ──> ???         │
│                                            │
│ Option B: Different pattern entirely?      │
│   (IEntityExporter interface?)             │
└────────────┬───────────────────────────────┘
             │
             ▼
┌────────────────────────┐
│ FlatBuffers Binary     │
│  (save_file.bin)       │
└────────────────────────┘
```

## Architecture Comparison

### Current: Using Inheritance for Data Passing

```
┌──────────────────────────────────────┐
│        SceneData (base)              │
│  ┌────────────────────────────────┐  │
│  │ + scene_info                   │  │
│  │ + scene_resources_config       │  │
│  │ + scene_asset_config           │  │
│  └────────────────────────────────┘  │
└──────────┬───────────────────────────┘
           │
           │ inheritance (is-a)
           │
┌──────────▼───────────────────────────┐
│     FbsSceneData (derived)           │
│  ┌────────────────────────────────┐  │
│  │ Inherits: scene_info, etc.     │  │
│  │ + entity_collection: ptr ──────┼──┼──> EntityCollectionFbs
│  └────────────────────────────────┘  │  (Heavy FlatBuffers data)
└──────────────────────────────────────┘

Problem: "Smuggling" pointer through inheritance
         Requires downcasting: SceneData* → FbsSceneData*
```

### Proposed Solution 1: Add NativeSceneData (Symmetric)

```
                    ┌──────────────────────────────────────┐
                    │        SceneData (base)              │
                    │  ┌────────────────────────────────┐  │
                    │  │ + scene_info                   │  │
                    │  │ + scene_resources_config       │  │
                    │  │ + scene_asset_config           │  │
                    │  └────────────────────────────────┘  │
                    └──────────┬───────────────────────────┘
                               │
           ┌───────────────────┴───────────────────┐
           │                                       │
           │ IMPORT                                │ EXPORT
           │                                       │
┌──────────▼─────────────────┐      ┌─────────────▼──────────────────┐
│   FbsSceneData             │      │   NativeSceneData              │
│ ┌────────────────────────┐ │      │ ┌────────────────────────────┐ │
│ │ + entity_collection: * ─┼─┼──>   │ │ + entity_memory_pool: *  ──┼─┼──>
│ └────────────────────────┘ │ FBS  │ └────────────────────────────┘ │ EMP
└────────────────────────────┘      └────────────────────────────────┘

Advantage: Symmetric pattern for import/export
Disadvantage: Still uses inheritance for data passing
```

### Proposed Solution 2: Importer/Exporter Pattern (Clean)

```
┌──────────────────────────────────────┐
│        SceneData (lightweight)       │
│  ┌────────────────────────────────┐  │
│  │ + scene_info                   │  │
│  │ + scene_resources_config       │  │
│  │ + scene_asset_config           │  │
│  └────────────────────────────────┘  │
│  NO heavy data pointers!             │
└──────────────────────────────────────┘
           │
           │ Used by both import and export
           │
    ┌──────┴──────┐
    │             │
    ▼             ▼
┌──────────────────────────────────┐   ┌──────────────────────────────────┐
│     IEntityImporter              │   │     IEntityExporter              │
│  (Interface)                     │   │  (Interface)                     │
│                                  │   │                                  │
│  + ImportEntities(emp)           │   │  + ExportEntities(emp, out_size) │
└──────────┬───────────────────────┘   └──────────┬───────────────────────┘
           │                                      │
           │ Implemented by                       │ Implemented by
           │                                      │
┌──────────▼───────────────────────┐   ┌──────────▼───────────────────────┐
│ FlatbuffersEntityImporter        │   │ FlatbuffersEntityExporter        │
│                                  │   │                                  │
│  Constructor:                    │   │  Constructor:                    │
│    (EntityCollectionFbs&)        │   │    (no heavy data needed)        │
│                                  │   │                                  │
│  ImportEntities(emp):            │   │  ExportEntities(emp, out_size):  │
│    - Read FlatBuffers            │   │    - Read EntityMemoryPool       │
│    - Configure components        │   │    - Build FlatBuffers           │
│    - Populate emp                │   │    - Return binary               │
└──────────────────────────────────┘   └──────────────────────────────────┘

Advantages:
  ✓ No inheritance for data passing
  ✓ No downcasting required
  ✓ Clear separation: data vs. operations
  ✓ Extensible (add JsonImporter, NetworkImporter, etc.)
```

## Data Flow Comparison

### Current Import vs. Proposed Export (Solution 1)

```
IMPORT:                              EXPORT:
┌──────────────┐                     ┌──────────────┐
│ FlatBuffers  │                     │ Runtime EMP  │
│    File      │                     │   (Scene)    │
└──────┬───────┘                     └──────┬───────┘
       │                                    │
       ▼                                    ▼
┌──────────────┐                     ┌──────────────┐
│   Load FB    │                     │   Create     │
│    Data      │                     │   Native     │
└──────┬───────┘                     │  SceneData   │
       │                             └──────┬───────┘
       ▼                                    │
┌──────────────┐                            │
│   Create     │                            ▼
│ FbsSceneData │                     ┌──────────────┐
│  (ptr to FB) │                     │NativeScene   │
└──────┬───────┘                     │Data(ptr→EMP)│
       │                             └──────┬───────┘
       ▼                                    │
┌──────────────┐                            ▼
│ Configurator │                     ┌──────────────┐
│  (downcast)  │                     │   Exporter   │
└──────┬───────┘                     │  (downcast)  │
       │                             └──────┬───────┘
       ▼                                    │
┌──────────────┐                            ▼
│   Entity     │                     ┌──────────────┐
│ Configurator │                     │  Build FB    │
└──────┬───────┘                     └──────┬───────┘
       │                                    │
       ▼                                    ▼
┌──────────────┐                     ┌──────────────┐
│ EntityMemory │                     │ FlatBuffers  │
│     Pool     │                     │    File      │
└──────────────┘                     └──────────────┘

    SYMMETRIC PATTERN
```

### Clean Architecture (Solution 2)

```
IMPORT:                              EXPORT:
┌──────────────┐                     ┌──────────────┐
│ FlatBuffers  │                     │ Runtime EMP  │
│    File      │                     │              │
└──────┬───────┘                     └──────┬───────┘
       │                                    │
       ▼                                    │
┌──────────────┐                            │
│   Load FB    │                            │
│    Data      │                            │
└──────┬───────┘                            │
       │                                    │
       ▼                                    ▼
┌──────────────┐                     ┌──────────────┐
│   Create     │                     │   Create     │
│FB Importer   │                     │FB Exporter   │
│(FB data ref) │                     │              │
└──────┬───────┘                     └──────┬───────┘
       │                                    │
       │ NO DOWNCASTING                     │ NO DOWNCASTING
       │                                    │
       ▼                                    ▼
┌──────────────┐                     ┌──────────────┐
│  Importer    │                     │  Exporter    │
│ .Import(emp) │                     │.Export(emp)  │
└──────┬───────┘                     └──────┬───────┘
       │                                    │
       ▼                                    ▼
┌──────────────┐                     ┌──────────────┐
│ EntityMemory │                     │ FlatBuffers  │
│     Pool     │                     │    File      │
└──────────────┘                     └──────────────┘

    CLEAN SEPARATION
    Operations as interfaces, not data as inheritance
```

## Type Hierarchy Diagrams

### Current Hierarchy

```
                    SceneData
                       │
                       │ virtual destructor
                       │ scene_info
                       │ scene_resources_config
                       │ scene_asset_config
                       │
                       └─── FbsSceneData
                              │
                              └─ entity_collection: ptr
                                 (FlatBuffers data)
```

### Solution 1: Add NativeSceneData

```
                    SceneData
                       │
                       │ virtual destructor
                       │ scene_info
                       │ scene_resources_config
                       │ scene_asset_config
                       │
          ┌────────────┴────────────┐
          │                         │
     FbsSceneData            NativeSceneData
          │                         │
          └─ entity_collection*     └─ entity_memory_pool*
             (for IMPORT)               (for EXPORT)
```

### Solution 2: No Inheritance for Data

```
                    SceneData
                       │
                       │ (no virtual destructor needed)
                       │ scene_info
                       │ scene_resources_config
                       │ scene_asset_config
                       │
                       └─ USED AS-IS (no derivation)


    IEntityImporter          IEntityExporter
          │                         │
          │                         │
    FlatbuffersEntity-      FlatbuffersEntity-
        Importer                Exporter
          │                         │
    (stores FB data)          (receives EMP ref)
```

## Interface Design

### Solution 1 Interfaces

```cpp
// Import
class IEntityImporter {
  virtual ImportEntities(EntityMemoryPool&) = 0;
};

// Export
class IEntityExporter {
  virtual ExportEntities(const NativeSceneData*) = 0;
};

// Usage still requires downcasting internally
```

### Solution 2 Interfaces

```cpp
// Import
class IEntityImporter {
  virtual ImportEntities(EntityMemoryPool&) = 0;
};

// Export  
class IEntityExporter {
  virtual ExportEntities(const EntityMemoryPool&) = 0;
};

// No downcasting - data passed directly
```

## Summary Diagram: The Choice

```
┌─────────────────────────────────────────────────────────────────┐
│                    ARCHITECTURAL CHOICE                          │
└─────────────────────────────────────────────────────────────────┘

Option 1: Status Quo
  ✗ No export capability
  ✗ Asymmetric architecture
  ✗ Downcasting required
  ✓ No changes needed

Option 2: Add NativeSceneData (RECOMMENDED SHORT-TERM)
  ✓ Export capability
  ✓ Symmetric architecture
  ✗ Still requires downcasting
  ✓ Minimal changes (2-3 days)
  → Unblocks current work

Option 3: Importer/Exporter Pattern (RECOMMENDED LONG-TERM)
  ✓ Export capability
  ✓ Symmetric architecture
  ✓ No downcasting
  ✓ Clean architecture
  ✗ Significant refactoring (1-2 weeks)
  → Better long-term solution

RECOMMENDED: Two-Phase Approach
  Phase 1: Implement Option 2 (immediate needs)
  Phase 2: Migrate to Option 3 (when resources allow)
```

---

**Visual Diagrams Version**: 1.0  
**Date**: 2026-01-06  
**Related Documents**: SCENEDATA_ARCHITECTURE_ANALYSIS.md, ADR_SCENEDATA_IMPORT_EXPORT.md
