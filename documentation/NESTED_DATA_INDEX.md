# Nested Data Handling Documentation - Index

## Overview

This documentation suite addresses the problem of inconsistent and clunky nested data access patterns in the SteamRot codebase. It provides analysis, guidance, and architectural decisions for a unified approach to handling nested data structures.

## Problem Statement

Currently, accessing nested data (e.g., AssetConfig inside EngineData) requires multiple method calls through viewer interfaces:

```cpp
// Current (clunky): 4+ calls to get AssetConfig
auto provider = factory.GetEngineDataProvider().value();
auto resources = provider->LoadEngineResourcesConfig().value();
auto viewer = provider->GetAssetConfigViewer().value();
auto assets = viewer->ProvideAssetConfig().value();
```

This makes code verbose, error-prone, and inconsistent across different data providers.

## The Solution: Three Simple Rules

1. **Base structs must contain nested structs** - Nest logically related data directly
2. **Providers create the full struct** - Return complete, populated structures
3. **Cache the main struct** - Optionally cache for easy nested data access

Result:
```cpp
// Proposed (clean): 1 call gets everything
auto engine_data = provider->LoadEngineData().value();
auto assets = engine_data.GetAssetConfig();
```

## Documentation Files

### For Quick Information

📄 **[NESTED_DATA_QUICK_REFERENCE.md](NESTED_DATA_QUICK_REFERENCE.md)**
- TL;DR summary of the pattern
- Quick before/after examples
- Pattern template to copy-paste
- When to use checklist
- ~5 minute read

### For Understanding

📊 **[NESTED_DATA_DIAGRAMS.md](NESTED_DATA_DIAGRAMS.md)**
- Visual ASCII diagrams
- Architecture comparisons
- Call flow illustrations
- Real-world usage scenarios
- ~10 minute read

### For Implementation

📘 **[workflows/NESTED_DATA_PATTERN_GUIDE.md](workflows/NESTED_DATA_PATTERN_GUIDE.md)**
- Step-by-step implementation guide
- How to add nested data to existing structures
- How to create new data providers
- Best practices and common patterns
- Testing guidelines
- Full migration checklist
- ~20 minute read

### For Deep Analysis

📖 **[analysis/NESTED_DATA_HANDLING_PATTERNS.md](analysis/NESTED_DATA_HANDLING_PATTERNS.md)**
- Detailed problem analysis
- Current implementation review
- Proposed solution design
- Complete migration strategy (5 phases)
- Benefits and tradeoffs
- Decision points with rationales
- ~25 minute read

### For Decision Making

📋 **[analysis/ADR_NESTED_DATA_PATTERN.md](analysis/ADR_NESTED_DATA_PATTERN.md)**
- Architecture Decision Record
- Context and rationale
- Alternatives considered
- Consequences (positive/negative)
- Mitigation strategies
- Implementation plan with metrics
- ~15 minute read

## Recommended Reading Order

### For Developers

If you need to work with data providers:

1. Start: **Quick Reference** (5 min) - Get the gist
2. Then: **Diagrams** (10 min) - Understand visually
3. When implementing: **Pattern Guide** (20 min) - Follow step-by-step

### For Architects/Leads

If you need to evaluate or approve the pattern:

1. Start: **Quick Reference** (5 min) - Understand the proposal
2. Then: **ADR** (15 min) - See decision rationale
3. For details: **Analysis** (25 min) - Deep dive
4. For visuals: **Diagrams** (10 min) - See architecture

### For New Team Members

Learning the codebase data patterns:

1. Start: **Quick Reference** (5 min) - Learn the rules
2. Then: **Diagrams** (10 min) - See the structure
3. When needed: **Pattern Guide** (20 min) - Learn how to implement

## Key Concepts

### Unified Data Wrapper

Instead of loading data piecemeal, create a wrapper struct that contains all related data:

```cpp
struct EngineData {
  EngineResourcesConfig resources_config;
  EngineConfig engine_config;
  EngineState engine_state;
  AssetConfig asset_config;  // Nested!
  
  // Accessor methods
  const AssetConfig& GetAssetConfig() const { return asset_config; }
};
```

### Single Load Method

Providers offer one method that returns the complete structure:

```cpp
class IEngineDataProvider {
public:
  virtual std::expected<EngineData, FailInfo> LoadEngineData() const = 0;
};
```

### Optional Caching

Providers can cache the complete structure for performance:

```cpp
class FlatbuffersEngineDataProvider {
private:
  mutable std::optional<EngineData> m_cached_data;
  
public:
  std::expected<EngineData, FailInfo> LoadEngineData() const {
    if (m_cached_data.has_value()) {
      return m_cached_data.value();  // Return cached
    }
    // Load, cache, and return...
  }
};
```

## Benefits Summary

- ✨ **Consistency** - Same pattern across all providers
- 🎯 **Simplicity** - One call instead of many
- 🔗 **Easy Nesting** - Direct access to nested data
- 🚀 **Performance** - Optional caching reduces I/O
- 📈 **Future-Proof** - Easy to add new nested types
- 🛡️ **Type-Safe** - Compile-time checking
- 📚 **Well-Documented** - Multiple guides for different needs

## Implementation Status

**Current Phase**: Analysis and Documentation (Complete)

This is documentation-only. No code changes have been made.

### Migration Phases

- **Phase 1**: Documentation ✅ (Current)
- **Phase 2**: Create wrapper structs (Future, if approved)
- **Phase 3**: Implement provider methods (Future)
- **Phase 4**: Migrate callers (Future)
- **Phase 5**: Apply to other providers (Future)
- **Phase 6**: Cleanup old methods (Far future)

## Examples in Current Codebase

### Already Using Parts of This Pattern

- ✅ `EngineConfig` nests `DisplayConfig` and `UserPreferencesConfig`
- ✅ `FlatbuffersEngineDataProvider` loads multiple data types
- ⚠️ But: No unified wrapper, no single load method, inconsistent patterns

### Will Improve With This Pattern

- ✅ `EngineData` will wrap all engine-related data
- ✅ `SceneData` will include nested `AssetConfig` if needed
- ✅ All providers will have unified `LoadXData()` methods
- ✅ Easy access via accessors: `engine_data.GetAssetConfig()`

## FAQs

### Q: Do we have to implement all phases at once?
**A**: No. The migration strategy is designed for gradual adoption. Start with Phase 2 (wrapper structs) whenever ready.

### Q: Will this break existing code?
**A**: No. Old methods remain for backward compatibility. Migration is opt-in.

### Q: What if my nested data is optional?
**A**: Use `std::optional<NestedType>` and provide `HasNested()` accessor method.

### Q: How deep can nesting go?
**A**: Keep to 2-3 levels maximum for maintainability.

### Q: Do I need to update FlatBuffers schemas?
**A**: No. Wrapper structs are C++ only. FlatBuffers schemas stay the same.

### Q: What about performance?
**A**: Caching improves performance. Single load reduces I/O overhead.

### Q: When should I NOT use this pattern?
**A**: When data is truly independent, comes from different sources, or changes frequently.

## Questions or Feedback?

For questions about implementation, see the **Pattern Guide**.
For questions about the decision, see the **ADR**.
For quick lookup, use the **Quick Reference**.

## Related Documentation

- **Data Provider Pattern**: Existing architecture for loading data
- **FlatBuffers Usage**: Serialization format documentation
- **Error Handling**: `std::expected` pattern usage

---

**Status**: Proposed - Awaiting approval and implementation  
**Created**: 2024-12-25  
**Last Updated**: 2024-12-25  
**Author**: GitHub Copilot Agent (Analysis Phase)
