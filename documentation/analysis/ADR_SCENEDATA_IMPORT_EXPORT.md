# Architecture Decision Record: SceneData and Data Import/Export

## Status

**PROPOSED** - Awaiting team review and decision

## Context

The SteamRot engine uses a `SceneData` hierarchy to configure scenes during loading. The current implementation uses inheritance to pass pointers to "heavy" data structures (large objects expensive to copy):

- **Import Path**: `FbsSceneData` extends `SceneData` and adds a pointer to FlatBuffers `EntityCollectionFbs`
- **Export Path**: No equivalent structure exists for the reverse direction (runtime → serialized data)

### The Problem

1. **Asymmetric Architecture**: Import has `FbsSceneData`, but export has no equivalent
2. **Code Smell**: Using inheritance to "smuggle" data pointers feels like a workaround
3. **Runtime Casting**: Configurators must `dynamic_cast` (with `const_cast`) to access derived data
4. **Future Export**: Need for save/load functionality raises question: "Do we need `NativeSceneData`?"

### Current Implementation Pattern

```cpp
// Import: FbsSceneData holds pointer to FlatBuffers data
struct FbsSceneData : public SceneData {
  const EntityCollectionFbs *entity_collection = nullptr;
};

// Configurator must downcast to use it
FbsSceneData *fbs_data = dynamic_cast<FbsSceneData*>(
    const_cast<SceneData*>(scene_data));
```

**Problem**: This pattern works for import, but for export we'd need:

```cpp
// Export: NativeSceneData would hold pointer to runtime data?
struct NativeSceneData : public SceneData {
  const EntityMemoryPool *entity_memory_pool = nullptr;
};
```

## Decision Drivers

1. **Export Functionality**: Need to implement save/load for game state
2. **Architectural Consistency**: Import and export should follow similar patterns
3. **Code Maintainability**: Avoid const-casting and runtime type checking where possible
4. **Migration Cost**: Balance ideal architecture vs. refactoring effort
5. **Extensibility**: Support future data formats (JSON, network, etc.)

## Options Considered

### Option 1: Status Quo (Document Current Pattern)

**Approach**: Keep current architecture, clearly document that it's import-only, handle export differently.

**Pros**:
- Zero refactoring cost
- Works for current needs
- Simple to implement

**Cons**:
- Asymmetric architecture (different patterns for import/export)
- Still requires downcasting and const-casting
- Technical debt remains
- No clear path forward

### Option 2: Add NativeSceneData (Minimal Change)

**Approach**: Create `NativeSceneData` as mirror of `FbsSceneData` for export operations.

```cpp
struct NativeSceneData : public SceneData {
  const EntityMemoryPool *entity_memory_pool = nullptr;
};

class IEntityExporter {
  virtual std::expected<std::unique_ptr<uint8_t[]>, FailInfo>
  ExportEntities(const NativeSceneData *scene_data, size_t &out_size) = 0;
};
```

**Pros**:
- Symmetric with `FbsSceneData`
- Minimal code changes
- Clear documentation of pattern
- Enables export functionality

**Cons**:
- Still uses inheritance for data passing
- Still requires downcasting in exporters
- Doesn't address underlying architectural concern
- Perpetuates code smell

### Option 3: Separate Importer/Exporter Hierarchies (Clean Architecture)

**Approach**: Remove heavy data from `SceneData`, use dedicated import/export interfaces.

```cpp
// SceneData stays lightweight
struct SceneData {
  SceneInfo scene_info;
  SceneResourcesConfig scene_resources_config;
  AssetConfig scene_asset_config;
  // No pointers to heavy data
};

// Import through dedicated interface
class IEntityImporter {
  virtual std::expected<std::monostate, FailInfo>
  ImportEntities(EntityMemoryPool &emp) = 0;
};

class FlatbuffersEntityImporter : public IEntityImporter {
  FlatbuffersEntityImporter(const EntityCollectionFbs &entity_collection);
  // ...
};

// Export through symmetric interface
class IEntityExporter {
  virtual std::expected<std::unique_ptr<uint8_t[]>, FailInfo>
  ExportEntities(const EntityMemoryPool &emp, size_t &out_size) = 0;
};
```

**Pros**:
- Clean separation: data vs. operations
- No downcasting required
- No const-casting required
- Symmetric import/export
- Extensible for new formats
- Follows SOLID principles

**Cons**:
- Significant refactoring cost
- Changes configurator interfaces
- More classes to manage
- Migration path needed

### Option 4: Context Objects

**Approach**: Pass configuration data separately from runtime data.

```cpp
struct ImportContext {
  const EntityCollectionFbs *entity_collection;
  EventHandler &event_handler;
};

ConfigureEntities(Scene &scene, const SceneData *scene_data, 
                 const ImportContext &context);
```

**Pros**:
- No inheritance for data passing
- Explicit dependencies
- Type-safe

**Cons**:
- Many interface changes
- More parameters everywhere
- Less clear ownership semantics

## Decision

**RECOMMENDED**: Implement **Option 2** (NativeSceneData) immediately, plan migration to **Option 3** (Importer/Exporter) as future work.

### Rationale

**Short-term (Option 2)**:
- Unblocks export/save functionality with minimal effort
- Achieves symmetry with existing `FbsSceneData` pattern
- Documents the pattern clearly for team understanding
- Low risk, high value for immediate needs

**Long-term (Option 3)**:
- Addresses root architectural concern
- Provides cleaner, more maintainable code
- Supports future extensibility needs
- Can be done incrementally without blocking current work

### Two-Phase Implementation

**Phase 1: Enable Export (Option 2)**
1. Create `NativeSceneData` struct
2. Create `IEntityExporter` interface
3. Implement `FlatbuffersEntityExporter`
4. Document pattern in code and docs
5. Implement save/load functionality

**Estimated Effort**: 2-3 days

**Phase 2: Refactor to Importer/Exporter (Option 3)**
1. Create `IEntityImporter` interface
2. Refactor `FlatbuffersEntityConfigurator` to `FlatbuffersEntityImporter`
3. Update configurator interfaces to use importers
4. Update all configurator implementations
5. Deprecate `FbsSceneData`
6. Update documentation

**Estimated Effort**: 1-2 weeks (can be done incrementally)

## Consequences

### Positive

- **Export enabled**: Can implement save/load functionality
- **Symmetry achieved**: Import and export follow parallel patterns
- **Documentation**: Pattern is clearly documented and understood
- **Migration path**: Clear route to better architecture without blocking work
- **Low risk**: Phase 1 is minimal changes to existing code

### Negative

- **Technical debt**: Phase 1 doesn't address the underlying code smell
- **Future work**: Phase 2 refactoring still needed for ideal architecture
- **Pattern duplication**: Two similar patterns (FbsSceneData and NativeSceneData) exist temporarily
- **Cast still required**: Exporters still need to downcast NativeSceneData

### Neutral

- **Team decision required**: Need consensus on two-phase approach
- **Documentation burden**: Must maintain docs during transition
- **Testing**: Need tests for both import and export paths

## Implementation Guidelines

### Phase 1: Immediate Implementation (Option 2)

**Files to Create**:
- `src/types/core/NativeSceneData.h` - Runtime scene data for export
- `src/interfaces/IEntityExporter.h` - Export interface
- `src/entity/FlatbuffersEntityExporter.h` - FlatBuffers export implementation
- `src/entity/FlatbuffersEntityExporter.cpp` - Implementation
- `tests/unit/entity/FlatbuffersEntityExporter.test.cpp` - Tests

**Files to Document**:
- Update `documentation/architecture/DATA_FLOW.md` (create if needed)
- Update code comments in `SceneData.h` and `FbsSceneData.h`
- Document pattern in `.github/copilot-instructions.md`

**Key Principles**:
1. `SceneData` - Base configuration (lightweight)
2. `FbsSceneData` - Import only (FlatBuffers → Runtime)
3. `NativeSceneData` - Export only (Runtime → Serialized)
4. Never use `FbsSceneData` for export or `NativeSceneData` for import

### Phase 2: Future Refactoring (Option 3)

**Migration Strategy**:
1. Create new interfaces alongside existing code
2. Implement new classes (importers/exporters)
3. Update one configurator at a time
4. Add deprecation warnings to old pattern
5. Remove old code once migration complete

**Backwards Compatibility**:
- Keep `FbsSceneData` during migration
- Support both patterns temporarily
- Clear migration path for each component

## Alternatives Considered But Rejected

### Using Templates Instead of Inheritance

```cpp
template<typename DataSource>
struct SceneDataWrapper {
  SceneData base;
  DataSource *data_source;
};
```

**Why rejected**: 
- More complex than inheritance approach
- Doesn't solve downcasting problem
- Less clear semantics

### Visitor Pattern

```cpp
class SceneDataVisitor {
  virtual void Visit(FbsSceneData &data) = 0;
  virtual void Visit(NativeSceneData &data) = 0;
};
```

**Why rejected**:
- Overengineered for the problem
- Doesn't address data passing concern
- More complexity without clear benefit

## References

- [SCENEDATA_ARCHITECTURE_ANALYSIS.md](./SCENEDATA_ARCHITECTURE_ANALYSIS.md) - Detailed analysis
- [SCENEDATA_CODE_EXAMPLES.md](./SCENEDATA_CODE_EXAMPLES.md) - Implementation examples
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) - Coding standards
- Current implementation: `src/types/core/SceneData.h`, `src/types/core/FbsSceneData.h`

## Review and Approval

**Proposed by**: Architecture Analysis (Copilot Agent)
**Date**: 2026-01-06
**Status**: PROPOSED

**Reviewers** (to be determined):
- [ ] Lead Developer
- [ ] Architecture Team
- [ ] Project Owner

**Approval Status**: PENDING

---

## Appendix: Code Smell Analysis

### What is the "Code Smell"?

Using inheritance to pass pointers to unrelated data structures is a **misuse of inheritance**. The OOP principle states:

> "Inheritance should model 'is-a' relationships, not 'has-a' or 'uses-a' relationships."

**Current code**:
- `FbsSceneData` **is-a** `SceneData` (inheritance)
- BUT: Relationship is really "`FbsSceneData` **has-a** pointer to FlatBuffers data"

This is the "smuggling data through inheritance" code smell.

### Why It Matters

1. **Type Safety**: Configurators receive `SceneData*` but need `FbsSceneData*`
2. **Runtime Checking**: Must use `dynamic_cast` to check actual type
3. **Const-Correctness**: Must cast away `const` to perform `dynamic_cast`
4. **Tight Coupling**: Configurator knows about specific derived types
5. **Fragile**: Adding new derived types requires updating all configurators

### Better Alternatives

**Composition over Inheritance**:
```cpp
struct SceneImportData {
  SceneData config;
  const EntityCollectionFbs *source_data;
};
```

**Strategy Pattern**:
```cpp
class IEntityImporter {
  // Operation as interface, not data as inheritance
};
```

Both avoid the inheritance code smell while achieving the same goal.

---

**Document Version**: 1.0
**Last Updated**: 2026-01-06
**Next Review**: After team decision on implementation approach
