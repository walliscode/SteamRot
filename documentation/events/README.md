# Event System Documentation - Quick Start

**Purpose**: Central index for all event system documentation  
**Last Updated**: 2026-02-12

---

## Overview

The SteamRot event system uses a centralized event bus pattern with subscriber-based dispatch. This documentation provides analysis of the current implementation and practical guidance for working with events.

---

## Documentation Index

### 1. Architecture Analysis (Deep Dive)
**File**: `documentation/architecture/EVENT_SYSTEM_ANALYSIS.md`

**Contents**:
- Complete architecture overview
- Current strengths and weaknesses
- Extensibility analysis
- Multi-part data handling patterns
- Entity communication patterns
- Detailed recommendations (short, medium, and long-term)
- Migration strategies
- Testing considerations

**Read this if**: You want to understand the event system design, its limitations, or plan architectural improvements.

### 2. Adding New Events (Practical Guide)
**File**: `documentation/workflows/ADDING_NEW_EVENTS.md`

**Contents**:
- Step-by-step workflow for adding events
- 12-step checklist covering all required changes
- Code examples for each step
- Testing guidelines
- Common patterns and troubleshooting

**Read this if**: You need to add a new event type to the codebase.

---

## Quick Reference

### Current Event Types

| Event Type | Data Type | Purpose |
|------------|-----------|---------|
| NONE | std::monostate | Default/invalid |
| TEST | std::monostate | Testing |
| USER_INPUT | UserInputBitset | Keyboard/mouse input |
| TOGGLE_UI | UserInterfaceName | Show/hide UI elements |
| CHANGE_SCENE | SceneChangePacket | Scene transitions |
| QUIT_GAME | std::monostate | Application exit |
| TOGGLE_DROPDOWN | UserInterfaceName | Dropdown interactions |
| LOGIC_TOGGLE | ToggleName | Generic logic triggers |

### Key Files to Know

**Core Types**:
- `src/types/events/EventType.h` - Native event type enum
- `src/types/events/EventPacket.h` - Event packet structure and data variant
- `src/types/events/Subscriber.h` - Subscriber struct

**Event Handler**:
- `src/events/EventHandler.{h,cpp}` - Event bus management

**Data Conversion**:
- `src/events/event_factory.{h,cpp}` - FlatBuffers to native conversion
- `src/events/event_type_conversion.{h,cpp}` - Enum conversion

**Schemas**:
- `src/types/flatbuffers/events/events.fbs` - FlatBuffers event definitions

**Tests**:
- `tests/unit/types/events/` - Event type tests
- `tests/unit/events/` - Event handler tests

---

## Top Recommendations Summary

### Immediate Actions (High Priority)

#### 1. Use the Workflow Document ⭐
When adding new events, follow `ADDING_NEW_EVENTS.md` checklist to ensure all locations are updated.

#### 2. Consider String Identifier Alternatives ⭐
Current `ToggleName` and `UserInterfaceName` are just string wrappers. Consider:
- Using enums for compile-time validation
- Creating strong types with validation
- This prevents typos and improves refactoring

**Example Issue**:
```cpp
// Current: Easy to mistype
if (toggle_name == "initiate_machina_form_scaffold") { ... }

// Better: Compile-time checked
if (toggle_name == ToggleId::InitiateMachinaFormScaffold) { ... }
```

#### 3. Validate All FlatBuffers Data ⭐
Always check for null before accessing FlatBuffers fields:

```cpp
// BAD - Can segfault
auto name = data.name()->str();

// GOOD - Safe
if (data.name()) {
  auto name = data.name()->str();
}
```

### Near-Term Improvements (1-2 Months)

#### 4. Add Event Channels
Organize events by domain (SYSTEM, UI, ENTITY, LOGIC) for better filtering and performance.

#### 5. Add Direct Entity Addressing
Enable targeted events with optional `target_id` field for entity-to-entity communication.

#### 6. Create Common Data Structs Library
Build reusable data structures for common event patterns (entity targets, resource changes, locations).

### Long-Term Considerations (3-6 Months)

#### 7. Evaluate EntityMessenger
Consider separate system for direct entity-to-entity messaging alongside global event bus.

#### 8. Schema-Driven Generation
Define events in YAML/JSON, generate C++ code automatically to eliminate manual sync issues.

---

## Common Pain Points and Solutions

### Pain Point: Adding Events is Tedious
**Issue**: Must update 6 files in 10 locations for each new event.

**Solution**: 
- Use the `ADDING_NEW_EVENTS.md` checklist
- Consider future macro-based registry (Recommendation #3 in analysis)

### Pain Point: Events Feel "Clunky"
**Issue**: String-based identifiers, no type safety, easy to make mistakes.

**Solution**:
- Replace string wrappers with strong types (Recommendation #4)
- Add enum validation utilities (Recommendation #2)

### Pain Point: Entity-to-Entity Communication
**Issue**: No clean way for entities to send messages to each other.

**Solution**:
- Add optional `target_id` field to EventPacket (Recommendation #7)
- Or create separate EntityMessenger system (Recommendation #9)

### Pain Point: Multi-Part Data
**Issue**: Need new struct for each complex event, perpetuating extensibility problem.

**Solution**:
- Create library of common data structures (Recommendation #8)
- Use composition to build complex events from reusable pieces

---

## Architecture Decision: When to Add an Event?

### Use Events For:
✅ System-level state changes (scene transitions, quit)  
✅ User input that multiple systems care about  
✅ UI interactions that trigger logic  
✅ Cross-system notifications (damage, status effects)  
✅ When you need loose coupling between producer and consumer

### Don't Use Events For:
❌ Internal logic within a single system (use direct function calls)  
❌ High-frequency updates (every frame position updates)  
❌ Synchronous operations requiring immediate response  
❌ Simple getter/setter operations  
❌ When direct coupling is clearer and simpler

---

## Event System Workflow (High-Level)

```
1. Event Generated
   ├─ SFML input (HandleSFMLEvents)
   ├─ UI action (button click)
   ├─ Logic class (game event)
   └─ System (scene manager)
         ↓
2. Added to Waiting Room Bus
   └─ AddEvent() called
         ↓
3. Moved to Global Bus
   └─ ProcessWaitingRoomEventBus()
         ↓
4. Subscribers Updated
   └─ UpdateSubscribersFromGlobalEventBus()
   └─ Pattern matching: type + optional data
         ↓
5. Subscribers Processed
   └─ Logic classes check subscriber->m_active
   └─ Extract received_event_data
   └─ Execute domain logic
         ↓
6. Lifetime Management
   └─ TickGlobalEventBus()
   └─ Decrement lifetimes
   └─ Remove expired events
```

---

## Key Design Principles

### 1. Events Describe "What Happened"
Not "what to do". Events are facts, not commands.

**Good**: `ENTITY_DAMAGED` (fact)  
**Bad**: `DamageEntity` (command)

### 2. Loose Coupling via Subscribers
Event producers don't know about consumers. Subscribers register interest, consumers check activation.

### 3. TTL Prevents Event Accumulation
Default 1-tick lifetime. Events are ephemeral notifications, not persistent state.

### 4. Two-Phase Bus Prevents Race Conditions
Waiting room collects all events for a frame before processing. Prevents mid-frame issues.

### 5. Type Safety via Variants
`std::variant<...>` provides compile-time type checking. Use `std::holds_alternative` and `std::get` for safe access.

---

## Testing Guidelines

### Unit Tests
- Test data struct construction
- Test variant assignment and retrieval
- Test factory functions (success + error cases)
- Test enum conversions

### Integration Tests
- Test full event flow (generation → bus → subscriber → processing)
- Test subscriber activation and data extraction
- Test event lifetime management

### Pattern for Event Tests
```cpp
TEST_CASE("Event data construction", "[unit][EventName]") { ... }
TEST_CASE("Event in variant", "[unit][EventName]") { ... }
TEST_CASE("Factory success", "[unit][event_factory]") { ... }
TEST_CASE("Factory validation", "[unit][event_factory]") { ... }
```

---

## Migration Path (If Implementing Recommendations)

### Phase 1: Documentation & Safety (1-2 weeks)
- ✅ Complete - Created analysis and workflow docs
- Add enum validation utilities
- Replace string identifiers with strong types

### Phase 2: Extensibility (2-4 weeks)
- Add event channels for domain separation
- Implement optional direct addressing
- Create common data structures library

### Phase 3: Entity Communication (1-2 months)
- Evaluate EntityMessenger pattern
- Refactor entity interactions
- Maintain backward compatibility with global bus

---

## Questions and Answers

### Q: Should every state change be an event?
**A**: No. Use events for cross-system communication. Internal logic should use direct function calls.

### Q: What if I need to send complex, nested data?
**A**: Create a struct with the required fields, add to variant and FlatBuffers. See recommendations for common data structure library.

### Q: How do I target a specific entity with an event?
**A**: Currently, set `source_id` and have receiver check it. Future recommendation: add optional `target_id` field.

### Q: Can I extend EventData without modifying the variant?
**A**: No, `std::variant` is a closed set. This is a known limitation. See analysis document for alternatives (type erasure, registry pattern).

### Q: Why do we have two event enum systems?
**A**: One for C++ (`EventType`), one for FlatBuffers (`EventTypeFbs`). Enables serialization for testing and data-driven configuration. Recommendation: add validation to keep in sync.

### Q: What's the performance of the event system?
**A**: Good for typical game loads. O(1) subscriber lookup by type, O(n) event iteration. No profiling data yet. See recommendations for benchmarking.

---

## Future Considerations

### Schema-Driven Event Generation
Define events in a schema file, generate all boilerplate:

```yaml
events:
  - name: EntityDamage
    type: ENTITY_DAMAGE
    channel: ENTITY
    data:
      attacker_id: uuid
      target_id: uuid
      damage_amount: float
```

**Benefits**:
- Single source of truth
- No manual synchronization
- Auto-generated tests
- Reduced human error

### Reactive Event Streams
Treat events as streams with functional operators:

```cpp
event_stream
  .filter(is_damage_event)
  .map(calculate_damage)
  .subscribe(apply_damage);
```

**Benefits**:
- Declarative style
- Composable logic
- Testable transformations

### Event Sourcing
Store all events for replay and debugging:

```cpp
event_log.Store(event);
// Later: Replay all events to reconstruct state
```

**Benefits**:
- Debugging (replay game state)
- Analytics (what happened)
- Undo/redo functionality

---

## Getting Help

### Issues with Adding Events
1. Check `ADDING_NEW_EVENTS.md` workflow guide
2. Review troubleshooting section
3. Check existing event implementations for patterns

### Architectural Questions
1. Read `EVENT_SYSTEM_ANALYSIS.md` for design rationale
2. Review recommendations section for improvement ideas
3. Consider trade-offs documented in analysis

### Test Failures
1. Check factory function validation logic
2. Verify all locations in checklist updated
3. Ensure FlatBuffers headers regenerated (rebuild)

---

## Document Maintenance

**Last Review**: 2026-02-12  
**Next Review**: After implementing short-term recommendations  
**Owner**: Development team  
**Status**: Living document - update as event system evolves

---

## Related Documentation

- Project README.md - General project structure
- `documentation/workflows/ADDING_LOGIC.md` - Logic class workflow
- `documentation/configuration/CONTEXT_CONFIGURATION.md` - Context configuration

---

**Quick Links**:
- [Full Analysis](../architecture/EVENT_SYSTEM_ANALYSIS.md)
- [Adding Events Workflow](../workflows/ADDING_NEW_EVENTS.md)
- [GitHub Repository](https://github.com/walliscode/SteamRot)
