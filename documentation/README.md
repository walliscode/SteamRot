# SteamRot Documentation

This directory contains design documents, analysis, and implementation guides for the SteamRot game engine.

## TestEngine Simulation Analysis

A comprehensive analysis of architectural approaches for implementing tick-by-tick snapshot comparison in the TestEngine.

### Quick Start

1. **Need a quick decision?** → Read [TESTENGINE_QUICK_DECISION_GUIDE.md](TESTENGINE_QUICK_DECISION_GUIDE.md)
2. **Ready to implement?** → Follow [TESTENGINE_IMPLEMENTATION_ROADMAP.md](TESTENGINE_IMPLEMENTATION_ROADMAP.md)
3. **Want deep analysis?** → Review [TESTENGINE_SIMULATION_ANALYSIS.md](TESTENGINE_SIMULATION_ANALYSIS.md)

### Documents

#### [TESTENGINE_QUICK_DECISION_GUIDE.md](TESTENGINE_QUICK_DECISION_GUIDE.md)
**Purpose:** Fast decision-making with visual aids  
**Read if:** You want a quick TL;DR and clear recommendation  
**Time:** 5-10 minutes  

**Contents:**
- Visual comparison of both approaches
- One-sentence summaries
- Decision factors matrix
- Real-world debugging examples
- Common objections addressed

**Recommendation:** Approach 1 (Snapshot Copying) ✅

---

#### [TESTENGINE_IMPLEMENTATION_ROADMAP.md](TESTENGINE_IMPLEMENTATION_ROADMAP.md)
**Purpose:** Step-by-step implementation guide  
**Read if:** You're ready to implement the data bank  
**Time:** Reference document (implementation: 7-17 hours)  

**Contents:**
- 4-phase implementation plan
- Detailed code examples for each phase
- Unit and integration test strategies
- Common issues and solutions
- Performance targets
- Validation checklist

**Phases:**
1. Basic Data Bank Structure (2-4 hours)
2. Snapshot Capture Integration (2-3 hours)
3. Test Harness Comparison (3-4 hours)
4. Enhanced Snapshots - Optional (4-6 hours)

---

#### [TESTENGINE_SIMULATION_ANALYSIS.md](TESTENGINE_SIMULATION_ANALYSIS.md)
**Purpose:** Comprehensive architectural analysis  
**Read if:** You want to understand the full rationale  
**Time:** 20-30 minutes  

**Contents:**
- Problem statement and background
- Detailed analysis of Approach 1 (Snapshot Copying)
- Detailed analysis of Approach 2 (Per-Tick Matcher)
- Pros/cons comparison matrix
- Memory and performance analysis
- Implementation strategies
- Code examples and usage scenarios
- Future enhancements
- Edge cases and considerations

**Conclusion:** Approach 1 is superior due to:
- Better separation of concerns
- Superior debugging capability
- Lower coupling
- Alignment with existing patterns
- Negligible memory overhead for tests

---

## Summary

### The Problem

TestEngine needs to:
1. Run data through multi-tick simulations
2. Compare EntityMemoryPool state tick-by-tick
3. Provide clear failure diagnostics

### Two Approaches Analyzed

**Approach 1: Snapshot Copying** ✅ RECOMMENDED
- Copy EntityMemoryPool at each tick
- Store in data bank (map<tick, EntityMemoryPool>)
- Compare all snapshots at the end using matchers

**Approach 2: Per-Tick Matcher** ❌ NOT RECOMMENDED
- Pass EMP by reference during execution
- Inject comparison engine into TestEngine
- Compare immediately at each tick

### Key Decision Factors

| Factor | Approach 1 | Approach 2 |
|--------|-----------|-----------|
| **Separation of Concerns** | ✅ Excellent | ❌ Poor |
| **Debugging** | ✅ Full history | ❌ Limited |
| **Coupling** | ✅ Low | ❌ High |
| **Memory** | ⚠️ Higher (negligible) | ✅ Lower |
| **Flexibility** | ✅ High | ❌ Low |
| **Maintenance** | ✅ Easy | ❌ Complex |

### Memory Impact

- Typical test (5 ticks, 50 entities): **~125 KB** ← trivial
- Stress test (100 ticks, 1000 entities): **~50 MB** ← acceptable

**Conclusion:** Memory overhead is not a concern for test infrastructure.

### Implementation Timeline

**Core Functionality (Phases 1-3):** 7-11 hours
- Add data bank to TestEngine
- Implement snapshot capture
- Integrate with test harness

**Enhanced Snapshots (Phase 4):** +4-6 hours (optional)
- Add EventBus state
- Add Scene information
- Enhanced comparison

**Total Effort:** Low complexity, well-defined path

### Next Steps

1. Review the quick decision guide
2. If approved, follow the implementation roadmap
3. Start with Phase 1 (Basic Data Bank Structure)
4. Iterate through phases with tests
5. Update related documentation

## Related Documentation

- `tests/harness/README.md` - Test harness overview (mentions "data bank")
- `tests/matchers/EntityMemoryPoolEqualsMatcher.h` - Existing matcher infrastructure
- `src/types/test_structs/TestData.h` - TestData with expected_engine_snapshots
- `tests/context/test_context.h` - TestContext for enriched error messages

## Questions?

The documentation is comprehensive and self-contained. Each document serves a specific purpose:
- **Quick Guide** for fast decision-making
- **Roadmap** for implementation guidance
- **Analysis** for deep understanding

All three documents converge on the same recommendation: **Approach 1 (Snapshot Copying)**.
