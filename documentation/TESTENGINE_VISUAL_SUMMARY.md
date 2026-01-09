# TestEngine Data Bank - Visual Summary

## The Question

> How should TestEngine compare tick-by-tick snapshots during simulation testing?

## The Answer

**Approach 1: Snapshot Copying with Deferred Comparison** ✅

## Visual Flow Diagram

```
┌───────────────────────────────────────────────────────────────────┐
│                         TEST EXECUTION                             │
└───────────────────────────────────────────────────────────────────┘

    ┌─────────────────────┐
    │   Test Data Config  │  ← JSON with expected snapshots
    └──────────┬──────────┘
               │
               ▼
    ┌─────────────────────┐
    │    TestEngine       │
    │  ┌───────────────┐  │
    │  │ RunGameLoop() │  │
    │  └───────┬───────┘  │
    │          │          │
    │  ┌───────▼───────┐  │
    │  │  Tick 0       │──┼─► m_data_bank[0] = Copy(EntityMemoryPool)
    │  └───────┬───────┘  │
    │          │          │
    │  ┌───────▼───────┐  │
    │  │  Tick 1       │──┼─► m_data_bank[1] = Copy(EntityMemoryPool)
    │  └───────┬───────┘  │
    │          │          │
    │  ┌───────▼───────┐  │
    │  │  Tick 2       │──┼─► m_data_bank[2] = Copy(EntityMemoryPool)
    │  └───────┬───────┘  │
    │          │          │
    │  ┌───────▼───────┐  │
    │  │   Tick ...    │──┼─► m_data_bank[...] = ...
    │  └───────────────┘  │
    └──────────┬──────────┘
               │
               │ GetDataBank()
               ▼
    ┌──────────────────────────────────────┐
    │  std::map<size_t, EntityMemoryPool>  │
    │  ┌────────────────────────────────┐  │
    │  │ [0] → EntityMemoryPool (tick 0)│  │
    │  │ [1] → EntityMemoryPool (tick 1)│  │
    │  │ [2] → EntityMemoryPool (tick 2)│  │
    │  │ [...] → ...                    │  │
    │  └────────────────────────────────┘  │
    └──────────┬───────────────────────────┘
               │
               ▼
    ┌─────────────────────┐
    │   Test Harness      │
    │                     │
    │  for each expected  │
    │    snapshot:        │
    │                     │
    │    actual = bank[t] │
    │    expected = cfg[t]│
    │                     │
    │    REQUIRE_THAT(    │
    │      actual,        │
    │      Equals(        │
    │        expected,    │
    │        context))    │
    │                     │
    └──────────┬──────────┘
               │
               ▼
    ┌─────────────────────┐
    │  ✅ All Pass        │
    │  or                 │
    │  ❌ Failures with   │
    │     detailed msgs   │
    └─────────────────────┘
```

## Side-by-Side Comparison

### Approach 1: Copy & Compare Later (RECOMMENDED) ✅

```
┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│  TestEngine  │────▶│  Data Bank   │────▶│ Test Harness │
│              │     │              │     │              │
│ • Simulates  │     │ • Stores     │     │ • Compares   │
│ • Captures   │     │   history    │     │ • Reports    │
└──────────────┘     └──────────────┘     └──────────────┘

Pros:
✅ Clean separation
✅ Full debugging history
✅ Low coupling
✅ Easy to extend

Cons:
⚠️ Uses more memory (negligible for tests)
```

### Approach 2: Inject & Compare During (NOT RECOMMENDED) ❌

```
┌─────────────────────────────────────┐
│          TestEngine                 │
│  ┌──────────┐     ┌──────────────┐ │
│  │Simulates │────▶│Comparison    │ │
│  │          │     │Engine        │ │
│  │          │     │(injected)    │ │
│  └──────────┘     └──────────────┘ │
└─────────────────────────────────────┘

Pros:
✅ Lower memory

Cons:
❌ Tight coupling
❌ No debugging history
❌ Complex to maintain
❌ Violates single responsibility
```

## Memory Visualization

### Typical Test (5 ticks, 50 entities)

```
Tick 0: [Entity][Entity]...[Entity]  ← 25 KB
Tick 1: [Entity][Entity]...[Entity]  ← 25 KB
Tick 2: [Entity][Entity]...[Entity]  ← 25 KB
Tick 3: [Entity][Entity]...[Entity]  ← 25 KB
Tick 4: [Entity][Entity]...[Entity]  ← 25 KB
                                      ────────
                           Total: 125 KB  ✅ TRIVIAL
```

### Stress Test (100 ticks, 1000 entities)

```
100 ticks × 500 KB per tick = 50 MB  ✅ ACCEPTABLE

(Modern machines have GB of RAM)
```

## Debugging Experience

### Approach 1: Full History Available

```
Test fails at Tick 7
                                    ❌ Failure
                                    │
├── Tick 0 ✓ ────────────────────┐ │
├── Tick 1 ✓ ────────────────────┤ │
├── Tick 2 ✓ ────────────────────┤ │
├── Tick 3 ✓ ────────────────────┤ │  ◄── Can inspect
├── Tick 4 ✓ ────────────────────┤ │      any tick
├── Tick 5 ✓ ────────────────────┤ │
├── Tick 6 ✓ ─────────── Last good│ │
├── Tick 7 ❌ ───────────────────┴─┘
├── Tick 8 ✓ ────────────────────┐
└── Tick 9 ✓ ────────────────────┘
     │
     └─► All snapshots available for analysis
```

### Approach 2: Limited Context

```
Test fails at Tick 7
                                    ❌ Failure
                                    │
├── Tick 0 ✗ ─────────── Lost ─────┤
├── Tick 1 ✗ ─────────── Lost ─────┤
├── Tick 2 ✗ ─────────── Lost ─────┤
├── Tick 3 ✗ ─────────── Lost ─────┤  ◄── Cannot inspect
├── Tick 4 ✗ ─────────── Lost ─────┤      previous ticks
├── Tick 5 ✗ ─────────── Lost ─────┤
├── Tick 6 ✗ ─────────── Lost ─────┤
├── Tick 7 ❌ ───────────────────┴─┘
└── Tick 8+ not executed (aborted)
     │
     └─► Must re-run to debug
```

## Architecture Comparison

### Approach 1: Loosely Coupled ✅

```
┌──────────────────────────────────────────────┐
│              Test Layer                      │
│  ┌────────────────┐    ┌─────────────────┐  │
│  │  Test Harness  │    │    Matchers     │  │
│  │   - Compare    │◄───│ - Equals...     │  │
│  │   - Report     │    │ - TestContext   │  │
│  └────────┬───────┘    └─────────────────┘  │
└───────────┼──────────────────────────────────┘
            │ Uses
            │ (no dependency)
┌───────────▼──────────────────────────────────┐
│           Engine Layer                       │
│  ┌────────────────┐    ┌─────────────────┐  │
│  │  TestEngine    │    │   Data Bank     │  │
│  │   - Simulate   │    │   - Store       │  │
│  │   - Capture    │───▶│   - Retrieve    │  │
│  └────────────────┘    └─────────────────┘  │
└──────────────────────────────────────────────┘

Legend: ───▶ Dependency
        ◄─── Usage (no dependency)
```

### Approach 2: Tightly Coupled ❌

```
┌──────────────────────────────────────────────┐
│              Test Layer                      │
│  ┌────────────────┐    ┌─────────────────┐  │
│  │IComparison     │    │    Matchers     │  │
│  │Engine          │◄───│ - Equals...     │  │
│  │(interface)     │    │ - TestContext   │  │
│  └────────┬───────┘    └─────────────────┘  │
└───────────┼──────────────────────────────────┘
            │
            │ Injected ❌
            │
┌───────────▼──────────────────────────────────┐
│           Engine Layer                       │
│  ┌─────────────────────────────────────────┐ │
│  │         TestEngine                      │ │
│  │   - Simulate                            │ │
│  │   - Compare (depends on test infra) ❌  │ │
│  │   - Report                              │ │
│  └─────────────────────────────────────────┘ │
└──────────────────────────────────────────────┘

Legend: ───▶ Dependency (violation!)
```

## Key Metrics Comparison

```
┌────────────────────────────┬──────────────┬──────────────┐
│         Metric             │  Approach 1  │  Approach 2  │
├────────────────────────────┼──────────────┼──────────────┤
│ Memory (typical)           │   125 KB     │    < 1 KB    │
│ Memory (stress)            │    50 MB     │    < 1 MB    │
│ Debugging capability       │   ★★★★★     │     ★★☆☆☆    │
│ Separation of concerns     │   ★★★★★     │     ★☆☆☆☆    │
│ Coupling                   │     Low      │     High     │
│ Complexity                 │   Simple     │    Complex   │
│ Maintainability            │   ★★★★★     │     ★★☆☆☆    │
│ Extensibility              │   ★★★★★     │     ★★☆☆☆    │
│ Aligns with patterns       │   ★★★★★     │     ★☆☆☆☆    │
│                            │              │              │
│ OVERALL SCORE              │   ★★★★★     │     ★★☆☆☆    │
└────────────────────────────┴──────────────┴──────────────┘

Winner: Approach 1 (Snapshot Copying) ✅
```

## Implementation Timeline

```
Week 1
├── Day 1-2: Phase 1 - Basic Data Bank Structure
│   └─► Add m_data_bank, GetDataBank(), unit tests
│
├── Day 2-3: Phase 2 - Snapshot Capture Integration  
│   └─► Implement CaptureSnapshot(), update RunGameLoop()
│
└── Day 3-5: Phase 3 - Test Harness Comparison
    └─► Comparison functions, RunTestEngineTest()

Week 2 (Optional)
└── Day 1-3: Phase 4 - Enhanced Snapshots
    └─► Add EventBus, Scene state, extended comparison

Total: 7-17 hours (low to medium complexity)
```

## Decision Tree

```
                    Need tick-by-tick comparison?
                              │
                        Yes ──┘
                              │
                              ▼
                    ┌─────────────────────┐
                    │ Memory constraint?  │
                    └──────┬─────┬────────┘
                           │     │
                      No ──┘     └── Yes
                       │              │
                       ▼              ▼
              ┌────────────┐   Is it really?
              │ Approach 1 │   (Tests use KB-MB)
              │     ✅     │          │
              └────────────┘     No ──┘
                                      │
                                      └─► Still Approach 1 ✅

Conclusion: Always choose Approach 1 for test infrastructure
```

## One-Sentence Summary

**TestEngine should capture state history in a data bank and let the test harness validate it later, because memory is cheap but debugging complex multi-tick failures without history is expensive.**

## The Bottom Line

```
┌─────────────────────────────────────────────────────────────┐
│                                                             │
│  Memory: ~125 KB (typical) or ~50 MB (stress)              │
│  ───────────────────────────────────────────────────────   │
│                         vs                                  │
│  ───────────────────────────────────────────────────────   │
│  Debugging: Full history, clean architecture, loose coupling│
│                                                             │
│                      CHOICE IS CLEAR                        │
│                                                             │
│              Choose Approach 1 ✅                           │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## See Also

- [Quick Decision Guide](TESTENGINE_QUICK_DECISION_GUIDE.md) - Fast reference
- [Implementation Roadmap](TESTENGINE_IMPLEMENTATION_ROADMAP.md) - How to build it
- [Full Analysis](TESTENGINE_SIMULATION_ANALYSIS.md) - Deep dive
- [Documentation Index](README.md) - Navigation
