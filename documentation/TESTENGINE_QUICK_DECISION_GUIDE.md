# TestEngine Simulation: Quick Decision Guide

## TL;DR

**Use Approach 1: Snapshot Copying with Deferred Comparison**

Copy the EntityMemoryPool at each tick and compare all snapshots at the end.

## Visual Comparison

### Approach 1: Snapshot Copying (RECOMMENDED ✅)

```
┌─────────────────────────────────────────────────────┐
│                    TestEngine                        │
│                                                      │
│  RunGameLoop() {                                     │
│    for each tick:                                    │
│      ├─ ExecuteTick()                               │
│      └─ m_data_bank[tick] = CopyEntityMemoryPool()  │
│  }                                                   │
│                                                      │
│  GetDataBank() → returns m_data_bank                 │
└─────────────────────────────────────────────────────┘
                         │
                         │ Data Bank
                         ▼
┌─────────────────────────────────────────────────────┐
│                   Test Harness                       │
│                                                      │
│  for each expected_snapshot:                         │
│    ├─ actual = data_bank[tick]                      │
│    ├─ expected = test_data.snapshots[tick]          │
│    └─ REQUIRE_THAT(actual,                          │
│         EqualsEntityMemoryPool(expected, context))   │
└─────────────────────────────────────────────────────┘
```

**Flow:**
1. TestEngine runs simulation
2. TestEngine captures snapshots
3. TestEngine returns data bank
4. Test harness compares snapshots

### Approach 2: Per-Tick Matcher (NOT RECOMMENDED ❌)

```
┌─────────────────────────────────────────────────────┐
│                    TestEngine                        │
│                                                      │
│  RunGameLoop() {                                     │
│    for each tick:                                    │
│      ├─ ExecuteTick()                               │
│      ├─ GetEntityMemoryPool() → reference           │
│      └─ m_comparison_engine->Compare(                │
│           tick, pool_ref, expected, context)         │
│  }                                                   │
└─────────────────────────────────────────────────────┘
          │                     ▲
          │ EMP reference       │ Comparison Engine
          ▼                     │ (injected dependency)
┌─────────────────────────────────────────────────────┐
│              IComparisonEngine                       │
│                                                      │
│  CompareSnapshot(tick, actual, expected, context) {  │
│    matcher = EntityMemoryPoolEqualsMatcher(...)      │
│    return matcher.match(actual)                      │
│  }                                                   │
└─────────────────────────────────────────────────────┘
```

**Flow:**
1. TestEngine runs simulation
2. TestEngine calls injected comparison engine per tick
3. Comparison happens inside TestEngine
4. TestEngine coupled to test infrastructure

## One-Sentence Summary

| Approach | Summary |
|----------|---------|
| **Approach 1** | TestEngine captures state history; test harness validates it later |
| **Approach 2** | TestEngine validates state as it executes using injected matcher |

## Decision Factors

### Choose Approach 1 If:
- ✅ You want clean separation between simulation and validation
- ✅ You need to debug failures by examining tick history
- ✅ You want to add new comparisons without changing TestEngine
- ✅ You prefer simpler code with lower coupling
- ✅ Memory usage is not a critical constraint (it's not for tests)

### Choose Approach 2 If:
- ❌ You absolutely must minimize memory (you don't for tests)
- ❌ You need immediate failure detection (deferred is better for debugging)
- ❌ You want TestEngine to handle validation (violates single responsibility)

## Key Differences

| Aspect | Approach 1 | Approach 2 |
|--------|-----------|-----------|
| **Who validates?** | Test harness | TestEngine |
| **When?** | After all ticks | During each tick |
| **Coupling** | Low | High |
| **Debugging** | Easy (full history) | Hard (no history) |
| **Memory** | Higher (negligible) | Lower |
| **Complexity** | Simple | Complex |

## Real-World Impact

### Debugging a Test Failure

**Approach 1:**
```
Test fails on tick 7 of 10.

Developer:
1. Examines data_bank[6] - state before failure
2. Examines data_bank[7] - state at failure
3. Examines data_bank[8] - state after failure
4. Identifies the exact tick where state diverged
5. Re-runs comparison with additional debug output
6. Fixes issue quickly
```

**Approach 2:**
```
Test fails on tick 7 of 10.

Developer:
1. Test aborted at tick 7 (or continued with error flag?)
2. Cannot examine tick 6 or tick 8
3. Must re-run simulation to inspect previous state
4. Must modify TestEngine to add debug output
5. Slower iteration cycle
```

### Adding a New Comparison Type

**Approach 1:**
```cpp
// In test harness (no TestEngine changes)
const auto& data_bank = engine.GetDataBank();

// Add EventBus comparison
for (const auto& [tick, snapshot] : expected_snapshots) {
  const auto& actual_events = data_bank.GetEventBus(tick);
  REQUIRE_THAT(actual_events, EqualsEventBus(snapshot.event_bus));
}
```

**Approach 2:**
```cpp
// Must modify TestEngine
class TestEngine {
  void RunGameLoop() {
    // Add new comparison logic here
    m_comparison_engine->CompareEventBus(...);
  }
};

// Must extend interface
class IComparisonEngine {
  virtual CompareEventBus(...) = 0;
};

// Must update all implementations
// More files to change, more coupling
```

## Example: Memory Usage

### Typical Test Scenario
```
Entity Count: 50
Component Size: ~100 bytes each
Components: 5
Ticks: 5

Memory per snapshot: 50 × 100 × 5 = 25 KB
Total memory: 25 KB × 5 = 125 KB
```

**125 KB is trivial** for a test that provides full debugging capability.

### Stress Test Scenario
```
Entity Count: 1000
Ticks: 100

Total memory: 1000 × 100 × 5 × 100 = 50 MB
```

**50 MB is still acceptable** for a comprehensive stress test. Modern machines have GB of RAM.

## Recommended Implementation Path

### Phase 1: Basic Data Bank (Week 1)
```cpp
// TestEngine.h
class TestEngine : public Engine {
private:
  std::map<size_t, EntityMemoryPool> m_data_bank;
  void CaptureSnapshot(size_t tick);
public:
  const std::map<size_t, EntityMemoryPool>& GetDataBank() const;
};
```

### Phase 2: Test Harness Integration (Week 1)
```cpp
// test_harness.cpp
std::expected<std::monostate, FailInfo> RunTestEngineTest(const TestDataConfig* config) {
  TestEngine engine(test_data);
  engine.RunGame();
  
  const auto& data_bank = engine.GetDataBank();
  // Compare snapshots using existing matchers
}
```

### Phase 3: Enhanced Snapshots (Week 2)
```cpp
// Add EventBus, Scene state, etc.
struct EngineSnapshot {
  EntityMemoryPool entity_pool;
  EventBus event_bus;
  SceneType active_scene;
};
```

## Common Objections Addressed

### "But Approach 2 uses less memory!"
**Counter:** The memory difference is negligible for tests (KB to low MB range). The debugging benefits of Approach 1 far outweigh this trivial memory cost.

### "But Approach 2 fails faster!"
**Counter:** Failing fast is actually worse for debugging. Seeing all failures at once gives you the complete picture. Plus, you can still examine earlier ticks with Approach 1.

### "But Approach 2 gives TestEngine more control!"
**Counter:** TestEngine *shouldn't* have comparison control. That violates single responsibility. Simulation and validation are separate concerns.

### "But copying is slow!"
**Counter:** Standard library containers are optimized for copying. For test entity counts (typically <100), copying takes microseconds. This is not a bottleneck.

## Alignment with Project Patterns

### Existing Infrastructure Supports Approach 1

The project already has:
- ✅ `EntityMemoryPoolEqualsMatcher` - Designed for external use
- ✅ `TestContext` - Metadata structure for enriched error messages
- ✅ `test_harness` functions - Orchestration layer separate from execution
- ✅ `TestData.expected_engine_snapshots` - Map structure suggesting deferred comparison

All of these point to **Approach 1** being the intended design.

### README Already Mentions "Data Bank"

From `tests/harness/README.md`:
```markdown
## Purpose
- **Data bank** - capture scene state at each tick for comparison
- **Tick snapshot validation** - compare captured state with expected snapshots
```

The README literally describes **Approach 1** by name.

## Final Recommendation

**Implement Approach 1: Snapshot Copying with Deferred Comparison**

It is:
- ✅ Simpler to implement
- ✅ Easier to debug
- ✅ More maintainable
- ✅ Better aligned with existing patterns
- ✅ More flexible for future enhancements
- ✅ Following SOLID principles

The memory overhead is trivial, and the debugging benefits are enormous.

## Questions to Ask Yourself

Before choosing Approach 2, ask:

1. **Do I really need to validate during execution?** (Probably not)
2. **Is memory really a constraint for tests?** (No)
3. **Will I ever need to debug a multi-tick test?** (Yes)
4. **Do I want TestEngine to depend on test infrastructure?** (No)
5. **Will I ever want to add new comparison types?** (Yes)

If you answered the above questions honestly, **Approach 1** is the clear choice.

## See Also

- [TESTENGINE_SIMULATION_ANALYSIS.md](TESTENGINE_SIMULATION_ANALYSIS.md) - Full detailed analysis
- `tests/harness/README.md` - Test harness documentation
- `tests/matchers/EntityMemoryPoolEqualsMatcher.h` - Existing matcher infrastructure
