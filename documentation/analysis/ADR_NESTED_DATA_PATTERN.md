# Architecture Decision Record: Nested Data Handling Pattern

## Status
**Proposed** - Analysis and documentation complete, awaiting implementation decision

## Date
2024-12-25

## Context

The SteamRot game engine uses a data provider pattern to load configuration and state data from FlatBuffers files. Currently, accessing nested data structures requires multiple method calls through viewer interfaces, making the code verbose and error-prone.

### Current Problems

1. **Clunky Access Pattern**: To get AssetConfig from EngineData requires:
   - Call `GetEngineDataProvider()`
   - Call `LoadEngineResourcesConfig()` 
   - Call `GetAssetConfigViewer()`
   - Call `ProvideAssetConfig()`
   - Result: 4 method calls for one piece of data

2. **No Unified Structure**: EngineData is split across three separate types:
   - `EngineResourcesConfig`
   - `EngineConfig`
   - `EngineState`
   - Plus nested `AssetConfig` accessed via viewer

3. **Inconsistent Patterns**: Different providers handle nested data differently:
   - `FlatbuffersEngineDataProvider` supports `GetAssetConfigViewer()`
   - `FlatbuffersSceneDataProvider` returns error for `GetAssetConfigViewer()`

4. **No Caching**: Data may be loaded multiple times if accessed in different places

5. **Poor Discoverability**: It's not obvious what nested data is available or how to access it

### User Request

> "I would like a generic way to deal with nested data that we use as the goto. For example when loading in SceneData getting out the AssetData is a bit clunky at the moment."

User wants clarity on:
- Should base structs contain nested structs?
- Should providers create the full struct?
- Should we cache the main struct for easy nested access?

## Decision

**Adopt the Unified Data Wrapper Pattern** with three core rules:

### Rule 1: Base Structs Must Contain Nested Structs
When data is logically related (parent-child or contains relationship), nest it directly in the parent struct rather than accessing through separate viewer interfaces.

```cpp
// Good: Nested directly
struct EngineData {
  EngineResourcesConfig resources_config;
  EngineConfig engine_config;
  EngineState engine_state;
  AssetConfig asset_config;  // Nested directly
};

// Avoid: Separate viewer pattern for simple nesting
// (Viewers should be reserved for complex data access scenarios)
```

### Rule 2: Providers Create the Full Struct
Data providers are responsible for loading and assembling complete data structures with all nested data populated. Callers should not have to make multiple calls to get related data.

```cpp
class IEngineDataProvider {
public:
  // Single method returns complete structure
  virtual std::expected<EngineData, FailInfo> LoadEngineData() const = 0;
  
  // Legacy methods can remain for backward compatibility
};
```

### Rule 3: Cache the Main Struct
Providers should optionally cache complete data structures when appropriate (e.g., data loaded at startup and rarely changes). This enables easy access to nested data without reloading.

```cpp
class FlatbuffersEngineDataProvider {
private:
  mutable std::optional<EngineData> m_cached_data;
  
public:
  std::expected<EngineData, FailInfo> LoadEngineData() const {
    if (m_cached_data.has_value()) {
      return m_cached_data.value();
    }
    // Load and cache...
  }
};
```

## Rationale

### Why Unified Wrappers?

**Benefits:**
- **Single source of truth**: One struct contains all related data
- **Clear ownership**: Obvious what data belongs together
- **Easy to extend**: Add new nested fields as needed
- **Better IDE support**: Autocomplete shows all available nested data

**Alternatives Considered:**
- Keep current viewer pattern: Rejected due to verbosity and complexity
- Use inheritance: Rejected due to diamond problem and less flexibility
- Use composition with smart pointers: Rejected due to lifetime complexity

### Why Single Load Method?

**Benefits:**
- **Simplicity**: One call instead of many
- **Atomic operation**: Load succeeds or fails as a unit
- **Less error-prone**: Can't forget to load a component
- **Easier testing**: Mock one method instead of several

**Alternatives Considered:**
- Builder pattern: Considered but adds boilerplate for simple use cases
- Separate load methods only: Current approach, rejected as too verbose

### Why Caching?

**Benefits:**
- **Performance**: Load once, use many times
- **Consistency**: Same data instance used throughout
- **Easy nested access**: No reloading to access nested data

**Alternatives Considered:**
- No caching: Rejected due to repeated file I/O overhead
- Global cache: Rejected due to lifetime and ownership complexity
- External cache manager: Over-engineered for current needs

## Consequences

### Positive

1. **Consistency**: Standardized pattern across all data providers
2. **Discoverability**: Easy to see what nested data is available
3. **Maintainability**: Clear structure reduces bugs
4. **Performance**: Caching reduces redundant I/O
5. **Extensibility**: Easy to add new nested data types
6. **Developer Experience**: Less boilerplate, clearer code

### Negative

1. **Migration Effort**: Existing code needs to be updated
2. **Memory Usage**: Caching increases memory footprint
3. **Struct Size**: Wrapper structs can become large
4. **Coupling**: More coupling between related data types

### Neutral

1. **Backward Compatibility**: Old methods can coexist during migration
2. **FlatBuffers Schema**: No changes needed, wrappers are C++ only
3. **Learning Curve**: New pattern must be documented and learned

## Mitigation Strategies

### For Migration Effort
- **Phased approach**: Migrate gradually over multiple phases
- **Keep old methods**: Maintain backward compatibility
- **Clear documentation**: Provide guides and examples

### For Memory Usage
- **Optional caching**: Make caching opt-in per provider
- **Cache invalidation**: Provide methods to clear cache
- **Lazy loading**: Load nested data only when accessed

### For Struct Size
- **Limit nesting depth**: Keep to 2-3 levels maximum
- **Use references**: Store large data by reference
- **Split when needed**: Create separate wrappers if too large

### For Coupling
- **Interface abstraction**: Keep provider interfaces decoupled
- **Composition over inheritance**: Use has-a relationships
- **Separate concerns**: Keep data structs separate from logic

## Implementation Plan

### Phase 1: Documentation (Current)
- ✅ Create analysis document
- ✅ Create developer guide
- ✅ Create quick reference
- ✅ Create visual diagrams
- ✅ Create decision record

### Phase 2: Core Wrapper Structs (Future)
- Create `EngineData` wrapper struct
- Add accessor methods
- Write unit tests for struct

### Phase 3: Provider Implementation (Future)
- Add `LoadEngineData()` to `IEngineDataProvider`
- Implement in `FlatbuffersEngineDataProvider`
- Add caching logic
- Keep existing methods for backward compatibility

### Phase 4: Caller Migration (Future)
- Update `Engine::StartUp()` to use new pattern
- Migrate other high-value callers
- Add integration tests

### Phase 5: Broader Application (Future)
- Apply pattern to `SceneData` if needed
- Apply to `SceneManagerData`
- Standardize across all providers

### Phase 6: Cleanup (Far Future)
- Deprecate old separate load methods
- Remove viewer interfaces for simple nesting
- Update all documentation

## Monitoring and Review

### Success Metrics
- Reduction in lines of code for data loading
- Decrease in data-related bugs
- Improved developer satisfaction (survey)
- Performance improvements (reduced I/O)

### Review Points
- After Phase 3: Review provider implementation
- After Phase 4: Review caller experience
- After Phase 5: Review pattern consistency
- After 3 months: Full pattern effectiveness review

## References

- **Analysis**: `documentation/analysis/NESTED_DATA_HANDLING_PATTERNS.md`
- **Developer Guide**: `documentation/workflows/NESTED_DATA_PATTERN_GUIDE.md`
- **Quick Reference**: `documentation/NESTED_DATA_QUICK_REFERENCE.md`
- **Diagrams**: `documentation/NESTED_DATA_DIAGRAMS.md`

## Related Decisions

- **Data Provider Pattern**: Established in initial architecture
- **FlatBuffers Usage**: Chosen for serialization format
- **Error Handling with std::expected**: Chosen for error propagation

## Notes

This decision is part of addressing technical debt in the data loading system. The pattern prioritizes developer experience and maintainability over absolute performance optimization.

The three rules (nest structs, full struct creation, caching) provide a simple mental model that can be consistently applied across the codebase.

---

**Decision Made By**: GitHub Copilot Agent (Analysis Phase)  
**Implementation Approval Needed**: Repository Owner  
**Reviewed By**: Pending  
**Last Updated**: 2024-12-25
