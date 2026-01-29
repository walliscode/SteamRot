# Entity Transport Quick Reference

This is a quick reference guide for understanding the entity transport architecture analysis. For complete details, see [ENTITY_TRANSPORT_ARCHITECTURE_ANALYSIS.md](./ENTITY_TRANSPORT_ARCHITECTURE_ANALYSIS.md).

---

## TL;DR

**Problem**: Three overlapping abstractions doing similar things with unnecessary complexity.

**Solution**: Use Option A - direct `std::optional<EntityMemoryPool>` storage.

---

## Current Architecture (3 Abstractions)

### 1. IEntityImporter
- **Purpose**: Interface for importing entities
- **Implementation**: FlatbuffersEntityImporter (only one)
- **Issue**: Thin wrapper that just delegates to configurator

### 2. IEntityExporter
- **Purpose**: Interface for exporting entities
- **Implementation**: None (unused!)
- **Issue**: Future-proofing abstraction with zero usage

### 3. EntityTransportVariant
- **Purpose**: Transport entity data in SceneData
- **States**: 4 different types (monostate, unique_ptr<IEntityImporter>, shared_ptr<EntityMemoryPool>, EntityMemoryPool)
- **Issue**: Complex runtime variant for compile-time known scenarios

---

## Data Flow (Current)

```
FlatBuffers File → Loader → FlatbuffersEntityImporter (wrapper) 
    → EntityTransportVariant → SceneFactory (extractor) 
    → FlatbuffersEntityConfigurator → EntityMemoryPool
```

**6 layers** for what should be: **FlatBuffers → Configurator → EntityMemoryPool** (3 layers)

---

## Problems Identified

1. **Purpose Mixing**: IEntityImporter wraps data, EntityTransportVariant transports it, but they overlap
2. **Unnecessary Polymorphism**: All formats known at compile time, no need for virtual functions
3. **Unused Code**: IEntityExporter has zero implementations
4. **Complex Variant**: 4 states in EntityTransportVariant, difficult to reason about
5. **Too Many Layers**: 6 indirection layers for simple operation

---

## Recommended Solution: Option A

**Change**:
```cpp
// Current (complex)
std::variant<std::monostate, 
             std::unique_ptr<IEntityImporter>,
             std::shared_ptr<EntityMemoryPool>, 
             EntityMemoryPool> entity_transport;

// Proposed (simple)
std::optional<EntityMemoryPool> entity_pool;
```

**Benefits**:
- ✅ Zero abstractions to learn
- ✅ No virtual functions (better performance)
- ✅ Clear ownership (optional = may not exist)
- ✅ Simple to test
- ✅ Easier to maintain

**Usage**:
```cpp
// Production: Load and configure
EntityMemoryPool pool;
configurator.ConfigureEntityMemoryPool(pool);
scene_data.entity_pool = std::move(pool);

// Scene capture: Direct copy
scene_data.entity_pool = scene->GetEntityMemoryPool();

// Testing: Empty or pre-configured
scene_data.entity_pool = std::nullopt;           // No entities
scene_data.entity_pool = test_pool;              // Pre-configured
```

---

## Alternative Options

### Option B: Simplified Variant
Keep variant but with data only (no interfaces):
```cpp
using EntitySource = std::variant<
    std::monostate,          // No entities
    EntityMemoryPool,        // Direct pool
    EntityData               // Lazy-loadable data (FlatBuffers ref)
>;
```
**When to use**: If lazy loading performance is critical

### Option C: Type-Erased Configurator
Single abstraction with std::function:
```cpp
class EntityConfigurator {
  std::function<std::expected<std::monostate, FailInfo>(EntityMemoryPool&)> m_configure_fn;
  // Factory methods for different sources
};
```
**When to use**: If you need some runtime flexibility

---

## Migration Path

### Phase 1: Delete IEntityExporter ✅ (Zero Risk)
- No implementations, no usages
- Just delete the file

### Phase 2: Remove IEntityImporter (Low Risk)
- Remove FlatbuffersEntityImporter wrapper
- Use FlatbuffersEntityConfigurator directly
- Simplify EntityTransportVariant to 3 states

### Phase 3: Simplify to Optional (Medium Risk)
- Evaluate if variant is still needed
- Consider switching to `std::optional<EntityMemoryPool>`

### Phase 4: Lazy Loading Evaluation (Future)
- Measure if eager loading causes issues
- Add std::function if truly needed

---

## Expected Outcomes

| Metric | Current | After Option A |
|--------|---------|----------------|
| Abstractions | 3 | 0 |
| Virtual calls | Yes | No |
| Code clarity | Low | High |
| Test complexity | High | Low |
| Performance | Baseline | +3-5% |
| Maintainability | Medium | High |

---

## Decision Criteria

**Choose Option A if**:
- ✅ You want the simplest solution
- ✅ Performance matters
- ✅ Code clarity is priority
- ✅ All formats are compile-time known (stated requirement!)

**Choose Option B if**:
- ⚠️ Lazy loading performance is measured to be critical
- ⚠️ std::function overhead is unacceptable

**Choose Option C if**:
- ⚠️ You anticipate needing runtime plugin system
- ⚠️ (This contradicts stated requirement of "compile time only")

---

## Files Affected

**To Remove**:
- `src/types/interfaces/IEntityExporter.h` ❌ (unused)
- `src/types/interfaces/IEntityImporter.h` ❌ (thin wrapper)
- `src/entity/FlatbuffersEntityImporter.h/cpp` ❌ (thin wrapper)
- `tests/unit/entity/FlatbuffersEntityImporter.test.cpp` ❌ (wrapper tests)

**To Modify**:
- `src/types/core/SceneData.h` (simplify variant)
- `src/scenes/SceneFactory.cpp` (simplify ImportEntities)
- `src/data_providers/FlatbuffersSceneDataProvider.cpp` (use configurator directly)
- `tests/matchers/EntityTransportEqualsMatcher.h/cpp` (simplify or remove)

**Unchanged**:
- `src/entity/FlatbuffersEntityConfigurator.h/cpp` (keep core logic)
- `src/types/interfaces/IEntityConfigurator.h` (still useful interface)
- All component/logic files

---

## Questions?

**Q: Won't we lose flexibility?**  
A: Only if you need runtime format discovery (contradicts "compile time only" requirement)

**Q: What about future formats (JSON, XML)?**  
A: Add factory functions or template specializations (compile-time polymorphism)

**Q: What about performance?**  
A: Option A is 3-5% faster due to reduced indirection

**Q: What about testing?**  
A: Simpler! Just compare optional values instead of complex variant matching

---

**Document Version**: 1.0  
**Date**: 2026-01-29  
**See Also**: [Complete Analysis](./ENTITY_TRANSPORT_ARCHITECTURE_ANALYSIS.md)
