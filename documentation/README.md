# SteamRot Documentation

Welcome to the SteamRot game engine documentation. This directory contains guides, examples, and references for developing with SteamRot.

## Quick Navigation

### 🚀 Getting Started
- **[../README.md](../README.md)** - Main project README with workflows and structure
- **[../.github/copilot-instructions.md](../.github/copilot-instructions.md)** - Development guidelines

### 📁 File Naming System (NEW!)
A cohesive naming system for organizing code as the project scales.

- **[NAMING_SYSTEM_OVERVIEW.md](NAMING_SYSTEM_OVERVIEW.md)** ⭐ **Start here!**
  - High-level overview and quick start
  - Links to all naming documentation
  - Common questions answered

- **[NAMING_QUICK_REFERENCE.md](NAMING_QUICK_REFERENCE.md)** 📋 **Daily use**
  - Decision trees and lookup tables
  - Examples and anti-patterns
  - Quick pattern reference

- **[FILE_NAMING_CONVENTIONS.md](FILE_NAMING_CONVENTIONS.md)** 📕 **Complete guide**
  - Detailed rules and rationale
  - Migration strategy
  - Extensibility guidelines

- **[naming_system_diagram.md](naming_system_diagram.md)** 📊 **Visual guide**
  - ASCII diagrams and decision trees
  - Growth patterns visualization
  - Step-by-step examples

### 📚 Code Examples

#### Logic Classes
- **[examples/README.md](examples/README.md)** - Logic class examples overview
- **[examples/ExampleLogic.h](examples/ExampleLogic.h)** - Example header
- **[examples/ExampleLogic.cpp](examples/ExampleLogic.cpp)** - Example implementation  
- **[examples/ExampleLogic.test.cpp](examples/ExampleLogic.test.cpp)** - Example tests

#### File Naming Examples
- **[examples/naming_examples/README.md](examples/naming_examples/README.md)** - Concrete naming scenarios
  - Real-world examples
  - Directory structures
  - Migration paths

#### Component Usage
- **[examples/CUIState_usage.md](examples/CUIState_usage.md)** - CUIState component guide

### 🔧 Feature Guides

- **[dropdown_data_population.md](dropdown_data_population.md)** - Dropdown data population guide

### 🧪 Testing Infrastructure

- **[TEST_DATA_CONFIGURATION.md](TEST_DATA_CONFIGURATION.md)** - Data-driven testing guide
  - Creating test data files
  - Using TestDataLoader
  - Simulations and tick-based execution
  
- **[TEST_HARNESS_DATA_FLOW.md](TEST_HARNESS_DATA_FLOW.md)** ⭐ **Architecture overview**
  - Complete data/function flow
  - Component interactions
  - Test lifecycle from JSON to execution
  - Extension points and patterns

- **[tests/harness/README.md](../tests/harness/README.md)** - Test harness API reference

### 🏗️ Architecture & Configuration

- **[CONTEXT_CONFIGURATION.md](CONTEXT_CONFIGURATION.md)** - Context configuration system
- **[CONTEXT_CONFIGURATION_QUICK_REF.md](CONTEXT_CONFIGURATION_QUICK_REF.md)** - Quick reference
- **[RESOURCE_CONFIGURATION.md](RESOURCE_CONFIGURATION.md)** - Resource configuration
- **[RESOURCE_CONFIGURATION_QUICK_REF.md](RESOURCE_CONFIGURATION_QUICK_REF.md)** - Quick reference
- **[RESOURCE_CONTEXT_ARCHITECTURE.md](RESOURCE_CONTEXT_ARCHITECTURE.md)** - Architecture overview

## Documentation Structure

```
documentation/
├── README.md (this file)              # Documentation index
│
├── Naming System/
│   ├── NAMING_SYSTEM_OVERVIEW.md      # Start here for naming
│   ├── NAMING_QUICK_REFERENCE.md      # Daily quick reference
│   ├── FILE_NAMING_CONVENTIONS.md     # Complete naming guide
│   └── naming_system_diagram.md       # Visual diagrams
│
├── Testing Infrastructure/
│   ├── TEST_HARNESS_DATA_FLOW.md      # Architecture & flow overview
│   └── TEST_DATA_CONFIGURATION.md     # Data-driven testing guide
│
├── Architecture & Configuration/
│   ├── CONTEXT_CONFIGURATION.md       # Context configuration
│   ├── RESOURCE_CONFIGURATION.md      # Resource configuration
│   └── RESOURCE_CONTEXT_ARCHITECTURE.md  # Architecture overview
│
├── examples/
│   ├── README.md                      # Examples overview
│   ├── ExampleLogic.{h,cpp,test.cpp} # Logic class examples
│   ├── CUIState_usage.md             # Component examples
│   └── naming_examples/
│       └── README.md                  # Naming pattern examples
│
└── Feature Guides/
    └── dropdown_data_population.md    # Specific feature docs
```

## Finding What You Need

### I want to...

**...understand the file naming system**
→ Start with [NAMING_SYSTEM_OVERVIEW.md](NAMING_SYSTEM_OVERVIEW.md)

**...quickly look up a naming pattern**
→ Check [NAMING_QUICK_REFERENCE.md](NAMING_QUICK_REFERENCE.md)

**...learn how to add a Logic class**
→ See [examples/README.md](examples/README.md) and main [README.md](../README.md)

**...see concrete naming examples**
→ Browse [examples/naming_examples/README.md](examples/naming_examples/README.md)

**...understand the complete naming rationale**
→ Read [FILE_NAMING_CONVENTIONS.md](FILE_NAMING_CONVENTIONS.md)

**...visualize the naming patterns**
→ Review [naming_system_diagram.md](naming_system_diagram.md)

**...know where to add new functions**
→ Use the decision tree in [NAMING_QUICK_REFERENCE.md](NAMING_QUICK_REFERENCE.md)

**...understand the test harness architecture**
→ Start with [TEST_HARNESS_DATA_FLOW.md](TEST_HARNESS_DATA_FLOW.md)

**...write data-driven tests**
→ Check [TEST_DATA_CONFIGURATION.md](TEST_DATA_CONFIGURATION.md)

**...use the test harness API**
→ See [tests/harness/README.md](../tests/harness/README.md)

## Contributing to Documentation

When adding new documentation:

1. **Place it logically** - Use subdirectories for related content
2. **Update this index** - Add links to new documents
3. **Cross-reference** - Link to related documentation
4. **Use clear titles** - Make the content discoverable
5. **Include examples** - Show, don't just tell
6. **Keep it current** - Update when code patterns change

## Documentation Conventions

### File Names
- Use `UPPERCASE_WITH_UNDERSCORES.md` for major guides
- Use `lowercase_with_underscores.md` for specific features
- Use descriptive names that indicate content

### Structure
- Start with a clear title and purpose
- Include a table of contents for long documents
- Use headers to organize content
- Provide examples and code snippets
- End with related documentation links

### Markdown Style
- Use relative links between documentation files
- Include code blocks with language specifications
- Use tables for comparisons and references
- Use lists for steps and options
- Use callouts (✅❌⚠️) for emphasis

## Getting Help

- **Project questions**: Check [main README](../README.md)
- **Code style**: See [copilot instructions](../.github/copilot-instructions.md)
- **Naming patterns**: Use the naming system docs (above)
- **Examples**: Browse the [examples/](examples/) directory
- **Still stuck**: Ask the team

## Documentation TODO

Potential future documentation:

- [ ] Component creation workflow guide
- [ ] FlatBuffers schema guide
- [x] Testing patterns and strategies (see TEST_HARNESS_DATA_FLOW.md)
- [ ] Asset management guide
- [ ] Scene configuration guide
- [ ] Event system overview
- [ ] Architecture decision records

## Recent Updates

- **2024**: Added comprehensive test harness data flow documentation
  - Complete architecture overview in TEST_HARNESS_DATA_FLOW.md
  - Data flow from JSON to test execution
  - Component interactions and lifecycle
  - Extension points and usage patterns

- **2024**: Added comprehensive file naming system documentation
  - Overview, quick reference, complete guide, and visual diagrams
  - Examples and migration strategies
  - Integration with existing documentation

---

**Note**: Documentation is a living resource. Keep it updated as the project evolves!
