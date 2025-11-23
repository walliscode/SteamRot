# Tick-by-Tick Comparison - Visual Examples

This document provides visual examples of how tick-by-tick comparison works in the test harness.

## Execution Flow Diagram

### Current System (Start/End Comparison Only)

```
Test Execution Timeline
========================

START STATE                                                    END STATE
(defined in                                                    (defined in
start_entity_collection)                                       expected_entity_collection)
    |                                                              |
    v                                                              v
+--------+                                                     +--------+
| Tick 0 | -> inputs -> events -> simulation -> tick bus ->  |        |
+--------+                                                     |        |
                                                               |        |
+--------+                                                     |        |
| Tick 1 | -> inputs -> events -> simulation -> tick bus ->  |        |
+--------+                                                     |        |
                                                               | Compare|
+--------+                                                     | (only  |
| Tick 2 | -> inputs -> events -> simulation -> tick bus ->  | at end)|
+--------+                                                     |        |
                                                               |        |
+--------+                                                     |        |
| Tick 3 | -> inputs -> events -> simulation -> tick bus ->  |        |
+--------+                                                     +--------+
                                                                   ^
                                                                   |
                                                          Single comparison point

Problem: If comparison fails, we don't know which tick caused the issue!
```

### Proposed System (Tick-by-Tick Comparison)

```
Test Execution Timeline with Snapshots
=======================================

START STATE              SNAPSHOT 1          SNAPSHOT 2          END STATE
    |                        |                   |                   |
    v                        v                   v                   v
+--------+               +--------+          +--------+          +--------+
| Tick 0 | -> inputs -> | Compare| -> bus   |        |          |        |
+--------+    events     +--------+          |        |          |        |
              sim                            |        |          |        |
                                             |        |          |        |
+--------+                                +--------+  |          |        |
| Tick 1 | -> inputs -> events -> sim -> | Compare| -> bus      |        |
+--------+                                +--------+  |          |        |
                                                      |          |        |
+--------+                                            |       +--------+  |
| Tick 2 | -> inputs -> events -> sim -> tick bus -> |       | Compare|  |
+--------+                                            |       +--------+  |
                                                      |                   |
+--------+                                            |                +--------+
| Tick 3 | -> inputs -> events -> sim -> tick bus -> |                | Compare|
+--------+                                                             +--------+
    ^                   ^                   ^                              ^
    |                   |                   |                              |
Multiple comparison points - we know exactly when state diverges!
```

## Detailed Per-Tick Execution

### Without Tick Snapshot (Current)

```
Tick N Execution
================

┌─────────────────────────────────────────────────┐
│  1. Execute inputs scheduled for tick N         │
│     - Mouse moves, clicks                       │
│     - Keyboard presses                          │
└─────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────┐
│  2. Execute events scheduled for tick N         │
│     - Add events to waiting room                │
└─────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────┐
│  3. Process event waiting room                  │
│     - Move events to global bus                 │
└─────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────┐
│  4. Execute simulation steps                    │
│     - Logic classes                             │
│     - Free functions                            │
└─────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────┐
│  5. Tick the global event bus                   │
│     - Decrement lifetimes                       │
│     - Remove expired events                     │
└─────────────────────────────────────────────────┘
                    ↓
            (Continue to next tick)
```

### With Tick Snapshot (Proposed)

```
Tick N Execution with Snapshot
===============================

┌─────────────────────────────────────────────────┐
│  1. Execute inputs scheduled for tick N         │
│     - Mouse moves, clicks                       │
│     - Keyboard presses                          │
└─────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────┐
│  2. Execute events scheduled for tick N         │
│     - Add events to waiting room                │
└─────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────┐
│  3. Process event waiting room                  │
│     - Move events to global bus                 │
└─────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────┐
│  4. Execute simulation steps                    │
│     - Logic classes                             │
│     - Free functions                            │
└─────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────┐
│  5. ⭐ NEW: Check for tick snapshot ⭐           │
│     - Find snapshot for tick N                  │
│     - If found:                                 │
│       • Create expected EntityMemoryPool        │
│       • Compare with current pool               │
│       • Include snapshot description in errors  │
└─────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────┐
│  6. Tick the global event bus                   │
│     - Decrement lifetimes                       │
│     - Remove expired events                     │
└─────────────────────────────────────────────────┘
                    ↓
            (Continue to next tick)
```

## Example Scenario: Button Toggle

### Scenario Setup

```
Entity: UI Button
Initial State: Invisible (is_visible = false)
Action: User clicks button each tick to toggle visibility
Expected: Button alternates between visible and invisible
```

### Execution Timeline

```
Tick-by-Tick State Changes
===========================

START:
┌─────────────────────────┐
│ Button: Invisible       │
│ (is_visible = false) │
└─────────────────────────┘
            ↓ user clicks
            ↓
TICK 0: ⭐ SNAPSHOT 1 ⭐
┌─────────────────────────┐
│ Button: Visible         │
│ (is_visible = true)  │ ← Verify state after first click
└─────────────────────────┘
            ↓ user clicks
            ↓
TICK 1: ⭐ SNAPSHOT 2 ⭐
┌─────────────────────────┐
│ Button: Invisible       │
│ (is_visible = false) │ ← Verify state after second click
└─────────────────────────┘
            ↓ user clicks
            ↓
TICK 2: ⭐ SNAPSHOT 3 ⭐
┌─────────────────────────┐
│ Button: Visible         │
│ (is_visible = true)  │ ← Verify state after third click
└─────────────────────────┘

Each snapshot verifies the state is correct at that specific tick!
```

### Without Snapshots

```
Problem Scenario: Toggle Logic Broken
======================================

START:
┌─────────────────────────┐
│ Button: Invisible       │
└─────────────────────────┘
            ↓ user clicks
            ↓
TICK 0: (no check)
┌─────────────────────────┐
│ Button: Visible         │ ← Correct, but we don't verify
└─────────────────────────┘
            ↓ user clicks
            ↓
TICK 1: (no check)
┌─────────────────────────┐
│ Button: VISIBLE         │ ← ❌ BUG! Should be invisible
└─────────────────────────┘  but we don't know yet...
            ↓ user clicks
            ↓
TICK 2: (no check)
┌─────────────────────────┐
│ Button: VISIBLE         │ ← Still wrong...
└─────────────────────────┘
            ↓
END STATE: ❌ FAIL
┌─────────────────────────┐
│ Expected: Visible       │
│ Actual: Visible         │ ← Test fails but we don't know
└─────────────────────────┘  WHICH tick caused the issue!

Without snapshots, we only know the END state is wrong.
We have to manually debug to find that tick 1 was the problem.
```

### With Snapshots

```
Success Scenario: Early Detection
==================================

START:
┌─────────────────────────┐
│ Button: Invisible       │
└─────────────────────────┘
            ↓ user clicks
            ↓
TICK 0: ⭐ SNAPSHOT 1 ⭐
┌─────────────────────────┐
│ Button: Visible ✓       │ ← Snapshot passes!
└─────────────────────────┘
            ↓ user clicks
            ↓
TICK 1: ⭐ SNAPSHOT 2 ⭐
┌─────────────────────────┐
│ Expected: Invisible     │
│ Actual: VISIBLE         │ ← ❌ FAIL at tick 1!
└─────────────────────────┘
        ↓
    TEST FAILS HERE
    
Error Message:
"EntityMemoryPool mismatch [Tick 1 (After second click - should be invisible)]:
 Component value differs at entity 0:
   CUserInterface.is_visible: actual=true, expected=false"

With snapshots, we IMMEDIATELY know:
- The failure happened at tick 1
- It was after the second click
- The button should have been invisible
- We can now debug the toggle logic for the second click specifically!
```

## Data Structure Comparison

### Test Data Structure

```
Current Structure:
==================

TestDataConfig
├── metadata
├── start_entity_collection ─┐
├── simulation_data          │  Only 2 comparison points
├── input_sequence           │  (start and end)
└── expected_entity_collection ┘


Proposed Structure:
===================

TestDataConfig
├── metadata
├── start_entity_collection ─┐
├── simulation_data          │
├── input_sequence           │  Multiple comparison points!
├── tick_snapshots ───────────┼─────┐
│   ├── [0] tick: 0          │     │
│   │       entity_collection ┘     │  Intermediate states
│   ├── [1] tick: 2                 │  verified at specific ticks
│   │       entity_collection       │
│   └── [2] tick: 5                 │
│           entity_collection       │
└── expected_entity_collection ─────┘
```

## Use Case Examples

### Use Case 1: State Machine Verification

```
State Machine Test
==================

Tick 0: IDLE state
    ↓ (event arrives)
    ⭐ Snapshot: Verify IDLE state ⭐
    
Tick 1: Process event
    ↓ (transition to PROCESSING)
    
Tick 2: PROCESSING state
    ⭐ Snapshot: Verify PROCESSING state ⭐
    ↓ (work completes)
    
Tick 3: Transition to COMPLETE
    
Tick 4: COMPLETE state
    ⭐ Snapshot: Verify COMPLETE state ⭐
    ↓ (cleanup)
    
Tick 5: Back to IDLE
    ⭐ Snapshot: Verify returned to IDLE ⭐
```

### Use Case 2: Accumulation Testing

```
Counter Accumulation Test
=========================

Tick 0: counter = 0
    ⭐ Snapshot: counter = 0 ⭐
    ↓ (+1)
    
Tick 1: counter = 1
    ⭐ Snapshot: counter = 1 ⭐
    ↓ (+2)
    
Tick 2: counter = 3
    ⭐ Snapshot: counter = 3 ⭐
    ↓ (+3)
    
Tick 3: counter = 6
    ⭐ Snapshot: counter = 6 ⭐
    ↓ (+4)
    
Tick 4: counter = 10
    ⭐ Snapshot: counter = 10 ⭐

Verify accumulation pattern at each step!
```

### Use Case 3: Event Lifetime Testing

```
Event Lifetime Test
===================

Tick 0: Add event (lifetime = 5)
    ⭐ Snapshot: 1 event in bus, lifetime = 5 ⭐
    ↓ (tick bus)
    
Tick 1: Event ages
    ⭐ Snapshot: 1 event in bus, lifetime = 4 ⭐
    ↓ (tick bus)
    
Tick 2: Event ages
    ⭐ Snapshot: 1 event in bus, lifetime = 3 ⭐
    ↓ (tick bus)
    
    ... (continue ticking) ...
    
Tick 5: Event expires
    ⭐ Snapshot: 0 events in bus ⭐

Verify event lifetime decrements correctly each tick!
```

## Comparison: With vs Without Snapshots

### Debugging Effort

```
WITHOUT SNAPSHOTS:
==================
Test fails at end
    ↓
Manually add print statements
    ↓
Re-run test
    ↓
Analyze output
    ↓
Guess which tick failed
    ↓
Repeat until found
    
Time: 15-30 minutes


WITH SNAPSHOTS:
===============
Test fails at tick N
    ↓
Error message says:
  "Tick N (description)"
    ↓
Know exactly where to look!
    
Time: 1-2 minutes
```

### Test Confidence

```
WITHOUT SNAPSHOTS:
==================
┌────────────────────────┐
│ ? ? ? ? ? ? ? ? PASS ✓ │  Final state correct,
└────────────────────────┘  but were all intermediate
    ^                ^         states correct? Unknown!
    Start            End


WITH SNAPSHOTS:
===============
┌────────────────────────┐
│ ✓ ✓ ✓ ✓ ✓ ✓ ✓ ✓ PASS ✓ │  Every checked state
└────────────────────────┘  was verified correct!
    ^   ^   ^   ^      ^
  Start │   │   │     End
     Snapshots
     
Higher confidence in correctness!
```

## Summary

### Key Advantages of Tick-by-Tick Comparison

1. **Precise Failure Location** - Know exactly which tick failed
2. **Faster Debugging** - No guessing or manual instrumentation
3. **Incremental Verification** - Build confidence step by step
4. **Better Test Coverage** - Verify intermediate states
5. **Clear Documentation** - Snapshots document expected behavior

### When to Use

✅ **Use Snapshots When:**
- Testing multi-step transformations
- Verifying state machine transitions
- Testing accumulating values
- Debugging complex simulations
- Need high confidence in intermediate states

❌ **Don't Need Snapshots When:**
- Simple single-tick tests
- Only end state matters
- No complex state transformations
- Quick smoke tests

---

**Note**: All diagrams in this document represent the proposed system design. Implementation has not yet begun - this is proposal only.
