# Dropdown Data Population Feature

## Overview

The dropdown data population system allows dropdown lists to dynamically populate their items based on game data. This is implemented using the `DataPopulateFunction` enum and the `entity_helpers` namespace.

## Components

### 1. DataPopulateFunction Enum

Located in `src/flatbuffers_headers/user_interface.fbs`:

```fbs
enum DataPopulateFunction: byte {
    None = 0,
    PopulateWithFragmentData = 1,
    PopulateWithJointData = 2,
}
```

### 2. entity_helpers Namespace

Located in `src/logic/entity_helpers.h/.cpp`:

Functions for extracting data from game components:
- `GetAllFragmentNames(const CGrimoireMachina &)` - Returns vector of all fragment names
- `GetAllJointNames(const CGrimoireMachina &)` - Returns vector of all joint names

### 3. DropDownListElement Field

The `DropDownListElement` now has a `data_populate_function` field that stores which population function to use.

## Usage

### In FlatBuffers Schema (JSON)

When defining a dropdown list in your UI JSON data, set the `data_populate_function` field:

```json
{
  "element": {
    "type": "DropDownListData",
    "base_data": {
      "position": {"x": 100, "y": 100},
      "size": {"x": 200, "y": 40},
      "children_active": true,
      "children": []
    },
    "label": "Select Fragment",
    "expanded_label": "Fragments",
    "data_populate_function": "PopulateWithFragmentData"
  }
}
```

### Available Functions

- **`None`**: No dynamic population (default)
- **`PopulateWithFragmentData`**: Populates dropdown with all available fragments from CGrimoireMachina
- **`PopulateWithJointData`**: Populates dropdown with all available joints from CGrimoireMachina

### How It Works

1. **Configuration Time**: The `data_populate_function` is read from FlatBuffers data and stored in the `DropDownListElement`

2. **Action Processing**: When `UIActionLogic` processes the dropdown (typically when it's clicked or opened), it calls `ProcessDropDownListElementActions()`

3. **Dynamic Population**: The function:
   - Finds the `CGrimoireMachina` component in the scene
   - Calls the appropriate `entity_helpers` function
   - Clears existing child elements
   - Creates new `DropDownItemElement` instances for each item
   - Each item gets both a `label` (display text) and `value` (data value)

## Adding New Population Functions

To add a new data population function:

### 1. Update the FlatBuffers Schema

Edit `src/flatbuffers_headers/user_interface.fbs`:

```fbs
enum DataPopulateFunction: byte {
    None = 0,
    PopulateWithFragmentData = 1,
    PopulateWithJointData = 2,
    PopulateWithNewData = 3,  // Add new value
}
```

### 2. Regenerate FlatBuffers Headers

```bash
cmake --build build --target flatbuffers_generate_headers
```

### 3. Add Helper Function

In `src/logic/entity_helpers.h`:

```cpp
std::vector<std::string>
GetNewDataNames(const CYourComponent &component);
```

In `src/logic/entity_helpers.cpp`:

```cpp
std::vector<std::string>
GetNewDataNames(const CYourComponent &component) {
  std::vector<std::string> names;
  // Extract names from your component
  for (const auto &[name, data] : component.m_your_map) {
    names.push_back(name);
  }
  return names;
}
```

### 4. Add Case to Switch Statement

In `src/logic/UIActionLogic.cpp`, add a new case to `ProcessDropDownListElementActions()`:

```cpp
case DataPopulateFunction::DataPopulateFunction_PopulateWithNewData: {
  // Find your component in the scene
  ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<CYourComponent>();
  
  const auto it = logic_context.archetypes.find(archetype_id);
  if (it != logic_context.archetypes.end()) {
    const Archetype &archetype = it->second;
    
    if (!archetype.empty()) {
      size_t entity_id = *archetype.begin();
      const CYourComponent &component =
          emp_helpers::GetComponent<CYourComponent>(
              entity_id, logic_context.scene_entities);
      
      std::vector<std::string> names =
          entity_helpers::GetNewDataNames(component);
      
      dropdown_list_element.child_elements.clear();
      
      for (const std::string &name : names) {
        auto item = std::make_unique<DropDownItemElement>();
        item->label = name;
        item->value = name;
        dropdown_list_element.child_elements.push_back(std::move(item));
      }
    }
  }
  break;
}
```

### 5. Write Tests

Create tests in `tests/logic/entity_helpers.test.cpp`:

```cpp
TEST_CASE("GetNewDataNames returns all names", "[entity_helpers]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::CYourComponent component;
  
  // Add test data
  component.m_your_map["item1"] = YourData{};
  component.m_your_map["item2"] = YourData{};
  
  std::vector<std::string> names =
      steamrot::entity_helpers::GetNewDataNames(component);
  
  REQUIRE(names.size() == 2);
  REQUIRE(std::find(names.begin(), names.end(), "item1") != names.end());
  REQUIRE(std::find(names.begin(), names.end(), "item2") != names.end());
}
```

## Architecture Notes

### Design Decisions

1. **Namespace Choice**: `entity_helpers` follows the pattern of `emp_helpers` for entity manipulation utilities

2. **Location**: Files are in `src/logic/` because they're used by Logic classes and manipulate entity state

3. **Dispatch Pattern**: Switch statement provides clear, compile-time-checked dispatch with good error messages

4. **Dynamic Creation**: Child elements are cleared and recreated each time to ensure fresh data

### Avoiding Subscriber Storage

The enum is stored on the UIElement (DropDownListElement) rather than on Subscribers. This:
- Keeps the Subscriber focused on event handling
- Makes the data source explicit in the UI structure
- Allows the same subscriber type to work with different data sources

### Future Improvements

Potential enhancements:
- Cache populated data to avoid regenerating on every action
- Add parameters to population functions (e.g., filter criteria)
- Support combining multiple data sources
- Add validation for empty data sets
