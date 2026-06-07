///////////////////////////////////////////////
/// @file
/// @brief Unit tests for descriptors_analysis_event_helpers.h.
///////////////////////////////////////////////

///////////////////////////////////////////////
/// Headers
///////////////////////////////////////////////
#include "descriptors_analysis_event_helpers.h"
#include "part_graph_library.h"
#include <catch2/catch_test_macros.hpp>

using namespace steamrot::logic::descriptors;

namespace {
struct TestContext {
  AnalysisTrace trace{};
};
} // namespace

TEST_CASE("make_scope_begin_event resolves anchor alias from part graph",
          "[unit][logic][descriptors]") {
  const AnalysisEvent event = make_scope_begin_event(
      "test_chain", ScopeKind::Chain, steamrot::tests::pair.part_graph, 2, 0);

  REQUIRE(event.kind == TraceEventKind::ScopeBegin);
  REQUIRE(event.scope_name == "test_chain");
  REQUIRE(event.scope_kind == ScopeKind::Chain);
  REQUIRE(event.depth == 2);
  REQUIRE(event.anchor_id.has_value());
  REQUIRE(event.anchor_id.value() == 0);
  REQUIRE(event.part_id_alias == "f0");
}

TEST_CASE("node and traversal helpers resolve aliases from part graph",
          "[unit][logic][descriptors]") {
  const AnalysisEvent node_eval = make_node_eval_event(
      1, 1, "is_joint", steamrot::tests::pair.part_graph, true,
      "node holds JointInstance");
  REQUIRE(node_eval.kind == TraceEventKind::NodeEval);
  REQUIRE(node_eval.part_id == 1);
  REQUIRE(node_eval.predicate_name == "is_joint");
  REQUIRE(node_eval.part_id_alias == "j0");
  REQUIRE(node_eval.result);
  REQUIRE(node_eval.reason == "node holds JointInstance");

  const AnalysisEvent moving = make_moving_to_neighbour_event(
      3, 0, 11, 1, 22, steamrot::tests::pair.part_graph);
  REQUIRE(moving.kind == TraceEventKind::MovingToNeighbour);
  REQUIRE(moving.from_id == 0);
  REQUIRE(moving.to_id == 1);
  REQUIRE(moving.from_socket_id == 11);
  REQUIRE(moving.to_socket_id == 22);
  REQUIRE(moving.from_id_alias == "f0");
  REQUIRE(moving.to_id_alias == "j0");

  const AnalysisEvent backtracking = make_backtracking_event(
      3, 1, 22, 0, 11, steamrot::tests::pair.part_graph);
  REQUIRE(backtracking.kind == TraceEventKind::Backtracking);
  REQUIRE(backtracking.from_id == 1);
  REQUIRE(backtracking.to_id == 0);
  REQUIRE(backtracking.from_socket_id == 22);
  REQUIRE(backtracking.to_socket_id == 11);
  REQUIRE(backtracking.from_id_alias == "j0");
  REQUIRE(backtracking.to_id_alias == "f0");
}

TEST_CASE("append_event and add_* helpers append to context trace",
          "[unit][logic][descriptors]") {
  TestContext context{};
  add_empty_part_graph_event(context);
  add_empty_chain_steps_event(context);
  add_invalid_subgraph_isolated_event(context, 4);
  add_scope_end_event(context, "test_chain", ScopeKind::Chain, false, 1);
  add_machina_part_result_event(context, "is_joint", true, 2);

  REQUIRE(context.trace.size() == 5);
  REQUIRE(context.trace[0].kind == TraceEventKind::EmtpyPartGraph);
  REQUIRE(context.trace[1].kind == TraceEventKind::EmtpyChainSteps);
  REQUIRE(context.trace[2].kind == TraceEventKind::InvalidSubgraphIsolated);
  REQUIRE(context.trace[2].depth == 4);
  REQUIRE(context.trace[3].kind == TraceEventKind::ScopeEnd);
  REQUIRE(context.trace[3].scope_name == "test_chain");
  REQUIRE(context.trace[3].scope_kind == ScopeKind::Chain);
  REQUIRE_FALSE(context.trace[3].result);
  REQUIRE(context.trace[3].depth == 1);
  REQUIRE(context.trace[4].kind == TraceEventKind::MachinaPartResult);
  REQUIRE(context.trace[4].predicate_name == "is_joint");
  REQUIRE(context.trace[4].result);
  REQUIRE(context.trace[4].depth == 2);
}
