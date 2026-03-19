# Integration Test Engine — Component Analysis & Roadmap

## Overview

This document analyses the current state of the SteamRot test engine
infrastructure and identifies the discrete components that must each reach a
viable state before end-to-end integration tests can be authored and run
reliably.

The test engine is not a single class. It is a pipeline of cooperating
components — data loading, engine initialisation, per-tick simulation, state
capture, and snapshot comparison — that must all function correctly before a
test author can write a meaningful integration test without fighting the
framework.

---

## What "viable for integration tests" means

An integration test in SteamRot simulates one or more ticks of a scene, injects
input or events, runs the relevant Logic classes, captures the resulting engine
state in a data bank, and compares that captured state against an expected
snapshot.

For this to be reliable the following must all be true:

1. Test data can be loaded from FlatBuffers binary files without error.
2. `TestEngine::StartUp()` successfully initialises the engine, configures the
   `SceneManager`, and pre-loads any specified `EventBus` state.
3. Per-tick execution correctly sequences: input injection → event injection →
   waiting-room promotion → simulation steps → event-bus tick.
4. Every Logic class that a test may wish to execute can be dispatched by
   `SimulationRunner`.
5. Mouse and keyboard input can be injected into `EventHandler` and
   `SceneContext` at a specific tick.
6. Arbitrary `EventPacket` objects can be injected into the waiting-room event
   bus at a specific tick.
7. After all ticks have run, the data bank (snapshots per tick) can be compared
   against expected snapshots from the test data.
8. There is a single, simple entry-point function that wires all of the above
   together.
9. There is a live test executable that loads all test-data files from a
   directory and runs them automatically, giving a green/red result per file.

---

## Current state — summary

| # | Component | Status | Blocker for integration? |
|---|-----------|--------|--------------------------|
| 1 | `FlatbuffersTestDataLoader` / `FlatbuffersTestDataProvider` | Implemented, unit-tested | No — loads existing format; new tick fields need extending |
| 2 | `TestEngine` | Implemented, unit-tested | Partial — RunGameLoop is functional; tick-level input/event wiring missing |
| 3 | `SimulationRunner` | Implemented, unit-tested | Partial — only UI logic classes dispatched; `CraftingRenderLogic` case returns error |
| 4 | `input_simulation` | **Not implemented** | Yes — no way to inject mouse/keyboard per tick |
| 5 | `event_simulation` | **Not implemented** | Yes — no way to inject `EventPacket` objects per tick |
| 6 | `test_harness` | **Not implemented** | Yes — no unified orchestration entry point |
| 7 | `test_data_comparison` | **Not implemented** | Yes — `RunSnapshotComparisons` exists in `harness_runner` but lacks tick-snapshot support |
| 8 | `simulation_tests` activation | Commented out | Yes — data-driven integration tests cannot run |
| 9 | `tests/integration/` test suite | Empty | Yes — no integration tests exist |

---

## Component dependency graph

```
tests/integration/  ──►  test_harness
simulation_tests/   ──►  test_harness
                             │
                    ┌────────┴──────────┐
                    │                   │
              TestEngine          test_data_comparison
                    │
          ┌─────────┼────────────┐
          │         │            │
  FlatbuffersTest  SimulationRunner  (per-tick pipeline)
  DataLoader/                        │
  Provider                  ┌────────┴────────┐
                             │                 │
                     input_simulation   event_simulation
```

Work should proceed roughly bottom-up: the loader and simulation runner must be
stable before the engine, which must be stable before the harness and
comparison, which must exist before any integration test can be written.

---

## Separate component plans

Each component has its own detailed plan document in this directory:

| Document | Component |
|----------|-----------|
| [PLAN_01_TEST_DATA_LOADER.md](PLAN_01_TEST_DATA_LOADER.md) | FlatBuffers data loading API |
| [PLAN_02_TEST_ENGINE.md](PLAN_02_TEST_ENGINE.md) | `TestEngine` — tick pipeline & input/event wiring |
| [PLAN_03_SIMULATION_RUNNER.md](PLAN_03_SIMULATION_RUNNER.md) | `SimulationRunner` — full Logic class dispatch |
| [PLAN_04_INPUT_SIMULATION.md](PLAN_04_INPUT_SIMULATION.md) | `input_simulation` — mouse/keyboard injection |
| [PLAN_05_EVENT_SIMULATION.md](PLAN_05_EVENT_SIMULATION.md) | `event_simulation` — `EventPacket` injection |
| [PLAN_06_TEST_HARNESS.md](PLAN_06_TEST_HARNESS.md) | `test_harness` — unified orchestration API |
| [PLAN_07_TEST_DATA_COMPARISON.md](PLAN_07_TEST_DATA_COMPARISON.md) | `test_data_comparison` — snapshot comparison |
| [PLAN_08_SIMULATION_TESTS.md](PLAN_08_SIMULATION_TESTS.md) | `simulation_tests` activation |
| [PLAN_09_INTEGRATION_TEST_SUITE.md](PLAN_09_INTEGRATION_TEST_SUITE.md) | `tests/integration/` test suite |

---

## Recommended implementation order

Complete the components in the following sequence to avoid blockers:

1. **Plan 01** — data loader (all other components depend on being able to load
   test data).
2. **Plan 03** — simulation runner (the engine calls this; it should be
   complete before the engine is exercised end-to-end).
3. **Plan 04** — input simulation (per-tick wiring needed by the engine).
4. **Plan 05** — event simulation (per-tick wiring needed by the engine).
5. **Plan 02** — test engine (integrate the per-tick pipeline).
6. **Plan 07** — data comparison (comparison logic needed by harness and
   integration tests).
7. **Plan 06** — test harness (unified API that connects everything).
8. **Plan 08** — activate simulation tests (first real end-to-end validation).
9. **Plan 09** — integration test suite (first purpose-written integration
   tests).
