# CUIState Component Usage Guide

## Overview

The `CUIState` component manages UI visibility based on scene state. It maps state keys (strings) to UI visibility configurations and can be triggered by events through the Subscriber system.

## Architecture

- **State Storage**: `std::unordered_map<std::string, bool>` - Maps state keys to boolean values
- **UI Visibility**: `std::unordered_map<std::string, UIVisibilityState>` - Maps state keys to UI configurations
- **Subscribers**: `std::unordered_map<std::string, std::vector<std::shared_ptr<Subscriber>>>` - Maps state keys to multiple event subscribers

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
  subscribers: [SubscriberData];  // Multiple subscriber configurations
}

table UIStateData {
  mappings: [UIStateMapping] (required);
}
```

### JSON Configuration Example

#### Simple Example: Single Subscriber per State

```json
{
  "c_ui_state": {
    "mappings": [
      {
        "state_key": "menu_open",
        "ui_names_on": ["menu_panel", "close_button"],
        "ui_names_off": ["play_button", "settings_button"],
        "subscribers": [
          {
            "event_type_data": "EVENT_USER_INPUT",
            "trigger_data_type": "UserInputBitsetData",
            "trigger_data": {
              "keyboard_buttons": ["Escape"]
            }
          }
        ]
      }
    ]
  }
}
```

#### Complex Example: Multiple Subscribers per State

Multiple subscribers allow you to create complex state conditions. The state activates when **any** subscriber is triggered:

```json
{
  "c_ui_state": {
    "mappings": [
      {
        "state_key": "pause_menu_open",
        "ui_names_on": ["pause_panel", "resume_button", "quit_button"],
        "ui_names_off": ["hud_panel", "inventory_button"],
        "subscribers": [
          {
            "event_type_data": "EVENT_USER_INPUT",
            "trigger_data_type": "UserInputBitsetData",
            "trigger_data": {
              "keyboard_buttons": ["Escape"]
            }
          },
          {
            "event_type_data": "EVENT_USER_INPUT",
            "trigger_data_type": "UserInputBitsetData",
            "trigger_data": {
              "keyboard_buttons": ["P"]
            }
          },
          {
            "event_type_data": "EVENT_TOGGLE_DROPDOWN"
          }
        ]
      }
    ]
  }
}
```

In this example, the "pause_menu_open" state will activate if:
- The user presses **Escape**, OR
- The user presses **P**, OR
- An **EVENT_TOGGLE_DROPDOWN** event is triggered

## How It Works

### 1. Configuration Phase

When `FlatbuffersConfigurator` processes a `CUIState` component:

1. For each `UIStateMapping`:
   - Resolves UI component names to entity indices
   - Stores the visibility configuration
   - Initializes the state value to `false`
   - Creates and registers all subscribers (if provided)

### 2. Runtime Phase

The `UIStateLogic` runs every frame as an Action logic:

1. Finds all entities with active `CUIState` components
2. For each state's subscribers:
   - Checks if **any** subscriber is active
   - If any subscriber is active: Sets the state value to `true`
   - Deactivates all active subscribers after processing

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

### Simple Example: Single Subscriber

1. User presses Escape key
2. EventHandler receives EVENT_USER_INPUT with keyboard data
3. EventHandler activates matching subscribers
4. UIStateLogic runs and checks subscribers
5. Finds one of "pause_menu_open" subscribers is active
6. Sets `m_state_values["pause_menu_open"] = true`
7. Deactivates all active subscribers for that state
8. Other logic can now check the state and respond

### Complex Example: Multiple Subscribers

With multiple subscribers, **any** of them can activate the state:

1. State "pause_menu_open" has 3 subscribers (Escape, P, TOGGLE_DROPDOWN)
2. User presses **P** key
3. EventHandler activates the "P" subscriber
4. UIStateLogic runs and finds one subscriber active
5. Sets `m_state_values["pause_menu_open"] = true`
6. Deactivates the "P" subscriber
7. The state remains true until explicitly reset

This enables OR logic: the state activates if Escape **OR** P **OR** TOGGLE_DROPDOWN occurs.

## Best Practices

1. **State Naming**: Use descriptive, lowercase snake_case names (e.g., "menu_open", "dialog_visible")

2. **Multiple Subscribers**: Use multiple subscribers when you want OR logic - the state activates when **any** subscriber triggers. For AND logic (requiring multiple conditions), create a custom Logic class instead.

3. **Subscriber Design**: Keep trigger conditions simple and specific. Complex logic should be in Logic classes, not in subscriber triggers.

4. **State Reset**: States persist until explicitly changed. If you need a state to reset, create another subscriber or Logic that sets it back to false.

5. **Testing**: Use the `SubscriberFactory` to create and register subscribers for testing:

```cpp
SubscriberFactory subscriber_factory(event_handler);
auto subscriber_result = subscriber_factory.CreateAndRegisterSubscriber(
    EventType::EVENT_TEST);
```

## Component Members

- `m_state_to_ui_visibility`: Maps state keys to UI visibility configurations
- `m_state_values`: Maps state keys to their current boolean values (default: false)
- `m_state_subscribers`: Maps state keys to a vector of subscribers (supports multiple subscribers per state)

## Related Classes

- **UIStateLogic**: Processes subscriber activations and updates state values
- **FlatbuffersConfigurator**: Configures CUIState from FlatBuffers data
- **SubscriberFactory**: Creates and registers subscribers with EventHandler
- **Subscriber**: Lightweight event listener that gets activated when events match
