# Proposals

This directory contains design proposals for new features or significant changes to the SteamRot engine.

## Active Proposals

### [Engine Data Organization](ENGINE_DATA_ORGANIZATION.md)

**Status**: Proposal  
**Date**: 2025-12-06  
**Type**: Architecture - Data Organization

**Summary**: Proposes a clear, intuitive organization strategy for data and resources in the Engine class. Addresses the current lack of clear categorization between global resources, configuration data, and engine-specific runtime state.

**Key Points**:
1. Three distinct categories: Core Resources, Configuration, Engine State
2. Clear naming conventions that indicate category and purpose
3. Decision tree for categorizing new data
4. Gradual migration path without breaking changes
5. Future-proof pattern that scales with engine growth

**Implementation Complexity**: Low to Medium
- Non-breaking addition of new structs alongside existing members
- Gradual migration member by member
- Compatibility accessors during transition
- Clear patterns for future extensions

**Benefits**:
- Intuitive understanding of data ownership and lifetime
- Easy to determine where new data should go
- Better testability through category-based mocking
- Improved maintainability and code organization

**Related**: [ENGINE_ARCHITECTURE_IMPROVEMENTS.md](ENGINE_ARCHITECTURE_IMPROVEMENTS.md)

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
