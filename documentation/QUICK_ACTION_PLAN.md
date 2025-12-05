# Quick Action Plan - SteamRot Improvements

**Generated**: December 5, 2025  
**Based On**: Current State Analysis 2025

---

## Executive Summary

This document provides **actionable steps** to address identified issues in the SteamRot codebase. Each plan includes estimated hours, prerequisites, and acceptance criteria.

**Total Estimated Effort**: 18-26 hours for immediate priorities

---

## Immediate Actions (Do This Week)

### Action 1: Archive Completed Proposals

**Effort**: 30 minutes  
**Priority**: HIGH  
**Blocks**: Nothing, improves clarity

#### Steps

```bash
# 1. Create archive directories
mkdir -p documentation/proposals/archive/completed
mkdir -p documentation/proposals/archive/rejected

# 2. Move completed proposals
cd documentation/proposals

# Data Loading Interface (completed Dec 2024)
mv DATA_LOADING_INTERFACE_*.md archive/completed/

# Tick-by-Tick Comparison (completed Nov 2025)
mv TICK_BY_TICK_COMPARISON_*.md archive/completed/

# Entity Memory Pool (completed Nov 2025)
mv ENTITY_MEMORY_POOL_COPYABILITY.md archive/completed/

# 3. Create archive README
cat > archive/completed/README.md << 'EOF'
# Completed Proposals Archive

These proposals have been implemented and are kept for historical reference.

## Data Loading Interface System (December 2024)
- Phase 1-3 completed
- Provider pattern implemented for 5 data types
- Phase 4 (entity configuration) deferred by design

## Tick-by-Tick Comparison (November 2025)
- Test harness tick snapshot feature
- EntityMemoryPool comparison with metadata
- Fully implemented and documented

## Entity Memory Pool Copyability (November 2025)
- Clone method pattern implemented
- Deep copying of polymorphic UIElement hierarchy
- Enables state comparison testing
EOF
```

**Acceptance Criteria**:
- [ ] Archive directories created
- [ ] 11 proposal files moved to archive
- [ ] Archive README created
- [ ] Main proposals/ directory only has active proposals

---

### Action 2: Create Active Proposals Index

**Effort**: 30 minutes  
**Priority**: HIGH  
**Depends On**: Action 1

#### Create File

```markdown
# documentation/proposals/ACTIVE.md

# Active Proposals

**Last Updated**: December 5, 2025

## Under Consideration

### 1. Unified Tick Architecture
- **File**: UNIFIED_TICK_ARCHITECTURE.md + UNIFIED_TICK_ARCHITECTURE_VISUALS.md
- **Status**: Proposal - Under Review
- **Size**: Large (149KB combined)
- **Effort**: 40-60 hours (2-3 weeks)
- **Priority**: LOW (deferred)
- **Issue**: GameEngine and TestEngine have different tick execution order
- **Decision Needed**: Q1 2026

**Recommendation**: DEFER - System works well, large effort, low urgency

### 2. Engine Architecture Improvements
- **File**: ENGINE_ARCHITECTURE_IMPROVEMENTS.md
- **Status**: Proposal - Under Review
- **Size**: Medium (38KB)
- **Effort**: 20-30 hours
- **Priority**: LOW (deferred)
- **Issue**: Method naming clarity, extensibility patterns
- **Decision Needed**: Q2 2026

**Recommendation**: DEFER - Nice-to-have improvements, not blocking work

## Proposal Guidelines

### When to Create a Proposal

Create a proposal for:
- Architectural changes affecting multiple systems
- Breaking changes to public APIs
- New major features (>20 hours effort)
- Patterns that will be reused across codebase

### When NOT to Create a Proposal

Don't create proposals for:
- Bug fixes
- Small refactorings (<5 hours)
- Documentation updates
- Test additions
- Minor feature additions

### Proposal Template

See `TEMPLATE.md` for the standard proposal structure.

## Recently Completed

Completed proposals have been moved to `archive/completed/`:

1. **Data Loading Interface System** (Dec 2024)
   - Provider pattern for data loading
   - 5 providers implemented
   - Phase 4 deferred

2. **Tick-by-Tick Comparison** (Nov 2025)
   - Test harness tick snapshots
   - Enhanced debugging

3. **Entity Memory Pool Copyability** (Nov 2025)
   - Clone pattern for polymorphic copying
   - Enables state comparison

See `archive/completed/README.md` for details.
```

**Acceptance Criteria**:
- [ ] ACTIVE.md created
- [ ] All active proposals listed with status
- [ ] Decision timelines documented
- [ ] Guidelines for future proposals included

---

### Action 3: Fix ADDING_LOGIC.md

**Effort**: 2 hours  
**Priority**: HIGH (developer workflow doc)  
**Depends On**: None

#### Find and Replace

**File**: `documentation/workflows/ADDING_LOGIC.md`

**Changes needed** (4 locations):

1. **Line 159**: Update example
```markdown
# BEFORE:
steamrot::tests::TestContext test_context;

# AFTER:
steamrot::tests::TestFixture fixture;
fixture.Initialize();
```

2. **Line 165**: Update method call
```markdown
# BEFORE:
steamrot::NewLogic new_logic(
    test_context.GetLogicContextForTestScene());

# AFTER:
auto& game_context = fixture.GetGameContext();
auto& scene_context = fixture.GetSceneContext();
// Pass appropriate context to Logic constructor
```

3. **Line 176**: Update logic initialization
```markdown
# BEFORE:
auto logic_context = test_context.GetLogicContextForTestScene();

# AFTER:
auto& scene_context = fixture.GetSceneContext();
// Use scene_context members to build LogicContext
```

4. **Line 224**: Update LogicFactory test
```markdown
# BEFORE:
steamrot::tests::TestContext test_context{
    steamrot::SceneType::SceneType_YOUR_SCENE};

# AFTER:
steamrot::tests::TestFixture fixture(
    steamrot::SceneType::SceneType_YOUR_SCENE);
fixture.Initialize();
```

**Also Add**: Section on TestEngine for integration testing

```markdown
## Testing Logic with TestEngine (Integration Testing)

For multi-tick or data-driven testing, use TestEngine:

\`\`\`cpp
TEST_CASE("Test Logic with TestEngine", "[integration][NewLogic]") {
  // 1. Load test configurations
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  // 2. Use Catch2 generator for each config
  const auto* config = GENERATE_COPY(from_range(configs.value()));
  
  // 3. Run test - TestEngine handles execution
  auto result = steamrot::tests::RunTestEngineTest(config);
  REQUIRE(result.has_value());
}
\`\`\`

See [Testing Decision Guide](../testing/TESTING_DECISION_GUIDE.md) for
when to use TestFixture vs TestEngine.
```

**Acceptance Criteria**:
- [ ] All TestContext references replaced with TestFixture
- [ ] Examples compile and work correctly
- [ ] TestEngine section added
- [ ] Link to decision guide included

---

### Action 4: Create Testing Decision Guide

**Effort**: 3 hours  
**Priority**: HIGH  
**Depends On**: None

#### Create File

```markdown
# documentation/testing/TESTING_DECISION_GUIDE.md

# Testing Decision Guide

**Purpose**: Choose the right testing approach for your situation

---

## Quick Decision Tree

\`\`\`
Need to test something?
    │
    ├─ Single class/function? ────────────► Use TestFixture
    │
    └─ Multiple components/ticks? ────────► Consider TestEngine
        │
        ├─ Simple scenario (<3 ticks)? ───► Use TestFixture
        │
        └─ Complex scenario (3+ ticks)? ──► Use TestEngine
\`\`\`

---

## TestFixture: Unit Testing

### When to Use

✓ Testing a single Logic class  
✓ Testing a single function  
✓ Testing component behavior  
✓ Fast feedback (<1ms per test)  
✓ Simple state setup  
✓ Direct assertions

### Example

\`\`\`cpp
TEST_CASE("UIActionLogic handles button click", "[unit][UIActionLogic]") {
  // 1. Setup test fixture
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  
  // 2. Get contexts
  auto& game_context = fixture.GetGameContext();
  auto& scene_context = fixture.GetSceneContext();
  
  // 3. Setup test data
  // ... create button entity, etc.
  
  // 4. Create Logic instance
  steamrot::UIActionLogic logic(/* LogicContext */);
  
  // 5. Execute and assert
  logic.RunLogic();
  REQUIRE(button_clicked == true);
}
\`\`\`

### Pros
- Fast execution
- Simple setup
- Direct control
- Easy debugging
- Immediate feedback

### Cons
- Manual setup required
- Limited to single tick
- Can't easily test sequences
- No data-driven testing

---

## TestEngine: Integration/Data-Driven Testing

### When to Use

✓ Testing multiple components together  
✓ Testing multi-tick scenarios  
✓ Testing state changes over time  
✓ Testing event sequences  
✓ Data-driven test cases  
✓ Comparing tick snapshots

### Example

\`\`\`cpp
TEST_CASE("UI interaction sequence", "[integration][ui]") {
  // 1. Load test data configurations
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  // 2. Use Catch2 generator - runs for each config
  const auto* config = GENERATE_COPY(from_range(configs.value()));
  
  // 3. Run test - TestEngine handles everything
  auto result = steamrot::tests::RunTestEngineTest(config);
  REQUIRE(result.has_value());
}
\`\`\`

### Pros
- Data-driven testing
- Multi-tick scenarios
- Tick snapshot comparison
- JSON configuration
- Reusable test data

### Cons
- Slower execution
- More complex setup (JSON files)
- Indirect assertions (via matchers)
- Harder to debug
- Requires FlatBuffers understanding

---

## Comparison Table

| Feature | TestFixture | TestEngine |
|---------|-------------|------------|
| **Execution Speed** | Very Fast (<1ms) | Slower (10-100ms) |
| **Setup Complexity** | Simple (C++ code) | Complex (JSON files) |
| **Test Scope** | Single class/function | Multiple systems |
| **Tick Support** | Single tick only | Multi-tick sequences |
| **Data-Driven** | No | Yes (JSON configs) |
| **Debugging** | Easy (direct code) | Harder (via config) |
| **Maintenance** | Low (inline code) | Higher (separate files) |
| **Reusability** | Low | High (share configs) |

---

## Migration Path

### Starting Point: TestFixture
Most tests should start with TestFixture for simplicity.

### When to Migrate to TestEngine

Migrate when you need:
1. **Multiple ticks** - State changes over 3+ iterations
2. **Data-driven** - Same test with many configurations
3. **Snapshot comparison** - Validate state at specific ticks
4. **Event sequences** - Complex input/event patterns

### How to Migrate

1. Extract test setup to JSON
2. Create test_data.json file
3. Build with CMake (generates .test_data.bin)
4. Replace test with TestEngine version
5. Verify results match

---

## Common Patterns

### Pattern 1: Logic Class Unit Test (TestFixture)

\`\`\`cpp
TEST_CASE("Logic construction", "[unit][MyLogic]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  
  steamrot::MyLogic logic(/* context */);
  REQUIRE_NOTHROW(logic.RunLogic());
}
\`\`\`

### Pattern 2: Multi-tick Scenario (TestEngine)

\`\`\`json
{
  "metadata": {
    "test_name": "multi_tick_test",
    "description": "Test state changes over time"
  },
  "simulation_data": {
    "num_ticks": 5
  },
  "tick_snapshots": [
    { "tick": 1, "expected_entity_collection": { ... } },
    { "tick": 5, "expected_entity_collection": { ... } }
  ]
}
\`\`\`

### Pattern 3: Event Sequence Test (TestEngine)

\`\`\`json
{
  "simulation_data": {
    "event_sequence": [
      {
        "tick": 1,
        "events": [
          {
            "event_packet": {
              "event_type": "BUTTON_CLICKED",
              "data": "button_1"
            }
          }
        ]
      }
    ]
  }
}
\`\`\`

---

## Troubleshooting

### "My TestFixture test is too complex"
→ Consider using TestEngine with JSON config

### "My TestEngine test is slow"
→ Consider breaking into smaller TestFixture tests

### "I need to test just one tick with complex setup"
→ Use TestFixture with helper functions

### "I need to test sequences but TestEngine is overkill"
→ Use TestFixture with a loop

---

## See Also

- [Testing Overview](TESTING_OVERVIEW.md)
- [TestFixture Guide](TESTFIXTURE_GUIDE.md)
- [TestEngine Quick Start](TESTENGINE_QUICKSTART.md)
- [Test Data Configuration](TEST_DATA_CONFIGURATION.md)
```

**Acceptance Criteria**:
- [ ] Decision guide created
- [ ] Clear examples for both approaches
- [ ] Comparison table included
- [ ] Migration path documented

---

### Action 5: Create Architecture Current State Doc

**Effort**: 4 hours  
**Priority**: HIGH  
**Depends On**: Actions 1-2

#### Create File

```markdown
# documentation/architecture/ARCHITECTURE_CURRENT_STATE.md

# Architecture Current State

**Last Updated**: December 5, 2025  
**Type**: Reference Documentation

---

## Purpose

This document describes **what is implemented right now**, not what is planned
or proposed. For proposals, see `documentation/proposals/`.

---

## What's Fully Implemented

### 1. Engine Class Hierarchy ✅

```
Engine (abstract base)
  ├── GameEngine (production)
  └── TestEngine (testing)
```

**Status**: Complete and working well

**Key Features**:
- Shared GameCore, SceneManager ownership
- Virtual hook pattern for specialization
- Common StartUp() flow
- Different loop execution (SFML vs fixed ticks)

**Documentation**: 
- `documentation/architecture/GAME_LOOP.md`
- `documentation/testing/TESTING_HARNESS_LOOP.md`

---

### 2. Entity-Component-Archetype System ✅

**Status**: Complete and performant

**Components**:
- Pure data containers (17 component types)
- No logic in components
- Registered in ComponentRegister tuple

**Archetypes**:
- Efficient grouping by active components
- ArchetypeManager handles organization
- Quick iteration over entities with specific components

**Documentation**:
- `documentation/workflows/ADDING_COMPONENTS.md`
- Component contract tests in place

---

### 3. Logic System ✅

**Status**: Complete with both classes and free functions

**Logic Classes** (6 total):
1. UIEventActionLogic ✅ (has unit tests)
2. UIActionLogic ⚠️ (no unit tests)
3. UICollisionLogic ⚠️ (no unit tests)
4. UIRenderLogic ⚠️ (no unit tests)
5. PlayerActionLogic ⚠️ (no unit tests)
6. CraftingLogic ⚠️ (no unit tests)

**Free Function Modules** (3 total):
- logic_ui.h/.cpp ⚠️ (partial coverage)
- logic_action.h/.cpp ⚠️ (partial coverage)
- logic_collision.h/.cpp ⚠️ (partial coverage)

**LogicFactory**: Creates appropriate Logic instances per scene

**Documentation**:
- `documentation/workflows/ADDING_LOGIC.md`
- `documentation/architecture/LOGIC_SYSTEM.md`

---

### 4. Provider Pattern (Partial) ⚠️

**Status**: 5 of 6 providers implemented

**Implemented Providers**:

1. ✅ **IEngineDataProvider**
   - Native struct: EngineCoreData
   - Implementation: FlatbuffersEngineDataProvider
   - Used by: Engine.cpp

2. ✅ **ISceneDataProvider**
   - Native struct: SceneCoreData
   - Implementation: FlatbuffersSceneDataProvider
   - Used by: SceneFactory.cpp

3. ✅ **IAssetDataProvider**
   - Native struct: AssetData
   - Implementation: FlatbuffersAssetDataProvider
   - Used by: AssetManager.cpp

4. ✅ **IFragmentDataProvider**
   - Native struct: Fragment (already existed)
   - Implementation: FlatbuffersFragmentDataProvider
   - Used by: Various

5. ✅ **IGameConfigProvider**
   - ⚠️ Returns FlatBuffers types (temporary)
   - Implementation: FlatbuffersGameConfigProvider
   - Used by: GameEngine.cpp

**Not Implemented**:

6. ❌ **IUIStyleProvider** - StylesConfigurator uses FlatbuffersDataLoader directly
   - **Decision**: Deferred - low priority, works fine
   - **Effort to add**: ~8 hours
   - **See**: "What's Deferred" section

**Documentation**:
- `documentation/DATA_PROVIDER_SYSTEM.md`
- `documentation/proposals/archive/completed/DATA_LOADING_INTERFACE_*.md`

---

### 5. Test Infrastructure ✅

**Status**: Two complementary approaches available

**Approach 1: TestFixture** (Unit Testing)
- Provides GameContext/SceneContext for tests
- Owns GameCore, SceneCore, EntityManager
- Simple C++ code setup
- Fast execution
- Used by: Most unit tests

**Approach 2: TestEngine** (Integration/Data-Driven Testing)
- Extends Engine base class
- JSON-configured test scenarios
- Multi-tick execution
- Tick snapshot comparison
- Data bank for state capture
- Used by: Integration tests

**Documentation**:
- `documentation/testing/TESTING_DECISION_GUIDE.md` ⚠️ (to be created)
- `documentation/testing/TESTING_OVERVIEW.md`
- `tests/harness/README.md`
- `tests/context/TestFixture.h`

---

## What's Partially Implemented

### 1. FlatBuffers Migration ⚠️

**Goal**: Abstract data format from game code

**Status**: 80% complete

**What's Using Providers**:
- ✅ Engine core configuration
- ✅ Scene core configuration
- ✅ Asset loading
- ✅ Fragment loading
- ✅ Game config (returns FlatBuffers temporarily)

**What's NOT Using Providers**:
- ❌ Entity/Component configuration (FlatbuffersConfigurator)
- ❌ UI style loading (StylesConfigurator)

**Why Not Complete**:
- Entity config is complex (polymorphic UIElements, deep hierarchy)
- Estimated 4-6 weeks effort for Phase 4
- 80% of benefits achieved
- Remaining 20% is nice-to-have

**Decision**: Intentionally deferred (see "What's Deferred")

---

### 2. Test Coverage ⚠️

**Status**: Good foundation, gaps in Logic tests

**Coverage by Area**:
- Components: 29% (5 of 17 have tests)
- Logic Classes: 17% (1 of 6 have dedicated tests)
- Free Functions: ~50% (partial coverage)
- Entity System: 31% (4 of 13 files tested)
- Events: Good (6 test files)

**Plan**: See "Step-by-Step Improvement Plans" in analysis doc

---

## What's Planned (Active Proposals)

### 1. Unified Tick Architecture 🔮

**Status**: Proposal under review

**Issue**: GameEngine and TestEngine have different event ordering

**Effort**: 40-60 hours (2-3 weeks)

**Priority**: LOW - System works, large effort, deferred

**Document**: `documentation/proposals/UNIFIED_TICK_ARCHITECTURE.md`

**Decision Needed**: Q1 2026

---

### 2. Engine Architecture Improvements 🔮

**Status**: Proposal under review

**Issue**: Method naming clarity, extensibility patterns

**Effort**: 20-30 hours

**Priority**: LOW - Nice-to-have, not blocking

**Document**: `documentation/proposals/ENGINE_ARCHITECTURE_IMPROVEMENTS.md`

**Decision Needed**: Q2 2026

---

## What's Deferred (Intentional)

### 1. Entity Configuration Provider Pattern 🚫

**Reason**: High effort, low incremental benefit

**Current**: FlatbuffersConfigurator uses FlatbuffersDataLoader directly

**To Implement Would Need**:
1. IEntityDataProvider interface
2. Native EntityData structs
3. Conversion of polymorphic UIElement hierarchy
4. Extensive refactoring

**Effort**: 4-6 weeks (HIGH RISK)

**Benefits Already Achieved**:
- 80% of provider pattern benefits
- Main game systems use providers
- Can mock most data for testing

**When to Revisit**:
- If need to support JSON entity format
- If testing pain points emerge
- After higher priority items done

**Workaround**: TestDataConfig allows injecting entity data for tests

---

### 2. UI Style Provider 🚫

**Reason**: Low priority, current system works

**Current**: StylesConfigurator uses FlatbuffersDataLoader directly

**To Implement Would Need**:
1. IUIStyleProvider interface
2. Native UIStyleData struct
3. Move logic from StylesConfigurator

**Effort**: ~8 hours

**Decision**: Not worth the effort, mark as intentional exception

---

### 3. Alternative Format Providers (JSON/XML/Lua) 🚫

**Reason**: No current requirement

**Current**: Only FlatBuffers implementations exist

**To Implement Would Need**:
- JsonEngineDataProvider
- JsonSceneDataProvider
- etc.

**Benefits**: 
- Human-readable configs
- Easier modding
- Hot-reload during development

**Decision**: Implement when/if needed, architecture supports it

---

## Design Decisions

### Why Two Test Approaches?

**Question**: Why have both TestFixture and TestEngine?

**Answer**: Different use cases

- **TestFixture**: Fast, simple, direct - for unit tests
- **TestEngine**: Data-driven, multi-tick - for integration tests

Not redundant - complementary tools.

---

### Why Not Complete Provider Migration?

**Question**: Why leave entity config using FlatBuffersDataLoader?

**Answer**: Diminishing returns

- 5 providers give 80% of benefits
- Entity config is 20% of remaining work but 80% of effort
- System works well as-is
- Can revisit if needs change

---

### Why Keep Proposals After Implementation?

**Question**: Why not delete completed proposals?

**Answer**: Historical context

- Show decision-making process
- Explain why things are the way they are
- Reference for future similar changes

Moved to archive/completed/ to reduce clutter.

---

## See Also

### Architecture Documentation
- [Game Loop](GAME_LOOP.md)
- [Logic System](LOGIC_SYSTEM.md)
- [Error Handling](ERROR_HANDLING.md)

### Workflow Guides
- [Adding Components](../workflows/ADDING_COMPONENTS.md)
- [Adding Logic](../workflows/ADDING_LOGIC.md)
- [Adding UI Elements](../workflows/ADDING_UI_ELEMENTS.md)

### Proposals
- [Active Proposals](../proposals/ACTIVE.md)
- [Completed Proposals](../proposals/archive/completed/README.md)

### Analysis
- [Current State Analysis](../analysis/CURRENT_STATE_ANALYSIS_2025.md)
- [Comprehensive Analysis](../analysis/COMPREHENSIVE_CODEBASE_ANALYSIS.md)
- [Architecture Diagrams](../analysis/ARCHITECTURE_DIAGRAMS.md)
```

**Acceptance Criteria**:
- [ ] Current state doc created
- [ ] All systems categorized (implemented/partial/planned/deferred)
- [ ] Design decisions documented
- [ ] Clear status for each area

---

## Next Week Actions

### Action 6: Update Main README

**Effort**: 1 hour  
**Priority**: MEDIUM  
**Depends On**: Actions 1-5

Add section to main README.md:

```markdown
## Architecture Status

The SteamRot engine is feature-complete with some intentional gaps:

- ✅ **Engine System**: GameEngine + TestEngine working
- ✅ **ECS**: Complete entity-component-archetype system
- ✅ **Logic**: 6 Logic classes + free functions (needs more tests)
- ⚠️ **Data Loading**: 5 of 6 providers implemented (entity config deferred)
- ✅ **Testing**: TestFixture (unit) + TestEngine (integration)

**Current State**: See [Architecture Current State](documentation/architecture/ARCHITECTURE_CURRENT_STATE.md)

**Proposals**: See [Active Proposals](documentation/proposals/ACTIVE.md)

**Improvements**: See [Quick Action Plan](documentation/QUICK_ACTION_PLAN.md)
```

---

### Action 7: Add Missing Unit Tests

**Effort**: 15-20 hours total (3-4 hours per Logic class)  
**Priority**: MEDIUM  
**Depends On**: Action 3 (for updated examples)

For each Logic class without tests:

1. UIActionLogic.test.cpp
2. UICollisionLogic.test.cpp
3. UIRenderLogic.test.cpp
4. PlayerActionLogic.test.cpp
5. CraftingLogic.test.cpp

**Pattern** (use TestFixture):

```cpp
#include "UIActionLogic.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UIActionLogic construction", "[unit][UIActionLogic]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  
  // Create LogicContext from fixture contexts
  // ...
  
  steamrot::UIActionLogic logic(/* context */);
  SUCCEED("UIActionLogic instantiated successfully");
}

TEST_CASE("UIActionLogic processes button clicks", "[unit][UIActionLogic]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  
  // Setup: Create button entity
  // ...
  
  // Execute: Run logic
  logic.RunLogic();
  
  // Assert: Button clicked
  REQUIRE(button_clicked == true);
}
```

**Acceptance Criteria (per test file)**:
- [ ] Construction test passes
- [ ] At least 2 behavior tests added
- [ ] Test follows existing patterns
- [ ] All tests pass

---

## Summary Checklist

### Week 1 (8-12 hours total)

- [ ] Action 1: Archive proposals (30 min)
- [ ] Action 2: Create active proposals index (30 min)
- [ ] Action 3: Fix ADDING_LOGIC.md (2 hours)
- [ ] Action 4: Create testing decision guide (3 hours)
- [ ] Action 5: Create architecture current state (4 hours)

### Week 2 (6-8 hours total)

- [ ] Action 6: Update main README (1 hour)
- [ ] Action 7: Start unit test additions (5-7 hours this week)

### Weeks 3-4 (10-15 hours remaining)

- [ ] Action 7: Complete unit test additions
- [ ] Review and polish documentation
- [ ] Validate all links work

---

## Success Metrics

After completing these actions:

1. **Documentation Clarity**
   - [ ] No confusion about TestContext vs TestFixture
   - [ ] Clear when to use each test approach
   - [ ] Active proposals clearly marked
   - [ ] Completed work archived

2. **Developer Onboarding**
   - [ ] New developer can choose test approach
   - [ ] Workflow guides have correct examples
   - [ ] Architecture state is clear

3. **Test Coverage**
   - [ ] All Logic classes have unit tests
   - [ ] Confidence to refactor Logic code
   - [ ] Foundation for integration tests

4. **Documentation Health**
   - [ ] Proposals directory < 100KB
   - [ ] No outdated technical examples
   - [ ] Single source of truth for current state

---

## Not Included (Deferred)

These items are intentionally NOT in this action plan:

- ❌ Complete entity configuration provider
- ❌ Implement unified tick architecture
- ❌ Add JSON format providers
- ❌ Create UI style provider
- ❌ Extensive refactoring

**Reason**: High effort, low incremental benefit, not blocking work

---

## Questions?

- **When to do Action 7?** After documentation cleanup (Actions 1-6)
- **Can I skip some actions?** Actions 1-5 are high priority, 6-7 can wait
- **What if I find more issues?** Update this doc, don't start new work immediately

---

**End of Quick Action Plan**

See [Current State Analysis](analysis/CURRENT_STATE_ANALYSIS_2025.md) for detailed rationale.
