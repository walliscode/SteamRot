# SceneData and FbsSceneData Memory Management Analysis

## Overview

This document analyzes critical memory management issues in the `SceneData` and `FbsSceneData` classes, specifically focusing on the improper storage of FlatBuffers-generated data pointers.

**Date:** 2026-01-03  
**Issue:** Memory leak and dangling pointer in FbsSceneData::scene_data_fbs  
**Severity:** Critical - Results in memory leaks and undefined behavior  

---

## Problem Statement

### Current Design

The `FbsSceneData` struct stores a raw pointer to FlatBuffers-generated data:

```cpp
// src/types/core/FbsSceneData.h
struct FbsSceneData : public SceneData {
  const SceneDataFbs *scene_data_fbs{nullptr};  // ← Raw pointer to FlatBuffers data
};
```

The base class `SceneData` contains:

```cpp
// src/types/core/SceneData.h
struct SceneData {
  virtual ~SceneData() = default;
  SceneInfo scene_info;
  AssetConfig scene_asset_config;
};
```

### Data Flow and Memory Management Issues

#### 1. Memory Allocation in DataLoader

The `DataLoader::LoadBinaryData()` allocates raw memory:

```cpp
// src/data_providers/DataLoader.cpp
char *DataLoader::LoadBinaryData(const std::filesystem::path &file_path) const {
  std::ifstream infile;
  infile.open(file_path, std::ios::binary | std::ios::in);
  infile.seekg(0, std::ios::end);
  int length = infile.tellg();
  infile.seekg(0, std::ios::beg);
  char *data = new char[length];  // ← Raw allocation with new[]
  infile.read(data, length);
  infile.close();
  return data;
}
```

**Issue:** Memory allocated with `new[]` is never freed with `delete[]`.

#### 2. FlatBuffers Pointer Creation

The `FlatbuffersDataLoader` creates FlatBuffers pointers from this raw data:

```cpp
// src/data_providers/FlatbuffersDataLoader.cpp
std::expected<const SceneDataFbs *, FailInfo>
FlatbuffersDataLoader::ProvideDefaultSceneData(const SceneType scene_type) const {
  // ... file path construction ...
  
  // load the scene data
  const steamrot::SceneDataFbs *scene_data =
      GetSceneDataFbs(LoadBinaryData(scene_path));  // ← Creates pointer to unmanaged memory
  
  return scene_data;
}
```

**Issue:** The `SceneDataFbs*` points to memory allocated by `LoadBinaryData()`, but that memory has no ownership tracking.

#### 3. Storage in FbsSceneData

The `FlatbuffersSceneDataProvider` stores the pointer:

```cpp
// src/data_providers/FlatbuffersSceneDataProvider.cpp
std::expected<std::unique_ptr<SceneData>, FailInfo>
FlatbuffersSceneDataProvider::ProvideDefaultSceneData(const SceneType scene_type) const {
  FlatbuffersDataLoader data_loader;
  auto load_data_result = data_loader.ProvideDefaultSceneData(scene_type);
  if (!load_data_result.has_value()) {
    return std::unexpected(load_data_result.error());
  }
  const SceneDataFbs &fb_data = *load_data_result.value();
  
  // create SceneData
  FbsSceneData scene_data;
  
  scene_data.scene_info.type = scene_type;
  scene_data.scene_data_fbs = load_data_result.value();  // ← Storing raw pointer
  
  // ... configure AssetConfig ...
  
  return std::make_unique<FbsSceneData>(scene_data);
}
```

**Issues:**
1. `scene_data_fbs` holds a raw pointer to FlatBuffers data
2. The underlying `char*` buffer from `LoadBinaryData()` is never tracked or freed
3. No ownership semantics - who is responsible for cleanup?
4. When `FbsSceneData` is destroyed, the FlatBuffers data is not freed

#### 4. Usage in Configurators

The `FlatbuffersSceneConfigurator` uses this pointer extensively:

```cpp
// src/scenes/FlatbuffersSceneConfigurator.cpp
std::expected<std::monostate, FailInfo>
FlatbuffersSceneConfigurator::ConfigureSceneResources(
    Scene &scene, const SceneData *scene_data) {
  
  // cast to derived SceneData type
  FbsSceneData *fbs_scene_data =
      dynamic_cast<FbsSceneData *>(const_cast<SceneData *>(scene_data));
  
  // check for SceneDataFbs
  if (!fbs_scene_data->scene_data_fbs)
    return std::unexpected(
        FailInfo(FailMode::FlatbuffersDataNotFound, "SceneDataFbs not found"));
  
  // Access FlatBuffers data
  sf::Vector2u size{
      fbs_scene_data->scene_data_fbs->scene_resources()->texture_width(),
      fbs_scene_data->scene_data_fbs->scene_resources()->texture_height()};
  
  // ...
}
```

**Issue:** The pointer is used but never checked if the underlying memory is still valid.

---

## Identified Problems

### 1. **Memory Leak**

**Severity:** Critical

The `char*` allocated by `LoadBinaryData()` is never freed:

```cpp
char *data = new char[length];  // Allocated
// ... used to create FlatBuffers pointer ...
// NEVER freed with delete[]
```

**Impact:**
- Every scene load leaks memory equal to the size of the scene data file
- Repeated scene transitions will accumulate leaked memory
- Long-running game sessions will eventually exhaust memory

### 2. **Unclear Ownership Semantics**

**Severity:** High

The `FbsSceneData::scene_data_fbs` member has no clear ownership:

```cpp
const SceneDataFbs *scene_data_fbs{nullptr};
```

**Questions without answers:**
- Who owns this pointer?
- Who is responsible for freeing the underlying buffer?
- What is the lifetime of the pointed-to data?
- Can this pointer become dangling?

**Impact:**
- Maintenance burden - developers must trace through code to understand ownership
- Risk of double-free or use-after-free bugs
- Difficult to reason about object lifetimes

### 3. **Violation of RAII Principles**

**Severity:** High

The current design violates RAII (Resource Acquisition Is Initialization):

- Resource (memory) is acquired in `LoadBinaryData()`
- Resource is never released
- No automatic cleanup on destruction
- Manual memory management in modern C++

**Impact:**
- Error-prone code
- Difficult to maintain
- Doesn't leverage C++ automatic resource management

### 4. **Const-Casting Away Constness**

**Severity:** Medium

The configurator performs const-casting:

```cpp
FbsSceneData *fbs_scene_data =
    dynamic_cast<FbsSceneData *>(const_cast<SceneData *>(scene_data));
```

**Issues:**
- The `SceneData*` parameter is `const`, indicating read-only intent
- Casting away `const` is necessary because of the interface design
- This is a code smell indicating poor interface design

**Impact:**
- Undermines const-correctness
- Can lead to unintended modifications
- Makes code harder to reason about

### 5. **Tight Coupling Between Layers**

**Severity:** Medium

`FbsSceneData` tightly couples the domain model with FlatBuffers implementation:

- `SceneData` is the abstract interface
- `FbsSceneData` is the FlatBuffers-specific implementation
- But `FbsSceneData` exposes raw FlatBuffers pointer publicly

**Issues:**
- Leaks implementation details
- Makes it difficult to swap serialization formats
- Violates information hiding principle

---

## Root Cause Analysis

The fundamental issue is **mixing ownership models**:

1. **Raw Pointers (C-style):** `LoadBinaryData()` returns `char*`
2. **FlatBuffers Pointers:** `GetSceneDataFbs()` returns `const SceneDataFbs*` 
3. **Smart Pointers (Modern C++):** `ProvideDefaultSceneData()` returns `unique_ptr<SceneData>`

The ownership of the `char*` buffer is lost, but that buffer is required for the lifetime of the `SceneDataFbs*` to remain valid.

**FlatBuffers Requirement:**
FlatBuffers operates on raw memory buffers. The `SceneDataFbs*` returned by `GetSceneDataFbs()` is actually just a typed pointer into the buffer. The buffer **must remain valid** for the entire lifetime of any `SceneDataFbs*` pointing into it.

**Current Failure:**
The buffer's lifetime is unmanaged, so we have:
- A `SceneDataFbs*` that points into a buffer
- No tracking of that buffer
- No cleanup of that buffer
- No guarantee the buffer outlives the pointer

---

## Impact Assessment

### Functional Impact
- **Memory Leaks:** Confirmed - every scene load leaks memory
- **Potential Crashes:** If the buffer is somehow freed elsewhere, using `scene_data_fbs` causes undefined behavior
- **Resource Exhaustion:** Long-running sessions will accumulate leaks

### Code Quality Impact
- **Maintainability:** Poor - unclear ownership makes modifications risky
- **Testability:** Difficult to test memory management in isolation
- **Readability:** Code requires deep understanding of multiple layers to comprehend

### Performance Impact
- **Memory Usage:** Grows unbounded with scene transitions
- **Allocation Overhead:** Each scene load allocates but never frees

---

## Recommendations

### Option 1: Store Buffer with RAII Wrapper (Recommended)

Modify `FbsSceneData` to own the underlying buffer:

```cpp
// src/types/core/FbsSceneData.h
#include <memory>

struct FbsSceneData : public SceneData {
  // Store the raw buffer with proper ownership
  std::unique_ptr<char[]> fbs_buffer;
  
  // FlatBuffers pointer into the buffer
  const SceneDataFbs *scene_data_fbs{nullptr};
  
  // Custom destructor to ensure proper cleanup order
  ~FbsSceneData() override {
    // scene_data_fbs is invalidated before buffer is freed
    // This is handled automatically by member destruction order
  }
};
```

Update `FlatbuffersSceneDataProvider`:

```cpp
std::expected<std::unique_ptr<SceneData>, FailInfo>
FlatbuffersSceneDataProvider::ProvideDefaultSceneData(
    const SceneType scene_type) const {
  
  // Load the raw buffer
  FlatbuffersDataLoader data_loader;
  // Modify ProvideDefaultSceneData to return the buffer
  auto load_result = data_loader.ProvideDefaultSceneDataWithBuffer(scene_type);
  if (!load_result.has_value()) {
    return std::unexpected(load_result.error());
  }
  
  auto [buffer, scene_data_fbs] = std::move(load_result.value());
  
  // Create FbsSceneData with proper ownership
  auto fbs_scene_data = std::make_unique<FbsSceneData>();
  fbs_scene_data->fbs_buffer = std::move(buffer);
  fbs_scene_data->scene_data_fbs = scene_data_fbs;
  fbs_scene_data->scene_info.type = scene_type;
  
  // Configure AssetConfig
  if (scene_data_fbs->asset_config()) {
    auto result = ConfigureAssetConfig(
        fbs_scene_data->scene_asset_config, 
        scene_data_fbs->asset_config());
    if (!result.has_value()) {
      return std::unexpected(result.error());
    }
  }
  
  return fbs_scene_data;
}
```

Update `DataLoader`:

```cpp
// Return unique_ptr instead of raw pointer
std::unique_ptr<char[]> LoadBinaryData(const std::filesystem::path &file_path) const {
  std::ifstream infile(file_path, std::ios::binary | std::ios::in);
  infile.seekg(0, std::ios::end);
  size_t length = infile.tellg();
  infile.seekg(0, std::ios::beg);
  
  auto data = std::make_unique<char[]>(length);
  infile.read(data.get(), length);
  
  return data;
}
```

**Advantages:**
- ✅ Automatic memory management via RAII
- ✅ Clear ownership semantics
- ✅ No memory leaks
- ✅ Buffer lifetime guaranteed to exceed pointer lifetime
- ✅ Minimal changes to existing interface

**Disadvantages:**
- Requires changes to multiple files
- Need to ensure proper destruction order (buffer after FlatBuffers pointer)

### Option 2: Use FlatBuffers Buffer Wrapper

Use FlatBuffers' own buffer management:

```cpp
#include "flatbuffers/flatbuffers.h"

struct FbsSceneData : public SceneData {
  // FlatBuffers owns the buffer
  std::unique_ptr<flatbuffers::DetachedBuffer> fbs_buffer;
  
  // Pointer into the buffer
  const SceneDataFbs *scene_data_fbs{nullptr};
};
```

**Advantages:**
- ✅ Uses FlatBuffers' intended buffer management
- ✅ Integrates well with FlatBuffers ecosystem
- ✅ Proper lifetime management

**Disadvantages:**
- Requires loading data through FlatBuffers' API
- May require restructuring data loading

### Option 3: Copy Data Out of FlatBuffers (Not Recommended)

Copy all needed data from FlatBuffers into `SceneData` members:

```cpp
struct SceneData {
  virtual ~SceneData() = default;
  SceneInfo scene_info;
  AssetConfig scene_asset_config;
  
  // Add all data fields here
  SceneResources scene_resources;
  EntityCollection entity_collection;
  LogicCollectionData logic_collection;
};
```

**Advantages:**
- ✅ No FlatBuffers pointers to manage
- ✅ Clear ownership - all data is value-based

**Disadvantages:**
- ❌ Requires duplicating entire data structure
- ❌ Performance overhead of copying large data
- ❌ Defeats the purpose of zero-copy FlatBuffers design
- ❌ Large refactoring effort

---

## Recommended Solution

**Option 1: Store Buffer with RAII Wrapper** is the recommended solution.

### Implementation Steps

1. **Modify `FbsSceneData`** to store the buffer:
   ```cpp
   struct FbsSceneData : public SceneData {
     std::unique_ptr<char[]> fbs_buffer;
     const SceneDataFbs *scene_data_fbs{nullptr};
   };
   ```

2. **Update `DataLoader::LoadBinaryData()`** to return `unique_ptr`:
   ```cpp
   std::unique_ptr<char[]> LoadBinaryData(const std::filesystem::path &file_path) const;
   ```

3. **Update `FlatbuffersDataLoader`** to return buffer and pointer:
   ```cpp
   std::expected<std::pair<std::unique_ptr<char[]>, const SceneDataFbs*>, FailInfo>
   ProvideDefaultSceneDataWithBuffer(const SceneType scene_type) const;
   ```

4. **Update `FlatbuffersSceneDataProvider`** to store both buffer and pointer

5. **Update tests** to verify memory is properly managed

### Benefits

- **Memory Safety:** Automatic cleanup, no leaks
- **Clear Ownership:** Buffer owned by `FbsSceneData`
- **Minimal Changes:** Incremental refactoring
- **Type Safety:** Smart pointers prevent misuse
- **Modern C++:** Leverages RAII properly

---

## Testing Considerations

After implementing the fix, verify:

1. **Memory Leak Tests:**
   - Use Valgrind or AddressSanitizer
   - Load and unload scenes repeatedly
   - Verify no memory is leaked

2. **Lifetime Tests:**
   - Ensure `scene_data_fbs` remains valid for entire `FbsSceneData` lifetime
   - Verify buffer is destroyed after FlatBuffers pointer

3. **Unit Tests:**
   - Test `FbsSceneData` construction and destruction
   - Test buffer ownership transfer
   - Test null/empty cases

4. **Integration Tests:**
   - Test full scene loading and configuration pipeline
   - Verify all FlatBuffers data is accessible

---

## Related Code

### Files to Modify
- `src/types/core/FbsSceneData.h` - Add buffer storage
- `src/data_providers/DataLoader.h/cpp` - Return `unique_ptr`
- `src/data_providers/FlatbuffersDataLoader.h/cpp` - Update API
- `src/data_providers/FlatbuffersSceneDataProvider.cpp` - Store buffer
- `tests/unit/data_providers/FlatbuffersSceneDataProvider.test.cpp` - Update tests

### Files to Review
- `src/scenes/FlatbuffersSceneConfigurator.cpp` - Verify usage still works
- All test files using `FbsSceneData`

---

## Conclusion

The current design has a critical memory leak and unclear ownership semantics. The recommended solution stores the FlatBuffers buffer with proper RAII semantics in `FbsSceneData`, ensuring automatic cleanup and clear ownership. This maintains FlatBuffers' zero-copy benefits while providing memory safety through modern C++ practices.

The issue represents a common pitfall when integrating C-style memory management (FlatBuffers) with modern C++ (smart pointers). The solution bridges this gap by wrapping the raw buffer in a `unique_ptr` while keeping the FlatBuffers pointer for efficient access.

---

## Appendix: Current Data Flow

```
┌─────────────────────────────────────────────────────────────┐
│ FlatbuffersSceneDataProvider::ProvideDefaultSceneData()    │
└─────────────────────┬───────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────┐
│ FlatbuffersDataLoader::ProvideDefaultSceneData()           │
│   - Constructs file path                                    │
└─────────────────────┬───────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────┐
│ DataLoader::LoadBinaryData()                                │
│   - Allocates: char *data = new char[length]   ← LEAKED!   │
│   - Returns: char*                                          │
└─────────────────────┬───────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────┐
│ GetSceneDataFbs(char*)                                      │
│   - FlatBuffers utility function                            │
│   - Returns: const SceneDataFbs* (pointer into buffer)      │
└─────────────────────┬───────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────┐
│ FbsSceneData::scene_data_fbs = pointer                      │
│   - Stores raw pointer                                      │
│   - Buffer ownership: UNKNOWN                               │
└─────────────────────┬───────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────┐
│ std::unique_ptr<SceneData> returned                         │
│   - FbsSceneData owned by unique_ptr                        │
│   - When destroyed, buffer NOT freed                        │
│   - MEMORY LEAK                                             │
└─────────────────────────────────────────────────────────────┘
```

**Legend:**
- ← Indicates the point where memory management fails
- Buffer allocated with `new[]` is never freed
- `SceneDataFbs*` pointer becomes dangling (undefined if used after buffer freed elsewhere)

---

## Appendix: Proposed Data Flow (After Fix)

```
┌─────────────────────────────────────────────────────────────┐
│ FlatbuffersSceneDataProvider::ProvideDefaultSceneData()    │
└─────────────────────┬───────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────┐
│ FlatbuffersDataLoader::ProvideDefaultSceneDataWithBuffer() │
│   - Constructs file path                                    │
└─────────────────────┬───────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────┐
│ DataLoader::LoadBinaryData()                                │
│   - Allocates: auto data = make_unique<char[]>(length)     │
│   - Returns: unique_ptr<char[]>  ← Ownership tracked!      │
└─────────────────────┬───────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────┐
│ GetSceneDataFbs(buffer.get())                               │
│   - FlatBuffers utility function                            │
│   - Returns: const SceneDataFbs* (pointer into buffer)      │
│   - Returns: pair<unique_ptr<char[]>, const SceneDataFbs*> │
└─────────────────────┬───────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────┐
│ FbsSceneData construction                                   │
│   - fbs_buffer = move(buffer)  ← Buffer ownership!         │
│   - scene_data_fbs = pointer                                │
└─────────────────────┬───────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────┐
│ std::unique_ptr<SceneData> returned                         │
│   - FbsSceneData owned by unique_ptr                        │
│   - When destroyed:                                         │
│     1. scene_data_fbs invalidated                           │
│     2. fbs_buffer automatically freed  ← RAII!              │
│   - NO MEMORY LEAK                                          │
└─────────────────────────────────────────────────────────────┘
```

**Legend:**
- ← Indicates ownership tracking points
- Buffer managed by `unique_ptr<char[]>`
- Automatic cleanup when `FbsSceneData` is destroyed
- Memory safety guaranteed by RAII
