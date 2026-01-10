# EngineSnapshot Integration Analysis

> **Status**: Analysis Complete ✅ | **Type**: Documentation Only | **Date**: 2026-01-10

## Overview

This directory contains comprehensive analysis and design documentation for integrating `EngineSnapshot` as the primary container for engine initialization and state export in the SteamRot game engine.

## Problem Statement

Make `EngineSnapshot` serve dual purposes:
1. **Input**: Container to configure and initialize GameEngine/TestEngine
2. **Output**: Container to export engine state for saving, testing, and future network sync

## Documents in This Analysis

### 📘 Main Analysis Document
**[ENGINE_SNAPSHOT_INTEGRATION.md](ENGINE_SNAPSHOT_INTEGRATION.md)** (27KB)

Comprehensive analysis including:
- Current state analysis (EngineSnapshot, Engine initialization, SaveData)
- Requirements analysis (functional and non-functional)
- Design options comparison (3 options with detailed pros/cons)
- **Recommended solution**: Provider/Consumer pattern
- Detailed interface designs with complete code examples
- 6 critical issues identified with concrete solutions
- Phased implementation plan (5 phases, 3 weeks)
- Testing strategy (unit, integration, system tests)
- Migration guide for existing code

**Read this if:** You need complete details, rationale, and implementation guidance.

### 📗 Quick Reference
**[ENGINE_SNAPSHOT_INTEGRATION_QUICK_REF.md](ENGINE_SNAPSHOT_INTEGRATION_QUICK_REF.md)** (11KB)

Condensed reference including:
- TL;DR architecture summary
- Interface signatures and use cases
- Key design decisions explained
- Critical issues and solutions (condensed)
- Code examples (4 practical scenarios)
- Benefits/drawbacks summary
- When to use each interface

**Read this if:** You need a quick overview or lookup reference.

### 📊 Architecture Diagrams
**[ENGINE_SNAPSHOT_INTEGRATION_DIAGRAMS.md](ENGINE_SNAPSHOT_INTEGRATION_DIAGRAMS.md)** (22KB)

Visual representations including:
- High-level architecture diagram
- Interface structure
- Data flow comparisons (current vs proposed)
- Snapshot capture/consumption flows
- TestEngine integration
- Save/Load integration
- Class hierarchy
- Conversion utilities

**Read this if:** You prefer visual learning or need diagrams for presentations.

## Executive Summary

### Recommended Solution: Provider/Consumer Pattern

Two new interfaces following existing codebase patterns:

```cpp
class IEngineSnapshotProvider {
  // Export engine state to snapshot
  virtual std::expected<EngineSnapshot, FailInfo>
  CaptureSnapshot() const = 0;
  
  virtual std::expected<EngineSnapshot, FailInfo>
  CapturePartialSnapshot(SnapshotFields fields) const = 0;
};

class IEngineSnapshotConsumer {
  // Import snapshot to configure engine
  virtual std::expected<std::monostate, FailInfo>
  ConsumeSnapshot(const EngineSnapshot& snapshot) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ApplySnapshotUpdate(const EngineSnapshot& snapshot,
                      SnapshotMode mode) = 0;
};

class Engine : public IEngineSnapshotProvider, 
               public IEngineSnapshotConsumer {
  // Implements both interfaces
};
```

### Why This Approach?

| Criterion | Evaluation |
|-----------|------------|
| **Consistency** | ✅ Matches existing patterns (IEngineDataProvider, IEntityImporter/Exporter) |
| **Separation of Concerns** | ✅ Read and write operations clearly separated |
| **Testability** | ✅ Easy to mock and test in isolation |
| **Polymorphism** | ✅ Supports different engine implementations |
| **Extensibility** | ✅ Easy to add new snapshot sources/destinations |
| **Backward Compatibility** | ✅ Existing code continues to work unchanged |

### Extended EngineSnapshot

All fields optional for flexibility:

```cpp
struct EngineSnapshot {
  // Existing (TestEngine usage)
  std::optional<size_t> tick_number;
  SceneCollectionData scene_collection_data;
  std::optional<EventBus> global_event_bus;
  
  // New (complete state capture)
  std::optional<EngineConfig> engine_config;
  std::optional<EngineState> engine_state;
  std::optional<EngineResourcesConfig> engine_resources_config;
  std::optional<AssetConfig> loaded_assets;
  std::optional<SceneManagerData> scene_manager_data;
  
  // Validation helpers
  bool HasMinimalData() const;   // For testing
  bool HasCompleteData() const;  // For saving
};
```

### Key Benefits

1. **Unified Data Model**: Single container for all engine state (no EngineData vs EngineSnapshot confusion)
2. **Extensibility**: Add fields without breaking changes (all optional)
3. **Testing**: Easy to create partial snapshots for focused tests
4. **Save/Load**: Direct path from engine state to save file via snapshot
5. **Debugging**: Capture snapshots for issue reproduction
6. **Future-Proof**: Foundation for network sync, replay systems

### Critical Issues Addressed

| Issue | Solution |
|-------|----------|
| Two-Phase Initialization | Common initialization helper used by both paths |
| Partial vs Complete Snapshots | Validation methods + clear documentation per use case |
| SceneManager State Sync | Export/Import methods on SceneManager |
| Non-Serializable Resources | Store config, recreate resources on restore |
| Backward Compatibility | Keep existing patterns, convert internally |
| Performance Impact | Partial snapshots, move semantics, profiling |

## Implementation Roadmap (If Proceeding)

### Phase 1: Foundation (Week 1)
- Create interface definitions (`IEngineSnapshotProvider`, `IEngineSnapshotConsumer`)
- Extend `EngineSnapshot` with new optional fields
- Add enums (`SnapshotFields`, `SnapshotMode`)
- Write unit tests for interfaces

### Phase 2: Engine Integration (Weeks 1-2)
- Implement interfaces in `Engine` base class
- Add snapshot-based constructor
- Refactor `StartUp()` to use snapshots internally
- Update `TestEngine` to use new interfaces
- Write integration tests

### Phase 3: Save/Load Integration (Week 2)
- Create `EngineSnapshotFbs` FlatBuffers schema
- Implement conversion utilities (EngineSnapshot ↔ SaveData)
- Update data providers to use snapshots
- Write serialization tests

### Phase 4: SceneManager Integration (Weeks 2-3)
- Add `ExportData()` method to SceneManager
- Add `ImportData()` method to SceneManager
- Integrate with Engine snapshot capture
- Write SceneManager snapshot tests

### Phase 5: Documentation and Migration (Week 3)
- Write migration guide from EngineData to EngineSnapshot
- Create practical code examples
- Update workflow documentation
- Add deprecation notices for old patterns

## Quick Example Use Cases

### 1. Initialize Engine from Snapshot
```cpp
EngineSnapshot snapshot = LoadFromFile("save_001.bin");
Engine engine(snapshot);
engine.RunGameLoop();
```

### 2. Save Current Engine State
```cpp
auto snapshot = engine.CaptureSnapshot().value();
SaveData save = ConvertSnapshotToSaveData(snapshot, meta);
SerializeToFile(save, "save_001.bin");
```

### 3. Partial Snapshot for Testing
```cpp
auto snapshot = engine.CapturePartialSnapshot(
  SnapshotFields::SceneData | SnapshotFields::TickNumber);
// Only captures needed fields - faster, clearer intent
```

### 4. Hot-Restore from Checkpoint
```cpp
EngineSnapshot checkpoint = LoadCheckpoint();
engine.ApplySnapshotUpdate(checkpoint, SnapshotMode::Merge);
```

## Migration Path

### Existing Code (No Changes Required)
```cpp
Engine engine;
engine.RunGame();  // Still works, uses snapshots internally
```

### New Code (Optional Adoption)
```cpp
EngineSnapshot snapshot = CreateOrLoadSnapshot();
Engine engine(snapshot);  // Direct snapshot initialization
engine.RunGameLoop();
```

## Document Reading Guide

### For Quick Understanding
1. Read **Quick Reference** (10 minutes)
2. Review **Diagrams** for visual overview (5 minutes)

### For Implementation Planning
1. Read **Main Analysis** thoroughly (45 minutes)
2. Reference **Quick Reference** during implementation
3. Use **Diagrams** for team discussions

### For Architecture Review
1. Review **Diagrams** first for visual context (10 minutes)
2. Read **Main Analysis** design section (20 minutes)
3. Read issues and solutions section (15 minutes)

## Status

**Analysis**: ✅ Complete  
**Design**: ✅ Complete  
**Documentation**: ✅ Complete  
**Implementation**: ⏸️ Not Started (analysis only, as requested)

## Next Steps

If proceeding with implementation:
1. Review all three documents with the team
2. Get approval on the recommended Provider/Consumer pattern
3. Start with Phase 1 (Foundation) - interfaces and extended snapshot
4. Follow the phased implementation plan
5. Use the testing strategy to ensure quality at each phase

## Related Documentation

- [Engine Class Documentation](../../src/engine/Engine.h)
- [EngineSnapshot Structure](../../src/types/core/EngineSnapshot.h)
- [TestEngine Implementation](../../tests/harness/TestEngine.h)
- [SaveData Structure](../../src/types/core/SaveData.h)

## Author Notes

This analysis was created as a **documentation-only** deliverable as requested in the problem statement. No implementation code changes were made. The documentation provides complete guidance for implementation if the team decides to proceed with this design.

The analysis:
- Evaluates multiple design options objectively
- Recommends the best approach with clear justification
- Identifies potential problems proactively
- Provides concrete solutions for each problem
- Includes complete implementation roadmap
- Ensures backward compatibility
- Maintains consistency with existing architecture

---

**Questions or feedback?** Please review the detailed analysis documents and reach out to discuss specific aspects.
