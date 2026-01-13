# Event System vs Component System - Architecture Comparison

## Visual Overview

This document provides a visual comparison between the component architecture (the established pattern) and the event architecture (current state needing alignment).

## Architecture Comparison Diagram

### Component Architecture (Established Pattern)

```
┌─────────────────────────────────────────────────────────────┐
│                    COMPONENT PATTERN                         │
└─────────────────────────────────────────────────────────────┘

Runtime Types (src/types/components/)
┌──────────────────────────────────────┐
│ Component.h                          │
│ - Pure interface                     │
│ - NO FlatBuffers includes            │
└──────────────────────────────────────┘
                ▲
                │ inherits
                │
┌──────────────────────────────────────┐
│ CUserInterface.h                     │
│ - Pure data struct                   │
│ - std::string m_name                 │
│ - std::unique_ptr<UIElement> m_root  │
│ - bool m_visible                     │
│ - NO FlatBuffers includes            │
└──────────────────────────────────────┘

FlatBuffers Schemas (src/types/flatbuffers/entities/)
┌──────────────────────────────────────┐
│ user_interface.fbs                   │
│                                      │
│ table UserInterfaceFbs {             │
│   root_ui_element: PanelData;        │
│   ui_name: string;                   │
│   is_visible: bool;                  │
│ }                                    │
└──────────────────────────────────────┘

Conversion Layer (src/entity/)
┌──────────────────────────────────────┐
│ FlatbuffersEntityConfigurator        │
│                                      │
│ ConfigureComponent(                  │
│   UserInterfaceFbs* data,            │
│   CUserInterface& component)         │
│                                      │
│ - Converts FlatBuffers → Runtime     │
│ - Centralized conversion logic       │
└──────────────────────────────────────┘

✅ Clean separation: Runtime ⟷ Conversion ⟷ Serialization
```

### Event Architecture (Current State - Needs Alignment)

```
┌─────────────────────────────────────────────────────────────┐
│                    EVENT PATTERN (CURRENT)                   │
└─────────────────────────────────────────────────────────────┘

Runtime Types (src/types/events/)
┌──────────────────────────────────────┐
│ EventPacket.h                        │
│ ❌ #include "events_generated.h"     │
│ ❌ #include "scene_types_generated.h"│
│                                      │
│ struct EventPacket {                 │
│   EventType event_type;  ⚠️ FlatBuffers enum │
│   EventData event_data;              │
│   uuids::uuid event_id;              │
│ }                                    │
│                                      │
│ ❌ TIGHTLY COUPLED TO FLATBUFFERS    │
└──────────────────────────────────────┘

FlatBuffers Schemas (src/types/flatbuffers/events/)
┌──────────────────────────────────────┐
│ events.fbs                           │
│                                      │
│ enum EventType { ... }               │
│ union EventDataData { ... } ⚠️       │
│                                      │
│ subscriber.fbs                       │
│ table SubscriberFbs { ... } ⚠️       │
│                                      │
│ event_packet_data.fbs                │
│ table EventPacketData { ... }        │
│                                      │
│ ⚠️ Inconsistent naming               │
└──────────────────────────────────────┘

Conversion Layer (src/events/)
┌──────────────────────────────────────┐
│ ❌ SCATTERED LOGIC                   │
│                                      │
│ event_factory.h/cpp                  │
│ - CreateEventPacketFromData()        │
│                                      │
│ event_bus_conversion.h/cpp           │
│ - ConvertEventBusDataToEventBus()    │
│                                      │
│ subscriber_factory.h/cpp             │
│ - CreateSubscriber()                 │
│                                      │
│ ⚠️ No centralized configurator       │
└──────────────────────────────────────┘

Cross-Schema Dependencies
┌──────────────────────────────────────┐
│ user_interface.fbs                   │
│ ❌ include "../events/subscriber.fbs"│
│ ❌ include "../events/event_packet..." │
│                                      │
│ table UIElementData {                │
│   subscriber_data: SubscriberFbs;    │
│   response_event: EventPacketData;   │
│ }                                    │
│                                      │
│ ❌ TIGHT COUPLING BETWEEN SUBSYSTEMS │
└──────────────────────────────────────┘

❌ Problems:
- Runtime types depend on FlatBuffers
- Inconsistent schema naming
- Scattered conversion logic
- Entity schemas depend on event schemas
```

### Event Architecture (Proposed After Refactoring)

```
┌─────────────────────────────────────────────────────────────┐
│                EVENT PATTERN (PROPOSED)                      │
└─────────────────────────────────────────────────────────────┘

Runtime Types (src/types/)
┌──────────────────────────────────────┐
│ core/EventType.h (NEW)               │
│ - Pure enum (NO FlatBuffers)         │
│                                      │
│ enum class EventType : uint64_t {    │
│   None = 0, Test = 1, ... }          │
│                                      │
│ ✅ Independent of serialization      │
└──────────────────────────────────────┘
                ▲
                │ uses
                │
┌──────────────────────────────────────┐
│ events/EventPacket.h                 │
│ - NO FlatBuffers includes            │
│ - Uses EventType.h (own definition)  │
│                                      │
│ struct EventPacket {                 │
│   EventType event_type;  ✅          │
│   EventData event_data;              │
│   uuids::uuid event_id;              │
│ }                                    │
│                                      │
│ ✅ Decoupled from FlatBuffers        │
└──────────────────────────────────────┘

FlatBuffers Schemas (src/types/flatbuffers/events/)
┌──────────────────────────────────────┐
│ events.fbs                           │
│                                      │
│ enum EventTypeFbs { ... } ✅         │
│ union EventDataUnion { ... } ✅      │
│                                      │
│ subscriber.fbs                       │
│ table SubscriberData { ... } ✅      │
│                                      │
│ event_packet_data.fbs                │
│ table EventPacketData { ... } ✅     │
│                                      │
│ ✅ Consistent naming convention      │
└──────────────────────────────────────┘

Conversion Layer (src/events/)
┌──────────────────────────────────────┐
│ event_type_conversion.h/cpp (NEW)    │
│ - ConvertFromFlatBuffers()           │
│ - ConvertToFlatBuffers()             │
│                                      │
│ FlatbuffersEventConfigurator (NEW)   │
│ - ConfigureEventPacket()             │
│ - ConfigureSubscriber()              │
│ - ConfigureEventBus()                │
│                                      │
│ ✅ Centralized conversion logic      │
└──────────────────────────────────────┘

Core Event References (NEW)
┌──────────────────────────────────────┐
│ core/event_reference.fbs (NEW)       │
│                                      │
│ table SubscriberRef {                │
│   event_type: EventTypeFbs;          │
│   has_trigger_data: bool;            │
│   config_id: string;                 │
│ }                                    │
│                                      │
│ table EventPacketRef {               │
│   event_type: EventTypeFbs;          │
│   event_lifetime: int;               │
│   config_id: string;                 │
│ }                                    │
│                                      │
│ ✅ Lightweight references            │
└──────────────────────────────────────┘

Updated Entity Schema
┌──────────────────────────────────────┐
│ user_interface.fbs                   │
│ ✅ include "../core/event_reference.fbs" │
│                                      │
│ table UIElementData {                │
│   subscriber_ref: SubscriberRef;     │
│   response_event_ref: EventPacketRef;│
│ }                                    │
│                                      │
│ ✅ Loose coupling via references     │
└──────────────────────────────────────┘

✅ Aligned with component pattern:
- Runtime types independent of FlatBuffers
- Consistent schema naming
- Centralized conversion logic
- Minimal cross-schema dependencies
```

## Data Flow Comparison

### Component Data Flow (Current Standard)

```
JSON File
   │
   ├─→ FlatBuffers Compiler (compile time)
   │      │
   │      ↓
   │   user_interface_generated.h
   │
   ↓
Binary .bin File
   │
   ↓
FlatBuffersEntityConfigurator
   │
   ├─→ Read: const UserInterfaceFbs* data
   │
   ├─→ Convert: data → component
   │
   └─→ Write: CUserInterface& component
          │
          ↓
       Runtime Usage (NO FlatBuffers)
```

### Event Data Flow (Current - Problematic)

```
JSON File
   │
   ├─→ FlatBuffers Compiler
   │      │
   │      ↓
   │   events_generated.h ─────┐
   │                           │
   ↓                           │
Binary .bin File               │
   │                           │
   ↓                           │
event_factory                  │
   │                           │
   ├─→ Read: EventPacketData*  │
   │                           │
   ├─→ Convert: data → event   │
   │                           │
   └─→ Write: EventPacket ─────┤
          │                    │
          ↓                    │
       Runtime Usage           │
          │                    │
          │ ❌ INCLUDES ───────┘
          ↓
    events_generated.h STILL NEEDED
```

### Event Data Flow (Proposed - Aligned)

```
JSON File
   │
   ├─→ FlatBuffers Compiler
   │      │
   │      ↓
   │   event_packet_data_generated.h
   │
   ↓
Binary .bin File
   │
   ↓
FlatbuffersEventConfigurator
   │
   ├─→ Read: const EventPacketData* data
   │
   ├─→ Convert: FlatBuffers enum → Runtime enum
   │    via event_type_conversion
   │
   └─→ Write: EventPacket& event
          │
          ↓
       Runtime Usage (NO FlatBuffers)
       Uses EventType.h only
```

## Naming Convention Comparison

### Component FlatBuffers Naming (Standard)

| Type | Example | Convention |
|------|---------|------------|
| Table (Component) | `GrimoireMachinaData` | `ComponentNameData` |
| Table (Entity) | `EntityDataFbs` | `TypeNameFbs` |
| Enum | `LayoutFbs` | `TypeNameFbs` |
| Union | `UIElementDataUnion` | `TypeNameUnion` |

**Pattern:** Mostly consistent, prefers `Data` or `Fbs` suffix

### Event FlatBuffers Naming (Current - Inconsistent)

| Type | Example | Issue |
|------|---------|-------|
| Enum | `EventType` | No suffix (conflicts with runtime) |
| Union | `EventDataData` | ❌ Confusing double "Data" |
| Table | `SubscriberFbs` | Uses `Fbs` suffix |
| Table | `EventPacketData` | Uses `Data` suffix |
| Table | `UserInputBitsetData` | Uses `Data` suffix |

**Pattern:** ❌ Inconsistent - mixed suffixes

### Event FlatBuffers Naming (Proposed - Consistent)

| Type | Example | Convention |
|------|---------|------------|
| Enum | `EventTypeFbs` | `TypeNameFbs` |
| Union | `EventDataUnion` | `TypeNameUnion` |
| Table | `SubscriberData` | `TypeNameData` |
| Table | `EventPacketData` | `TypeNameData` |
| Table | `UserInputBitsetData` | `TypeNameData` |

**Pattern:** ✅ Consistent - clear suffixes for all types

## Coupling Analysis

### Component System (Loose Coupling) ✅

```
┌─────────────┐         ┌─────────────┐
│  Component  │         │ FlatBuffers │
│   Runtime   │         │   Schema    │
│             │         │             │
│  NO deps ◀──┼─────────┼──X (no dep) │
└─────────────┘         └─────────────┘
      ▲                       ▲
      │                       │
      │    ┌────────────┐    │
      └────│ Conversion │────┘
           │   Layer    │
           └────────────┘

Independent evolution
```

### Event System (Tight Coupling) ❌

```
┌─────────────┐         ┌─────────────┐
│    Event    │         │ FlatBuffers │
│   Runtime   │◀────────│   Schema    │
│             │  ❌ includes events_generated.h
│  DEPENDS ◀──┼─────────┤             │
└─────────────┘         └─────────────┘
      ▲                       
      │                       
      │    ┌────────────┐    
      └────│ Conversion │ (scattered)
           │   Logic    │
           └────────────┘

Runtime types can't change without FlatBuffers
```

### Entity-Event Coupling (Current) ❌

```
┌─────────────┐         ┌─────────────┐
│   Entity    │         │    Event    │
│   Schema    │────────▶│   Schema    │
│             │  ❌ includes subscriber.fbs
│             │  ❌ includes event_packet_data.fbs
└─────────────┘         └─────────────┘

Entity definitions depend on event definitions
Both schemas must evolve together
```

### Entity-Event Coupling (Proposed) ✅

```
┌─────────────┐         ┌─────────────┐
│   Entity    │         │    Core     │
│   Schema    │────────▶│ References  │
│             │  ✅ includes event_reference.fbs
└─────────────┘         └─────────────┘
                              │
                              │ lightweight
                              ↓
                        ┌─────────────┐
                        │    Event    │
                        │   Schema    │
                        └─────────────┘

Entity uses lightweight references
Event schema can evolve independently
```

## File Organization Comparison

### Component Files (Standard)

```
src/types/components/
├── Component.h                    ← Base interface
├── CUserInterface.h               ← Runtime type
├── CGrimoireMachina.h             ← Runtime type
└── containers.h                   ← Component register

src/types/flatbuffers/entities/
├── user_interface.fbs             ← Schema
├── grimoire_machina.fbs           ← Schema
└── entities.fbs                   ← Collection schema

src/entity/
├── FlatbuffersEntityConfigurator.h ← Conversion
└── FlatbuffersEntityConfigurator.cpp

✅ Clear organization: Types → Schemas → Conversion
```

### Event Files (Current - Scattered)

```
src/types/events/
├── EventPacket.h                  ← Runtime (includes FlatBuffers) ❌
├── Subscriber.h                   ← Runtime (includes FlatBuffers) ❌
└── UserInputBitset.h              ← Runtime

src/types/flatbuffers/events/
├── events.fbs                     ← Mixed enums/unions
├── event_packet_data.fbs          ← Event packet schema
├── subscriber.fbs                 ← Subscriber schema
├── user_input.fbs                 ← Input schema
└── event_bus_data.fbs             ← Bus schema

src/events/
├── event_factory.h/cpp            ← Conversion (partial) ❌
├── event_bus_conversion.h/cpp     ← Conversion (partial) ❌
├── subscriber_factory.h/cpp       ← Conversion (partial) ❌
├── EventHandler.h/cpp             ← Handler
└── event_handler_tick.h/cpp       ← Tick logic

❌ Scattered organization, tight coupling
```

### Event Files (Proposed - Aligned)

```
src/types/core/
└── EventType.h                    ← Pure runtime enum ✅

src/types/events/
├── EventPacket.h                  ← Runtime (NO FlatBuffers) ✅
├── Subscriber.h                   ← Runtime (NO FlatBuffers) ✅
└── UserInputBitset.h              ← Runtime

src/types/flatbuffers/core/
└── event_reference.fbs            ← Lightweight references ✅

src/types/flatbuffers/events/
├── events.fbs                     ← Enums/unions (renamed) ✅
├── event_packet_data.fbs          ← Event packet schema
├── subscriber.fbs                 ← Subscriber schema (renamed) ✅
├── user_input.fbs                 ← Input schema
└── event_bus_data.fbs             ← Bus schema

src/events/
├── event_type_conversion.h/cpp    ← Enum conversion ✅
├── FlatbuffersEventConfigurator.h ← Unified conversion ✅
├── FlatbuffersEventConfigurator.cpp
├── EventHandler.h/cpp             ← Handler
└── event_handler_tick.h/cpp       ← Tick logic

✅ Aligned organization: Clean separation
```

## Key Takeaways

### Problems with Current Event System

1. ❌ **Runtime types include FlatBuffers headers**
   - Makes runtime code dependent on serialization format
   - Increases compilation dependencies
   - Violates separation of concerns

2. ❌ **Inconsistent FlatBuffers naming**
   - Confusing schemas (`EventDataData`)
   - Mixed conventions (`Data` vs `Fbs`)
   - Harder to maintain

3. ❌ **Cross-schema dependencies**
   - Entity schemas include event schemas
   - Circular dependency risk
   - Can't evolve independently

4. ❌ **Scattered conversion logic**
   - Multiple factory files
   - No single source of truth
   - Harder to maintain

### Benefits of Proposed Changes

1. ✅ **Independent runtime types**
   - No FlatBuffers dependencies
   - Easier to test
   - Clear separation of concerns

2. ✅ **Consistent naming**
   - Clear conventions throughout
   - Easier to understand
   - Professional codebase

3. ✅ **Loose coupling**
   - Lightweight references
   - Independent evolution
   - Reduced dependencies

4. ✅ **Centralized conversion**
   - Single configurator
   - Follows component pattern
   - Easier to maintain

## Implementation Priority

### Phase 1: Critical (Must Do)
- Create independent EventType enum
- Add conversion layer
- Remove FlatBuffers from runtime types

### Phase 2: Important (Should Do)
- Standardize FlatBuffers naming
- Update all schemas consistently

### Phase 3: Beneficial (Nice to Have)
- Create event references
- Reduce cross-schema coupling

### Phase 4: Cleanup (Eventually)
- Consolidate conversion logic
- Deprecate old factories

## Success Visualization

```
Before Refactoring:
┌─────────────────────────────────────┐
│  Runtime Event Types                │
│  ❌ Coupled to FlatBuffers          │
│  ❌ Inconsistent naming             │
│  ❌ Scattered conversions           │
│  ❌ Cross-schema dependencies       │
└─────────────────────────────────────┘

After Refactoring:
┌─────────────────────────────────────┐
│  Runtime Event Types                │
│  ✅ Independent of FlatBuffers      │
│  ✅ Consistent patterns             │
│  ✅ Centralized conversions         │
│  ✅ Minimal dependencies            │
└─────────────────────────────────────┘

Aligned with Component Architecture! 🎉
```
