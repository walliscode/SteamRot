# Data Loading Quick Reference

## Quick Lookup Guide

This document provides quick reference for common data loading and configuration tasks in SteamRot.

## Table of Contents

- [Common Patterns](#common-patterns)
- [Decision Flowcharts](#decision-flowcharts)
- [Interface Summary](#interface-summary)
- [Code Snippets](#code-snippets)

## Common Patterns

### Which Pattern Should I Use?

| Task | Pattern | Interface |
|------|---------|-----------|
| Load save file | Provider Pattern | `ISaveDataProvider` |
| Load scene data | Provider Pattern | `ISceneDataProvider` |
| Configure scene object | Configurator Pattern | `ISceneConfigurator` |
| Configure entities | Configurator Pattern | `IEntityConfigurator` |
| Create UI elements | Configurator Pattern (Create) | `IUIElementConfigurator` |
| Configure UI styles | Configurator Pattern | `IUIStyleConfigurator` |
| Import entity data | Importer Pattern | `IEntityImporter` |

### When to Use Configure() vs Create()

```
Do I have an existing object to populate?
├── YES → Use Configure(object, data)
│   └── Example: ConfigureScene(scene, scene_data)
│
└── NO → Use Create(data)
    └── Example: CreateRootUIElement()
```

### When to Use Provider vs Configurator

```
What do I need?
├── Load data from file/source
│   └── Use IDataProvider
│       └── Returns native object (SceneData, SaveData, etc.)
│
└── Configure existing runtime object
    └── Use IConfigurator
        └── Populates object from data
```

## Decision Flowcharts

### Adding New Data Support

```
Need to add new data support?
│
├── Is it a new DATA TYPE? (XML, JSON, etc.)
│   │
│   ├── 1. Add to DataType enum
│   ├── 2. Implement IDataProvider interface
│   ├── 3. Implement IConfigurator interface (if needed)
│   ├── 4. Add SetXXXDataProviders() to DataAccessFactory
│   ├── 5. Update SetDataProviders() switch statement
│   └── 6. Recompile
│
└── Is it a new NATIVE OBJECT? (NewData struct)
    │
    ├── 1. Define native object struct (src/types/core/)
    ├── 2. Create INewDataProvider interface
    ├── 3. Implement FlatbuffersNewDataProvider
    ├── 4. Add to DataAccessFactory
    ├── 5. Create .fbs schema (if FlatBuffers)
    └── 6. Recompile
```

### Loading Data at Runtime

```
Need to load data?
│
├── 1. Get DataAccessFactory instance
│   └── DataAccessFactory factory(event_handler);
│
├── 2. Get appropriate provider
│   ├── Scene data? → GetSceneDataProvider()
│   ├── Save data? → GetSaveDataProvider()
│   ├── Engine data? → GetEngineDataProvider()
│   └── Scene manager? → GetSceneManagerDataProvider()
│
├── 3. Call provider method
│   └── auto data = provider->ProvideData(/* criteria */);
│
├── 4. Check result
│   ├── Has value? → Use data
│   └── Error? → Handle FailInfo
│
└── 5. Pass native object to game code
    └── No format-specific types!
```

### Configuring Objects

```
Need to configure object?
│
├── 1. Have native data object
│   └── SceneData, SaveData, etc.
│
├── 2. Get configurator from factory
│   └── auto configurator = factory.GetXXXConfigurator();
│
├── 3. Call configure method
│   └── auto result = configurator->Configure(object, data);
│
├── 4. Check result
│   ├── Success? → Object is configured
│   └── Error? → Handle FailInfo
│
└── 5. Use configured object
    └── Ready for game logic
```

## Interface Summary

### Data Providers

| Interface | Method | Returns | Purpose |
|-----------|--------|---------|---------|
| `ISaveDataProvider` | `ProvideSaveData()` | `SaveData` | Load save file |
| `ISceneDataProvider` | `ProvideDefaultSceneData(SceneType)` | `SceneData` | Load scene for type |
| `ISceneDataProvider` | `ProvideSceneDataFromData(SceneDataFbs*)` | `SceneData` | Convert FlatBuffers to native |
| `IEngineDataProvider` | `ProvideEngineData()` | `EngineData` | Load engine config |
| `ISceneManagerDataProvider` | `ProvideSceneManagerData()` | `SceneManagerData` | Load scene manager config |
| `IUIStyleDataProvider` | `ProvideUIStyle(name)` | `UIStyle` | Load UI style |

### Configurators

| Interface | Method | Modifies | Purpose |
|-----------|--------|----------|---------|
| `ISceneConfigurator` | `ConfigureScene(Scene&, SceneData&)` | `Scene` | Configure scene from data |
| `ISceneConfigurator` | `ConfigureSceneInfo(Scene&, SceneData&)` | `Scene` | Configure scene info |
| `ISceneConfigurator` | `ImportEntities(Scene&, SceneData&)` | `Scene` | Import entities into scene |
| `IEntityConfigurator` | `ConfigureEntityMemoryPool(EMP&)` | `EntityMemoryPool` | Configure all entities |
| `IEntityConfigurator` | `ConfigureCUserInterface(CUI&)` | Component | Configure UI component |
| `IUIElementConfigurator` | `CreateRootUIElement()` | Returns new | Create UI element tree |
| `IUIStyleConfigurator` | `ConfigureStyle(name)` | Returns new | Create UI style |

### Importers/Exporters

| Interface | Method | Purpose |
|-----------|--------|---------|
| `IEntityImporter` | `ImportEntities(EntityMemoryPool&)` | Import entities from source |
| `IEntityExporter` | `ExportEntities(const EntityMemoryPool&)` | Export entities to destination |

## Code Snippets

### Loading Scene Data

```cpp
// Get provider from factory
auto provider_result = data_access_factory.GetSceneDataProvider();
if (!provider_result) {
  // Handle error
  return std::unexpected(provider_result.error());
}

// Get scene data for specific scene type
auto scene_data_result = provider_result.value()->ProvideDefaultSceneData(
    SceneType::SceneType_CRAFTING);
if (!scene_data_result) {
  // Handle error
  return std::unexpected(scene_data_result.error());
}

SceneData scene_data = scene_data_result.value();

// Use scene_data (format-agnostic!)
ProcessScene(scene_data);
```

### Configuring Scene Object

```cpp
// Have scene_data from provider
SceneData scene_data = /* from provider */;

// Create scene object
Scene scene(scene_resources, asset_manager);

// Get configurator
auto configurator_result = data_access_factory.GetSceneConfigurator();
if (!configurator_result) {
  return std::unexpected(configurator_result.error());
}

// Configure scene
auto config_result = configurator_result.value()->ConfigureScene(
    scene, scene_data);
if (!config_result) {
  return std::unexpected(config_result.error());
}

// Scene is now ready to use
scene.RunLogic();
```

### Creating UI Elements

```cpp
// Have UI data (FlatBuffers)
const UserInterfaceFbs& ui_data = /* from somewhere */;

// Create configurator (specific to FlatBuffers)
FlatbuffersUIElementConfigurator configurator(event_handler, ui_data);

// Create UI element tree
auto ui_result = configurator.CreateRootUIElement();
if (!ui_result) {
  return std::unexpected(ui_result.error());
}

// Use UI element
std::unique_ptr<UIElement> ui_element = std::move(ui_result.value());
RenderUI(*ui_element);
```

### Nested Configuration

```cpp
// Provider configures nested structures
std::expected<SceneData, FailInfo>
ProvideSceneData(const SceneDataFbs* fb_data) const {
  SceneData scene_data;
  
  // Configure nested SceneInfo
  auto info_result = ConfigureSceneInfo(
      scene_data.scene_info, 
      fb_data->scene_info());
  if (!info_result)
    return std::unexpected(info_result.error());
  
  // Configure nested SceneResourcesConfig
  auto resources_result = ConfigureSceneResourcesConfig(
      scene_data.scene_resources_config,
      fb_data->scene_resources_config());
  if (!resources_result)
    return std::unexpected(resources_result.error());
  
  return scene_data;
}
```

### Entity Import with Layers

```cpp
// Configurator handles component dependencies
std::expected<std::monostate, FailInfo>
ConfigureEntityMemoryPool(EntityMemoryPool& emp) override {
  // Layer 1: Independent components
  auto layer1_result = ConfigureFirstLayerComponents(emp);
  if (!layer1_result)
    return std::unexpected(layer1_result.error());
  
  // Layer 2: Dependent components
  auto layer2_result = ConfigureSecondLayerComponents(emp);
  if (!layer2_result)
    return std::unexpected(layer2_result.error());
  
  return std::monostate{};
}
```

### Error Handling Pattern

```cpp
// Standard pattern with std::expected
auto result = provider->ProvideData();

// Check for error
if (!result) {
  FailInfo error = result.error();
  
  // Log error
  LOG_ERROR("Failed to provide data: {} (mode: {})", 
            error.message, 
            static_cast<int>(error.mode));
  
  // Propagate error
  return std::unexpected(error);
}

// Extract value
T data = result.value();

// Use data
ProcessData(data);
```

### Switching Data Type at Runtime

```cpp
// Initialize with FlatBuffers
DataAccessFactory factory(event_handler, DataType::Flatbuffers);

// Use FlatBuffers
auto scene_data = factory.GetSceneDataProvider()
    ->ProvideDefaultSceneData(scene_type);

// Switch to XML (hypothetical)
auto switch_result = factory.SetDataType(DataType::XML);
if (!switch_result) {
  // Handle error
}

// Now using XML providers
auto scene_data_xml = factory.GetSceneDataProvider()
    ->ProvideDefaultSceneData(scene_type);
```

## Architecture Cheat Sheet

### Layer Diagram

```
┌─────────────────────────────────────┐
│      Game Logic Layer               │  Uses native objects
│  (Scene, Engine, Game code)         │  (SceneData, SaveData)
└─────────────┬───────────────────────┘
              │
              │ uses
              ↓
┌─────────────────────────────────────┐
│   Native Object Layer                │  Format-agnostic types
│  (SceneData, SaveData, EngineData)  │  (No FlatBuffers/XML/JSON)
└─────────────┬───────────────────────┘
              │
              │ configured by
              ↓
┌─────────────────────────────────────┐
│  Provider/Configurator Layer        │  Interface implementations
│  (IDataProvider, IConfigurator)     │  (FlatbuffersXXX, XmlXXX)
└─────────────┬───────────────────────┘
              │
              │ reads
              ↓
┌─────────────────────────────────────┐
│      Data Layer                     │  Format-specific
│  (FlatBuffers, XML, JSON files)    │  (SceneDataFbs, etc.)
└─────────────────────────────────────┘
```

### Data Flow

```
File/Source → Provider → Native Object → Game Logic
                   ↑
                   │
              Configurator
```

### Factory Responsibilities

```
DataAccessFactory
├── Manages provider instances
├── Manages configurator instances
├── Switches data types at runtime
├── Returns raw pointers (maintains ownership)
└── Single point of data access control
```

## Common Mistakes to Avoid

❌ **Don't pass format-specific types to game code**
```cpp
// Bad
void ProcessScene(const SceneDataFbs* fb_data);

// Good
void ProcessScene(const SceneData& scene_data);
```

❌ **Don't create providers directly in game code**
```cpp
// Bad
FlatbuffersSceneDataProvider provider(event_handler);

// Good
auto provider = factory.GetSceneDataProvider();
```

❌ **Don't forget to check std::expected results**
```cpp
// Bad
auto data = provider->ProvideData().value();  // May crash!

// Good
auto result = provider->ProvideData();
if (!result) {
  // Handle error
}
auto data = result.value();
```

❌ **Don't skip null checks for FlatBuffers data**
```cpp
// Bad
auto value = fb_data->field()->str();  // May segfault!

// Good
if (fb_data && fb_data->field()) {
  auto value = fb_data->field()->str();
}
```

❌ **Don't duplicate configuration logic**
```cpp
// Bad - inline everything
std::expected<SceneData, FailInfo> ProvideSceneData() {
  // 100 lines of configuration
}

// Good - use helper methods
std::expected<SceneData, FailInfo> ProvideSceneData() {
  auto info_result = ConfigureSceneInfo(/*...*/);
  auto resources_result = ConfigureSceneResources(/*...*/);
  // Composable, testable, reusable
}
```

## Checklist: Adding New Data Type

- [ ] Add enum value to `DataType`
- [ ] Create format-specific data structures (.fbs, .xsd, etc.)
- [ ] Implement `IDataProvider` interfaces
  - [ ] `ISaveDataProvider`
  - [ ] `ISceneDataProvider`
  - [ ] `IEngineDataProvider`
  - [ ] `ISceneManagerDataProvider`
- [ ] Implement `IConfigurator` interfaces (if needed)
  - [ ] `ISceneConfigurator`
  - [ ] `IEntityConfigurator`
- [ ] Update `DataAccessFactory`
  - [ ] Add `SetXXXDataProviders()` method
  - [ ] Update `SetDataProviders()` switch
  - [ ] Add provider member variables
  - [ ] Add getter methods
- [ ] Recompile
- [ ] Test with real data files

## Checklist: Adding New Native Object

- [ ] Define native struct in `src/types/core/`
- [ ] Create `INewDataProvider` interface in `src/types/interfaces/`
- [ ] Implement `FlatbuffersNewDataProvider` in `src/data_providers/`
- [ ] Create `.fbs` schema (if using FlatBuffers)
- [ ] Update `DataAccessFactory`
  - [ ] Add provider member variable
  - [ ] Add to `SetFlatbuffersDataProviders()`
  - [ ] Add getter method
- [ ] Write tests
- [ ] Recompile

## Quick Reference Tables

### Native Object → Provider Mapping

| Native Object | Provider Interface |
|---------------|-------------------|
| `SaveData` | `ISaveDataProvider` |
| `SceneData` | `ISceneDataProvider` |
| `EngineData` | `IEngineDataProvider` |
| `SceneManagerData` | `ISceneManagerDataProvider` |
| `UIStyle` | `IUIStyleDataProvider` |

### Object → Configurator Mapping

| Object to Configure | Configurator Interface |
|---------------------|----------------------|
| `Scene` | `ISceneConfigurator` |
| `EntityMemoryPool` | `IEntityConfigurator` |
| `UIElement` (create) | `IUIElementConfigurator` |
| `UIStyle` (create) | `IUIStyleConfigurator` |

### Current Implementations

| Interface | FlatBuffers Implementation | XML Implementation | JSON Implementation |
|-----------|---------------------------|-------------------|---------------------|
| `ISaveDataProvider` | `FlatbuffersSaveDataProvider` | ❌ Not yet | ❌ Not yet |
| `ISceneDataProvider` | `FlatbuffersSceneDataProvider` | ❌ Not yet | ❌ Not yet |
| `IEngineDataProvider` | `FlatbuffersEngineDataProvider` | ❌ Not yet | ❌ Not yet |
| `ISceneManagerDataProvider` | `FlatbuffersSceneManagerDataProvider` | ❌ Not yet | ❌ Not yet |
| `IUIStyleDataProvider` | `FlatbuffersUIStyleDataProvider` | ❌ Not yet | ❌ Not yet |
| `ISceneConfigurator` | `FlatbuffersSceneConfigurator` | ❌ Not yet | ❌ Not yet |
| `IEntityConfigurator` | `FlatbuffersEntityConfigurator` | ❌ Not yet | ❌ Not yet |
| `IUIElementConfigurator` | `FlatbuffersUIElementConfigurator` | ❌ Not yet | ❌ Not yet |

## Related Documentation

- [Full Architecture Guide](DATA_LOADING_AND_CONFIGURATION.md): Complete design philosophy and patterns
- [Error Handling](../workflows/ERROR_HANDLING.md): Using `std::expected` and `FailInfo`
- [Adding Components](../workflows/ADDING_COMPONENTS.md): Component workflow
- [FlatBuffers Schemas](../../src/types/flatbuffers/): FlatBuffer .fbs files
