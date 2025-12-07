# Provider vs. Viewer Pattern - Scene Data Architecture

**Date**: December 7, 2025  
**Context**: Response to question "why not use a viewer pattern? so ISceneViewer"  
**Related**: SCENE_DATA_PROVIDER_ARCHITECTURE_ANALYSIS.md, SAVE_LOAD_WORKFLOW_ANALYSIS.md

---

## The Question

**"Why not use a viewer pattern? so ISceneViewer"**

Instead of:
```cpp
ISceneDataProvider& provider = GetSceneDataProvider();
auto scene_data = provider.LoadSceneData(SceneType::SceneType_TITLE);
```

Why not:
```cpp
ISceneViewer& viewer = GetSceneViewer();
auto scene_data = viewer.GetSceneData(SceneType::SceneType_TITLE);
```

---

## Short Answer

**Both patterns are valid.** The current `ISceneDataProvider` pattern was recommended because:

1. ✅ **Simpler for current needs** - SceneData is small and simple (4 fields)
2. ✅ **Already implemented** - Working code, no changes needed
3. ✅ **Consistent with codebase** - Matches existing provider patterns
4. ✅ **Adequate for now** - View pattern benefits don't justify migration cost

**However**, `ISceneViewer` would be beneficial when:
- Scene data becomes large/complex (entities, assets included)
- Need zero-copy access to nested data
- Want lazy loading capabilities
- Save files become very large (MB+)

---

## Pattern Comparison

### Provider Pattern (Current)

**Concept**: "Load and convert data to native format"

```cpp
class ISceneDataProvider {
  // Returns fully converted native struct
  virtual std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType scene_type) const = 0;
};

// Usage
ISceneDataProvider& provider = GetSceneDataProvider();
auto scene_data = provider.LoadSceneData(SceneType::SceneType_TITLE);
// scene_data is a native SceneData struct, ready to mutate
```

**Characteristics**:
- ✅ Returns native C++ structs
- ✅ Mutable data (can modify after load)
- ✅ Simple to use
- ✅ Hides data source completely
- ⚠️ Copies data from source (FlatBuffers) to native struct
- ⚠️ Eager loading (loads everything immediately)

---

### Viewer Pattern (Alternative)

**Concept**: "Provide a view into data without copying"

```cpp
class ISceneViewer {
  // Returns view interface to data
  virtual std::expected<std::unique_ptr<ISceneDataView>, FailInfo>
  GetSceneView(SceneType scene_type) const = 0;
};

class ISceneDataView {
  // Accessor methods (read-only)
  virtual SceneType GetSceneType() const = 0;
  virtual std::string GetSceneID() const = 0;
  virtual uint32_t GetRenderTextureWidth() const = 0;
  virtual uint32_t GetRenderTextureHeight() const = 0;
  
  // Can also provide conversion if needed
  virtual SceneData ToSceneData() const = 0;
};

// Usage
ISceneViewer& viewer = GetSceneViewer();
auto scene_view = viewer.GetSceneView(SceneType::SceneType_TITLE);
// scene_view provides read-only access, no copying
SceneType type = scene_view->GetSceneType();
```

**Characteristics**:
- ✅ Zero-copy access (views directly into FlatBuffers/source)
- ✅ Can support lazy loading
- ✅ Memory efficient for large data
- ✅ Can navigate nested structures efficiently
- ⚠️ Read-only (must copy to mutate)
- ⚠️ More complex API (getter methods vs direct fields)
- ⚠️ Buffer lifetime management (view must outlive source buffer)

---

## When to Use Each Pattern

### Use Provider Pattern When:

1. **Data is small/simple**
   - SceneData is only 4 fields (~64 bytes)
   - Copying is negligible cost

2. **Need mutability**
   - Want to modify data after loading
   - Need to build up structs incrementally

3. **Want simplicity**
   - Direct field access: `scene_data.render_texture_width`
   - No getter methods needed

4. **Already implemented**
   - Working code, no migration cost
   - Consistent with existing patterns

**Current SceneData is perfect for Provider pattern:**
```cpp
struct SceneData {
  SceneType scene_type;           // 4 bytes
  std::string scene_id;           // 32 bytes (with string object)
  uint32_t render_texture_width;  // 4 bytes
  uint32_t render_texture_height; // 4 bytes
};
// Total: ~64 bytes - trivial to copy
```

---

### Use Viewer Pattern When:

1. **Data is large/complex**
   - Contains arrays of entities
   - Contains nested UI hierarchies
   - Includes large asset lists
   - Total size: KB to MB range

2. **Zero-copy is important**
   - Performance critical path
   - Large datasets
   - Frequent access patterns

3. **Lazy loading needed**
   - Don't want to load everything upfront
   - Only access subset of data
   - Navigation pattern (drill down into nested data)

4. **Read-only access sufficient**
   - Just reading data for configuration
   - Don't need to mutate

**Example where Viewer shines:**
```cpp
// If SceneData included full entity collections
struct LargeSceneData {
  SceneType scene_type;
  std::string scene_id;
  uint32_t render_texture_width;
  uint32_t render_texture_height;
  std::vector<EntityData> entities;        // Could be 100+ entities
  std::vector<AssetInfo> assets;           // Could be dozens of assets
  UIElementHierarchy ui_tree;              // Deep nested structure
  std::vector<LogicConfig> logic_configs;  // Configuration data
};
// Total: 10KB - 1MB+ - Viewer pattern beneficial!
```

---

## Existing Pattern in Codebase: ISubscriberViewer

The codebase already uses a Viewer pattern for subscribers:

```cpp
// src/data_providers/ISubscriberViewer.h
class ISubscriberViewer {
public:
  virtual std::expected<std::vector<std::shared_ptr<Subscriber>>, FailInfo>
  GetSubscribers() const = 0;
};
```

**Why Viewer here?**
- Subscribers can be accessed from multiple data sources
- Used as a **mixin** (composition pattern)
- Provides unified access regardless of source
- Returns shared_ptrs (can share ownership)

**Key difference from Provider:**
- Viewer = "view into existing data" (read-only concept)
- Provider = "load and convert data" (transformation concept)

---

## Recommendation for Scene Data

### Current State: Keep Provider Pattern ✅

**Reasons:**

1. **Adequate Performance**
   ```cpp
   // Copying 64 bytes is ~1-2 nanoseconds
   // Not a bottleneck
   SceneData data = provider.LoadSceneData(type);
   ```

2. **Simplicity Wins**
   ```cpp
   // Provider: Direct field access
   uint32_t width = scene_data.render_texture_width;
   
   // vs Viewer: Getter methods
   uint32_t width = scene_view->GetRenderTextureWidth();
   ```

3. **Mutability Needed**
   ```cpp
   // Can modify after load (useful for testing, overrides)
   scene_data.render_texture_width = 1920;
   ```

4. **No Migration Cost**
   - Already implemented
   - Already working
   - No issues identified

---

### Future: Consider Viewer When Scene Data Expands

**Scenario 1: If SceneData Includes Entities**

```cpp
// Current: Small, simple
struct SceneData {
  SceneType scene_type;
  std::string scene_id;
  uint32_t render_texture_width;
  uint32_t render_texture_height;
};

// Future: Large, complex
struct ExpandedSceneData {
  // Core fields (same as now)
  SceneType scene_type;
  std::string scene_id;
  uint32_t render_texture_width;
  uint32_t render_texture_height;
  
  // NEW: Heavy data
  std::vector<EntityData> entities;  // 100+ entities × 1KB each = 100KB+
  AssetCollection assets;            // Texture paths, fonts, etc.
  UIElementTree ui_hierarchy;        // Deep nested structure
  LogicConfiguration logic_data;     // Logic setup
};

// At this point, Viewer pattern becomes attractive
```

**Then use Viewer:**
```cpp
class ISceneViewer {
  // Core data as native struct (small, mutable)
  virtual SceneData GetSceneCore() const = 0;
  
  // Heavy data as views (large, read-only, zero-copy)
  virtual IEntityCollectionView GetEntities() const = 0;
  virtual IAssetCollectionView GetAssets() const = 0;
  virtual IUIHierarchyView GetUIHierarchy() const = 0;
};
```

**Scenario 2: If Save Files Become Large**

```cpp
// Currently, save files are small (metadata only)
// Future: Could be MB+ with full game state

// Viewer pattern allows:
ISceneViewer& viewer = GetSceneViewer(SaveSource::FromFile("save.bin"));

// Navigate without loading everything
auto scene_count = viewer.GetSceneCount();
for (size_t i = 0; i < scene_count; ++i) {
  auto scene_view = viewer.GetSceneView(i);
  // Only load what we need
  if (scene_view->GetSceneType() == SceneType::SceneType_CRAFTING) {
    // Load this one fully
    SceneData data = scene_view->ToSceneData();
    LoadScene(data);
  }
}
```

---

## Hybrid Approach (Best of Both Worlds)

### Option: Provider Returns View, View Can Convert

```cpp
class ISceneDataProvider {
  // Returns view for efficiency
  virtual std::expected<std::unique_ptr<ISceneDataView>, FailInfo>
  LoadSceneView(SceneType scene_type) const = 0;
};

class ISceneDataView {
  // View methods (zero-copy, read-only)
  virtual SceneType GetSceneType() const = 0;
  virtual std::string GetSceneID() const = 0;
  virtual uint32_t GetRenderTextureWidth() const = 0;
  virtual uint32_t GetRenderTextureHeight() const = 0;
  
  // Convert to native struct if mutation needed
  virtual SceneData ToSceneData() const = 0;
};

// Usage - Best of both worlds
ISceneDataProvider& provider = GetSceneDataProvider();

// Option 1: Zero-copy view (efficient)
auto view = provider.LoadSceneView(SceneType::SceneType_TITLE);
uint32_t width = view->GetRenderTextureWidth();

// Option 2: Mutable native struct (when needed)
SceneData data = view->ToSceneData();
data.render_texture_width = 1920;
LoadScene(data);
```

**Benefits:**
- ✅ Efficient by default (view)
- ✅ Mutable when needed (convert)
- ✅ Flexible for different use cases
- ⚠️ More complex API

---

## Comparison Table

| Aspect | Provider Pattern | Viewer Pattern | Hybrid |
|--------|------------------|----------------|--------|
| **Simplicity** | ✅ Simple | ⚠️ More complex | ⚠️ Most complex |
| **Performance** | ⚠️ Copies data | ✅ Zero-copy | ✅ Zero-copy default |
| **Mutability** | ✅ Mutable | ❌ Read-only | ✅ Via conversion |
| **API Style** | Direct fields | Getter methods | Both |
| **Memory** | ⚠️ Duplicates data | ✅ No duplication | ✅ No duplication |
| **Buffer Lifetime** | ✅ Independent | ⚠️ Must manage | ⚠️ Must manage |
| **Best For** | Small, simple data | Large, complex data | Large data, need mutation |
| **Current SceneData** | ✅ Perfect fit | ⚠️ Overkill | ⚠️ Overkill |

---

## Decision Criteria

### Stay with Provider Pattern If:

- [x] SceneData remains small (<1KB)
- [x] Need to mutate data frequently
- [x] Direct field access preferred
- [x] No performance issues identified
- [x] Already implemented and working

**Current state: All criteria met** ✅

---

### Migrate to Viewer Pattern When:

- [ ] SceneData grows to include entities/assets (10KB+)
- [ ] Performance profiling shows copy overhead
- [ ] Need lazy loading for large save files
- [ ] Want to support efficient nested navigation
- [ ] Have resources for migration (40-60 hours)

**Current state: No criteria met** ❌

---

## Answer to Original Question

### "Why not use a viewer pattern? so ISceneViewer"

**Three-part answer:**

1. **Current Pattern is Sufficient**
   - SceneData is small (64 bytes)
   - Provider pattern is simple and appropriate
   - No performance issues
   - Already implemented

2. **Viewer Would Be Overkill Now**
   - Added complexity for no current benefit
   - SceneData too simple to justify view abstraction
   - Zero-copy optimization not needed for 64 bytes

3. **Viewer is the Future Path**
   - When SceneData includes entities/assets
   - When save files grow large
   - See "Solution 3: View Pattern" in analysis docs
   - Deferred as Phase 3 enhancement

---

## Naming Consistency Note

If we were to use Viewer pattern, naming should follow existing pattern:

```cpp
// Current naming pattern in codebase
class ISubscriberViewer {
  virtual std::expected<std::vector<std::shared_ptr<Subscriber>>, FailInfo>
  GetSubscribers() const = 0;
};

// Consistent naming for scene viewer would be
class ISceneDataViewer {  // Not ISceneViewer - include "Data" for consistency
  virtual std::expected<std::unique_ptr<ISceneDataView>, FailInfo>
  GetSceneDataView(SceneType scene_type) const = 0;
};
```

Or keep Provider naming:
```cpp
// Current
class ISceneDataProvider {
  virtual std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType scene_type) const = 0;
};

// If returning views, keep Provider name but change return type
class ISceneDataProvider {
  virtual std::expected<std::unique_ptr<ISceneDataView>, FailInfo>
  LoadSceneView(SceneType scene_type) const = 0;
};
```

---

## Conclusion

**Current recommendation stands: Keep Provider pattern.**

Reasons:
1. Current SceneData is small and simple
2. Provider pattern is adequate and appropriate
3. No issues identified with current approach
4. Migration cost not justified by benefits

**Viewer pattern is documented as future enhancement** when:
- Scene data grows to include heavy nested data
- Performance becomes critical
- Save files become large

See **SCENE_DATA_PROVIDER_ARCHITECTURE_ANALYSIS.md** "Solution 3: View Pattern (Future Enhancement)" for detailed view pattern design when needed.

---

## Related Documentation

- **Scene Provider Analysis**: SCENE_DATA_PROVIDER_ARCHITECTURE_ANALYSIS.md
- **View Pattern Details**: SAVE_LOAD_WORKFLOW_ANALYSIS.md
- **Current Architecture**: ../DATA_PROVIDER_SYSTEM.md
- **Subscriber Viewer Example**: src/data_providers/ISubscriberViewer.h

---

**Analysis Complete**: December 7, 2025
