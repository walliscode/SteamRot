# Logic Test Refactoring - Documentation Index

## Quick Start

**New to the plan?** Start here:
1. Read [LOGIC_TEST_REFACTORING_SUMMARY.md](LOGIC_TEST_REFACTORING_SUMMARY.md) (5 min)
2. Review [Template README](../tests/unit/logic/data/templates/README.md) (10 min)
3. Try migrating one test using a template (30 min)
4. Consult [Full Plan](LOGIC_TEST_REFACTORING_PLAN.md) for detailed guidance

**Need templates?** Go to: `tests/unit/logic/data/templates/`

## Documentation Structure

### Planning Documents

| Document | Purpose | When to Read |
|----------|---------|-------------|
| [LOGIC_TEST_REFACTORING_SUMMARY.md](LOGIC_TEST_REFACTORING_SUMMARY.md) | Executive summary, quick reference | First - overview and quick start |
| [LOGIC_TEST_REFACTORING_PLAN.md](LOGIC_TEST_REFACTORING_PLAN.md) | Complete 6-stage plan with examples | Reference during migration |
| [This file](LOGIC_TEST_REFACTORING_INDEX.md) | Navigation guide | When lost or looking for specific info |

### Templates and Usage

| Resource | Purpose | When to Use |
|----------|---------|------------|
| [Template README](../tests/unit/logic/data/templates/README.md) | Template usage guide | Creating new test data files |
| [simple_logic_test.template.json](../tests/unit/logic/data/templates/simple_logic_test.template.json) | Single Logic class template | Simple, straightforward tests |
| [multi_step_workflow.template.json](../tests/unit/logic/data/templates/multi_step_workflow.template.json) | Multi-step Logic template | Workflow and integration tests |
| [with_input_simulation.template.json](../tests/unit/logic/data/templates/with_input_simulation.template.json) | Input simulation template | Mouse/keyboard interaction tests |

### Related Documentation

| Document | Relevance |
|----------|-----------|
| [TEST_HARNESS_ADOPTION_PLAN.md](TEST_HARNESS_ADOPTION_PLAN.md) | Overall test harness adoption strategy (Phase 2.3 = this plan) |
| [TEST_DATA_CONFIGURATION.md](TEST_DATA_CONFIGURATION.md) | Test data system documentation |
| [tests/harness/README.md](../tests/harness/README.md) | Test harness API reference |

## Common Questions

### "How do I get started?"

1. Read the [executive summary](LOGIC_TEST_REFACTORING_SUMMARY.md)
2. Pick a simple test to migrate (e.g., constructor test)
3. Copy `simple_logic_test.template.json` to `tests/unit/logic/data/`
4. Customize it following the [template README](../tests/unit/logic/data/templates/README.md)
5. Build and test: `cmake --build --preset Debug && ctest --preset Debug -R logic`

### "What template should I use?"

**Single Logic class execution?** → `simple_logic_test.template.json`
- Basic collision detection
- Simple rendering
- Single action processing

**Multiple Logic classes in sequence?** → `multi_step_workflow.template.json`
- Button click workflows
- Multi-step UI interactions
- Logic dependency chains

**Need mouse/keyboard input?** → `with_input_simulation.template.json`
- Click interactions
- Keyboard input
- Drag-and-drop
- Multi-frame scenarios

### "Where do I find examples?"

**In templates:**
- See `tests/unit/logic/data/templates/*.template.json`

**In test harness:**
- See `tests/harness/data/*.test_data.json`
- Especially `sample_simulation_test.test_data.json`
- And `sample_complete_workflow.test_data.json`

**In full plan:**
- [LOGIC_TEST_REFACTORING_PLAN.md](LOGIC_TEST_REFACTORING_PLAN.md) has Before/After examples
- See "Migration Patterns" section

### "What's the timeline?"

**6 weeks total:**
- Week 1: Setup and POC (Stage 0)
- Week 2: Simple tests (Stage 1) 
- Week 3: Multi-step workflows (Stage 2)
- Week 4: Input/event integration (Stage 3)
- Week 5: Edge cases (Stage 4)
- Week 6: Cleanup and finalization (Stage 5)

See [full timeline](LOGIC_TEST_REFACTORING_PLAN.md#timeline) for details.

### "How do I validate my test data?"

**Build and test:**
```bash
cmake --build --preset Debug
ctest --preset Debug -R logic
```

**Check test output:**
- Use `INFO("Test: " << config->metadata()->test_name()->str());`
- Failures show detailed entity comparisons
- See template README for debugging tips

### "What if my test is too complex?"

**Options:**
1. **Break it down** - Split into multiple smaller tests
2. **Use multi-step** - Leverage `multi_step_workflow.template.json`
3. **Add input simulation** - Use `with_input_simulation.template.json`
4. **Keep it manual** - Some tests may be too complex; that's OK
5. **Extend harness** - Propose new features if needed

Consult the [risk mitigation section](LOGIC_TEST_REFACTORING_PLAN.md#risks-and-mitigations) for guidance.

## Stage-Specific Guidance

### Stage 0: Preparation (Week 1)

**Focus:** Infrastructure setup and proof-of-concept

**Read:**
- [Executive summary](LOGIC_TEST_REFACTORING_SUMMARY.md)
- [Template README](../tests/unit/logic/data/templates/README.md)
- [Stage 0 section](LOGIC_TEST_REFACTORING_PLAN.md#stage-0-preparation-week-1)

**Do:**
- Set up `tests/unit/logic/data/` directory structure
- Copy and customize one template
- Migrate 1-2 simple tests as POC
- Validate approach with team

### Stage 1: Simple Logic Tests (Week 2)

**Focus:** Single Logic class execution

**Read:**
- [Stage 1 section](LOGIC_TEST_REFACTORING_PLAN.md#stage-1-simple-logic-tests-week-2)
- [Pattern 1: Simple Logic Execution](LOGIC_TEST_REFACTORING_PLAN.md#pattern-1-simple-logic-execution)

**Use:**
- `simple_logic_test.template.json`

**Migrate:**
- Constructor tests
- Basic collision tests
- Simple render tests
- Basic action tests

### Stage 2: Multi-Step Logic Sequences (Week 3)

**Focus:** Multiple Logic classes in sequence

**Read:**
- [Stage 2 section](LOGIC_TEST_REFACTORING_PLAN.md#stage-2-multi-step-logic-sequences-week-3)
- [Pattern 2: Multi-Step Workflow](LOGIC_TEST_REFACTORING_PLAN.md#pattern-2-multi-step-workflow)

**Use:**
- `multi_step_workflow.template.json`

**Migrate:**
- Collision → action workflows
- Collision → render → action sequences
- Multi-Logic interactions

### Stage 3: Input and Event Integration (Week 4)

**Focus:** Input sequences and event sequences

**Read:**
- [Stage 3 section](LOGIC_TEST_REFACTORING_PLAN.md#stage-3-input-and-event-integration-week-4)
- [Pattern 3: Input + Simulation](LOGIC_TEST_REFACTORING_PLAN.md#pattern-3-input--simulation)

**Use:**
- `with_input_simulation.template.json`

**Migrate:**
- Mouse interaction tests
- Keyboard input tests
- Event injection tests
- Tick-based scenarios

### Stage 4: Edge Cases and Negative Tests (Week 5)

**Focus:** Edge cases and error conditions

**Read:**
- [Stage 4 section](LOGIC_TEST_REFACTORING_PLAN.md#stage-4-edge-cases-and-negative-tests-week-5)

**Test:**
- Boundary conditions
- Invalid configurations
- Missing entities
- Error handling
- Use `expected_to_pass: false` for negative tests

### Stage 5: Cleanup and Optimization (Week 6)

**Focus:** Finalization

**Read:**
- [Stage 5 section](LOGIC_TEST_REFACTORING_PLAN.md#stage-5-cleanup-and-optimization-week-6)
- [Validation Criteria](LOGIC_TEST_REFACTORING_PLAN.md#validation-criteria)

**Do:**
- Remove old manual tests
- Consolidate similar tests
- Optimize test data
- Update documentation
- Performance testing

## Need Help?

### Documentation Issues
- Check [full plan](LOGIC_TEST_REFACTORING_PLAN.md) for detailed guidance
- Review [template README](../tests/unit/logic/data/templates/README.md) for usage patterns
- Consult [test harness API](../tests/harness/README.md)

### Technical Issues
- Review [test data configuration docs](TEST_DATA_CONFIGURATION.md)
- Check [test harness adoption plan](TEST_HARNESS_ADOPTION_PLAN.md)
- Look at existing examples in `tests/harness/data/`

### Process Questions
- See [risk mitigation](LOGIC_TEST_REFACTORING_PLAN.md#risks-and-mitigations)
- Review [validation criteria](LOGIC_TEST_REFACTORING_PLAN.md#validation-criteria)
- Check [timeline](LOGIC_TEST_REFACTORING_PLAN.md#timeline)

## Quick Links

**Essential Reading:**
- [Executive Summary →](LOGIC_TEST_REFACTORING_SUMMARY.md)
- [Full Plan →](LOGIC_TEST_REFACTORING_PLAN.md)
- [Template Guide →](../tests/unit/logic/data/templates/README.md)

**Templates:**
- [Simple Logic Test →](../tests/unit/logic/data/templates/simple_logic_test.template.json)
- [Multi-Step Workflow →](../tests/unit/logic/data/templates/multi_step_workflow.template.json)
- [Input Simulation →](../tests/unit/logic/data/templates/with_input_simulation.template.json)

**Reference:**
- [Test Harness API →](../tests/harness/README.md)
- [Test Data Configuration →](TEST_DATA_CONFIGURATION.md)
- [Adoption Plan Phase 2.3 →](TEST_HARNESS_ADOPTION_PLAN.md#23-logic-class-tests-with-simulations)

## Status Tracking

Track overall progress:
- [ ] Stage 0: Preparation (Week 1)
- [ ] Stage 1: Simple tests (Week 2)
- [ ] Stage 2: Multi-step sequences (Week 3)
- [ ] Stage 3: Input/event integration (Week 4)
- [ ] Stage 4: Edge cases (Week 5)
- [ ] Stage 5: Cleanup (Week 6)

Update checkboxes as stages complete in [LOGIC_TEST_REFACTORING_PLAN.md](LOGIC_TEST_REFACTORING_PLAN.md).
