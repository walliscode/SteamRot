# Logic Unit Testing Refactoring Documentation

## Overview

This directory contains comprehensive documentation for refactoring logic unit tests from manual setup to data-driven tests using the test harness system.

**Project:** SteamRot Game Engine
**Phase:** Test Harness Adoption - Phase 2.3
**Timeline:** 6 weeks (Stages 0-5)
**Status:** Planning Complete - Ready for Implementation

## 📚 Documentation Set

### Quick Start (Read First)

**New to this refactoring?**

1. **Start here:** [LOGIC_TEST_REFACTORING_SUMMARY.md](LOGIC_TEST_REFACTORING_SUMMARY.md)
   - 5-minute overview
   - Quick reference guide
   - Benefits and timeline

2. **Navigate:** [LOGIC_TEST_REFACTORING_INDEX.md](LOGIC_TEST_REFACTORING_INDEX.md)
   - Documentation map
   - Common questions
   - Stage-specific guidance

3. **Implement:** [Template Guide](../tests/unit/logic/data/templates/README.md)
   - Ready-to-use templates
   - Step-by-step instructions
   - Usage examples

### Complete Documentation

| Document | Purpose | Size | Audience |
|----------|---------|------|----------|
| [LOGIC_TEST_REFACTORING_SUMMARY.md](LOGIC_TEST_REFACTORING_SUMMARY.md) | Executive summary | 6KB | Everyone (start here) |
| [LOGIC_TEST_REFACTORING_INDEX.md](LOGIC_TEST_REFACTORING_INDEX.md) | Navigation guide | 9KB | All team members |
| [LOGIC_TEST_REFACTORING_PLAN.md](LOGIC_TEST_REFACTORING_PLAN.md) | Complete plan | 23KB | Implementers, reviewers |
| [LOGIC_TEST_MIGRATION_CHECKLIST.md](LOGIC_TEST_MIGRATION_CHECKLIST.md) | Progress tracker | 9KB | Migration team |
| [Template Guide](../tests/unit/logic/data/templates/README.md) | Template usage | 10KB | Developers |

**Total:** ~70KB of documentation + templates

### Templates

Located in `tests/unit/logic/data/templates/`:

- **simple_logic_test.template.json** - Single Logic class execution
- **multi_step_workflow.template.json** - Multi-step Logic sequences  
- **with_input_simulation.template.json** - Input simulation tests
- **README.md** - Comprehensive usage guide

## 🎯 What This Delivers

### The Change

**From:** Manual test setup (verbose, hard to maintain)
```cpp
TEST_CASE("Logic test") {
  steamrot::PathProvider pp{...};
  steamrot::tests::TestFixture fixture;
  // 20-30 lines of manual setup
  steamrot::MyLogic logic(...);
  logic.RunLogic();
  // Manual assertions
}
```

**To:** Data-driven tests (concise, maintainable)
```cpp
TEST_CASE("Logic tests", "[data-driven]") {
  auto configs = steamrot::tests::load_test_data_configs();
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

Plus JSON test data defining entities, simulation steps, and expected state.

### 6-Stage Plan

| Week | Stage | Focus | Output |
|------|-------|-------|--------|
| 1 | 0 | Setup & POC | Infrastructure + validation |
| 2 | 1 | Simple tests | 4-8 basic test files |
| 3 | 2 | Multi-step | 8-12 workflow files |
| 4 | 3 | Input/Events | 10-15 input sim files |
| 5 | 4 | Edge cases | 8-12 edge case files |
| 6 | 5 | Cleanup | Finalization |

### Benefits

- **~40% code reduction** - Less test code to maintain
- **Easier to add tests** - Just create JSON files
- **Better testing** - Multi-step workflows, input simulation, tick-based execution
- **Parameterization** - Use Catch2 generators
- **Maintainability** - Clear, reviewable test data
- **Alignment** - Fits TEST_HARNESS_ADOPTION_PLAN.md Phase 2.3

## 🚀 How to Use

### For Management

**Want a quick overview?**
→ Read [LOGIC_TEST_REFACTORING_SUMMARY.md](LOGIC_TEST_REFACTORING_SUMMARY.md)

**Key Points:**
- 6-week migration plan
- Clear deliverables per week
- Success metrics defined
- Risk mitigation included
- Aligns with overall adoption strategy

### For Developers

**Ready to migrate tests?**

1. Read [LOGIC_TEST_REFACTORING_INDEX.md](LOGIC_TEST_REFACTORING_INDEX.md)
2. Go to `tests/unit/logic/data/templates/`
3. Copy appropriate template
4. Customize following [Template README](../tests/unit/logic/data/templates/README.md)
5. Build and test

**Need help?**
- Check [Index](LOGIC_TEST_REFACTORING_INDEX.md) for common questions
- Consult [Full Plan](LOGIC_TEST_REFACTORING_PLAN.md) for detailed guidance
- Review [Template Guide](../tests/unit/logic/data/templates/README.md) for examples

### For Project Tracking

**Tracking progress?**
→ Use [LOGIC_TEST_MIGRATION_CHECKLIST.md](LOGIC_TEST_MIGRATION_CHECKLIST.md)

- Pre-migration checklist
- Stage-by-stage tasks
- Success metrics tracking
- Notes sections for learnings

## 📋 Implementation Readiness

### ✅ Planning Complete

All planning documentation created:
- [x] Complete 6-stage plan
- [x] Executive summary
- [x] Navigation index  
- [x] Migration checklist
- [x] Test templates (3)
- [x] Template usage guide
- [x] Integration with existing docs

### 🎯 Ready to Start

**Stage 0 can begin immediately:**
1. Review plan with team
2. Set up directory structure
3. Run proof-of-concept migration
4. Validate approach
5. Get team approval

**Then proceed through Stages 1-5.**

## 🔗 Related Documentation

### Context

- [TEST_HARNESS_ADOPTION_PLAN.md](../TEST_HARNESS_ADOPTION_PLAN.md) - Overall adoption strategy (this is Phase 2.3)
- [TEST_DATA_CONFIGURATION.md](../TEST_DATA_CONFIGURATION.md) - Test data system documentation
- [tests/harness/README.md](../../../tests/harness/README.md) - Test harness API reference

### Examples

- `tests/harness/data/*.test_data.json` - Working examples
- Especially: `sample_simulation_test.test_data.json`
- And: `sample_complete_workflow.test_data.json`

## 📊 Success Metrics

Track these throughout migration:

| Metric | Target | 
|--------|--------|
| Tests migrated | ≥90% |
| Code reduction | ≥40% |
| Coverage maintained | 100% |
| Performance | ≤110% of original |
| Test data files | > test code files |
| Team satisfaction | High |

See [Validation Criteria](LOGIC_TEST_REFACTORING_PLAN.md#validation-criteria) for details.

## 🎓 Learning Path

**Recommended reading order:**

1. **[Summary](LOGIC_TEST_REFACTORING_SUMMARY.md)** (5 min) - Get overview
2. **[Index](LOGIC_TEST_REFACTORING_INDEX.md)** (10 min) - Understand structure
3. **[Template Guide](../tests/unit/logic/data/templates/README.md)** (15 min) - Learn usage
4. **Try one migration** (30 min) - Hands-on practice
5. **[Full Plan](LOGIC_TEST_REFACTORING_PLAN.md)** (30 min) - Deep dive when needed
6. **[Checklist](LOGIC_TEST_MIGRATION_CHECKLIST.md)** - Use during execution

**Total time to get started:** ~30 minutes
**Total time to full understanding:** ~90 minutes

## 💡 Key Concepts

### Data-Driven Testing

Test behavior defined in JSON data files instead of C++ code:
- **Entities:** Configured in `start_entity_collection`
- **Actions:** Defined in `simulation_data` steps
- **Validation:** Compared with `expected_entity_collection`

### Test Harness Integration

Uses the test harness system (`tests/harness/`):
- **Fixture creation:** Automatic from test data
- **Entity config:** From JSON
- **Simulation:** Executes Logic classes/functions
- **Input/Events:** Simulates user interaction
- **Tick-based:** Precise timing control

### Migration Strategy

Progressive, staged approach:
- **Keep old tests** initially for validation
- **Validate at each stage** before proceeding
- **Clear criteria** for stage completion
- **Risk mitigation** for each identified risk

## 🤝 Team Collaboration

### Roles

- **Implementers:** Migrate tests using templates
- **Reviewers:** Validate migrations at each stage
- **Tech Lead:** Oversee progress, resolve blockers
- **Team:** Provide feedback, adopt approach

### Communication

- Review plan together before starting
- Regular check-ins during migration
- Document learnings as you go
- Share feedback on process

## 📝 Notes

### Design Decisions

- **JSON over C++:** More readable, easier to maintain
- **Staged approach:** Reduces risk, validates incrementally
- **Templates:** Accelerates migration, ensures consistency
- **Tick-based:** Enables complex timing scenarios

### Implementation Notes

- No build system changes needed
- Test harness already supports all required features
- Templates work with existing infrastructure
- Aligns with Phase 2.3 of adoption plan

## 🎉 Get Started

**Ready to begin?**

1. **Read:** [LOGIC_TEST_REFACTORING_SUMMARY.md](LOGIC_TEST_REFACTORING_SUMMARY.md)
2. **Navigate:** [LOGIC_TEST_REFACTORING_INDEX.md](LOGIC_TEST_REFACTORING_INDEX.md)
3. **Review:** Team walkthrough of plan
4. **Start:** Stage 0 - Preparation
5. **Execute:** Follow checklist

**Questions?** Consult the [Index](LOGIC_TEST_REFACTORING_INDEX.md) or [Full Plan](LOGIC_TEST_REFACTORING_PLAN.md).

---

**Document Version:** 1.0
**Last Updated:** 2025-11-09
**Status:** Planning Complete - Ready for Implementation
