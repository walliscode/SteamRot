# Event System Architecture Analysis

**Date**: 2026-02-12  
**Status**: Analysis Complete  
**Purpose**: Evaluate the current EventPacket/EventType system design for extensibility, maintainability, and scalability

---

## Executive Summary

The SteamRot event system uses a centralized event bus pattern with type-safe events and subscriber-based dispatch. While functional for current needs, the system shows signs of ad-hoc growth and has several architectural concerns around extensibility, type safety, and communication patterns.

**Key Findings**:
- ✅ Core event bus pattern is solid
- ✅ Subscriber system provides good decoupling
- ⚠️ EventData variant requires code changes for each new type
- ⚠️ Limited support for complex multi-part data
- ⚠️ No structured entity-to-entity communication pattern
- ⚠️ Dual enum system (native + FlatBuffers) creates maintenance burden

---

## Current Architecture

### 1. Core Components

#### EventPacket Structure
```cpp
struct EventPacket {
  EventType event_type{EventType::NONE};        // Enum-based type
  EventData event_data{std::monostate{}};       // Variant payload
  uuids::uuid event_id;                         // Unique identifier
  uuids::uuid source_id;                        // Event origin
  uint8_t event_lifetime{1};                    // Time-to-live in ticks
};
```

**Design**: Simple POD-like struct with UUID-based tracking and TTL management.

#### EventType Enum
```cpp
enum class EventType : uint64_t {
  NONE, TEST, USER_INPUT, TOGGLE_UI, 
  CHANGE_SCENE, QUIT_GAME, TOGGLE_DROPDOWN, LOGIC_TOGGLE
};
```

**Current Types**: 8 types (including NONE and TEST)
- **System Events**: USER_INPUT, CHANGE_SCENE, QUIT_GAME
- **UI Events**: TOGGLE_UI, TOGGLE_DROPDOWN  
- **Logic Events**: LOGIC_TOGGLE

#### EventData Variant
```cpp
using EventData = std::variant<
  std::monostate,           // Empty/no data
  UserInputBitset,          // Keyboard/mouse input
  SceneChangePacket,        // Scene transition data
  UserInterfaceName,        // UI element identifier
  ToggleName                // Generic toggle identifier
>;
```

**Design**: Closed variant set requiring code modifications for new types.

### 2. Event Flow Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     Event Generation                         │
│  (SFML Events, UI Actions, Logic Classes)                   │
└───────────────────────┬─────────────────────────────────────┘
                        │ AddEvent()
                        ▼
┌─────────────────────────────────────────────────────────────┐
│              Waiting Room Event Bus                          │
│         (Collects events during frame)                       │
└───────────────────────┬─────────────────────────────────────┘
                        │ ProcessWaitingRoomEventBus()
                        ▼
┌─────────────────────────────────────────────────────────────┐
│               Global Event Bus                               │
│         (Active events, TTL managed)                         │
└───────────┬───────────────────────────┬─────────────────────┘
            │                           │
            │ UpdateSubscribers()       │ TickGlobalEventBus()
            ▼                           ▼
┌─────────────────────┐      ┌──────────────────────┐
│  Subscriber Updates │      │  Lifetime Management │
│  (Pattern Matching) │      │  (Decrement + Clean) │
└─────────────────────┘      └──────────────────────┘
```

**Two-Phase Design**:
1. **Collection Phase**: Events added to waiting room
2. **Processing Phase**: Moved to global bus, subscribers updated, lifetimes managed

### 3. Subscriber Pattern

```cpp
struct Subscriber {
  bool m_active{false};
  EventType m_trigger_event_type{EventType::NONE};
  std::optional<EventData> m_trigger_event_data{std::nullopt};
  std::optional<EventData> m_received_event_data{std::nullopt};
};
```

**Features**:
- **Type Matching**: Events filtered by EventType
- **Data Matching**: Optional exact/subset matching for EventData
- **Activation Model**: Flag-based, consumers check `m_active`
- **Data Access**: Received event data stored for consumer processing

**Registration**: Subscribers register via EventHandler, stored by EventType in hash map.

### 4. Data Flow Examples

#### Example 1: UI Button Click
```
User Click → SFML Event → EventHandler::HandleSFMLEvents() →
  EventPacket(USER_INPUT, UserInputBitset) → Waiting Room →
  Global Bus → UI Collision Logic → Button Subscriber Activated →
  ProcessButtonElementActions() → New EventPacket(TOGGLE_UI) → Cycle
```

#### Example 2: Scene Change
```
Button Event → EventPacket(CHANGE_SCENE, SceneChangePacket) →
  SceneManager Subscriber → Active → SceneManager Logic →
  Scene Transition
```

---

## Strengths

### 1. ✅ Clean Separation of Concerns
- Events describe **what happened**, not **what to do**
- Subscribers decouple event generation from handling
- No direct coupling between event producers and consumers

### 2. ✅ Lifetime Management
- TTL system prevents event accumulation
- Two-phase bus prevents mid-frame race conditions
- Clear ownership model (value semantics)

### 3. ✅ UUID Tracking
- Events and sources uniquely identified
- Enables debugging and tracing
- Potential for event causality tracking

### 4. ✅ Pattern Matching
- Type-based filtering efficient via hash map
- Optional data filtering for specialized subscriptions
- Subset matching for UserInputBitset (bitwise operations)

### 5. ✅ Integration with FlatBuffers
- Event data serializable for testing
- Schema-driven data validation
- Cross-language potential (though not currently used)

---

## Weaknesses and Pain Points

### 1. ⚠️ Limited Extensibility

**Problem**: Adding new event types requires changes in multiple files:
```
1. EventType.h          - Add enum value + EnumNameEventType()
2. EventPacket.h        - Add type to EventData variant
3. events.fbs           - Add to EventTypeFbs + EventDataData union
4. event_factory.cpp    - Add conversion case
5. event_type_conversion.cpp - Add enum conversion
```

**Impact**: 
- High friction for adding events
- Easy to forget updates in one location
- Brittle maintenance
- Discourages experimentation with new event types

**Example Addition Flow**:
```cpp
// 1. EventType.h
enum class EventType : uint64_t {
  // ... existing ...
  NEW_EVENT_TYPE  // Add here
};

// 2. EventPacket.h
struct NewEventData { /* fields */ };
using EventData = std::variant<
  // ... existing ...
  NewEventData  // Add here
>;

// 3-5. Repeat in FlatBuffers, factory, conversion...
```

### 2. ⚠️ Dual Enum System

**Problem**: Two parallel enum systems must stay synchronized:
- `EventType` (native C++ enum)
- `EventTypeFbs` (FlatBuffers enum)

**Issues**:
- Manual conversion functions required
- No compile-time enforcement of sync
- Naming inconsistencies (TOGGLE_UI vs EVENT_TOGGLE_UI)
- Extra boilerplate in conversion layer

**Current Overhead**:
```cpp
// Native
enum class EventType { TOGGLE_UI };

// FlatBuffers
enum EventTypeFbs { EVENT_TOGGLE_UI };

// Conversion
std::expected<EventType, FailInfo>
ConvertEventTypeFbsToEventType(EventTypeFbs fbs_type) {
  switch (fbs_type) {
    case EventTypeFbs_EVENT_TOGGLE_UI: return EventType::TOGGLE_UI;
    // ... manual case for each type
  }
}
```

### 3. ⚠️ Awkward Multi-Part Data

**Problem**: Some events need multiple pieces of related data.

**Current Workarounds**:
1. **Struct Wrappers**: `SceneChangePacket` = `std::pair<optional<uuid>, SceneType>`
2. **String-Based**: `UserInterfaceName` and `ToggleName` are just strings
3. **Bitsets**: `UserInputBitset` packs multiple inputs

**Limitations**:
- No standard pattern for complex event data
- String-based identifiers lose type safety
- Difficult to pass structured entity data

**Example Need**:
```cpp
// Want: Entity X wants to notify Entity Y about complex state change
// Current: Limited to predefined structs or string-based identifiers

// Desired event data (not currently possible without new struct):
struct EntityInteractionData {
  uuids::uuid source_entity;
  uuids::uuid target_entity;
  InteractionType type;
  std::vector<ResourceChange> changes;
  std::optional<Position> location;
};
```

### 4. ⚠️ Entity-to-Entity Communication

**Problem**: No clear pattern for entities to communicate via events.

**Current State**:
- Events are mostly system-level (scene changes, UI toggles)
- No standard way for entity X to signal entity Y
- Logic classes check global event bus, not entity-specific queues

**Gap**:
```cpp
// Entities don't have:
- Event mailboxes (per-entity queues)
- Targeting mechanisms (send to specific entity)
- Request-response patterns
- Event composition (complex multi-step workflows)
```

**Workaround Example**:
```cpp
// Current: Global broadcast with string matching
EventPacket event(EventType::LOGIC_TOGGLE, ToggleName("specific_action"), 1);
event.source_id = entity_uuid;
event_handler.AddEvent(event);

// Receiver must check source_id manually
if (subscriber->m_active && 
    event.source_id == expected_entity_id) {
  // Process...
}
```

### 5. ⚠️ String-Based Identifiers

**Problem**: `UserInterfaceName` and `ToggleName` are thin wrappers around `std::string`.

**Issues**:
- No compile-time validation
- Typos cause silent failures
- Hard to refactor (find all string usages)
- No autocompletion or IDE support

**Current Usage**:
```cpp
// EventPacket.h
struct ToggleName : std::string {
  ToggleName(const std::string &str) : std::string(str) {}
};

// GrimoireMachinaActionLogic.cpp
if (toggle_name == "initiate_machina_form_scaffold") {  // Magic string!
  // ...
}
```

**Risk**: Easy to mistype, difficult to track all uses.

### 6. ⚠️ Subscriber Lifecycle Management

**Problem**: Subscribers use `weak_ptr` but cleanup is manual.

**Current**:
- `CleanExpiredSubscribers()` called once per frame
- Gap between expiration and cleanup
- Potential for processing expired subscribers

**Code**:
```cpp
// EventHandler.cpp
void UpdateSubscriber(std::weak_ptr<Subscriber> &subscriber, ...) {
  auto locked_subscriber = subscriber.lock();
  if (!locked_subscriber) return;  // Already handles expiration
  // But expired subscribers still in register until next cleanup
}
```

### 7. ⚠️ No Event Prioritization

**Problem**: All events processed in insertion order.

**Missing Features**:
- Priority queues
- Event coalescing (merge similar events)
- Event cancellation
- Conditional processing

---

## Extensibility Analysis

### Adding a New Event Type: Step-by-Step Impact

**Scenario**: Add `ENTITY_DAMAGE` event with damage amount, attacker ID, target ID.

#### Required Changes:

1. **EventType.h** (1 location)
```cpp
enum class EventType : uint64_t {
  // ...
  ENTITY_DAMAGE  // Add
};

inline std::string EnumNameEventType(EventType type) {
  // Add case
}
```

2. **EventPacket.h** (2 locations)
```cpp
struct EntityDamageData {
  uuids::uuid attacker_id;
  uuids::uuid target_id;
  float damage_amount;
};

using EventData = std::variant<
  // ...
  EntityDamageData  // Add to variant
>;
```

3. **events.fbs** (3 locations)
```fbs
enum EventTypeFbs {
  // ...
  EVENT_ENTITY_DAMAGE  // Add
}

table EntityDamageDataFbs {
  attacker_id: string;
  target_id: string;
  damage_amount: float;
}

union EventDataData {
  // ...
  EntityDamageDataFbs  // Add
}
```

4. **event_type_conversion.cpp** (1 location)
```cpp
std::expected<EventType, FailInfo>
ConvertEventTypeFbsToEventType(EventTypeFbs fbs_type) {
  // Add case for EVENT_ENTITY_DAMAGE
}
```

5. **event_factory.h** (1 location)
```cpp
std::expected<EntityDamageData, FailInfo>
CreateEntityDamageData(const EntityDamageDataFbs &data);
```

6. **event_factory.cpp** (2 locations)
```cpp
std::expected<EntityDamageData, FailInfo>
CreateEntityDamageData(const EntityDamageDataFbs &data) {
  // Implement conversion
}

std::expected<EventData, FailInfo>
CreateEventData(const EventDataData data_type, const void *data) {
  // Add case for EntityDamageDataFbs
}
```

**Total Impact**: 10 locations across 6 files

**Risk**: Easy to miss one location, causing runtime errors or test failures.

### Comparison: Alternative Architectures

#### Option A: Type-Erased Events (Current: No)
```cpp
// Could reduce variant updates
struct EventPacket {
  EventType type;
  std::any data;  // or std::shared_ptr<void>
};
```
**Trade-off**: More flexible, but loses type safety.

#### Option B: Event Registry Pattern (Current: No)
```cpp
// Register event types at runtime
EventRegistry::Register<EntityDamageData>("ENTITY_DAMAGE");
```
**Trade-off**: More extensible, but loses compile-time checks.

#### Option C: Template-Based Events (Current: No)
```cpp
template<typename T>
struct Event {
  T data;
  // Common fields
};
```
**Trade-off**: Type-safe and extensible, but complicates subscriber registration.

---

## Multi-Part Data Handling

### Current Patterns

#### Pattern 1: Struct Wrappers
```cpp
// SceneChangePacket: Multiple related fields
using SceneChangePacket = std::pair<std::optional<uuids::uuid>, SceneType>;

// Access
if (auto* packet = std::get_if<SceneChangePacket>(&event.event_data)) {
  auto entity_id = packet->first;
  auto scene_type = packet->second;
}
```

**Pros**: Type-safe, clear semantics
**Cons**: Requires new struct for each multi-part event

#### Pattern 2: String Encoding
```cpp
// UserInterfaceName: Simple string wrapper
struct UserInterfaceName : std::string { };

// Could abuse with encoding, but not currently done:
// "entity_id:action:param1:param2"  // BAD!
```

**Pros**: Flexible
**Cons**: No type safety, error-prone, parsing required

#### Pattern 3: Bitsets
```cpp
// UserInputBitset: Multiple inputs in single bitset
struct UserInputBitset : public std::bitset<kTotalBits> { };
```

**Pros**: Efficient, supports subset matching
**Cons**: Limited to boolean flags, complex encoding

### Limitations for Complex Data

**Example**: Crafting system interaction
```cpp
// Want to communicate:
// - Source entity (crafter)
// - Target entity (workbench)
// - Recipe ID
// - Input items (vector of item IDs + quantities)
// - Output item
// - Duration

// Current: Would need new struct type
struct CraftingInteractionData {
  uuids::uuid crafter_id;
  uuids::uuid workbench_id;
  std::string recipe_id;
  std::vector<std::pair<std::string, int>> inputs;
  std::string output_item_id;
  float duration_seconds;
};
// Then add to variant, FlatBuffers, etc.
```

**Challenge**: Each new game interaction needs a new struct type, perpetuating the extensibility problem.

### Potential Improvements

#### Option 1: Nested Variants
```cpp
// EventData contains variants, which contain variants
struct EntityInteractionData {
  uuids::uuid source;
  uuids::uuid target;
  std::variant<
    CombatData,
    TradeData,
    DialogueData
  > interaction_type;
};
```
**Pros**: Structured hierarchy
**Cons**: Still requires new types, complex pattern matching

#### Option 2: Property Bags
```cpp
struct EventPacket {
  EventType type;
  std::unordered_map<std::string, std::any> properties;
};
```
**Pros**: Infinitely flexible
**Cons**: No type safety, runtime errors, slow

#### Option 3: FlatBuffers Tables as Data
```cpp
// Store FlatBuffers table directly
struct EventPacket {
  EventType type;
  std::vector<uint8_t> flatbuffer_data;  // Serialized table
};
```
**Pros**: Schema validation, flexible structure
**Cons**: Serialization overhead, less ergonomic

---

## Entity Communication Patterns

### Current State: Global Event Bus Only

**Architecture**:
```
┌─────────────────────────────────────────────┐
│         Global Event Bus                    │
│  (All events visible to all subscribers)    │
└─────────────────────────────────────────────┘
         │              │              │
         ▼              ▼              ▼
    Entity A       Entity B       Entity C
  (Subscribe to  (Subscribe to  (Subscribe to
   relevant       relevant       relevant
   EventTypes)    EventTypes)    EventTypes)
```

**Characteristics**:
- Broadcast-only model
- No direct entity addressing
- Filtering via EventType + optional data matching
- No guaranteed delivery or ordering

### Use Cases Analysis

#### Use Case 1: System-Level Events ✅
**Examples**: Scene changes, quit game, toggle UI
```cpp
// Works well: Global scope appropriate
EventPacket event(EventType::QUIT_GAME, std::monostate{}, 1);
event_handler.AddEvent(event);
```
**Verdict**: Current system is appropriate.

#### Use Case 2: UI Interactions ✅
**Examples**: Button clicks, dropdown toggles
```cpp
// Works: UI elements have subscribers
EventPacket event(EventType::TOGGLE_UI, UserInterfaceName("settings_panel"), 1);
event_handler.AddEvent(event);
```
**Verdict**: Adequate, though string-based targeting is fragile.

#### Use Case 3: Entity-to-Entity Messaging ⚠️
**Examples**: Combat, trading, dialogue
```cpp
// Current workaround: Use source_id + global bus
EventPacket event(EventType::LOGIC_TOGGLE, ToggleName("entity_action"), 1);
event.source_id = attacker_uuid;  // Identify sender
event_handler.AddEvent(event);

// Receiver must manually check:
if (event.source_id == target_uuid) {
  // Process
}
```
**Verdict**: Awkward, no direct targeting, inefficient filtering.

#### Use Case 4: Request-Response ❌
**Examples**: Inventory queries, status checks
```cpp
// Not currently supported
// Want: Entity A asks Entity B for data, expects reply
```
**Verdict**: Not possible without custom patterns.

#### Use Case 5: Multi-Step Workflows ❌
**Examples**: Quest progression, dialogue trees
```cpp
// Not currently supported
// Want: Event chains, conditional progression
```
**Verdict**: Would require building on top of event system.

### Gaps and Challenges

#### Gap 1: No Direct Addressing
```cpp
// Can't do:
event_handler.SendTo(target_entity_uuid, event);

// Must do:
event_handler.AddEvent(event);  // Broadcast
// Receiver filters manually
```

#### Gap 2: No Per-Entity Event Queues
```cpp
// Can't do:
auto entity_events = entity.GetPendingEvents();
for (auto& event : entity_events) { /* process */ }

// Must do:
for (auto& event : global_event_bus) {
  if (event.source_id == my_uuid || is_relevant(event)) {
    // Process
  }
}
```

#### Gap 3: No Event Composition
```cpp
// Can't do:
event_handler.Chain({event1, event2, event3});  // Sequential
event_handler.Parallel({event1, event2});       // Concurrent
```

### Potential Patterns for Entity Communication

#### Pattern A: Targeted Events (Requires Architecture Change)
```cpp
struct EventPacket {
  EventType type;
  EventData data;
  uuids::uuid source_id;
  std::optional<uuids::uuid> target_id;  // NEW: Direct addressing
  uint8_t lifetime;
};

// Usage
EventPacket event(...);
event.target_id = target_entity_uuid;
event_handler.AddEvent(event);

// EventHandler routes to entity-specific queue
```

**Pros**: Direct routing, efficient
**Cons**: Requires per-entity queues, architecture change

#### Pattern B: Event Channels (Moderate Change)
```cpp
enum class EventChannel {
  GLOBAL,     // Current behavior
  UI,         // UI-specific events
  COMBAT,     // Combat system
  INVENTORY,  // Inventory system
  // etc.
};

// Subscribers register to channels
subscriber->m_channels = {EventChannel::COMBAT, EventChannel::INVENTORY};
```

**Pros**: Better filtering, domain separation
**Cons**: Still broadcast within channel

#### Pattern C: Mediator Pattern (Architectural Change)
```cpp
class EntityMediator {
  void SendMessage(uuid from, uuid to, Message msg);
  std::vector<Message> GetMessages(uuid entity_id);
};

// Sits alongside event system for entity-specific comms
```

**Pros**: Clean separation, no impact on global events
**Cons**: Parallel system to maintain

---

## Recommendations

### Short-Term Improvements (Low Risk)

#### 1. Document Event Addition Workflow ⭐ High Priority
**Action**: Create `/documentation/workflows/ADDING_NEW_EVENTS.md`

**Content**:
- Step-by-step checklist for adding events
- Required file changes
- Testing requirements
- Common pitfalls

**Impact**: Reduces errors, improves onboarding.

#### 2. Enum Validation Utilities ⭐ High Priority
**Action**: Add compile-time checks for enum sync

```cpp
// event_type_validation.h
namespace steamrot::event::validation {
  // Static assert that all EventType values have FlatBuffers counterparts
  constexpr bool ValidateEventTypeSync() {
    // Use constexpr magic to check enum counts match
    return true;
  }
  static_assert(ValidateEventTypeSync(), 
                "EventType and EventTypeFbs out of sync!");
}
```

**Impact**: Catches enum desync at compile time.

#### 3. Event Type Registry ⭐ Medium Priority
**Action**: Create centralized event type definitions

```cpp
// event_registry.h
#define STEAMROT_EVENT_TYPES(X) \
  X(NONE, "None") \
  X(TEST, "Test") \
  X(USER_INPUT, "UserInput") \
  X(TOGGLE_UI, "ToggleUI") \
  // etc.

// Generate enums from macro
enum class EventType : uint64_t {
  #define ENUM_ENTRY(name, str) name,
  STEAMROT_EVENT_TYPES(ENUM_ENTRY)
  #undef ENUM_ENTRY
};

// Generate name function
inline std::string EnumNameEventType(EventType type) {
  #define CASE_ENTRY(name, str) case EventType::name: return str;
  switch (type) {
    STEAMROT_EVENT_TYPES(CASE_ENTRY)
    default: return "UNKNOWN";
  }
  #undef CASE_ENTRY
}
```

**Impact**: Single source of truth, easier maintenance.

#### 4. Strong Type Aliases for Identifiers ⭐ High Priority
**Action**: Replace string wrappers with type-safe identifiers

```cpp
// Instead of: struct ToggleName : std::string
// Use enum or strong typedef:

enum class ToggleId {
  InitiateMachinaFormScaffold,
  ClearMachinaFormScaffold,
  // Compile-time enumeration
};

// Or strong typedef with validation:
struct ToggleName {
  explicit ToggleName(std::string_view name);
  static std::optional<ToggleName> TryCreate(std::string_view name);
  std::string_view Get() const;
private:
  std::string m_name;
  static const std::unordered_set<std::string_view>& ValidNames();
};
```

**Impact**: Compile-time checks, better refactoring, IDE support.

#### 5. Event Builder Pattern ⭐ Low Priority
**Action**: Simplify event creation

```cpp
// event_builder.h
class EventBuilder {
public:
  EventBuilder& Type(EventType type);
  EventBuilder& Data(EventData data);
  EventBuilder& Lifetime(uint8_t ticks);
  EventBuilder& SourceId(uuids::uuid id);
  EventPacket Build();
};

// Usage
auto event = EventBuilder()
  .Type(EventType::TOGGLE_UI)
  .Data(UserInterfaceName("panel"))
  .Lifetime(1)
  .Build();
```

**Impact**: Cleaner creation code, fewer constructor overloads.

### Medium-Term Improvements (Moderate Risk)

#### 6. Event Channels ⭐ High Priority
**Action**: Add domain-specific event channels

```cpp
enum class EventChannel {
  SYSTEM,    // Scene, quit, etc.
  UI,        // All UI interactions
  ENTITY,    // Entity-to-entity
  LOGIC,     // Logic-specific
};

struct EventPacket {
  EventChannel channel{EventChannel::SYSTEM};  // NEW
  EventType type;
  EventData data;
  // ...
};

// Subscribers filter by channel
subscriber->m_channels = {EventChannel::UI, EventChannel::ENTITY};
```

**Impact**: Better performance (less filtering), clearer domain separation.

#### 7. Direct Entity Addressing ⭐ Medium Priority
**Action**: Add optional target field to events

```cpp
struct EventPacket {
  std::optional<uuids::uuid> target_id;  // NEW: Optional direct target
  // ...
};

// EventHandler maintains per-entity queues
class EventHandler {
  std::unordered_map<uuids::uuid, EventBus> m_entity_event_queues;  // NEW
  EventBus m_global_event_bus;  // Existing
};
```

**Impact**: Enables entity-to-entity messaging, more efficient routing.

#### 8. Multi-Part Data Structs Library ⭐ Medium Priority
**Action**: Create common data structure library

```cpp
// event_data_common.h
namespace steamrot::event::data {

struct EntityTargetData {
  uuids::uuid source;
  uuids::uuid target;
};

struct ResourceChangeData {
  std::string resource_type;
  float amount;
  ChangeType change_type;  // ADD, REMOVE, SET
};

struct LocationData {
  sf::Vector2f position;
  std::optional<uuids::uuid> area_id;
};

// Compose for complex events
struct EntityInteractionData {
  EntityTargetData entities;
  std::vector<ResourceChangeData> resources;
  std::optional<LocationData> location;
};

}  // namespace
```

**Impact**: Reusable building blocks, consistent patterns.

### Long-Term Architectural Considerations (High Risk)

#### 9. Event System V2: Hybrid Approach ⭐ Long-Term
**Vision**: Maintain current system for global events, add targeted messaging for entities.

```cpp
// Dual system:
// 1. EventHandler (global) - existing, for system-wide events
// 2. EntityMessenger (targeted) - new, for entity-to-entity

class EntityMessenger {
public:
  void SendMessage(uuids::uuid from, uuids::uuid to, Message msg);
  std::vector<Message> GetMessages(uuids::uuid entity);
  void ClearMessages(uuids::uuid entity);
};

class GameEngine {
  EventHandler m_event_handler;        // System events
  EntityMessenger m_entity_messenger;  // Entity messages
};
```

**Impact**: Maintains backward compatibility, adds new capabilities.

#### 10. Schema-Driven Event System ⭐ Long-Term
**Vision**: Define all events in schemas, generate C++ code.

```yaml
# events.yaml
events:
  - name: EntityDamage
    type: ENTITY_DAMAGE
    channel: ENTITY
    data:
      - attacker_id: uuid
      - target_id: uuid
      - damage: float
      - damage_type: DamageType
```

**Generate**: Enums, structs, factories, tests automatically.

**Impact**: Eliminates manual sync issues, reduces boilerplate.

#### 11. Reactive Streams for Events ⭐ Long-Term
**Vision**: Treat events as streams with operators (filter, map, reduce).

```cpp
// RxCpp or similar
event_stream
  .filter([](auto& e) { return e.type == EventType::ENTITY_DAMAGE; })
  .map([](auto& e) { return ProcessDamage(e); })
  .subscribe([](auto& result) { ApplyResult(result); });
```

**Impact**: Declarative event handling, composable logic.

---

## Migration Strategies

### Strategy 1: Incremental Enhancement (Recommended)
**Approach**: Improve existing system piece by piece.

**Phase 1** (1-2 weeks):
- Add event registry macro
- Create documentation
- Add enum validation
- Introduce strong types for identifiers

**Phase 2** (2-4 weeks):
- Add event channels
- Implement direct addressing (optional field)
- Create common data structs library

**Phase 3** (1-2 months):
- Add EntityMessenger for targeted communication
- Refactor existing entity interactions to use new system
- Maintain EventHandler for global events

**Risks**: Low. Each phase is backward compatible.

### Strategy 2: Parallel Development
**Approach**: Build V2 alongside V1, migrate gradually.

**Steps**:
1. Implement EventSystemV2 with all desired features
2. Support both systems in GameEngine
3. Migrate events one domain at a time (UI → Entity → System)
4. Deprecate V1 once all events migrated

**Risks**: Medium. Requires maintaining two systems temporarily.

### Strategy 3: Big Bang Rewrite
**Approach**: Redesign and replace entire system.

**Not Recommended**: High risk, long development time, breaks all existing code.

---

## Testing Considerations

### Current Test Coverage
- ✅ EventHandler tests (subscriber registration, bus operations)
- ✅ EventPacket equality matchers
- ✅ Event type conversion tests
- ✅ Data factory tests (FlatBuffers → native)
- ⚠️ Limited integration tests for event flow
- ❌ No performance benchmarks

### Recommended Testing Additions

#### 1. Event Flow Integration Tests
```cpp
TEST_CASE("UI button click generates scene change event") {
  // End-to-end: SFML event → USER_INPUT → UI Logic → CHANGE_SCENE → Scene Manager
}
```

#### 2. Enum Synchronization Tests
```cpp
TEST_CASE("EventType and EventTypeFbs have same count") {
  REQUIRE(GetEventTypeCount() == GetEventTypeFbsCount());
}
```

#### 3. Performance Benchmarks
```cpp
BENCHMARK("Process 1000 events with 100 subscribers") {
  // Measure event bus throughput
}
```

#### 4. Event Data Validation Tests
```cpp
TEST_CASE("Invalid UUIDs in SceneChangePacket are rejected") {
  // Test error handling in data factories
}
```

---

## Conclusion

### Summary of Findings

The SteamRot event system is **functionally sound** for current needs but shows **architectural growing pains**. The core event bus and subscriber patterns are solid, but extensibility is hampered by:

1. **Manual synchronization** across multiple files when adding events
2. **Limited support** for complex, multi-part data
3. **No structured approach** to entity-to-entity communication
4. **Dual enum system** creating maintenance overhead

### Prioritized Action Plan

#### Immediate (Do Now):
1. ✅ **Document event addition workflow** - Prevent errors
2. ✅ **Add enum validation** - Catch sync issues early
3. ✅ **Replace string identifiers** - Improve type safety

#### Near-Term (1-2 Months):
4. ✅ **Implement event channels** - Better domain separation
5. ✅ **Add direct entity addressing** - Enable targeted messaging
6. ✅ **Create common data structs** - Reusable building blocks

#### Long-Term (3-6 Months):
7. ⏳ **Evaluate EntityMessenger** - Parallel system for entity comms
8. ⏳ **Consider schema-driven generation** - Reduce boilerplate

### Final Recommendation

**Proceed with incremental enhancement** (Strategy 1). The current system is a solid foundation. Rather than a disruptive rewrite, evolve it through targeted improvements. Focus on:

- **Documentation and tooling** to reduce friction
- **Type safety improvements** to catch errors earlier
- **Selective additions** (channels, addressing) to enable new use cases
- **Backward compatibility** to avoid breaking existing code

This approach delivers value quickly while managing risk and maintaining development velocity.

---

## Appendix A: Event Type Catalog

### System Events
| Event Type | Data Type | Purpose | Lifetime | Scope |
|------------|-----------|---------|----------|-------|
| NONE | std::monostate | Default/invalid | N/A | - |
| TEST | std::monostate | Testing | 1 | Test |
| QUIT_GAME | std::monostate | Application exit | 1 | Global |
| CHANGE_SCENE | SceneChangePacket | Scene transitions | 1 | Global |

### UI Events
| Event Type | Data Type | Purpose | Lifetime | Scope |
|------------|-----------|---------|----------|-------|
| USER_INPUT | UserInputBitset | Keyboard/mouse input | 1 | Global |
| TOGGLE_UI | UserInterfaceName | Show/hide UI elements | 1 | UI |
| TOGGLE_DROPDOWN | UserInterfaceName | Dropdown interactions | 1 | UI |

### Logic Events
| Event Type | Data Type | Purpose | Lifetime | Scope |
|------------|-----------|---------|----------|-------|
| LOGIC_TOGGLE | ToggleName | Generic logic triggers | 1 | Logic |

---

## Appendix B: Code Reference

### Key Files
- **Core Types**: `src/types/events/EventType.h`, `EventPacket.h`
- **Handler**: `src/events/EventHandler.{h,cpp}`
- **Factories**: `src/events/event_factory.{h,cpp}`
- **Conversions**: `src/events/event_type_conversion.{h,cpp}`
- **Schemas**: `src/types/flatbuffers/events/events.fbs`

### Test Files
- **Unit**: `tests/unit/types/events/`, `tests/unit/events/`
- **Matchers**: `tests/matchers/EventPacketEqualsMatcher.{h,cpp}`

---

**Document Status**: Complete  
**Last Updated**: 2026-02-12  
**Next Review**: After implementing short-term recommendations
