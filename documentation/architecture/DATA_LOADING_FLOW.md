# Data Loading Flow Diagrams

## Overview

This document provides visual representations of data flow through the SteamRot engine's data loading architecture.

---

## Architecture Layers

### Layer Diagram

```
┌─────────────────────────────────────────────────────────────────────┐
│                         LAYER 0: EXTERNAL DATA                      │
│                         (FlatBuffers Binary)                        │
├─────────────────────────────────────────────────────────────────────┤
│  - Binary files (.bin)                                              │
│  - FlatBuffers generated types: SceneDataFbs, EngineDataFbs, etc.   │
│  - Access via FlatBuffers API (requires null checks)                │
│  - NOT directly used by game logic                                  │
└────────────────────────┬────────────────────────────────────────────┘
                         │
                         │  DataLoader + DataProvider
                         │  (Load from disk, convert types)
                         │
                         ▼
┌─────────────────────────────────────────────────────────────────────┐
│                      LAYER 1: NATIVE STRUCTS                        │
│                      (Safe C++ Types)                               │
├─────────────────────────────────────────────────────────────────────┤
│  - Standard C++ types (std::string, std::vector, enums)             │
│  - Native structs: SceneData, EngineData, EventPacket, etc.         │
│  - No null checks needed                                            │
│  - Passed by value or reference                                     │
│  - Used by Configurators                                            │
└────────────────────────┬────────────────────────────────────────────┘
                         │
                         │  Configurators
                         │  (Configure game objects)
                         │
                         ▼
┌─────────────────────────────────────────────────────────────────────┐
│                      LAYER 2: GAME OBJECTS                          │
│                      (Behavior + Data)                              │
├─────────────────────────────────────────────────────────────────────┤
│  - Active game objects: Scene, Engine, EntityMemoryPool             │
│  - Contain both data AND behavior (methods)                         │
│  - Have lifecycle (constructed, updated, destroyed)                 │
│  - Used directly by game logic                                      │
│  - NO MORE DATA LOADING after this point                            │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Complete Data Flow: Scene Loading

### High-Level Flow

```
┌──────────────┐
│ Application  │
│   Startup    │
└──────┬───────┘
       │
       │ Create DataAccessFactory
       ▼
┌─────────────────────────┐
│  DataAccessFactory      │
│  (DataType::Flatbuffers)│
└──────┬──────────────────┘
       │
       │ Creates concrete providers
       ▼
┌──────────────────────────────────┐
│ - FlatbuffersEngineDataProvider  │
│ - FlatbuffersSceneDataProvider   │
│ - FlatbuffersSceneManager...     │
└──────┬───────────────────────────┘
       │
       │ SceneFactory requests provider
       ▼
┌──────────────────────────────────┐
│  ISceneDataProvider::LoadData()  │
│  (Polymorphic call)              │
└──────┬───────────────────────────┘
       │
       ▼
╔══════════════════════════════════╗
║     LAYER 0 → LAYER 1            ║
║  (FlatBuffers → Native Structs)  ║
╚══════════════════════════════════╝
       │
       ▼
┌─────────────────────────────────────┐
│ FlatbuffersSceneDataProvider        │
│   LoadSceneData()                   │
└──────┬──────────────────────────────┘
       │
       │ Load binary data
       ▼
┌─────────────────────────────────────┐
│ FlatbuffersDataLoader               │
│   ProvideDefaultSceneData()         │
└──────┬──────────────────────────────┘
       │
       │ Returns SceneDataFbs*
       ▼
┌─────────────────────────────────────┐
│ Convert FlatBuffers → Native        │
│ - SceneDataFbs → SceneData          │
│ - Populate SceneInfo                │
│ - Populate SceneConfig              │
│ - Populate SceneResourcesConfig     │
└──────┬──────────────────────────────┘
       │
       │ Return SceneData (Layer 1)
       ▼
╔══════════════════════════════════╗
║     LAYER 1 → LAYER 2            ║
║  (Native Structs → Game Objects) ║
╚══════════════════════════════════╝
       │
       ▼
┌─────────────────────────────────────┐
│ SceneFactory creates Scene object   │
└──────┬──────────────────────────────┘
       │
       │ Configure scene
       ▼
┌─────────────────────────────────────┐
│ ISceneConfigurator::ConfigureScene()│
│ (Polymorphic call)                  │
└──────┬──────────────────────────────┘
       │
       ▼
┌─────────────────────────────────────┐
│ FlatbuffersSceneConfigurator        │
│ - ConfigureSceneInfo()              │
│ - ConfigureSceneResources()         │
│ - ConfigureSceneConfig()            │
│ - ConfigureLogicMap()               │
│ - ImportEntities()                  │
└──────┬──────────────────────────────┘
       │
       │ Configure entities
       ▼
┌─────────────────────────────────────┐
│ FlatbuffersEntityConfigurator       │
│   ConfigureEntityMemoryPool()       │
└──────┬──────────────────────────────┘
       │
       │ Configure components
       ▼
┌─────────────────────────────────────┐
│ ConfigureFirstLayerComponents()     │
│ - ConfigureCUserInterface()         │
│ - ConfigureCGrimoireMachina()       │
└──────┬──────────────────────────────┘
       │
       │ Configure UI elements
       ▼
┌─────────────────────────────────────┐
│ FlatbuffersUIElementConfigurator    │
│   CreateRootUIElement()             │
└──────┬──────────────────────────────┘
       │
       │ Create UI hierarchy
       ▼
┌─────────────────────────────────────┐
│ CreateUIElement() (recursive)       │
│ - ConfigurePanelElement()           │
│ - ConfigureButtonElement()          │
│ - ConvertLayout() (static)          │
└──────┬──────────────────────────────┘
       │
       │ Scene fully configured
       ▼
┌─────────────────────────────────────┐
│        Configured Scene             │
│    (Ready for Game Loop)            │
└─────────────────────────────────────┘
```

---

## Detailed Entity Configuration Flow

### Entity Memory Pool Configuration

```
┌────────────────────────────────────────┐
│ FlatbuffersEntityConfigurator          │
│   ConfigureEntityMemoryPool()          │
└────────────┬───────────────────────────┘
             │
             │ Step 1: Resize pool
             ▼
     ┌────────────────────────┐
     │ entity::memory::       │
     │   ResizeEntityMemory   │
     │   Pool(emp, size)      │
     └────────┬───────────────┘
              │
              │ Step 2: First layer
              ▼
     ┌────────────────────────────────┐
     │ ConfigureFirstLayerComponents()│
     └────────┬───────────────────────┘
              │
              │ For each entity in FlatBuffers data
              ▼
     ┌─────────────────────────────────────┐
     │ Loop: For entity_data in entities   │
     └────┬────────────────────────────────┘
          │
          │ Update current entity pointer
          ▼
     ┌────────────────────────────┐
     │ m_current_entity_data =    │
     │   entity_data              │
     └────┬───────────────────────┘
          │
          │ Check for CUserInterface
          ▼
     ┌────────────────────────────┐
     │ if entity_data->           │
     │   c_user_interface()       │
     └────┬───────────────────────┘
          │
          │ YES
          ▼
     ┌─────────────────────────────────────┐
     │ ConfigureCUserInterface()           │
     │   (Overloaded method)               │
     └────┬────────────────────────────────┘
          │
          │ Call base configuration
          ▼
     ┌─────────────────────────────────────┐
     │ ConfigureComponent(Component&)      │
     │   - Sets m_active = true            │
     └────┬────────────────────────────────┘
          │
          │ Get FlatBuffers data
          ▼
     ┌─────────────────────────────────────┐
     │ ui_data = m_current_entity_data->   │
     │   c_user_interface()                │
     └────┬────────────────────────────────┘
          │
          │ Null check
          ▼
     ┌─────────────────────────────────────┐
     │ if (!ui_data) return error          │
     └────┬────────────────────────────────┘
          │
          │ Configure UI component fields
          ▼
     ┌─────────────────────────────────────┐
     │ if (ui_data->ui_name())             │
     │   ui_component.m_name = ...         │
     │ ui_component.m_visible = ...        │
     └────┬────────────────────────────────┘
          │
          │ Create UI element configurator
          ▼
     ┌─────────────────────────────────────┐
     │ FlatbuffersUIElementConfigurator    │
     │   ui_configurator(event_handler,    │
     │                   *ui_data)         │
     └────┬────────────────────────────────┘
          │
          │ Create root UI element
          ▼
     ┌─────────────────────────────────────┐
     │ ui_configurator.CreateRootUIElement()│
     └────┬────────────────────────────────┘
          │
          │ Assign to component
          ▼
     ┌─────────────────────────────────────┐
     │ ui_component.m_root_element =       │
     │   std::move(root_element)           │
     └─────────────────────────────────────┘
          │
          │ Continue to next component type
          │
          │ Check for CGrimoireMachina
          ▼
     ┌────────────────────────────┐
     │ if entity_data->           │
     │   c_grimoire_machina()     │
     └────┬───────────────────────┘
          │
          │ YES
          ▼
     ┌─────────────────────────────────────┐
     │ ConfigureCGrimoireMachina()         │
     │   (Overloaded method)               │
     └─────────────────────────────────────┘
          │
          │ Continue loop for next entity
          │
          ▼
     ┌────────────────────────────────┐
     │ ConfigureSecondLayerComponents()│
     └────────────────────────────────┘
```

---

## UI Element Configuration Flow

### Recursive UI Element Creation

```
┌────────────────────────────────────────┐
│ FlatbuffersUIElementConfigurator       │
│   CreateRootUIElement()                │
└────────────┬───────────────────────────┘
             │
             │ Check for root element
             ▼
     ┌────────────────────────────┐
     │ if (!m_ui_data.            │
     │    root_ui_element())      │
     │   return error             │
     └────┬───────────────────────┘
          │
          │ Create root (always Panel)
          ▼
     ┌─────────────────────────────────────┐
     │ CreateUIElement(                    │
     │   UIElementDataUnion_PanelData,     │
     │   m_ui_data.root_ui_element())      │
     └────┬────────────────────────────────┘
          │
          ▼
╔═══════════════════════════════════════════╗
║       CreateUIElement (switch on type)    ║
╚═══════════════════════════════════════════╝
          │
          │ Switch on data_type
          ▼
     ┌─────────────────────────────────────┐
     │ case UIElementDataUnion_PanelData:  │
     └────┬────────────────────────────────┘
          │
          │ Cast data
          ▼
     ┌─────────────────────────────────────┐
     │ panel_data = static_cast<           │
     │   const PanelData *>(data)          │
     └────┬────────────────────────────────┘
          │
          │ Create element
          ▼
     ┌─────────────────────────────────────┐
     │ auto panel =                        │
     │   std::make_unique<PanelElement>()  │
     └────┬────────────────────────────────┘
          │
          │ Configure specific type
          ▼
     ┌─────────────────────────────────────┐
     │ ConfigurePanelElement(*panel,       │
     │                       *panel_data)  │
     └────┬────────────────────────────────┘
          │
          │ Get base data
          ▼
     ┌─────────────────────────────────────┐
     │ base_data = panel_data->base_data() │
     └────┬────────────────────────────────┘
          │
          │ Configure base properties
          ▼
     ┌─────────────────────────────────────┐
     │ ConfigureBaseUIElement(*panel,      │
     │                        *base_data)  │
     └────┬────────────────────────────────┘
          │
          │ Set position, size, etc.
          ▼
     ┌─────────────────────────────────────┐
     │ element.position = base_data->      │
     │   position()                        │
     │ element.size = base_data->size()    │
     └────┬────────────────────────────────┘
          │
          │ Convert enums
          ▼
     ┌─────────────────────────────────────┐
     │ element.layout = ConvertLayout(     │
     │   base_data->layout())              │
     │   (Static method)                   │
     └────┬────────────────────────────────┘
          │
          │ Check for children
          ▼
     ┌─────────────────────────────────────┐
     │ if (base_data->children())          │
     └────┬────────────────────────────────┘
          │
          │ YES - Process children
          ▼
     ┌─────────────────────────────────────┐
     │ for child_data in children:         │
     └────┬────────────────────────────────┘
          │
          │ Recursive call
          ▼
     ┌─────────────────────────────────────┐
     │ CreateUIElement(                    │
     │   child_data->child_element_type(), │
     │   child_data->child_element())      │
     │   (RECURSIVE)                       │
     └────┬────────────────────────────────┘
          │
          │ Add child to parent
          ▼
     ┌─────────────────────────────────────┐
     │ element.child_elements.push_back(   │
     │   std::move(child))                 │
     └─────────────────────────────────────┘
          │
          │ Return created element
          ▼
     ┌─────────────────────────────────────┐
     │ return std::move(element)           │
     └─────────────────────────────────────┘
```

---

## Event Data Conversion Flow

### Event Bus Configuration

```
┌────────────────────────────────────────┐
│ event::ConfigureEventHandler           │
│   FromEventBusData()                   │
└────────────┬───────────────────────────┘
             │
             │ Convert EventBusData
             ▼
     ┌─────────────────────────────────────┐
     │ event::ConvertEventBusData          │
     │   ToEventBus(event_bus_data)        │
     │   (Free function)                   │
     └────┬────────────────────────────────┘
          │
          │ Validate input
          ▼
     ┌─────────────────────────────────────┐
     │ if (!event_bus_data)                │
     │   return error                      │
     └────┬────────────────────────────────┘
          │
          │ Create EventBus
          ▼
     ┌─────────────────────────────────────┐
     │ EventBus event_bus;                 │
     └────┬────────────────────────────────┘
          │
          │ Check for events
          ▼
     ┌─────────────────────────────────────┐
     │ if (!event_bus_data->events() ||    │
     │     events->size() == 0)            │
     │   return event_bus (empty)          │
     └────┬────────────────────────────────┘
          │
          │ Loop through events
          ▼
     ┌─────────────────────────────────────┐
     │ for packet_data in events:          │
     └────┬────────────────────────────────┘
          │
          │ Convert packet
          ▼
     ┌─────────────────────────────────────┐
     │ CreateEventPacketFromData(          │
     │   packet_data)                      │
     │   (Free function)                   │
     └────┬────────────────────────────────┘
          │
          │ Extract lifetime and type
          ▼
     ┌─────────────────────────────────────┐
     │ lifetime = packet_data->            │
     │   event_lifetime()                  │
     │ type = packet_data->event_type()    │
     └────┬────────────────────────────────┘
          │
          │ Convert event data
          ▼
     ┌─────────────────────────────────────┐
     │ CreateEventData(                    │
     │   packet_data->event_data_type(),   │
     │   packet_data->event_data())        │
     │   (Free function)                   │
     └────┬────────────────────────────────┘
          │
          │ Switch on union type
          ▼
     ┌─────────────────────────────────────┐
     │ switch (data_type):                 │
     │   case UserInputBitsetData:         │
     │     CreateUserInputBitset()         │
     │   case SceneChangePacketData:       │
     │     CreateSceneChangePacket()       │
     │   case UserInterfaceNameData:       │
     │     CreateUserInterfaceName()       │
     └────┬────────────────────────────────┘
          │
          │ Create EventPacket
          ▼
     ┌─────────────────────────────────────┐
     │ EventPacket packet{                 │
     │   lifetime, type, event_data        │
     │ }                                   │
     └────┬────────────────────────────────┘
          │
          │ Add to bus
          ▼
     ┌─────────────────────────────────────┐
     │ event_bus.push_back(packet)         │
     └─────────────────────────────────────┘
          │
          │ Return EventBus
          ▼
     ┌─────────────────────────────────────┐
     │ return event_bus                    │
     └────┬────────────────────────────────┘
          │
          │ Back to ConfigureEventHandler
          ▼
     ┌─────────────────────────────────────┐
     │ for event in event_bus:             │
     │   event_handler.AddEvent(event)     │
     └────┬────────────────────────────────┘
          │
          │ Process waiting room
          ▼
     ┌─────────────────────────────────────┐
     │ event_handler.                      │
     │   ProcessWaitingRoomEventBus()      │
     └─────────────────────────────────────┘
```

---

## Pattern Usage by Layer

### Layer 0 → Layer 1 Transitions

**Pattern: Interface + Implementation (Data Providers)**

```
┌────────────────────────────────────────┐
│         Layer 0 (FlatBuffers)          │
│  SceneDataFbs, EngineDataFbs, etc.     │
└────────────┬───────────────────────────┘
             │
             │ IDataProvider interface
             │ (Polymorphism point)
             │
             ▼
     ┌─────────────────────────────┐
     │  FlatbuffersDataProvider    │
     │  - LoadData() override      │
     │  - Private Populate methods │
     └────────┬────────────────────┘
              │
              │ Uses free functions for
              │ simple conversions
              │
              ▼
     ┌─────────────────────────────┐
     │  Free Functions             │
     │  - ConfigureAssetConfig()   │
     │  - CreateSubscriber()       │
     └────────┬────────────────────┘
              │
              ▼
┌────────────────────────────────────────┐
│        Layer 1 (Native Structs)        │
│  SceneData, EngineData, EventPacket    │
└────────────────────────────────────────┘
```

### Layer 1 → Layer 2 Transitions

**Pattern: Interface + Implementation (Configurators)**

```
┌────────────────────────────────────────┐
│       Layer 1 (Native Structs)         │
│  SceneData, EntityCollectionData, etc. │
└────────────┬───────────────────────────┘
             │
             │ IConfigurator interface
             │ (Polymorphism point)
             │
             ▼
     ┌─────────────────────────────┐
     │  FlatbuffersConfigurator    │
     │  - Configure() override     │
     │  - Overloaded methods       │
     └────────┬────────────────────┘
              │
              │ Overloaded for each type
              │
              ▼
     ┌─────────────────────────────┐
     │  Overloaded Methods         │
     │  - ConfigureCUserInterface()│
     │  - ConfigureCGrimoire...()  │
     │  - ConfigureButtonElement() │
     └────────┬────────────────────┘
              │
              │ Uses static methods for
              │ enum conversions
              │
              ▼
     ┌─────────────────────────────┐
     │  Static Methods             │
     │  - ConvertLayout()          │
     │  - ConvertSpacingAndSizing()│
     └────────┬────────────────────┘
              │
              ▼
┌────────────────────────────────────────┐
│        Layer 2 (Game Objects)          │
│  Scene, EntityMemoryPool, UIElement    │
└────────────────────────────────────────┘
```

---

## Data Access Factory Flow

### Runtime Data Source Selection

```
┌────────────────────────────────────────┐
│      Application Initialization        │
└────────────┬───────────────────────────┘
             │
             │ Create factory
             ▼
     ┌─────────────────────────────────────┐
     │ DataAccessFactory factory(          │
     │   event_handler,                    │
     │   DataType::Flatbuffers)            │
     └────────┬────────────────────────────┘
              │
              │ Constructor calls
              ▼
     ┌─────────────────────────────────────┐
     │ SetDataProviders()                  │
     └────────┬────────────────────────────┘
              │
              │ Switch on m_data_type
              ▼
     ┌─────────────────────────────────────┐
     │ switch (m_data_type):               │
     │   case DataType::Flatbuffers:       │
     │     SetFlatbuffersDataProviders()   │
     └────────┬────────────────────────────┘
              │
              ▼
     ┌─────────────────────────────────────┐
     │ SetFlatbuffersDataProviders()       │
     ├─────────────────────────────────────┤
     │ Creates:                            │
     │ - FlatbuffersEngineDataProvider     │
     │ - FlatbuffersSceneDataProvider      │
     │ - FlatbuffersSceneManager...        │
     └────────┬────────────────────────────┘
              │
              │ Store in factory
              ▼
     ┌─────────────────────────────────────┐
     │ m_engine_data_provider =            │
     │   std::make_unique<...>()           │
     │ m_scene_data_provider =             │
     │   std::make_unique<...>()           │
     └────────┬────────────────────────────┘
              │
              │ Factory ready
              ▼
     ┌─────────────────────────────────────┐
     │     Factory Provides Pointers       │
     ├─────────────────────────────────────┤
     │ GetEngineDataProvider()             │
     │ GetSceneDataProvider()              │
     │ GetSceneManagerDataProvider()       │
     └─────────────────────────────────────┘
              │
              │ Game systems request providers
              ▼
     ┌─────────────────────────────────────┐
     │ auto provider =                     │
     │   factory.GetSceneDataProvider()    │
     │   .value()                          │
     └────────┬────────────────────────────┘
              │
              │ Use through interface
              ▼
     ┌─────────────────────────────────────┐
     │ auto scene_data =                   │
     │   provider->LoadSceneData(type)     │
     │   (Virtual call)                    │
     └─────────────────────────────────────┘
```

### Future: Adding JSON Support

```
┌────────────────────────────────────────┐
│ DataAccessFactory factory(             │
│   event_handler,                       │
│   DataType::JSON)  // <-- New type     │
└────────────┬───────────────────────────┘
             │
             │ Switch in SetDataProviders()
             ▼
     ┌─────────────────────────────────────┐
     │ switch (m_data_type):               │
     │   case DataType::Flatbuffers:       │
     │     SetFlatbuffersDataProviders()   │
     │   case DataType::JSON:              │
     │     SetJSONDataProviders()  // NEW  │
     └────────┬────────────────────────────┘
              │
              ▼
     ┌─────────────────────────────────────┐
     │ SetJSONDataProviders()              │
     ├─────────────────────────────────────┤
     │ Creates:                            │
     │ - JSONEngineDataProvider            │
     │ - JSONSceneDataProvider             │
     │ - JSONSceneManagerDataProvider      │
     └────────┬────────────────────────────┘
              │
              │ Same interfaces, different impl
              ▼
     ┌─────────────────────────────────────┐
     │ Game code unchanged!                │
     │ Uses IDataProvider* interface       │
     └─────────────────────────────────────┘
```

---

## Summary

### Key Takeaways

1. **Three-Layer Architecture**: Layer 0 (External) → Layer 1 (Native) → Layer 2 (Game Objects)

2. **Layer Transitions**:
   - Layer 0→1: Data Providers (load and convert)
   - Layer 1→2: Configurators (configure objects)

3. **Pattern Usage**:
   - **Interfaces**: For extensibility across data sources
   - **Overloaded Methods**: For type-specific configuration
   - **Free Functions**: For simple, reusable conversions
   - **Static Methods**: For class-specific conversions
   - **Private Populate**: For decomposing complex operations

4. **Data Flow Direction**: Always unidirectional (Layer 0 → Layer 1 → Layer 2)

5. **No Backward Dependencies**: Layer 2 never accesses Layer 0 directly

---

**Last Updated**: 2026-01-13
