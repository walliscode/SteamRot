# SteamRot Architecture Documentation

This directory contains architecture analysis and design documentation for the SteamRot game engine.

## Native Object Creation and Configuration

A comprehensive analysis and proposal for generalizing the native object creation and configuration system.

### Document Overview

#### [ARCHITECTURE_SUMMARY.md](ARCHITECTURE_SUMMARY.md) - Start Here
**Executive summary of the proposed architecture**

Read this first for a high-level overview of:
- The problem statement
- Proposed solution overview
- Key design decisions
- Benefits and comparison with current system
- Migration strategy
- Direct answers to specific questions

**Audience:** All team members, decision makers

---

#### [NATIVE_OBJECT_CREATION_ARCHITECTURE.md](NATIVE_OBJECT_CREATION_ARCHITECTURE.md) - Deep Dive
**Detailed architecture analysis and design**

Comprehensive documentation covering:
- Current architecture analysis (strengths and weaknesses)
- Proposed generalized architecture
- Core principles and patterns
- Template-based factory design
- Addressing specific concerns from problem statement
- Benefits and comparison tables
- Example implementations

**Audience:** Developers implementing the system

---

#### [ADDRESSING_SPECIFIC_QUESTIONS.md](ADDRESSING_SPECIFIC_QUESTIONS.md) - Q&A Format
**Direct answers to problem statement questions**

Each question from the problem statement answered with:
- Clear, concise answer
- Key insight explanation
- Working code examples
- Benefits summary

Questions addressed:
1. How to accept specific data types without type erasure?
2. Should top-level objects source their own data?
3. How does A's interface call B's interface when A contains B?
4. Should shared objects have their own interface?

**Audience:** Developers with specific questions

---

#### [IMPLEMENTATION_GUIDE.md](IMPLEMENTATION_GUIDE.md) - How-To
**Step-by-step implementation instructions**

Practical guide for implementing the proposed architecture:
- Prerequisites and setup
- Creating core template infrastructure
- Creating registry specializations
- Refactoring existing configurators
- Adding new data types
- Testing approach
- Complete code examples

**Audience:** Developers implementing the changes

---

#### [examples/CONFIGURATOR_PATTERN_EXAMPLES.md](examples/CONFIGURATOR_PATTERN_EXAMPLES.md) - Code Examples
**Concrete, working code examples**

Five complete examples showing:
1. Basic template-based configurator
2. Configurator registry infrastructure
3. Usage patterns
4. Composite configuration (A contains B)
5. Adding new data type (JSON)
6. Free functions for reusability
7. Migration from current code

**Audience:** Developers looking for copy-paste examples

---

## Quick Start Guide

### For Decision Makers
1. Read [ARCHITECTURE_SUMMARY.md](ARCHITECTURE_SUMMARY.md)
2. Review "Benefits" and "Comparison with Current System" sections
3. Evaluate migration strategy

### For Architects/Lead Developers
1. Read [ARCHITECTURE_SUMMARY.md](ARCHITECTURE_SUMMARY.md)
2. Read [NATIVE_OBJECT_CREATION_ARCHITECTURE.md](NATIVE_OBJECT_CREATION_ARCHITECTURE.md)
3. Review [ADDRESSING_SPECIFIC_QUESTIONS.md](ADDRESSING_SPECIFIC_QUESTIONS.md)
4. Evaluate design decisions and trade-offs

### For Implementing Developers
1. Read [ARCHITECTURE_SUMMARY.md](ARCHITECTURE_SUMMARY.md)
2. Follow [IMPLEMENTATION_GUIDE.md](IMPLEMENTATION_GUIDE.md)
3. Reference [examples/CONFIGURATOR_PATTERN_EXAMPLES.md](examples/CONFIGURATOR_PATTERN_EXAMPLES.md) as needed
4. Consult [ADDRESSING_SPECIFIC_QUESTIONS.md](ADDRESSING_SPECIFIC_QUESTIONS.md) for specific issues

### For New Team Members
1. Read [ARCHITECTURE_SUMMARY.md](ARCHITECTURE_SUMMARY.md) for overview
2. Browse [examples/CONFIGURATOR_PATTERN_EXAMPLES.md](examples/CONFIGURATOR_PATTERN_EXAMPLES.md) for patterns
3. Consult other docs as needed for specific areas

---

## Key Concepts

### Template-Based Configuration
Instead of runtime switching, use compile-time template specialization:

```cpp
// Generic interface
template<typename DataType, typename ObjectType>
class IConfigurator { /* ... */ };

// Concrete implementation knows types at compile time
class FlatbuffersSceneConfigurator 
    : public IConfigurator<SceneDataFbs, Scene> { /* ... */ };
```

### Configurator Registry
Single location for data type coupling:

```cpp
template<DataSourceType Source, typename Data, typename Object>
struct ConfiguratorRegistry {
  static std::unique_ptr<IConfigurator<Data, Object>> Create(...);
};

// Specialization - only coupling point
template<>
struct ConfiguratorRegistry<Flatbuffers, SceneDataFbs, Scene> {
  static auto Create(...) { 
    return std::make_unique<FlatbuffersSceneConfigurator>(...);
  }
};
```

### Composition for Nested Objects
When A contains B, A's configurator calls B's configurator:

```cpp
class SceneConfigurator {
  std::expected<std::monostate, FailInfo>
  Configure(Scene& scene, const SceneDataFbs& data) override {
    // Get entity configurator from factory
    auto entity_config = m_factory.GetConfigurator<EntityData, Entities>();
    
    // Use it to configure scene's entities
    entity_config->Configure(scene.GetEntities(), data.entities());
  }
};
```

### Free Functions for Simple Types
Stateless configuration functions for simple nested types:

```cpp
std::expected<std::monostate, FailInfo>
ConfigureSceneInfo(SceneInfo& info, const SceneInfoFbs* fb_info) {
  if (!fb_info) return std::unexpected(...);
  info.type = fb_info->type();
  return std::monostate{};
}
```

---

## Design Principles

1. **Interfaces for standalone types**: Scene, Entity, UIStyle get interfaces
2. **Free functions for nested types**: SceneInfo, SceneState use free functions
3. **Stateless configuration**: All functions receive what they need as parameters
4. **Composition over inheritance**: Configurators call other configurators
5. **Compile-time extensibility**: New types via template specialization
6. **Explicit coupling**: Only in registry specializations

---

## Benefits Summary

### Extensibility
- ✅ Add new data type with single template specialization
- ✅ No modification to existing code
- ✅ Compile-time type checking

### Maintainability
- ✅ Clear coupling points (registry only)
- ✅ Consistent pattern across all types
- ✅ Self-documenting (template parameters explicit)

### Reusability
- ✅ Shared objects configured in one place
- ✅ Free functions callable from anywhere
- ✅ Composition promotes reuse

### Type Safety
- ✅ No type erasure
- ✅ Full compile-time checking
- ✅ No runtime casts

### Performance
- ✅ Compile-time dispatch (no switch overhead)
- ✅ Factory caches instances
- ✅ Direct function calls

---

## Related Documentation

- `.github/copilot-instructions.md` - General coding guidelines
- `README.md` - Project overview
- Source code in `src/` directory

---

## Feedback and Questions

This is analysis and documentation only - no code changes have been made.

For questions or feedback:
1. Review the specific document addressing your concern
2. Consult the examples for clarification
3. Discuss with team leads for implementation decisions

---

## Document Status

- **Status**: Analysis and Proposal
- **Date**: 2026-01-13
- **Scope**: Architecture design, no implementation
- **Next Steps**: Team review and decision on adoption

---

## Version History

- **v1.0** (2026-01-13): Initial architecture analysis and proposal
  - Executive summary
  - Detailed architecture documentation
  - Q&A addressing specific questions
  - Implementation guide
  - Working code examples
