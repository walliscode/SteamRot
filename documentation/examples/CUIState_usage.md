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

#### Complex Example: Multiple Subscribers per State (AND Logic)

Multiple subscribers allow you to create complex state conditions. The state activates when **ALL** subscribers are triggered:

```json
{
  "c_ui_state": {
    "mappings": [
      {
        "state_key": "secret_menu_unlock",
        "ui_names_on": ["secret_panel", "special_button"],
        "ui_names_off": ["normal_panel"],
        "subscribers": [
          {
            "event_type_data": "EVENT_USER_INPUT",
            "trigger_data_type": "UserInputBitsetData",
            "trigger_data": {
              "keyboard_buttons": ["Shift"]
            }
          },
          {
            "event_type_data": "EVENT_USER_INPUT",
            "trigger_data_type": "UserInputBitsetData",
            "trigger_data": {
              "keyboard_buttons": ["Ctrl"]
            }
          },
          {
            "event_type_data": "EVENT_USER_INPUT",
            "trigger_data_type": "UserInputBitsetData",
            "trigger_data": {
              "keyboard_buttons": ["S"]
            }
          }
        ]
      }
    ]
  }
}
```

In this example, the "secret_menu_unlock" state will activate **only if**:
- The user presses **Shift** AND
- The user presses **Ctrl** AND
- The user presses **S**

All three subscribers must be active simultaneously.

#### OR Logic: Repeating State Keys

For OR logic (activate on any condition), repeat the same state key with different subscriber configurations:

```json
{
  "c_ui_state": {
    "mappings": [
      {
        "state_key": "pause_menu_open",
        "ui_names_on": ["pause_panel"],
        "ui_names_off": ["hud_panel"],
        "subscribers": [
          {
            "event_type_data": "EVENT_USER_INPUT",
            "trigger_data": {"keyboard_buttons": ["Escape"]}
          }
        ]
      },
      {
        "state_key": "pause_menu_open",
        "ui_names_on": ["pause_panel"],
        "ui_names_off": ["hud_panel"],
        "subscribers": [
          {
            "event_type_data": "EVENT_USER_INPUT",
            "trigger_data": {"keyboard_buttons": ["P"]}
          }
        ]
      }
    ]
  }
}
```

This activates "pause_menu_open" when Escape **OR** P is pressed.

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
   - Checks if **ALL** subscribers are active (AND logic)
   - If all subscribers are active: Sets the state value to `true`
   - Deactivates all subscribers after processing

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
5. Finds the "menu_open" subscriber is active
6. Sets `m_state_values["menu_open"] = true`
7. Deactivates the subscriber
8. Other logic can now check the state and respond

### Complex Example: Multiple Subscribers (AND Logic)

With multiple subscribers, **ALL** of them must be active to activate the state:

1. State "secret_menu_unlock" has 3 subscribers (Shift, Ctrl, S)
2. User presses and holds **Shift** key - first subscriber activates
3. User presses and holds **Ctrl** key - second subscriber activates
4. User presses **S** key - third subscriber activates
5. UIStateLogic runs and finds **ALL** three subscribers active
6. Sets `m_state_values["secret_menu_unlock"] = true`
7. Deactivates all three subscribers
8. The state remains true until explicitly reset

This enables AND logic: the state activates only if Shift **AND** Ctrl **AND** S are all pressed.

### OR Logic Example: Repeating State Keys

For OR logic, repeat the state key in multiple mappings:

1. State "pause_menu_open" defined twice with different subscribers
2. First mapping: Escape key subscriber
3. Second mapping: P key subscriber
4. If user presses **Escape**, first mapping activates the state
5. If user presses **P**, second mapping activates the state
6. Either condition (Escape **OR** P) will activate "pause_menu_open"

## Best Practices

1. **State Naming**: Use descriptive, lowercase snake_case names (e.g., "menu_open", "dialog_visible")

2. **AND Logic with Multiple Subscribers**: Use multiple subscribers in a single mapping when you need **ALL** conditions to be true simultaneously (e.g., Shift+Ctrl+S for a secret menu).

3. **OR Logic with Repeated State Keys**: For **any** condition logic, repeat the same state key in multiple mappings with different subscriber configurations (e.g., pause on Escape OR P).

4. **Subscriber Design**: Keep trigger conditions simple and specific. Complex logic should be in Logic classes, not in subscriber triggers.

5. **State Reset**: States persist until explicitly changed. If you need a state to reset, create another subscriber or Logic that sets it back to false.

6. **Testing**: Use the `SubscriberFactory` to create and register subscribers for testing:

```cpp
SubscriberFactory subscriber_factory(event_handler);
auto subscriber_result = subscriber_factory.CreateAndRegisterSubscriber(
    EventType::EVENT_TEST);
```

## Component Members

- `m_state_to_ui_visibility`: Maps state keys to UI visibility configurations
- `m_state_values`: Maps state keys to their current boolean values (default: false)
- `m_state_subscribers`: Maps state keys to a vector of subscribers (ALL must be active for AND logic)

## Related Classes

- **UIStateLogic**: Processes subscriber activations and updates state values
- **FlatbuffersConfigurator**: Configures CUIState from FlatBuffers data
- **SubscriberFactory**: Creates and registers subscribers with EventHandler
- **Subscriber**: Lightweight event listener that gets activated when events match
