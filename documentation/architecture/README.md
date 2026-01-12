# Architecture Documentation

This directory contains architecture documentation for the SteamRot game engine.

## Native Bridging Structures

Documentation for making SaveData, EngineSnapshot, and TestData work as bidirectional bridging structures between external formats and the game engine.

### Documents

1. **[QUICK_REFERENCE.md](QUICK_REFERENCE.md)** - Start here for a quick overview
   - TL;DR summary
   - Class signatures to implement
   - Common usage patterns
   - Implementation order

2. **[SOLUTION_SUMMARY.md](SOLUTION_SUMMARY.md)** - Detailed explanation
   - Problem analysis
   - Complete solution design
   - Workflow examples
   - Why this approach is best
   - Implementation checklist

3. **[NATIVE_BRIDGING_STRUCTURES.md](NATIVE_BRIDGING_STRUCTURES.md)** - Full architecture
   - Current system analysis
   - Three-tier architecture design
   - Detailed implementation specs
   - Use cases with code examples
   - Design principles
   - Alternative approaches comparison

### Reading Order

**For quick understanding:**
1. Read QUICK_REFERENCE.md (5 min)
2. Skim SOLUTION_SUMMARY.md (10 min)

**For implementation:**
1. Read SOLUTION_SUMMARY.md thoroughly (20 min)
2. Reference NATIVE_BRIDGING_STRUCTURES.md for details (30 min)
3. Keep QUICK_REFERENCE.md handy while coding

**For architecture review:**
1. Read NATIVE_BRIDGING_STRUCTURES.md completely (30 min)
2. Review alternative approaches section
3. Consider design principles

## Key Concepts

### Problem
Current system uses format-specific interfaces (FlatBuffers) with one-way data flow. Need bidirectional flow for:
- Testing (create native data → import to engine)
- Serialization (export engine state → native structs → files)
- Debugging (snapshot state → compare → restore)

### Solution
**Dual-Mode Interface Pattern**: Keep IEntityImporter/IEntityExporter separate, add native implementations.

```
External Formats (FlatBuffers/JSON)
    ↕ Format Importers/Exporters
Native Structs (SaveData/EngineSnapshot)
    ↕ Native Importers/Exporters
Runtime Engine (EntityMemoryPool)
```

### Key Classes

- **NativeEntityImporter**: Import from native EntityMemoryPool → runtime EMP
- **NativeEntityExporter**: Export from runtime EMP → native EntityMemoryPool (configurator pattern)
- **SceneData**: Enhanced with `entity_source` variant (importer/exporter/native)
- **Snapshot utilities**: Helper functions for capture/restore/diff

## Implementation Status

- [x] Architecture designed and documented
- [x] Solution approach selected and justified
- [x] Documentation complete with examples
- [x] Updated to use variant pattern
- [x] Updated exporter to use configurator pattern
- [ ] NativeEntityImporter implementation
- [ ] NativeEntityExporter implementation
- [ ] SceneData variant enhancement
- [ ] Snapshot utilities
- [ ] Tests
- [ ] Integration

## Questions?

Refer to the **"Questions?"** section in SOLUTION_SUMMARY.md for common questions and answers.

## Related Documentation

- Main README.md in repository root
- GitHub Copilot Instructions (.github/copilot-instructions.md)
- Test data configuration documentation
