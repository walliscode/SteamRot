# Architecture Improvement Recommendations - Executive Summary

**Date:** 2026-01-07  
**Status:** Analysis Complete, Implementation Pending

---

## Quick Overview

This is a **documentation-only analysis** of the SteamRot architecture. The findings show the codebase is fundamentally sound but has opportunities for improvement in layer organization, dependency management, and build efficiency.

**Key Finding:** The architecture exhibits some **layer mixing** and **circular dependencies** that can be resolved with targeted refactoring.

---

## Top 5 Recommendations

### 1. ✅ Move Data Structures to types/core/ (PRIORITY: HIGH)

**Problem:**
- `SceneLoadData.h` and `SceneInfoProvider.h` are in `interfaces/` but they're data structures, not interfaces
- This creates confusion about what belongs in `interfaces/`

**Solution:**
```bash
# Move files
mv src/interfaces/SceneLoadData.h src/types/core/
mv src/interfaces/SceneInfoProvider.h src/types/core/

# Update includes in all files
find src tests -name "*.cpp" -o -name "*.h" | xargs sed -i 's|"SceneLoadData.h"|"SceneLoadData.h"|g'
```

**Benefits:**
- ✅ Clear separation: interfaces = abstractions, types = data
- ✅ Better discoverability
- ✅ Follows C++ conventions

**Risk:** LOW (just file moves)  
**Effort:** 1-2 hours  
**Impact:** Immediate clarity improvement

---

### 2. ✅ Convert Components to Header-Only (PRIORITY: HIGH)

**Problem:**
- Components are **pure data structs** but built as a **NORMAL library**
- Each component has a `.cpp` file just for `GetComponentRegisterIndex()` (one line!)
- This adds unnecessary compilation overhead

**Current:**
```cpp
// Component.h
struct Component {
  virtual ~Component();  // Needs .cpp file
  virtual size_t GetComponentRegisterIndex() const = 0;
  bool m_active{false};
};

// CMeta.cpp (entire file for one line!)
size_t CMeta::GetComponentRegisterIndex() const {
  return TupleTypeIndex<CMeta, ComponentRegister>;
}
```

**Proposed:**
```cpp
// types/components/Component.h
struct Component {
  virtual ~Component() = default;  // Inline
  virtual constexpr size_t GetComponentRegisterIndex() const = 0;
  bool m_active{false};
};

// types/components/CMeta.h
struct CMeta : public Component {
  constexpr size_t GetComponentRegisterIndex() const override {
    return TupleTypeIndex<CMeta, ComponentRegister>;
  }
};
```

**Benefits:**
- ✅ No compilation overhead for components
- ✅ Faster incremental builds (no recompilation needed)
- ✅ Makes it clearer that components are data-only
- ✅ Reduces coupling

**Risk:** MEDIUM (requires code refactoring)  
**Effort:** 4-6 hours  
**Impact:** Significant build time improvement

---

### 3. ✅ Merge types and interfaces (PRIORITY: MEDIUM)

**Problem:**
- `types` and `interfaces` are both INTERFACE libraries
- They have a **circular dependency**: types → interfaces, interfaces → types
- This creates confusion about the foundation layer

**Solution:**
Merge into single `types` library organized by subdirectory:
```
types/
├── core/           # Core data structures
├── components/     # Component definitions
├── interfaces/     # Abstract interfaces
├── events/         # Event types
├── assets/         # Asset types
├── user_interface/ # UI element types
└── flatbuffers/    # Generated headers
```

**Benefits:**
- ✅ Eliminates circular dependency
- ✅ Single foundation layer
- ✅ Easier mental model: "types contains all type definitions"
- ✅ Reduces CMake complexity

**Risk:** MEDIUM (many CMakeLists.txt changes)  
**Effort:** 2-4 hours  
**Impact:** Clearer architecture, easier maintenance

---

### 4. ⚠️ Remove configuration and resources Libraries (PRIORITY: MEDIUM)

**Problem:**
- `configuration` is an INTERFACE library with only 2 header files
- `resources` is an INTERFACE library with only 1 header file
- Both primarily serve as **dependency aggregators** without clear added value

**Current:**
```cmake
# configuration/CMakeLists.txt
add_library(configuration INTERFACE)
target_link_libraries(configuration INTERFACE types interfaces events SFML::Graphics)

# resources/CMakeLists.txt
add_library(resources INTERFACE)
target_link_libraries(resources INTERFACE types assets events)
```

**Proposed:**
- Move `IUIElementConfigurator.h` → `types/interfaces/`
- Move `IUIStyleConfigurator.h` → `types/interfaces/`
- Move `EngineResources.h` → `types/core/`
- Remove both libraries

**Benefits:**
- ✅ Less indirection
- ✅ Simpler CMake structure
- ✅ Clearer what you depend on
- ✅ Easier to navigate codebase

**Risk:** LOW (just moving files and updating CMake)  
**Effort:** 2-3 hours  
**Impact:** Reduced complexity

---

### 5. ⚠️ Break assets ↔ user_interface Circular Dependency (PRIORITY: LOW)

**Problem:**
- `assets` depends on `user_interface`
- `user_interface` depends on `assets`
- This creates tight coupling between two systems that should be independent

**Analysis:**
Looking at the code:
- `AssetManager` needs to load UI-related assets (fonts, textures)
- `FlatbuffersUIElementConfigurator` needs `AssetManager` to get assets

**Solution:**
Make asset loading **generic** and **unaware of UI**:
```cpp
// Current (in AssetManager)
void LoadUIAssets() { ... }  // UI-specific method

// Proposed (in AssetManager)
void LoadAsset(const AssetConfig& config) { ... }  // Generic method
```

Then `user_interface` can use `assets` without `assets` knowing about UI.

**Benefits:**
- ✅ Unidirectional dependency: user_interface → assets (not vice versa)
- ✅ Asset system becomes more reusable
- ✅ Clearer separation of concerns

**Risk:** MEDIUM-HIGH (requires refactoring asset loading code)  
**Effort:** 6-8 hours  
**Impact:** Better modularity, easier to extend

---

## Implementation Roadmap

### Phase 1: Low-Risk, High-Impact (Do First) ⭐

**Steps:**
1. Move `SceneLoadData.h` to `types/core/`
2. Move `SceneInfoProvider.h` to `types/core/`
3. Update includes
4. Test build

**Time:** 1-2 hours  
**Risk:** LOW  
**Value:** Immediate organization improvement

---

### Phase 2: Component Refactoring (High Value) ⭐⭐

**Steps:**
1. Make `Component` destructor inline: `virtual ~Component() = default;`
2. Make `GetComponentRegisterIndex()` constexpr and inline in headers
3. Remove all `.cpp` files from `src/components/`
4. Move headers to `types/components/`
5. Update `CMakeLists.txt` to remove components library
6. Update all includes
7. Run full test suite

**Time:** 4-6 hours  
**Risk:** MEDIUM  
**Value:** Faster builds, cleaner architecture

---

### Phase 3: Foundation Consolidation (Medium Value) ⭐

**Steps:**
1. Move `interfaces/*.h` to `types/interfaces/`
2. Move `configuration/*.h` to `types/interfaces/`
3. Move `resources/*.h` to `types/core/`
4. Update `types/CMakeLists.txt` to include all subdirectories
5. Remove `interfaces/`, `configuration/`, `resources/` libraries
6. Update all dependent `CMakeLists.txt` files
7. Update all includes
8. Run full test suite

**Time:** 4-6 hours  
**Risk:** MEDIUM  
**Value:** Simpler architecture, no circular deps

---

### Phase 4: Break Circular Dependencies (Optional) 

**Steps:**
1. Refactor `AssetManager` to be UI-agnostic
2. Remove `user_interface` dependency from `assets/CMakeLists.txt`
3. Update asset loading code
4. Run full test suite

**Time:** 6-8 hours  
**Risk:** HIGH  
**Value:** Better modularity (but current structure works)

---

## What About SceneLoadData?

**Question:** How do we account for data structures like SceneLoadData?

**Answer:**
- **Current location:** `interfaces/` ❌
- **Should be:** `types/core/` ✅
- **Rationale:** It's a **data transfer object (DTO)**, not an interface
- **Rule:** Put data structures in `types/`, put abstract interfaces in `interfaces/` (or `types/interfaces/` after merge)

**SceneLoadData specifically:**
```cpp
struct SceneLoadData {
  SceneData scene_data;                         // Data structure
  std::unique_ptr<IEntityImporter> entity_importer;  // Uses interface
};
```

This is a **data structure that uses an interface**, not an interface itself. It belongs in `types/core/`.

---

## Can We Collapse Libraries?

**Question:** Can we collapse down any libraries to lower levels? Could components be collapsed down to a types level library?

**Answer: YES!** Here's what can be collapsed:

### ✅ Components → types/components/ (STRONGLY RECOMMENDED)

**Current:**
- `components/` is a **NORMAL library** with .cpp files
- Contains pure data structs

**Proposed:**
- Move to `types/components/` as **header-only**
- No library to compile or link

**Benefits:**
- Faster builds (no compilation needed)
- Clearer that components are data-only
- Reduced coupling

---

### ✅ interfaces → types/interfaces/ (RECOMMENDED)

**Current:**
- `interfaces/` is separate INTERFACE library
- Has circular dependency with `types`

**Proposed:**
- Move to `types/interfaces/`
- Part of single `types` library

**Benefits:**
- No circular dependency
- Single foundation layer
- Simpler CMake

---

### ✅ configuration → types/interfaces/ (RECOMMENDED)

**Current:**
- `configuration/` is thin INTERFACE library
- Only 2 header files

**Proposed:**
- Move to `types/interfaces/`
- Remove library

**Benefits:**
- Less indirection
- Simpler structure

---

### ✅ resources → types/core/ (RECOMMENDED)

**Current:**
- `resources/` is thin INTERFACE library
- Only 1 header file

**Proposed:**
- Move `EngineResources.h` to `types/core/`
- Remove library

**Benefits:**
- Less indirection
- Clearer organization

---

### ❌ Other Libraries Should NOT Be Collapsed

**Keep separate:**
- `events` - Has implementation (.cpp files) for event handling
- `entity` - Has implementation for entity management
- `assets` - Has implementation for asset loading
- `user_interface` - Has implementation for UI configuration
- `data_providers` - Has implementation for data loading
- `context`, `logic`, `scenes`, `display`, `engine`, `error_loop`, `logger` - All have significant implementation

**Rationale:**
These libraries have **actual implementation code** (.cpp files) and represent distinct subsystems. They should remain as separate libraries.

---

## Robustness & Extensibility Considerations

### Robustness Improvements

**1. Reduced Compilation Dependencies**
- Header-only components mean changes don't trigger recompilation cascade
- Faster incremental builds = faster development cycle
- Less coupling = fewer build errors

**2. Clearer Dependency Graph**
- No circular dependencies = easier to reason about
- Clear layer structure = easier to test in isolation
- Better separation = easier to find bugs

**3. Type Safety**
- All types in `types/` makes it easy to find and use `std::expected`
- Consistent error handling patterns
- Compile-time checks via constexpr

### Extensibility Improvements

**1. Adding New Components**
- **Before:** Create .h and .cpp, add to CMakeLists.txt, recompile library
- **After:** Just add .h file to `types/components/`, include it, done!

**2. Adding New Data Types**
- **Before:** Choose between types/, interfaces/, configuration/, resources/
- **After:** Just add to appropriate subdirectory in `types/`

**3. Plugin Architecture**
- **Before:** Circular dependencies make runtime loading difficult
- **After:** Clean layer structure enables plugin systems

**4. Hot Reloading**
- **Before:** Recompile libraries, relink everything
- **After:** With header-only components, easier to support hot reloading

---

## Summary of Benefits

| Improvement | Risk | Effort | Build Time | Maintainability | Extensibility |
|-------------|------|--------|------------|-----------------|---------------|
| Move data structures to types/core/ | LOW | 1-2h | 0% | +++ | + |
| Convert components to header-only | MED | 4-6h | -20% | +++ | +++ |
| Merge types and interfaces | MED | 2-4h | -5% | ++ | ++ |
| Remove configuration/resources | LOW | 2-3h | -2% | ++ | + |
| Break circular dependencies | HIGH | 6-8h | 0% | ++ | +++ |

**Total estimated effort:** 13-20 hours  
**Total build time improvement:** ~20-25%  
**Maintainability improvement:** Significant  
**Extensibility improvement:** Significant

---

## Recommendation

**Start with Phases 1 and 2** (low to medium risk, high impact):
1. Move data structures (1-2 hours)
2. Convert components to header-only (4-6 hours)

These two changes alone will provide:
- ✅ Better organization
- ✅ Faster builds (~20% improvement)
- ✅ Clearer architecture
- ✅ Foundation for future improvements

**Then consider Phase 3** (foundation consolidation) when ready for larger refactoring.

**Phase 4** (breaking circular dependencies) is optional and can be deferred until the asset/UI systems need to be refactored for other reasons.

---

## Questions?

For detailed analysis, see:
- `ARCHITECTURE_LAYER_ANALYSIS.md` - Complete technical analysis
- `ARCHITECTURE_DIAGRAMS.md` - Visual diagrams of current vs proposed architecture

For implementation guidance, follow the roadmap above step-by-step.
