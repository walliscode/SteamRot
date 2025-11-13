# Tick-by-Tick Comparison - Quick Reference

This is a quick reference guide for the proposed tick-by-tick comparison feature.

## At a Glance

**Feature**: Verify entity states at intermediate ticks during multi-tick simulations  
**Status**: Proposal (not yet implemented)  
**Impact**: Test infrastructure enhancement  
**Compatibility**: 100% backward compatible  

## Quick Syntax

### Minimal Example

```json
{
  "metadata": {...},
  "num_ticks": 3,
  "tick_snapshots": [
    {
      "tick": 1,
      "description": "After first action",
      "entity_collection": {...}
    }
  ]
}
```

### Full Example

```json
{
  "metadata": {
    "test_name": "my_tick_test",
    "description": "Test with tick verification"
  },
  "num_ticks": 5,
  "start_entity_collection": {
    "entity_memory_pool_size": 3,
    "entities": [...]
  },
  "tick_snapshots": [
    {
      "tick": 0,
      "description": "After tick 0",
      "entity_collection": {
        "entity_memory_pool_size": 3,
        "entities": [...]
      }
    },
    {
      "tick": 2,
      "description": "After tick 2",
      "entity_collection": {
        "entity_memory_pool_size": 3,
        "entities": [...]
      }
    },
    {
      "tick": 4,
      "description": "Final state",
      "entity_collection": {
        "entity_memory_pool_size": 3,
        "entities": [...]
      }
    }
  ],
  "simulation_data": {...},
  "input_sequence": {...}
}
```

## Field Reference

### TickSnapshot Table

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `tick` | uint32 | Yes | Tick number when to compare (0-based) |
| `entity_collection` | EntityCollection | Yes | Expected entity state at this tick |
| `description` | string | No | Human-readable description for this checkpoint |

### TestDataConfig Addition

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `tick_snapshots` | [TickSnapshot] | No | Array of intermediate state checkpoints |

## Execution Timing

```
Per Tick:
1. Execute inputs
2. Execute events  
3. Process waiting room
4. Execute simulation
5. ⭐ Compare snapshot (if exists for this tick) ⭐
6. Tick event bus
```

**Snapshot checked AFTER simulation, BEFORE event bus tick**

## Common Patterns

### Every Tick

Verify state at every single tick:

```json
"num_ticks": 3,
"tick_snapshots": [
  {"tick": 0, "entity_collection": {...}},
  {"tick": 1, "entity_collection": {...}},
  {"tick": 2, "entity_collection": {...}}
]
```

### Sparse Snapshots

Only verify key ticks:

```json
"num_ticks": 10,
"tick_snapshots": [
  {"tick": 0, "description": "Start", "entity_collection": {...}},
  {"tick": 5, "description": "Mid", "entity_collection": {...}},
  {"tick": 9, "description": "End", "entity_collection": {...}}
]
```

### Transition Points

Verify before/after critical operations:

```json
"num_ticks": 5,
"tick_snapshots": [
  {"tick": 2, "description": "Before transition", "entity_collection": {...}},
  {"tick": 3, "description": "After transition", "entity_collection": {...}}
]
```

## Error Messages

### Success
```
Test passes silently (no snapshot-related output)
```

### Failure
```
EntityMemoryPool mismatch [Tick 2 (After toggle)]: 
Component value differs at entity 0: 
  CUserInterface.start_visible: actual=false, expected=true
```

Error includes:
- Tick number where mismatch occurred
- Snapshot description (if provided)
- Detailed component differences

## Use Cases Cheat Sheet

| Use Case | Pattern | Example |
|----------|---------|---------|
| Toggle verification | Every tick | Button visibility alternates |
| State machine | Transition points | IDLE → PROCESSING → COMPLETE |
| Accumulation | Every tick | Counter increments each tick |
| Event lifetime | Every N ticks | Event expires after 5 ticks |
| Complex workflow | Sparse snapshots | Check at steps 2, 5, 8 |

## Best Practices

### ✅ DO

- Use descriptive snapshot descriptions
- Verify at transition points
- Keep pool sizes consistent
- Only snapshot what you need to verify
- Use sparse snapshots for long tests

### ❌ DON'T

- Snapshot every tick for very long tests (unless needed)
- Forget to match entity_memory_pool_size
- Have multiple snapshots for same tick
- Specify snapshots for non-existent ticks (though harmless)

## Edge Cases

| Scenario | Behavior |
|----------|----------|
| No `tick_snapshots` field | Test runs normally (backward compatible) |
| Empty array `[]` | Test runs normally (no comparisons) |
| Snapshot tick > num_ticks | Snapshot never checked (no error) |
| Multiple snapshots same tick | Only first checked |
| Snapshot without description | Works fine, just less informative errors |

## Decision Tree

```
Should I use tick snapshots?
│
├─ Is this a single-tick test?
│  └─ No → Consider snapshots
│  └─ Yes → Probably don't need them
│
├─ Do intermediate states matter?
│  └─ Yes → Use snapshots
│  └─ No → Skip snapshots
│
├─ Is debugging complex behavior?
│  └─ Yes → Use snapshots
│  └─ No → Maybe skip
│
└─ Need high confidence in each step?
   └─ Yes → Use snapshots
   └─ No → End comparison may suffice
```

## Performance Impact

| Scenario | Overhead |
|----------|----------|
| No snapshots | ~0 (single null check per tick) |
| With snapshots | Same as end comparison per snapshot |
| Many snapshots | Linear O(n) where n = number of snapshots |

**Optimization**: Pre-index snapshots by tick if >10 snapshots

## Migration Guide

### Existing Test

```json
{
  "metadata": {...},
  "num_ticks": 5,
  "start_entity_collection": {...},
  "simulation_data": {...},
  "expected_entity_collection": {...}
}
```

### Add Snapshots (Optional)

```json
{
  "metadata": {...},
  "num_ticks": 5,
  "start_entity_collection": {...},
  "simulation_data": {...},
  "tick_snapshots": [              // ⭐ NEW
    {
      "tick": 2,
      "description": "Mid-point check",
      "entity_collection": {...}
    }
  ],
  "expected_entity_collection": {...}
}
```

**No changes required to existing tests!**

## Implementation Status

- [x] Design proposal created
- [x] Visual examples documented
- [x] Quick reference created
- [ ] Schema changes (test_data.fbs)
- [ ] Implementation (tick_executor.cpp/h)
- [ ] Unit tests
- [ ] Integration tests
- [ ] Documentation updates
- [ ] Example test data files

**Status**: Awaiting review and approval

## Related Documentation

- **Full Design**: [TICK_BY_TICK_COMPARISON_DESIGN.md](TICK_BY_TICK_COMPARISON_DESIGN.md)
- **Visual Examples**: [TICK_BY_TICK_COMPARISON_VISUALS.md](TICK_BY_TICK_COMPARISON_VISUALS.md)
- **Proposals Index**: [README.md](README.md)

## FAQ

**Q: Will this break my existing tests?**  
A: No. `tick_snapshots` is optional. Existing tests work unchanged.

**Q: Can I have snapshots at non-consecutive ticks?**  
A: Yes! Sparse snapshots are fine (e.g., ticks 0, 5, 9).

**Q: What if snapshot comparison fails?**  
A: Test fails with detailed error showing tick, description, and differences.

**Q: Can I use snapshots with input_sequence and event_sequence?**  
A: Yes! They all work together.

**Q: How many snapshots is too many?**  
A: Use as many as you need. For very long tests (>100 ticks), consider sparse snapshots.

**Q: Can I compare partial entity state?**  
A: Not in initial version. Full EntityMemoryPool comparison only. (Partial snapshots could be future enhancement)

**Q: When is the snapshot comparison done?**  
A: After simulation steps, before event bus tick.

**Q: What if I have multiple entities?**  
A: Specify all entities you want to verify in the snapshot's entity_collection.

**Q: Is there a performance penalty?**  
A: Minimal. Only when snapshot exists for a tick, same cost as existing end comparison.

---

**Document Version**: 1.0  
**Date**: 2025-11-12  
**Status**: Proposal  
**Note**: This feature is not yet implemented. This is a reference for the proposed design.
