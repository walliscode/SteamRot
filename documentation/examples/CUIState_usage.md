# CUIState Component Usage Guide

## Overview

The `CUIState` component manages UI visibility based on scene state. It maps state keys (strings) to UI visibility configurations and can be triggered by events through the Subscriber system.

## Architecture

- **State Storage**: `std::unordered_map<std::string, bool>` - Maps state keys to boolean values
- **UI Visibility**: `std::unordered_map<std::string, UIVisibilityState>` - Maps state keys to UI configurations
- **Subscribers**: `std::unordered_map<std::string, std::shared_ptr<Subscriber>>` - Maps state keys to event subscribers

## Data-Driven Configuration

States and their subscribers are configured through FlatBuffers data files.

### FlatBuffers Schema

```fbs
// ui_state.fbs
include "subscriber_config.fbs";

namespace steamrot;

table UIStateMapping {
  state_key: string (required);
  ui_names_on: [string];
  ui_names_off: [string];
  subscriber_data: SubscriberData;  // Optional subscriber configuration
}

table UIStateData {
  mappings: [UIStateMapping] (required);
}
```

### JSON Configuration Example

```json
{
  "c_ui_state": {
    "mappings": [
      {
        "state_key": "menu_open",
        "ui_names_on": ["menu_panel", "close_button"],
        "ui_names_off": ["play_button", "settings_button"],
        "subscriber_data": {
          "event_type_data": "EVENT_USER_INPUT",
          "trigger_data_type": "UserInputBitsetData",
          "trigger_data": {
            "mouse_buttons": ["Left"],
            "keyboard_buttons": ["Escape"]
          }
        }
      },
      {
        "state_key": "settings_open",
        "ui_names_on": ["settings_panel"],
        "ui_names_off": ["menu_panel"],
        "subscriber_data": {
          "event_type_data": "EVENT_USER_INPUT",
          "trigger_data_type": "UserInputBitsetData",
          "trigger_data": {
            "mouse_buttons": ["Left"]
          }
        }
      }
    ]
  }
}
```

## How It Works

### 1. Configuration Phase

When `FlatbuffersConfigurator` processes a `CUIState` component:

1. For each `UIStateMapping`:
   - Resolves UI component names to entity indices
   - Stores the visibility configuration
   - Initializes the state value to `false`
   - Creates and registers a subscriber (if subscriber_data is provided)

### 2. Runtime Phase

The `UIStateLogic` runs every frame as an Action logic:

1. Finds all entities with active `CUIState` components
2. For each state's subscriber:
   - Checks if the subscriber is active
   - If active: Sets the state value to `true`
   - Deactivates the subscriber after processing

### 3. State Usage

Other logic classes can check state values:

```cpp
// Get the CUIState component
CUIState &ui_state = emp_helpers::GetComponent<CUIState>(
    entity_id, entity_memory_pool);

// Check if a state is active
if (ui_state.m_state_values["menu_open"]) {
  // Menu is open - handle accordingly
  
  // Get the UI visibility configuration
  const UIVisibilityState &visibility = 
      ui_state.m_state_to_ui_visibility["menu_open"];
  
  // Show/hide UI elements based on configuration
  for (size_t ui_index : visibility.m_ui_indices_on) {
    // Show UI at ui_index
  }
  for (size_t ui_index : visibility.m_ui_indices_off) {
    // Hide UI at ui_index
  }
}
```

## Event Flow Example

1. User presses Escape key
2. EventHandler receives EVENT_USER_INPUT with keyboard data
3. EventHandler activates matching subscribers
4. UIStateLogic runs and checks subscribers
5. Finds "menu_open" subscriber is active
6. Sets `m_state_values["menu_open"] = true`
7. Deactivates the subscriber
8. Other logic can now check the state and respond

## Best Practices

1. **State Naming**: Use descriptive, lowercase snake_case names (e.g., "menu_open", "dialog_visible")

2. **Subscriber Design**: Keep trigger conditions simple and specific. Complex logic should be in Logic classes, not in subscriber triggers.

3. **State Reset**: States persist until explicitly changed. If you need a state to reset, create another subscriber or Logic that sets it back to false.

4. **Testing**: Use the `SubscriberFactory` to create and register subscribers for testing:

```cpp
SubscriberFactory subscriber_factory(event_handler);
auto subscriber_result = subscriber_factory.CreateAndRegisterSubscriber(
    EventType::EVENT_TEST);
```

## Component Members

- `m_state_to_ui_visibility`: Maps state keys to UI visibility configurations
- `m_state_values`: Maps state keys to their current boolean values (default: false)
- `m_state_subscribers`: Maps state keys to their associated subscribers (optional)

## Related Classes

- **UIStateLogic**: Processes subscriber activations and updates state values
- **FlatbuffersConfigurator**: Configures CUIState from FlatBuffers data
- **SubscriberFactory**: Creates and registers subscribers with EventHandler
- **Subscriber**: Lightweight event listener that gets activated when events match
