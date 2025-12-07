# Scene Data Access Patterns - Visual Guide

**Date**: December 7, 2025  
**Context**: Visual diagrams for scene data access patterns  
**Related**: SCENE_DATA_PROVIDER_ARCHITECTURE_ANALYSIS.md

---

## Overview Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                     Scene Data Sources                       │
└─────────────────────────────────────────────────────────────┘
                             │
                 ┌───────────┴───────────┐
                 │                       │
                 ↓                       ↓
    ┌────────────────────┐   ┌────────────────────┐
    │   Default Data     │   │    Saved Data      │
    │  (Template/New)    │   │   (Load Game)      │
    └────────────────────┘   └────────────────────┘
                 │                       │
                 ↓                       ↓
    ┌────────────────────┐   ┌────────────────────┐
    │ISceneDataProvider  │   │ISaveDataProvider   │
    └────────────────────┘   └────────────────────┘
                 │                       │
                 ↓                       ↓
    ┌────────────────────┐   ┌────────────────────┐
    │  LoadSceneData()   │   │    LoadSave()      │
    └────────────────────┘   └────────────────────┘
                 │                       │
                 │                       ↓
                 │           ┌────────────────────┐
                 │           │SceneDataExtractor  │
                 │           │   (Future)         │
                 │           └────────────────────┘
                 │                       │
                 └───────────┬───────────┘
                             ↓
                 ┌────────────────────┐
                 │    SceneData       │
                 │  (Unified Format)  │
                 └────────────────────┘
                             │
                             ↓
                 ┌────────────────────┐
                 │  LoadScene(data)   │
                 │ (Single Code Path) │
                 └────────────────────┘
```

---

## Data Structure Comparison

### Default Scene Data (Flat)

```
File: data/scenes/title.scene_data.bin
┌─────────────────────────────┐
│      SceneDataFbs           │
│  (Root Table)               │
├─────────────────────────────┤
│ - scene_info                │ ← Direct access
│ - scene_resources           │ ← Direct access
│ - assets                    │ ← Direct access
│ - entity_collection         │ ← Direct access
│ - logic_collection_data     │ ← Direct access
└─────────────────────────────┘
        ↓ Convert directly
┌─────────────────────────────┐
│        SceneData            │
│  (C++ Struct)               │
├─────────────────────────────┤
│ - scene_type                │
│ - scene_id                  │
│ - render_texture_width      │
│ - render_texture_height     │
└─────────────────────────────┘
```

### Saved Scene Data (Nested)

```
File: saves/slot_0/save_data.bin
┌─────────────────────────────────────────┐
│           SaveData                      │
│        (Root Table)                     │
├─────────────────────────────────────────┤
│ - metadata                              │
│ - current_scene_type                    │
│ - scene_states[] (Future)               │
│   └─ SceneStateData[]                   │ ← Nested!
│      ├─ scene_id                        │
│      ├─ scene_type                      │
│      └─ entity_collection               │ ← Deep nesting
└─────────────────────────────────────────┘
        ↓ Extract via SceneDataExtractor
┌─────────────────────────────────────────┐
│        SceneData[]                      │
│  (C++ Struct Array)                     │
├─────────────────────────────────────────┤
│ [0] scene_type, scene_id, dimensions    │
│ [1] scene_type, scene_id, dimensions    │
│ [2] scene_type, scene_id, dimensions    │
└─────────────────────────────────────────┘
```

---

## Flow Diagram: New Game (Direct Access)

```
┌─────────────┐
│ New Game    │
│ Button      │
└──────┬──────┘
       │
       ↓
┌─────────────────────────────┐
│ GameManager::NewGame()      │
└──────┬──────────────────────┘
       │
       ↓
┌─────────────────────────────┐
│ ISceneDataProvider          │
│ .LoadSceneData(TITLE)       │
└──────┬──────────────────────┘
       │
       ↓
┌─────────────────────────────┐
│ FlatbuffersDataLoader       │
│ .ProvideDefaultSceneData()  │
└──────┬──────────────────────┘
       │
       ↓ Read file
┌─────────────────────────────┐
│ data/scenes/                │
│   title.scene_data.bin      │
└──────┬──────────────────────┘
       │
       ↓ Convert FlatBuffers → C++
┌─────────────────────────────┐
│ SceneData struct            │
│ (Ready to use)              │
└──────┬──────────────────────┘
       │
       ↓
┌─────────────────────────────┐
│ SceneManager                │
│ .LoadScene(scene_data)      │
└─────────────────────────────┘
```

**Key Points**:
- ✅ Single, linear flow
- ✅ Direct conversion
- ✅ No nesting to navigate
- ✅ Fast and simple

---

## Flow Diagram: Load Game (Nested Access)

```
┌─────────────┐
│ Load Game   │
│ Button      │
└──────┬──────┘
       │
       ↓
┌─────────────────────────────┐
│ GameManager::LoadGame(0)    │
└──────┬──────────────────────┘
       │
       ↓
┌─────────────────────────────┐
│ ISaveDataProvider           │
│ .LoadSave(0)                │
└──────┬──────────────────────┘
       │
       ↓
┌─────────────────────────────┐
│ FlatbuffersSaveDataProvider │
│ .LoadSave()                 │
└──────┬──────────────────────┘
       │
       ↓ Read file
┌─────────────────────────────┐
│ saves/slot_0/               │
│   save_data.bin             │
└──────┬──────────────────────┘
       │
       ↓ Convert FlatBuffers → C++
┌─────────────────────────────┐
│ SaveData struct             │
│ (Contains nested scenes)    │
└──────┬──────────────────────┘
       │
       ↓
┌─────────────────────────────┐
│ SceneDataExtractor          │ ← Extraction step
│ .ExtractAllScenesFromSave() │
└──────┬──────────────────────┘
       │
       ↓ Extract and convert
┌─────────────────────────────┐
│ SceneData[] array           │
│ (Ready to use)              │
└──────┬──────────────────────┘
       │
       ↓ For each scene
┌─────────────────────────────┐
│ SceneManager                │
│ .LoadScene(scene_data)      │ ← Same method!
└─────────────────────────────┘
```

**Key Points**:
- ⚠️ Additional extraction step
- ⚠️ Navigate nested structure
- ✅ Unified consumption (same LoadScene)
- ✅ Clear conversion point

---

## Provider Responsibility Diagram

```
┌────────────────────────────────────────────────────────┐
│              Data Provider Responsibilities             │
└────────────────────────────────────────────────────────┘

ISceneDataProvider
┌──────────────────────────────────────┐
│ Responsibility:                      │
│ - Load default/template scene config │
│ - Provide fresh scene data           │
│                                      │
│ Input:  SceneType enum               │
│ Output: SceneData struct             │
│                                      │
│ Files: data/scenes/*.scene_data.bin  │
└──────────────────────────────────────┘

ISaveDataProvider
┌──────────────────────────────────────┐
│ Responsibility:                      │
│ - Load saved game data               │
│ - Manage save slots                  │
│ - Provide scene states (future)      │
│                                      │
│ Input:  Slot index                   │
│ Output: SaveData struct              │
│                                      │
│ Files: saves/slot_X/save_data.bin    │
└──────────────────────────────────────┘

SceneDataExtractor (Future)
┌──────────────────────────────────────┐
│ Responsibility:                      │
│ - Convert saved → standard format    │
│ - Extract scenes from nested data    │
│                                      │
│ Input:  SaveData struct              │
│ Output: SceneData[] array            │
│                                      │
│ Files: None (in-memory conversion)   │
└──────────────────────────────────────┘
```

---

## Data Transformation Pipeline

### Pipeline 1: Default Scene Data

```
Step 1: File I/O
┌─────────────────────────┐
│ title.scene_data.bin    │ Binary FlatBuffers file
└───────────┬─────────────┘
            │
Step 2: FlatBuffers Deserialization
            ↓
┌─────────────────────────┐
│ SceneDataFbs*           │ FlatBuffers table pointer
└───────────┬─────────────┘
            │
Step 3: Provider Conversion
            ↓
┌─────────────────────────┐
│ SceneData               │ C++ struct
│ (Mutable, Native)       │
└───────────┬─────────────┘
            │
Step 4: Consumption
            ↓
┌─────────────────────────┐
│ LoadScene(scene_data)   │ Scene loading logic
└─────────────────────────┘

Total Steps: 4
Complexity: Low
```

### Pipeline 2: Saved Scene Data (Future)

```
Step 1: File I/O
┌─────────────────────────┐
│ save_data.bin           │ Binary FlatBuffers file
└───────────┬─────────────┘
            │
Step 2: FlatBuffers Deserialization
            ↓
┌─────────────────────────┐
│ SaveData (FlatBuffers)  │ FlatBuffers table pointer
└───────────┬─────────────┘
            │
Step 3: Provider Conversion
            ↓
┌─────────────────────────┐
│ SaveData (C++ struct)   │ C++ struct (nested)
│ └─ scene_states[]       │
└───────────┬─────────────┘
            │
Step 4: Extraction
            ↓
┌─────────────────────────┐
│ SceneDataExtractor      │ Flatten nested structure
└───────────┬─────────────┘
            │
Step 5: Flattened Data
            ↓
┌─────────────────────────┐
│ SceneData[]             │ C++ struct array
│ (Mutable, Native)       │
└───────────┬─────────────┘
            │
Step 6: Consumption
            ↓
┌─────────────────────────┐
│ LoadScene(scene_data)   │ Scene loading logic (same!)
└─────────────────────────┘

Total Steps: 6
Complexity: Medium
Additional Step: Extraction (Step 4)
```

---

## Decision Tree: Which Provider to Use?

```
                  ┌─────────────────┐
                  │  Need Scene     │
                  │     Data?       │
                  └────────┬────────┘
                           │
           ┌───────────────┴───────────────┐
           │                               │
           ↓                               ↓
  ┌────────────────┐            ┌─────────────────┐
  │  New Game or   │            │  Load Saved     │
  │  Default Scene?│            │     Game?       │
  └────────┬───────┘            └────────┬────────┘
           │                              │
           │ Yes                          │ Yes
           ↓                              ↓
  ┌────────────────────┐        ┌──────────────────────┐
  │ISceneDataProvider  │        │ISaveDataProvider     │
  │.LoadSceneData()    │        │.LoadSave()           │
  └────────┬───────────┘        └──────────┬───────────┘
           │                                │
           │ Returns SceneData              │ Returns SaveData
           ↓                                ↓
  ┌────────────────────┐        ┌──────────────────────┐
  │  Use Directly      │        │SceneDataExtractor    │
  │                    │        │.Extract()            │
  └────────┬───────────┘        └──────────┬───────────┘
           │                                │
           │                                │ Returns SceneData[]
           └──────────┬─────────────────────┘
                      │
                      ↓
          ┌───────────────────────┐
          │ LoadScene(scene_data) │
          └───────────────────────┘
```

---

## Memory Layout Comparison

### Default Scene Data: Flat Structure

```
Memory Layout (Simple)
┌──────────────────────────────────────┐
│         SceneData Object             │
│                                      │
│  +0x00: scene_type (4 bytes)        │
│  +0x04: padding (4 bytes)           │
│  +0x08: scene_id (string)           │
│         - ptr (8 bytes)              │
│         - data (heap)                │
│  +0x10: render_texture_width (4 b)  │
│  +0x14: render_texture_height (4 b) │
│                                      │
│  Total Stack: ~32 bytes              │
│  Total Heap:  ~64 bytes (string)     │
└──────────────────────────────────────┘

Access: Direct member access
Complexity: O(1)
```

### Saved Scene Data: Nested Structure

```
Memory Layout (Nested)
┌──────────────────────────────────────┐
│        SaveData Object               │
│                                      │
│  +0x00: metadata (struct)            │
│         - save_name                  │
│         - created_at                 │
│         - ...                        │
│  +0x50: current_scene_type           │
│  +0x58: version                      │
│  +0x60: scene_states (vector)        │
│         - ptr → heap array           │
│                                      │
│  Nested in heap:                     │
│  ┌────────────────────────────────┐  │
│  │ SceneStateData[0]              │  │
│  │  - scene_id                    │  │
│  │  - scene_type                  │  │
│  │  - entity_collection           │  │
│  ├────────────────────────────────┤  │
│  │ SceneStateData[1]              │  │
│  │  - ...                         │  │
│  └────────────────────────────────┘  │
│                                      │
│  Total Stack: ~128 bytes             │
│  Total Heap:  Variable (KB-MB)       │
└──────────────────────────────────────┘

Access: Iterate vector, extract fields
Complexity: O(n) for n scenes
```

---

## Implementation Phases

### Phase 1: Current State ✅

```
┌─────────────────────────────────────┐
│     Implemented and Working         │
├─────────────────────────────────────┤
│                                     │
│  [✅] ISceneDataProvider            │
│  [✅] FlatbuffersSceneDataProvider  │
│  [✅] SceneData struct              │
│  [✅] LoadSceneData(SceneType)      │
│                                     │
│  [✅] ISaveDataProvider             │
│  [✅] FlatbuffersSaveDataProvider   │
│  [✅] SaveData struct (minimal)     │
│  [✅] LoadSave(slot_index)          │
│                                     │
└─────────────────────────────────────┘

Status: Complete
Usage: New game works, basic save/load works
```

### Phase 2: Future Enhancement

```
┌─────────────────────────────────────┐
│      Planned for Future             │
├─────────────────────────────────────┤
│                                     │
│  [ ] Add scene_states to SaveData   │
│  [ ] Update save_data.fbs schema    │
│  [ ] Implement SceneDataExtractor   │
│  [ ] Add extraction methods         │
│  [ ] Update GameManager load path   │
│  [ ] Write extractor tests          │
│                                     │
└─────────────────────────────────────┘

Status: Not yet implemented
Trigger: When scene state saving is needed
Timeline: When entity serialization is done
```

### Phase 3: Long-term (Optional)

```
┌─────────────────────────────────────┐
│     Optional Future Enhancement      │
├─────────────────────────────────────┤
│                                     │
│  [ ] ISceneDataView interface       │
│  [ ] View pattern implementation    │
│  [ ] Zero-copy navigation           │
│  [ ] Performance optimization       │
│                                     │
└─────────────────────────────────────┘

Status: Deferred
Reason: Current approach sufficient
Reconsider: If save files become very large
```

---

## Summary Visual

```
┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃   Scene Data Access: Two Patterns, One Goal  ┃
┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

Pattern 1: Direct                Pattern 2: Nested
──────────────────              ──────────────────
Default Scene Data              Saved Scene Data
        ↓                               ↓
ISceneDataProvider              ISaveDataProvider
        ↓                               ↓
  SceneData                        SaveData
   (Ready)                             ↓
        │                      SceneDataExtractor
        │                               ↓
        │                         SceneData[]
        │                          (Ready)
        │                               │
        └───────────┬───────────────────┘
                    ↓
        ┌───────────────────────┐
        │ LoadScene(scene_data) │
        │  (Unified Code Path)  │
        └───────────────────────┘

Key Insight: Both patterns converge to SceneData
             for unified downstream processing
```

---

**Visual Guide Complete**: December 7, 2025
