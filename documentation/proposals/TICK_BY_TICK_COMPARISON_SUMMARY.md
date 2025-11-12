# Tick-by-Tick Entity State Comparison - Summary

## Overview

This proposal adds the ability to verify entity states at intermediate ticks during multi-step test simulations. Instead of only comparing start and end states, test data can now specify "snapshots" - expected entity states at specific ticks.

## The Problem

**Current limitation**: Tests can only verify entity state at two points:
1. Start (before any ticks execute)
2. End (after all ticks complete)

**Impact**: 
- Debugging failures in multi-tick tests is difficult
- Can't isolate which tick caused a problem
- Complex transformations are hard to validate incrementally

## The Solution

Add **tick snapshots** - optional checkpoints that specify expected entity states at specific ticks. During test execution, the harness automatically compares actual state with expected state when a snapshot exists for that tick.

### Key Features

✅ **Optional** - Backward compatible, existing tests unchanged  
✅ **Data-Driven** - Snapshots defined in JSON test data  
✅ **Precise** - Identify exact tick where failure occurs  
✅ **Flexible** - Verify every tick or just key points  
✅ **Minimal** - One new function, one line added to existing code  

## Example

### Before (Current System)

```json
{
  "num_ticks": 5,
  "start_entity_collection": {"entities": [...]},
  "simulation_data": {...},
  "expected_entity_collection": {"entities": [...]}
}
```

**Limitation**: Only start and end verified

### After (With Snapshots)

```json
{
  "num_ticks": 5,
  "start_entity_collection": {"entities": [...]},
  "tick_snapshots": [
    {
      "tick": 1,
      "description": "After first action",
      "entity_collection": {"entities": [...]}
    },
    {
      "tick": 3,
      "description": "After critical operation",
      "entity_collection": {"entities": [...]}
    }
  ],
  "simulation_data": {...},
  "expected_entity_collection": {"entities": [...]}
}
```

**Benefit**: Start, ticks 1, 3, and end all verified!

## Implementation

### Schema Changes

Add two items to `test_data.fbs`:

```fbs
// New table
table TickSnapshot {
  tick: uint32;
  entity_collection: EntityCollection (required);
  description: string;
}

// Add to TestDataConfig
table TestDataConfig {
  // ... existing fields ...
  tick_snapshots: [TickSnapshot];  // NEW
}
```

### Code Changes

Add one new function in `tick_executor.cpp`:
```cpp
std::expected<std::monostate, FailInfo>
compare_tick_snapshot(uint32_t tick, const TestDataConfig *config,
                     const EntityMemoryPool &actual_pool);
```

Modify `execute_single_tick()` - add one line:
```cpp
// After simulation, before event bus tick:
auto result = compare_tick_snapshot(tick, config, actual_pool);
```

**Total changes**: ~50 lines of code

## Benefits

1. **Precise Failure Location** - Know exactly which tick failed
2. **Faster Debugging** - No manual instrumentation needed
3. **Incremental Verification** - Build confidence step by step
4. **Better Coverage** - Verify intermediate states
5. **Documentation** - Snapshots document expected behavior

## Use Cases

### State Machine Testing
```
Tick 0: IDLE → Verify
Tick 2: PROCESSING → Verify
Tick 5: COMPLETE → Verify
```

### Toggle Testing
```
Tick 0: Visible → Verify
Tick 1: Invisible → Verify
Tick 2: Visible → Verify
```

### Accumulation Testing
```
Tick 0: counter = 0 → Verify
Tick 1: counter = 1 → Verify
Tick 2: counter = 3 → Verify
```

## Documentation Included

This proposal includes comprehensive documentation:

1. **Full Design** (22KB)
   - Complete specification
   - Implementation details
   - Edge cases and error handling
   - Performance analysis
   - Testing strategy

2. **Visual Examples** (14KB)
   - Execution flow diagrams
   - Timeline visualizations
   - Before/after comparisons
   - Use case examples

3. **Quick Reference** (7KB)
   - Syntax cheat sheet
   - Common patterns
   - Decision tree
   - FAQ

4. **This Summary** (3KB)
   - High-level overview
   - Key points
   - Status and next steps

**Total documentation**: ~46KB across 4 files

## Current Status

**Phase**: Proposal (awaiting review)

**What's Done**:
- [x] Complete design specification
- [x] Visual examples and diagrams
- [x] Quick reference guide
- [x] Implementation plan
- [x] Example test data structures
- [x] Performance analysis
- [x] Edge case documentation

**What's Next** (pending approval):
- [ ] Schema changes
- [ ] Implementation
- [ ] Unit tests
- [ ] Integration tests
- [ ] Update user documentation
- [ ] Create example test data files

## Request for Feedback

Please review and provide feedback on:

1. **Overall Approach** - Is this the right solution?
2. **Schema Design** - Is the TickSnapshot table well-designed?
3. **Execution Timing** - Is comparing after simulation, before event bus tick correct?
4. **Edge Cases** - Are all edge cases covered?
5. **Performance** - Any performance concerns?
6. **API Design** - Is the interface intuitive?
7. **Documentation** - Is anything unclear or missing?

## Files in This Proposal

```
documentation/proposals/
├── README.md                                    # Proposals index
├── TICK_BY_TICK_COMPARISON_DESIGN.md           # Full design (22KB)
├── TICK_BY_TICK_COMPARISON_VISUALS.md          # Visual examples (14KB)
├── TICK_BY_TICK_COMPARISON_QUICK_REF.md        # Quick reference (7KB)
└── TICK_BY_TICK_COMPARISON_SUMMARY.md          # This file (3KB)
```

## Next Steps

1. **Review Period** - Team reviews proposal
2. **Feedback Incorporation** - Address any concerns
3. **Approval Decision** - Go/no-go decision
4. **Implementation** (if approved)
   - Schema changes
   - Code implementation
   - Testing
   - Documentation updates
5. **Release** - Merge to main branch

## Timeline Estimate

**If approved today**:
- Schema changes: 30 minutes
- Implementation: 2-3 hours
- Unit tests: 1-2 hours
- Integration tests: 1 hour
- Documentation: 2 hours
- **Total**: ~1 day of work

## Success Criteria

This feature will be considered successful when:

1. ✅ Test data can specify tick snapshots
2. ✅ Snapshots are automatically compared during execution
3. ✅ Failures include tick number and description
4. ✅ All existing tests pass unchanged
5. ✅ Example test data demonstrates the feature
6. ✅ Documentation is complete and clear
7. ✅ No performance regression for tests without snapshots

## Risks and Mitigation

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Performance overhead | Low | Low | Only check when snapshot exists |
| Test data verbosity | Medium | Low | Snapshots are optional |
| Complexity creep | Low | Medium | Keep implementation minimal |
| Configuration errors | Medium | Low | Reuse existing infrastructure |

## Alternatives Considered

1. **Pause Points** - Manual comparison in test code
   - Rejected: Not data-driven, requires code per test

2. **Record and Replay** - Record all states
   - Rejected: More complex, harder to specify expected states

3. **Diff-Based** - Specify only changes
   - Rejected: More complex to maintain

4. **Function Callbacks** - Custom validation per tick
   - Rejected: Not data-driven

**Chosen approach** (tick snapshots) is the simplest and most data-driven.

## Conclusion

Tick-by-tick entity state comparison is a valuable addition to the test harness that:

- Solves a real problem (debugging multi-tick tests)
- Has minimal implementation cost (~50 lines of code)
- Is 100% backward compatible
- Provides significant value for complex test scenarios
- Follows existing patterns and conventions

**Recommendation**: Proceed with implementation as proposed.

---

## Contact

For questions or feedback on this proposal:
- Open an issue in the repository
- Comment on the PR containing this proposal
- Contact the team through your normal channels

---

**Proposal Version**: 1.0  
**Date**: 2025-11-12  
**Status**: Under Review  
**Author**: GitHub Copilot Coding Agent

---

## Appendix: Quick Stats

- **Documentation Pages**: 4
- **Total Documentation**: ~46KB
- **Example Code Snippets**: 15+
- **Diagrams**: 10+
- **Use Case Examples**: 5
- **Code Changes Required**: ~50 lines
- **Breaking Changes**: 0
- **Estimated Implementation Time**: 1 day
