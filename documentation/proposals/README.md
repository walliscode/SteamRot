# Proposals

This directory contains design proposals for new features or significant changes to the SteamRot engine.

## Active Proposals

### [Entity View Pattern Implementation](ENTITY_VIEW_PATTERN_IMPLEMENTATION.md)

**Status**: Proposal - Planning Phase  
**Date**: 2025-12-05  
**Type**: Architecture Enhancement  
**Priority**: MEDIUM

**Summary**: Completes the data loading provider pattern migration by implementing a view/iterator pattern for entity/component configuration. Eliminates the remaining FlatBuffers coupling in `FlatbuffersConfigurator` by introducing format-independent view interfaces for navigating nested entity data.

**Key Points**:
1. Zero-copy navigation of nested entity/component data
2. Format-independent interfaces (`IEntityCollectionView`, `IEntityView`, `IComponentView`)
3. Complete decoupling from FlatBuffers (final piece of provider pattern)
4. Mock providers for testing without file dependencies
5. Handles complex polymorphic UIElement hierarchies

**Implementation Complexity**: Medium (9 weeks)
- Phase 1-2: Foundation and UIElement views (4 weeks, Medium Risk)
- Phase 3-4: Remaining components and new configurator (3 weeks, Medium Risk)
- Phase 5-6: Migration and mock provider (2 weeks, Low Risk)

**Benefits**:
- ✅ Completes architectural consistency (6/6 systems using provider pattern)
- ✅ Superior testing (no file dependencies, easy mocking)
- ✅ Zero-copy performance for nested data
- ✅ Format flexibility (ready for JSON/XML/database providers)

**Documents**:
- [Implementation Roadmap](ENTITY_VIEW_PATTERN_ROADMAP.md) - **START HERE** - Executive summary and timeline
- [Full Implementation Plan](ENTITY_VIEW_PATTERN_IMPLEMENTATION.md) - Complete design and phasing
- [Quick Reference](ENTITY_VIEW_PATTERN_QUICK_REF.md) - Patterns and examples
- [Migration Guide](ENTITY_VIEW_PATTERN_MIGRATION_GUIDE.md) - How to migrate existing code

**Related Analysis**:
- [Current State Analysis](../analysis/CURRENT_STATE_ANALYSIS_2025.md) - Context and motivation
- [View Pattern Diagrams](../analysis/VIEW_PATTERN_DIAGRAMS_AND_EXAMPLES.md) - Pattern examples
- [Configuration vs Data Structs](../analysis/CONFIGURATION_VS_DATA_STRUCTS_ANALYSIS.md) - Design rationale

---

### [Data Loading Interface System](DATA_LOADING_INTERFACE_EXECUTIVE_SUMMARY.md)

**Status**: Proposal  
**Date**: 2024-12-04  
**Type**: Architecture Enhancement

**Summary**: Proposes implementing an interface-based data loading system to abstract serialization formats (FlatBuffers, JSON, XML, Lua) from game code. Game code would work exclusively with native C++ structs, while providers handle format-specific loading.

**Key Points**:
1. Pattern already proven with `ISaveDataProvider` and `IUserPreferencesProvider`
2. Native structs for game code (e.g., `EngineData`, `SceneData`)
3. Format-agnostic interfaces (e.g., `IEngineDataProvider`)
4. Format-specific implementations (e.g., `FlatbuffersEngineDataProvider`, `JsonSceneDataProvider`)
5. Three-layer architecture: Game Code → Interfaces → Implementations

**Implementation Complexity**: Medium
- Phase 1: Create interfaces (1-2 weeks, Low Risk)
- Phase 2: Migrate integration points (2-3 weeks, Moderate Risk)
- Phase 3: Cleanup (1 week, Low Risk)
- Phase 4: Entity system (4-6 weeks, High Risk - DEFERRED)

**Documents**:
- [Executive Summary](DATA_LOADING_INTERFACE_EXECUTIVE_SUMMARY.md) - Start here!
- [Full Analysis](DATA_LOADING_INTERFACE_SYSTEM.md) - Complete 40+ page analysis
- [Quick Reference](DATA_LOADING_INTERFACE_QUICK_REF.md) - Patterns and templates
- [Architecture Diagrams](DATA_LOADING_INTERFACE_DIAGRAMS.md) - Visual representations

---

### [EntityMemoryPool Copyability](ENTITY_MEMORY_POOL_COPYABILITY.md)

**Status**: Proposal  
**Date**: 2025-11-30  
**Type**: Design Decision

**Summary**: Addresses the non-copyability of EntityMemoryPool due to `std::unique_ptr` members in CUserInterface and CGrimoireMachina. Proposes implementing the Clone method pattern to enable deep copying of the polymorphic UIElement hierarchy.

**Key Points**:
1. UIElement and derived types need Clone() method for polymorphic copying
2. CUserInterface and CGrimoireMachina need copy constructor/assignment operators
3. Enables tick-by-tick comparison testing
4. Prepares codebase for future state snapshot features

**Implementation Complexity**: Low to Medium
- Add Clone() to 7 UIElement types
- Add copy semantics to 2 components
- ~6 hours estimated effort

---

### [Unified Tick Architecture](UNIFIED_TICK_ARCHITECTURE.md)

**Status**: Proposal  
**Date**: 2025-11-27  
**Type**: Architecture Unification

**Summary**: Addresses the architectural divergence between the game engine's main loop and the test harness execution model. Creates a unified tick execution abstraction that both systems can use.

**Key Points**:
1. Common derivative for test harness and game engine loop ordering
2. Logic provider interface for injectable logic configuration
3. Unified data loading patterns for game and tests
4. Clarification of expected_data_collection vs tick_snapshots
5. Rename TestContext to reduce confusion with GameContext/SceneContext

**Implementation Complexity**: Medium
- Creates new abstractions (TickExecutor, ILogicProvider, IEntityDataSource)
- Refactors game engine and test harness to use common base
- Incremental migration path provided

---

### [Tick-by-Tick Entity State Comparison](TICK_BY_TICK_COMPARISON_DESIGN.md)

**Status**: Proposal  
**Date**: 2025-11-12  
**Type**: Test Infrastructure Enhancement

**Summary**: Add the ability to compare entity states on a tick-by-tick basis within the test harness, allowing verification of intermediate states during multi-step simulations.

**Key Features**:
- Optional `tick_snapshots` field in test data
- Pause and compare at specific ticks
- Reuses existing EntityMemoryPool comparison infrastructure
- 100% backward compatible

**Use Cases**:
- Multi-step UI state changes
- Event-driven state machines
- Accumulating values over time
- Complex transformation sequences

**Implementation Complexity**: Low to Medium
- Schema changes: Simple (add 2 tables)
- Code changes: Minimal (1 new function, 1 line added)
- Documentation: Medium (3 files to update)

## Proposal Process

1. **Creation**: Proposal documents should be placed in this directory
2. **Review**: Team reviews the proposal and provides feedback
3. **Decision**: Proposal is accepted, modified, or rejected
4. **Implementation**: If accepted, implementation proceeds per the plan
5. **Archival**: Completed proposals move to `archive/` subdirectory

## Template

When creating a new proposal, include:

1. **Executive Summary** - High-level overview
2. **Background** - Current state and problem statement
3. **Proposed Solution** - Detailed design
4. **Implementation Plan** - Step-by-step approach
5. **Benefits** - Why this is valuable
6. **Risks** - What could go wrong
7. **Alternatives** - Other approaches considered
8. **Future Work** - What's out of scope

## Status Definitions

- **Proposal**: Under review, not yet approved
- **Approved**: Accepted, ready for implementation
- **In Progress**: Currently being implemented
- **Completed**: Implementation finished, moved to archive
- **Rejected**: Not approved, documented for reference
- **Deferred**: On hold, may be revisited later
