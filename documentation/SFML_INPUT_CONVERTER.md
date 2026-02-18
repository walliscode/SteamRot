# SFML Input Converter

This documentation describes how to use the SFML input converter system to convert SFML keyboard and mouse events to InputPayload for the event system.

## Overview

The SFML input converter provides a clean, extensible way to map SFML input events (keyboard keys and mouse buttons) to game-specific InputActions. The system consists of two main components:

1. **InputMappingRegistry**: An extensible, runtime-configurable registry that maps SFML keys/buttons to InputActions
2. **ConvertSFMLEventToInputPayload**: Free functions that convert SFML events to InputPayload using a registry

## Quick Start

### Basic Usage with Default Mappings

```cpp
#include "sfml_input_converter.h"

// Poll SFML events
sf::Event event;
while (window.pollEvent(event)) {
    // Convert SFML event to InputPayload
    auto inputPayload = steamrot::events::ConvertSFMLEventToInputPayload(event);
    
    if (inputPayload.has_value()) {
        // Create an EventPacket and add to event bus
        auto eventPacket = steamrot::events::CreateInputEventPacket(
            1,  // lifetime in ticks
            inputPayload->action
        );
        
        if (eventPacket.has_value()) {
            event_handler.AddEvent(eventPacket.value());
        }
    }
}
```

### Default Mappings

By default, the following keys/buttons are mapped to SELECT action:
- **Keyboard**: Enter, Space
- **Mouse**: Left Button

### Custom Mappings

For custom control schemes, create a registry and configure it:

```cpp
#include "InputMappingRegistry.h"
#include "sfml_input_converter.h"

// Create a custom registry
steamrot::events::InputMappingRegistry registry;

// Add custom mappings
registry.MapKeyboardKey(sf::Keyboard::Key::A, 
                       steamrot::InputPayload::InputAction::SELECT);
registry.MapKeyboardKey(sf::Keyboard::Key::E, 
                       steamrot::InputPayload::InputAction::SELECT);
registry.MapMouseButton(sf::Mouse::Button::Right,
                       steamrot::InputPayload::InputAction::SELECT);

// Use the custom registry
sf::Event event;
while (window.pollEvent(event)) {
    auto inputPayload = steamrot::events::ConvertSFMLEventToInputPayload(event, registry);
    
    if (inputPayload.has_value()) {
        // Process the input...
    }
}
```

## API Reference

### InputMappingRegistry

#### Constructor
```cpp
InputMappingRegistry();
```
Creates a registry initialized with default mappings.

#### Mapping Methods
```cpp
void MapKeyboardKey(sf::Keyboard::Key key, InputPayload::InputAction action);
void MapMouseButton(sf::Mouse::Button button, InputPayload::InputAction action);
```
Add or override a key/button mapping.

#### Query Methods
```cpp
InputPayload::InputAction GetActionForKey(sf::Keyboard::Key key) const;
InputPayload::InputAction GetActionForMouseButton(sf::Mouse::Button button) const;
```
Query the action mapped to a key/button. Returns `InputAction::NONE` if not mapped.

#### Removal Methods
```cpp
void UnmapKeyboardKey(sf::Keyboard::Key key);
void UnmapMouseButton(sf::Mouse::Button button);
void ClearKeyboardMappings();
void ClearMouseMappings();
void ResetToDefaults();
```

### ConvertSFMLEventToInputPayload

#### With Custom Registry
```cpp
std::optional<InputPayload> 
ConvertSFMLEventToInputPayload(const sf::Event &event,
                               const InputMappingRegistry &registry);
```
Converts an SFML event using the provided registry. Returns `std::nullopt` if the event doesn't map to an action.

#### With Default Mappings
```cpp
std::optional<InputPayload> 
ConvertSFMLEventToInputPayload(const sf::Event &event);
```
Convenience function that uses default mappings. For performance-critical code, prefer creating a registry once and reusing it.

## Event Handling

The converter only processes:
- **KeyPressed** events (not KeyReleased)
- **MouseButtonPressed** events (not MouseButtonReleased)

Other SFML events (window close, resize, etc.) return `std::nullopt` and should be handled separately.

## Performance Considerations

- For best performance, create one `InputMappingRegistry` and reuse it across multiple conversions
- The single-parameter `ConvertSFMLEventToInputPayload` creates a temporary registry each time
- Registry lookups use `std::unordered_map` for O(1) average lookup time

## Examples

### Example 1: Multiple Actions

```cpp
// When InputPayload supports more actions, you can map different keys to different actions
registry.MapKeyboardKey(sf::Keyboard::Key::Enter, InputPayload::InputAction::SELECT);
registry.MapKeyboardKey(sf::Keyboard::Key::Escape, InputPayload::InputAction::CANCEL);
```

### Example 2: User-Configurable Controls

```cpp
// Load user preferences
auto userKeyBinding = LoadUserKeyBinding("select_action");

// Apply to registry
registry.MapKeyboardKey(userKeyBinding, InputPayload::InputAction::SELECT);

// Save updated registry for next session
SaveUserKeyBinding("select_action", userKeyBinding);
```

### Example 3: Context-Specific Mappings

```cpp
// Different mappings for different game states
InputMappingRegistry menuRegistry;
menuRegistry.MapKeyboardKey(sf::Keyboard::Key::W, InputPayload::InputAction::MOVE_UP);
menuRegistry.MapKeyboardKey(sf::Keyboard::Key::S, InputPayload::InputAction::MOVE_DOWN);

InputMappingRegistry gameplayRegistry;
gameplayRegistry.MapKeyboardKey(sf::Keyboard::Key::W, InputPayload::InputAction::JUMP);
gameplayRegistry.MapKeyboardKey(sf::Keyboard::Key::S, InputPayload::InputAction::CROUCH);

// Switch based on game state
const auto& activeRegistry = (gameState == Menu) ? menuRegistry : gameplayRegistry;
auto payload = ConvertSFMLEventToInputPayload(event, activeRegistry);
```

## Testing

Comprehensive unit tests are provided in:
- `tests/unit/events/InputMappingRegistry.test.cpp`
- `tests/unit/events/sfml_input_converter.test.cpp`

Run tests with:
```bash
ctest --preset Debug -R InputMappingRegistry
ctest --preset Debug -R sfml_input_converter
```

## Future Enhancements

Possible future additions:
- Key combination support (Ctrl+Key, Shift+Key)
- Gamepad/joystick support
- Configuration file loading/saving
- Conflict detection (same key mapped to multiple actions)
- Dead zone configuration for analog inputs
