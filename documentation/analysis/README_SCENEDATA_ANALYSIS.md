# SceneData Architecture Documentation - Overview

## Purpose

This directory contains comprehensive analysis and recommendations for the SceneData architecture, specifically addressing concerns about using derived SceneData instances to pass heavy data structures (like entity data pointers) and the asymmetric nature of the current import-only design.

## Problem Statement

> "I'm still not entirely convinced by using derived instances of the SceneData to pass heavy structs around (such as pointers to entity data). This also becomes problematic in reverse on data exporting. Do we have a NativeSceneData struct that contains a ref/pointer to the EntityMemoryPool then for exporting?"

## Documents in This Analysis

### 1. [SCENEDATA_ARCHITECTURE_ANALYSIS.md](./SCENEDATA_ARCHITECTURE_ANALYSIS.md)

**Comprehensive architectural analysis covering**:
- Current SceneData and FbsSceneData implementation
- Detailed data flow for import operations
- The export direction problem and architectural asymmetry
- Analysis of advantages and disadvantages
- Four alternative architectures with trade-offs
- Short, medium, and long-term recommendations

**Read this first** for a complete understanding of the issue.

### 2. [SCENEDATA_CODE_EXAMPLES.md](./SCENEDATA_CODE_EXAMPLES.md)

**Detailed code examples showing**:
- Current implementation with actual code snippets
- Issues illustrated with code (downcasting, const-casting)
- Proposed Solution 1: NativeSceneData (minimal change)
  - Complete implementation with header and source files
  - IEntityExporter interface design
  - FlatbuffersEntityExporter implementation
  - Usage examples for save/load
- Proposed Solution 2: Importer/Exporter hierarchies (clean architecture)
  - Refactored interfaces without inheritance for data passing
  - Updated configurator code
  - Benefits comparison table
- Testing considerations and examples

**Read this second** for concrete implementation details.

### 3. [SCENEDATA_VISUAL_DIAGRAMS.md](./SCENEDATA_VISUAL_DIAGRAMS.md)

**Visual ASCII diagrams showing**:
- Current import data flow
- Export direction problem visualization
- Architecture comparison diagrams
- Type hierarchy comparisons
- Interface design comparisons
- Summary diagram of architectural choices

**Read this third** for visual understanding.

### 4. [ADR_SCENEDATA_IMPORT_EXPORT.md](./ADR_SCENEDATA_IMPORT_EXPORT.md)

**Architecture Decision Record containing**:
- Formal decision documentation format
- Status: PROPOSED (awaiting team review)
- Decision drivers and constraints
- Four options evaluated with pros/cons
- **RECOMMENDED**: Two-phase approach
  - Phase 1: Add NativeSceneData (immediate, minimal change)
  - Phase 2: Refactor to Importer/Exporter pattern (future, clean architecture)
- Implementation guidelines for each phase
- Consequences analysis (positive, negative, neutral)
- Appendix explaining the "code smell" concern

**Read this fourth** for the formal decision recommendation.

### 5. [IMPORTER_EXPORTER_IMPLEMENTATION_DETAILS.md](./IMPORTER_EXPORTER_IMPLEMENTATION_DETAILS.md)

**Detailed Q&A addressing specific implementation questions**:
- Would IEntityImporter/Exporter replace the Configurator?
- How would EntityCollectionFbs data move through the system?
- How do we pass the data to the importer in SceneFactory?
- Complete code examples showing data flow
- Two implementation options compared (Provider vs Factory creates Importer)
- Migration strategy from current to proposed pattern

**Read this for practical implementation guidance** when ready to implement the Importer/Exporter pattern.

### 6. [CLARIFICATION_IMPORTER_VS_CONFIGURATOR.md](./CLARIFICATION_IMPORTER_VS_CONFIGURATOR.md)

**Clarification of key concepts**:
- Is IEntityImporter like a DTO with methods? (Yes - smart wrapper)
- How does Provider wrap data in the Importer? (Complete examples)
- Where is IEntityConfigurator called? (Inside IEntityImporter - uses it internally)
- Relationship between IEntityImporter (facade) and IEntityConfigurator (implementation)
- Complete call chain from Provider to EntityMemoryPool modification
- Why both interfaces exist and their distinct responsibilities

**Read this for conceptual clarity** on how Importer and Configurator work together.

## Quick Summary

### The Current Problem

**Import** (Loading):
```
FlatBuffers File → FbsSceneData → EntityMemoryPool (runtime)
                    ↑ Contains pointer to EntityCollectionFbs
```

**Export** (Saving): 
```
EntityMemoryPool (runtime) → ??? → FlatBuffers File
                              ↑ No clear pattern
```

### The Code Smell

Using inheritance (`FbsSceneData extends SceneData`) to pass pointers to heavy data structures violates the principle of "inheritance for is-a relationships." It creates:
- Required downcasting with `dynamic_cast`
- Required const-casting (unsafe)
- Asymmetric architecture (import works, export unclear)
- Tight coupling between configurators and derived types

### Recommended Solution

**Two-Phase Approach**:

**Phase 1 (Immediate)**: Add `NativeSceneData` for symmetry
- Minimal code changes (2-3 days)
- Enables export/save functionality
- Documents pattern clearly
- Unblocks current work

**Phase 2 (Future)**: Refactor to Importer/Exporter pattern
- Clean architecture (1-2 weeks)
- No downcasting required
- Separates data from operations
- Extensible for future needs

## For Developers

### If You're Working on Import/Loading

**Current Pattern**:
1. `FlatbuffersSceneDataProvider` creates `FbsSceneData`
2. Attaches pointer to `EntityCollectionFbs` 
3. `FlatbuffersSceneConfigurator` downcasts to access it
4. Creates `FlatbuffersEntityConfigurator` with entity data

**Key Files**:
- `src/types/core/SceneData.h` - Base structure
- `src/types/core/FbsSceneData.h` - Import-specific derived type
- `src/scenes/FlatbuffersSceneConfigurator.cpp` - Uses downcasting

### If You're Working on Export/Saving

**Recommended Pattern** (after Phase 1 implementation):
1. Create `NativeSceneData` with `entity_memory_pool` pointer
2. Pass to `IEntityExporter` implementation
3. `FlatbuffersEntityExporter` converts to FlatBuffers binary
4. Write binary to file

**Key Files** (to be created):
- `src/types/core/NativeSceneData.h` - Export-specific derived type
- `src/interfaces/IEntityExporter.h` - Export interface
- `src/entity/FlatbuffersEntityExporter.h/.cpp` - Implementation

### If You're Planning Major Refactoring

Consider **Phase 2** (Importer/Exporter pattern):
- Remove `FbsSceneData` and `NativeSceneData`
- Keep `SceneData` lightweight (no heavy data pointers)
- Create `IEntityImporter` and `IEntityExporter` interfaces
- Pass heavy data directly to importer/exporter constructors
- No downcasting needed in configurators

See [SCENEDATA_CODE_EXAMPLES.md](./SCENEDATA_CODE_EXAMPLES.md) Section "Proposed Solution 2" for full implementation.

## Key Insights

1. **Current architecture works for import** but has no clear export pattern
2. **Using inheritance to pass pointers is a code smell** (composition would be better)
3. **Symmetry is desirable** - import and export should follow similar patterns
4. **Trade-off exists** between quick fix (NativeSceneData) and clean architecture (Importer/Exporter)
5. **Two-phase approach balances** immediate needs with long-term architecture quality

## Decision Status

**PROPOSED** - Awaiting team review and approval

### Next Steps

1. **Team Review**: Discuss analysis and recommendations
2. **Decision**: Choose between options or modify recommendation
3. **Implementation**: If approved, implement Phase 1
4. **Documentation**: Update this analysis based on actual implementation
5. **Future Planning**: Schedule Phase 2 when appropriate

## Questions for Discussion

1. **Urgency**: How soon do we need export/save functionality?
2. **Resources**: Can we afford Phase 2 refactoring effort (1-2 weeks)?
3. **Risk Tolerance**: Comfortable with Phase 1 perpetuating code smell temporarily?
4. **Alternative Formats**: Do we plan to support non-FlatBuffers formats (JSON, etc.)?
5. **Migration Strategy**: If doing Phase 2, incremental or all-at-once?

## Related Documentation

- `.github/copilot-instructions.md` - Overall architecture guidelines
- `README.md` - Project structure and build instructions
- `documentation/workflows/` - Component and Logic workflows
- `src/interfaces/ISceneConfigurator.h` - Current configurator interface
- `src/entity/FlatbuffersEntityConfigurator.h` - Current entity configuration

## Feedback and Updates

This analysis is a living document. After team review:

- [ ] Update with team decision
- [ ] Document chosen approach in ADR
- [ ] Create implementation tickets if needed
- [ ] Update related documentation
- [ ] Track migration progress (if Phase 2)

---

**Analysis Date**: 2026-01-06  
**Status**: Analysis Complete, Awaiting Review  
**Version**: 1.0  
**Primary Author**: Architecture Analysis (Copilot Agent)  
**Reviewers**: TBD

## Document Change History

| Date | Version | Changes | Author |
|------|---------|---------|--------|
| 2026-01-06 | 1.0 | Initial analysis and documentation | Copilot Agent |

---

## Quick Reference: Files Location

```
documentation/analysis/
├── README_SCENEDATA_ANALYSIS.md                    (this file)
├── SCENEDATA_ARCHITECTURE_ANALYSIS.md              (comprehensive analysis)
├── SCENEDATA_CODE_EXAMPLES.md                      (implementation examples)
├── SCENEDATA_VISUAL_DIAGRAMS.md                    (ASCII flowcharts)
├── ADR_SCENEDATA_IMPORT_EXPORT.md                  (decision record)
├── IMPORTER_EXPORTER_IMPLEMENTATION_DETAILS.md     (Q&A and implementation guide)
└── CLARIFICATION_IMPORTER_VS_CONFIGURATOR.md       (concepts and relationships)
```

## Questions or Need Clarification?

Refer to:
1. **High-level understanding**: Start with this README
2. **Detailed analysis**: Read SCENEDATA_ARCHITECTURE_ANALYSIS.md
3. **Implementation details**: Check SCENEDATA_CODE_EXAMPLES.md
4. **Visual diagrams**: Review SCENEDATA_VISUAL_DIAGRAMS.md
5. **Decision rationale**: Review ADR_SCENEDATA_IMPORT_EXPORT.md
6. **Practical Q&A**: Check IMPORTER_EXPORTER_IMPLEMENTATION_DETAILS.md
7. **Conceptual clarity**: Read CLARIFICATION_IMPORTER_VS_CONFIGURATOR.md

For further questions, contact the architecture team or review discussion in related issues/PRs.
