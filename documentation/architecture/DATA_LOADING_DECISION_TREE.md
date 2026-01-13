# Data Loading Pattern Decision Tree

## Quick Reference Guide

This document provides a visual decision tree to help you choose the right pattern for data loading and configuration in the SteamRot engine.

---

## Decision Tree

```
START: Need to load/configure data
│
├─ Q1: Is this a HIGH-LEVEL data provider?
│      (Engine, Scene, SceneManager level)
│      │
│      ├─ YES → Use Interface + Implementation Pattern
│      │         Examples: IEngineDataProvider, ISceneDataProvider
│      │         Files: src/types/interfaces/ + src/data_providers/
│      │
│      └─ NO → Continue to Q2
│             │
│             ├─ Q2: Is this a CONFIGURATOR?
│             │      (Configures Layer 2 objects from Layer 1 data)
│             │      │
│             │      ├─ YES → Use Interface + Implementation Pattern
│             │      │         Examples: IEntityConfigurator, IUIElementConfigurator
│             │      │         Files: src/types/interfaces/ + src/entity/, src/user_interface/
│             │      │
│             │      └─ NO → Continue to Q3
│             │             │
│             │             ├─ Q3: Does it handle MULTIPLE TYPES?
│             │             │      (Different components, UI elements, etc.)
│             │             │      │
│             │             │      ├─ YES → Use Overloaded Methods
│             │             │      │         Within the configurator class
│             │             │      │         Examples: ConfigureComponent(CUserInterface&)
│             │             │      │                   ConfigureButtonElement()
│             │             │      │
│             │             │      └─ NO → Continue to Q4
│             │             │             │
│             │             │             ├─ Q4: Is it a SIMPLE CONVERSION?
│             │             │             │      (Enum, small struct, utility)
│             │             │             │      │
│             │             │             │      ├─ YES → Continue to Q5
│             │             │             │      │         │
│             │             │             │      │         ├─ Q5: Is it REUSABLE across classes?
│             │             │             │      │         │      │
│             │             │             │      │         │      ├─ YES → Free Function
│             │             │             │      │         │      │         Examples: ConvertEventBusDataToEventBus()
│             │             │             │      │         │      │                   CreateSubscriber()
│             │             │             │      │         │      │         Files: src/events/, src/data_providers/
│             │             │             │      │         │      │
│             │             │             │      │         │      └─ NO → Static Method
│             │             │             │      │         │                Examples: ConvertLayout()
│             │             │             │      │         │                          ConvertSpacingAndSizing()
│             │             │             │      │         │                Inside: FlatbuffersUIElementConfigurator
│             │             │             │      │         │
│             │             │             │      │         └─ (If complex, go to Q6)
│             │             │             │      │
│             │             │             │      └─ NO → Continue to Q6
│             │             │             │             │
│             │             │             │             └─ Q6: Is it DECOMPOSING a large operation?
│             │             │             │                    (Breaking down complex logic)
│             │             │             │                    │
│             │             │             │                    ├─ YES → Private Populate Methods
│             │             │             │                    │         Examples: PopulateEngineConfig()
│             │             │             │                    │                   PopulateEngineState()
│             │             │             │                    │         Inside: FlatbuffersEngineDataProvider
│             │             │             │                    │
│             │             │             │                    └─ NO → Re-evaluate design
│             │             │             │                              (May need refactoring)
```

---

## Pattern Selection Matrix

| Question | YES → Pattern | NO → Next Question |
|----------|--------------|-------------------|
| **Q1**: High-level provider? | Interface + Implementation | Q2 |
| **Q2**: Configurator? | Interface + Implementation | Q3 |
| **Q3**: Multiple types? | Overloaded Methods | Q4 |
| **Q4**: Simple conversion? | Q5 | Q6 |
| **Q5**: Reusable? | Free Function | Static Method |
| **Q6**: Decomposing? | Private Populate Methods | Re-evaluate |

---

## Examples by Pattern

### Pattern 1: Interface + Implementation

**Trigger**: Q1 (High-level provider) or Q2 (Configurator)

**Use When**:
- Creating data providers for Engine, Scene, SceneManager
- Creating configurators for entities, UI elements, scenes
- Need to support multiple data sources in future

**Example**:
```cpp
// Interface
class IEngineDataProvider {
public:
    virtual ~IEngineDataProvider() = default;
    virtual std::expected<EngineData, FailInfo> LoadEngineData() const = 0;
};

// Concrete
class FlatbuffersEngineDataProvider : public IEngineDataProvider {
    std::expected<EngineData, FailInfo> LoadEngineData() const override;
};
```

**Files**:
- Interface: `src/types/interfaces/I*.h`
- Concrete: `src/data_providers/Flatbuffers*.h/cpp`

---

### Pattern 2: Overloaded Methods

**Trigger**: Q3 (Multiple types within one data source)

**Use When**:
- Configuring different component types
- Configuring different UI element types
- Type-specific behavior within a single configurator

**Example**:
```cpp
class FlatbuffersEntityConfigurator : public IEntityConfigurator {
private:
    // Overloaded for different component types
    std::expected<std::monostate, FailInfo>
    ConfigureComponent(Component &component);
    
    std::expected<std::monostate, FailInfo>
    ConfigureComponent(CUserInterface &component);
    
    std::expected<std::monostate, FailInfo>
    ConfigureComponent(CGrimoireMachina &component);
};
```

**Files**:
- Inside configurator classes: `src/entity/Flatbuffers*Configurator.cpp`

---

### Pattern 3: Free Function

**Trigger**: Q4 → YES, Q5 → YES (Simple, reusable conversion)

**Use When**:
- Converting event bus data
- Creating subscribers or event packets
- Configuring asset config (used by multiple places)
- Stateless utilities

**Example**:
```cpp
namespace steamrot::event {
    std::expected<EventBus, FailInfo>
    ConvertEventBusDataToEventBus(const EventBusData *event_bus_data);
}

namespace steamrot {
    std::expected<std::monostate, FailInfo>
    ConfigureAssetConfig(AssetConfig &asset_config,
                        const AssetConfigFbs *asset_config_fb_data);
}
```

**Files**:
- `src/events/event_bus_conversion.h/cpp`
- `src/events/event_factory.h/cpp`
- `src/events/subscriber_factory.h/cpp`
- `src/data_providers/asset_config_factory.h/cpp`

---

### Pattern 4: Static Method

**Trigger**: Q4 → YES, Q5 → NO (Simple conversion, tied to one class)

**Use When**:
- Enum conversions specific to one configurator
- Type conversions logically grouped with a class
- No instance state needed

**Example**:
```cpp
class FlatbuffersUIElementConfigurator : public IUIElementConfigurator {
public:
    static Layout ConvertLayout(int8_t fbs_layout);
    static SpacingAndSizing ConvertSpacingAndSizing(int8_t fbs_spacing);
};
```

**Files**:
- Inside configurator classes: `src/user_interface/FlatbuffersUIElementConfigurator.h/cpp`

---

### Pattern 5: Private Populate Methods

**Trigger**: Q6 (Decomposing complex operation)

**Use When**:
- Breaking down large LoadData() or Configure() method
- Each sub-operation has single responsibility
- Need better organization and readability

**Example**:
```cpp
class FlatbuffersEngineDataProvider : public IEngineDataProvider {
private:
    std::expected<std::monostate, FailInfo>
    PopulateEngineResourcesConfig(EngineResourcesConfig &config,
                                 const EngineResourcesConfigFbs *data) const;
    
    std::expected<std::monostate, FailInfo>
    PopulateEngineConfig(EngineConfig &config,
                        const EngineConfigFbs *data) const;
    
    std::expected<std::monostate, FailInfo>
    PopulateEngineState(EngineState &state,
                       const EngineStateFbs *data) const;
    
public:
    std::expected<EngineData, FailInfo> LoadEngineData() const override {
        // Calls Populate methods
    }
};
```

**Files**:
- Inside provider classes: `src/data_providers/Flatbuffers*Provider.cpp`

---

## Common Scenarios

### Scenario 1: Adding a New Component Type

```
1. Component definition exists in src/types/components/
2. FlatBuffers schema exists in src/types/flatbuffers/entities/
3. Need to configure component in entities

Decision Path: Q1→NO, Q2→NO, Q3→YES
→ Use Overloaded Methods

Implementation:
- Add overloaded ConfigureComponent() method in FlatbuffersEntityConfigurator
- Method signature: ConfigureComponent(CNewComponent &component)
```

### Scenario 2: Adding a New UI Element Type

```
1. UIElement definition exists in src/types/user_interface/
2. FlatBuffers schema exists in src/types/flatbuffers/entities/
3. Need to configure UI element

Decision Path: Q1→NO, Q2→NO, Q3→YES
→ Use Overloaded Methods

Implementation:
- Add overloaded ConfigureXXXElement() method in FlatbuffersUIElementConfigurator
- Method signature: ConfigureXXXElement(XXXElement &element, const XXXData &data)
```

### Scenario 3: Adding Event Conversion

```
1. New event type needs conversion from FlatBuffers to native
2. Used by multiple configurators
3. Simple, stateless conversion

Decision Path: Q1→NO, Q2→NO, Q3→NO, Q4→YES, Q5→YES
→ Use Free Function

Implementation:
- Add function to src/events/event_factory.h/cpp
- Function signature: CreateNewEventType(const NewEventData *data)
- Place in steamrot::event namespace
```

### Scenario 4: Adding High-Level Data Provider

```
1. New subsystem needs data loading (e.g., AudioData)
2. Might support JSON in future
3. High-level provider

Decision Path: Q1→YES
→ Use Interface + Implementation

Implementation:
- Create IAudioDataProvider in src/types/interfaces/
- Create FlatbuffersAudioDataProvider in src/data_providers/
- Add to DataAccessFactory
```

### Scenario 5: Complex Data Provider with Many Fields

```
1. Loading EngineData with multiple sub-structs
2. Single method would be >100 lines
3. Need to decompose

Decision Path: Q1→YES (interface), then Q6→YES (decompose)
→ Use Interface + Implementation + Private Populate Methods

Implementation:
- Interface in src/types/interfaces/IEngineDataProvider.h
- Concrete in src/data_providers/FlatbuffersEngineDataProvider.h/cpp
- Private PopulateXXX() methods in concrete class
```

---

## Anti-Patterns to Avoid

### ❌ Don't: Mix Layer 0 and Layer 2

```cpp
// BAD: Scene directly accessing FlatBuffers
class Scene {
    void LoadData(const SceneDataFbs *fb_data) {  // ❌
        // Never access FlatBuffers directly in game objects
    }
};
```

**Why**: Breaks layer isolation, couples Scene to FlatBuffers format.

**Fix**: Use Layer 1 native structs as intermediary.

### ❌ Don't: Create Free Functions When State is Needed

```cpp
// BAD: Free function needs to track state
std::expected<UIElement, FailInfo>
CreateUIElement(const UIElementData *data) {  // ❌
    // Can't maintain state across calls
    // Can't access m_event_handler
}
```

**Why**: Free functions are stateless. Use class methods instead.

**Fix**: Use configurator class with member variables.

### ❌ Don't: Make Everything an Interface

```cpp
// BAD: Unnecessary interface for simple conversion
class ILayoutConverter {  // ❌
    virtual Layout Convert(int8_t fbs_layout) = 0;
};

class FlatbuffersLayoutConverter : public ILayoutConverter {
    Layout Convert(int8_t fbs_layout) override { ... }
};
```

**Why**: Over-engineering for a simple enum conversion.

**Fix**: Use static method or free function.

### ❌ Don't: Put Conversion Logic in Game Objects

```cpp
// BAD: Entity knows about FlatBuffers
class Entity {
    void LoadFromFlatbuffers(const EntityDataFbs *fb_data) {  // ❌
        // Game objects shouldn't know about data formats
    }
};
```

**Why**: Couples game logic to data format.

**Fix**: Use configurator to configure Entity from Layer 1 data.

---

## Quick Lookup Table

| What Are You Doing? | Pattern to Use |
|-------------------|----------------|
| Loading Engine/Scene/SceneManager data | Interface + Implementation (IDataProvider) |
| Configuring entities/UI/scenes | Interface + Implementation (IConfigurator) |
| Configuring different component types | Overloaded Methods (within configurator) |
| Converting FlatBuffers enum to native enum | Static Method (if tied to one class) OR Free Function (if reusable) |
| Converting event data structures | Free Function (in event_factory.h) |
| Creating subscribers | Free Function (in subscriber_factory.h) |
| Breaking down large provider method | Private Populate Methods (within class) |
| Configuring asset config | Free Function (reusable by multiple classes) |

---

## Summary

### The Golden Rules

1. **High-level providers and configurators** → Interface + Implementation
2. **Multiple types within one data source** → Overloaded Methods
3. **Simple, reusable conversions** → Free Function
4. **Simple, class-specific conversions** → Static Method
5. **Large operations** → Break down with Private Populate Methods

### When in Doubt

Ask yourself:
1. **Will I need multiple data sources?** → Interface
2. **Is it reusable across classes?** → Free Function
3. **Does it handle multiple types?** → Overloaded Methods
4. **Is it too large?** → Decompose with Private Populate Methods

---

**Last Updated**: 2026-01-13
