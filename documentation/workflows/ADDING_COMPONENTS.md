# Adding and Modifying Components

[← Back to Documentation](../README.md) | [Workflows Overview](../README.md#workflows)

This guide walks through the complete workflow for adding a new component to the SteamRot game engine. Components are pure data containers that inherit from the `Component` struct and contain no logic.

**Related Documentation:**
- [Testing Overview](../testing/TESTING_OVERVIEW.md) - Test-driven development
- [Style Guide](../style/STYLE_GUIDE.md) - Code conventions
- [Examples](../examples/) - Code examples

---

### Adding/Modifying Components

This workflow guides you through adding a new component to the game engine. 
Components are pure data containers that inherit from the `Component` struct and
are designed to hold no logic.

#### Step 1: Creating the Component Struct

1. Create header and source files in `src/components/` directory:
   - Header file: `CNewComponent.h`
   - Source file: `CNewComponent.cpp`

2. The component struct should:
   - Inherit from `Component` struct
   - Have a `C` prefix (e.g., `CNewComponent`)
   - Be default-constructible (provide default values for all member variables)
   - Contain only data members (no logic/methods except `GetComponentRegisterIndex()`)
   - Use `m_` prefix for member variables

**Example Component Header (`src/components/CNewComponent.h`):**

```cpp
////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of the CNewComponent class.
////////////////////////////////////////////////////////////

#pragma once

#include "Component.h"
#include <string>
#include <vector>

namespace steamrot {

struct CNewComponent : public Component {
  CNewComponent() = default;

  ////////////////////////////////////////////////////////////
  /// @brief Description of the data member
  ////////////////////////////////////////////////////////////
  std::string m_component_name{"default_name"};

  ////////////////////////////////////////////////////////////
  /// @brief Another data member example
  ////////////////////////////////////////////////////////////
  int m_value{0};

  ////////////////////////////////////////////////////////////
  /// @brief Get the position of the Component in the Component Register.
  ///
  /// @return index of the component in the component register
  ////////////////////////////////////////////////////////////
  size_t GetComponentRegisterIndex() const override;
};
} // namespace steamrot
```

**Example Component Source (`src/components/CNewComponent.cpp`):**

```cpp
////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CNewComponent class.
////////////////////////////////////////////////////////////

#include "CNewComponent.h"
#include "containers.h"

namespace steamrot {

////////////////////////////////////////////////////////////
size_t CNewComponent::GetComponentRegisterIndex() const {
  return TupleTypeIndex<CNewComponent, ComponentRegister>;
}

} // namespace steamrot
```

#### Step 2: Register the Component

Add the new component to the `ComponentRegister` tuple in `src/components/containers.h`:

```cpp
typedef std::tuple<CMeta, CUserInterface, CMachinaForm, CGrimoireMachina, CNewComponent>
    ComponentRegister;
```

**Important:** The order matters for indexing, so add new components at the end.

Also add the component header to the includes section:

```cpp
#include "CNewComponent.h"
```

#### Step 3: Create FlatBuffers Schema

1. Create a new `.fbs` file in `src/flatbuffers_headers/` directory:
   - File name: `new_component.fbs`
   - Table name: `NewComponentData` (add `Data` suffix)

**Example Schema (`src/flatbuffers_headers/new_component.fbs`):**

```fbs
namespace steamrot;

table NewComponentData {
  component_name: string;
  value: int;
}
```

2. Add the schema to `src/flatbuffers_headers/generate_flatbuffers_headers.cmake`:

```cmake
set(schema_files
    ${CMAKE_CURRENT_SOURCE_DIR}/ui_style.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/scenes.fbs
    # ... existing schemas ...
    ${CMAKE_CURRENT_SOURCE_DIR}/new_component.fbs  # Add this line
)
```

3. Include the schema in `src/flatbuffers_headers/entities.fbs`:

```fbs
include "user_interface.fbs";
include "grimoire_machina.fbs";
include "new_component.fbs";  // Add this line
namespace steamrot;

table EntityData{
  index: uint32;
  c_user_interface: UserInterfaceData;
  c_grimoire_machina: GrimoireMachinaData;
  c_new_component: NewComponentData;  // Add this line
}
```

4. Rebuild the project to generate FlatBuffers headers:

```bash
cmake --build --preset Debug
```

This generates `new_component_generated.h` automatically.

#### Step 4: Create ConfigureComponent Method (TDD Approach)

1. **Write Tests First**: Create or update test file in `tests/entity/FlatbuffersConfigurator.test.cpp`

2. Add the overloaded `ConfigureComponent` method to `FlatbuffersConfigurator`:

**In `src/entity/FlatbuffersConfigurator.h`:**

Add the include:
```cpp
#include "CNewComponent.h"
#include "new_component_generated.h"
```

Add the method declaration:
```cpp
private:
  ////////////////////////////////////////////////////////////
  /// @brief Overloaded method for configuring CNewComponent component
  ///
  /// @param component_data Flatbuffers table data for NewComponent
  /// @param component CNewComponent instance to be configured
  ////////////////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(const NewComponentData *component_data,
                     CNewComponent &component);
```

**In `src/entity/FlatbuffersConfigurator.cpp`:**

Add the implementation:
```cpp
////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersConfigurator::ConfigureComponent(
    const NewComponentData *component_data,
    CNewComponent &component) {

  // Configure the underlying Component type first
  auto configure_result =
      ConfigureComponent(static_cast<Component &>(component));

  if (!configure_result.has_value())
    return std::unexpected(configure_result.error());

  // Configure component-specific data with null checks
  // FlatBuffers data is susceptible to segfaults, so validate everything
  if (component_data->component_name())
    component.m_component_name = component_data->component_name()->str();

  // For primitive types, direct assignment is safe
  component.m_value = component_data->value();

  return std::monostate{};
}
```

#### Step 5: Add Configuration Call in Main Configure Method

In `FlatbuffersConfigurator::ConfigureEntitiesFromDefaultData`, add the component
configuration block:

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

#### Step 6: Add Component Tests

If testing the component itself (not just configuration), create a test file:

`tests/components/CNewComponent.test.cpp`:

```cpp
#include "CNewComponent.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CNewComponent is default constructible", "[CNewComponent]") {
  steamrot::CNewComponent component;
  REQUIRE(component.m_active == false);
  REQUIRE(component.m_component_name == "default_name");
  REQUIRE(component.m_value == 0);
}
```

Add the test to `tests/components/CMakeLists.txt`:

```cmake
add_executable(test_components
  CGrimoireMachina.test.cpp
  CNewComponent.test.cpp  # Add this line
)
```

#### Step 7: Build and Test

```bash
# Configure the project (if not already done)
cmake --preset Debug

# Build the project
cmake --build --preset Debug

# Run tests
ctest --preset Debug
```

#### Important Notes

- **Null Checks**: Always wrap FlatBuffers field access in `if` statements to
  avoid segfaults. FlatBuffers schema doesn't allow all data types to be required.
- **Base Configuration**: Always call the base `ConfigureComponent(Component&)`
  method first to configure the underlying Component data (e.g., `m_active`).
- **Default Construction**: All components must be default-constructible for the
  memory pool to work correctly.
- **TDD**: Write tests before implementing configuration logic to ensure correctness.
- **Member Prefix**: Use `m_` prefix for all member variables.
- **Component Prefix**: Use `C` prefix for all component class names.
