# Proposals

This directory contains design proposals for new features or significant changes to the SteamRot engine.

## Active Proposals

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
