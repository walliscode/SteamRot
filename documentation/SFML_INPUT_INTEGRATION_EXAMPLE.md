# Integration Example: Using SFML Input Converter with EventHandler

This document shows how to integrate the new SFML input converter with the existing `EventHandler::HandleSFMLEvents` function.

## Current State

The `HandleSFMLEvents` function in `src/events/EventHandler.cpp` is currently empty:

```cpp
void HandleSFMLEvents(sf::RenderWindow &window, EventHandler &event_handler) {}
```

## Proposed Implementation

Here's an example implementation using the new converter:

```cpp
#include "sfml_input_converter.h"
#include "event_factory.h"

void HandleSFMLEvents(sf::RenderWindow &window, EventHandler &event_handler) {
    // Create a persistent registry (could be a member of EventHandler or static)
    static steamrot::events::InputMappingRegistry input_registry;
    
    // Poll all SFML events
    sf::Event event;
    while (window.pollEvent(event)) {
        
        // Handle window close event
        if (event.is<sf::Event::Closed>()) {
            window.close();
        }
        
        // Convert SFML input events to InputPayload
        auto input_payload = steamrot::events::ConvertSFMLEventToInputPayload(
            event, input_registry);
        
        // If we got a valid InputPayload, create an EventPacket and add to waiting room
        if (input_payload.has_value()) {
            auto event_packet = steamrot::events::CreateInputEventPacket(
                1,  // lifetime: 1 tick
                input_payload->action
            );
            
            if (event_packet.has_value()) {
                event_handler.AddEvent(event_packet.value());
            }
        }
    }
}
```

## Alternative Implementation: With Registry as EventHandler Member

If you want more control over the input mappings, you could make the registry a member of `EventHandler`:

### In EventHandler.h:
```cpp
#include "InputMappingRegistry.h"

class EventHandler {
    // ... existing members ...
    
private:
    steamrot::events::InputMappingRegistry m_input_mapping_registry;
    
public:
    // Add method to access registry for runtime configuration
    steamrot::events::InputMappingRegistry& GetInputMappingRegistry() {
        return m_input_mapping_registry;
    }
};
```

### In EventHandler.cpp:
```cpp
void HandleSFMLEvents(sf::RenderWindow &window, EventHandler &event_handler) {
    sf::Event event;
    while (window.pollEvent(event)) {
        
        // Handle window close event
        if (event.is<sf::Event::Closed>()) {
            window.close();
        }
        
        // Use the EventHandler's registry
        auto input_payload = steamrot::events::ConvertSFMLEventToInputPayload(
            event, event_handler.GetInputMappingRegistry());
        
        if (input_payload.has_value()) {
            auto event_packet = steamrot::events::CreateInputEventPacket(
                1, input_payload->action);
            
            if (event_packet.has_value()) {
                event_handler.AddEvent(event_packet.value());
            }
        }
    }
}
```

This approach allows other parts of the system to reconfigure input mappings at runtime:
```cpp
// Example: Allow user to remap keys
event_handler.GetInputMappingRegistry().MapKeyboardKey(
    sf::Keyboard::Key::E, 
    steamrot::InputPayload::InputAction::SELECT
);
```

## Event Flow

With this implementation, the event flow becomes:

```
SFML Window Events
       ↓
HandleSFMLEvents()
       ↓
ConvertSFMLEventToInputPayload() ← InputMappingRegistry
       ↓
InputPayload
       ↓
CreateInputEventPacket()
       ↓
EventPacket
       ↓
event_handler.AddEvent() → waiting_room_event_bus
       ↓
ProcessWaitingRoomEventBus()
       ↓
global_event_bus
       ↓
UpdateSubscribersFromGlobalEventBus()
       ↓
Subscribers activated with payload
```

## Lifetime Considerations

The lifetime parameter in `CreateInputEventPacket` controls how many ticks the event persists:
- **1 tick**: Event processed immediately and removed
- **Higher values**: Event persists for multiple ticks (useful for sustained input)

For immediate input responses, use lifetime of 1.

## Testing the Integration

After implementing, you can test by:

1. Adding a Subscriber that listens for InputPayload events
2. Running the game and pressing Enter, Space, or clicking left mouse button
3. Verifying the Subscriber gets activated with the SELECT action

Example test Subscriber:
```cpp
auto subscriber = std::make_shared<Subscriber>(
    EventType::USER_INPUT,
    InputPayload{InputPayload::InputAction::SELECT}
);

event_handler.RegisterSubscriber(subscriber);

// After input, check:
REQUIRE(subscriber->IsActivated());
```

## Future Enhancements

- Add support for multiple InputActions (when they're added to InputPayload enum)
- Implement key combination support (Ctrl+Key, etc.)
- Add configuration file loading for user-customizable controls
- Support for gamepad/joystick input
