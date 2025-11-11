# GitHub Copilot Instructions for SteamRot

This document provides guidelines for GitHub Copilot agents working on the SteamRot game engine repository.

## ⚠️ IMPORTANT: No Build or Test Steps for Agents

**Agents should NEVER attempt to build, test, or lint this repository.**

All building, testing, and linting is done locally by the user. Your role is to:
- Make code changes only
- Write test files (but not run them)
- Focus on code implementation
- Trust that the user will build and test your changes locally

**Do NOT:**
- Run `cmake` commands
- Run build commands
- Run test commands (`ctest`, individual test executables)
- Run linters
- Attempt to verify that code compiles
- Check if tests pass

The testing and build infrastructure documentation in this file is for **reference only** to help you understand the project structure and write appropriate code and tests.

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

**Variables and Code Elements:**
- Member variables: `m_variable_name` (prefix with `m_`)
- Classes: `PascalCase`
- Functions: `PascalCase` for public methods
- Namespaces: `lowercase`
- Components: Prefix with `C` (e.g., `CUserInterface`, `CGrimoireMachina`)

**File Naming:**
See [FILE_NAMING_CONVENTIONS.md](../documentation/naming/FILE_NAMING_CONVENTIONS.md) for complete details.

Quick reference:
- **Classes**: `ClassName.h/cpp` (PascalCase) → `steamrot::ClassName`
- **Free Functions**: `subsystem_category.h/cpp` (snake_case) → `steamrot::subsystem::category`
- **Template Utils**: `SubsystemUtils.h` (PascalCase, header-only) → `steamrot::subsystem`
- **Test Helpers**: `subsystem_test_helpers.h/cpp` (snake_case) → `steamrot::tests::subsystem`
- **Test Files**: `ModuleName.test.cpp` (matches module being tested)

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

**Note:** Building is done locally by the user. FlatBuffers headers are auto-generated during the build process by the user.

#### 4. Create ConfigureComponent Method

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

#### 7. Completion

**Note:** Building and testing are done locally by the user. Agents should focus on code changes and writing test files. Tests will be run and verified locally.

#### Critical Points

- **Always** wrap FlatBuffers field access in `if` statements (strings, vectors, tables)
- **Always** call base `ConfigureComponent(Component&)` first
- **Always** use default initialization for all member variables
- **Write tests** for new functionality (but don't run them - user will test locally)
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

Logic classes implement game system behaviors (collision, rendering, actions, movement).

**Complete Examples**: See `documentation/examples/` for full example files (ExampleLogic.h, ExampleLogic.cpp, ExampleLogic.test.cpp) demonstrating all patterns.

#### Quick Reference Steps

1. **Create Logic Class** (`src/logic/NewLogic.h` and `.cpp`)
   - Inherit from `Logic` abstract class
   - Override `ProcessLogic()` private method
   - Constructor takes `const LogicContext` parameter

2. **Write Unit Tests** (`tests/logic/NewLogic.test.cpp`)
   - Test construction
   - Test logic execution with various states
   - Add to `tests/logic/CMakeLists.txt`

3. **Update LogicFactory Tests** (`tests/logic/LogicFactory.test.cpp`)
   - Add test case for scene using new Logic
   - Verify order and count in LogicCollection

4. **Update LogicFactory** (`src/logic/LogicFactory.cpp`)
   - Add include for new Logic class
   - Add to appropriate Create method (Render, Collision, Action)
   - Add to correct scene case in switch statement

5. **Update Test Helpers** (`tests/logic/logic_helpers.cpp`)
   - Add include for new Logic class
   - Update `CheckStaticLogicCollections()` for affected scenes
   - Verify count, order, and types with `dynamic_cast`

6. **Completion**
   - Building and testing done locally by the user
   - Focus on code implementation and test file writing
   - Tests will be run and verified locally by the user

#### Detailed Workflow

Follow these steps to create a new derived Logic class:

##### 1. Create Logic Header File

**File: `src/logic/NewLogic.h`**

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the NewLogic class.
/////////////////////////////////////////////////

#pragma once

#include "Logic.h"

namespace steamrot {

class NewLogic : public Logic {

private:
  /////////////////////////////////////////////////
  /// @brief Override method to encapsulate all logic for NewLogic
  /////////////////////////////////////////////////
  void ProcessLogic() override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for NewLogic.
  ///
  /// @param logic_context LogicContext object containing the context for the logic.
  /////////////////////////////////////////////////
  NewLogic(const LogicContext logic_context);
};

} // namespace steamrot
```

##### 2. Implement Logic Class

**File: `src/logic/NewLogic.cpp`**

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the NewLogic class.
/////////////////////////////////////////////////

#include "NewLogic.h"
#include "ArchetypeHelpers.h"
#include "CYourComponent.h"
#include "emp_helpers.h"

namespace steamrot {

/////////////////////////////////////////////////
NewLogic::NewLogic(const LogicContext logic_context)
    : Logic(logic_context) {}

/////////////////////////////////////////////////
void NewLogic::ProcessLogic() {

  // Generate archetype ID for needed components
  ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<CYourComponent>();

  const auto it = m_logic_context.archetypes.find(archetype_id);
  
  if (it != m_logic_context.archetypes.end()) {
    const Archetype &archetype = it->second;

    for (size_t entity_id : archetype) {
      CYourComponent &component = emp_helpers::GetComponent<CYourComponent>(
          entity_id, m_logic_context.scene_entities);

      // Perform logic operations
      // Access m_logic_context members as needed:
      // - scene_texture, game_window, asset_manager
      // - event_handler, mouse_position, etc.
    }
  }
}

} // namespace steamrot
```

##### 3. Write Unit Tests

**File: `tests/logic/NewLogic.test.cpp`**

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for NewLogic class
/////////////////////////////////////////////////

#include "NewLogic.h"
#include "TestContext.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("NewLogic::NewLogic Constructor", "[NewLogic]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;
  
  steamrot::NewLogic new_logic(
      test_context.GetLogicContextForTestScene());
  SUCCEED("NewLogic instantiated successfully");
}

TEST_CASE("NewLogic::ProcessLogic performs expected logic", "[NewLogic]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;
  
  auto logic_context = test_context.GetLogicContextForTestScene();
  
  // Set up test entities and components
  // ...
  
  steamrot::NewLogic new_logic(logic_context);
  new_logic.RunLogic();
  
  // Assert expected state changes
  // REQUIRE(...);
}
```

**Add to `tests/logic/CMakeLists.txt`:**
```cmake
add_executable(test_logic
  # ... existing tests ...
  NewLogic.test.cpp
)
```

##### 4. Update LogicFactory Tests

**In `tests/logic/LogicFactory.test.cpp`:**

```cpp
TEST_CASE("LogicFactory creates correct Logic instances for YourScene",
          "[LogicFactory]") {

  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context{
      steamrot::SceneType::SceneType_YOUR_SCENE};
  
  steamrot::LogicFactory logic_factory(
      steamrot::SceneType::SceneType_YOUR_SCENE,
      test_context.GetLogicContextForYourScene());

  auto logic_map_result = logic_factory.CreateLogicMap();
  REQUIRE(logic_map_result.has_value());

  steamrot::tests::CheckStaticLogicCollections(
      logic_map_result.value(), 
      steamrot::SceneType::SceneType_YOUR_SCENE);
}
```

##### 5. Update LogicFactory Implementation

**In `src/logic/LogicFactory.cpp`:**

Add include:
```cpp
#include "NewLogic.h"
```

Add to appropriate Create method:
```cpp
std::expected<LogicVector, FailInfo> LogicFactory::CreateActionLogics() {
  LogicVector action_logics;

  switch (m_scene_type) {
  case SceneType::SceneType_YOUR_SCENE: {
    action_logics.push_back(std::make_unique<NewLogic>(m_logic_context));
    break;
  }
  // ... other cases ...
  default:
    return std::unexpected(FailInfo{FailMode::NonExistentEnumValue,
                                    "Unsupported scene type"});
  }
  return action_logics;
}
```

**Note:** Order matters - Logic classes execute in vector order.

##### 6. Update Test Helper Functions

**In `tests/logic/logic_helpers.cpp`:**

Add include:
```cpp
#include "NewLogic.h"
```

Update `CheckStaticLogicCollections()`:
```cpp
void CheckStaticLogicCollections(const LogicCollection &collection,
                                 const SceneType &scene_type) {
  switch (scene_type) {
  case SceneType::SceneType_YOUR_SCENE: {
    REQUIRE(collection.size() == 3);
    REQUIRE(collection.find(LogicType::Action) != collection.end());
    
    const LogicVector &action_logics = collection.at(LogicType::Action);
    REQUIRE(action_logics.size() == 2);  // Updated count
    REQUIRE(dynamic_cast<NewLogic *>(action_logics[0].get()));
    REQUIRE(dynamic_cast<UIActionLogic *>(action_logics[1].get()));
    
    // ... check other LogicTypes ...
    break;
  }
  // ... other cases ...
  }
}
```

#### Logic Class Patterns

**Archetype-Based Processing:**
```cpp
ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<CComponent1, CComponent2>();
const auto it = m_logic_context.archetypes.find(archetype_id);
if (it != m_logic_context.archetypes.end()) {
  // Process entities
}
```

**Component Access:**
```cpp
CComponent &comp = emp_helpers::GetComponent<CComponent>(
    entity_id, m_logic_context.scene_entities);
```

**Rendering Logic:**
```cpp
m_logic_context.scene_texture.draw(drawable);
```

**Event Handling:**
```cpp
m_logic_context.event_handler.AddEvent(event_packet);
```

#### LogicType Categories

- **Collision**: Spatial interactions (UI collision, physics)
- **Render**: Drawing to render texture (UI, entities)
- **Action**: Input processing and event triggering
- **Movement**: Position updates (player, AI)

#### Testing Best Practices

1. **Unit Tests**: Test Logic in isolation
   - Constructor without errors
   - ProcessLogic() with various states
   - Edge cases (empty archetypes, null checks)

2. **Integration Tests**: Test with LogicFactory
   - Correct Logic creation for scenes
   - Correct order in LogicVector
   - Correct LogicType assignment

3. **Use TestContext**: Provides mock dependencies
   - `GetLogicContextForTestScene()`
   - `GetLogicContextForTitleScene()`
   - `GetLogicContextForCraftingScene()`

4. **Validation Helpers**: Use `CheckStaticLogicCollections()`
   - Verifies count, order, and types
   - Uses `dynamic_cast` for type checking
   - Add case for each scene type

#### Common Issues and Solutions

**Issue**: Logic not executing
- **Check**: Archetype exists in scene
- **Check**: Entities have required components
- **Check**: Logic added to correct LogicType

**Issue**: Test failures with "dynamic_cast returned null"
- **Check**: Logic added to LogicFactory for that scene
- **Check**: Include statement in LogicFactory.cpp
- **Check**: Order in vector matches test expectations

**Issue**: Wrong Logic order
- **Remember**: Order in LogicVector = execution order
- **Check**: Test helper validates expected order
- **Update**: Both LogicFactory and test helper together

#### Key Reminders

- **Always** call base constructor: `Logic(logic_context)`
- **Always** check archetype existence before iterating
- **Write tests** for new implementation (but don't run them - user tests locally)
- **Always** update test helpers when changing LogicFactory
- **Never** add logic to component classes (keep them pure data)
- **Never** modify entities outside their archetype's Logic
- Use `m_logic_context` members (don't store duplicates)
- Use visual dividers (`/////////////////////////////////////////////////`)
- Use Doxygen documentation (`///`)
- Use 2-space indentation

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

## Testing Guidelines (Reference Only)

**Note: Agents do NOT run tests. This section is for reference to help you write test files.**

### Test-Driven Development (TDD)
- Write test files when implementing features
- Users will run tests locally
- Important areas for test coverage:
  - Component configuration
  - Logic classes
  - LogicFactory updates

### Test Organization and Classification

**Directory Structure:**
```
tests/
├── unit/                    # Unit tests organized by subsystem
│   ├── assets/
│   ├── components/
│   ├── data_handlers/
│   ├── display/
│   ├── entity/
│   ├── events/
│   ├── logic/
│   ├── scenes/
│   ├── systems/
│   └── user_interface/
├── integration/             # Integration tests (2+ components)
│   └── scene_change/
├── system/                  # End-to-end tests (future)
├── perf/                    # Performance tests (future)
│   └── benchmarks/
├── data/                    # Test data files
└── context/                 # Test utilities
```

**Note**: All unit tests have been migrated to `unit/` directory and organized by subsystem.

**Test Tags:**
All tests must include appropriate Catch2 tags:
- `[unit]` - Unit tests (isolated with mocked dependencies)
- `[integration]` - Integration tests (multiple components)
- `[system]` - System tests (end-to-end, future)
- `[perf]` - Performance tests (benchmarks, future)
- `[.visual]` - Visual confirmation tests (hidden tag - automatically excluded from default runs, require user input/visual verification)

**Example:**
```cpp
TEST_CASE("ClassName constructor", "[unit][ClassName]") {
  // Test implementation
}

TEST_CASE("Feature workflow", "[integration][feature_name]") {
  // Test implementation
}

TEST_CASE("Visual confirmation test", "[unit][ClassName][.visual]") {
  // Test requiring user input or visual confirmation
  // These tests open windows and wait for user interaction
  // Hidden tag [.visual] means excluded by default
}
```

### Running Tests

**Note:** Building is done locally by the user. The following commands are for reference on how tests will be run locally.

**Using CMake Presets (Reference):**
```bash
# Tests will be run locally after code changes

# Run all tests (visual tests automatically excluded via hidden tag)
ctest --preset Debug

# Run only unit tests
ctest --preset Debug -L unit

# Run only integration tests
ctest --preset Debug -L integration

# Run tests from specific subsystem
ctest --preset Debug -R logic

# Run ONLY visual tests (explicit request for manual verification)
ctest --preset Debug --test-args "[.visual]"

# Run all hidden tests
ctest --preset Debug --test-args "[.]"

# Verbose output on failure
ctest --preset Debug --output-on-failure
```

### Test File Naming
- Unit tests: `ClassName.test.cpp` (existing, in subsystem directories)
- Integration tests: `feature_name.integration.test.cpp`
- System tests: `workflow.system.test.cpp` (future)
- Performance tests: `component.perf.test.cpp` (future)

## Build System (Reference Only)

**Note: Agents do NOT build. This section is for reference only.**

### CMake Configuration
- Minimum CMake version: 3.31
- C++23 standard required
- Enable testing with `enable_testing()`
- **Uses CMake Presets** (see `CMakePresets.json`)

### Build Commands (Reference Only - DO NOT USE)

**Agents must NEVER run these commands. This is documentation only.**

```bash
# Example commands (for reference, DO NOT RUN):
# cmake --preset Debug
# cmake --build --preset Debug
# ctest --preset Debug
```

### Available Presets
- **Debug**: Debug build with Ninja generator
- **Release**: Release build with Ninja generator

### Key CMake Variables
- `data_dir`: `${CMAKE_SOURCE_DIR}/data`
- `test_data_dir`: `${CMAKE_SOURCE_DIR}/tests/data`

### Important Notes
- **Agents NEVER build, test, or lint** - All done locally by the user
- CMake presets are used for configuration and building (done locally only)
- Focus on making code changes and writing test files

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
3. **Write Tests**: Create test files for new features (but don't run them)
4. **Null Safety**: Always validate FlatBuffers data before access
5. **Error Handling**: Use std::expected for runtime failures, exceptions for critical errors
6. **Documentation**: Update README when adding new workflows or patterns
7. **Consistency**: Follow existing patterns in the codebase
8. **Default Construction**: Ensure all components can be default-constructed
9. **Reusable Test Infrastructure**: Use TestScenarios, assertions, and mixins for tests
10. **Test Tags**: Always tag tests with `[unit]`, `[integration]`, or `[system]`

## Context Configuration

### Overview

The context configuration system provides data-driven context configuration via FlatBuffers. This allows game and scene settings to be externalized to JSON files.

See `documentation/configuration/CONTEXT_CONFIGURATION.md` for complete documentation.

### Configuration Files

- **Production**: `data/context/context_data.json`
- **Test**: `tests/data/context/test_context_data.json`

### Using ContextConfigurator

```cpp
// Load configuration
steamrot::FlatbuffersDataLoader loader;
auto context_data = loader.ProvideContextData().value();

// Create configurator
steamrot::ContextConfigurator configurator(context_data);

// Get builder with static settings pre-configured
auto builder = configurator.CreateGameContextBuilder().value();

// Add runtime objects
builder.SetWindow(window_ptr)
       .SetEventHandler(handler_ptr)
       .SetAssetManager(assets_ptr)
       .SetLoopNumber(loop_num_ptr);

// Build context
auto context = builder.Build().value();
```

### Configuration Structure

- **GameContextConfig**: Window size, title, framerate, environment type
- **SceneContextConfig**: Entity pool size, render texture dimensions per scene
- **ContextData**: Root table containing game and scene configurations

### Key Features

- **Environment Type Support**: Separate configs for Test/Production
- **Scene-Specific Settings**: Each scene can have different entity pool sizes
- **Automatic Build Integration**: JSON files auto-converted to binary
- **Error Handling**: std::expected with detailed FailInfo messages

### Related Documentation

- `documentation/configuration/CONTEXT_CONFIGURATION.md` - Complete usage guide

## Testing Infrastructure

### Reusable Test Components

SteamRot provides reusable test infrastructure to reduce boilerplate:

#### TestScenarios

Pre-configured entity and component setups:

```cpp
#include "TestScenarios.h"

// Create 10 UI entities
auto pool = steamrot::tests::TestScenarios::CreatePoolWithMultipleUIEntities(10);

// Create archetype manager
auto manager = steamrot::tests::TestScenarios::CreatePopulatedArchetypeManager(pool);

// Activate/deactivate components
steamrot::tests::TestScenarios::ActivateComponent<CMyComponent>(0, pool);
```

Available methods:
- `CreateEmptyPool()` - Empty entity pool
- `CreatePoolWithNEntities(n)` - Pool with n entities
- `CreatePoolWithArchetype(archetype_id, n)` - Entities matching archetype
- `CreatePoolWithSingleUIEntity()` - One UI entity
- `CreatePoolWithMultipleUIEntities(n)` - n UI entities
- `CreatePoolWithSingleGrimoireEntity()` - One grimoire entity
- `CreatePopulatedArchetypeManager(pool)` - Configured manager
- `ActivateComponent<T>(entity_id, pool)` - Activate component
- `DeactivateComponent<T>(entity_id, pool)` - Deactivate component

#### Test Assertions

Domain-specific assertions:

```cpp
#include "test_assertions.h"

steamrot::tests::AssertComponentActive(component, true, "TestName");
steamrot::tests::AssertEntityInArchetype(id, archetype_id, manager, "TestName");
steamrot::tests::AssertArchetypeEntityCount(archetype_id, 5, manager, "TestName");
```

#### ComponentTestMixin

Template for component contract tests:

```cpp
#include "component_test_mixin.h"

TEST_CASE("MyComponent contract", "[unit][MyComponent]") {
  steamrot::tests::ComponentTestMixin<MyComponent>::RunAllTests();
}
```

Validates:
- Default construction
- Component register index
- Activation/deactivation
- Copy construction/assignment

#### LogicTestBase

Base class for Logic tests:

```cpp
#include "logic_test_base.h"

class MyLogicTest : public steamrot::tests::LogicTestBase<MyLogic> {
protected:
  void TestProcessWithSingleEntity() override {
    SetUp();
    // Setup entities...
    logic = CreateLogic();
    REQUIRE_NOTHROW(logic->RunLogic());
    // Verify...
  }
};
```

### Guidelines for Agents

When implementing new features:

1. **Write test files** using reusable infrastructure (but don't run them)
2. **Use TestScenarios** for entity/component setup in test files
3. **Use domain assertions** for clear test intent
4. **Use mixins/base classes** to reduce boilerplate
5. **Tag tests appropriately** with `[unit]`, `[integration]`, or `[system]`
6. **Keep tests independent** - no shared state between tests
7. **Test edge cases** - boundary conditions and errors in test files
8. **Note:** Building and test execution done locally by user - agents only write code and test files

### Example Workflow

**Adding a new Component:**

```cpp
// 1. Write test file
TEST_CASE("NewComponent contract", "[unit][NewComponent]") {
  ComponentTestMixin<NewComponent>::RunAllTests();
}

TEST_CASE("NewComponent defaults", "[unit][NewComponent]") {
  NewComponent c;
  REQUIRE(c.m_value == 0);
}

// 2. Implement minimal component
// 3. Add to ComponentRegister
// 4. Create FlatBuffers schema
// 5. Implement configurator with test files
// Note: Building and testing done locally by user
```

**Adding a new Logic:**

```cpp
// 1. Write test file using LogicTestBase
class NewLogicTest : public LogicTestBase<NewLogic> {
  void TestProcessWithSingleEntity() override {
    // Test implementation
  }
};

// 2. Implement minimal Logic
// 3. Add to LogicFactory with test files
// Note: Building and testing done locally by user
```

## Test Data Configuration System

### Overview

The Test Data Configuration System provides a FlatBuffers-based framework for data-driven testing. Tests can be defined in JSON files and loaded at runtime, enabling easy addition of test cases without code changes.

**Documentation:**
- **Human Workflow Guide**: `documentation/workflows/FILLING_TEST_DATA.md` - Practical guide for creating test_data.json files
- **Technical Reference**: `documentation/testing/TEST_DATA_CONFIGURATION.md` - Complete API and schema reference

**Documentation:**
- **Human Workflow Guide**: `documentation/workflows/FILLING_TEST_DATA.md` - Practical guide for creating test_data.json files
- **Technical Reference**: `documentation/testing/TEST_DATA_CONFIGURATION.md` - Complete API and schema reference

**When helping users create test_data.json files:**
1. Direct them to `documentation/workflows/FILLING_TEST_DATA.md` for workflow-based guidance
2. Reference the specific workflow section that matches their testing scenario:
   - Workflow 1: Metadata-Only Test
   - Workflow 2: Simple Entity Comparison
   - Workflow 3: Entity State Transformation
   - Workflow 4: Simulation with Logic Classes
   - Workflow 5: Simulation with Free Functions
   - Workflow 6: Input Sequence Testing
   - Workflow 7: Event Sequence Testing
   - Workflow 8: Combined Testing (Input + Events + Simulation)
   - Workflow 9: Negative Testing (Expected to Fail)
3. Use the field reference and examples in the workflow guide
4. For technical details about schemas and APIs, reference `TEST_DATA_CONFIGURATION.md`

### EntityMemoryPool Matcher with Test Metadata

The `EntityMemoryPoolEqualsMatcher` supports optional test metadata that is included in failure messages. When using data-driven tests with `run_fixture_test()` or `run_entity_memory_pool_comparison_test()`, test metadata (test name and description) from JSON files is automatically extracted and included in error messages, making it easier to identify which test case failed.

**Example usage:**
```cpp
// Automatic - metadata extracted from config
auto result = steamrot::tests::run_fixture_test(config);

// Manual - pass metadata string
steamrot::tests::run_entity_memory_pool_comparison_test(
    actual_pool, expected_pool, "Test: my_test");
```

### Quick Start

#### 1. Create Test Data JSON

```json
// tests/components/data/my_test.test_data.json
{
  "metadata": {
    "test_name": "my_component_test",
    "description": "Test component configuration",
    "tags": ["unit", "component"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 5,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "test_ui",
          "start_visible": true,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 100, "y": 200 },
              "size": { "x": 50, "y": 30 },
              "children_active": false,
              "children": [],
              "layout": "Horizontal",
              "spacing_strategy": "None"
            }
          }
        }
      }
    ]
  },
  "expected_entity_collection": {
    "entity_memory_pool_size": 5,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "test_ui",
          "start_visible": true,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 100, "y": 200 },
              "size": { "x": 50, "y": 30 },
              "children_active": false,
              "children": [],
              "layout": "Horizontal",
              "spacing_strategy": "None"
            }
          }
        }
      }
    ]
  }
}
```

#### 2. Build Project (done locally)

**Note:** Building is done locally by the user. FlatBuffers compilation from JSON to binary happens during the build process.

#### 3. Use test_data_harness in Tests

```cpp
#include "test_data_harness.h"

TEST_CASE("Load and use test data", "[unit][data-driven]") {
  // Load all test data configs from adjacent data/ directory
  auto configs = steamrot::tests::load_test_data_configs();
  
  REQUIRE(configs.has_value());
  const auto* config = configs.value()[0];  // Get first config
  
  // Access metadata
  REQUIRE(config->metadata()->test_name()->str() == "my_component_test");
  
  // Access entity data
  if (config->start_entity_collection()) {
    // Process starting entities
  }
  if (config->expected_entity_collection()) {
    // Process expected entities
  }
}
```

### File Naming Convention

- **JSON files**: `<test_name>.test_data.json`
- **Compiled binaries**: `<test_name>.test_data.bin` (auto-generated)
- **Location**: `tests/<test_executable_dir>/data/`

### Key Features

- **Extensible Schema**: Add new data types without breaking existing tests
- **Auto-Discovery**: Automatically finds all .test_data.bin files
- **Simple API**: Single function call to load all test configs
- **Type-Safe**: FlatBuffers ensures compile-time type checking
- **Organized**: Subdirectory structure matches test organization

### Example: Data-Driven Testing with Generators

```cpp
TEST_CASE("Run all component test data", "[unit][data-driven]") {
  // Load all test data configs with one call
  auto configs = steamrot::tests::load_test_data_configs("components");
  REQUIRE(configs.has_value());
  
  // Use Catch2 generator to run test for each config
  const auto* config = GENERATE_COPY(from_range(configs.value()));
  
  REQUIRE(config->metadata() != nullptr);
  // Run test with this configuration
}
```

### Adding New Data Types

To extend the schema with new data types:

1. **Create FlatBuffers schema** for new data
2. **Update `test_data.fbs`** with new optional field
3. **Build happens locally** - Headers will be generated during local build
4. **Use in JSON** test data files

Example:

```fbs
// test_data.fbs
table TestDataConfig {
  metadata: TestMetadata (required);
  start_entity_collection: EntityCollection;
  expected_entity_collection: EntityCollection;
  new_data_type: NewDataType;  // Add new field
}
```

### Best Practices

- **Organize by category**: Place test data in matching subdirectories
- **Descriptive names**: Use clear names for test data files
- **Include metadata**: Always fill out test metadata fields
- **Keep focused**: One scenario per test data file
- **Reuse types**: Leverage existing FlatBuffers tables
- **Follow schema**: Use `start_entity_collection` and `expected_entity_collection` as defined in `test_data.fbs`

### test_data_harness Functions

```cpp
// Load all test data configs from adjacent data/ directory
std::expected<std::vector<const TestDataConfig*>, FailInfo>
load_test_data_configs();

// Load test data configs from specific subdirectory
std::expected<std::vector<const TestDataConfig*>, FailInfo>
load_test_data_configs(const std::string& subdirectory);
```

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

1. **NEVER build, test, or lint** - All building, testing, and linting is done locally by the user
2. **Always read README.md first** to understand current project state
3. **Write test files** but do NOT run them - tests will be executed locally
4. **Focus on code implementation only** - trust the user will verify your changes
5. **Validate FlatBuffers data** with extensive null checks in your code
6. **Update README** when adding new workflows or significant features
7. **Maintain consistency** with existing code patterns
8. **Use visual dividers** (`////////////////////////////////////////////////////////////`) as shown in existing code
9. **Document with Doxygen** style comments
10. **Keep changes minimal** - don't refactor unless necessary
