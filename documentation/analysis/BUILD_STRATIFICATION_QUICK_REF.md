# Build Stratification Quick Reference

**Purpose:** Fast reference for implementing the stratification strategy  
**See Also:** [BUILD_DEPENDENCY_ANALYSIS.md](./BUILD_DEPENDENCY_ANALYSIS.md) for complete analysis

---

## Three-Layer Architecture at a Glance

```
┌─────────────────────────────────────────────────────────────┐
│ Layer 3: Orchestration                                       │
│ → scene_management, display_management, engine_core         │
│ → Coordinates lower layers, entry points                    │
│ → Can depend on Layers 1 & 2                                │
└─────────────────────────────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│ Layer 2: Implementation                                      │
│ → data_providers, entity_system, event_system, logic_system │
│ → Business logic, concrete implementations                  │
│ → Can depend on Layer 1 only                                │
└─────────────────────────────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│ Layer 1: Data & Interfaces                                   │
│ → types, data_structures, interfaces, components            │
│ → No circular dependencies allowed                          │
│ → Minimal, stable foundation                                │
└─────────────────────────────────────────────────────────────┘
```

---

## Key Rules

### Layer 1 (Foundation)
- ✅ Only depend on external libraries (SFML, STL)
- ✅ No implementation in headers
- ❌ NO dependencies on Layer 2 or 3
- ❌ NO FlatBuffers generated headers in .h files
- ❌ NO circular dependencies

### Layer 2 (Implementation)
- ✅ Depend on Layer 1
- ✅ Hide FlatBuffers in .cpp files only
- ✅ Implement interfaces from Layer 1
- ❌ NO dependencies on Layer 3
- ❌ NO circular dependencies

### Layer 3 (Orchestration)
- ✅ Depend on Layers 1 & 2
- ✅ Coordinate and orchestrate
- ❌ NO business logic here
- ❌ NO direct FlatBuffers usage

---

## Critical Fixes

### Fix 1: Break components → user_interface Dependency

**Current Problem:**
```cpp
// components/CUserInterface.h
#include "UIElement.h"  // Pulls in engine headers
```

**Solution:**
```cpp
// components/CUserInterface.h
class UIElement;  // Forward declaration

struct CUserInterface : public Component {
  std::unique_ptr<UIElement> root_ui_element;
  ~CUserInterface();  // Define in .cpp
};

// components/CUserInterface.cpp
#include "UIElement.h"  // Only in .cpp
```

### Fix 2: Remove FlatBuffers from Component Headers

**Current Problem:**
```cpp
// components/CMeta.h
#include "meta_generated.h"
```

**Solution:**
```cpp
// components/CMeta.h
// No FlatBuffers includes

struct CMeta : public Component {
  std::string name;
  uint32_t entity_id;
};

// entity/FlatbuffersEntityConfigurator.cpp
#include "meta_generated.h"  // Only in configurator
```

### Fix 3: Complete Provider Pattern

**Missing Providers:**
- IContextDataProvider / FlatbuffersContextDataProvider
- IUIStyleProvider / FlatbuffersUIStyleProvider
- ILogicDataProvider / FlatbuffersLogicDataProvider

**Pattern:**
```cpp
// interfaces/IContextDataProvider.h
class IContextDataProvider {
public:
  virtual ~IContextDataProvider() = default;
  virtual std::expected<ContextConfigData, FailInfo>
    LoadContextConfig() const = 0;
};

// data_providers/FlatbuffersContextDataProvider.cpp
#include "context_data_generated.h"  // FlatBuffers ONLY here

class FlatbuffersContextDataProvider : public IContextDataProvider {
  std::expected<ContextConfigData, FailInfo>
    LoadContextConfig() const override {
    // Load FlatBuffers, convert to native struct
  }
};
```

### Fix 4: Break Circular Dependencies

| Cycle | Solution |
|-------|----------|
| `engine ↔ context` | Create IGameContext interface |
| `display ↔ scenes` | Create ISceneManager interface |
| `engine ↔ display` | Create IDisplayManager interface |
| `logic ↔ engine` | Fixed by breaking components → user_interface |

---

## Library Migration Map

### Layer 1: Foundation

| New Library | Old Library | Changes |
|-------------|-------------|---------|
| `types` | NEW | Extract common types, enums |
| `data_structures` | NEW | Native C++ structs from providers |
| `interfaces` | NEW | Abstract interfaces (I* classes) |
| `components` | `components` | Remove user_interface, FlatBuffers deps |
| `config` | `config` | Keep as is |
| `logger` | `logger` | Keep as is |

### Layer 2: Implementation

| New Library | Old Library | Changes |
|-------------|-------------|---------|
| `data_providers` | `data_providers` | Add 3 missing providers |
| `data_handlers` | `data_handlers` | Remove components dependency |
| `entity_system` | `entity` | Rename for consistency |
| `event_system` | `events` | Rename for consistency |
| `logic_system` | `logic` | Rename for consistency |
| `ui_system` | `user_interface` + `ui_styles` | Merge into one |
| `asset_system` | `assets` | Rename for consistency |
| `context_impl` | `context` | Separate interface from impl |
| `configuration` | `configuration` | Keep as is |

### Layer 3: Orchestration

| New Library | Old Library | Changes |
|-------------|-------------|---------|
| `scene_management` | `scenes` | Extract interfaces |
| `display_management` | `display` | Extract interfaces |
| `engine_core` | `engine` | Reduce dependencies |

---

## Implementation Checklist

### Phase 1: Foundation Cleanup ✅

- [ ] Create src/types/ directory and library
- [ ] Create src/data_structures/ directory and library
- [ ] Create src/interfaces/ directory and library
- [ ] Fix components → user_interface (forward declaration)
- [ ] Remove FlatBuffers from component headers
- [ ] Update components/CMakeLists.txt
- [ ] Verify Layer 1 compiles independently
- [ ] Measure Layer 1 build time (target: <10 seconds)

### Phase 2: Provider Pattern Completion ✅

- [ ] Create IContextDataProvider + implementation
- [ ] Create IUIStyleProvider + implementation
- [ ] Create ILogicDataProvider + implementation
- [ ] Refactor FlatbuffersDataLoader to use providers
- [ ] Update all consumer code
- [ ] Verify FlatBuffers headers only in 3-5 .cpp files
- [ ] Add tests for new providers

### Phase 3: Break Circular Dependencies ✅

- [ ] Create IGameContext interface
- [ ] Break engine ↔ context
- [ ] Create ISceneManager interface
- [ ] Break display ↔ scenes
- [ ] Create IDisplayManager interface
- [ ] Break engine ↔ display
- [ ] Verify 0 circular dependencies with tools
- [ ] Generate clean dependency graph

### Phase 4: Library Restructuring ✅

- [ ] Create layer1_foundation/ directory
- [ ] Create layer2_implementation/ directory
- [ ] Create layer3_orchestration/ directory
- [ ] Move libraries to correct layers
- [ ] Rename libraries (*_system pattern)
- [ ] Update CMakeLists.txt for layers
- [ ] Update all #include statements
- [ ] Update test structure

### Phase 5: Build Optimization ✅

- [ ] Configure unity builds
- [ ] Create precompiled headers for each layer
- [ ] Audit headers for forward declarations
- [ ] Split large headers
- [ ] Measure build time improvements
- [ ] Document results

### Phase 6: Validation ✅

- [ ] Generate dependency graphs
- [ ] Verify layer boundaries with tools
- [ ] Measure final build times
- [ ] Update architecture documentation
- [ ] Create developer guidelines
- [ ] Create enforcement tools

---

## Quick Wins (High Impact, Low Effort)

### Win 1: Forward Declare UIElement (2 hours)
**Impact:** Breaks major circular dependency  
**Effort:** Minimal code changes

### Win 2: Remove FlatBuffers from Component Headers (2 hours)
**Impact:** Faster component compilation  
**Effort:** Move includes to .cpp files

### Win 3: Extract Common Types (4 hours)
**Impact:** Reduce header coupling  
**Effort:** Create new library, move types

### Win 4: Complete Provider Pattern (8 hours)
**Impact:** Isolate FlatBuffers changes  
**Effort:** 3 new provider pairs

**Total:** 16 hours for major improvements

---

## Verification Commands

### Check for Circular Dependencies
```bash
# Python script (see BUILD_DEPENDENCY_ANALYSIS.md Appendix A)
python3 tools/check_circular_deps.py
```

### Measure Build Time
```bash
# Clean build
rm -rf build/
time cmake --preset Debug
time cmake --build --preset Debug

# Layer 1 only
time cmake --build --preset Debug --target types data_structures interfaces components
```

### Verify Layer Dependencies
```cmake
# Add to CMakeLists.txt
include(cmake/VerifyLayerDependencies.cmake)
verify_layer_dependency(components)
```

### Check FlatBuffers Usage
```bash
# Find FlatBuffers includes in headers
grep -r "_generated.h" src/ --include="*.h" | grep -v "flatbuffers_headers"
# Should only be in provider headers after Phase 2
```

---

## Common Patterns

### Provider Pattern
```cpp
// 1. Interface (header-only, native types)
class IDataProvider {
  virtual std::expected<NativeData, FailInfo> Load() const = 0;
};

// 2. Implementation (.cpp only FlatBuffers)
class FlatbuffersDataProvider : public IDataProvider {
  std::expected<NativeData, FailInfo> Load() const override;
};

// 3. Factory
std::unique_ptr<IDataProvider> CreateDataProvider(DataSource source);
```

### Forward Declaration Pattern
```cpp
// Header: Forward declare
class LargeType;

class MyClass {
  std::unique_ptr<LargeType> ptr;  // OK with forward declaration
  LargeType* GetPtr();              // OK
  ~MyClass();                       // Define in .cpp
};

// .cpp: Full definition
#include "LargeType.h"
MyClass::~MyClass() = default;
```

### Interface Extraction Pattern
```cpp
// Step 1: Extract interface
class IManager {
  virtual void DoSomething() = 0;
};

// Step 2: Implement interface
class ConcreteManager : public IManager {
  void DoSomething() override;
};

// Step 3: Use interface in dependencies
class Client {
  IManager& manager;  // Depends on interface, not implementation
};
```

---

## Success Metrics

| Metric | Baseline | Target | How to Measure |
|--------|----------|--------|----------------|
| Circular dependencies | 10+ | 0 | Python script |
| Clean build time | 100% | 60-70% | time cmake --build |
| Incremental build (1 file) | 100% | 40-50% | touch + rebuild |
| Layer 1 build time | 100% | 20-30% | Build layer 1 only |
| FlatBuffers in headers | 14 libs | 3-5 .cpp | grep _generated.h |
| Component dependencies | 2 problematic | 0 | Check CMakeLists.txt |

---

## References

- **Complete Analysis:** [BUILD_DEPENDENCY_ANALYSIS.md](./BUILD_DEPENDENCY_ANALYSIS.md)
- **Existing Patterns:** [DATA_PROVIDER_SYSTEM.md](../DATA_PROVIDER_SYSTEM.md)
- **Provider Examples:** `src/data_providers/I*Provider.h`
