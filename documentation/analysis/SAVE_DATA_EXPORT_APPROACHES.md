# SaveData Export Approaches and Interface Design

## Executive Summary

This document analyzes various approaches for exporting `SaveData` from the codebase and designing interfaces to transform save data into other formats. The analysis provides recommendations for responsibility division to maintain a clean, maintainable codebase following established architectural patterns in SteamRot.

**Key Findings:**
- **4 distinct export approaches** identified, each with specific use cases
- Export functionality should follow the **Provider/Configurator pattern** already established in the codebase
- Clear separation between **data types**, **export interfaces**, and **format implementations** maintains architectural integrity
- Recommended approach: **Abstract Exporter Interface** with format-specific implementations

**Recommended Actions:**
1. Create `ISaveDataExporter` interface (follows existing Provider pattern)
2. Implement format-specific exporters (FlatBuffers, JSON, XML, etc.)
3. Maintain separation between SaveData (types layer) and exporters (services layer)
4. Use dependency injection for exporter selection

**Estimated Effort:** 2-3 days for complete implementation

---

## Table of Contents

1. [Current State Analysis](#current-state-analysis)
2. [Export Approaches Overview](#export-approaches-overview)
3. [Approach 1: Direct Serialization Methods](#approach-1-direct-serialization-methods)
4. [Approach 2: Abstract Exporter Interface](#approach-2-abstract-exporter-interface)
5. [Approach 3: Visitor Pattern](#approach-3-visitor-pattern)
6. [Approach 4: Strategy Pattern with Format Registry](#approach-4-strategy-pattern-with-format-registry)
7. [Comparative Analysis](#comparative-analysis)
8. [Responsibility Division](#responsibility-division)
9. [Recommended Architecture](#recommended-architecture)
10. [Implementation Roadmap](#implementation-roadmap)
11. [Example Use Cases](#example-use-cases)
12. [Testing Strategy](#testing-strategy)

---

## Current State Analysis

### Existing Architecture

SteamRot follows a layered architecture with clear separation of concerns:

**Layer 1 - Types/Data:**
- `src/types/core/SaveData.h` - Pure data struct
- `src/types/core/SaveMetaData.h` - Metadata struct
- `src/types/core/SceneData.h` - Scene data struct
- Zero dependencies on other internal packages

**Layer 2 - Interfaces:**
- `src/interfaces/ISaveDataProvider.h` - Abstract interface for loading SaveData
- Provider interfaces define contracts without implementation details

**Layer 3 - Services/Implementation:**
- `src/data_providers/FlatbuffersSaveDataProvider.h` - Concrete provider implementation
- Handles conversion from FlatBuffers format to SaveData types

### Current SaveData Structure

```cpp
namespace steamrot {

struct SaveData {
  SaveMetaData meta_data;              // Timestamp, version, save name, file_id
  SceneManagerData scene_manager_data; // SceneManager state
  SceneCollectionData scene_collection_data; // All scene data
};

struct SaveMetaData {
  uuids::uuid file_id;      // Unique identifier
  std::string save_name;    // Display name
};

}
```

### Current Data Flow (Import Only)

```
┌──────────────────────────────────────────────────────┐
│  FlatBuffers Binary File (.bin)                      │
│  - Serialized SaveDataFbs                            │
└──────────────────────┬───────────────────────────────┘
                       │
                       │ Read
                       ▼
┌──────────────────────────────────────────────────────┐
│  FlatbuffersSaveDataProvider                         │
│  - implements ISaveDataProvider                      │
│  - ConfigureSaveMetaData()                           │
│  - ProvideSaveData()                                 │
└──────────────────────┬───────────────────────────────┘
                       │
                       │ Returns
                       ▼
┌──────────────────────────────────────────────────────┐
│  SaveData (C++ native types)                         │
│  - Used by game engine at runtime                    │
└──────────────────────────────────────────────────────┘
```

**Missing:** Export path (SaveData → Various Formats)

---

## Export Approaches Overview

| Approach | Complexity | Flexibility | Testability | Coupling | Best For |
|----------|-----------|-------------|-------------|----------|----------|
| **1. Direct Serialization** | Low | Low | Medium | High | Simple, single-format exports |
| **2. Abstract Exporter** | Medium | High | High | Low | Multiple formats, extensible |
| **3. Visitor Pattern** | High | Very High | High | Low | Complex data transformations |
| **4. Strategy Registry** | Medium-High | Very High | High | Low | Plugin systems, runtime format selection |

---

## Approach 1: Direct Serialization Methods

### Description

Add serialization methods directly to `SaveData` struct or create free functions that operate on `SaveData`.

### Implementation

**Option 1A: Methods on SaveData**

```cpp
// src/types/core/SaveData.h
namespace steamrot {

struct SaveData {
  SaveMetaData meta_data;
  SceneManagerData scene_manager_data;
  SceneCollectionData scene_collection_data;
  
  // Export methods
  std::expected<std::string, FailInfo> ToJSON() const;
  std::expected<std::vector<uint8_t>, FailInfo> ToFlatBuffers() const;
  std::expected<std::string, FailInfo> ToXML() const;
};

}
```

**Option 1B: Free Functions**

```cpp
// src/data_providers/save_data_serializers.h
namespace steamrot {

std::expected<std::string, FailInfo> 
SaveDataToJSON(const SaveData& save_data);

std::expected<std::vector<uint8_t>, FailInfo> 
SaveDataToFlatBuffers(const SaveData& save_data);

std::expected<std::string, FailInfo> 
SaveDataToXML(const SaveData& save_data);

}
```

### Data Flow

```
┌──────────────────────────────────────────┐
│  SaveData (Runtime)                      │
│  - meta_data                             │
│  - scene_manager_data                    │
│  - scene_collection_data                 │
└──────────┬───────────────────────────────┘
           │
           │ save_data.ToJSON()
           │ or SaveDataToJSON(save_data)
           │
           ▼
┌──────────────────────────────────────────┐
│  JSON String / FlatBuffers / XML         │
│  - Serialized format ready for export    │
└──────────────────────────────────────────┘
```

### Pros

✅ **Simple to understand** - Straightforward function calls
✅ **Easy to implement initially** - No complex architecture needed
✅ **Direct access to data** - No abstraction overhead
✅ **Low initial development time** - Quick to get working

### Cons

❌ **Violates Single Responsibility Principle** - SaveData knows about serialization formats
❌ **High coupling** - SaveData depends on multiple serialization libraries (JSON, FlatBuffers, XML)
❌ **Poor extensibility** - Adding new format requires modifying SaveData or adding more free functions
❌ **Violates Open/Closed Principle** - Must modify existing code for new formats
❌ **Difficult testing** - Hard to mock serialization behavior
❌ **Layer violation** - Types layer (SaveData) depends on services (serialization)
❌ **Bloated types** - SaveData becomes responsible for too many concerns

### When to Use

- **Prototyping** - Quick proof-of-concept
- **Single format only** - If you'll only ever export to one format
- **Throwaway code** - Temporary exports that won't be maintained
- **Simple utilities** - Command-line tools with no extensibility requirements

### Responsibility Division

| Component | Responsibility |
|-----------|---------------|
| `SaveData` | ❌ Data structure AND serialization (violates SRP) |
| Serialization libs | ❌ Tightly coupled to SaveData (hard to test) |

---

## Approach 2: Abstract Exporter Interface

### Description

Create an abstract `ISaveDataExporter` interface that defines the export contract. Implement concrete exporters for each format. This follows the established Provider pattern in the codebase.

### Implementation

**Interface Definition:**

```cpp
// src/interfaces/ISaveDataExporter.h
namespace steamrot {

class ISaveDataExporter {
public:
  virtual ~ISaveDataExporter() = default;
  
  /////////////////////////////////////////////////
  /// @brief Export SaveData to the target format
  ///
  /// @param save_data The SaveData to export
  /// @return Serialized data as byte vector, or error
  /////////////////////////////////////////////////
  virtual std::expected<std::vector<uint8_t>, FailInfo>
  ExportSaveData(const SaveData& save_data) const = 0;
  
  /////////////////////////////////////////////////
  /// @brief Get the file extension for this format
  ///
  /// @return File extension (e.g., ".json", ".bin", ".xml")
  /////////////////////////////////////////////////
  virtual std::string GetFileExtension() const = 0;
  
  /////////////////////////////////////////////////
  /// @brief Get the MIME type for this format
  ///
  /// @return MIME type (e.g., "application/json")
  /////////////////////////////////////////////////
  virtual std::string GetMimeType() const = 0;
};

}
```

**Concrete Implementations:**

```cpp
// src/data_providers/FlatbuffersSaveDataExporter.h
namespace steamrot {

class FlatbuffersSaveDataExporter : public ISaveDataExporter {
public:
  FlatbuffersSaveDataExporter() = default;
  
  std::expected<std::vector<uint8_t>, FailInfo>
  ExportSaveData(const SaveData& save_data) const override;
  
  std::string GetFileExtension() const override { return ".bin"; }
  std::string GetMimeType() const override { return "application/octet-stream"; }

private:
  /////////////////////////////////////////////////
  /// @brief Convert SaveMetaData to FlatBuffers table
  /////////////////////////////////////////////////
  std::expected<flatbuffers::Offset<SaveMetaDataFbs>, FailInfo>
  SerializeSaveMetaData(flatbuffers::FlatBufferBuilder& builder,
                        const SaveMetaData& meta_data) const;
};

}
```

```cpp
// src/data_providers/JSONSaveDataExporter.h
namespace steamrot {

class JSONSaveDataExporter : public ISaveDataExporter {
public:
  JSONSaveDataExporter() = default;
  
  std::expected<std::vector<uint8_t>, FailInfo>
  ExportSaveData(const SaveData& save_data) const override;
  
  std::string GetFileExtension() const override { return ".json"; }
  std::string GetMimeType() const override { return "application/json"; }

private:
  /////////////////////////////////////////////////
  /// @brief Convert SaveMetaData to JSON object
  /////////////////////////////////////////////////
  std::expected<nlohmann::json, FailInfo>
  SerializeSaveMetaData(const SaveMetaData& meta_data) const;
};

}
```

```cpp
// src/data_providers/XMLSaveDataExporter.h
namespace steamrot {

class XMLSaveDataExporter : public ISaveDataExporter {
public:
  XMLSaveDataExporter() = default;
  
  std::expected<std::vector<uint8_t>, FailInfo>
  ExportSaveData(const SaveData& save_data) const override;
  
  std::string GetFileExtension() const override { return ".xml"; }
  std::string GetMimeType() const override { return "application/xml"; }
};

}
```

### Data Flow

```
┌────────────────────────────────────────────────┐
│  SaveData (Runtime - Layer 1)                  │
│  - Pure data struct                            │
│  - No dependencies                             │
└────────────┬───────────────────────────────────┘
             │
             │ Passed to exporter
             │
             ▼
┌────────────────────────────────────────────────┐
│  ISaveDataExporter (Layer 2 - Interface)       │
│  - ExportSaveData()                            │
│  - GetFileExtension()                          │
│  - GetMimeType()                               │
└────────────┬───────────────────────────────────┘
             │
             │ Implemented by
             │
    ┌────────┴────────┬────────────┬─────────┐
    │                 │            │         │
    ▼                 ▼            ▼         ▼
┌─────────┐  ┌──────────┐  ┌──────────┐  ┌─────┐
│Flatbuffs│  │  JSON    │  │   XML    │  │ ... │
│Exporter │  │ Exporter │  │ Exporter │  │     │
│         │  │          │  │          │  │     │
│(Layer 3)│  │ (Layer 3)│  │ (Layer 3)│  │     │
└────┬────┘  └────┬─────┘  └────┬─────┘  └─────┘
     │            │             │
     ▼            ▼             ▼
┌─────────────────────────────────────────────────┐
│  Serialized Output                              │
│  - .bin (FlatBuffers binary)                    │
│  - .json (JSON text)                            │
│  - .xml (XML text)                              │
└─────────────────────────────────────────────────┘
```

### Usage Example

```cpp
// Example: Export to JSON
void ExportSaveToJSON(const SaveData& save_data, 
                      const std::string& file_path) {
  // Create JSON exporter
  JSONSaveDataExporter exporter;
  
  // Export data
  auto export_result = exporter.ExportSaveData(save_data);
  if (!export_result.has_value()) {
    // Handle error
    return;
  }
  
  // Write to file
  std::string full_path = file_path + exporter.GetFileExtension();
  std::ofstream file(full_path, std::ios::binary);
  file.write(reinterpret_cast<const char*>(export_result.value().data()),
             export_result.value().size());
}

// Example: Export with dependency injection
class SaveManager {
  const ISaveDataExporter& m_exporter;
  
public:
  SaveManager(const ISaveDataExporter& exporter)
    : m_exporter(exporter) {}
    
  void Save(const SaveData& data, const std::string& path) {
    auto result = m_exporter.ExportSaveData(data);
    // Write to file...
  }
};

// Usage:
JSONSaveDataExporter json_exporter;
SaveManager save_manager(json_exporter);
save_manager.Save(save_data, "my_save");
```

### Pros

✅ **Follows established patterns** - Matches ISaveDataProvider, IFontProvider architecture
✅ **Low coupling** - SaveData independent of serialization logic
✅ **Extensible** - Add new formats without modifying existing code (Open/Closed Principle)
✅ **High testability** - Easy to create mock exporters for testing
✅ **Single Responsibility** - Each class has one clear purpose
✅ **Dependency injection friendly** - Easy to swap exporters
✅ **Symmetric with Provider** - Natural pairing: Provider (import) ↔ Exporter (export)
✅ **Clear layer separation** - Types → Interfaces → Implementations

### Cons

⚠️ **More files to manage** - One interface + multiple implementations
⚠️ **Slightly more boilerplate** - Need interface and concrete classes
⚠️ **Indirect** - Cannot call save_data.Export() directly (must use exporter)

### When to Use

- ✅ **Multiple export formats** needed
- ✅ **Future extensibility** is important
- ✅ **Testing** is a priority
- ✅ **Following established patterns** (matches Provider pattern)
- ✅ **Professional/production** code
- ✅ **Long-term maintenance** expected

### Responsibility Division

| Component | Responsibility |
|-----------|---------------|
| `SaveData` | ✅ Pure data structure (Layer 1) |
| `ISaveDataExporter` | ✅ Export contract definition (Layer 2) |
| `FlatbuffersSaveDataExporter` | ✅ FlatBuffers serialization logic (Layer 3) |
| `JSONSaveDataExporter` | ✅ JSON serialization logic (Layer 3) |
| `XMLSaveDataExporter` | ✅ XML serialization logic (Layer 3) |

---

## Approach 3: Visitor Pattern

### Description

Implement the Visitor pattern to separate export logic from data structures. This allows adding new operations (export formats) without modifying the data classes.

### Implementation

```cpp
// src/interfaces/ISaveDataVisitor.h
namespace steamrot {

// Forward declarations
struct SaveData;
struct SaveMetaData;
struct SceneManagerData;
struct SceneData;

class ISaveDataVisitor {
public:
  virtual ~ISaveDataVisitor() = default;
  
  virtual void Visit(const SaveData& data) = 0;
  virtual void Visit(const SaveMetaData& data) = 0;
  virtual void Visit(const SceneManagerData& data) = 0;
  virtual void Visit(const SceneData& data) = 0;
};

}
```

```cpp
// src/types/core/SaveData.h - Modified to accept visitors
namespace steamrot {

struct SaveData {
  SaveMetaData meta_data;
  SceneManagerData scene_manager_data;
  SceneCollectionData scene_collection_data;
  
  /////////////////////////////////////////////////
  /// @brief Accept a visitor for double dispatch
  /////////////////////////////////////////////////
  void Accept(ISaveDataVisitor& visitor) const {
    visitor.Visit(*this);
    meta_data.Accept(visitor);
    scene_manager_data.Accept(visitor);
    for (const auto& scene : scene_collection_data) {
      scene->Accept(visitor);
    }
  }
};

}
```

```cpp
// src/data_providers/JSONExportVisitor.h
namespace steamrot {

class JSONExportVisitor : public ISaveDataVisitor {
  nlohmann::json m_json_output;
  
public:
  void Visit(const SaveData& data) override {
    m_json_output["version"] = "1.0";
    // Initialize root structure
  }
  
  void Visit(const SaveMetaData& data) override {
    m_json_output["metadata"]["save_name"] = data.save_name;
    m_json_output["metadata"]["file_id"] = uuids::to_string(data.file_id);
  }
  
  void Visit(const SceneManagerData& data) override {
    // Serialize scene manager data
  }
  
  void Visit(const SceneData& data) override {
    // Serialize individual scene
  }
  
  std::string GetJSON() const {
    return m_json_output.dump(2); // Pretty print with 2-space indent
  }
};

}
```

### Data Flow

```
┌────────────────────────────────────────────────┐
│  SaveData + nested data structures             │
│  - Each structure has Accept() method          │
└────────────┬───────────────────────────────────┘
             │
             │ save_data.Accept(visitor)
             │
             ▼
┌────────────────────────────────────────────────┐
│  ISaveDataVisitor (Traverses hierarchy)        │
│  - Visit(SaveData)                             │
│  - Visit(SaveMetaData)                         │
│  - Visit(SceneManagerData)                     │
│  - Visit(SceneData)                            │
└────────────┬───────────────────────────────────┘
             │
             │ Implemented by format-specific visitors
             │
    ┌────────┴────────┬────────────┐
    │                 │            │
    ▼                 ▼            ▼
┌──────────┐  ┌──────────┐  ┌──────────┐
│  JSON    │  │FlatBuffrs│  │   XML    │
│ Visitor  │  │ Visitor  │  │ Visitor  │
│          │  │          │  │          │
│Accumulate│  │Accumulate│  │Accumulate│
│ state    │  │ state    │  │ state    │
└────┬─────┘  └────┬─────┘  └────┬─────┘
     │            │             │
     │ GetJSON()  │ GetBinary() │ GetXML()
     ▼            ▼             ▼
┌─────────────────────────────────────────────────┐
│  Serialized Output                              │
└─────────────────────────────────────────────────┘
```

### Usage Example

```cpp
// Export to JSON using visitor
void ExportToJSON(const SaveData& save_data) {
  JSONExportVisitor visitor;
  save_data.Accept(visitor);
  std::string json = visitor.GetJSON();
  
  // Write to file
  std::ofstream file("save.json");
  file << json;
}
```

### Pros

✅ **Highly extensible** - Add new operations without modifying data structures
✅ **Separation of concerns** - Export logic completely separate from data
✅ **Type-safe traversal** - Compiler ensures all types are handled
✅ **Supports complex operations** - Can maintain state during traversal
✅ **Flexible** - Can combine multiple visitors (e.g., validation + export)

### Cons

❌ **High complexity** - Requires understanding of double dispatch
❌ **Intrusive** - Must add Accept() methods to all data structures
❌ **Verbose** - Requires visitor interface with methods for each type
❌ **Coupling to hierarchy** - Visitor interface changes when data structure changes
❌ **Overkill for simple exports** - Complex pattern for straightforward serialization
❌ **Layer violation** - Data structs (Layer 1) must know about visitors (Layer 2)

### When to Use

- Complex transformations needed during export
- Multiple operations on data structure (export, validation, analysis, etc.)
- Need to maintain state during traversal
- Data hierarchy is stable (doesn't change often)
- Team familiar with design patterns

### Responsibility Division

| Component | Responsibility |
|-----------|---------------|
| `SaveData` | ⚠️ Data structure + Accept visitor (coupled) |
| `ISaveDataVisitor` | ✅ Visitor contract (Layer 2) |
| `JSONExportVisitor` | ✅ JSON export logic + state management (Layer 3) |
| `FlatBuffersExportVisitor` | ✅ FlatBuffers export logic + state management (Layer 3) |

---

## Approach 4: Strategy Pattern with Format Registry

### Description

Combine the Strategy pattern with a dynamic registry that allows runtime registration and selection of export formats. This provides maximum flexibility for plugin systems or user-selectable export formats.

### Implementation

```cpp
// src/interfaces/ISaveDataExporter.h
namespace steamrot {

class ISaveDataExporter {
public:
  virtual ~ISaveDataExporter() = default;
  
  virtual std::expected<std::vector<uint8_t>, FailInfo>
  ExportSaveData(const SaveData& save_data) const = 0;
  
  virtual std::string GetFormatName() const = 0;
  virtual std::string GetFileExtension() const = 0;
  virtual std::string GetDescription() const = 0;
};

}
```

```cpp
// src/data_providers/SaveDataExporterRegistry.h
namespace steamrot {

class SaveDataExporterRegistry {
  std::unordered_map<std::string, std::unique_ptr<ISaveDataExporter>> m_exporters;
  
  // Singleton instance
  static SaveDataExporterRegistry& GetInstance() {
    static SaveDataExporterRegistry instance;
    return instance;
  }
  
public:
  /////////////////////////////////////////////////
  /// @brief Register an exporter for a format
  ///
  /// @param format_name Format identifier (e.g., "json", "flatbuffers")
  /// @param exporter Unique pointer to exporter implementation
  /////////////////////////////////////////////////
  static void RegisterExporter(const std::string& format_name,
                                std::unique_ptr<ISaveDataExporter> exporter) {
    GetInstance().m_exporters[format_name] = std::move(exporter);
  }
  
  /////////////////////////////////////////////////
  /// @brief Get an exporter for a specific format
  ///
  /// @param format_name Format identifier
  /// @return Pointer to exporter, or nullptr if not found
  /////////////////////////////////////////////////
  static ISaveDataExporter* GetExporter(const std::string& format_name) {
    auto& exporters = GetInstance().m_exporters;
    auto it = exporters.find(format_name);
    return (it != exporters.end()) ? it->second.get() : nullptr;
  }
  
  /////////////////////////////////////////////////
  /// @brief Get list of all registered formats
  /////////////////////////////////////////////////
  static std::vector<std::string> GetAvailableFormats() {
    std::vector<std::string> formats;
    for (const auto& [name, _] : GetInstance().m_exporters) {
      formats.push_back(name);
    }
    return formats;
  }
};

}
```

```cpp
// src/data_providers/FlatbuffersSaveDataExporter.h
namespace steamrot {

class FlatbuffersSaveDataExporter : public ISaveDataExporter {
public:
  // Auto-register on static initialization
  static bool RegisterFormat() {
    SaveDataExporterRegistry::RegisterExporter(
      "flatbuffers",
      std::make_unique<FlatbuffersSaveDataExporter>()
    );
    return true;
  }
  
  std::expected<std::vector<uint8_t>, FailInfo>
  ExportSaveData(const SaveData& save_data) const override {
    // Implementation
  }
  
  std::string GetFormatName() const override { return "flatbuffers"; }
  std::string GetFileExtension() const override { return ".bin"; }
  std::string GetDescription() const override {
    return "FlatBuffers binary format";
  }

private:
  static inline bool registered = RegisterFormat();
};

}
```

### Data Flow

```
┌────────────────────────────────────────────────┐
│  SaveData (Runtime)                            │
└────────────┬───────────────────────────────────┘
             │
             │ format_name (user selection)
             │
             ▼
┌────────────────────────────────────────────────┐
│  SaveDataExporterRegistry                      │
│  - Maintains format → exporter mapping         │
│  - GetExporter(format_name)                    │
│  - GetAvailableFormats()                       │
└────────────┬───────────────────────────────────┘
             │
             │ Returns exporter
             │
             ▼
┌────────────────────────────────────────────────┐
│  ISaveDataExporter (Specific format)           │
│  - ExportSaveData()                            │
└────────────┬───────────────────────────────────┘
             │
             ▼
┌────────────────────────────────────────────────┐
│  Serialized Output                             │
└────────────────────────────────────────────────┘
```

### Usage Example

```cpp
// Automatic registration (in exporter implementation files)
// This runs at static initialization time
namespace steamrot {
  static bool json_registered = []() {
    SaveDataExporterRegistry::RegisterExporter(
      "json",
      std::make_unique<JSONSaveDataExporter>()
    );
    return true;
  }();
}

// Runtime usage - user selects format
void ExportSave(const SaveData& save_data, const std::string& format) {
  // Get exporter for requested format
  auto* exporter = SaveDataExporterRegistry::GetExporter(format);
  if (!exporter) {
    // Format not supported
    return;
  }
  
  // Export
  auto result = exporter->ExportSaveData(save_data);
  if (result.has_value()) {
    // Write to file with appropriate extension
    std::string filename = "save" + exporter->GetFileExtension();
    // ... write file
  }
}

// List available formats for UI
void ShowExportOptions() {
  auto formats = SaveDataExporterRegistry::GetAvailableFormats();
  std::cout << "Available export formats:\n";
  for (const auto& format : formats) {
    auto* exporter = SaveDataExporterRegistry::GetExporter(format);
    std::cout << "  " << format << " - " 
              << exporter->GetDescription() << "\n";
  }
}
```

### Pros

✅ **Maximum flexibility** - Runtime format selection
✅ **Plugin architecture** - Easy to add formats without recompiling
✅ **User-friendly** - Can list available formats for UI
✅ **Auto-registration** - Formats register themselves
✅ **Decoupled** - New formats don't affect existing code
✅ **Dynamic discovery** - Can load exporters from DLLs/shared libraries

### Cons

❌ **Complex initialization** - Static initialization order can be tricky
❌ **Global state** - Registry is a singleton (can complicate testing)
❌ **Runtime overhead** - String-based lookup vs compile-time dispatch
❌ **Memory management** - Registry owns exporters (lifetime considerations)
❌ **Debugging difficulty** - Harder to trace registration and lookup
❌ **Overkill for fixed formats** - If formats are known at compile time, simpler approaches work

### When to Use

- Plugin system where formats loaded dynamically
- User-selectable export formats in UI
- Format support varies by platform/build configuration
- Need to discover available formats at runtime
- Building a framework/library for others to extend
- Export formats may be added by third parties

### Responsibility Division

| Component | Responsibility |
|-----------|---------------|
| `SaveData` | ✅ Pure data structure (Layer 1) |
| `ISaveDataExporter` | ✅ Export contract (Layer 2) |
| `SaveDataExporterRegistry` | ✅ Format registration and lookup (Layer 2/3 boundary) |
| `FlatbuffersSaveDataExporter` | ✅ FlatBuffers export + self-registration (Layer 3) |
| `JSONSaveDataExporter` | ✅ JSON export + self-registration (Layer 3) |

---

## Comparative Analysis

### Feature Comparison Matrix

| Feature | Direct Serialization | Abstract Exporter | Visitor Pattern | Strategy Registry |
|---------|---------------------|-------------------|-----------------|-------------------|
| **Extensibility** | ⭐ Poor | ⭐⭐⭐⭐ Excellent | ⭐⭐⭐⭐ Excellent | ⭐⭐⭐⭐⭐ Outstanding |
| **Testability** | ⭐⭐ Fair | ⭐⭐⭐⭐⭐ Excellent | ⭐⭐⭐⭐ Very Good | ⭐⭐⭐⭐ Very Good |
| **Complexity** | ⭐⭐⭐⭐⭐ Simple | ⭐⭐⭐ Moderate | ⭐ Complex | ⭐⭐ Moderate-Complex |
| **Performance** | ⭐⭐⭐⭐⭐ Excellent | ⭐⭐⭐⭐⭐ Excellent | ⭐⭐⭐⭐ Very Good | ⭐⭐⭐⭐ Very Good |
| **Coupling** | ⭐ High | ⭐⭐⭐⭐⭐ Very Low | ⭐⭐ Moderate | ⭐⭐⭐⭐⭐ Very Low |
| **Follows SteamRot Patterns** | ❌ No | ✅ Yes | ❌ No | ⚠️ Partial |
| **Initial Dev Time** | ⭐⭐⭐⭐⭐ Fast | ⭐⭐⭐ Moderate | ⭐ Slow | ⭐⭐ Moderate-Slow |
| **Maintenance** | ⭐ Difficult | ⭐⭐⭐⭐⭐ Easy | ⭐⭐⭐ Moderate | ⭐⭐⭐⭐ Easy |

### Use Case Recommendations

| Use Case | Recommended Approach | Reason |
|----------|---------------------|---------|
| **Quick prototype** | Direct Serialization | Fast to implement, acceptable for throwaway code |
| **Production game saves** | Abstract Exporter | Follows established patterns, extensible, testable |
| **Multiple formats needed** | Abstract Exporter | Clean interface for each format |
| **Complex data transformations** | Visitor Pattern | Maintains state during traversal |
| **User-selectable formats** | Strategy Registry | Runtime format discovery and selection |
| **Plugin system** | Strategy Registry | Dynamic format loading |
| **Simple utility** | Direct Serialization | Minimal overhead |
| **Long-term project** | Abstract Exporter | Best maintainability/extensibility balance |

### Code Metrics Comparison

Estimated lines of code for exporting to 3 formats (JSON, FlatBuffers, XML):

| Approach | Interface LOC | Implementation LOC (per format) | Total LOC |
|----------|--------------|--------------------------------|----------|
| Direct Serialization | 0 | ~100-150 | ~300-450 |
| Abstract Exporter | ~50 | ~150-200 | ~500-650 |
| Visitor Pattern | ~100 | ~200-250 | ~700-850 |
| Strategy Registry | ~150 | ~200-250 | ~750-900 |

---

## Responsibility Division

### Layered Architecture

Following SteamRot's established three-layer architecture:

```
┌─────────────────────────────────────────────────────────┐
│  Layer 1: Types/Data (src/types/)                       │
│  - SaveData, SaveMetaData, SceneData                    │
│  - Pure data structs                                    │
│  - ZERO dependencies on other internal packages         │
│  ✅ Responsibility: Data structure definition only      │
└─────────────────────────────────────────────────────────┘
                        ▲
                        │ depends on
                        │
┌─────────────────────────────────────────────────────────┐
│  Layer 2: Interfaces (src/interfaces/)                  │
│  - ISaveDataExporter, ISaveDataProvider                 │
│  - Abstract contracts                                   │
│  - Depends only on Layer 1 (types)                      │
│  ✅ Responsibility: Define contracts and behavior       │
└─────────────────────────────────────────────────────────┘
                        ▲
                        │ implements
                        │
┌─────────────────────────────────────────────────────────┐
│  Layer 3: Services/Implementation (src/data_providers/) │
│  - FlatbuffersSaveDataExporter                          │
│  - JSONSaveDataExporter                                 │
│  - XMLSaveDataExporter                                  │
│  - Depends on Layers 1-2 + external libraries           │
│  ✅ Responsibility: Concrete serialization logic        │
└─────────────────────────────────────────────────────────┘
```

### Comparison: Import vs Export Responsibilities

**Current Import Architecture:**

```
ISaveDataProvider (Interface)
    ↓ implements
FlatbuffersSaveDataProvider (Concrete)
    ↓ provides
SaveData (Data)
```

**Proposed Export Architecture (Symmetric):**

```
SaveData (Data)
    ↓ passed to
ISaveDataExporter (Interface)
    ↓ implements
FlatbuffersSaveDataExporter (Concrete)
    ↓ produces
Serialized Output
```

### Class Responsibility Summary

| Class | Layer | Responsibilities | Dependencies |
|-------|-------|-----------------|--------------|
| `SaveData` | 1 | Data structure only | None (pure data) |
| `SaveMetaData` | 1 | Metadata structure | None (pure data) |
| `SceneData` | 1 | Scene data structure | None (pure data) |
| `ISaveDataExporter` | 2 | Export contract | Layer 1 types |
| `FlatbuffersSaveDataExporter` | 3 | FlatBuffers serialization | Layers 1-2 + FlatBuffers lib |
| `JSONSaveDataExporter` | 3 | JSON serialization | Layers 1-2 + JSON lib |
| `XMLSaveDataExporter` | 3 | XML serialization | Layers 1-2 + XML lib |

---

## Recommended Architecture

### Primary Recommendation: Abstract Exporter Interface (Approach 2)

**Rationale:**
1. **Follows established patterns** - Mirrors ISaveDataProvider architecture
2. **Balanced complexity** - Not too simple, not over-engineered
3. **Extensible** - Easy to add new formats
4. **Testable** - Mock exporters for testing
5. **Maintainable** - Clear responsibilities
6. **Production-ready** - Suitable for long-term projects

### Architecture Diagram

```
┌──────────────────────────────────────────────────────────────┐
│                     Game Runtime                             │
│                                                              │
│  ┌────────────┐                                             │
│  │  SaveData  │ (Layer 1 - Types)                           │
│  └─────┬──────┘                                             │
│        │                                                     │
│        │ passed to                                           │
│        ▼                                                     │
│  ┌─────────────────┐                                        │
│  │ISaveDataExporter│ (Layer 2 - Interface)                  │
│  └────────┬────────┘                                        │
│           │                                                  │
│    ┌──────┴───────┬──────────────┬─────────────┐           │
│    │              │              │             │           │
│    ▼              ▼              ▼             ▼           │
│  ┌────┐       ┌────┐        ┌────┐       ┌────┐          │
│  │FBs │       │JSON│        │XML │       │CSV │ (Layer 3)│
│  │Exp.│       │Exp.│        │Exp.│       │Exp.│          │
│  └─┬──┘       └─┬──┘        └─┬──┘       └─┬──┘          │
│    │            │             │            │              │
└────┼────────────┼─────────────┼────────────┼──────────────┘
     │            │             │            │
     ▼            ▼             ▼            ▼
┌──────────────────────────────────────────────────────────────┐
│                    Output Files                              │
│  save.bin     save.json     save.xml     save.csv           │
└──────────────────────────────────────────────────────────────┘
```

### Interface Definition (Recommended)

```cpp
// src/interfaces/ISaveDataExporter.h
/////////////////////////////////////////////////
/// @file
/// @brief Definition of the ISaveDataExporter interface.
/////////////////////////////////////////////////

#pragma once

#include "FailInfo.h"
#include "SaveData.h"
#include <expected>
#include <string>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @class ISaveDataExporter
/// @brief Interface for exporting SaveData to various formats.
///
/// This interface defines the contract for exporting SaveData
/// to different serialization formats (FlatBuffers, JSON, XML, etc.).
/// Follows the Provider pattern established in the codebase.
/////////////////////////////////////////////////
class ISaveDataExporter {
public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor.
  /////////////////////////////////////////////////
  virtual ~ISaveDataExporter() = default;

  /////////////////////////////////////////////////
  /// @brief Export SaveData to byte buffer.
  ///
  /// Pure serialization without I/O. Use for network uploads,
  /// testing, clipboard operations, or in-memory processing.
  ///
  /// @param save_data The SaveData to export
  /// @return Serialized data as byte vector, or FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<std::vector<uint8_t>, FailInfo>
  ExportSaveData(const SaveData &save_data) const = 0;

  /////////////////////////////////////////////////
  /// @brief Export SaveData directly to file (convenience method).
  ///
  /// Default implementation serializes to buffer then writes to file.
  /// Override for format-specific optimizations.
  ///
  /// @param save_data The SaveData to export
  /// @param file_path Full path where file should be written
  /// @return Success (monostate) or FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ExportToFile(const SaveData &save_data,
               const std::string &file_path) const;

  /////////////////////////////////////////////////
  /// @brief Get the file extension for this format.
  ///
  /// @return File extension including dot (e.g., ".json", ".bin")
  /////////////////////////////////////////////////
  virtual std::string GetFileExtension() const = 0;

  /////////////////////////////////////////////////
  /// @brief Get the MIME type for this format.
  ///
  /// @return MIME type string (e.g., "application/json")
  /////////////////////////////////////////////////
  virtual std::string GetMimeType() const = 0;
};

} // namespace steamrot
```

### Implementation Template

```cpp
// src/data_providers/JSONSaveDataExporter.h
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of JSONSaveDataExporter class.
/////////////////////////////////////////////////

#pragma once

#include "ISaveDataExporter.h"
#include "SaveData.h"
#include <nlohmann/json.hpp>

namespace steamrot {

class JSONSaveDataExporter : public ISaveDataExporter {
public:
  /////////////////////////////////////////////////
  /// @brief Constructor for JSONSaveDataExporter.
  /////////////////////////////////////////////////
  JSONSaveDataExporter() = default;

  /////////////////////////////////////////////////
  /// @brief Export SaveData to JSON format.
  ///
  /// @param save_data The SaveData to export
  /// @return JSON data as byte vector, or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::vector<uint8_t>, FailInfo>
  ExportSaveData(const SaveData &save_data) const override;

  /////////////////////////////////////////////////
  /// @brief Get file extension for JSON format.
  ///
  /// @return ".json"
  /////////////////////////////////////////////////
  std::string GetFileExtension() const override { return ".json"; }

  /////////////////////////////////////////////////
  /// @brief Get MIME type for JSON format.
  ///
  /// @return "application/json"
  /////////////////////////////////////////////////
  std::string GetMimeType() const override { return "application/json"; }

private:
  /////////////////////////////////////////////////
  /// @brief Serialize SaveMetaData to JSON object.
  ///
  /// @param meta_data SaveMetaData to serialize
  /// @return JSON object, or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<nlohmann::json, FailInfo>
  SerializeSaveMetaData(const SaveMetaData &meta_data) const;

  /////////////////////////////////////////////////
  /// @brief Serialize SceneManagerData to JSON object.
  ///
  /// @param scene_manager_data SceneManagerData to serialize
  /// @return JSON object, or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<nlohmann::json, FailInfo> SerializeSceneManagerData(
      const SceneManagerData &scene_manager_data) const;
};

} // namespace steamrot
```

### Optional Enhancement: Strategy Registry

For projects that need runtime format selection (e.g., UI with export options), combine Approach 2 (Abstract Exporter) with Approach 4 (Registry):

```cpp
// src/data_providers/SaveDataExporterFactory.h
namespace steamrot {

class SaveDataExporterFactory {
public:
  /////////////////////////////////////////////////
  /// @brief Create an exporter for the specified format.
  ///
  /// @param format_name Format identifier ("json", "flatbuffers", etc.)
  /// @return Unique pointer to exporter, or nullptr if format unknown
  /////////////////////////////////////////////////
  static std::unique_ptr<ISaveDataExporter>
  CreateExporter(const std::string &format_name) {
    if (format_name == "json") {
      return std::make_unique<JSONSaveDataExporter>();
    } else if (format_name == "flatbuffers") {
      return std::make_unique<FlatbuffersSaveDataExporter>();
    } else if (format_name == "xml") {
      return std::make_unique<XMLSaveDataExporter>();
    }
    return nullptr;
  }

  /////////////////////////////////////////////////
  /// @brief Get list of supported formats.
  ///
  /// @return Vector of format names
  /////////////////////////////////////////////////
  static std::vector<std::string> GetSupportedFormats() {
    return {"json", "flatbuffers", "xml"};
  }
};

} // namespace steamrot
```

---

## Return Type Alternatives: Addressing Type Erasure and File I/O

### Question: Can We Avoid `std::vector<uint8_t>`?

The recommended interface uses `std::vector<uint8_t>` as a common return type across all exporters:

```cpp
virtual std::expected<std::vector<uint8_t>, FailInfo>
ExportSaveData(const SaveData& save_data) const = 0;
```

This raises valid questions about whether we can:
1. **Overload return types** - Return different types from the same method
2. **Avoid type erasure** - Return exact types (e.g., `std::string` for JSON, `flatbuffers::DetachedBuffer` for FlatBuffers)
3. **Export directly to file** - Write files internally, eliminating the need for return values

### Option Analysis

#### Option 1: Return Type Overloading (Not Possible in C++)

**C++ Limitation:**
```cpp
// ❌ This is NOT valid C++ - cannot overload by return type alone
class ISaveDataExporter {
  virtual std::string ExportSaveData(...) const = 0;  // JSON
  virtual std::vector<uint8_t> ExportSaveData(...) const = 0;  // Binary
  // ERROR: Cannot overload solely on return type
};
```

**Why it doesn't work:**
- C++ function resolution is based on function name + parameters, NOT return type
- The compiler cannot determine which overload to call based on how the return value is used
- This is a fundamental language limitation, not a design choice

**Alternative (Templates with Tag Dispatch):**
```cpp
template<typename FormatTag>
class ISaveDataExporter {
  auto ExportSaveData(const SaveData& data) const;
};

// Specializations
template<>
std::string ISaveDataExporter<JSONTag>::ExportSaveData(...) const;

template<>
std::vector<uint8_t> ISaveDataExporter<BinaryTag>::ExportSaveData(...) const;
```

**Problems with this approach:**
- ❌ Loses polymorphism - can't use interface pointers
- ❌ Breaks dependency injection pattern
- ❌ Cannot store different exporters in same collection
- ❌ No runtime format selection
- ❌ More complex than the benefit warrants

**Verdict:** Not recommended for this use case.

---

#### Option 2: Format-Specific Return Types with `std::variant`

**Implementation:**
```cpp
// Define variant for all possible return types
using ExportResult = std::variant<
  std::string,                      // JSON, XML text
  std::vector<uint8_t>,             // Binary formats
  flatbuffers::DetachedBuffer       // FlatBuffers specific
>;

class ISaveDataExporter {
public:
  virtual std::expected<ExportResult, FailInfo>
  ExportSaveData(const SaveData& save_data) const = 0;
  
  // Helper to identify what's in the variant
  virtual ExportResultType GetResultType() const = 0;
};
```

**Usage:**
```cpp
JSONSaveDataExporter exporter;
auto result = exporter.ExportSaveData(save_data);

if (result.has_value()) {
  std::visit([](auto&& data) {
    using T = std::decay_t<decltype(data)>;
    if constexpr (std::is_same_v<T, std::string>) {
      // Handle string (JSON/XML)
      std::cout << data;
    } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
      // Handle binary
      WriteToFile(data);
    }
  }, result.value());
}
```

**Pros:**
✅ Type-safe - compiler ensures all types handled
✅ No data copying - can move out of variant
✅ More expressive - caller knows exact type

**Cons:**
❌ More complex API - requires `std::visit` or `std::get`
❌ Caller must handle all variant types
❌ Still type erasure, just different form
❌ Error-prone - easy to forget a type in visitor
❌ Doesn't simplify the common case (write to file)

**Verdict:** Adds complexity without significant benefit for file I/O use case.

---

#### Option 3: Direct File Export (Recommended Alternative)

**Implementation:**
```cpp
// src/interfaces/ISaveDataExporter.h
class ISaveDataExporter {
public:
  virtual ~ISaveDataExporter() = default;
  
  /////////////////////////////////////////////////
  /// @brief Export SaveData directly to file
  ///
  /// @param save_data The SaveData to export
  /// @param file_path Full path where file should be written
  /// @return Success (monostate) or FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ExportToFile(const SaveData& save_data, 
               const std::string& file_path) const = 0;
  
  /////////////////////////////////////////////////
  /// @brief Get the file extension for this format
  /////////////////////////////////////////////////
  virtual std::string GetFileExtension() const = 0;
};
```

**Implementation Example:**
```cpp
// src/data_providers/JSONSaveDataExporter.cpp
std::expected<std::monostate, FailInfo>
JSONSaveDataExporter::ExportToFile(const SaveData& save_data,
                                   const std::string& file_path) const {
  // Serialize to JSON string
  nlohmann::json json_obj;
  json_obj["metadata"]["save_name"] = save_data.meta_data.save_name;
  // ... populate rest of JSON
  
  // Write directly to file
  std::ofstream file(file_path);
  if (!file.is_open()) {
    return std::unexpected(FailInfo{FailMode::FileOpenFailed,
                                    "Could not open file: " + file_path});
  }
  
  file << json_obj.dump(2);  // Pretty print with 2-space indent
  
  if (file.fail()) {
    return std::unexpected(FailInfo{FailMode::FileWriteFailed,
                                    "Failed to write to file"});
  }
  
  return std::monostate{};
}
```

**Usage:**
```cpp
// Simple and clean
JSONSaveDataExporter exporter;
auto result = exporter.ExportToFile(save_data, "/path/to/save.json");

if (!result.has_value()) {
  std::cerr << "Export failed: " << result.error().message << "\n";
}
```

**Pros:**
✅ **Simple API** - Single responsibility: export to file
✅ **No return type complexity** - Returns success/failure only
✅ **Efficient** - No intermediate buffer, write directly
✅ **Natural for file I/O** - Matches most common use case
✅ **Clean error handling** - File errors caught at export time
✅ **Matches common usage** - Most exports go to files anyway

**Cons:**
⚠️ **Less flexible** - Cannot export to memory (e.g., network, clipboard)
⚠️ **Harder to test** - Requires file system access in tests
⚠️ **Mixing concerns** - Exporter handles both serialization AND I/O

**When to use:**
- ✅ Primary use case is file export
- ✅ Don't need in-memory serialization
- ✅ Simplicity is priority
- ⚠️ May need to add in-memory method later if requirements change

---

#### Option 4: Hybrid Approach (Best of Both Worlds)

**Implementation:**
```cpp
class ISaveDataExporter {
public:
  virtual ~ISaveDataExporter() = default;
  
  /////////////////////////////////////////////////
  /// @brief Export SaveData to byte buffer (for flexibility)
  ///
  /// Use this when you need the serialized data in memory
  /// (e.g., network upload, clipboard, testing)
  /////////////////////////////////////////////////
  virtual std::expected<std::vector<uint8_t>, FailInfo>
  ExportSaveData(const SaveData& save_data) const = 0;
  
  /////////////////////////////////////////////////
  /// @brief Export SaveData directly to file (convenience)
  ///
  /// Default implementation uses ExportSaveData() + file write.
  /// Implementations can override for format-specific optimizations.
  ///
  /// @param save_data The SaveData to export
  /// @param file_path Full path where file should be written
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ExportToFile(const SaveData& save_data,
               const std::string& file_path) const {
    // Default implementation: serialize + write
    auto data_result = ExportSaveData(save_data);
    if (!data_result.has_value()) {
      return std::unexpected(data_result.error());
    }
    
    // Write to file
    std::ofstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
      return std::unexpected(FailInfo{FailMode::FileOpenFailed,
                                      "Could not open: " + file_path});
    }
    
    const auto& data = data_result.value();
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    
    if (file.fail()) {
      return std::unexpected(FailInfo{FailMode::FileWriteFailed,
                                      "Failed to write file"});
    }
    
    return std::monostate{};
  }
  
  virtual std::string GetFileExtension() const = 0;
  virtual std::string GetMimeType() const = 0;
};
```

**Key Benefits:**
- **ExportSaveData()** - Pure serialization, returns data buffer
  - Required for implementations
  - Flexible: network, testing, clipboard, etc.
  - Testable without file system
  
- **ExportToFile()** - High-level convenience method
  - Has default implementation (calls ExportSaveData())
  - Can be overridden for optimization
  - Simple for common file export case

**Usage - Simple File Export:**
```cpp
JSONSaveDataExporter exporter;
auto result = exporter.ExportToFile(save_data, "save.json");
// Clean and simple!
```

**Usage - Flexible (Network/Testing):**
```cpp
JSONSaveDataExporter exporter;
auto data = exporter.ExportSaveData(save_data);
if (data.has_value()) {
  // Upload to cloud
  UploadToCloud(data.value());
  // Or test contents
  REQUIRE(data.value().size() > 0);
}
```

**Format-Specific Optimization:**
```cpp
// FlatBuffers can optimize by avoiding copy
class FlatbuffersSaveDataExporter : public ISaveDataExporter {
  std::expected<std::monostate, FailInfo>
  ExportToFile(const SaveData& save_data,
               const std::string& file_path) const override {
    // Build directly to file without intermediate buffer
    flatbuffers::FlatBufferBuilder builder;
    // ... build ...
    
    // Write directly from builder (more efficient)
    std::ofstream file(file_path, std::ios::binary);
    file.write(reinterpret_cast<const char*>(builder.GetBufferPointer()),
               builder.GetSize());
    return std::monostate{};
  }
};
```

**Pros:**
✅ **Flexible** - Both in-memory and file export supported
✅ **Simple common case** - Just call ExportToFile()
✅ **Testable** - ExportSaveData() doesn't touch file system
✅ **Optimizable** - Implementations can override for efficiency
✅ **Backward compatible** - Add ExportToFile() without breaking existing code
✅ **Matches real usage** - File export common, but not only use case

**Cons:**
⚠️ **Two methods** - Slightly more API surface
⚠️ **std::vector<uint8_t> still used** - But only for flexible use cases

**Verdict:** ⭐ **Recommended** - Best balance of simplicity and flexibility.

---

### Recommendation Summary

| Approach | Complexity | Flexibility | Testability | Recommended? |
|----------|-----------|-------------|-------------|--------------|
| **Original (vector return)** | Low | High | High | ✅ Good baseline |
| **Return type overloading** | N/A | N/A | N/A | ❌ Not possible in C++ |
| **std::variant return** | High | Medium | Medium | ⚠️ Overkill |
| **Direct file export only** | Low | Low | Medium | ⚠️ Too restrictive |
| **Hybrid (both methods)** | Low-Medium | High | High | ⭐ **Best choice** |

### Recommended Interface (Updated)

```cpp
// src/interfaces/ISaveDataExporter.h
namespace steamrot {

class ISaveDataExporter {
public:
  virtual ~ISaveDataExporter() = default;
  
  /////////////////////////////////////////////////
  /// @brief Export SaveData to byte buffer
  ///
  /// Pure serialization without I/O. Use for:
  /// - Network uploads
  /// - Testing (no file system dependency)
  /// - Clipboard operations
  /// - In-memory processing
  ///
  /// @param save_data The SaveData to export
  /// @return Serialized data, or FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<std::vector<uint8_t>, FailInfo>
  ExportSaveData(const SaveData& save_data) const = 0;
  
  /////////////////////////////////////////////////
  /// @brief Export SaveData directly to file (convenience)
  ///
  /// Default implementation serializes to buffer then writes.
  /// Override for format-specific optimizations.
  ///
  /// @param save_data The SaveData to export
  /// @param file_path Full path where file should be written
  /// @return Success (monostate) or FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ExportToFile(const SaveData& save_data,
               const std::string& file_path) const;
  
  virtual std::string GetFileExtension() const = 0;
  virtual std::string GetMimeType() const = 0;
};

} // namespace steamrot
```

**Key Points:**
1. **Keep `ExportSaveData()` with `std::vector<uint8_t>`** - It's the right abstraction for serialized data
2. **Add `ExportToFile()` as convenience** - Covers 90% of use cases with simple API
3. **Provide default implementation** - Implementers only need to override if optimizing
4. **No return type overloading needed** - Two separate methods with clear purposes

This hybrid approach addresses all concerns:
- ✅ Avoids complexity of `std::variant` or templates
- ✅ Provides simple file export without return value confusion
- ✅ Maintains flexibility for non-file use cases
- ✅ Stays testable and maintainable
- ✅ Follows established C++ patterns

---

## Assembling SaveData from Engine State

### Question: Should ISaveDataProvider Support Creating SaveData from Engine?

The current `ISaveDataProvider` interface is designed for **loading** SaveData from external sources (files):

```cpp
class ISaveDataProvider {
  virtual std::expected<SaveData, FailInfo> ProvideSaveData() const = 0;
};
```

The question asks: Could we extend this interface to **assemble** SaveData from the current Engine state? This would complete the save/load cycle:

```
Engine State → SaveData → Export to File (save)
File → Import to SaveData → Engine State (load)
```

### Current Architecture Analysis

**What we have:**
- **ISaveDataProvider** - Loads SaveData from files (import)
- **ISaveDataExporter** (proposed) - Writes SaveData to files (export)

**What's missing:**
- **Creating SaveData from runtime Engine state**

This is actually a third operation distinct from import/export:
1. **Import** (Provider): File → SaveData
2. **Export** (Exporter): SaveData → File
3. **Capture** (???): Engine → SaveData

### Option 1: Extend ISaveDataProvider (Not Recommended)

**Implementation:**
```cpp
class ISaveDataProvider {
public:
  // Existing: Load from file
  virtual std::expected<SaveData, FailInfo> ProvideSaveData() const = 0;
  
  // New: Create from Engine state
  virtual std::expected<SaveData, FailInfo> 
  AssembleSaveData(const Engine& engine) const = 0;
};
```

**Problems:**
❌ **Violates Interface Segregation Principle** - Not all providers need both methods
❌ **Name confusion** - "Provider" implies providing from external source, not creating
❌ **Wrong responsibility** - Provider's job is to load, not to capture state
❌ **Asymmetric** - No corresponding method on ISaveDataExporter
❌ **Coupling** - Provider would depend on Engine (high-level depends on low-level)

**Verdict:** Don't extend ISaveDataProvider for this purpose.

---

### Option 2: Create ISaveDataAssembler Interface (Recommended)

**Implementation:**
```cpp
// src/interfaces/ISaveDataAssembler.h
namespace steamrot {

/////////////////////////////////////////////////
/// @class ISaveDataAssembler
/// @brief Interface for creating SaveData from runtime Engine state.
///
/// This interface handles the "capture" operation: extracting current
/// game state from the Engine and packaging it into a SaveData object.
/// This is distinct from:
/// - ISaveDataProvider (loads SaveData from files)
/// - ISaveDataExporter (writes SaveData to files)
/////////////////////////////////////////////////
class ISaveDataAssembler {
public:
  virtual ~ISaveDataAssembler() = default;
  
  /////////////////////////////////////////////////
  /// @brief Assemble SaveData from current Engine state.
  ///
  /// Extracts all necessary state information from the Engine
  /// (scenes, entities, game progress, etc.) and packages it
  /// into a SaveData object ready for export.
  ///
  /// @param engine Reference to the Engine containing current state
  /// @return Assembled SaveData, or FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<SaveData, FailInfo>
  AssembleSaveData(const Engine& engine) const = 0;
};

} // namespace steamrot
```

**Concrete Implementation:**
```cpp
// src/data_providers/EngineSaveDataAssembler.h
namespace steamrot {

class EngineSaveDataAssembler : public ISaveDataAssembler {
public:
  EngineSaveDataAssembler() = default;
  
  std::expected<SaveData, FailInfo>
  AssembleSaveData(const Engine& engine) const override;

private:
  /////////////////////////////////////////////////
  /// @brief Extract SaveMetaData from Engine
  /////////////////////////////////////////////////
  std::expected<SaveMetaData, FailInfo>
  ExtractMetaData(const Engine& engine) const;
  
  /////////////////////////////////////////////////
  /// @brief Extract SceneManagerData from Engine
  /////////////////////////////////////////////////
  std::expected<SceneManagerData, FailInfo>
  ExtractSceneManagerData(const Engine& engine) const;
  
  /////////////////////////////////////////////////
  /// @brief Extract SceneCollectionData from Engine
  /////////////////////////////////////////////////
  std::expected<SceneCollectionData, FailInfo>
  ExtractSceneCollectionData(const Engine& engine) const;
};

} // namespace steamrot
```

**Usage - Complete Save Operation:**
```cpp
void SaveGame(const Engine& engine, const std::string& save_name) {
  // Step 1: Assemble SaveData from Engine state
  EngineSaveDataAssembler assembler;
  auto save_data_result = assembler.AssembleSaveData(engine);
  
  if (!save_data_result.has_value()) {
    std::cerr << "Failed to assemble save data\n";
    return;
  }
  
  SaveData save_data = save_data_result.value();
  save_data.meta_data.save_name = save_name;
  
  // Step 2: Export SaveData to file
  FlatbuffersSaveDataExporter exporter;
  auto export_result = exporter.ExportToFile(save_data, 
                                             "/saves/" + save_name + ".bin");
  
  if (!export_result.has_value()) {
    std::cerr << "Failed to export save data\n";
    return;
  }
  
  std::cout << "Game saved successfully!\n";
}
```

**Usage - Complete Load Operation:**
```cpp
std::expected<std::monostate, FailInfo> 
LoadGame(Engine& engine, const std::string& save_name) {
  // Step 1: Load SaveData from file
  FlatbuffersSaveDataProvider provider;
  auto save_data_result = provider.ProvideSaveData();
  
  if (!save_data_result.has_value()) {
    return std::unexpected(save_data_result.error());
  }
  
  SaveData save_data = save_data_result.value();
  
  // Step 2: Apply SaveData to Engine
  // (This would be a separate operation, not covered in this document)
  // Could be: engine.RestoreFromSaveData(save_data);
  // Or: SaveDataRestorer restorer; restorer.RestoreEngine(engine, save_data);
  
  return std::monostate{};
}
```

**Pros:**
✅ **Clear separation of concerns** - Each interface has one purpose
✅ **Interface Segregation** - Implementations only need what they use
✅ **Intuitive naming** - "Assembler" clearly indicates creating from parts
✅ **Symmetric architecture** - Three clear operations (assemble, export, import)
✅ **Independent evolution** - Can change assembler without affecting provider/exporter
✅ **Testable** - Can mock Engine for testing

**Cons:**
⚠️ **More interfaces** - Three instead of two (but each is focused)
⚠️ **Potential coupling** - Assembler depends on Engine (but this is acceptable - it's the assembler's job)

---

### Option 3: Save Manager Facade (Alternative)

**Implementation:**
```cpp
// src/engine/SaveManager.h
namespace steamrot {

/////////////////////////////////////////////////
/// @class SaveManager
/// @brief High-level facade for save/load operations.
///
/// Coordinates between Assembler, Exporter, and Provider
/// to provide simple save/load API.
/////////////////////////////////////////////////
class SaveManager {
  const ISaveDataAssembler& m_assembler;
  const ISaveDataExporter& m_exporter;
  const ISaveDataProvider& m_provider;

public:
  SaveManager(const ISaveDataAssembler& assembler,
              const ISaveDataExporter& exporter,
              const ISaveDataProvider& provider)
    : m_assembler(assembler)
    , m_exporter(exporter)
    , m_provider(provider) {}
  
  /////////////////////////////////////////////////
  /// @brief Save current Engine state to file.
  ///
  /// High-level operation that:
  /// 1. Assembles SaveData from Engine
  /// 2. Exports SaveData to file
  ///
  /// @param engine Current Engine instance
  /// @param save_name Name for the save file
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  SaveGame(const Engine& engine, const std::string& save_name) {
    // Assemble
    auto save_data_result = m_assembler.AssembleSaveData(engine);
    if (!save_data_result.has_value()) {
      return std::unexpected(save_data_result.error());
    }
    
    SaveData save_data = save_data_result.value();
    save_data.meta_data.save_name = save_name;
    
    // Export
    std::string file_path = "/saves/" + save_name + 
                           m_exporter.GetFileExtension();
    return m_exporter.ExportToFile(save_data, file_path);
  }
  
  /////////////////////////////////////////////////
  /// @brief Load game state from file into Engine.
  ///
  /// High-level operation that:
  /// 1. Imports SaveData from file
  /// 2. Restores Engine state from SaveData
  ///
  /// @param engine Engine instance to restore into
  /// @param save_name Name of the save file to load
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  LoadGame(Engine& engine, const std::string& save_name) {
    // Import
    auto save_data_result = m_provider.ProvideSaveData();
    if (!save_data_result.has_value()) {
      return std::unexpected(save_data_result.error());
    }
    
    // Restore (would need implementation)
    // return engine.RestoreFromSaveData(save_data_result.value());
    return std::monostate{};
  }
};

} // namespace steamrot
```

**Usage:**
```cpp
// Setup
EngineSaveDataAssembler assembler;
FlatbuffersSaveDataExporter exporter;
FlatbuffersSaveDataProvider provider;

SaveManager save_manager(assembler, exporter, provider);

// Save
auto save_result = save_manager.SaveGame(engine, "quicksave");

// Load
auto load_result = save_manager.LoadGame(engine, "quicksave");
```

**Pros:**
✅ **Simple API** - Single point of contact for save/load
✅ **Encapsulates complexity** - Hides assembler/exporter/provider details
✅ **Coordinated operations** - Ensures correct sequence
✅ **Format agnostic** - Can swap exporter/provider implementations

**Cons:**
⚠️ **Additional layer** - Adds abstraction (but simplifies usage)
⚠️ **Dependency injection** - Requires all three components

---

### Complete Architecture: Three Operations

```
┌─────────────────────────────────────────────────────────┐
│                    Engine (Runtime)                      │
│  - Scenes, Entities, Game State                         │
└───────────┬─────────────────────────────────────────────┘
            │
            │ 3. Capture State
            │ (ISaveDataAssembler)
            ▼
┌─────────────────────────────────────────────────────────┐
│                   SaveData (Memory)                      │
│  - meta_data, scene_manager_data, scene_collection_data │
└───────────┬─────────────────────────────────────────────┘
            │
            │ 2. Export           1. Import
            │ (ISaveDataExporter) (ISaveDataProvider)
            │                     │
            ▼                     ▼
┌─────────────────────────────────────────────────────────┐
│              File System (.bin, .json, etc.)             │
└─────────────────────────────────────────────────────────┘
```

**Three distinct interfaces:**
1. **ISaveDataProvider** - Import: File → SaveData
2. **ISaveDataExporter** - Export: SaveData → File
3. **ISaveDataAssembler** - Capture: Engine → SaveData

**Optional coordination:**
- **SaveManager** - Facade that combines all three

---

### Recommendation Summary

| Approach | Clarity | Separation | Recommended? |
|----------|---------|------------|--------------|
| **Extend ISaveDataProvider** | ❌ Confusing | ❌ Mixed concerns | ❌ No |
| **Create ISaveDataAssembler** | ✅ Clear | ✅ Single responsibility | ⭐ **Yes** |
| **SaveManager Facade** | ✅ Very clear | ✅ Coordinated | ⭐ **Yes** (in addition) |

### Recommended Implementation

**Primary:**
- Create `ISaveDataAssembler` interface for Engine → SaveData
- Keep `ISaveDataProvider` for File → SaveData
- Keep `ISaveDataExporter` for SaveData → File

**Optional Enhancement:**
- Create `SaveManager` facade to coordinate all three
- Provides simple `SaveGame()` and `LoadGame()` methods
- Hides complexity from game code

**Benefits:**
- ✅ Each interface has single, clear purpose
- ✅ Testable in isolation
- ✅ Can evolve independently
- ✅ Optional high-level API via SaveManager
- ✅ Maintains architectural integrity

**Implementation Order:**
1. Implement ISaveDataAssembler + EngineSaveDataAssembler
2. Test assembler independently
3. (Optional) Implement SaveManager facade
4. Test complete save/load cycle

This architecture provides maximum flexibility while maintaining clean separation of concerns.

---

## Implementation Roadmap

### Phase 1: Core Infrastructure (Day 1)

**Deliverables:**
- [ ] Create `ISaveDataExporter` interface
- [ ] Write unit tests for interface contract
- [ ] Document interface in Doxygen format

**Files to create:**
- `src/interfaces/ISaveDataExporter.h`
- `tests/unit/interfaces/ISaveDataExporter.test.cpp` (if needed)

**Estimated time:** 2-4 hours

### Phase 2: FlatBuffers Implementation (Day 1-2)

**Deliverables:**
- [ ] Implement `FlatbuffersSaveDataExporter`
- [ ] Mirror existing `FlatbuffersSaveDataProvider` structure
- [ ] Write comprehensive unit tests
- [ ] Test roundtrip: Provider → SaveData → Exporter → Binary

**Files to create:**
- `src/data_providers/FlatbuffersSaveDataExporter.h`
- `src/data_providers/FlatbuffersSaveDataExporter.cpp`
- `tests/unit/data_providers/FlatbuffersSaveDataExporter.test.cpp`

**Estimated time:** 4-6 hours

### Phase 3: JSON Implementation (Day 2)

**Deliverables:**
- [ ] Implement `JSONSaveDataExporter`
- [ ] Handle UUID serialization
- [ ] Pretty-print option
- [ ] Write unit tests

**Files to create:**
- `src/data_providers/JSONSaveDataExporter.h`
- `src/data_providers/JSONSaveDataExporter.cpp`
- `tests/unit/data_providers/JSONSaveDataExporter.test.cpp`

**Estimated time:** 3-5 hours

### Phase 4: Integration and Testing (Day 3)

**Deliverables:**
- [ ] Integration tests with real SaveData
- [ ] Roundtrip tests (import → export → import)
- [ ] Performance benchmarks
- [ ] Update documentation

**Files to create/update:**
- `tests/integration/save_data/export_import_roundtrip.test.cpp`
- `documentation/workflows/EXPORTING_SAVE_DATA.md`
- Update `README.md` if needed

**Estimated time:** 4-6 hours

### Phase 5: Optional Enhancements (Day 3+)

**Optional deliverables:**
- [ ] XML exporter implementation
- [ ] CSV exporter (for data analysis)
- [ ] Factory/Registry for runtime format selection
- [ ] Command-line utility for format conversion

**Estimated time:** Variable based on requirements

---

## Example Use Cases

### Use Case 1: Backup Saves to JSON

**Scenario:** User wants to backup their save files in human-readable format.

```cpp
void BackupSaveToJSON(const std::string &save_file_path) {
  // Load SaveData using existing Provider
  FlatbuffersSaveDataProvider provider;
  auto load_result = provider.ProvideSaveData();
  
  if (!load_result.has_value()) {
    // Handle error
    return;
  }
  
  SaveData save_data = load_result.value();
  
  // Export to JSON
  JSONSaveDataExporter json_exporter;
  auto export_result = json_exporter.ExportSaveData(save_data);
  
  if (!export_result.has_value()) {
    // Handle error
    return;
  }
  
  // Write to backup file
  std::string backup_path = save_file_path + ".backup.json";
  std::ofstream file(backup_path, std::ios::binary);
  file.write(reinterpret_cast<const char *>(export_result.value().data()),
             export_result.value().size());
}
```

### Use Case 2: Cross-Platform Save Sharing

**Scenario:** Export saves in standardized JSON format for sharing across platforms.

```cpp
class SaveShareManager {
  const ISaveDataExporter &m_exporter;

public:
  SaveShareManager(const ISaveDataExporter &exporter) 
    : m_exporter(exporter) {}

  std::expected<std::vector<uint8_t>, FailInfo>
  PrepareForSharing(const SaveData &save_data) {
    // Export using injected exporter
    return m_exporter.ExportSaveData(save_data);
  }
};

// Usage:
JSONSaveDataExporter json_exporter;
SaveShareManager share_manager(json_exporter);

auto shareable_data = share_manager.PrepareForSharing(current_save);
// Upload to cloud, send to friend, etc.
```

### Use Case 3: Save Data Analytics

**Scenario:** Export all saves to CSV for analysis in Excel/Python.

```cpp
class SaveAnalytics {
public:
  void ExportAllSavesToCSV(const std::vector<SaveData> &all_saves,
                           const std::string &output_path) {
    CSVSaveDataExporter csv_exporter;
    
    std::ofstream csv_file(output_path);
    csv_file << "save_name,file_id,scene_count,timestamp\n";
    
    for (const auto &save_data : all_saves) {
      auto csv_result = csv_exporter.ExportSaveData(save_data);
      if (csv_result.has_value()) {
        csv_file.write(
            reinterpret_cast<const char *>(csv_result.value().data()),
            csv_result.value().size());
      }
    }
  }
};
```

### Use Case 4: Format Conversion Tool

**Scenario:** Command-line tool to convert between save formats.

```cpp
int main(int argc, char *argv[]) {
  if (argc < 4) {
    std::cout << "Usage: save_converter <input> <output_format> <output>\n";
    return 1;
  }

  std::string input_path = argv[1];
  std::string output_format = argv[2];
  std::string output_path = argv[3];

  // Load save (always from FlatBuffers currently)
  FlatbuffersSaveDataProvider provider;
  auto save_result = provider.ProvideSaveData();
  if (!save_result.has_value()) {
    std::cerr << "Failed to load save: " 
              << save_result.error().message << "\n";
    return 1;
  }

  // Select exporter based on format
  std::unique_ptr<ISaveDataExporter> exporter;
  if (output_format == "json") {
    exporter = std::make_unique<JSONSaveDataExporter>();
  } else if (output_format == "xml") {
    exporter = std::make_unique<XMLSaveDataExporter>();
  } else {
    std::cerr << "Unknown format: " << output_format << "\n";
    return 1;
  }

  // Export
  auto export_result = exporter->ExportSaveData(save_result.value());
  if (!export_result.has_value()) {
    std::cerr << "Failed to export: " 
              << export_result.error().message << "\n";
    return 1;
  }

  // Write output
  std::ofstream file(output_path, std::ios::binary);
  file.write(reinterpret_cast<const char *>(export_result.value().data()),
             export_result.value().size());

  std::cout << "Converted " << input_path << " to " << output_path << "\n";
  return 0;
}
```

### Use Case 5: Automated Testing

**Scenario:** Export known-good saves for regression testing.

```cpp
TEST_CASE("SaveData roundtrip preserves data", "[integration][save_data]") {
  // Create test SaveData
  SaveData original_save;
  original_save.meta_data.save_name = "Test Save";
  original_save.meta_data.file_id = uuids::uuid_system_generator{}();
  // ... populate rest of data

  // Export to FlatBuffers
  FlatbuffersSaveDataExporter fb_exporter;
  auto export_result = fb_exporter.ExportSaveData(original_save);
  REQUIRE(export_result.has_value());

  // Re-import
  // (Would need to write binary, then load with provider)
  // ... 

  // Verify data matches
  // REQUIRE(loaded_save.meta_data.save_name == original_save.meta_data.save_name);
}
```

---

## Testing Strategy

### Unit Tests

**Test each exporter in isolation:**

```cpp
// tests/unit/data_providers/JSONSaveDataExporter.test.cpp
TEST_CASE("JSONSaveDataExporter exports valid JSON", 
          "[unit][JSONSaveDataExporter]") {
  // Arrange
  steamrot::SaveData save_data;
  save_data.meta_data.save_name = "Test Save";
  save_data.meta_data.file_id = uuids::uuid_system_generator{}();
  
  steamrot::JSONSaveDataExporter exporter;
  
  // Act
  auto result = exporter.ExportSaveData(save_data);
  
  // Assert
  REQUIRE(result.has_value());
  
  // Parse JSON to verify validity
  std::string json_str(result.value().begin(), result.value().end());
  auto json = nlohmann::json::parse(json_str);
  
  REQUIRE(json["metadata"]["save_name"] == "Test Save");
  REQUIRE(json["metadata"]["file_id"] == uuids::to_string(save_data.meta_data.file_id));
}

TEST_CASE("JSONSaveDataExporter handles empty SaveData",
          "[unit][JSONSaveDataExporter]") {
  steamrot::SaveData empty_save;
  steamrot::JSONSaveDataExporter exporter;
  
  auto result = exporter.ExportSaveData(empty_save);
  
  REQUIRE(result.has_value());
  // Verify produces valid (though minimal) JSON
}
```

### Integration Tests

**Test roundtrip: Import → Export → Import:**

```cpp
// tests/integration/save_data/roundtrip.test.cpp
TEST_CASE("FlatBuffers roundtrip preserves data",
          "[integration][save_data][roundtrip]") {
  // Load a known save file
  steamrot::FlatbuffersSaveDataProvider provider;
  auto load1 = provider.ProvideSaveData();
  REQUIRE(load1.has_value());
  
  // Export back to FlatBuffers
  steamrot::FlatbuffersSaveDataExporter exporter;
  auto export_result = exporter.ExportSaveData(load1.value());
  REQUIRE(export_result.has_value());
  
  // Write to temp file
  std::string temp_file = "/tmp/test_save.bin";
  std::ofstream file(temp_file, std::ios::binary);
  file.write(reinterpret_cast<const char*>(export_result.value().data()),
             export_result.value().size());
  file.close();
  
  // Load again
  auto load2 = provider.ProvideSaveData();
  REQUIRE(load2.has_value());
  
  // Verify data matches
  REQUIRE(load1.value().meta_data.save_name == 
          load2.value().meta_data.save_name);
  REQUIRE(load1.value().meta_data.file_id == 
          load2.value().meta_data.file_id);
  // ... verify other fields
}
```

### Mock Exporters for Testing

**Create test doubles for dependent components:**

```cpp
// tests/mocks/MockSaveDataExporter.h
class MockSaveDataExporter : public steamrot::ISaveDataExporter {
  bool m_should_fail = false;
  
public:
  void SetShouldFail(bool should_fail) { m_should_fail = should_fail; }
  
  std::expected<std::vector<uint8_t>, steamrot::FailInfo>
  ExportSaveData(const steamrot::SaveData& save_data) const override {
    if (m_should_fail) {
      return std::unexpected(
        steamrot::FailInfo{steamrot::FailMode::Unknown, "Mock failure"}
      );
    }
    return std::vector<uint8_t>{1, 2, 3}; // Dummy data
  }
  
  std::string GetFileExtension() const override { return ".mock"; }
  std::string GetMimeType() const override { return "test/mock"; }
};
```

---

## Conclusion

### Summary of Recommendations

1. **Primary Recommendation: Abstract Exporter Interface (Approach 2)**
   - Best balance of simplicity, extensibility, and maintainability
   - Follows established SteamRot architectural patterns
   - Suitable for production use

2. **Optional Enhancement: Strategy Registry (Approach 4)**
   - Add if runtime format selection is needed
   - Useful for UI with export options
   - Can be added later without affecting core architecture

3. **Avoid: Direct Serialization (Approach 1)**
   - Only use for quick prototypes or throwaway code
   - Violates SteamRot's layered architecture

4. **Avoid: Visitor Pattern (Approach 3)**
   - Overkill for straightforward serialization
   - More complex than needed for this use case

### Implementation Priority

**Must Have (Phase 1-2):**
- ISaveDataExporter interface
- FlatbuffersSaveDataExporter (for symmetry with Provider)

**Should Have (Phase 3):**
- JSONSaveDataExporter (human-readable format)

**Nice to Have (Phase 4+):**
- XMLSaveDataExporter
- CSVSaveDataExporter (for analytics)
- SaveDataExporterFactory (if runtime selection needed)

### Architectural Benefits

By following the recommended approach, SteamRot will have:

✅ **Symmetric architecture** - Provider (import) ↔ Exporter (export)  
✅ **Clear separation of concerns** - Types, interfaces, implementations  
✅ **Extensibility** - Add formats without modifying existing code  
✅ **Testability** - Easy to mock and test each component  
✅ **Maintainability** - Single responsibility for each class  
✅ **Consistency** - Follows established patterns in codebase  

### Next Steps

1. Review this analysis with the team
2. Approve the recommended approach
3. Begin implementation following the roadmap
4. Create workflow documentation once implementation is complete
5. Consider command-line utility for save format conversion

---

## References

### Related Documentation

- **ISaveDataProvider** - `src/interfaces/ISaveDataProvider.h`
- **FlatbuffersSaveDataProvider** - `src/data_providers/FlatbuffersSaveDataProvider.h`
- **Font Provider Decoupling Analysis** - `documentation/analysis/FONT_PROVIDER_DECOUPLING.md`
- **UI Decoupling Analysis** - `documentation/analysis/USER_INTERFACE_DECOUPLING_ANALYSIS.md`

### External Resources

- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [FlatBuffers Documentation](https://google.github.io/flatbuffers/)
- [nlohmann/json Documentation](https://github.com/nlohmann/json)
- [Dependency Inversion Principle](https://en.wikipedia.org/wiki/Dependency_inversion_principle)

---

**Document Version:** 1.0  
**Date:** 2026-01-06  
**Author:** GitHub Copilot Agent  
**Status:** Proposal for Review
