# Event System Refactoring - Quick Start Guide

## For the Busy Developer

This is a condensed guide for developers who need to understand and implement the event system refactoring quickly. For full details, see the comprehensive documents in this directory.

## 30-Second Summary

**Problem:** Event system uses FlatBuffers differently than components (tight coupling, inconsistent naming, scattered logic)

**Solution:** Align events with component patterns - decouple runtime types, standardize naming, consolidate conversion

**Effort:** 11-17 hours across 6 phases

**Status:** Analysis complete, ready for implementation

## 5-Minute Overview

### What's Wrong?

```cpp
// ❌ Current: Event types include FlatBuffers
#include "events_generated.h"  // FlatBuffers coupling

struct EventPacket {
    EventType event_type;  // FlatBuffers enum!
};
```

### What We Want

```cpp
// ✅ Proposed: Independent runtime types
#include "EventType.h"  // Own definition

struct EventPacket {
    EventType event_type;  // Runtime enum
};
```

### Core Issues

1. **Coupling:** Runtime event types include FlatBuffers headers
2. **Naming:** Inconsistent schemas (`EventDataData`, `SubscriberFbs` vs `EventPacketData`)
3. **Dependencies:** Entity schemas include event schemas directly
4. **Logic:** Conversion scattered across 3 factory files

## Reading Order

1. **EVENT_SYSTEM_COMPARISON.md** (10 min) - Visual diagrams showing the problems
2. **EVENT_SYSTEM_ANALYSIS.md** (20 min) - Detailed technical analysis
3. **EVENT_SYSTEM_REFACTORING_PLAN.md** (30 min) - Implementation steps

## Implementation Checklist

### Phase 1: Decouple Runtime Types (2-3 hours)

- [ ] Create `src/types/core/EventType.h` (pure enum)
- [ ] Create `src/events/event_type_conversion.h/cpp`
- [ ] Update `EventPacket.h` - remove FlatBuffers includes
- [ ] Update `Subscriber.h` - remove FlatBuffers includes
- [ ] Update event_factory to use conversion
- [ ] Add tests for conversion
- [ ] Verify all existing tests pass

**Key Files:**
- `src/types/core/EventType.h` (NEW)
- `src/events/event_type_conversion.h/cpp` (NEW)
- `src/types/events/EventPacket.h` (MODIFY)
- `src/types/events/Subscriber.h` (MODIFY)

### Phase 2: Standardize Naming (1-2 hours)

- [ ] Rename `EventType` → `EventTypeFbs` in events.fbs
- [ ] Rename `EventDataData` → `EventDataUnion` in events.fbs
- [ ] Rename `SubscriberFbs` → `SubscriberData` in subscriber.fbs
- [ ] Update all references in code
- [ ] Rebuild FlatBuffers headers
- [ ] Update tests

**Key Files:**
- `src/types/flatbuffers/events/events.fbs` (MODIFY)
- `src/types/flatbuffers/events/subscriber.fbs` (MODIFY)
- All code referencing these types

### Phase 3: Reduce Coupling (3-4 hours)

- [ ] Create `src/types/flatbuffers/core/event_reference.fbs`
- [ ] Define `SubscriberRef` and `EventPacketRef` tables
- [ ] Update `user_interface.fbs` to use references
- [ ] Create `event_config_resolver.h/cpp`
- [ ] Update UI configurator to resolve references
- [ ] Update test data files
- [ ] Verify UI functionality

**Key Files:**
- `src/types/flatbuffers/core/event_reference.fbs` (NEW)
- `src/events/event_config_resolver.h/cpp` (NEW)
- `src/types/flatbuffers/entities/user_interface.fbs` (MODIFY)
- `src/user_interface/FlatbuffersUIElementConfigurator.cpp` (MODIFY)

### Phase 4: Consolidate Logic (2-3 hours)

- [ ] Create `IEventConfigurator` interface
- [ ] Create `FlatbuffersEventConfigurator` class
- [ ] Move logic from event_factory → configurator
- [ ] Move logic from event_bus_conversion → configurator
- [ ] Move logic from subscriber_factory → configurator
- [ ] Update callers to use configurator
- [ ] Deprecate old factories

**Key Files:**
- `src/types/interfaces/IEventConfigurator.h` (NEW)
- `src/events/FlatbuffersEventConfigurator.h/cpp` (NEW)
- `src/events/event_factory.cpp` (MODIFY - thin wrapper)

### Phase 5: Testing (2-3 hours)

- [ ] Run full test suite: `ctest --preset Debug`
- [ ] Add unit tests for conversions
- [ ] Add integration tests for event flow
- [ ] Performance benchmarks for conversions
- [ ] Visual testing of UI interactions
- [ ] Verify no regressions

### Phase 6: Documentation (1-2 hours)

- [ ] Update main README
- [ ] Update code comments
- [ ] Create migration guide
- [ ] Document new patterns
- [ ] Remove deprecated code (after migration period)

## Quick Command Reference

```bash
# Build project
cmake --preset Debug
cmake --build --preset Debug

# Run tests
ctest --preset Debug --output-on-failure

# Run specific test suite
ctest --preset Debug -R events

# Rebuild FlatBuffers
cmake --build --preset Debug --target flatbuffers_generation

# Run with verbose output
ctest --preset Debug -VV
```

## Critical Code Patterns

### Creating Independent Enum

```cpp
// src/types/core/EventType.h
namespace steamrot {
    enum class EventType : uint64_t {
        None = 0,
        Test = 1,
        UserInput = 2,
        // ...
    };
}
```

### Conversion Functions

```cpp
// src/events/event_type_conversion.cpp
std::expected<EventType, FailInfo>
ConvertFromFlatBuffers(EventTypeFbs fbs_type) {
    switch (fbs_type) {
        case EventTypeFbs::EVENT_NONE: return EventType::None;
        case EventTypeFbs::EVENT_TEST: return EventType::Test;
        // ...
    }
}
```

### Using Conversion

```cpp
// In event_factory.cpp
auto event_type_result = 
    event::ConvertFromFlatBuffers(packet_data->event_type());
if (!event_type_result.has_value()) {
    return std::unexpected(event_type_result.error());
}
event_packet.event_type = event_type_result.value();
```

### Event References

```flatbuffers
// src/types/flatbuffers/core/event_reference.fbs
table SubscriberRef {
    event_type: EventTypeFbs;
    has_trigger_data: bool;
    config_id: string;
}
```

## Common Pitfalls

### ❌ Don't: Include FlatBuffers in Runtime Types

```cpp
// BAD
#include "events_generated.h"
struct EventPacket { ... };
```

### ✅ Do: Keep Types Independent

```cpp
// GOOD
#include "EventType.h"
struct EventPacket { ... };
```

### ❌ Don't: Embed Full Data in Cross-Subsystem Schemas

```flatbuffers
// BAD
include "../events/event_packet_data.fbs";
table UIElementData {
    response_event_data: EventPacketData;
}
```

### ✅ Do: Use Lightweight References

```flatbuffers
// GOOD
include "../core/event_reference.fbs";
table UIElementData {
    response_event_ref: EventPacketRef;
}
```

### ❌ Don't: Scatter Conversion Logic

```cpp
// BAD - multiple factory files
event_factory.cpp
event_bus_conversion.cpp
subscriber_factory.cpp
```

### ✅ Do: Centralize in Configurator

```cpp
// GOOD - single configurator
FlatbuffersEventConfigurator {
    ConfigureEventPacket();
    ConfigureSubscriber();
    ConfigureEventBus();
}
```

## Testing Checklist

Before marking a phase complete:

- [ ] Code compiles without errors
- [ ] All existing tests pass
- [ ] New tests added for changes
- [ ] No new compiler warnings
- [ ] Code follows style guide
- [ ] Documentation updated
- [ ] Changes reviewed

## Validation Commands

```bash
# Compile test
cmake --build --preset Debug 2>&1 | grep -i error

# Run unit tests
ctest --preset Debug -L unit

# Run integration tests
ctest --preset Debug -L integration

# Check for regressions
ctest --preset Debug --output-on-failure

# Verify no FlatBuffers in event types
grep -r "events_generated.h" src/types/events/
# Should return no results after Phase 1
```

## Time Estimates by Role

### Solo Developer
- **Full implementation:** 11-17 hours
- **Phase 1 (critical):** 2-3 hours
- **Phases 2-3 (important):** 4-6 hours
- **Phases 4-6 (beneficial):** 5-8 hours

### Team of 2
- **Full implementation:** 6-9 hours
- **Phase 1:** 1-2 hours (pair programming)
- **Phases 2-3:** 2-3 hours (split work)
- **Phases 4-6:** 3-4 hours (split work)

### Team of 3+
- **Full implementation:** 4-6 hours
- **Parallel phases:** Multiple phases simultaneously
- **Integration:** Plan coordination points

## When to Stop and Ask for Help

Stop and create an issue if:

1. Tests fail and you can't identify the cause
2. FlatBuffers compilation fails
3. Conversion logic shows performance issues
4. UI interactions break unexpectedly
5. Schema changes cause widespread breakage
6. Circular dependencies emerge

## Success Indicators

You'll know you're done when:

- [ ] `grep -r "events_generated.h" src/types/events/` returns nothing
- [ ] All FlatBuffers schemas use consistent naming
- [ ] Entity schemas don't include event schemas
- [ ] Single `FlatbuffersEventConfigurator` handles all conversions
- [ ] All tests pass (100% pass rate)
- [ ] No regression in functionality

## Quick Wins

If you have limited time, prioritize:

1. **Phase 1** - Biggest architectural improvement
2. **Phase 2** - Easy naming fixes, high readability impact
3. **Phase 4** - Consolidation improves maintainability

Phases 3, 5, 6 can be deferred if needed.

## Resources

### Primary Documents
- `EVENT_SYSTEM_COMPARISON.md` - Visual diagrams
- `EVENT_SYSTEM_ANALYSIS.md` - Technical analysis
- `EVENT_SYSTEM_REFACTORING_PLAN.md` - Detailed steps

### Code References
- Component pattern: `src/types/components/`, `src/entity/FlatbuffersEntityConfigurator.*`
- Current events: `src/types/events/`, `src/events/`
- FlatBuffers schemas: `src/types/flatbuffers/events/`

### External Links
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [FlatBuffers Documentation](https://flatbuffers.dev/)
- [std::expected Reference](https://en.cppreference.com/w/cpp/utility/expected)

## Need More Detail?

This guide gives you the essentials. For:
- **Architecture understanding:** Read EVENT_SYSTEM_COMPARISON.md
- **Problem analysis:** Read EVENT_SYSTEM_ANALYSIS.md
- **Implementation details:** Read EVENT_SYSTEM_REFACTORING_PLAN.md
- **Code examples:** All three documents have extensive examples

## Questions?

Common questions answered in full docs:
- "Why decouple?" → See Analysis document
- "How exactly do I...?" → See Refactoring Plan
- "What does it look like?" → See Comparison document
- "What if it breaks?" → See Rollback Plan in Refactoring document

---

**Last Updated:** 2026-01-13  
**Status:** Ready for implementation  
**Next Action:** Review with team or begin Phase 1
