# Event System Quick Reference

This is a quick reference companion to the full implementation plan. For complete details, see `EVENT_SYSTEM_IMPLEMENTATION_PLAN.md`.

## What You Asked For

1. ✅ **Event Factory**: Free functions for creating EventPackets from parameters
2. ✅ **Random Event Generator**: Function for producing random EventPackets for testing
3. ✅ **EventPacket Matcher Verification**: Ensure matcher is fully functional
4. ✅ **Payload Matching Functions**: In-game code for subscriber filtering

## Current System Status

### What Exists ✅
- EventPacket, EventPayload, EventType, Subscriber, EventHandler structures
- EventPacketEqualsMatcher and EventPayloadEqualsMatcher (appear complete)
- Configure functions for loading events from FlatBuffers
- Comprehensive test suite for EventHandler

### What's Missing ❌
- **Event factory functions** for easy event creation
- **Random event generator** for testing
- **Payload matching functions** for subscriber filtering
- **Implementation of TODO** in EventHandler.cpp line 133

## Files to Create

### Core Implementation (4 files)
1. `src/events/event_factory.h` - Factory function declarations
2. `src/events/event_factory.cpp` - Factory implementations + random generation
3. `src/events/payload_matcher.h` - Matching function declarations
4. `src/events/payload_matcher.cpp` - Matching implementations

### Tests (4 files)
5. `tests/unit/events/event_factory.test.cpp`
6. `tests/unit/events/payload_matcher.test.cpp`
7. `tests/unit/events/random_event_propagation.test.cpp`
8. `tests/integration/event_system/event_propagation.integration.test.cpp`

### Files to Modify (2 files)
- `src/events/EventHandler.cpp` - Replace TODO with Matches() call
- Update CMakeLists.txt files (2 files)

## Key API Examples

### Event Factory Usage

```cpp
#include "event_factory.h"

// Create input event
auto event = steamrot::CreateInputEvent(
    steamrot::InputPayload::InputAction::SELECT,
    2  // lifetime
);

// Create UI event
auto event = steamrot::CreateUIEvent(
    "menu",          // UI name (optional)
    std::nullopt,    // State name (don't care)
    steamrot::UIPayload::UIAction::TOGGLE,
    1                // lifetime (default)
);

// Create random event for testing
auto random = steamrot::CreateRandomEventPacket();
```

### Payload Matching Usage

```cpp
#include "payload_matcher.h"

// Create filter
steamrot::UIPayload filter;
filter.c_user_interface_name = "crafting_ui";
filter.c_ui_state_name = std::nullopt;  // Don't care
filter.action = steamrot::UIPayload::UIAction::TOGGLE;

// Check if event matches
steamrot::UIPayload event;
event.c_user_interface_name = "crafting_ui";
event.c_ui_state_name = "any_state";  // OK, filter doesn't care
event.action = steamrot::UIPayload::UIAction::TOGGLE;

bool matches = steamrot::Matches(filter, event);  // true
```

### Subscriber Filtering

```cpp
// Create subscriber with filter
auto subscriber = std::make_shared<steamrot::Subscriber>();
subscriber->event_type = steamrot::EventType::UI;

// Only activate for "crafting_ui" with TOGGLE action
subscriber->filter_payload = steamrot::CreateUIPayload(
    "crafting_ui",
    std::nullopt,  // Don't care about state
    steamrot::UIPayload::UIAction::TOGGLE
);

event_handler.RegisterSubscriber(subscriber);
```

## Matching Semantics

### Rules
1. **Variant Type**: Filter and event must be same type (InputPayload, UIPayload, etc.)
2. **Required Fields**: Enums and non-optional fields must match exactly
3. **Optional Fields**:
   - `nullopt` in filter = "don't care" (matches anything)
   - Value in filter = must match exactly

### Examples

```cpp
// Example 1: Partial matching with optionals
UIPayload filter{
    .c_user_interface_name = "menu",
    .c_ui_state_name = std::nullopt,  // Don't care
    .action = UIPayload::UIAction::TOGGLE
};

UIPayload event1{
    .c_user_interface_name = "menu",
    .c_ui_state_name = "main",
    .action = UIPayload::UIAction::TOGGLE
};
// Matches(filter, event1) -> true

UIPayload event2{
    .c_user_interface_name = "menu",
    .c_ui_state_name = "settings",
    .action = UIPayload::UIAction::TOGGLE
};
// Matches(filter, event2) -> true (state doesn't matter)

UIPayload event3{
    .c_user_interface_name = "inventory",
    .c_ui_state_name = "main",
    .action = UIPayload::UIAction::TOGGLE
};
// Matches(filter, event3) -> false (name differs)
```

## Implementation Order

### Recommended Sequence
1. **Start with Event Factory** (easiest, no dependencies)
   - Implement factory functions
   - Write tests
   - Refactor existing tests to use factories

2. **Add Random Generation** (built into factory)
   - Implement CreateRandomEventPacket()
   - Write propagation tests

3. **Verify Matchers** (probably already work)
   - Add edge case tests
   - Ensure all payload types covered

4. **Implement Payload Matching** (most critical)
   - Implement Matches() functions
   - Write comprehensive tests
   - Update EventHandler.cpp

5. **Integration** (verify everything works together)
   - Run all tests
   - Fix broken tests
   - End-to-end integration tests

## Critical Change: EventHandler.cpp

**Current (line 119-138)**:
```cpp
void UpdateSubscriber(std::weak_ptr<Subscriber> &subscriber,
                      const EventPayload &event_payload) {
  auto locked_subscriber = subscriber.lock();
  if (!locked_subscriber)
    return;

  if (locked_subscriber->filter_payload.has_value()) {
    const auto &filter_payload_data = locked_subscriber->filter_payload.value();
    // [TODO:] implement Matches functions for Subscribers
  }

  locked_subscriber->m_active = true;
  locked_subscriber->captured_payload = event_payload;
}
```

**Updated**:
```cpp
#include "payload_matcher.h"  // Add this

void UpdateSubscriber(std::weak_ptr<Subscriber> &subscriber,
                      const EventPayload &event_payload) {
  auto locked_subscriber = subscriber.lock();
  if (!locked_subscriber)
    return;

  if (locked_subscriber->filter_payload.has_value()) {
    const auto &filter_payload_data = locked_subscriber->filter_payload.value();
    
    // Use Matches function
    if (!Matches(filter_payload_data, event_payload)) {
      return;  // Don't activate if doesn't match
    }
  }

  locked_subscriber->m_active = true;
  locked_subscriber->captured_payload = event_payload;
}
```

## Testing Strategy

### Unit Tests
- Test each factory function independently
- Test each Matches() function independently
- Test all enum values
- Test all optional combinations

### Integration Tests
- EventHandler with filtered subscribers
- Random event propagation
- Factory + Matcher integration

### Test Coverage Goals
- All payload types
- All action enums
- All optional field combinations
- Edge cases (empty optionals, mismatches)

## Common Pitfalls to Avoid

1. **EventType Mismatch**: Factory must set correct EventType for payload
   ```cpp
   // WRONG
   EventPacket packet;
   packet.type = EventType::UI;  // Type says UI
   packet.payload = InputPayload{};  // But payload is Input!
   
   // RIGHT (factory does this automatically)
   auto packet = CreateInputEvent(...);  // Type and payload match
   ```

2. **Optional Semantics**: nullopt in filter means "don't care", not "must be empty"
   ```cpp
   // Filter with nullopt
   filter.c_ui_state_name = std::nullopt;
   
   // Matches this
   event.c_ui_state_name = "any_value";  // YES
   event.c_ui_state_name = std::nullopt; // YES
   ```

3. **Variant Type Check**: Always check variant type before field comparison
   ```cpp
   // WRONG
   if (std::get<UIPayload>(filter).action == ...) // Throws if wrong type!
   
   // RIGHT
   if (filter.index() != event.index()) return false;
   if (std::holds_alternative<UIPayload>(filter)) {
     // Safe to use std::get now
   }
   ```

## Quick Validation

After implementation, verify:
1. ✅ `ctest --preset Debug -R event_factory` passes
2. ✅ `ctest --preset Debug -R payload_matcher` passes
3. ✅ `ctest --preset Debug -R EventHandler` passes
4. ✅ All existing event tests still pass
5. ✅ Subscribers with filters work correctly
6. ✅ Subscribers without filters work correctly

## Next Steps

1. Read full implementation plan: `EVENT_SYSTEM_IMPLEMENTATION_PLAN.md`
2. Start with Phase 1 (Event Factory)
3. Test each phase before moving to next
4. Use checklist in full plan to track progress

## Getting Help

If stuck:
- Check full plan for detailed examples
- Look at existing test patterns in `tests/unit/events/EventHandler.test.cpp`
- Look at existing matchers in `tests/matchers/`
- Review existing configure functions in `src/data_providers/configure/configure_event.cpp`

---

**Remember**: This is a plan for YOU to implement. Take it step by step, test thoroughly, and don't hesitate to adjust as needed based on what you learn during implementation.
