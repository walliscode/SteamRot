# Data Structure Separation Strategy

## Problem Statement

The SteamRot codebase had grown with tightly coupled dependencies where:
- Data structures were mixed with their implementations
- Circular dependencies existed between libraries
- Simple changes triggered extensive recompilation
- Build times were longer than necessary
- Test infrastructure was mixed with production code

## Solution Overview

We implemented a **separation of concerns** architecture following the principle:

```
Data Source (JSON, Binary, XML)
    ↓
Interface/Adapter Layer (Data Providers)
    ↓
Standard Data Structures (Core Library)
    ↓
Implementation (Game Systems)
```

## Core Library

The heart of the solution is the **`core` interface library**:

### Characteristics
- **Header-only**: No compilation unit, minimal rebuild impact
- **Zero external dependencies**: Only C++ standard library
- **Bottom of dependency tree**: All libraries can safely depend on it
- **Pure data structures**: No complex implementation logic

### Contents
- `FailInfo.h` - Error handling types
- `Component.h` - Base component interface
- `type_traits.h` - Template metaprogramming utilities

See [CORE_LIBRARY_DESIGN.md](CORE_LIBRARY_DESIGN.md) for complete documentation.

## Architecture Changes

### Before

```
┌─────────┐     ┌──────────┐
│ Engine  │────▶│ Display  │
└────┬────┘     └──────────┘
     │
     ▼
┌──────────────┐     ┌──────────┐
│  Components  │◄───▶│  Logger  │
└──────┬───────┘     └────┬─────┘
       │                  │
       └──────────────────┘
         Circular Deps!
```

Problems:
- Circular dependencies between components and logger
- FailInfo in logger but needed by components
- Component interface in components but needed by entity
- Heavy external dependencies even for simple types

### After

```
┌─────────┐     ┌──────────┐
│ Engine  │────▶│ Display  │
└────┬────┘     └──────────┘
     │
     ▼
┌──────────────┐
│  Components  │
└──────┬───────┘
       │
       ▼
┌──────────────┐     ┌──────────────┐
│   Logger     │     │ Data Handlers │
└──────┬───────┘     └──────┬────────┘
       │                    │
       └────────┬───────────┘
                │
                ▼
         ┌─────────────┐
         │    Core     │  ← Zero dependencies
         └─────────────┘
```

Benefits:
- No circular dependencies
- Clear dependency hierarchy
- Minimal recompilation on changes
- Separation of data from implementation

## Dependency Levels

### Level 0: Core (No Dependencies)
- Pure data structures
- Base interfaces
- Template utilities
- Error types

### Level 1: Infrastructure (Depends on Core)
- `config` - Configuration paths
- `logger` - Logging infrastructure

### Level 2: Data Layer (Depends on Core + Level 1)
- `data_handlers` - File I/O
- `flatbuffers_headers` - Generated schemas

### Level 3: Game Types (Depends on Core + Level 1-2)
- `components` - Game component definitions
- `assets` - Asset management
- `events` - Event system

### Level 4: Game Logic (Depends on Core + Level 1-3)
- `entity` - Entity management
- `logic` - Game logic systems
- `scenes` - Scene management

### Level 5: Engine (Depends on Everything)
- `engine` - Top-level engine coordination

## Migration Strategy

### Phase 1: Create Core (Completed ✅)
1. Created `src/core/` directory with INTERFACE library
2. Moved fundamental types:
   - `FailInfo` from logger
   - `Component` from components
   - Template utilities from containers
3. Created forward headers for backward compatibility
4. Updated CMakeLists.txt dependencies

### Phase 2: Data Provider Refinement (Future)
1. Ensure all data providers return standard structures
2. Decouple FlatBuffers from core types
3. Create clean adapter interfaces

### Phase 3: Break Remaining Cycles (Future)
1. Remove matchers (test lib) from events (production lib)
2. Separate UI types from engine dependencies
3. Consider additional types for core

## Build System Changes

### CMakeLists.txt Structure

```cmake
# src/CMakeLists.txt
add_subdirectory(core)        # Level 0 - First!
add_subdirectory(config)      # Level 1
add_subdirectory(logger)      # Level 1
add_subdirectory(data_handlers) # Level 2
# ... etc in dependency order
```

### Compiler Change

Changed from `clang++` to `g++-14`:
- Reason: std::expected support (C++23 feature)
- clang++ libc++ doesn't have std::expected yet
- g++-14 libstdc++ fully supports C++23

## Impact Analysis

### Build Time Improvements

**Before:**
- Changing FailInfo.h → Rebuild logger, components, entity, logic, etc.
- Changing Component.h → Rebuild components, entity, logic, etc.

**After:**
- Changing FailInfo.h in core → Rebuild only affected translation units
- Interface library has zero compilation unit itself
- Forward headers minimize include graph changes

### Dependency Clarity

**Before:**
```cmake
# components/CMakeLists.txt
target_link_libraries(components PUBLIC
  nlohmann_json SFML magic_enum flatbuffers 
  flatbuffers_headers logger engine user_interface)
  # Circular dep: components -> engine -> components!
```

**After:**
```cmake
# components/CMakeLists.txt
target_link_libraries(components PUBLIC
  core  # Clear dependency
  nlohmann_json SFML magic_enum flatbuffers 
  flatbuffers_headers logger engine user_interface)
```

### Code Organization

**Before:**
- Fundamental types scattered across subsystem libraries
- No clear place for shared utilities
- Difficulty determining where new types belong

**After:**
- Core provides clear home for fundamentals
- Decision tree: "Does it have external deps? No → Core"
- Subsystem types stay in subsystems

## Guidelines for Future Development

### When to Add to Core

✅ **DO** add to core if:
- Multiple libraries need it
- It's a pure data structure
- No external dependencies (only std::)
- Creates circular deps elsewhere
- Fundamental to the engine

❌ **DON'T** add to core if:
- Specific to one subsystem
- Requires external libraries
- Contains complex logic
- Changes frequently

### Adding New Data Structures

1. **Evaluate placement:**
   - Subsystem-specific? → Put in that subsystem
   - Shared, simple, no deps? → Consider core
   - Shared, complex, has deps? → Create interface layer

2. **Consider dependencies:**
   ```
   Q: Does it depend on SFML, FlatBuffers, JSON, etc.?
   Yes → NOT core
   No → Might be core
   ```

3. **Check for cycles:**
   ```
   Q: Would placing it in subsystem X create circular deps?
   Yes → Consider core or interface pattern
   No → Keep in subsystem
   ```

### Data Provider Pattern

For data loaded from files:

```
FlatBuffers Data (Binary)
    ↓
FlatbuffersDataProvider (Adapter)
    ↓
Standard Data Struct (Core or subsystem)
    ↓
Game System (Uses the data)
```

**Example:**
```cpp
// Core type (simple, no deps)
struct EngineConfig {
    std::string window_title;
    uint32_t framerate_limit;
};

// Data provider (adapter)
class FlatbuffersEngineDataProvider {
    std::expected<EngineConfig, FailInfo> 
    ProvideEngineConfig();
};

// Usage
auto config_result = provider.ProvideEngineConfig();
if (config_result.has_value()) {
    engine.Configure(config_result.value());
}
```

## Benefits Realized

### 1. Faster Builds
- Interface library has no compilation unit
- Changes to core types only rebuild affected files
- Clean dependency tree reduces cascading rebuilds

### 2. Better Organization
- Clear place for fundamental types
- No more "where does this go?" questions
- Separation of data from implementation

### 3. Easier Testing
- Core types are simple and easily testable
- No mock external dependencies needed
- Test through integration with systems

### 4. Scalability
- Easy to add new fundamental types
- Clean dependency hierarchy supports growth
- Refactoring has clear targets

### 5. Maintenance
- Breaking changes have limited scope
- Forward headers maintain compatibility
- Documentation centralizes architecture decisions

## Lessons Learned

### ✅ What Worked

1. **Interface library pattern**: Zero-cost abstraction
2. **Forward headers**: Maintained backward compatibility
3. **Documentation first**: Clarified decisions before code
4. **Incremental approach**: Core first, refinements later

### ⚠️ What to Watch

1. **Template bloat**: Keep type_traits minimal
2. **Include graph**: Don't let core headers depend on each other
3. **Over-abstraction**: Not everything belongs in core
4. **Compilation time**: Monitor template instantiation costs

## Future Work

### Short Term
1. Fix pre-existing test failures (unrelated to refactoring)
2. Remove test library (matchers) from production deps (events)
3. Document all data provider interfaces

### Medium Term
1. Move more fundamental types to core:
   - ArchetypeID
   - SceneType enum
   - Common math utilities
2. Create interface libraries for other layers
3. Further reduce cross-cutting dependencies

### Long Term
1. Evaluate module system (C++20)
2. Consider precompiled headers for large templates
3. Measure and optimize build times
4. Create tooling for dependency visualization

## Conclusion

The separation of data structures into the core library successfully:

- ✅ Broke circular dependencies
- ✅ Established clear architectural layers
- ✅ Improved build time characteristics
- ✅ Provided a foundation for future scalability
- ✅ Maintained backward compatibility

The pattern of "data source → interface → standard structs → implementation" provides a clear mental model for organizing code and can be applied to other subsystems as the engine evolves.

## References

- [CORE_LIBRARY_DESIGN.md](CORE_LIBRARY_DESIGN.md) - Detailed core library documentation
- [ISUBSCRIBER_DATAPROVIDER_IMPLEMENTATION.md](../../ISUBSCRIBER_DATAPROVIDER_IMPLEMENTATION.md) - Data provider patterns
- [ENGINE_DATA_ORGANIZATION.md](../proposals/ENGINE_DATA_ORGANIZATION.md) - Engine data organization
- Main README.md - General coding guidelines

## Appendix: Quick Reference

### Core Library Files
```
src/core/
├── CMakeLists.txt      # Interface library definition
├── Component.h         # Base component interface
├── FailInfo.h         # Error handling types
└── type_traits.h      # Template utilities
```

### Dependency Commands
```cmake
# To use core in your library:
target_link_libraries(my_library PUBLIC core)

# Core has no dependencies:
add_library(core INTERFACE)
target_include_directories(core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
```

### Decision Tree
```
New data structure?
  ↓
Does it have external deps? (SFML, FlatBuffers, etc.)
  ├─ Yes → NOT core (put in subsystem or create adapter)
  └─ No → Is it fundamental? Shared across multiple systems?
           ├─ Yes → Consider core
           └─ No → Put in specific subsystem
```
