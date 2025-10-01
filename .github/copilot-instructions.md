# GitHub Copilot Instructions for SteamRot

This document provides guidelines for GitHub Copilot agents working on the SteamRot game engine repository.

## Project Overview

SteamRot is a C++ game engine built using:
- **Language**: C++23 (minimum required)
- **Build System**: CMake (minimum version 3.31)
- **Graphics**: SFML
- **Testing**: Catch2
- **Data Serialization**: FlatBuffers
- **Logging**: spdlog

## Code Style Guide

### General Style

Follow the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) with specific formatting conventions inspired by the [SFML Repository](https://github.com/SFML/SFML/tree/master).

### Formatting Conventions

#### File Structure
- Use `#pragma once` for header guards
- Organize headers with preprocessor directives section first
- Separate sections with visual dividers

#### Spacing Between Functions
Use the following divider between function definitions:
```cpp
////////////////////////////////////////////////////////////
```

#### Function Documentation
Use Doxygen-style comments with visual dividers:
```cpp
////////////////////////////////////////////////////////////
/// @brief Brief description of the function
///
/// @param param_name Description of parameter
/// @return Description of return value
////////////////////////////////////////////////////////////
```

#### Indentation
- Use **2 spaces** for indentation (not tabs)
- This is enforced in `.editorconfig`

#### Naming Conventions
- Member variables: `m_variable_name` (prefix with `m_`)
- Classes: `PascalCase`
- Functions: `PascalCase` for public methods
- Namespaces: `lowercase`
- Components: Prefix with `C` (e.g., `CUserInterface`, `CGrimoireMachina`)

#### Comments
- Prefer Doxygen-style documentation comments (`///`)
- Use `@brief`, `@param`, `@return` tags
- Add comments for complex logic, not obvious code

## Project Structure

### Directory Layout

The project follows the [Pitchfork](https://github.com/vector-of-bool/pitchfork) layout:

- `src/` - Source code
  - `components/` - Pure data container structs (no logic)
  - `data_handlers/` - File I/O and data loading
  - `entity/` - Entity management and configurators
  - `events/` - Event handling system
  - `logger/` - Logging utilities
  - `scenes/` - Scene management
  - `systems/` - Game systems (movement, rendering, etc.)
  - `logic/` - Game logic classes
  - `user_interface/` - UI element definitions
  - `flatbuffers_headers/` - Generated FlatBuffers headers

- `data/` - Non-code data (images, JSON, schemas)
  - Organize by purpose, not file type (e.g., `scene/` not `json/`)

- `tests/` - Test files (mirrors `src/` structure)

- `docs/` - Documentation (Doxygen)

- `cmake/` - CMake modules and scripts

### Key Classes and Patterns

#### PathProvider
- Provides absolute paths to data files
- Configured via CMake with source directory
- `EnvironmentType` is set once per runtime

#### Components
- Pure data containers with no logic
- Inherit from `Component` struct
- Must be default-constructible
- Register in `ComponentRegister` tuple

#### FlatBuffers Data
- Each component has a FlatBuffers schema (`.fbs` file)
- Naming: Add `Data` suffix (e.g., `NewComponentData`)
- Include in `entities.fbs` and `EntityData` table

## Error Handling

### Exception Handling
- Let exceptions propagate up the stack
- **Do not** use try/catch except at the top game loop level
- Top-level catch displays error screen with actionable information

### Expected Pattern
- Use `std::expected` for operations that may fail at runtime
- Return `std::expected<std::monostate, FailInfo>` for operations without return values
- Use `ErrorHandler` namespace to process errors by severity
- Critical errors throw exceptions

### Validation
- Add extensive null checks for FlatBuffers data (susceptible to segfaults)
- Use if statements to validate all optional fields before access

## Development Workflows

### Adding/Modifying Components

Follow these steps to add a new component to the game engine:

#### 1. Create Component Struct (`src/components/`)

**Files to create:**
- `src/components/CNewComponent.h` (header)
- `src/components/CNewComponent.cpp` (source)

**Component Requirements:**
- Inherit from `Component` struct
- Pure data container (no logic, only data members + `GetComponentRegisterIndex()`)
- Must be default-constructible (initialize all members)
- Use `C` prefix for class name (e.g., `CNewComponent`)
- Use `m_` prefix for member variables

**Example Header:**
```cpp
////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of the CNewComponent class.
////////////////////////////////////////////////////////////

#pragma once

#include "Component.h"
#include <string>

namespace steamrot {

struct CNewComponent : public Component {
  CNewComponent() = default;

  ////////////////////////////////////////////////////////////
  /// @brief Description of data member
  ////////////////////////////////////////////////////////////
  std::string m_data_field{"default_value"};
  
  int m_numeric_value{0};

  ////////////////////////////////////////////////////////////
  /// @brief Get the position of the Component in the Component Register.
  ///
  /// @return index of the component in the component register
  ////////////////////////////////////////////////////////////
  size_t GetComponentRegisterIndex() const override;
};
} // namespace steamrot
```

**Example Source:**
```cpp
#include "CNewComponent.h"
#include "containers.h"

namespace steamrot {

////////////////////////////////////////////////////////////
size_t CNewComponent::GetComponentRegisterIndex() const {
  return TupleTypeIndex<CNewComponent, ComponentRegister>;
}

} // namespace steamrot
```

#### 2. Register Component

**In `src/components/containers.h`:**

Add include:
```cpp
#include "CNewComponent.h"
```

Add to `ComponentRegister` tuple (at the end):
```cpp
typedef std::tuple<CMeta, CUserInterface, CMachinaForm, CGrimoireMachina, CNewComponent>
    ComponentRegister;
```

#### 3. Create FlatBuffers Schema (`.fbs` file)

**Create `src/flatbuffers_headers/new_component.fbs`:**
```fbs
namespace steamrot;

table NewComponentData {
  data_field: string;
  numeric_value: int;
}
```

**Add to `src/flatbuffers_headers/generate_flatbuffers_headers.cmake`:**
```cmake
set(schema_files
    # ... existing files ...
    ${CMAKE_CURRENT_SOURCE_DIR}/new_component.fbs
)
```

**Include in `src/flatbuffers_headers/entities.fbs`:**
```fbs
include "user_interface.fbs";
include "grimoire_machina.fbs";
include "new_component.fbs";  // Add this
namespace steamrot;

table EntityData{
  index: uint32;
  c_user_interface: UserInterfaceData;
  c_grimoire_machina: GrimoireMachinaData;
  c_new_component: NewComponentData;  // Add this
}
```

**Build to generate headers:**
```bash
cmake --build build
```

#### 4. Create ConfigureComponent Method (TDD approach)

**In `src/entity/FlatbuffersConfigurator.h`:**

Add includes:
```cpp
#include "CNewComponent.h"
#include "new_component_generated.h"
```

Add method declaration in private section:
```cpp
////////////////////////////////////////////////////////////
/// @brief Overloaded method for configuring CNewComponent
///
/// @param component_data Flatbuffers table data for NewComponent
/// @param component CNewComponent instance to be configured
////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureComponent(const NewComponentData *component_data,
                   CNewComponent &component);
```

**In `src/entity/FlatbuffersConfigurator.cpp`:**

Add implementation:
```cpp
////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersConfigurator::ConfigureComponent(
    const NewComponentData *component_data,
    CNewComponent &component) {

  // Configure base Component first
  auto configure_result =
      ConfigureComponent(static_cast<Component &>(component));

  if (!configure_result.has_value())
    return std::unexpected(configure_result.error());

  // Configure specific data with null checks
  if (component_data->data_field())
    component.m_data_field = component_data->data_field()->str();

  component.m_numeric_value = component_data->numeric_value();

  return std::monostate{};
}
```

#### 5. Add Configuration Call

**In `FlatbuffersConfigurator::ConfigureEntitiesFromDefaultData`:**

Add this block with other component configurations:
```cpp
// CNewComponent component configuration
if (entity_data->c_new_component()) {
  auto configure_result = ConfigureComponent(
      entity_data->c_new_component(),
      emp_helpers::GetComponent<CNewComponent>(i, entity_memory_pool));

  if (!configure_result.has_value())
    return std::unexpected(configure_result.error());
}
```

#### 6. Write Tests

**Create `tests/components/CNewComponent.test.cpp` (if testing component itself):**
```cpp
#include "CNewComponent.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CNewComponent is default constructible", "[CNewComponent]") {
  steamrot::CNewComponent component;
  REQUIRE(component.m_active == false);
  REQUIRE(component.m_data_field == "default_value");
}
```

**Add to `tests/components/CMakeLists.txt`:**
```cmake
add_executable(test_components
  CGrimoireMachina.test.cpp
  CNewComponent.test.cpp  # Add this
)
```

**Update `tests/entity/FlatbuffersConfigurator.test.cpp`** for configuration testing

#### 7. Build and Test

```bash
cmake --build build
cd build && ctest
```

#### Critical Points

- **Always** wrap FlatBuffers field access in `if` statements (strings, vectors, tables)
- **Always** call base `ConfigureComponent(Component&)` first
- **Always** use default initialization for all member variables
- **Follow TDD**: Write tests before implementing configuration
- **Primitive types** (int, bool, float) can be accessed directly from FlatBuffers
- **Complex types** (strings, vectors, tables) must be null-checked

### Adding UI Elements

1. **Create UIElement Class** (`src/user_interface/`)
   - Inherit from `UIElement`
   - Pure data container (no methods)
   - Virtual destructor for polymorphism
   - Include common data (position, size, visibility)

2. **Add Style and Drawing Methods**
   - Create style configuration
   - Implement drawing method

3. **Add Test Support**
   - Add static method to `TestUIElementDataProvider.h`
   - Add test function to `ui_element_factory_helpers.h`
   - Add template specialization to `ui_element_factory_helpers.cpp`

### Adding Logic Classes

1. **Define Logic Class** (TDD approach)
   - Inherit from `Logic` abstract class
   - Implement `PerformLogic()` method
   - Takes `EntityMemoryPool` unique pointer
   - Takes container of entity indices

2. **Update LogicFactory**
   - Write tests in `LogicFactory.test.cpp` first
   - Test order and number of Logic classes per scene
   - Add to relevant `LogicFactory` switch statement

### Adding Actions

1. **Check Action Enum**
   - Verify if action exists in `Actions` enum
   - If not, add to enum and recompile

2. **Create JSON Mapping**
   - Add to scene JSON file
   - Format:
     ```json
     {
       "actions": [
         {
           "name": "action_name",
           "inputs": [
             { "type": "keyboard/mouse", "value": "key_name/mouse_name" }
           ]
         }
       ]
     }
     ```

## Testing Guidelines

### Test-Driven Development (TDD)
- Write tests **before** implementing features
- Especially important for:
  - Component configuration
  - Logic classes
  - LogicFactory updates

### Test Organization
- Mirror `src/` structure in `tests/`
- Use Catch2 framework
- Test file naming: `*.test.cpp`

### Running Tests
```bash
cd build/Debug  # or your build directory
ctest
```

## Build System

### CMake Configuration
- Minimum CMake version: 3.31
- C++23 standard required
- Enable testing with `enable_testing()`

### Build Commands
```bash
cmake -B build -S .
cmake --build build
```

### Key CMake Variables
- `data_dir`: `${CMAKE_SOURCE_DIR}/data`
- `test_data_dir`: `${CMAKE_SOURCE_DIR}/tests/data`

## When Updating README

### Add New Workflows
When adding new features or workflows, update `README.md`:

1. **Add to Table of Contents**
   - Update the `<!--toc:start-->` section
   - Maintain consistent hierarchy

2. **Document in Workflows Section**
   - Add subsection under `## Workflows`
   - Include step-by-step instructions
   - Reference related sections
   - Provide code examples where helpful

3. **Update Relevant Sections**
   - `## src structure` - for new directories
   - `## Classes` - for new class types
   - `## Testing` - for new test patterns

### Formatting README Updates
- Use clear section headers
- Provide code examples in fenced code blocks
- Use bullet points for steps
- Link to external resources (Google Style Guide, Pitchfork, etc.)
- Keep consistent with existing documentation style

## Common Patterns

### Member Variable Initialization
```cpp
class MyClass {
private:
  int m_variable_name{0};  // Initialize with braces
  std::string m_name{};    // Default initialization
};
```

### FlatBuffers Null Checking
```cpp
if (flatbuffer_data) {
  if (flatbuffer_data->field()) {
    // Safe to use field
    auto value = flatbuffer_data->field();
  }
}
```

### Expected Pattern
```cpp
std::expected<std::monostate, FailInfo> 
ConfigureComponent(const Data* data, Component& component) {
  if (!data) {
    return std::unexpected(FailInfo{"Data is null"});
  }
  
  // Configuration logic
  
  return std::monostate{};
}
```

### Visual Dividers
```cpp
////////////////////////////////////////////////////////////
// Preprocessor directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "header.h"

////////////////////////////////////////////////////////////
/// @brief Function documentation
////////////////////////////////////////////////////////////
void Function() {
  // Implementation
}

////////////////////////////////////////////////////////////
```

## Dependencies and Libraries

### Core Dependencies
- SFML (graphics, window, system)
- FlatBuffers (serialization)
- Catch2 (testing)
- spdlog (logging)

### Adding New Dependencies
- Use `FetchContent` or find_package in CMake
- Document in CMakeLists.txt
- Update relevant README sections

## Best Practices

1. **Minimal Changes**: Make the smallest possible changes to achieve the goal
2. **Pure Data Containers**: Components and UIElements should have no logic
3. **TDD**: Write tests before implementation, especially for configurators
4. **Null Safety**: Always validate FlatBuffers data before access
5. **Error Handling**: Use std::expected for runtime failures, exceptions for critical errors
6. **Documentation**: Update README when adding new workflows or patterns
7. **Consistency**: Follow existing patterns in the codebase
8. **Default Construction**: Ensure all components can be default-constructed

## Common Gotchas

1. **FlatBuffers Segfaults**: Always check for null before accessing FlatBuffers fields
2. **Component Registration**: Don't forget to register new components in ComponentRegister
3. **PathProvider**: Set EnvironmentType only once per runtime
4. **Indentation**: Use 2 spaces, not tabs
5. **Member Prefix**: Remember `m_` prefix for member variables
6. **Component Prefix**: Use `C` prefix for component classes

## Resources

- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [SFML Repository](https://github.com/SFML/SFML/tree/master)
- [Pitchfork Layout](https://github.com/vector-of-bool/pitchfork)
- [Pitchfork Spec](https://github.com/vector-of-bool/pitchfork/blob/develop/data/spec.bs)
- Main README.md in repository root

## Agent-Specific Guidelines

When working as a GitHub Copilot agent:

1. **Always read README.md first** to understand current project state
2. **Check existing tests** before writing new code
3. **Follow TDD** when adding configurators or logic classes
4. **Validate FlatBuffers data** with extensive null checks
5. **Update README** when adding new workflows or significant features
6. **Maintain consistency** with existing code patterns
7. **Use visual dividers** (`////////////////////////////////////////////////////////////`) as shown in existing code
8. **Document with Doxygen** style comments
9. **Test incrementally** - build and test after each change
10. **Keep changes minimal** - don't refactor unless necessary
