# Data Loading Architecture Diagrams

## Visual Reference

This document provides visual diagrams to supplement the [Data Loading and Configuration Architecture](DATA_LOADING_AND_CONFIGURATION.md) documentation.

## Architecture Overview

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                         Game Logic Layer                        │
│                                                                 │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐      │
│  │  Scene   │  │  Engine  │  │  Logic   │  │   UI     │      │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘      │
│       │             │              │             │             │
└───────┼─────────────┼──────────────┼─────────────┼─────────────┘
        │ uses        │ uses         │ uses        │ uses
        ↓             ↓              ↓             ↓
┌─────────────────────────────────────────────────────────────────┐
│                      Native Object Layer                        │
│                     (Format Agnostic)                           │
│                                                                 │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐      │
│  │SceneData │  │EngineData│  │SaveData  │  │ UIStyle  │      │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘      │
│       │             │              │             │             │
└───────┼─────────────┼──────────────┼─────────────┼─────────────┘
        │ configured  │ configured   │ configured  │ configured
        │ by          │ by           │ by          │ by
        ↓             ↓              ↓             ↓
┌─────────────────────────────────────────────────────────────────┐
│              Provider/Configurator Layer                        │
│                   (Interface Based)                             │
│                                                                 │
│  ┌──────────────────┐        ┌──────────────────┐             │
│  │  IDataProvider   │        │  IConfigurator   │             │
│  │  - Provide()     │        │  - Configure()   │             │
│  └────────┬─────────┘        └────────┬─────────┘             │
│           │                           │                         │
│           ├───────────┬───────────────┼───────────┐            │
│           │           │               │           │            │
│           ↓           ↓               ↓           ↓            │
│  ┌──────────────┐ ┌──────────────┐ ┌──────────────┐          │
│  │ Flatbuffers  │ │     XML      │ │    JSON      │          │
│  │ Providers    │ │  Providers   │ │  Providers   │          │
│  └──────┬───────┘ └──────┬───────┘ └──────┬───────┘          │
│         │                │                │                    │
└─────────┼────────────────┼────────────────┼────────────────────┘
          │ reads          │ reads          │ reads
          ↓                ↓                ↓
┌─────────────────────────────────────────────────────────────────┐
│                        Data Layer                               │
│                  (Format Specific)                              │
│                                                                 │
│  ┌──────────────┐ ┌──────────────┐ ┌──────────────┐          │
│  │ .fbs files   │ │  .xml files  │ │ .json files  │          │
│  │  (binary)    │ │   (text)     │ │   (text)     │          │
│  └──────────────┘ └──────────────┘ └──────────────┘          │
└─────────────────────────────────────────────────────────────────┘
```

## Component Relationships

### DataAccessFactory Pattern

```
┌────────────────────────────────────────────────────────────────┐
│                      DataAccessFactory                         │
│                                                                │
│  ┌─────────────────────────────────────────────────────────┐  │
│  │  Member Variables                                       │  │
│  │  ─────────────────                                      │  │
│  │  m_data_type: DataType                                  │  │
│  │  m_engine_data_provider: unique_ptr<IEngineDataProvider>│  │
│  │  m_scene_data_provider: unique_ptr<ISceneDataProvider>  │  │
│  │  m_scene_manager_provider: unique_ptr<ISceneManager...> │  │
│  │  m_scene_configurator: unique_ptr<ISceneConfigurator>   │  │
│  └─────────────────────────────────────────────────────────┘  │
│                                                                │
│  ┌─────────────────────────────────────────────────────────┐  │
│  │  Methods                                                │  │
│  │  ───────                                                │  │
│  │  + GetEngineDataProvider() → IEngineDataProvider*      │  │
│  │  + GetSceneDataProvider() → ISceneDataProvider*        │  │
│  │  + GetSceneConfigurator() → ISceneConfigurator*        │  │
│  │  + SetDataType(DataType) → std::expected               │  │
│  │  - SetFlatbuffersDataProviders() → std::expected       │  │
│  │  - SetXmlDataProviders() → std::expected               │  │
│  └─────────────────────────────────────────────────────────┘  │
└────────────────────────────────────────────────────────────────┘
                              │
                              │ manages
                              ↓
        ┌──────────────────────────────────────────┐
        │                                          │
        ↓                                          ↓
┌───────────────────┐                    ┌───────────────────┐
│  Provider         │                    │  Configurator     │
│  Instances        │                    │  Instances        │
│                   │                    │                   │
│  Flatbuffers...   │                    │  Flatbuffers...   │
│  or XML...        │                    │  or XML...        │
│  or JSON...       │                    │  or JSON...       │
└───────────────────┘                    └───────────────────┘
```

### Provider Pattern Flow

```
┌──────────┐         ┌─────────────────┐         ┌──────────────┐
│  Client  │         │   IDataProvider │         │ Native Object│
│  Code    │         │   (Interface)   │         │  (SceneData) │
└────┬─────┘         └────────┬────────┘         └──────┬───────┘
     │                        │                         │
     │ 1. GetProvider()       │                         │
     │───────────────────────>│                         │
     │                        │                         │
     │ 2. ProvideData()       │                         │
     │───────────────────────>│                         │
     │                        │                         │
     │                        │ 3. Load file            │
     │                        │──────────┐              │
     │                        │          │              │
     │                        │<─────────┘              │
     │                        │                         │
     │                        │ 4. Configure native obj │
     │                        │────────────────────────>│
     │                        │                         │
     │                        │ 5. Return native obj    │
     │<─────────────────────────────────────────────────│
     │                        │                         │
     │ 6. Use native object   │                         │
     │ (format agnostic!)     │                         │
     │                        │                         │
```

### Configurator Pattern Flow

```
┌──────────┐      ┌────────────────┐      ┌────────────┐      ┌──────────┐
│  Client  │      │ IConfigurator  │      │   Object   │      │   Data   │
│  Code    │      │  (Interface)   │      │ (Scene)    │      │(SceneData)│
└────┬─────┘      └────────┬───────┘      └─────┬──────┘      └────┬─────┘
     │                     │                    │                   │
     │ 1. Get configurator │                    │                   │
     │────────────────────>│                    │                   │
     │                     │                    │                   │
     │ 2. Create object    │                    │                   │
     │────────────────────────────────────────>│                   │
     │                     │                    │                   │
     │ 3. Have data        │                    │                   │
     │───────────────────────────────────────────────────────────>│
     │                     │                    │                   │
     │ 4. Configure(obj, data)                  │                   │
     │────────────────────>│                    │                   │
     │                     │                    │                   │
     │                     │ 5. Read data       │                   │
     │                     │───────────────────────────────────────>│
     │                     │                    │                   │
     │                     │ 6. Modify object   │                   │
     │                     │───────────────────>│                   │
     │                     │                    │                   │
     │ 7. Object configured│                    │                   │
     │<────────────────────│                    │                   │
     │                     │                    │                   │
     │ 8. Use configured object                 │                   │
     │─────────────────────────────────────────>│                   │
     │                     │                    │                   │
```

## Nested Configuration Pattern

### Example: Scene Data Provider

```
┌──────────────────────────────────────────────────────────────────┐
│           FlatbuffersSceneDataProvider                           │
│                                                                  │
│  ProvideSceneDataFromData(SceneDataFbs* fb_data)                │
│  {                                                               │
│      SceneData scene_data;                                       │
│                                                                  │
│      ┌────────────────────────────────────────────────────────┐ │
│      │ ConfigureSceneInfo(scene_data.scene_info,             │ │
│      │                    fb_data->scene_info())              │ │
│      │ {                                                      │ │
│      │     // Validate and configure SceneInfo struct        │ │
│      │     info.type = fb_info->scene_type();                │ │
│      │     info.id = ParseUUID(fb_info->scene_id());         │ │
│      │ }                                                      │ │
│      └────────────────────────────────────────────────────────┘ │
│                          ↓                                       │
│      ┌────────────────────────────────────────────────────────┐ │
│      │ ConfigureSceneResourcesConfig(                        │ │
│      │     scene_data.scene_resources_config,                │ │
│      │     fb_data->scene_resources_config())                │ │
│      │ {                                                      │ │
│      │     // Validate and configure resources               │ │
│      │     config.texture_width = fb_config->texture_width();│ │
│      │     config.texture_height = fb_config->texture_height();│ │
│      │ }                                                      │ │
│      └────────────────────────────────────────────────────────┘ │
│                          ↓                                       │
│      ┌────────────────────────────────────────────────────────┐ │
│      │ ConfigureAssetConfig(scene_data.scene_asset_config,  │ │
│      │                      fb_data->asset_config())         │ │
│      └────────────────────────────────────────────────────────┘ │
│                                                                  │
│      return scene_data;                                          │
│  }                                                               │
└──────────────────────────────────────────────────────────────────┘

Benefits:
  ✓ Each Configure method is reusable
  ✓ Each Configure method is testable independently
  ✓ Single Responsibility Principle
  ✓ Composable configuration
```

## Entity Configuration Layers

### Layered Configuration Pattern

```
┌─────────────────────────────────────────────────────────────────┐
│         FlatbuffersEntityConfigurator                           │
│                                                                 │
│  ConfigureEntityMemoryPool(EntityMemoryPool& emp)              │
│  {                                                              │
│      ┌──────────────────────────────────────────────────────┐  │
│      │ LAYER 1: First Layer Components                     │  │
│      │ (No dependencies on other components)               │  │
│      │                                                      │  │
│      │  ConfigureFirstLayerComponents(emp) {               │  │
│      │      for entity in entities:                        │  │
│      │          ConfigureComponent(base_component)         │  │
│      │          ConfigureCUserInterface(ui_component)      │  │
│      │          ConfigureCGrimoireMachina(grimoire)        │  │
│      │          ConfigureCMachinaForm(form)                │  │
│      │  }                                                   │  │
│      └──────────────────────────────────────────────────────┘  │
│                           ↓                                     │
│      ┌──────────────────────────────────────────────────────┐  │
│      │ LAYER 2: Second Layer Components                    │  │
│      │ (Depend on first layer components)                  │  │
│      │                                                      │  │
│      │  ConfigureSecondLayerComponents(emp) {              │  │
│      │      for entity in entities:                        │  │
│      │          ConfigureCUIState(ui_state_component, emp) │  │
│      │          // CUIState needs CUserInterface data      │  │
│      │  }                                                   │  │
│      └──────────────────────────────────────────────────────┘  │
│                                                                 │
│      return success;                                            │
│  }                                                              │
└─────────────────────────────────────────────────────────────────┘

Order matters:
  Layer 1 components must be configured before Layer 2
  Layer 2 components can safely access Layer 1 data
```

## Data Type Switching

### Runtime Switching Flow

```
┌────────────────────────────────────────────────────────────────────┐
│                        Initial State                               │
└────────────────────────────────────────────────────────────────────┘
                              │
                              │ DataAccessFactory factory(event_handler,
                              │                          DataType::Flatbuffers)
                              ↓
┌────────────────────────────────────────────────────────────────────┐
│                   Factory with FlatBuffers                         │
│                                                                    │
│  m_data_type = Flatbuffers                                         │
│  m_scene_data_provider = FlatbuffersSceneDataProvider              │
│  m_engine_data_provider = FlatbuffersEngineDataProvider            │
│  ...                                                               │
└────────────────────────────────────────────────────────────────────┘
                              │
                              │ Game uses providers
                              │ auto scene = provider->ProvideSceneData()
                              │ (Returns native SceneData)
                              ↓
┌────────────────────────────────────────────────────────────────────┐
│                   Game Logic Using SceneData                       │
│                   (No FlatBuffers coupling!)                       │
└────────────────────────────────────────────────────────────────────┘
                              │
                              │ factory.SetDataType(DataType::XML)
                              ↓
┌────────────────────────────────────────────────────────────────────┐
│                      Factory with XML                              │
│                                                                    │
│  m_data_type = XML                                                 │
│  m_scene_data_provider = XmlSceneDataProvider                      │
│  m_engine_data_provider = XmlEngineDataProvider                    │
│  ...                                                               │
└────────────────────────────────────────────────────────────────────┘
                              │
                              │ Game uses providers
                              │ auto scene = provider->ProvideSceneData()
                              │ (Still returns native SceneData!)
                              ↓
┌────────────────────────────────────────────────────────────────────┐
│                   Game Logic Using SceneData                       │
│                   (Game code unchanged!)                           │
└────────────────────────────────────────────────────────────────────┘
```

## Error Handling Flow

### std::expected Pattern

```
┌──────────────┐
│Provider Call │
│ProvideData() │
└──────┬───────┘
       │
       ↓
┌────────────────────────────────────┐
│ File Loading                       │
│ Parse Data                         │
│ Validate Data                      │
└──────┬─────────────────────────────┘
       │
       ├─────────────────┐
       │                 │
       ↓                 ↓
  ┌─────────┐      ┌─────────────┐
  │ Success │      │   Error     │
  └────┬────┘      └──────┬──────┘
       │                  │
       ↓                  ↓
┌──────────────┐    ┌──────────────────┐
│std::expected │    │std::expected     │
│<SceneData,   │    │<SceneData,       │
│ FailInfo>    │    │ FailInfo>        │
│              │    │                  │
│has_value()   │    │!has_value()      │
│= true        │    │= false           │
│              │    │                  │
│value()       │    │error()           │
│→ SceneData   │    │→ FailInfo        │
└──────┬───────┘    └──────┬───────────┘
       │                   │
       │                   │
       ↓                   ↓
┌──────────────┐    ┌──────────────────┐
│ Use Data     │    │ Handle Error     │
│              │    │ - Log message    │
│ scene.Load() │    │ - Show user      │
│              │    │ - Propagate up   │
└──────────────┘    └──────────────────┘
```

## Interface Hierarchy

### Complete Interface Structure

```
┌────────────────────────────────────────────────────────────────┐
│                    Data Provider Interfaces                    │
└────────────────────────────────────────────────────────────────┘
                              │
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
        ↓                     ↓                     ↓
┌───────────────┐    ┌────────────────┐    ┌──────────────┐
│ISaveData      │    │ISceneData      │    │IEngineData   │
│Provider       │    │Provider        │    │Provider      │
│               │    │                │    │              │
│+ ProvideSave  │    │+ ProvideScene  │    │+ ProvideEng. │
│  Data()       │    │  Data()        │    │  Data()      │
└───────┬───────┘    └────────┬───────┘    └──────┬───────┘
        │                     │                    │
        │                     │                    │
        ↓                     ↓                    ↓
┌───────────────┐    ┌────────────────┐    ┌──────────────┐
│Flatbuffers    │    │Flatbuffers     │    │Flatbuffers   │
│SaveData       │    │SceneData       │    │EngineData    │
│Provider       │    │Provider        │    │Provider      │
└───────────────┘    └────────────────┘    └──────────────┘
        ↓                     ↓                    ↓
┌───────────────┐    ┌────────────────┐    ┌──────────────┐
│XML SaveData   │    │XML SceneData   │    │XML EngineData│
│Provider       │    │Provider        │    │Provider      │
└───────────────┘    └────────────────┘    └──────────────┘

┌────────────────────────────────────────────────────────────────┐
│                  Configurator Interfaces                       │
└────────────────────────────────────────────────────────────────┘
                              │
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
        ↓                     ↓                     ↓
┌───────────────┐    ┌────────────────┐    ┌──────────────┐
│IScene         │    │IEntity         │    │IUIElement    │
│Configurator   │    │Configurator    │    │Configurator  │
│               │    │                │    │              │
│+ Configure    │    │+ Configure     │    │+ Create      │
│  Scene()      │    │  EntityPool()  │    │  Element()   │
└───────┬───────┘    └────────┬───────┘    └──────┬───────┘
        │                     │                    │
        │                     │                    │
        ↓                     ↓                    ↓
┌───────────────┐    ┌────────────────┐    ┌──────────────┐
│Flatbuffers    │    │Flatbuffers     │    │Flatbuffers   │
│Scene          │    │Entity          │    │UIElement     │
│Configurator   │    │Configurator    │    │Configurator  │
└───────────────┘    └────────────────┘    └──────────────┘
```

## Object Ownership

### Memory Management

```
┌──────────────────────────────────────────────────────────┐
│                  DataAccessFactory                       │
│                                                          │
│  std::unique_ptr<ISceneDataProvider>                     │
│  std::unique_ptr<IEngineDataProvider>                    │
│  std::unique_ptr<ISceneConfigurator>                     │
│                                                          │
│  Factory OWNS all provider/configurator instances        │
└──────────────────┬───────────────────────────────────────┘
                   │
                   │ Returns raw pointers
                   │ (factory maintains ownership)
                   ↓
┌──────────────────────────────────────────────────────────┐
│                    Client Code                           │
│                                                          │
│  ISceneDataProvider* provider = factory.Get...();        │
│  // Use provider, but don't delete it!                   │
│  // Factory will clean up when destroyed                 │
└──────────────────────────────────────────────────────────┘

Provider/Configurator returns:
  - Native objects by value (SceneData, SaveData)
  - Or unique_ptr for polymorphic objects (UIElement)

Client owns returned native objects:
  - SceneData scene_data = provider->ProvideData();
  - unique_ptr<UIElement> element = configurator->CreateElement();
```

## Summary

This visual reference demonstrates:

- **Layered Architecture**: Clear separation between game logic, native objects, providers/configurators, and data
- **Factory Pattern**: Centralized management of providers and configurators
- **Interface Abstraction**: Format-agnostic game code through interfaces
- **Nested Configuration**: Composable, reusable configuration methods
- **Error Handling**: Explicit error handling with `std::expected`
- **Runtime Flexibility**: Switch data types without changing game code

For complete implementation details, see:
- [Data Loading and Configuration Architecture](DATA_LOADING_AND_CONFIGURATION.md)
- [Quick Reference Guide](DATA_LOADING_QUICK_REFERENCE.md)
- [XML Implementation Example](EXAMPLE_XML_IMPLEMENTATION.md)
