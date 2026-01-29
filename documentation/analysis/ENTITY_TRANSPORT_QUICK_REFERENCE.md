# Entity Transport Quick Reference

This is a quick reference guide for understanding the entity transport architecture analysis. For complete details, see [ENTITY_TRANSPORT_ARCHITECTURE_ANALYSIS.md](./ENTITY_TRANSPORT_ARCHITECTURE_ANALYSIS.md).

---

## TL;DR

**Problem**: Three overlapping abstractions doing similar things with unnecessary complexity.

**Key Constraint**: Must support lazy loading to avoid copying large entity data (50k-100k entities predicted).

**Solution**: Use Option B - simplified variant with lazy loading (eliminates interface, preserves lazy loading).

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

## Recommended Solution: Option B

**Change**:
```cpp
// Current (complex)
std::variant<std::monostate, 
             std::unique_ptr<IEntityImporter>,
             std::shared_ptr<EntityMemoryPool>, 
             EntityMemoryPool> entity_transport;

// Proposed (simpler, preserves lazy loading)
struct EntityLazyLoadData {
  const EntityCollectionFbs* flatbuffers_data;
};

std::variant<std::monostate,
             EntityMemoryPool,
             EntityLazyLoadData> entity_source;
```

**Benefits**:
- ✅ Eliminates interface abstraction (IEntityImporter)
- ✅ No virtual functions (compile-time polymorphism via std::visit)
- ✅ **Preserves lazy loading** (critical for 50k-100k entities)
- ✅ No copying of large entity data
- ✅ Clear ownership (3 simple states)
- ✅ Easier to test (no interface comparison issues)
- ✅ Better performance (~3-5% improvement)
- ✅ EventHandler provided at usage point (not stored in variant)

**Usage**:
```cpp
// Production: Store lazy load data (no copying!)
EntityLazyLoadData lazy_data{
  .flatbuffers_data = scene_data_fbs->entity_collection()
};
scene_data.entity_source = lazy_data;

// In SceneFactory (EventHandler from scene context):
FlatbuffersEntityConfigurator configurator(
    scene.GetSceneContext().event_handler, *lazy_data.flatbuffers_data);
configurator.ConfigureEntityMemoryPool(scene.GetEntityMemoryPool());

// Scene capture: Direct copy (already loaded)
scene_data.entity_source = scene->GetEntityMemoryPool();

// Testing: Empty or pre-configured
scene_data.entity_source = std::monostate{};    // No entities
scene_data.entity_source = test_pool;           // Pre-configured
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

### Phase 2: Replace IEntityImporter with EntityLazyLoadData (Low Risk)
- Define EntityLazyLoadData struct (FlatBuffers pointer only - no EventHandler)
- Update EntityTransportVariant to use EntityLazyLoadData instead of unique_ptr<IEntityImporter>
- Update SceneFactory to use std::visit and get EventHandler from scene context
- **Preserves lazy loading** - no copying of entity data

### Phase 3: Rename for Clarity (Low Risk)
- Rename entity_transport to entity_source
- Update documentation

### Phase 4: Monitor and Optimize (Future)
- Track FlatBuffers data lifetime
- Measure actual entity count distributions
- Optimize as needed

---

## Expected Outcomes

| Metric | Current | After Option B |
|--------|---------|----------------|
| Abstractions | 3 | 1 |
| Virtual calls | Yes | No |
| Code clarity | Low | High |
| Test complexity | High | Medium |
| Performance | Baseline | +3-5% |
| Lazy loading | Yes | Yes ✓ |
| Large entity support | Yes | Yes ✓ |
| Maintainability | Medium | High |

---

## Decision Criteria

**Choose Option B (Recommended) if**:
- ✅ You want to eliminate interface overhead
- ✅ Performance matters
- ✅ **Need to support 50k-100k entities** (your use case!)
- ✅ Code clarity is priority
- ✅ All formats are compile-time known (stated requirement!)

**Choose Option A only if**:
- ⚠️ Entity counts guaranteed small (< 1000)
- ⚠️ Eager loading is acceptable
- ⚠️ **NOT suitable for your 50k-100k entity use case**

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
A: Add new variant state or template specializations (compile-time polymorphism)

**Q: What about performance with 50k-100k entities?**  
A: Option B preserves lazy loading - no copying overhead. Option A would be disastrous.

**Q: What about testing?**  
A: Simpler! Just compare variant values instead of complex interface matching

**Q: Why not just use Option A for simplicity?**  
A: Option A requires eager loading, which means copying potentially hundreds of MB of entity data at scene load time. Completely unsuitable for large entity counts.

---

**Document Version**: 1.0  
**Date**: 2026-01-29  
**See Also**: [Complete Analysis](./ENTITY_TRANSPORT_ARCHITECTURE_ANALYSIS.md)
