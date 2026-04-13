# SteamRot Game Engine

A C++ game engine built with SFML, FlatBuffers, and modern C++23 features.

## Overview

SteamRot is a data-driven game engine that emphasizes:
- **Entity-Component System**: Pure data components with separate logic systems
- **FlatBuffers Integration**: Data serialization and configuration
- **Test-Driven Development**: Comprehensive test infrastructure
- **Modern C++**: C++23 features with CMake 3.31+

## Quick Start

### Prerequisites

- CMake 3.31+
- C++23 compatible compiler
- SFML
- FlatBuffers
- Catch2 (for testing)
- spdlog

### Building

```bash
# Configure
cmake --preset Debug

# Build
cmake --build --preset Debug

# Run tests
ctest --preset Debug
```

## Project Structure

```
SteamRot/
├── src/              # Source code
│   ├── components/   # Pure data container structs
│   ├── logic/        # Game systems (movement, rendering, etc.)
│   ├── entity/       # Entity management
│   ├── scenes/       # Scene management
│   └── ...
├── data/             # Game data (images, JSON, schemas)
├── tests/            # Test files (mirrors src/ structure)
├── documentation/    # Comprehensive documentation
└── docs/             # Generated Doxygen documentation
```

### Key Directories

- **`src/`** - Application source code following [Pitchfork layout](https://github.com/vector-of-bool/pitchfork)
- **`data/`** - Non-code data organized by purpose (e.g., `scene/` not `json/`)
- **`tests/`** - Unit, integration, and system tests
- **`documentation/`** - Guides, workflows, and references
- **`cmake/`** - CMake modules and scripts

## Architecture Overview

### Core Systems

- **PathProvider**: Provides absolute paths to data files, configured via CMake
- **Entity-Component System**: Components are pure data; Logic classes handle behavior
- **Logic System**: Scene-specific logic organized by type (Action, Render, Collision, Movement)
- **Event System**: Event-driven communication between systems
- **FlatBuffers**: Data serialization for entities, scenes, and configuration

### Design Principles

1. **Data-Driven**: Configuration through JSON/FlatBuffers
2. **Separation of Concerns**: Components contain data, Logic contains behavior
3. **TDD**: Write tests before implementation
4. **Fail Fast**: Let exceptions propagate, handle gracefully at top level
5. **Type Safety**: Use `std::expected` for runtime errors

## Wiki

The SteamRot wiki is a searchable, browsable site built from the `documentation/` directory
using [MkDocs Material](https://squidfunk.github.io/mkdocs-material/).

**Live wiki:** https://walliscode.github.io/SteamRot/

**Serve locally:**

```bash
# Activate the virtual environment (once per shell session)
source .venv/bin/activate   # macOS/Linux
# .venv\Scripts\activate    # Windows

# Install dependencies (once)
pip install -r tools/wiki/requirements.txt

# Start live-reloading local server
mkdocs serve
# Open http://127.0.0.1:8000
```

**Add a new page:** drop a `.md` file into `documentation/` and add it to the `nav`
section in `mkdocs.yml`. Mermaid diagrams render automatically in fenced ` ```mermaid ` blocks.

The wiki is rebuilt and published to GitHub Pages automatically on every push to `main`
that touches `documentation/`, `mkdocs.yml`, or `tools/wiki/requirements.txt`.

---

## Documentation

The `documentation/` directory contains comprehensive guides organized by topic:

### 📖 For New Developers
- **[Getting Started Guide](documentation/README.md)** - Start here!

### 🔧 Workflows (How-To Guides)
- **[Adding Components](documentation/workflows/ADDING_COMPONENTS.md)** - Create new component types
- **[Adding Logic](documentation/workflows/ADDING_LOGIC.md)** - Implement game systems
- **[Adding UI Elements](documentation/workflows/ADDING_UI_ELEMENTS.md)** - Create UI components
- **[Adding Actions](documentation/workflows/ADDING_ACTIONS.md)** - Handle user input
- **[Filling Out Test Data](documentation/workflows/FILLING_TEST_DATA.md)** - Create test_data.json files

### 🏗️ Architecture
- **[Error Handling](documentation/architecture/ERROR_HANDLING.md)** - Exception and error patterns
- **[Game Loop](documentation/architecture/GAME_LOOP.md)** - Main game loop structure
- **[Logic System](documentation/architecture/LOGIC_SYSTEM.md)** - Logic architecture

### ⚙️ Configuration
- **[Context Configuration](documentation/configuration/CONTEXT_CONFIGURATION.md)** - Game and scene contexts
- **[Resource Configuration](documentation/configuration/RESOURCE_CONFIGURATION.md)** - Resource management

### 🧪 Testing
- **[Testing Overview](documentation/testing/TESTING_OVERVIEW.md)** - Test structure and classification
- **[Test Data Configuration](documentation/testing/TEST_DATA_CONFIGURATION.md)** - Data-driven testing

### 📝 Style & Conventions
- **[Code Style Guide](documentation/style/STYLE_GUIDE.md)** - Formatting and conventions
- **[Naming Conventions](documentation/naming/FILE_NAMING_CONVENTIONS.md)** - File naming system
- **[Quick Reference](documentation/naming/NAMING_QUICK_REFERENCE.md)** - Naming lookup tables

### 📚 Examples
- **[Examples Directory](documentation/examples/)** - Code examples and patterns

### 📑 Reference
- **[Dropdown Data](documentation/reference/dropdown_data_population.md)** - Dropdown implementation

## Development Workflow

### Adding a New Feature

1. **Plan**: Review relevant documentation
2. **Test**: Write tests first (TDD approach)
3. **Implement**: Make minimal changes to pass tests
4. **Verify**: Build and run tests
5. **Document**: Update docs if needed

### Common Tasks

**Add a Component:**
```bash
# Follow the guide
documentation/workflows/ADDING_COMPONENTS.md
```

**Add Logic:**
```bash
# Follow the guide
documentation/workflows/ADDING_LOGIC.md
```

**Run Tests:**
```bash
# All tests
ctest --preset Debug

# Specific test type
ctest --preset Debug -L unit
ctest --preset Debug -L integration
```

## Key Concepts

### Components
- Pure data containers inheriting from `Component`
- No logic or methods (except `GetComponentRegisterIndex()`)
- Registered in `ComponentRegister` tuple
- Configured via FlatBuffers

### Logic Classes
- Inherit from `Logic` abstract class
- Process entities by archetype
- Organized by `LogicType` (Action, Render, Collision, Movement)
- Created by `LogicFactory` per scene

### Archetypes
- Entities grouped by active components
- Efficient iteration over entities with specific component combinations
- Managed by `ArchetypeManager`

### FlatBuffers
- Schema-driven data serialization
- Compiled from `.fbs` files to generated headers
- Null safety: Always validate optional fields

## Code Style

SteamRot follows the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) with specific conventions:

- **Classes**: `PascalCase`
- **Functions**: `PascalCase` (e.g., `GetComponent`, `ProcessLogic`)
- **Components**: Prefix with `C` (e.g., `CUserInterface`)
- **Member Variables**: Prefix with `m_` (e.g., `m_value`)
- **Indentation**: 2 spaces (not tabs)
- **Headers**: Use `#pragma once`
- **Documentation**: Doxygen-style comments

See [Style Guide](documentation/style/STYLE_GUIDE.md) for complete details.

## Testing

SteamRot uses Catch2 with a clear test taxonomy:

- **`[unit]`** - Unit tests (single class, mocked dependencies)
- **`[integration]`** - Integration tests (multiple components)
- **`[system]`** - End-to-end tests (full system)
- **`[.visual]`** - Visual confirmation tests (hidden by default)

See [Testing Overview](documentation/testing/TESTING_OVERVIEW.md) for details.

## Contributing

1. Follow the documented workflows
2. Write tests first (TDD)
3. Adhere to code style guide
4. Update documentation for new features
5. Keep changes minimal and focused

## Resources

- **Documentation**: [documentation/](documentation/)
- **Examples**: [documentation/examples/](documentation/examples/)
- **Pitchfork Layout**: [GitHub](https://github.com/vector-of-bool/pitchfork)
- **Google C++ Style**: [Guide](https://google.github.io/styleguide/cppguide.html)

## License

[License information here]

## Contact

[Contact information here]
