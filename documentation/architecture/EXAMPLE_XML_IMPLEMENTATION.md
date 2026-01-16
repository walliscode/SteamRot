# Example: Adding a New Data Type (XML)

## Overview

This guide demonstrates how to add XML as a new data type to the SteamRot engine, following the patterns described in the [Data Loading and Configuration Architecture](DATA_LOADING_AND_CONFIGURATION.md).

**Scenario**: Add XML support alongside existing FlatBuffers implementation.

**Goal**: Game code remains format-agnostic; switching between FlatBuffers and XML requires only changing DataAccessFactory configuration.

## Prerequisites

- Understanding of the architecture patterns (see [main documentation](DATA_LOADING_AND_CONFIGURATION.md))
- XML parsing library (e.g., TinyXML2, pugixml, RapidXML)
- Familiarity with existing FlatBuffers implementation

## Step-by-Step Implementation

### Step 1: Update DataType Enum

**File**: `src/types/core/DataType.h`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Enum for different data types used in the engine
/////////////////////////////////////////////////

#pragma once

#include <cstdint>

namespace steamrot {
enum class DataType : uint8_t {
  Flatbuffers = 0,
  XML = 1,          // NEW: Add XML data type
};
}
```

**Why**: Enum defines available data types at compile time.

### Step 2: Define XML Data Structures

**File**: `src/types/xml/scene_data_xml.h` (new file structure)

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief XML-specific data structures for scene data
/////////////////////////////////////////////////

#pragma once

#include <string>
#include <vector>

namespace steamrot {
namespace xml {

/////////////////////////////////////////////////
/// @struct SceneInfoXml
/// @brief XML representation of scene information
/////////////////////////////////////////////////
struct SceneInfoXml {
  std::string scene_type;
  std::string scene_id;
};

/////////////////////////////////////////////////
/// @struct SceneResourcesConfigXml
/// @brief XML representation of scene resources configuration
/////////////////////////////////////////////////
struct SceneResourcesConfigXml {
  uint32_t texture_width{0};
  uint32_t texture_height{0};
};

/////////////////////////////////////////////////
/// @struct SceneDataXml
/// @brief XML representation of complete scene data
/////////////////////////////////////////////////
struct SceneDataXml {
  SceneInfoXml scene_info;
  SceneResourcesConfigXml scene_resources_config;
  std::string entity_file_path;  // Path to entity XML file
};

} // namespace xml
} // namespace steamrot
```

**Why**: XML-specific structures mirror native objects but represent XML format.

### Step 3: Implement XML Scene Data Provider

**File**: `src/data_providers/XmlSceneDataProvider.h`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of XmlSceneDataProvider class
/////////////////////////////////////////////////

#pragma once

#include "ISceneDataProvider.h"
#include "scene_data_xml.h"
#include <tinyxml2.h>

namespace steamrot {

class XmlSceneDataProvider : public ISceneDataProvider {
private:
  /////////////////////////////////////////////////
  /// @brief Reference to EventHandler for entity import
  /////////////////////////////////////////////////
  EventHandler& m_event_handler;

  /////////////////////////////////////////////////
  /// @brief Configure SceneInfo from XML
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(SceneInfo& info, const xml::SceneInfoXml& xml_info) const;

  /////////////////////////////////////////////////
  /// @brief Configure SceneResourcesConfig from XML
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneResourcesConfig(SceneResourcesConfig& config,
                               const xml::SceneResourcesConfigXml& xml_config) const;

  /////////////////////////////////////////////////
  /// @brief Parse XML file to SceneDataXml
  /////////////////////////////////////////////////
  std::expected<xml::SceneDataXml, FailInfo>
  ParseSceneXmlFile(const std::string& file_path) const;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor
  /////////////////////////////////////////////////
  XmlSceneDataProvider(EventHandler& event_handler);

  /////////////////////////////////////////////////
  /// @brief Provide SceneData from default XML file for scene type
  /////////////////////////////////////////////////
  std::expected<SceneData, FailInfo>
  ProvideDefaultSceneData(const SceneType scene_type) const override;

  /////////////////////////////////////////////////
  /// @brief Provide SceneData from XML data structure
  /////////////////////////////////////////////////
  std::expected<SceneData, FailInfo>
  ProvideSceneDataFromData(const xml::SceneDataXml* xml_data) const;
};

} // namespace steamrot
```

**File**: `src/data_providers/XmlSceneDataProvider.cpp`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Implementation of XmlSceneDataProvider
/////////////////////////////////////////////////

#include "XmlSceneDataProvider.h"
#include "PathProvider.h"
#include "XmlEntityImporter.h"
#include "scene_types_generated.h"  // For SceneType enum
#include <filesystem>

namespace steamrot {

/////////////////////////////////////////////////
XmlSceneDataProvider::XmlSceneDataProvider(EventHandler& event_handler)
    : m_event_handler(event_handler) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
XmlSceneDataProvider::ConfigureSceneInfo(
    SceneInfo& info, 
    const xml::SceneInfoXml& xml_info) const {
  
  // Parse scene type string to enum
  if (xml_info.scene_type == "TITLE") {
    info.type = SceneType::SceneType_TITLE;
  } else if (xml_info.scene_type == "CRAFTING") {
    info.type = SceneType::SceneType_CRAFTING;
  } else {
    return std::unexpected(FailInfo{FailMode::InvalidValue,
                                    "Unknown scene type: " + xml_info.scene_type});
  }

  // Parse UUID if provided
  if (!xml_info.scene_id.empty()) {
    auto uuid_result = uuids::uuid::from_string(xml_info.scene_id);
    if (!uuid_result) {
      return std::unexpected(FailInfo{FailMode::InvalidUUID,
                                      "Invalid UUID in XML: " + xml_info.scene_id});
    }
    info.id = uuid_result.value();
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
XmlSceneDataProvider::ConfigureSceneResourcesConfig(
    SceneResourcesConfig& config,
    const xml::SceneResourcesConfigXml& xml_config) const {
  
  // Validate dimensions
  if (xml_config.texture_width == 0) {
    return std::unexpected(FailInfo{FailMode::InvalidValue,
                                    "Texture width must be > 0"});
  }
  if (xml_config.texture_height == 0) {
    return std::unexpected(FailInfo{FailMode::InvalidValue,
                                    "Texture height must be > 0"});
  }

  config.texture_width = xml_config.texture_width;
  config.texture_height = xml_config.texture_height;

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<xml::SceneDataXml, FailInfo>
XmlSceneDataProvider::ParseSceneXmlFile(const std::string& file_path) const {
  
  // Load XML document
  tinyxml2::XMLDocument doc;
  if (doc.LoadFile(file_path.c_str()) != tinyxml2::XML_SUCCESS) {
    return std::unexpected(FailInfo{FailMode::FileNotFound,
                                    "Failed to load XML file: " + file_path});
  }

  xml::SceneDataXml scene_data_xml;

  // Parse SceneInfo
  auto* scene_info_elem = doc.FirstChildElement("SceneData")->FirstChildElement("SceneInfo");
  if (!scene_info_elem) {
    return std::unexpected(FailInfo{FailMode::XmlParseError,
                                    "SceneInfo element not found"});
  }
  
  scene_data_xml.scene_info.scene_type = scene_info_elem->FirstChildElement("Type")->GetText();
  
  auto* id_elem = scene_info_elem->FirstChildElement("ID");
  if (id_elem) {
    scene_data_xml.scene_info.scene_id = id_elem->GetText();
  }

  // Parse SceneResourcesConfig
  auto* resources_elem = doc.FirstChildElement("SceneData")->FirstChildElement("Resources");
  if (!resources_elem) {
    return std::unexpected(FailInfo{FailMode::XmlParseError,
                                    "Resources element not found"});
  }

  resources_elem->FirstChildElement("TextureWidth")->QueryUnsignedText(
      &scene_data_xml.scene_resources_config.texture_width);
  resources_elem->FirstChildElement("TextureHeight")->QueryUnsignedText(
      &scene_data_xml.scene_resources_config.texture_height);

  // Parse entity file path
  auto* entities_elem = doc.FirstChildElement("SceneData")->FirstChildElement("Entities");
  if (entities_elem) {
    scene_data_xml.entity_file_path = entities_elem->FirstChildElement("FilePath")->GetText();
  }

  return scene_data_xml;
}

/////////////////////////////////////////////////
std::expected<SceneData, FailInfo>
XmlSceneDataProvider::ProvideDefaultSceneData(const SceneType scene_type) const {
  
  // Build file path based on scene type
  PathProvider path_provider;
  std::string scene_name;
  
  switch (scene_type) {
  case SceneType::SceneType_TITLE:
    scene_name = "title_scene";
    break;
  case SceneType::SceneType_CRAFTING:
    scene_name = "crafting_scene";
    break;
  default:
    return std::unexpected(FailInfo{FailMode::EnumValueNotHandled,
                                    "Unsupported scene type"});
  }

  std::string file_path = path_provider.GetSceneDataPath(scene_name + ".xml");

  // Parse XML file
  auto xml_data_result = ParseSceneXmlFile(file_path);
  if (!xml_data_result) {
    return std::unexpected(xml_data_result.error());
  }

  // Convert XML data to native SceneData
  return ProvideSceneDataFromData(&xml_data_result.value());
}

/////////////////////////////////////////////////
std::expected<SceneData, FailInfo>
XmlSceneDataProvider::ProvideSceneDataFromData(
    const xml::SceneDataXml* xml_data) const {
  
  if (!xml_data) {
    return std::unexpected(FailInfo{FailMode::NullPointer,
                                    "xml_data is null"});
  }

  SceneData scene_data;

  // Configure SceneInfo (nested configuration)
  auto info_result = ConfigureSceneInfo(
      scene_data.scene_info, 
      xml_data->scene_info);
  if (!info_result) {
    return std::unexpected(info_result.error());
  }

  // Configure SceneResourcesConfig (nested configuration)
  auto resources_result = ConfigureSceneResourcesConfig(
      scene_data.scene_resources_config,
      xml_data->scene_resources_config);
  if (!resources_result) {
    return std::unexpected(resources_result.error());
  }

  // Create entity importer
  if (!xml_data->entity_file_path.empty()) {
    auto entity_importer = std::make_unique<XmlEntityImporter>(
        m_event_handler,
        xml_data->entity_file_path);
    
    scene_data.entity_transport = std::move(entity_importer);
  }

  // Note: AssetConfig would be configured here as well
  // (omitted for brevity)

  return scene_data;
}

} // namespace steamrot
```

**Why**: Provider loads XML files and converts to native `SceneData` objects. Game code never sees XML-specific types.

### Step 4: Implement XML Entity Importer

**File**: `src/entity/XmlEntityImporter.h`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of XmlEntityImporter
/////////////////////////////////////////////////

#pragma once

#include "IEntityImporter.h"
#include "EventHandler.h"
#include <string>

namespace steamrot {

class XmlEntityImporter : public IEntityImporter {
private:
  EventHandler& m_event_handler;
  std::string m_entity_file_path;

public:
  XmlEntityImporter(EventHandler& event_handler,
                   const std::string& entity_file_path);

  std::expected<std::monostate, FailInfo>
  ImportEntities(EntityMemoryPool& emp) override;
};

} // namespace steamrot
```

**File**: `src/entity/XmlEntityImporter.cpp`

```cpp
#include "XmlEntityImporter.h"
#include "XmlEntityConfigurator.h"

namespace steamrot {

/////////////////////////////////////////////////
XmlEntityImporter::XmlEntityImporter(EventHandler& event_handler,
                                    const std::string& entity_file_path)
    : m_event_handler(event_handler)
    , m_entity_file_path(entity_file_path) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
XmlEntityImporter::ImportEntities(EntityMemoryPool& emp) {
  
  // Parse XML file to get entity collection
  // (Implementation details omitted for brevity)
  
  // Create configurator with parsed data
  XmlEntityConfigurator configurator(m_event_handler, /* xml entity data */);
  
  // Use configurator to populate EntityMemoryPool
  return configurator.ConfigureEntityMemoryPool(emp);
}

} // namespace steamrot
```

**Why**: Importer wraps XML entity data and provides uniform interface, hiding XML details from scene code.

### Step 5: Update DataAccessFactory

**File**: `src/data_providers/DataAccessFactory.h`

```cpp
// ... existing includes ...
#include "XmlSceneDataProvider.h"          // NEW
#include "XmlEngineDataProvider.h"         // NEW
#include "XmlSceneManagerDataProvider.h"   // NEW

namespace steamrot {
class DataAccessFactory {
private:
  // ... existing members ...

  /////////////////////////////////////////////////
  /// @brief Set all data providers to XML implementations
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> SetXmlDataProviders();  // NEW

  // ... existing methods ...
};
} // namespace steamrot
```

**File**: `src/data_providers/DataAccessFactory.cpp`

```cpp
#include "DataAccessFactory.h"
// ... existing includes ...
#include "XmlSceneDataProvider.h"
#include "XmlEngineDataProvider.h"
#include "XmlSceneManagerDataProvider.h"

namespace steamrot {

// ... existing code ...

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
DataAccessFactory::SetXmlDataProviders() {
  
  // Set IEngineDataProvider
  m_engine_data_provider = std::make_unique<XmlEngineDataProvider>();
  if (!m_engine_data_provider) {
    return std::unexpected(FailInfo{FailMode::NullPointer,
                                    "Failed to create XmlEngineDataProvider"});
  }

  // Set ISceneManagerDataProvider
  m_scene_manager_data_provider = std::make_unique<XmlSceneManagerDataProvider>();
  if (!m_scene_manager_data_provider) {
    return std::unexpected(FailInfo{FailMode::NullPointer,
                                    "Failed to create XmlSceneManagerDataProvider"});
  }

  // Set ISceneDataProvider
  m_scene_data_provider = std::make_unique<XmlSceneDataProvider>(m_event_handler);
  if (!m_scene_data_provider) {
    return std::unexpected(FailInfo{FailMode::NullPointer,
                                    "Failed to create XmlSceneDataProvider"});
  }

  // Note: Scene configurator would be set here as well if XML-specific

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
DataAccessFactory::SetDataProviders() {
  
  switch (m_data_type) {
  case DataType::Flatbuffers:
    return SetFlatbuffersDataProviders();
  
  case DataType::XML:                       // NEW
    return SetXmlDataProviders();           // NEW
  
  default:
    return std::unexpected(FailInfo{FailMode::EnumValueNotHandled,
                                    "Unsupported data type"});
  }
}

} // namespace steamrot
```

**Why**: Factory now manages XML providers alongside FlatBuffers providers.

### Step 6: Example XML Data File

**File**: `data/scene/crafting_scene.xml`

```xml
<?xml version="1.0" encoding="UTF-8"?>
<SceneData>
  <SceneInfo>
    <Type>CRAFTING</Type>
    <ID>550e8400-e29b-41d4-a716-446655440000</ID>
  </SceneInfo>
  
  <Resources>
    <TextureWidth>1920</TextureWidth>
    <TextureHeight>1080</TextureHeight>
  </Resources>
  
  <Entities>
    <FilePath>data/entities/crafting_entities.xml</FilePath>
  </Entities>
  
  <Assets>
    <Font name="main_font" path="assets/fonts/arial.ttf" size="16"/>
    <Texture name="background" path="assets/images/crafting_bg.png"/>
  </Assets>
</SceneData>
```

**Why**: XML format is human-readable and editable, good for prototyping or modding support.

### Step 7: Using the New XML Data Type

**Game code (format-agnostic)**:

```cpp
// Initialize engine with XML data type
DataAccessFactory factory(event_handler, DataType::XML);

// Get scene data provider (returns ISceneDataProvider*)
auto provider_result = factory.GetSceneDataProvider();
if (!provider_result) {
  // Handle error
  return std::unexpected(provider_result.error());
}

// Load scene data (returns native SceneData, NOT XML-specific type!)
auto scene_data_result = provider_result.value()->ProvideDefaultSceneData(
    SceneType::SceneType_CRAFTING);
if (!scene_data_result) {
  // Handle error
  return std::unexpected(scene_data_result.error());
}

SceneData scene_data = scene_data_result.value();

// Use scene data - completely format-agnostic
ProcessScene(scene_data);

// Switch to FlatBuffers at runtime (if needed)
auto switch_result = factory.SetDataType(DataType::Flatbuffers);
// Now using FlatBuffers, but game code unchanged!
```

**Why**: Game code works identically with XML or FlatBuffers - zero coupling to data format.

## Testing Strategy

### Unit Tests for XML Provider

**File**: `tests/unit/data_providers/XmlSceneDataProvider.test.cpp`

```cpp
#include "XmlSceneDataProvider.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("XmlSceneDataProvider loads scene data", "[unit][XmlSceneDataProvider]") {
  steamrot::EventHandler event_handler;
  steamrot::XmlSceneDataProvider provider(event_handler);
  
  auto result = provider.ProvideDefaultSceneData(
      steamrot::SceneType::SceneType_CRAFTING);
  
  REQUIRE(result.has_value());
  
  const auto& scene_data = result.value();
  REQUIRE(scene_data.scene_info.type == steamrot::SceneType::SceneType_CRAFTING);
  REQUIRE(scene_data.scene_resources_config.texture_width > 0);
}

TEST_CASE("XmlSceneDataProvider handles invalid XML", "[unit][XmlSceneDataProvider]") {
  steamrot::EventHandler event_handler;
  steamrot::XmlSceneDataProvider provider(event_handler);
  
  steamrot::xml::SceneDataXml invalid_xml;
  invalid_xml.scene_info.scene_type = "INVALID_TYPE";
  
  auto result = provider.ProvideSceneDataFromData(&invalid_xml);
  
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::InvalidValue);
}
```

### Integration Test

**File**: `tests/integration/data_type_switching.test.cpp`

```cpp
#include "DataAccessFactory.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Switch between FlatBuffers and XML", "[integration]") {
  steamrot::EventHandler event_handler;
  steamrot::DataAccessFactory factory(event_handler, steamrot::DataType::Flatbuffers);
  
  // Load with FlatBuffers
  auto fb_provider = factory.GetSceneDataProvider();
  REQUIRE(fb_provider.has_value());
  
  auto fb_scene = fb_provider.value()->ProvideDefaultSceneData(
      steamrot::SceneType::SceneType_CRAFTING);
  REQUIRE(fb_scene.has_value());
  
  // Switch to XML
  auto switch_result = factory.SetDataType(steamrot::DataType::XML);
  REQUIRE(switch_result.has_value());
  
  // Load with XML
  auto xml_provider = factory.GetSceneDataProvider();
  REQUIRE(xml_provider.has_value());
  
  auto xml_scene = xml_provider.value()->ProvideDefaultSceneData(
      steamrot::SceneType::SceneType_CRAFTING);
  REQUIRE(xml_scene.has_value());
  
  // Both scenes should have same content (different source formats)
  REQUIRE(fb_scene.value().scene_info.type == xml_scene.value().scene_info.type);
}
```

## Benefits Demonstrated

1. **Format Agnostic Game Code**: Game logic uses `SceneData`, not `SceneDataFbs` or `SceneDataXml`
2. **Runtime Switching**: Change data type via factory without rewriting game code
3. **Code Reuse**: Nested configuration methods (`ConfigureSceneInfo`, etc.) are reusable
4. **Testability**: Easy to test providers independently
5. **Extensibility**: Adding JSON, YAML, or other formats follows same pattern
6. **Type Safety**: Compile-time type checking for all operations

## Common Pitfalls and Solutions

### Pitfall 1: Exposing XML Types in Interfaces

**Bad**:
```cpp
// ISceneDataProvider interface (WRONG!)
virtual std::expected<SceneData, FailInfo>
ProvideSceneDataFromData(const xml::SceneDataXml* xml_data) const = 0;
```

**Good**:
```cpp
// ISceneDataProvider interface (CORRECT!)
virtual std::expected<SceneData, FailInfo>
ProvideDefaultSceneData(const SceneType scene_type) const = 0;

// XmlSceneDataProvider can have XML-specific methods as overloads
std::expected<SceneData, FailInfo>
ProvideSceneDataFromData(const xml::SceneDataXml* xml_data) const;
```

### Pitfall 2: Not Validating XML Data

**Bad**:
```cpp
// No null checks or validation
scene_data.scene_info.type = ParseSceneType(xml_info.scene_type);
```

**Good**:
```cpp
if (xml_info.scene_type.empty()) {
  return std::unexpected(FailInfo{FailMode::InvalidValue,
                                  "Scene type is empty"});
}

auto type_result = ParseSceneType(xml_info.scene_type);
if (!type_result) {
  return std::unexpected(type_result.error());
}
scene_data.scene_info.type = type_result.value();
```

### Pitfall 3: Duplicating Configuration Logic

**Bad**:
```cpp
// XmlSceneDataProvider duplicates all configuration
// FlatbuffersSceneDataProvider has identical logic but for FlatBuffers
```

**Solution**: Extract common configuration to helper functions or base class:

```cpp
// scene_data_helpers.h
namespace steamrot::helpers {

std::expected<std::monostate, FailInfo>
ValidateSceneResourcesConfig(const SceneResourcesConfig& config);

} // namespace steamrot::helpers

// Use in both XML and FlatBuffers providers
auto validation = helpers::ValidateSceneResourcesConfig(config);
if (!validation) return std::unexpected(validation.error());
```

## Next Steps

After implementing XML support:

1. **Add More Providers**: Implement `XmlEngineDataProvider`, `XmlSaveDataProvider`
2. **Add Configurators**: If needed, implement `XmlSceneConfigurator`, `XmlEntityConfigurator`
3. **Add Tests**: Comprehensive unit and integration tests
4. **Update Documentation**: Document XML schema requirements
5. **Performance Testing**: Compare FlatBuffers vs XML load times
6. **Consider JSON**: Same pattern applies for adding JSON support

## Summary

This example demonstrates:
- ✅ Adding a new data type (XML) without changing game code
- ✅ Using IDataProvider pattern for format abstraction
- ✅ Nested configuration for code reuse
- ✅ Factory pattern for runtime switching
- ✅ Test-driven approach
- ✅ Error handling with `std::expected`
- ✅ Separation of concerns (data format vs game logic)

The architecture achieves all design goals:
- **Compile-time types**: All types known at compile time
- **Template support**: Can use templates for generic code
- **No coupling**: Game code uses native objects only
- **Runtime flexibility**: Switch formats via factory
- **Code reusability**: Composable configuration methods
- **Easy integration**: Clear patterns and interfaces

## Related Documentation

- [Data Loading and Configuration Architecture](DATA_LOADING_AND_CONFIGURATION.md)
- [Quick Reference Guide](DATA_LOADING_QUICK_REFERENCE.md)
- [Error Handling](ERROR_HANDLING.md)
