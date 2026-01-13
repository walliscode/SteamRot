# Event System Architecture Analysis

## Executive Summary

This document analyzes the current event system architecture in SteamRot and identifies inconsistencies with the rest of the codebase. The analysis reveals several areas where the event system diverges from established patterns, particularly around type organization, FlatBuffers naming conventions, and coupling between subsystems.

## Current State Analysis

### 1. Type Organization

#### Components Pattern (Established Standard)
```
src/types/components/
├── Component.h           # Base type
├── CUserInterface.h      # Derived component (prefix: C)
├── CGrimoireMachina.h    # Derived component (prefix: C)
└── containers.h          # Component register
```

**Characteristics:**
- Pure data structs inheriting from `Component`
- Consistent `C` prefix for component classes
- Located in `src/types/components/`
- No FlatBuffers includes in type definitions

#### Events Pattern (Current - Inconsistent)
```
src/types/events/
├── EventPacket.h         # Main event type - INCLUDES FlatBuffers
├── Subscriber.h          # Event subscriber - INCLUDES FlatBuffers  
└── UserInputBitset.h     # Input data type
```

**Issues:**
- `EventPacket.h` directly includes `events_generated.h` (FlatBuffers)
- `EventPacket.h` includes `scene_types_generated.h` (FlatBuffers)
- `Subscriber.h` includes `EventPacket.h` (which brings FlatBuffers)
- Type definitions are tightly coupled to serialization format
- No separation between runtime types and serialization

### 2. FlatBuffers Schema Naming Conventions

#### Entity Schemas (Mostly Consistent)
```flatbuffers
// grimoire_machina.fbs
table GrimoireMachinaData { ... }

// user_interface.fbs  
table UserInterfaceFbs { ... }    // Note: Uses "Fbs" suffix
table PanelData { ... }
table ButtonData { ... }
```

**Pattern:** Either `Data` suffix OR `Fbs` suffix (inconsistent)

#### Event Schemas (Multiple Patterns)
```flatbuffers
// events.fbs
enum EventType { ... }             // No suffix
union EventDataData { ... }        // Double "Data" suffix

// event_packet_data.fbs
table EventPacketData { ... }      // "Data" suffix

// subscriber.fbs
table SubscriberFbs { ... }        // "Fbs" suffix

// user_input.fbs
table UserInputBitsetData { ... }  // "Data" suffix
enum KeyboardInput { ... }         // No suffix
enum MouseInput { ... }            // No suffix
```

**Issues:**
- Inconsistent suffixes: `Data`, `Fbs`, or none
- Confusing naming like `EventDataData` (union for EventData)
- No clear convention for when to use which suffix

#### Comparison with Entity FlatBuffers
```flatbuffers
// entities.fbs
table EntityDataFbs { ... }
table EntityCollectionFbs { ... }

// Seems to prefer "Fbs" suffix for main tables
// Uses "Data" suffix for nested/component data
```

### 3. Type-to-FlatBuffers Coupling

#### Components Approach (Loose Coupling)
```cpp
// CUserInterface.h - NO FlatBuffers includes
struct CUserInterface : public Component {
    std::string m_name{"Default UI"};
    std::unique_ptr<UIElement> m_root_element;
    bool m_visible{false};
};

// Conversion happens in separate configurator
// src/entity/FlatbuffersEntityConfigurator.cpp
```

**Benefits:**
- Component types independent of serialization
- Can change FlatBuffers schema without changing component
- Clear separation of concerns

#### Events Approach (Tight Coupling)
```cpp
// EventPacket.h - INCLUDES FlatBuffers directly
#include "events_generated.h"
#include "scene_types_generated.h"

using EventData = std::variant<std::monostate, UserInputBitset,
                               SceneChangePacket, UserInterfaceName>;

struct EventPacket {
    EventType event_type{EventType::EventType_NONE};  // FlatBuffers enum
    EventData event_data{std::monostate{}};
    // ...
};
```

**Issues:**
- Runtime type directly uses FlatBuffers enum `EventType`
- Type definition depends on FlatBuffers generated header
- Changes to FlatBuffers schema force recompilation of dependent code
- Violates separation of concerns principle

### 4. Cross-Schema Dependencies

#### Problem: Entity Schemas Include Event Schemas

```flatbuffers
// user_interface.fbs
include "../events/subscriber.fbs";
include "../events/event_packet_data.fbs";

table UIElementData {
    // ...
    subscriber_data: SubscriberFbs;
    response_event_data: EventPacketData;
    // ...
}
```

**Issues:**
- Entity definitions depend on event definitions
- Creates tight coupling between subsystems
- Circular dependency potential
- Makes both schemas harder to evolve independently

### 5. Conversion/Factory Logic Organization

#### Component Pattern
```
src/entity/
├── FlatbuffersEntityConfigurator.h
└── FlatbuffersEntityConfigurator.cpp
    - ConfigureComponent() overloads for each component
```

#### Event Pattern (Scattered)
```
src/events/
├── event_factory.h/cpp           # CreateEventPacketFromData()
├── event_bus_conversion.h/cpp    # ConvertEventBusDataToEventBus()
└── subscriber_factory.h/cpp      # CreateSubscriber()
```

**Issues:**
- Factory logic split across multiple files
- No clear ownership of conversion responsibilities
- subscriber_factory in `events/` but event_factory also exists

## Architectural Inconsistencies Summary

| Aspect | Components (Standard) | Events (Current) | Issue |
|--------|---------------------|------------------|-------|
| Type includes FlatBuffers | ❌ No | ✅ Yes | Tight coupling |
| FlatBuffers naming | `ComponentData` | Mixed: `Data`, `Fbs`, none | Inconsistent |
| Conversion location | Centralized configurator | Multiple factory files | Scattered |
| Cross-schema deps | Minimal | Entity→Event schemas | Tight coupling |
| Runtime enum source | Own definition | FlatBuffers enum | Coupling |

## Recommendations

### Phase 1: Separate Runtime Types from FlatBuffers

**Goal:** Remove FlatBuffers includes from runtime event types

#### 1.1 Create Pure Runtime Event Types

Move event enums and types to be independent:

```cpp
// src/types/events/EventType.h (NEW)
namespace steamrot {
    enum class EventType : uint64_t {
        None = 0,
        Test = 1,
        UserInput = 2,
        ToggleUI = 4,
        ChangeScene = 8,
        QuitGame = 16,
        ToggleDropdown = 32
    };
}

// EventPacket.h (MODIFIED - remove FlatBuffers includes)
#include "EventType.h"  // Own definition, not FlatBuffers
namespace steamrot {
    struct EventPacket {
        EventType event_type{EventType::None};
        // ...
    };
}
```

#### 1.2 Update FlatBuffers Schemas to Reference Core Types

```flatbuffers
// events.fbs (keep FlatBuffers enum for serialization)
enum EventTypeFbs: ulong (bit_flags) {
    NONE = 0,
    TEST = 1,
    USER_INPUT = 2,
    // ...
}
```

#### 1.3 Add Conversion Layer

```cpp
// src/events/event_type_conversion.h (NEW)
namespace steamrot::event {
    EventType ConvertFromFlatBuffers(EventTypeFbs fbs_type);
    EventTypeFbs ConvertToFlatBuffers(EventType type);
}
```

### Phase 2: Standardize FlatBuffers Naming

**Goal:** Consistent naming convention for all FlatBuffers schemas

**Proposed Convention:**
- **FlatBuffers tables representing runtime types:** `TypeNameData` suffix
  - `EventPacketData`, `SubscriberData`, `UserInputBitsetData`
- **FlatBuffers unions:** `TypeNameUnion` suffix  
  - `EventDataUnion` (instead of `EventDataData`)
- **FlatBuffers enums:** `TypeNameFbs` suffix or no suffix for core enums
  - `EventTypeFbs`, `KeyboardInput`, `MouseInput`

**Changes needed:**
```flatbuffers
// events.fbs
union EventDataUnion {        // WAS: EventDataData
    UserInputBitsetData,
    SceneChangePacketData,
    UserInterfaceNameData
}

table SubscriberData {        // WAS: SubscriberFbs
    event_type_data: EventTypeFbs;
    trigger_data: EventDataUnion;
    active: bool;
}
```

### Phase 3: Reduce Cross-Schema Coupling

**Goal:** Minimize dependencies between entity and event schemas

#### 3.1 Extract Common Types to Core Schemas

Create `src/types/flatbuffers/core/event_reference.fbs`:

```flatbuffers
namespace steamrot;

// Lightweight reference to subscriber (not full data)
table SubscriberRef {
    event_type: EventTypeFbs;
    has_trigger_data: bool;
}

// Lightweight reference to event packet (not full data)
table EventPacketRef {
    event_type: EventTypeFbs;
    event_lifetime: int;
}
```

#### 3.2 Update Entity Schemas to Use References

```flatbuffers
// user_interface.fbs
include "../core/event_reference.fbs";  // WAS: include "../events/..."

table UIElementData {
    // ...
    subscriber_ref: SubscriberRef;        // WAS: subscriber_data
    response_event_ref: EventPacketRef;   // WAS: response_event_data
    // ...
}
```

#### 3.3 Alternative: Use Opaque Identifiers

Instead of embedding event data in UI elements, use string identifiers:

```flatbuffers
table UIElementData {
    subscriber_id: string;      // "toggle_button_subscriber"
    response_event_id: string;  // "button_click_event"
}
```

Then maintain separate mapping files for event configurations.

### Phase 4: Consolidate Conversion Logic

**Goal:** Single source of truth for event type conversions

#### 4.1 Create Unified Event Configurator

```
src/events/
└── FlatbuffersEventConfigurator.h/cpp  (NEW)
    - ConfigureEventPacket()
    - ConfigureSubscriber()
    - ConfigureEventBus()
```

#### 4.2 Deprecate Scattered Factories

- Move logic from `event_factory` → `FlatbuffersEventConfigurator`
- Move logic from `event_bus_conversion` → `FlatbuffersEventConfigurator`
- Move logic from `subscriber_factory` → `FlatbuffersEventConfigurator`

#### 4.3 Update Naming Convention

Follow entity pattern:
- Interface: `IEventConfigurator` in `src/types/interfaces/`
- Implementation: `FlatbuffersEventConfigurator` in `src/events/`

## Implementation Strategy

### Incremental Approach (Recommended)

To avoid breaking existing code, implement changes incrementally:

1. **Add new types alongside old ones** (no breaking changes yet)
2. **Create conversion/adapter layer** between old and new
3. **Update test infrastructure** to work with both
4. **Migrate consumers** one by one to new types
5. **Deprecate and remove old types** after full migration

### Big Bang Approach (Alternative)

Implement all changes at once:
- **Pros:** Clean, consistent result faster
- **Cons:** Higher risk, more complex PR, harder to review

## Migration Path Example

### Before (Current State)
```cpp
// EventPacket.h
#include "events_generated.h"  // FlatBuffers coupling

struct EventPacket {
    EventType event_type{EventType::EventType_NONE};  // FlatBuffers enum
};
```

### After (Decoupled State)
```cpp
// EventPacket.h
#include "EventType.h"  // Own enum definition

struct EventPacket {
    EventType event_type{EventType::None};  // Own enum
};

// event_type_conversion.cpp
EventType ConvertFromFlatBuffers(EventTypeFbs fbs_type) {
    switch (fbs_type) {
        case EventTypeFbs::EVENT_NONE: return EventType::None;
        case EventTypeFbs::EVENT_TEST: return EventType::Test;
        // ...
    }
}
```

## Testing Strategy

1. **Unit tests for conversion functions**
   - FlatBuffers ↔ Runtime type conversions
   - Edge cases (unknown enums, null data)

2. **Integration tests for configurators**
   - Loading event data from FlatBuffers files
   - Verifying runtime behavior unchanged

3. **Regression tests**
   - Existing event system tests should pass
   - UI tests with subscribers should pass

## Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Breaking existing code | High | High | Incremental migration, maintain compatibility layer |
| Schema compatibility issues | Medium | Medium | Version FlatBuffers schemas, provide migration tools |
| Performance regression | Low | Medium | Benchmark conversion overhead, optimize hot paths |
| Incomplete migration | Medium | High | Track migration progress, enforce in code review |

## Success Criteria

The refactoring is successful when:

1. ✅ **No FlatBuffers includes in runtime event types**
   - `EventPacket.h`, `Subscriber.h`, etc. are FlatBuffers-free

2. ✅ **Consistent FlatBuffers naming**
   - All schemas follow `TypeNameData` or `TypeNameFbs` convention
   - No confusing names like `EventDataData`

3. ✅ **Minimal cross-schema dependencies**
   - Entity schemas don't directly include event schemas
   - Core types used for shared concepts

4. ✅ **Consolidated conversion logic**
   - Single `FlatbuffersEventConfigurator` owns all conversions
   - Clear separation between serialization and runtime logic

5. ✅ **All tests passing**
   - No regression in functionality
   - New tests cover conversion layer

## Conclusion

The current event system architecture diverges from the established component patterns in three key areas:

1. **Tight coupling** between runtime types and FlatBuffers
2. **Inconsistent naming** conventions for schemas
3. **Scattered conversion logic** across multiple files

Following the recommendations in this document will:
- Align event system with component architecture patterns
- Improve maintainability and testability
- Enable independent evolution of runtime and serialization layers
- Reduce compilation dependencies

The incremental migration strategy minimizes risk while progressively improving the codebase architecture.

## References

- Component patterns: `src/types/components/`, `src/entity/FlatbuffersEntityConfigurator.cpp`
- Event types: `src/types/events/`, `src/events/`
- FlatBuffers schemas: `src/types/flatbuffers/events/`, `src/types/flatbuffers/entities/`
- Google C++ Style Guide: https://google.github.io/styleguide/cppguide.html
