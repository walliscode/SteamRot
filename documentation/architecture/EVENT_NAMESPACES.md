# Event Namespace Documentation

This document clarifies the distinction between `steamrot::event` and `steamrot::events` namespaces.

## Namespace Purposes

### `steamrot::event` - Event Factory Functions

**Location**: `src/events/event_factory.{h,cpp}`, `src/events/event_bus_conversion.{h,cpp}`

**Purpose**: Factory functions for creating and converting event objects from FlatBuffers data.

**Functions**:
- `CreateUserInputBitset()` - Convert FlatBuffers UserInputBitsetData to UserInputBitset
- `CreateSceneChangePacket()` - Convert FlatBuffers SceneChangePacketData to SceneChangePacket
- `CreateUserInterfaceName()` - Convert FlatBuffers UserInterfaceNameData to UserInterfaceName
- `CreateEventData()` - Create EventData variant from FlatBuffers union type
- `CreateEventPacketFromData()` - Convert EventPacketData to EventPacket
- `ConvertEventBusDataToEventBus()` - Convert EventBusData to EventBus
- `ConfigureEventHandlerFromEventBusData()` - Configure EventHandler from EventBusData

**Usage Pattern**:
```cpp
#include "event_factory.h"

auto packet_result = steamrot::event::CreateEventPacketFromData(packet_data);
if (packet_result.has_value()) {
  EventPacket packet = packet_result.value();
  // Use packet...
}
```

**Key Characteristic**: These functions **create** event objects from data.

---

### `steamrot::events` - Event Handler Tick Functions

**Location**: `src/events/event_handler_tick.{h,cpp}`

**Purpose**: Consolidated event bus processing functions that can be used by both GameEngine and TestEngine.

**Functions**:
- `ProcessEventBusCycle()` - Process complete event bus cycle (waiting room → subscribers → tick)
- `ProcessWaitingRoomAndUpdateSubscribers()` - Process waiting room and update subscribers (without tick)

**Usage Pattern**:
```cpp
#include "event_handler_tick.h"

steamrot::events::ProcessEventBusCycle(event_handler);
// or
steamrot::events::ProcessWaitingRoomAndUpdateSubscribers(event_handler);
```

**Key Characteristic**: These functions **process** event handling cycles.

---

## Why Two Namespaces?

The separation reflects different responsibilities:

1. **`event` (singular)**: Deals with individual event creation/conversion
   - Factory pattern
   - Data transformation
   - FlatBuffers integration
   - Result: Individual event objects

2. **`events` (plural)**: Deals with bulk event processing
   - Event bus lifecycle
   - Subscriber notification
   - Tick management
   - Result: State changes in EventHandler

This follows the Single Responsibility Principle - each namespace has a distinct, focused purpose.

---

## Migration Note

If these namespaces are ever unified, consider:
- `steamrot::event::factory` - For factory functions
- `steamrot::event::processing` - For tick functions

However, the current structure is clear and follows established patterns in the codebase.

---

## Related Documentation

- [Event System Architecture](../architecture/EVENT_SYSTEM.md) - Overall event system design
- [Error Handling](../architecture/ERROR_HANDLING.md) - How event factory functions use std::expected
- [Game Loop](../architecture/GAME_LOOP.md) - How event processing fits into the game loop

---

**Last Updated**: December 4, 2025  
**Status**: Current and accurate
