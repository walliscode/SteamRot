/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the depth_first_search free functions.
///
/// Covers resolve_transition (all ChainStepKind combinations), the
/// depth_first_search entry checks, and integration scenarios that verify
/// correct subgraph recording, progress propagation, and cycle termination.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "depth_first_search.h"
#include "AnalysisTraceBuilder.h"
#include "PartGraphBuilder.h"
#include "TerminalDescriptorFormatter.h"
#include "TraceEqualsMatcher.h"
#include "descriptors_node_descriptors.h"
#include "part_graph_library.h"
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <vector>

namespace steamrot::tests {
using namespace steamrot::logic::descriptors;

namespace {

/////////////////////////////////////////////////
/// @brief Build a ChainStep with an explicit kind and optional minimum.
/////////////////////////////////////////////////
ChainStep make_step(NodeDescriptor predicate, ChainStepKind kind,
                    size_t min_repetitions = 1) {
  return ChainStep{std::move(predicate), kind, min_repetitions};
}

/////////////////////////////////////////////////
/// @brief Run depth_first_search from start_id and return the result.
///
/// Mirrors the setup that ChainDescriptorBuilder::Build performs so tests
/// can drive the DFS directly without going through the full builder API.
/////////////////////////////////////////////////
ChainDescriptorResult run_dfs(uint32_t start_id, std::vector<ChainStep> steps,
                              const steamrot::PartGraph &parts) {

  DFSContext context{steps};
  Cursor start{};
  start.current_id = start_id;
  start.steps_it = context.steps.cbegin();
  start.depth = 1;
  ChainDescriptorResult result{false};
  depth_first_search(start, context, parts, result);
  if (result.valid_subgraph.has_value())
    result.m_result = true;
  result.m_trace = std::move(context.trace);
  return result;
}

} // namespace

/////////////////////////////////////////////////
/// resolve_transition — Sequence
/////////////////////////////////////////////////
TEST_CASE("resolve_transition: Sequence steps",
          "[unit][logic][descriptors][dfs]") {

  ChainStep step{is_fragment(), ChainStepKind::Sequence};

  SECTION("predicate passes → ConsumeNodeAndAdvanceStep") {
    const Transition t = resolve_transition(step, true, {});
    REQUIRE(t.kind == TransitionKind::ConsumeNodeAndAdvanceStep);
  }

  SECTION("predicate fails → Reject") {
    const Transition t = resolve_transition(step, false, {});
    REQUIRE(t.kind == TransitionKind::Reject);
  }
}

/////////////////////////////////////////////////
/// resolve_transition — WhileIsTrue
/////////////////////////////////////////////////
TEST_CASE("resolve_transition: WhileIsTrue steps",
          "[unit][logic][descriptors][dfs]") {

  ChainStep step{is_serial(), ChainStepKind::WhileIsTrue};

  SECTION(
      "predicate passes first time → ConsumeNodeAndHoldStep, match_count=1") {
    const Transition t = resolve_transition(step, true, StepProgress{0});
    REQUIRE(t.kind == TransitionKind::ConsumeNodeAndHoldStep);
    REQUIRE(t.progress.match_count == 1);
  }

  SECTION("predicate passes again → ConsumeNodeAndHoldStep, match_count "
          "incremented") {
    const Transition t = resolve_transition(step, true, StepProgress{3});
    REQUIRE(t.kind == TransitionKind::ConsumeNodeAndHoldStep);
    REQUIRE(t.progress.match_count == 4);
  }

  SECTION("predicate fails with no prior matches → Reject") {
    const Transition t = resolve_transition(step, false, StepProgress{0});
    REQUIRE(t.kind == TransitionKind::Reject);
  }

  SECTION("predicate fails after at least one match → HoldNodeAndAdvanceStep") {
    const Transition t = resolve_transition(step, false, StepProgress{1});
    REQUIRE(t.kind == TransitionKind::HoldNodeAndAdvanceStep);
  }
}

/////////////////////////////////////////////////
/// resolve_transition — WhileIsTrueForMinimumN
/////////////////////////////////////////////////
TEST_CASE("resolve_transition: WhileIsTrueForMinimumN steps",
          "[unit][logic][descriptors][dfs]") {

  SECTION("predicate passes → ConsumeNodeAndHoldStep, count incremented") {
    ChainStep step =
        make_step(is_serial(), ChainStepKind::WhileIsTrueForMinimumN, 2);
    const Transition t = resolve_transition(step, true, StepProgress{1});
    REQUIRE(t.kind == TransitionKind::ConsumeNodeAndHoldStep);
    REQUIRE(t.progress.match_count == 2);
  }

  SECTION("predicate fails, count below minimum → Reject") {
    ChainStep step =
        make_step(is_serial(), ChainStepKind::WhileIsTrueForMinimumN, 3);
    const Transition t = resolve_transition(step, false, StepProgress{2});
    REQUIRE(t.kind == TransitionKind::Reject);
  }

  SECTION(
      "predicate fails, count exactly at minimum → HoldNodeAndAdvanceStep") {
    ChainStep step =
        make_step(is_serial(), ChainStepKind::WhileIsTrueForMinimumN, 2);
    const Transition t = resolve_transition(step, false, StepProgress{2});
    REQUIRE(t.kind == TransitionKind::HoldNodeAndAdvanceStep);
  }

  SECTION("predicate fails, count exceeds minimum → HoldNodeAndAdvanceStep") {
    ChainStep step =
        make_step(is_serial(), ChainStepKind::WhileIsTrueForMinimumN, 2);
    const Transition t = resolve_transition(step, false, StepProgress{5});
    REQUIRE(t.kind == TransitionKind::HoldNodeAndAdvanceStep);
  }
}

/////////////////////////////////////////////////
/// depth_first_search — single-node scenarios
/////////////////////////////////////////////////
TEST_CASE("depth_first_search: single-node scenarios",
          "[unit][logic][descriptors][dfs]") {

  SECTION("isolated node matching a Sequence step → one valid subgraph") {
    // fragment_no_socket has no sockets so there are no neighbours to visit;
    // the valid subgraph must be recorded when the Sequence step is consumed.
    const steamrot::PartGraph parts =
        steamrot::tests::PartGraphBuilder{}
            .AddFragmentInstance(steamrot::tests::FragmentNames::NoSocket, "f0")
            .Build()
            .part_graph;
    const ChainDescriptorResult result =
        run_dfs(0, {make_step(is_fragment(), ChainStepKind::Sequence)}, parts);

    REQUIRE(result.valid_subgraph.has_value());
    REQUIRE(*result.valid_subgraph == std::vector<uint32_t>{0});
    REQUIRE(result.invalid_subgraphs.empty());
  }

  SECTION("isolated node not matching a Sequence step → one invalid subgraph") {
    const steamrot::PartGraph parts =
        steamrot::tests::PartGraphBuilder{}
            .AddFragmentInstance(steamrot::tests::FragmentNames::NoSocket, "f0")
            .Build()
            .part_graph;
    const ChainDescriptorResult result =
        run_dfs(0, {make_step(is_joint(), ChainStepKind::Sequence)}, parts);

    REQUIRE_FALSE(result.valid_subgraph.has_value());
    REQUIRE(result.invalid_subgraphs.size() == 1);
  }

  SECTION(
      "start node missing from graph → invalid subgraph recorded, no crash") {
    const steamrot::PartGraph parts =
        steamrot::tests::PartGraphBuilder{}
            .AddFragmentInstance(steamrot::tests::FragmentNames::NoSocket, "f0")
            .Build()
            .part_graph;
    const ChainDescriptorResult result = run_dfs(
        9999, {make_step(is_fragment(), ChainStepKind::Sequence)}, parts);

    REQUIRE_FALSE(result.valid_subgraph.has_value());
    REQUIRE(result.invalid_subgraphs.size() == 1);
  }
}

/////////////////////////////////////////////////
/// depth_first_search — WhileIsTrue progress propagation
/////////////////////////////////////////////////
TEST_CASE(
    "depth_first_search: WhileIsTrue progress is propagated through DFS layers",
    "[unit][logic][descriptors][dfs]") {

  // LinearChain topology built fresh for this test:
  //   frag0(id=0, terminal) ─ joint0(id=2, serial) ─ frag1(id=1, terminal)
  //   frag0.socket[1] ↔ joint0.socket[0]
  //   joint0.socket[1] ↔ frag1.socket[0]
  const steamrot::tests::PartGraphPackage pkg =
      steamrot::tests::PartGraphBuilder{}
          .AddFragmentInstance(steamrot::tests::FragmentNames::TwoSockets,
                               "f0") // id=0
          .AddFragmentInstance(steamrot::tests::FragmentNames::TwoSockets,
                               "f1") // id=1
          .AddJointInstance(steamrot::tests::JointNames::TwoSockets,
                            "j0")    // id=2
          .Connect("f0", 1, "j0", 0) // f0.socket[1] ↔ j0.socket[0]
          .Connect("j0", 1, "f1", 0) // j0.socket[1] ↔ f1.socket[0]
          .Build();
  const steamrot::PartGraph &parts = pkg.part_graph;

  SECTION("WhileIsTrue(is_serial) only: joint0 recorded as valid via "
          "HoldNodeAndAdvanceStep path") {
    // joint0 passes is_serial and is consumed.  Both terminal neighbours then
    // fail is_serial; since match_count=1 >= 1 they trigger
    // HoldNodeAndAdvanceStep which re-enters with steps_end → records {joint0}.
    const ChainDescriptorResult result =
        run_dfs(2, {make_step(is_serial(), ChainStepKind::WhileIsTrue)}, parts);

    REQUIRE(result.valid_subgraph.has_value());
    REQUIRE(*result.valid_subgraph == std::vector<uint32_t>{2});

    const AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            .NodeEval("j0", "is_serial", true, 1,
                      "connection_count=2, expected==2")
            .MovingToNeighbour("j0", 0, "f0", 1, 1)
            .NodeEval("f0", "is_serial", false, 2,
                      "connection_count=1, expected==2")
            .Backtracking("f0", 1, "j0", 0, 1)
            .Build();

    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }

  SECTION(
      "WhileIsTrue(is_serial).Then(is_terminal): both terminal paths found") {
    // Correct progress propagation is required here: when is_serial passes for
    // joint0, match_count must be carried into the child cursor so that
    // is_serial failing at the terminal correctly resolves to
    // HoldNodeAndAdvanceStep (match_count=1 >= 1) rather than Reject.
    const ChainDescriptorResult result =
        run_dfs(2,
                {make_step(is_serial(), ChainStepKind::WhileIsTrue),
                 make_step(is_terminal(), ChainStepKind::Sequence)},
                parts);

    REQUIRE(result.valid_subgraph.has_value());
    REQUIRE(*result.valid_subgraph == std::vector<uint32_t>{2, 0});

    const AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            .NodeEval("j0", "is_serial", true, 1,
                      "connection_count=2, expected==2")
            .MovingToNeighbour("j0", 0, "f0", 1, 1)
            .NodeEval("f0", "is_serial", false, 2,
                      "connection_count=1, expected==2")
            .NodeEval("f0", "is_terminal", true, 2,
                      "connection_count=1, expected==1")
            .Backtracking("f0", 1, "j0", 0, 1)
            .Build();

    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }
}

/////////////////////////////////////////////////
/// depth_first_search — WhileIsTrueForMinimumN minimum enforcement
/////////////////////////////////////////////////
TEST_CASE("depth_first_search: WhileIsTrueForMinimumN minimum enforcement",
          "[unit][logic][descriptors][dfs]") {

  // topology: f0(terminal) ─ j1(serial) ─ f2(serial) ─ j3(terminal)
  const steamrot::tests::PartGraphPackage pkg =
      steamrot::tests::PartGraphBuilder{}
          .AddFragmentInstance(steamrot::tests::FragmentNames::OneSocket, "f0")
          .AddJointInstance(steamrot::tests::JointNames::TwoSockets, "j1")
          .AddFragmentInstance(steamrot::tests::FragmentNames::TwoSockets, "f2")
          .AddJointInstance(steamrot::tests::JointNames::OneSocket, "j3")
          .Connect("f0", 0, "j1", 0)
          .Connect("j1", 1, "f2", 0)
          .Connect("f2", 1, "j3", 0)
          .Build();
  const steamrot::PartGraph &parts = pkg.part_graph;

  SECTION("min=3 not satisfied: only 2 serial nodes available, all rejected") {
    const ChainDescriptorResult result = run_dfs(
        1,
        {make_step(is_serial(), ChainStepKind::WhileIsTrueForMinimumN, 3),
         make_step(is_terminal(), ChainStepKind::Sequence)},
        parts);

    REQUIRE_FALSE(result.valid_subgraph.has_value());

    const AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            .NodeEval("j1", "is_serial", true, 1,
                      "connection_count=2, expected==2")
            .MovingToNeighbour("j1", 0, "f0", 0, 1)
            .NodeEval("f0", "is_serial", false, 2,
                      "connection_count=1, expected==2")
            .Backtracking("f0", 0, "j1", 0, 1)
            .MovingToNeighbour("j1", 1, "f2", 0, 1)
            .NodeEval("f2", "is_serial", true, 2,
                      "connection_count=2, expected==2")
            .MovingToNeighbour("f2", 1, "j3", 0, 2)
            .NodeEval("j3", "is_serial", false, 3,
                      "connection_count=1, expected==2")
            .Backtracking("j3", 0, "f2", 1, 2)
            .Backtracking("f2", 0, "j1", 1, 1)
            .Build();

    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }

  SECTION("min=2 satisfied: serial run accepted, terminal follows") {
    const ChainDescriptorResult result = run_dfs(
        1,
        {make_step(is_serial(), ChainStepKind::WhileIsTrueForMinimumN, 2),
         make_step(is_terminal(), ChainStepKind::Sequence)},
        parts);

    REQUIRE(result.valid_subgraph.has_value());

    const AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            .NodeEval("j1", "is_serial", true, 1,
                      "connection_count=2, expected==2")
            .MovingToNeighbour("j1", 0, "f0", 0, 1)
            .NodeEval("f0", "is_serial", false, 2,
                      "connection_count=1, expected==2")
            .Backtracking("f0", 0, "j1", 0, 1)
            .MovingToNeighbour("j1", 1, "f2", 0, 1)
            .NodeEval("f2", "is_serial", true, 2,
                      "connection_count=2, expected==2")
            .MovingToNeighbour("f2", 1, "j3", 0, 2)
            .NodeEval("j3", "is_serial", false, 3,
                      "connection_count=1, expected==2")
            .NodeEval("j3", "is_terminal", true, 3,
                      "connection_count=1, expected==1")
            .Backtracking("j3", 0, "f2", 1, 2)
            .Backtracking("f2", 0, "j1", 1, 1)
            .Build();

    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }
}

/////////////////////////////////////////////////
/// depth_first_search — cycle detection
/////////////////////////////////////////////////
TEST_CASE("depth_first_search: DFS terminates on cyclic graphs",
          "[unit][logic][descriptors][dfs]") {

  SECTION("single WhileIsTrue step terminates on Ring, no valid subgraphs") {
    // All joints in the Ring are serial so HoldNodeAndAdvanceStep never fires
    // (no failing node exists). The cycle guard ensures the DFS terminates.
    const ChainDescriptorResult result =
        run_dfs(0, {make_step(is_serial(), ChainStepKind::WhileIsTrue)},
                ring().part_graph);
    REQUIRE_FALSE(result.valid_subgraph.has_value());
  }

  SECTION(
      "WhileIsTrue+Then(is_terminal) terminates on Ring, no valid subgraphs") {
    // Ring has no terminal nodes so the Then(is_terminal) step is never
    // satisfied; no valid subgraph can be found.
    const ChainDescriptorResult result =
        run_dfs(0,
                {make_step(is_serial(), ChainStepKind::WhileIsTrue),
                 make_step(is_terminal(), ChainStepKind::Sequence)},
                ring().part_graph);
    REQUIRE_FALSE(result.valid_subgraph.has_value());
  }
}
} // namespace steamrot::tests
