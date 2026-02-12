# Adding New Events to SteamRot

**Document Type**: Workflow Guide  
**Last Updated**: 2026-02-12  
**Prerequisites**: Understanding of C++ variants, FlatBuffers basics, SteamRot component architecture

---

## Quick Reference Checklist

When adding a new event type, you must update **6 files** in **10 locations**:

- [ ] 1. `src/types/events/EventType.h` - Add enum value (1 location)
- [ ] 2. `src/types/events/EventType.h` - Update `EnumNameEventType()` (1 location)
- [ ] 3. `src/types/events/EventPacket.h` - Define data struct if needed (1 location)
- [ ] 4. `src/types/events/EventPacket.h` - Add to `EventData` variant (1 location)
- [ ] 5. `src/types/flatbuffers/events/events.fbs` - Add to `EventTypeFbs` enum (1 location)
- [ ] 6. `src/types/flatbuffers/events/events.fbs` - Define FlatBuffers table (1 location)
- [ ] 7. `src/types/flatbuffers/events/events.fbs` - Add to `EventDataData` union (1 location)
- [ ] 8. `src/events/event_type_conversion.cpp` - Add conversion case (1 location)
- [ ] 9. `src/events/event_factory.h` - Declare factory function (1 location)
- [ ] 10. `src/events/event_factory.cpp` - Implement factory function (2 locations)
- [ ] 11. Write tests for new event type
- [ ] 12. Build and test locally

---

## Step-by-Step Workflow

### Step 1: Define Event Purpose and Data

Before writing code, answer:
- **What does this event represent?** (State change, user action, system notification)
- **What data does it carry?** (IDs, values, flags)
- **Who produces it?** (Logic class, UI, system)
- **Who consumes it?** (Specific entities, systems)
- **What is its lifetime?** (Default 1 tick, or longer?)

**Example**:
```
Event: ENTITY_DAMAGE
Purpose: Notify when an entity takes damage
Data: attacker_id (UUID), target_id (UUID), damage_amount (float), damage_type (enum)
Producer: Combat logic
Consumer: Damage display UI, health component
Lifetime: 1 tick (immediate processing)
```

### Step 2: Choose Event Type Name

**Naming Convention**:
- Use `SCREAMING_SNAKE_CASE`
- Be specific but concise
- Verb or noun phrase describing the event
- Avoid redundant "EVENT_" prefix (already in enum class)

**Good Examples**:
- `ENTITY_DAMAGE` - Clear, specific
- `INVENTORY_UPDATED` - Action-based
- `QUEST_COMPLETED` - State change

**Bad Examples**:
- `DAMAGE` - Too vague (UI damage? Entity damage?)
- `EVENT_ENTITY_DAMAGE_OCCURRED` - Redundant
- `DoEntityDamage` - Not an event name (use imperative verbs for functions)

### Step 3: Update EventType.h

**File**: `src/types/events/EventType.h`

#### Location 1: Add enum value

```cpp
enum class EventType : uint64_t {
  NONE = 0,
  TEST,
  USER_INPUT,
  TOGGLE_UI,
  CHANGE_SCENE,
  QUIT_GAME,
  TOGGLE_DROPDOWN,
  LOGIC_TOGGLE,
  ENTITY_DAMAGE  // ADD NEW EVENT HERE
};
```

**Notes**:
- Add at the end before closing brace
- No explicit value needed (auto-increments)
- Add a comment if the purpose is not obvious

#### Location 2: Update EnumNameEventType()

```cpp
inline std::string EnumNameEventType(EventType type) {
  switch (type) {
  case EventType::NONE:
    return "NONE";
  case EventType::TEST:
    return "TEST";
  // ... existing cases ...
  case EventType::ENTITY_DAMAGE:  // ADD CASE HERE
    return "ENTITY_DAMAGE";
  default:
    return "UNKNOWN";
  }
}
```

**Notes**:
- String should match enum name exactly
- Add before `default` case
- Used for debugging and logging

### Step 4: Define Event Data Structure (If Needed)

**File**: `src/types/events/EventPacket.h`

#### When to Add a Data Structure

**Add a struct if**:
- Event carries multiple fields
- Data has semantic meaning beyond primitives
- Data is complex (nested structures, vectors)

**Use existing types if**:
- Event has no data → use `std::monostate`
- Event carries a single simple identifier → consider reusing `UserInterfaceName` or `ToggleName`
- Event carries existing composite type → reuse (e.g., `SceneChangePacket`)

#### Location 3: Define the struct

```cpp
/////////////////////////////////////////////////
/// @brief Data payload for ENTITY_DAMAGE event
///
/// Contains information about a damage event between two entities.
/////////////////////////////////////////////////
struct EntityDamageData {
  uuids::uuid attacker_id;
  uuids::uuid target_id;
  float damage_amount{0.0f};
  DamageType damage_type{DamageType::Physical};  // If using enum
};
```

**Notes**:
- Place before the `EventData` variant definition
- Use descriptive field names
- Initialize all members with default values
- Add Doxygen-style comment
- Follow existing naming conventions (snake_case for members)

#### Location 4: Add to EventData variant

```cpp
using EventData =
    std::variant<std::monostate, UserInputBitset, SceneChangePacket,
                 UserInterfaceName, ToggleName, 
                 EntityDamageData>;  // ADD HERE
```

**Notes**:
- Add at the end of the variant list
- Keep formatting consistent (line breaks for readability)
- Order doesn't matter functionally, but keep alphabetical or grouped by category

### Step 5: Update FlatBuffers Schema

**File**: `src/types/flatbuffers/events/events.fbs`

#### Location 5: Add to EventTypeFbs enum

```fbs
enum EventTypeFbs: ulong (bit_flags) {
  EVENT_NONE = 0,
  EVENT_TEST,
  EVENT_USER_INPUT,
  EVENT_TOGGLE_UI,
  EVENT_CHANGE_SCENE,
  EVENT_QUIT_GAME,
  EVENT_TOGGLE_DROPDOWN,
  EVENT_TOGGLE_NAME,
  EVENT_ENTITY_DAMAGE  // ADD HERE
}
```

**Notes**:
- **Naming**: Add `EVENT_` prefix to match existing convention
- Values auto-increment, no need to specify
- Order should match native enum for clarity

#### Location 6: Define FlatBuffers table

```fbs
table EntityDamageDataFbs {
  attacker_id: string;    // UUIDs stored as strings in FlatBuffers
  target_id: string;
  damage_amount: float;
  damage_type: int;       // Enums stored as ints
}
```

**Notes**:
- Table name: `<StructName>Fbs` suffix
- Field names should match C++ struct (snake_case)
- UUID → `string` (FlatBuffers doesn't have UUID type)
- Enum → `int` or create separate FlatBuffers enum
- All fields optional by default in FlatBuffers

#### Location 7: Add to EventDataData union

```fbs
union EventDataData {
  UserInputBitsetData,
  SceneChangePacketData,
  UserInterfaceNameData,
  ToggleNameFbs,
  EntityDamageDataFbs  // ADD HERE
}
```

**Notes**:
- Add at the end of union
- No semicolons in FlatBuffers unions
- Name must match the table defined in step 6

### Step 6: Update Event Type Conversion

**File**: `src/events/event_type_conversion.cpp`

#### Location 8: Add conversion case

```cpp
std::expected<EventType, FailInfo>
ConvertEventTypeFbsToEventType(EventTypeFbs fbs_type) {
  switch (fbs_type) {
  case EventTypeFbs_EVENT_NONE:
    return EventType::NONE;
  case EventTypeFbs_EVENT_TEST:
    return EventType::TEST;
  // ... existing cases ...
  case EventTypeFbs_EVENT_ENTITY_DAMAGE:  // ADD CASE HERE
    return EventType::ENTITY_DAMAGE;
  default:
    return std::unexpected(
        FailInfo{FailMode::EnumValueNotHandled,
                 "Unsupported EventTypeFbs value in conversion"});
  }
}
```

**Notes**:
- Map FlatBuffers enum to native enum
- Add before `default` case
- Must match enum names exactly

### Step 7: Update Event Factory

#### Part A: Declare Factory Function

**File**: `src/events/event_factory.h`

#### Location 9: Add factory function declaration

```cpp
/////////////////////////////////////////////////
/// @brief Convert the flatbuffers EntityDamageDataFbs to EntityDamageData
///
/// @param data Data to convert
/// @return EntityDamageData on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<EntityDamageData, FailInfo>
CreateEntityDamageData(const EntityDamageDataFbs &data);
```

**Notes**:
- Function name: `Create<StructName>`
- Takes const reference to FlatBuffers table
- Returns `std::expected` with native struct or `FailInfo`
- Place with other factory function declarations

#### Part B: Implement Factory Function

**File**: `src/events/event_factory.cpp`

#### Location 10a: Implement factory function

```cpp
/////////////////////////////////////////////////
std::expected<EntityDamageData, FailInfo>
CreateEntityDamageData(const EntityDamageDataFbs &data) {
  
  EntityDamageData damage_data;

  // Validate and convert attacker_id (UUID)
  if (!data.attacker_id()) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "CreateEntityDamageData: attacker_id missing"});
  }
  if (!uuids::uuid::is_valid_uuid(data.attacker_id()->c_str())) {
    return std::unexpected(
        FailInfo{FailMode::InvalidUUID,
                 "CreateEntityDamageData: attacker_id is invalid UUID"});
  }
  damage_data.attacker_id = uuids::uuid::from_string(data.attacker_id()->c_str());

  // Validate and convert target_id (UUID)
  if (!data.target_id()) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "CreateEntityDamageData: target_id missing"});
  }
  if (!uuids::uuid::is_valid_uuid(data.target_id()->c_str())) {
    return std::unexpected(
        FailInfo{FailMode::InvalidUUID,
                 "CreateEntityDamageData: target_id is invalid UUID"});
  }
  damage_data.target_id = uuids::uuid::from_string(data.target_id()->c_str());

  // Direct field access (primitives don't need null checks)
  damage_data.damage_amount = data.damage_amount();
  damage_data.damage_type = static_cast<DamageType>(data.damage_type());

  return damage_data;
}
```

**Validation Rules**:
- **Strings**: Check with `if (!data.field())` before accessing
- **UUIDs**: Validate with `uuids::uuid::is_valid_uuid()`
- **Primitives** (int, float, bool): Access directly (no null check needed)
- **Vectors**: Check with `if (!data.field())` before iterating
- **Nested tables**: Check with `if (!data.field())` before accessing

**Error Handling**:
- Return `std::unexpected(FailInfo{...})` on validation failure
- Use descriptive error messages with function name
- Choose appropriate `FailMode` enum value

#### Location 10b: Add case to CreateEventData()

```cpp
std::expected<EventData, FailInfo>
CreateEventData(const EventDataData data_type, const void *data) {

  switch (data_type) {
  case EventDataData::EventDataData_UserInputBitsetData: {
    // ... existing cases ...
  }
  
  case EventDataData::EventDataData_EntityDamageDataFbs: {  // ADD CASE HERE
    // Validate data pointer before dereferencing
    if (!data) {
      return std::unexpected(
          FailInfo{FailMode::NullPointer,
                   "CreateEventData: EntityDamageDataFbs pointer is null"});
    }

    // Cast data to EntityDamageDataFbs
    auto damage_data_fbs = static_cast<const EntityDamageDataFbs *>(data);

    // Convert to EntityDamageData
    auto damage_data_result = CreateEntityDamageData(*damage_data_fbs);
    if (!damage_data_result.has_value())
      return std::unexpected(damage_data_result.error());

    return damage_data_result.value();
  }

  case EventDataData::EventDataData_NONE: {
    return std::monostate();
  }

  default:
    return std::unexpected(
        FailInfo{FailMode::EnumValueNotHandled,
                 "CreateEventData: EventDataData type not handled."});
  }
}
```

**Notes**:
- Add before `default` case
- Follow existing pattern: null check, cast, convert, return
- Use your factory function created in location 10a

### Step 8: Build Project

**Note**: As per project guidelines, agents do NOT build. This step is for reference on how users will build locally.

```bash
# Rebuild to generate FlatBuffers headers
cmake --build --preset Debug

# Check for compilation errors
# Fix any issues found
```

**Common Build Errors**:
- Missing include: Add to relevant .cpp/.h file
- FlatBuffers header not found: Rebuild (headers generated from .fbs files)
- Variant type error: Check EventData variant includes your struct
- Enum case missing: Check all switch statements updated

### Step 9: Write Tests

Create test file: `tests/unit/types/events/<EventName>.test.cpp`

**Example**: `tests/unit/types/events/EntityDamageData.test.cpp`

```cpp
#include "EventPacket.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("EntityDamageData default construction", "[unit][EntityDamageData]") {
  steamrot::EntityDamageData data;
  
  REQUIRE(data.damage_amount == 0.0f);
  // UUIDs are default-constructed (valid but unique)
}

TEST_CASE("EntityDamageData in EventData variant", "[unit][EntityDamageData]") {
  steamrot::EntityDamageData damage{};
  damage.damage_amount = 50.0f;
  
  steamrot::EventData event_data = damage;
  
  REQUIRE(std::holds_alternative<steamrot::EntityDamageData>(event_data));
  auto& extracted = std::get<steamrot::EntityDamageData>(event_data);
  REQUIRE(extracted.damage_amount == 50.0f);
}

TEST_CASE("EventPacket with ENTITY_DAMAGE type", "[unit][EventPacket]") {
  steamrot::EntityDamageData damage{};
  damage.damage_amount = 75.0f;
  
  steamrot::EventPacket packet(steamrot::EventType::ENTITY_DAMAGE, damage, 1);
  
  REQUIRE(packet.event_type == steamrot::EventType::ENTITY_DAMAGE);
  REQUIRE(std::holds_alternative<steamrot::EntityDamageData>(packet.event_data));
}
```

**Test Coverage**:
- Default construction
- Variant assignment and retrieval
- EventPacket creation
- Factory function (see below)

#### Factory Function Tests

Create test file: `tests/unit/events/event_factory_<EventName>.test.cpp`

```cpp
#include "event_factory.h"
#include "events_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <flatbuffers/flatbuffers.h>

TEST_CASE("CreateEntityDamageData success", "[unit][event_factory]") {
  flatbuffers::FlatBufferBuilder builder;
  
  auto attacker_id = builder.CreateString("123e4567-e89b-12d3-a456-426614174000");
  auto target_id = builder.CreateString("123e4567-e89b-12d3-a456-426614174001");
  
  auto damage_fbs = steamrot::CreateEntityDamageDataFbs(
      builder, attacker_id, target_id, 100.0f, 1);
  
  builder.Finish(damage_fbs);
  
  auto damage_data_fbs = flatbuffers::GetRoot<steamrot::EntityDamageDataFbs>(
      builder.GetBufferPointer());
  
  auto result = steamrot::event::CreateEntityDamageData(*damage_data_fbs);
  
  REQUIRE(result.has_value());
  REQUIRE(result.value().damage_amount == 100.0f);
}

TEST_CASE("CreateEntityDamageData missing attacker_id", "[unit][event_factory]") {
  flatbuffers::FlatBufferBuilder builder;
  
  auto target_id = builder.CreateString("123e4567-e89b-12d3-a456-426614174001");
  
  auto damage_fbs = steamrot::CreateEntityDamageDataFbs(
      builder, 0, target_id, 100.0f, 1);  // attacker_id is null
  
  builder.Finish(damage_fbs);
  
  auto damage_data_fbs = flatbuffers::GetRoot<steamrot::EntityDamageDataFbs>(
      builder.GetBufferPointer());
  
  auto result = steamrot::event::CreateEntityDamageData(*damage_data_fbs);
  
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("CreateEntityDamageData invalid UUID", "[unit][event_factory]") {
  flatbuffers::FlatBufferBuilder builder;
  
  auto attacker_id = builder.CreateString("not-a-uuid");
  auto target_id = builder.CreateString("123e4567-e89b-12d3-a456-426614174001");
  
  auto damage_fbs = steamrot::CreateEntityDamageDataFbs(
      builder, attacker_id, target_id, 100.0f, 1);
  
  builder.Finish(damage_fbs);
  
  auto damage_data_fbs = flatbuffers::GetRoot<steamrot::EntityDamageDataFbs>(
      builder.GetBufferPointer());
  
  auto result = steamrot::event::CreateEntityDamageData(*damage_data_fbs);
  
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::InvalidUUID);
}
```

**Add to CMakeLists.txt**: `tests/unit/events/CMakeLists.txt`

```cmake
add_executable(test_events
  EventHandler.test.cpp
  event_type_conversion.test.cpp
  subscriber_factory.test.cpp
  event_factory_EntityDamageData.test.cpp  # ADD THIS
)
```

### Step 10: Run Tests Locally

**Note**: As per project guidelines, agents do NOT run tests. This step is for reference on how users will test locally.

```bash
# Run all event tests
ctest --preset Debug -R event

# Run specific test
ctest --preset Debug -R EntityDamageData

# Verbose output on failure
ctest --preset Debug -R event --output-on-failure
```

**Common Test Failures**:
- Variant type mismatch: Check EventData variant updated
- Conversion error: Check factory function implementation
- Null pointer: Check FlatBuffers field validation

---

## Usage Example

Once your event is defined, use it in your code:

### Creating and Sending an Event

```cpp
// In a Logic class or UI handler
#include "EventHandler.h"
#include "EventPacket.h"

// Create the event data
steamrot::EntityDamageData damage_data;
damage_data.attacker_id = attacker_entity_uuid;
damage_data.target_id = target_entity_uuid;
damage_data.damage_amount = 50.0f;
damage_data.damage_type = DamageType::Fire;

// Create the event packet
steamrot::EventPacket event(
    steamrot::EventType::ENTITY_DAMAGE,
    damage_data,
    1  // lifetime: 1 tick
);

// Add to event handler
event_handler.AddEvent(event);
```

### Subscribing to the Event

```cpp
// Create a subscriber
auto damage_subscriber = std::make_shared<steamrot::Subscriber>(
    steamrot::EventType::ENTITY_DAMAGE
);

// Register with event handler
event_handler.RegisterSubscriber(damage_subscriber);

// In your Logic class update loop, check if active
if (damage_subscriber->m_active) {
    // Extract the event data
    if (damage_subscriber->m_received_event_data.has_value()) {
        const auto& event_data = damage_subscriber->m_received_event_data.value();
        
        if (std::holds_alternative<steamrot::EntityDamageData>(event_data)) {
            const auto& damage = std::get<steamrot::EntityDamageData>(event_data);
            
            // Process the damage
            ApplyDamage(damage.target_id, damage.damage_amount);
        }
    }
}
```

---

## Common Patterns

### Pattern 1: Simple Event (No Data)

If your event carries no data:

```cpp
// EventType.h - Add enum
SIMPLE_EVENT

// Skip EventPacket.h data struct

// EventPacket.h - Use std::monostate (already in variant)

// Usage
EventPacket event(EventType::SIMPLE_EVENT, std::monostate{}, 1);
```

### Pattern 2: Event with Single String

For events with a single identifier:

```cpp
// Reuse existing types
EventPacket event(EventType::MY_EVENT, UserInterfaceName("my_id"), 1);

// Or ToggleName
EventPacket event(EventType::MY_EVENT, ToggleName("toggle_id"), 1);
```

**Note**: Consider creating a dedicated struct if the string has specific validation or semantic meaning.

### Pattern 3: Event with Multiple Fields

Follow the full workflow (as documented above).

### Pattern 4: Event with Optional Data

```cpp
struct MyEventData {
  uuids::uuid entity_id;
  std::optional<float> optional_value;  // May or may not be present
  std::optional<std::string> optional_message;
};

// Factory function checks for presence
if (data.optional_value()) {
  my_data.optional_value = data.optional_value();
}
```

### Pattern 5: Event with Vector/Collection

```cpp
struct MyEventData {
  std::vector<uuids::uuid> entity_ids;
};

// FlatBuffers schema
table MyEventDataFbs {
  entity_ids: [string];  // Vector of strings
}

// Factory function
if (data.entity_ids()) {
  for (const auto* id_str : *data.entity_ids()) {
    if (uuids::uuid::is_valid_uuid(id_str->c_str())) {
      my_data.entity_ids.push_back(uuids::uuid::from_string(id_str->c_str()));
    }
  }
}
```

---

## Troubleshooting

### Issue: Build fails with "EventDataData_<MyType> not found"

**Cause**: FlatBuffers union not updated or not regenerated.

**Solution**:
1. Check `events.fbs` has your type in the `EventDataData` union
2. Rebuild project to regenerate FlatBuffers headers

### Issue: std::variant error "alternative not found"

**Cause**: Type not added to `EventData` variant.

**Solution**: Check `EventPacket.h` → `using EventData = std::variant<...>`

### Issue: "Unsupported EventTypeFbs value in conversion"

**Cause**: Missing case in `ConvertEventTypeFbsToEventType()`.

**Solution**: Check `event_type_conversion.cpp` → add case for your enum value

### Issue: Runtime error "CreateEventData: type not handled"

**Cause**: Missing case in `CreateEventData()` switch statement.

**Solution**: Check `event_factory.cpp` → `CreateEventData()` → add case for your FlatBuffers table

### Issue: Tests pass locally, but fail in CI

**Cause**: Likely forgot to update CMakeLists.txt with new test files.

**Solution**: Check `tests/unit/events/CMakeLists.txt` includes your test file

---

## Best Practices

### Do:
- ✅ Choose descriptive, specific event names
- ✅ Document the purpose of new events
- ✅ Validate all FlatBuffers data before use
- ✅ Write tests for happy path AND error cases
- ✅ Use existing types when appropriate (don't create unnecessary structs)
- ✅ Follow naming conventions consistently

### Don't:
- ❌ Skip validation in factory functions
- ❌ Forget to update both native and FlatBuffers enums
- ❌ Use generic names like `DATA_EVENT` or `THING_HAPPENED`
- ❌ Create events for internal-only logic (not all state changes need events)
- ❌ Assume FlatBuffers fields are always present (always validate)

---

## Related Documentation

- **Architecture Analysis**: `documentation/architecture/EVENT_SYSTEM_ANALYSIS.md`
- **Testing Guide**: See README.md section on testing
- **FlatBuffers**: Official FlatBuffers documentation

---

**Document Status**: Complete  
**Last Updated**: 2026-02-12  
**Feedback**: Report issues or suggestions to improve this workflow
