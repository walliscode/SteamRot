# SteamRot Testing Architecture Layers

This document provides a visual representation of the SteamRot architecture organized into testable layers.

## Overview

The SteamRot engine is organized into 7 distinct layers, where each layer depends only on layers below it. This creates a clear testing strategy: **test from the bottom up**.

## Layer Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                         Layer 6: Engine                         │
│                                                                 │
│  GameEngine - Top-level coordination, game loop, error handling │
│                                                                 │
│  Dependencies: All layers below                                │
└─────────────────────────────────────────────────────────────────┘
                              ↑
┌─────────────────────────────────────────────────────────────────┐
│                         Layer 5: Scenes                         │
│                                                                 │
│  TitleScene, CraftingScene - Game state coordination           │
│  SceneManager, SceneFactory - Scene lifecycle                  │
│                                                                 │
│  Dependencies: Layers 0-4                                       │
└─────────────────────────────────────────────────────────────────┘
                              ↑
┌─────────────────────────────────────────────────────────────────┐
│                        Layer 4: Logic                           │
│                                                                 │
│  UIRenderLogic, UIActionLogic, UICollisionLogic, UIStateLogic  │
│  CraftingRenderLogic, LogicFactory                             │
│                                                                 │
│  Dependencies: Layers 0-3                                       │
└─────────────────────────────────────────────────────────────────┘
                              ↑
┌─────────────────────────────────────────────────────────────────┐
│                        Layer 3: Managers                        │
│                                                                 │
│  EntityManager    - Entity lifecycle and memory                 │
│  AssetManager     - Font and UI style management                │
│  EventHandler     - Event bus and routing                       │
│  DisplayManager   - Window and rendering                        │
│  SceneManager     - Scene transitions (needs tests)             │
│                                                                 │
│  Dependencies: Layers 0-2                                       │
└─────────────────────────────────────────────────────────────────┘
                              ↑
┌─────────────────────────────────────────────────────────────────┐
│              Layer 2: Data Providers & Configurators            │
│                                                                 │
│  FlatbuffersDataLoader        - Main data coordinator           │
│  FlatbuffersAssetDataProvider - Asset loading                   │
│  FlatbuffersEngineDataProvider - Engine config                  │
│  FlatbuffersSceneDataProvider  - Scene data                     │
│  FlatbuffersEntityConfigurator - Entity configuration           │
│  FlatbuffersSceneConfigurator  - Scene configuration            │
│  FlatbuffersUIElementConfigurator - UI configuration            │
│                                                                 │
│  Dependencies: Layers 0-1                                       │
└─────────────────────────────────────────────────────────────────┘
                              ↑
┌─────────────────────────────────────────────────────────────────┐
│                       Layer 1: Components                       │
│                                                                 │
│  CMeta            - Entity metadata                             │
│  CUserInterface   - UI component data                           │
│  CGrimoireMachina - Grimoire system data                        │
│  CMachinaForm     - Machina form data                           │
│  CUIState         - UI state data                               │
│                                                                 │
│  Dependencies: Layer 0 only                                     │
└─────────────────────────────────────────────────────────────────┘
                              ↑
┌─────────────────────────────────────────────────────────────────┐
│                  Layer 0: Core Types & Utilities                │
│                                                                 │
│  Core Types:          Events:              UI Elements:         │
│  - FailInfo           - EventPacket        - UIElement          │
│  - SceneConfig        - Subscriber         - ButtonElement      │
│  - EngineConfig       - UserInputBitset    - PanelElement       │
│  - SceneInfo                               - DropDownList...    │
│  - Joint, Fragment                                              │
│                                                                 │
│  Logger:                                                        │
│  - Logger                                                       │
│                                                                 │
│  Dependencies: None (pure data, SFML, std library)              │
└─────────────────────────────────────────────────────────────────┘
```

## Layer Details

### Layer 0: Core Types & Utilities

**Purpose**: Foundation data structures with no game-specific dependencies

**Characteristics**:
- Pure data structures
- No dependencies on other layers
- Can use standard library and SFML types
- Simple value semantics

**Test Focus**:
- Default construction
- Field initialization
- Data integrity
- Copy/move semantics

**Examples**:
```cpp
// Core types
steamrot::FailInfo
steamrot::SceneConfig
steamrot::EngineConfig

// Event types
steamrot::EventPacket
steamrot::Subscriber
steamrot::UserInputBitset

// UI types
steamrot::UIElement
steamrot::ButtonElement
steamrot::PanelElement
```

**Current Status**: ⚠️ Partial coverage

---

### Layer 1: Components

**Purpose**: Pure data containers for entity-component system

**Characteristics**:
- Inherit from `Component` base
- No logic (only data + `GetComponentRegisterIndex()`)
- Must be default-constructible
- Registered in `ComponentRegister` tuple

**Test Focus**:
- Default construction
- Default values
- Component register index
- Active flag behavior
- Copy construction

**Examples**:
```cpp
steamrot::CMeta
steamrot::CUserInterface
steamrot::CGrimoireMachina
steamrot::CMachinaForm
steamrot::CUIState
```

**Current Status**: ✅ Good coverage

---

### Layer 2: Data Providers & Configurators

**Purpose**: Load data from files and configure objects

**Characteristics**:
- Handle FlatBuffers data loading
- Configure objects from data
- Validate data integrity
- Null-safe field access

**Test Focus**:
- Successful data loading
- Error handling (missing files)
- Null safety (FlatBuffers fields)
- Data validation
- Component activation

**Examples**:
```cpp
// Providers
steamrot::FlatbuffersDataLoader
steamrot::FlatbuffersAssetDataProvider
steamrot::FlatbuffersEngineDataProvider

// Configurators
steamrot::FlatbuffersEntityConfigurator
steamrot::FlatbuffersSceneConfigurator
```

**Current Status**: ✅ Good coverage (providers), ⚠️ Some configurators missing

---

### Layer 3: Managers

**Purpose**: Resource management and system coordination

**Characteristics**:
- Manage resource lifecycles
- Coordinate between systems
- Track system state
- Provide access to resources

**Test Focus**:
- Construction
- Resource operations (add, get, remove)
- State management
- Error conditions
- Memory safety

**Examples**:
```cpp
// Entity
steamrot::EntityManager
steamrot::ArchetypeManager

// Assets
steamrot::AssetManager

// Events
steamrot::EventHandler

// Display
steamrot::DisplayManager
```

**Current Status**: ⚠️ Partial coverage

---

### Layer 4: Logic

**Purpose**: Game system behavior and entity processing

**Characteristics**:
- Inherit from `Logic` base class
- Process entities by archetype
- Organized by type (Action, Render, Collision, Movement)
- Use `SceneContext` for resources

**Test Focus**:
- Construction with SceneContext
- RunLogic() execution
- Entity processing by archetype
- State changes
- Subscriber handling

**Examples**:
```cpp
steamrot::UIRenderLogic
steamrot::UIActionLogic
steamrot::UICollisionLogic
steamrot::UIStateLogic
steamrot::CraftingRenderLogic
steamrot::LogicFactory
```

**Current Status**: ✅ Good coverage

---

### Layer 5: Scenes

**Purpose**: Game state coordination and scene management

**Characteristics**:
- Inherit from `Scene` base class
- Implement system methods (sRender, sAction, etc.)
- Contain EntityManager, ArchetypeManager
- Hold scene-specific resources and state

**Test Focus**:
- Construction with GameContext
- Scene type initialization
- Render texture validity
- System method execution
- Entity creation
- Active state management

**Examples**:
```cpp
steamrot::TitleScene
steamrot::CraftingScene
steamrot::SceneFactory
steamrot::SceneManager
```

**Current Status**: ❌ Missing (Critical priority)

---

### Layer 6: Engine

**Purpose**: Top-level game coordination and game loop

**Characteristics**:
- Coordinates all systems
- Manages game loop
- Handles top-level error recovery
- Entry point to application

**Test Focus**:
- Construction
- Subsystem initialization
- Configuration loading
- Scene creation
- Resource initialization
- Error handling

**Examples**:
```cpp
steamrot::GameEngine
```

**Current Status**: ⚠️ Basic coverage

---

## Dependency Flow

### Bottom-Up Dependencies

Each layer can depend on layers below it:

```
Layer 6 → depends on → Layers 0-5
Layer 5 → depends on → Layers 0-4
Layer 4 → depends on → Layers 0-3
Layer 3 → depends on → Layers 0-2
Layer 2 → depends on → Layers 0-1
Layer 1 → depends on → Layer 0
Layer 0 → depends on → Nothing (standard library, SFML)
```

### Testing Strategy

**Test from bottom to top**:

1. ✅ **First**: Test Layer 0 (no dependencies)
2. ✅ **Second**: Test Layer 1 (only depends on Layer 0)
3. ✅ **Third**: Test Layer 2 (depends on Layers 0-1)
4. ⚠️ **Fourth**: Test Layer 3 (depends on Layers 0-2)
5. ✅ **Fifth**: Test Layer 4 (depends on Layers 0-3)
6. ❌ **Sixth**: Test Layer 5 (depends on Layers 0-4) - **NEEDS WORK**
7. ⚠️ **Seventh**: Test Layer 6 (depends on all layers)

### Benefits of Layer-Based Testing

1. **Clear dependencies**: Each layer's dependencies are explicit
2. **Incremental testing**: Test foundations before building on them
3. **Isolation**: Each layer can be tested in isolation
4. **Debugging**: Issues are easier to locate by layer
5. **Refactoring**: Changes to lower layers immediately show impact
6. **Onboarding**: New developers understand architecture through testing

## Directory Mapping

### Source Code Structure

```
src/
├── types/              → Layer 0
├── logger/             → Layer 0
├── components/         → Layer 1
├── data_providers/     → Layer 2
├── configuration/      → Layer 2
├── entity/             → Layer 2 (configurators), Layer 3 (managers)
├── user_interface/     → Layer 2 (configurators)
├── assets/             → Layer 3
├── events/             → Layer 3
├── display/            → Layer 3
├── logic/              → Layer 4
├── scenes/             → Layer 5
└── engine/             → Layer 6
```

### Test Structure

```
tests/unit/
├── types/              → Layer 0 (some exist, needs expansion)
├── components/         → Layer 1 (good coverage)
├── data_providers/     → Layer 2 (good coverage)
├── entity/             → Layer 2 & 3 (good coverage)
├── user_interface/     → Layer 2 (partial coverage)
├── assets/             → Layer 3 (good coverage)
├── events/             → Layer 3 (good coverage)
├── display/            → Layer 3 (basic coverage)
├── logic/              → Layer 4 (good coverage)
├── scenes/             → Layer 5 (MISSING - needs creation)
└── engine/             → Layer 6 (basic coverage)
```

## Coverage Summary

### Status Legend
- ✅ **Good**: Comprehensive tests exist
- ⚠️ **Partial**: Some tests exist, needs expansion
- ❌ **Missing**: No tests exist (critical)

### Layer Status

| Layer | Status | Test Coverage | Priority |
|-------|--------|---------------|----------|
| 0: Types | ⚠️ Partial | ~50% | Medium |
| 1: Components | ✅ Good | ~90% | Low |
| 2: Data/Config | ✅ Good | ~80% | Low |
| 3: Managers | ⚠️ Partial | ~60% | High |
| 4: Logic | ✅ Good | ~85% | Low |
| 5: Scenes | ❌ Missing | 0% | **Critical** |
| 6: Engine | ⚠️ Partial | ~30% | High |

### Next Priorities

1. **Immediate**: Create Layer 5 (Scenes) tests
2. **High**: Expand Layer 3 (Managers) tests
3. **High**: Expand Layer 6 (Engine) tests
4. **Medium**: Complete Layer 0 (Types) tests

## Using This Document

### For Test Implementation

1. **Identify your class's layer**
2. **Ensure lower layers are tested**
3. **Follow layer-specific patterns**
4. **Use TestFixture for higher layers**

### For Architecture Understanding

1. **Study the dependency flow**
2. **Understand what each layer provides**
3. **See how layers build on each other**
4. **Use layers to navigate codebase**

### For Code Reviews

1. **Verify dependency rules are followed**
2. **Check tests exist for the correct layer**
3. **Ensure lower layers are tested first**
4. **Validate test patterns match layer**

## Related Documentation

- **[Unit Testing Guide](UNIT_TESTING_GUIDE.md)** - Detailed testing instructions
- **[Testing Quick Reference](TESTING_QUICK_REFERENCE.md)** - Quick patterns
- **[Testing Examples](TESTING_EXAMPLES.md)** - Complete code examples
- **[README](README.md)** - Testing documentation overview

---

**Last Updated**: 2025-12-17  
**Document Version**: 1.0
