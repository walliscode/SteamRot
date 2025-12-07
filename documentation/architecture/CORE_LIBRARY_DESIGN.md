# Core Library Design

## Overview

The `core` library is a header-only interface library containing fundamental data structures and type utilities used throughout the SteamRot engine. It sits at the bottom of the dependency tree with **zero external dependencies** (except the C++ standard library).

## Purpose

The core library addresses several key architectural goals:

1. **Separation of Data from Implementation**: Pure data structures are separated from their usage
2. **Dependency Minimization**: No external library dependencies (SFML, FlatBuffers, etc.)
3. **Build Time Optimization**: Header-only interface library minimizes recompilation
4. **Clear Dependency Hierarchy**: Core is at the base, all other libraries can depend on it
5. **Breaking Circular Dependencies**: Provides shared types without creating cycles

## Contents

### Error Handling Types (`FailInfo.h`)

Contains fundamental error handling types:
- `FailMode` enum: Categorizes different types of failures
- `FailInfo` struct: Pairs a failure mode with a descriptive message

**Usage:**
```cpp
#include "FailInfo.h"  // From core or logger (forward header)

std::expected<std::monostate, steamrot::FailInfo> result = 
    DoSomething();
    
if (!result.has_value()) {
    const auto& error = result.error();
    // error.mode, error.message
}
```

### Component Base Interface (`Component.h`)

Pure interface for all game components:
- Virtual destructor
- `GetComponentRegisterIndex()` pure virtual method
- `m_active` flag for archetype calculations

**Usage:**
```cpp
#include "Component.h"  // From core or components (forward header)

struct CMyComponent : public steamrot::Component {
    size_t GetComponentRegisterIndex() const override;
    // Component-specific data members
};
```

### Type Traits Utilities (`type_traits.h`)

Generic template utilities for tuple manipulation:
- `ComponentContainer`: Maps tuple types to vector-of-types
- `IndexOf`: Compile-time type index lookup in tuples
- `TupleTypeIndex`: Helper alias for getting type indices

**Usage:**
```cpp
#include "type_traits.h"

using MyTuple = std::tuple<int, float, std::string>;
constexpr size_t idx = steamrot::TupleTypeIndex<float, MyTuple>;
// idx == 1
```

## Design Principles

### 1. Interface Library (Header-Only)

The core library is an `INTERFACE` library in CMake:

```cmake
add_library(core INTERFACE)
target_include_directories(core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
```

**Benefits:**
- No compilation unit for core itself
- Changes only trigger recompilation of dependent translation units
- Minimal build time impact

### 2. Zero External Dependencies

Core types must NOT depend on:
- SFML
- FlatBuffers
- nlohmann_json
- magic_enum
- Any non-standard library

**Only standard library headers are allowed.**

### 3. Pure Data Structures

Core should contain only:
- POD (Plain Old Data) types when possible
- Pure interfaces (abstract classes with no state)
- Template metaprogramming utilities
- Type traits and compile-time computations

### 4. Forward Compatibility

Existing code uses includes like:
```cpp
#include "Component.h"  // From components/
#include "FailInfo.h"   // From logger/
```

Forward headers maintain compatibility:
```cpp
// components/Component.h
#pragma once
#include "../core/Component.h"
```

## Dependency Architecture

### Before Core Library

```
┌─────────────┐
│ components  │◄───┐
└──────┬──────┘    │
       │           │
       ▼           │
   ┌────────┐     │
   │ logger │     │
   └───┬────┘     │
       │          │
       └──────────┘ Circular!
```

### After Core Library

```
     ┌──────────────┐
     │   Engine     │
     └──────┬───────┘
            │
     ┌──────▼───────┐
     │  Components  │
     └──────┬───────┘
            │
     ┌──────▼───────┐
     │   Logger     │
     └──────┬───────┘
            │
     ┌──────▼───────┐
     │   Core       │  ◄─── Zero dependencies
     └──────────────┘
```

## When to Add to Core

Add a type to core when:

1. ✅ **Multiple libraries need it** (shared across components, logger, entity, etc.)
2. ✅ **It's a pure data structure** (no complex logic)
3. ✅ **It has no external dependencies** (only std:: types)
4. ✅ **It creates circular dependencies** when placed elsewhere
5. ✅ **It's fundamental to the engine** (error types, base interfaces)

**Do NOT add to core:**

- ❌ Types specific to one subsystem only
- ❌ Types requiring external libraries (SFML, FlatBuffers, etc.)
- ❌ Implementation-heavy classes with complex logic
- ❌ Types that change frequently

## Migration Guide

### Moving a Type to Core

1. **Copy the header to `src/core/`**
   ```bash
   cp src/logger/FailInfo.h src/core/FailInfo.h
   ```

2. **Remove external dependencies**
   ```cpp
   // Before
   #include <nlohmann/json.hpp>
   
   // After
   #include <string>
   ```

3. **Create forward header in original location**
   ```cpp
   // src/logger/FailInfo.h
   #pragma once
   #include "../core/FailInfo.h"
   ```

4. **Update CMakeLists.txt dependencies**
   ```cmake
   target_link_libraries(logger PUBLIC core spdlog config)
   ```

### Using Core Types

Simply include from the original location (forward header) or directly:

```cpp
// Option 1: Use forward header (existing code keeps working)
#include "FailInfo.h"        // from components/
#include "Component.h"       // from logger/

// Option 2: Include directly from core
#include "core/FailInfo.h"
#include "core/Component.h"
```

## Build System Integration

### CMakeLists.txt Order

The core library must be added **first** in `src/CMakeLists.txt`:

```cmake
add_subdirectory(core)        # First - no dependencies
add_subdirectory(config)      # Can depend on core
add_subdirectory(logger)      # Can depend on core, config
add_subdirectory(components)  # Can depend on core, logger
# ... rest
```

### Link Order

Libraries should link to core explicitly:

```cmake
target_link_libraries(my_library
  PUBLIC
  core                 # Always safe to link
  other_dependencies
)
```

## Performance Considerations

### Compile-Time Impact

- **Header-only**: No linking step for core itself
- **Template-heavy**: May increase compilation time per translation unit
- **Minimal includes**: Keep core headers lean

### Runtime Impact

- **Zero overhead**: Interface library adds no runtime cost
- **Inlining**: Template functions can be fully inlined
- **No ABI boundary**: All definitions visible at call site

## Examples

### Example 1: Error Handling

```cpp
#include "FailInfo.h"
#include <expected>

std::expected<int, steamrot::FailInfo> 
DivideNumbers(int a, int b) {
    if (b == 0) {
        return std::unexpected(
            steamrot::FailInfo{
                steamrot::FailMode::InvalidInput,
                "Division by zero"
            }
        );
    }
    return a / b;
}
```

### Example 2: Component Definition

```cpp
#include "Component.h"
#include "containers.h"

namespace steamrot {

struct CMyComponent : public Component {
    float m_value{0.0f};
    std::string m_name;
    
    size_t GetComponentRegisterIndex() const override {
        return TupleTypeIndex<CMyComponent, ComponentRegister>;
    }
};

} // namespace steamrot
```

### Example 3: Type Trait Usage

```cpp
#include "type_traits.h"

// Define a component register
using MyComponentRegister = std::tuple<CComponent1, CComponent2, CComponent3>;

// Create entity memory pool type
using MyEntityMemoryPool = 
    ComponentContainer<MyComponentRegister>::ComponentVectorTuple;
// Expands to: std::tuple<std::vector<CComponent1>, 
//                        std::vector<CComponent2>, 
//                        std::vector<CComponent3>>
```

## Testing

Core types should be tested indirectly through their usage in other components:

- Component registration tests use `Component` interface
- Error handling tests use `FailInfo`
- Template utilities are tested via component systems

## Future Expansion

Candidates for moving to core:

1. **ArchetypeID**: Used by entity, logic, and archetype manager
2. **SceneType enum**: Fundamental to scene management
3. **Generic math utilities**: If they become widely used
4. **Common enums**: That appear across multiple subsystems

## Best Practices

### ✅ Do

- Keep core headers minimal
- Use standard library types
- Document all public APIs
- Maintain backward compatibility via forward headers
- Test through integration with other libraries

### ❌ Don't

- Add implementation (.cpp) files to core
- Include external library headers
- Add subsystem-specific types
- Create dependencies between core headers
- Break existing includes (use forward headers)

## Maintenance

### Adding New Core Types

1. Create header in `src/core/`
2. Update this documentation
3. Create forward header if needed
4. Update dependent CMakeLists.txt
5. Test through integration

### Removing Core Types

1. Ensure no libraries depend on the type
2. Remove from core
3. Remove forward headers
4. Update documentation

## Conclusion

The core library provides a clean foundation for the SteamRot engine by:

- Establishing clear dependency hierarchy
- Separating data from implementation
- Minimizing build times
- Breaking circular dependencies
- Providing reusable fundamental types

When in doubt, ask: "Is this type fundamental enough to be in core?" If yes, and it has no external dependencies, it belongs here.
