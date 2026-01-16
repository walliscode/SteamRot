# Data Loading & Configuration - Documentation Index

## Overview

This directory contains comprehensive documentation for SteamRot's data loading and configuration architecture. These documents address the design question: *"How do we load data from different sources and configure game objects without coupling our code to specific data formats?"*

## Document Guide

### 📚 Start Here

**[DATA_LOADING_AND_CONFIGURATION.md](DATA_LOADING_AND_CONFIGURATION.md)**
- **Purpose**: Main architecture document
- **Audience**: Developers new to the system, architects
- **Length**: ~27KB, comprehensive
- **Contents**:
  - Design philosophy and core principles
  - Architecture components (Providers, Configurators, Factory)
  - 6 workflow patterns with code examples
  - Step-by-step guides for extending the system
  - Design decisions and tradeoffs
  - Best practices

**When to read**: First time learning the architecture, or when you need to understand the "why" behind design decisions.

---

### ⚡ Quick Lookup

**[DATA_LOADING_QUICK_REFERENCE.md](DATA_LOADING_QUICK_REFERENCE.md)**
- **Purpose**: Fast reference for common tasks
- **Audience**: Developers actively working with the system
- **Length**: ~15KB, focused
- **Contents**:
  - Quick lookup tables
  - Decision flowcharts ("Which pattern should I use?")
  - Interface summary tables
  - Code snippets for common tasks
  - Checklists for adding new types
  - Common mistakes to avoid

**When to read**: When you need a quick reminder, code snippet, or decision tree while coding.

---

### 👁️ Visual Reference

**[DATA_LOADING_DIAGRAMS.md](DATA_LOADING_DIAGRAMS.md)**
- **Purpose**: Visual architecture diagrams
- **Audience**: Visual learners, team presentations
- **Length**: ~24KB, visual
- **Contents**:
  - High-level architecture diagram
  - Component relationship diagrams
  - Data flow diagrams
  - Sequence diagrams for patterns
  - Nested configuration visualization
  - Error handling flow
  - Memory management diagrams

**When to read**: When you want to see the big picture, or need diagrams for presentations or documentation.

---

### 📖 Complete Example

**[EXAMPLE_XML_IMPLEMENTATION.md](EXAMPLE_XML_IMPLEMENTATION.md)**
- **Purpose**: Step-by-step example of adding XML support
- **Audience**: Developers adding new data formats
- **Length**: ~24KB, tutorial
- **Contents**:
  - Complete walkthrough of adding XML as a data type
  - Real code for Provider, Configurator, Importer, Factory
  - Example XML data files
  - Testing strategy
  - Benefits demonstration
  - Common pitfalls and solutions

**When to read**: When implementing a new data format (XML, JSON, YAML, etc.) or need a concrete example.

---

## Reading Paths

### Path 1: "I'm New Here"

1. Start with **[DATA_LOADING_AND_CONFIGURATION.md](DATA_LOADING_AND_CONFIGURATION.md)**
   - Read "Overview" and "Design Philosophy" sections
   - Skim "Architecture Components" to understand the pieces
   - Read "Workflow Patterns" section for usage examples

2. Look at **[DATA_LOADING_DIAGRAMS.md](DATA_LOADING_DIAGRAMS.md)**
   - Review "Architecture Overview" diagram
   - Study "Provider Pattern Flow" diagram
   - Understand "Layer Diagram"

3. Keep **[DATA_LOADING_QUICK_REFERENCE.md](DATA_LOADING_QUICK_REFERENCE.md)** bookmarked
   - Use for code snippets as needed
   - Reference decision flowcharts when unsure

### Path 2: "I Need to Add a New Data Format"

1. Read **[EXAMPLE_XML_IMPLEMENTATION.md](EXAMPLE_XML_IMPLEMENTATION.md)**
   - Follow step-by-step from start to finish
   - Copy code patterns for your format
   - Adapt XML example to your needs

2. Reference **[DATA_LOADING_QUICK_REFERENCE.md](DATA_LOADING_QUICK_REFERENCE.md)**
   - Use "Checklist: Adding New Data Type"
   - Check decision flowcharts

3. Consult **[DATA_LOADING_AND_CONFIGURATION.md](DATA_LOADING_AND_CONFIGURATION.md)**
   - Section: "Adding New Data Types"
   - Section: "Best Practices"

### Path 3: "I Need to Add a New Native Object"

1. Reference **[DATA_LOADING_QUICK_REFERENCE.md](DATA_LOADING_QUICK_REFERENCE.md)**
   - Use "Checklist: Adding New Native Object"
   - Check code snippets

2. Read **[DATA_LOADING_AND_CONFIGURATION.md](DATA_LOADING_AND_CONFIGURATION.md)**
   - Section: "Adding New Native Object Types"
   - Section: "Pattern 4: Nested Configuration"

3. Study **[DATA_LOADING_DIAGRAMS.md](DATA_LOADING_DIAGRAMS.md)**
   - "Nested Configuration Pattern" diagram
   - "Provider Pattern Flow" diagram

### Path 4: "I'm Debugging an Issue"

1. Check **[DATA_LOADING_QUICK_REFERENCE.md](DATA_LOADING_QUICK_REFERENCE.md)**
   - Section: "Common Mistakes to Avoid"
   - Review error handling pattern

2. Review **[DATA_LOADING_DIAGRAMS.md](DATA_LOADING_DIAGRAMS.md)**
   - "Error Handling Flow" diagram
   - Check relevant component diagrams

3. Consult **[DATA_LOADING_AND_CONFIGURATION.md](DATA_LOADING_AND_CONFIGURATION.md)**
   - Section: "Best Practices"
   - Relevant workflow pattern section

### Path 5: "I'm Reviewing Code / Architecture"

1. Start with **[DATA_LOADING_DIAGRAMS.md](DATA_LOADING_DIAGRAMS.md)**
   - Get visual overview
   - Understand component relationships

2. Read **[DATA_LOADING_AND_CONFIGURATION.md](DATA_LOADING_AND_CONFIGURATION.md)**
   - Focus on "Design Decisions and Tradeoffs"
   - Review "Best Practices"

3. Use **[DATA_LOADING_QUICK_REFERENCE.md](DATA_LOADING_QUICK_REFERENCE.md)**
   - Check interface summary tables
   - Verify patterns match documented best practices

---

## Quick Answers to Common Questions

### "How do I load scene data?"

See: [Quick Reference - Loading Scene Data](DATA_LOADING_QUICK_REFERENCE.md#loading-scene-data)

```cpp
auto provider = factory.GetSceneDataProvider();
auto scene_data = provider->ProvideDefaultSceneData(scene_type);
```

### "How do I add XML/JSON support?"

See: [XML Implementation Example](EXAMPLE_XML_IMPLEMENTATION.md) - Follow this pattern for any format.

### "Why use interfaces instead of templates?"

See: [Main Doc - Design Decisions - Interface-Based Design](DATA_LOADING_AND_CONFIGURATION.md#interface-based-design)

### "How do I avoid coupling to FlatBuffers?"

See: [Best Practices - Avoid Data Type Coupling](DATA_LOADING_AND_CONFIGURATION.md#1-avoid-data-type-coupling-in-game-code)

### "When should I use Configure() vs Create()?"

See: [Quick Reference - Configure vs Create](DATA_LOADING_QUICK_REFERENCE.md#when-to-use-configure-vs-create)

### "How does nested configuration work?"

See: [Diagrams - Nested Configuration Pattern](DATA_LOADING_DIAGRAMS.md#nested-configuration-pattern)

### "What's the difference between Provider and Configurator?"

See: [Main Doc - Providers vs Configurators](DATA_LOADING_AND_CONFIGURATION.md#separation-of-providers-and-configurators)

---

## Document Statistics

| Document | Size | Sections | Code Examples | Diagrams |
|----------|------|----------|---------------|----------|
| Main Documentation | 27KB | 15+ | 50+ | Text diagrams |
| Quick Reference | 15KB | 10+ | 20+ | Flowcharts |
| Visual Diagrams | 24KB | 9+ | - | 15+ |
| XML Example | 24KB | 7+ | 30+ | - |
| **Total** | **~90KB** | **41+** | **100+** | **15+** |

---

## Key Concepts Summary

### Core Pattern

```
IDataProvider → Native Object → Game Logic
IConfigurator ↗
```

### Key Interfaces

- **IDataProvider**: Loads and returns native objects from data sources
- **IConfigurator**: Configures existing objects or creates new ones
- **IEntityImporter**: Specialized importer for entity data
- **DataAccessFactory**: Centralized management of providers/configurators

### Key Principles

1. **Format Agnostic**: Game code uses native objects, never format-specific types
2. **Interface Abstraction**: Providers and configurators hide data format details
3. **Factory Pattern**: Centralized switching between data sources
4. **Nested Configuration**: Composable, reusable configuration methods
5. **Compile-Time Safety**: All types known at compile time
6. **Error Handling**: Explicit errors with `std::expected`

### Design Goals Achieved

✅ General data loading solution
✅ Compile-time type safety
✅ No format coupling in game code
✅ IDataProvider and IConfigurator patterns
✅ Support for Create() and Configure()
✅ Factory pattern for runtime flexibility
✅ Nested configuration for code reuse
✅ Selection criteria support
✅ Easy integration with flowcharts
✅ Code readability prioritized

---

## Contributing

When updating this documentation:

1. **Keep documents synchronized**: Cross-reference changes across all docs
2. **Update examples**: Ensure code examples remain accurate
3. **Maintain consistency**: Follow naming conventions and patterns
4. **Add new patterns**: Document new patterns as they emerge
5. **Update this index**: Add new documents or sections here

---

## Related Documentation

### In This Directory
- [DATA_LOADING_AND_CONFIGURATION.md](DATA_LOADING_AND_CONFIGURATION.md) - Main architecture
- [DATA_LOADING_QUICK_REFERENCE.md](DATA_LOADING_QUICK_REFERENCE.md) - Quick lookup
- [DATA_LOADING_DIAGRAMS.md](DATA_LOADING_DIAGRAMS.md) - Visual diagrams
- [EXAMPLE_XML_IMPLEMENTATION.md](EXAMPLE_XML_IMPLEMENTATION.md) - Complete example

### Other Architecture Docs
- Error Handling (if exists)
- Game Loop (if exists)
- Logic System (if exists)

### Main Repository
- [README.md](../../README.md) - Project overview
- [Component Workflows](../workflows/) - Other workflows

---

## Feedback

If you find these documents helpful, confusing, or incomplete:
- Open an issue describing what's unclear
- Suggest improvements or additional examples
- Contribute updates via pull request

---

**Last Updated**: January 2026
**Version**: 1.0
**Status**: Complete and comprehensive documentation suite
