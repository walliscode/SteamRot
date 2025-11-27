# SteamRot Documentation

Welcome to the SteamRot game engine documentation hub. This directory contains comprehensive guides, references, and examples for developing with SteamRot.

## 📚 Documentation Structure

The documentation is organized into logical categories to help you find what you need quickly:

```
documentation/
├── workflows/        # How-to guides for common tasks
├── architecture/     # System design and architecture docs
├── configuration/    # Configuration guides
├── testing/          # Testing guides and strategies
├── naming/           # File and code naming conventions
├── style/            # Code style and formatting
├── examples/         # Code examples and patterns
├── reference/        # API and feature references
└── resources/        # External resources (PDFs, etc.)
```

## 🚀 Getting Started

### For New Developers

**Start Here:**
1. Read the [main README](../README.md) for project overview
2. Review [Code Style Guide](style/STYLE_GUIDE.md) for conventions
3. Understand [File Naming](naming/FILE_NAMING_CONVENTIONS.md) system
4. Explore [Examples](examples/) for code patterns

### For Contributors

**Before You Code:**
1. Review [Testing Overview](testing/TESTING_OVERVIEW.md) for TDD approach
2. Check [Workflows](#workflows) for your task (component, logic, etc.)
3. Read [Architecture](#architecture) docs for relevant systems
4. Follow [Style Guide](style/STYLE_GUIDE.md) conventions

## 🔧 Workflows

Step-by-step guides for common development tasks:

| Guide | Description |
|-------|-------------|
| **[Adding Components](workflows/ADDING_COMPONENTS.md)** | Create new component types with FlatBuffers schemas |
| **[Adding Logic](workflows/ADDING_LOGIC.md)** | Implement game systems (rendering, collision, etc.) |
| **[Adding UI Elements](workflows/ADDING_UI_ELEMENTS.md)** | Create UI components and elements |
| **[Adding Actions](workflows/ADDING_ACTIONS.md)** | Handle user input and actions |

### Quick Workflow Reference

**Need to add a component?** → [Components Workflow](workflows/ADDING_COMPONENTS.md)
1. Create component struct
2. Register in ComponentRegister
3. Create FlatBuffers schema
4. Implement configurator
5. Write tests

**Need to add logic?** → [Logic Workflow](workflows/ADDING_LOGIC.md)
1. Write tests first (TDD)
2. Create Logic class
3. Update LogicFactory
4. Add to scene
5. Verify tests pass

## 🏗️ Architecture

System design documentation:

| Document | Description |
|----------|-------------|
| **[Error Handling](architecture/ERROR_HANDLING.md)** | Exception handling and std::expected patterns |
| **[Game Loop](architecture/GAME_LOOP.md)** | Main game loop and update cycle |
| **[Logic System](architecture/LOGIC_SYSTEM.md)** | Logic classes and LogicFactory architecture |

### Key Architectural Concepts

- **Entity-Component System**: Components are pure data; Logic processes behavior
- **Archetype System**: Entities grouped by component combinations for efficient iteration
- **Data-Driven Design**: Configuration through JSON and FlatBuffers
- **Event System**: Decoupled communication between systems

## ⚙️ Configuration

Configuration system documentation:

| Document | Description |
|----------|-------------|
| **[Context Configuration](configuration/CONTEXT_CONFIGURATION.md)** | Game and scene context setup |
| **[Context Quick Ref](configuration/CONTEXT_CONFIGURATION_QUICK_REF.md)** | Quick reference for context config |
| **[Resource Configuration](configuration/RESOURCE_CONFIGURATION.md)** | Resource management and ownership |
| **[Resource Quick Ref](configuration/RESOURCE_CONFIGURATION_QUICK_REF.md)** | Quick reference for resources |
| **[Resource Architecture](configuration/RESOURCE_CONTEXT_ARCHITECTURE.md)** | Resource vs Context separation |

### Configuration Overview

SteamRot uses FlatBuffers-based configuration for:
- Game context (window size, framerate, etc.)
- Scene contexts (entity pool sizes, render dimensions)
- Resource loading and management

## 🧪 Testing

Testing guides and infrastructure:

| Document | Description |
|----------|-------------|
| **[Testing Overview](testing/TESTING_OVERVIEW.md)** | Test structure, classification, and TDD workflow |
| **[Testing Harness Loop](testing/TESTING_HARNESS_LOOP.md)** | Visual guide to tick-based test execution |
| **[Test Data Configuration](testing/TEST_DATA_CONFIGURATION.md)** | Data-driven testing with FlatBuffers |
| **[Test Data Naming](testing/TEST_DATA_NAMING_CONVENTIONS.md)** | Naming conventions for test data JSON files |

### Testing Quick Reference

**Test Types:**
- `[unit]` - Unit tests (single class, mocked dependencies)
- `[integration]` - Integration tests (multiple components)
- `[system]` - End-to-end tests
- `[.visual]` - Visual confirmation tests (hidden by default)

**Running Tests:**
```bash
ctest --preset Debug              # All tests
ctest --preset Debug -L unit      # Unit tests only
ctest --preset Debug -L integration  # Integration tests only
```

**TDD Workflow:**
1. Write test first
2. Run test (verify it fails)
3. Implement minimal code
4. Run tests (verify pass)
5. Refactor if needed

## 📝 Naming & Style

Conventions for code organization:

### Naming System

| Document | Description |
|----------|-------------|
| **[Naming Overview](naming/NAMING_SYSTEM_OVERVIEW.md)** | High-level naming system overview |
| **[Quick Reference](naming/NAMING_QUICK_REFERENCE.md)** | Decision trees and lookup tables |
| **[Complete Guide](naming/FILE_NAMING_CONVENTIONS.md)** | Detailed rules and rationale |
| **[Visual Guide](naming/naming_system_diagram.md)** | ASCII diagrams and examples |

### Style Guide

| Document | Description |
|----------|-------------|
| **[Code Style](style/STYLE_GUIDE.md)** | Formatting, naming, and conventions |

**Quick Style Reference:**
- Classes: `PascalCase`
- Components: `CComponentName` (C prefix)
- Member variables: `m_variable_name` (m_ prefix)
- Functions: `PascalCase` for public methods
- Indentation: 2 spaces (not tabs)
- Headers: Use `#pragma once`

## 📚 Examples

Code examples and patterns:

| Resource | Description |
|----------|-------------|
| **[Examples Index](examples/README.md)** | Overview of all examples |
| **[Logic Examples](examples/)** | ExampleLogic.h, .cpp, .test.cpp |
| **[CUIState Usage](examples/CUIState_usage.md)** | Component usage example |
| **[TestDataLoader](examples/README_TestDataLoader.md)** | Test data loading examples |
| **[Naming Examples](examples/naming_examples/)** | File naming scenarios |
| **[Matcher with Metadata](examples/ENTITY_MEMORY_POOL_MATCHER_WITH_METADATA.md)** | Testing matcher example |

### Example Workflow

**Learn by Example:**
1. Browse [examples/](examples/) for similar code
2. Copy pattern to your implementation
3. Adapt to your specific needs
4. Follow established conventions

## 📊 Analysis & Research

Research documents and architectural analysis:

| Document | Description |
|----------|-------------|
| **[Test Data Visualization Report](analysis/TEST_DATA_VISUALIZATION_REPORT.md)** | Analysis of approaches for visualizing test_data.json files |
| **[Event Packet Creation Analysis](analysis/EVENT_PACKET_CREATION_ANALYSIS.md)** | Analysis of event packet creation patterns |

## 📑 Reference

Feature and API references:

| Document | Description |
|----------|-------------|
| **[Dropdown Data](reference/dropdown_data_population.md)** | Dropdown implementation details |

## 🎓 Resources

External resources:

- **[Textbooks](resources/textbooks/)** - CMake and C++ references (PDFs)

### Recommended Reading

- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [Pitchfork Layout](https://github.com/vector-of-bool/pitchfork)
- CMake Professional editions (in resources/textbooks/)

## 🔍 Finding What You Need

### By Task

| I want to... | Go to... |
|--------------|----------|
| Add a component | [Adding Components](workflows/ADDING_COMPONENTS.md) |
| Add game logic | [Adding Logic](workflows/ADDING_LOGIC.md) |
| Add UI elements | [Adding UI Elements](workflows/ADDING_UI_ELEMENTS.md) |
| Handle user input | [Adding Actions](workflows/ADDING_ACTIONS.md) |
| Write tests | [Testing Overview](testing/TESTING_OVERVIEW.md) |
| Understand error handling | [Error Handling](architecture/ERROR_HANDLING.md) |
| Configure game/scene | [Context Configuration](configuration/CONTEXT_CONFIGURATION.md) |
| Look up naming patterns | [Naming Quick Reference](naming/NAMING_QUICK_REFERENCE.md) |
| See code examples | [Examples Directory](examples/) |
| Visualize test coverage | [Test Data Visualization Report](analysis/TEST_DATA_VISUALIZATION_REPORT.md) |

### By Topic

| Topic | Documents |
|-------|-----------|
| **Components** | [Adding Components](workflows/ADDING_COMPONENTS.md), [Examples](examples/) |
| **Logic** | [Adding Logic](workflows/ADDING_LOGIC.md), [Logic System](architecture/LOGIC_SYSTEM.md) |
| **Testing** | [Testing Overview](testing/TESTING_OVERVIEW.md), [Test Data Config](testing/TEST_DATA_CONFIGURATION.md), [Visualization](analysis/TEST_DATA_VISUALIZATION_REPORT.md) |
| **Configuration** | [Context](configuration/CONTEXT_CONFIGURATION.md), [Resources](configuration/RESOURCE_CONFIGURATION.md) |
| **Style** | [Style Guide](style/STYLE_GUIDE.md), [Naming](naming/FILE_NAMING_CONVENTIONS.md) |

## 📝 Documentation Conventions

### File Naming
- `UPPERCASE_WITH_UNDERSCORES.md` - Major guides and overviews
- `lowercase_with_underscores.md` - Specific features and references
- Descriptive names that indicate content

### Document Structure
- Clear title and purpose at the start
- Table of contents for long documents
- Organized with headers and sections
- Code examples with language tags
- Related documentation links at end

### Markdown Style
- Relative links between docs
- Code blocks with ```language```
- Tables for comparisons
- Lists for steps and options
- Callouts (✅❌⚠️💡) for emphasis

## 🤝 Contributing to Documentation

When adding or updating documentation:

1. **Choose the right location**
   - Workflows: Step-by-step guides
   - Architecture: System design
   - Reference: API and feature details
   - Examples: Code samples

2. **Follow conventions**
   - Use consistent naming
   - Include TOC for long docs
   - Add code examples
   - Cross-reference related docs

3. **Update this index**
   - Add entry in relevant section
   - Update "Finding What You Need"
   - Verify all links work

4. **Keep current**
   - Update when code changes
   - Remove outdated info
   - Improve clarity over time

## 🆘 Getting Help

**Can't find what you need?**

1. Check the [main README](../README.md) first
2. Search this documentation index
3. Browse [examples/](examples/) for patterns
4. Review [copilot instructions](../.github/copilot-instructions.md)
5. Ask the team

**Common Questions:**

- **"How do I add X?"** → Check [Workflows](#workflows)
- **"Why does X work this way?"** → Check [Architecture](#architecture)
- **"What's the naming convention?"** → Check [Naming](#naming--style)
- **"Where can I see an example?"** → Check [Examples](#examples)

## 📋 Documentation TODO

Future documentation improvements:

- [ ] Getting Started guide for new developers
- [ ] Quick Start tutorial (first feature)
- [ ] Asset management guide
- [ ] Scene configuration guide
- [ ] Event system overview
- [ ] Architecture decision records (ADRs)
- [ ] Performance optimization guide
- [ ] Debugging guide

## 🔄 Recent Updates

- **November 2024**: Complete documentation restructure
  - Organized into logical directories
  - Extracted workflows from main README
  - Created comprehensive documentation hub
  - Improved navigation and discoverability

- **October 2024**: File naming system
  - Added comprehensive naming documentation
  - Created decision trees and quick references
  - Documented migration strategies

---

**💡 Tip**: Bookmark this page! It's your starting point for all SteamRot documentation.

**📌 Note**: Documentation is a living resource. Keep it updated as the project evolves!
