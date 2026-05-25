/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for AnalysisTraceBuilder and TraceEqualsMatcher.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AnalysisTraceBuilder.h"
#include "TraceEqualsMatcher.h"
#include "TerminalDescriptorFormatter.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <stdexcept>
#include <unordered_map>

using namespace steamrot::logic::descriptors;
namespace tests = steamrot::tests;

// ─── AnalysisTraceBuilder ────────────────────────────────────────────────────

TEST_CASE("AnalysisTraceBuilder produces an empty trace by default",
          "[unit][descriptors][builder][matcher]") {
  tests::AnalysisTraceBuilder builder;
  REQUIRE(builder.Build().empty());
}

TEST_CASE("AnalysisTraceBuilder::NodeEval appends correct event",
          "[unit][descriptors][builder][matcher]") {
  tests::AnalysisTraceBuilder builder;
  builder.NodeEval(7u, "is_fragment", 2u);

  const AnalysisTrace trace = builder.Build();
  REQUIRE(trace.size() == 1);

  const AnalysisEvent &ev = trace[0];
  REQUIRE(ev.kind == TraceEventKind::NodeEval);
  REQUIRE(ev.part_id == 7u);
  REQUIRE(ev.predicate_name == "is_fragment");
  REQUIRE(ev.depth == 2u);
}

TEST_CASE("AnalysisTraceBuilder::NodeResult appends correct event",
          "[unit][descriptors][builder][matcher]") {
  tests::AnalysisTraceBuilder builder;
  builder.NodeResult(3u, "is_joint", true, "node holds JointInstance", 1u);

  const AnalysisTrace trace = builder.Build();
  REQUIRE(trace.size() == 1);

  const AnalysisEvent &ev = trace[0];
  REQUIRE(ev.kind == TraceEventKind::NodeResult);
  REQUIRE(ev.part_id == 3u);
  REQUIRE(ev.predicate_name == "is_joint");
  REQUIRE(ev.result == true);
  REQUIRE(ev.reason == "node holds JointInstance");
  REQUIRE(ev.depth == 1u);
}

TEST_CASE("AnalysisTraceBuilder::NodeResult reason defaults to empty",
          "[unit][descriptors][builder][matcher]") {
  tests::AnalysisTraceBuilder builder;
  builder.NodeResult(0u, "is_fragment", false);

  const AnalysisTrace trace = builder.Build();
  REQUIRE(trace[0].reason.empty());
}

TEST_CASE("AnalysisTraceBuilder::MovingToNeighbour appends correct event",
          "[unit][descriptors][builder][matcher]") {
  tests::AnalysisTraceBuilder builder;
  builder.MovingToNeighbour(1u, 2u, 0u, 3u);

  const AnalysisTrace trace = builder.Build();
  REQUIRE(trace.size() == 1);

  const AnalysisEvent &ev = trace[0];
  REQUIRE(ev.kind == TraceEventKind::MovingToNeighbour);
  REQUIRE(ev.from_id == 1u);
  REQUIRE(ev.to_id == 2u);
  REQUIRE(ev.socket_id == 0u);
  REQUIRE(ev.depth == 3u);
}

TEST_CASE("AnalysisTraceBuilder::Backtracking appends correct event",
          "[unit][descriptors][builder][matcher]") {
  tests::AnalysisTraceBuilder builder;
  builder.Backtracking(5u, 2u);

  const AnalysisTrace trace = builder.Build();
  REQUIRE(trace.size() == 1);

  const AnalysisEvent &ev = trace[0];
  REQUIRE(ev.kind == TraceEventKind::Backtracking);
  REQUIRE(ev.from_id == 5u);
  REQUIRE(ev.depth == 2u);
}

TEST_CASE("AnalysisTraceBuilder::ScopeBegin appends correct event",
          "[unit][descriptors][builder][matcher]") {
  tests::AnalysisTraceBuilder builder;

  SECTION("Chain scope with anchor") {
    builder.ScopeBegin("my_chain", ScopeKind::Chain, 0u, 4u);

    const AnalysisTrace trace = builder.Build();
    REQUIRE(trace.size() == 1);

    const AnalysisEvent &ev = trace[0];
    REQUIRE(ev.kind == TraceEventKind::ScopeBegin);
    REQUIRE(ev.scope_name == "my_chain");
    REQUIRE(ev.scope_kind == ScopeKind::Chain);
    REQUIRE(ev.depth == 0u);
    REQUIRE(ev.anchor_id.has_value());
    REQUIRE(*ev.anchor_id == 4u);
  }

  SECTION("Graph scope without anchor") {
    builder.ScopeBegin("my_graph", ScopeKind::Graph, 0u);

    const AnalysisTrace trace = builder.Build();
    REQUIRE(trace[0].scope_kind == ScopeKind::Graph);
    REQUIRE_FALSE(trace[0].anchor_id.has_value());
  }
}

TEST_CASE("AnalysisTraceBuilder::ScopeEnd appends correct event",
          "[unit][descriptors][builder][matcher]") {
  tests::AnalysisTraceBuilder builder;
  builder.ScopeEnd("my_chain", ScopeKind::Chain, true, 0u);

  const AnalysisTrace trace = builder.Build();
  REQUIRE(trace.size() == 1);

  const AnalysisEvent &ev = trace[0];
  REQUIRE(ev.kind == TraceEventKind::ScopeEnd);
  REQUIRE(ev.scope_name == "my_chain");
  REQUIRE(ev.scope_kind == ScopeKind::Chain);
  REQUIRE(ev.result == true);
  REQUIRE(ev.depth == 0u);
}

TEST_CASE("AnalysisTraceBuilder supports method chaining across all event types",
          "[unit][descriptors][builder][matcher]") {
  tests::AnalysisTraceBuilder builder;
  builder
    .ScopeBegin("chain", ScopeKind::Chain, 0u, 1u)
    .NodeEval(1u, "is_terminal", 1u)
    .NodeResult(1u, "is_terminal", true, "connection_count=1, expected<=1", 1u)
    .MovingToNeighbour(1u, 2u, 0u, 1u)
    .NodeEval(2u, "is_terminal", 2u)
    .NodeResult(2u, "is_terminal", false, "connection_count=2, expected<=1", 2u)
    .Backtracking(2u, 1u)
    .ScopeEnd("chain", ScopeKind::Chain, false, 0u);

  REQUIRE(builder.Build().size() == 8);
}

TEST_CASE("AnalysisTraceBuilder::Build returns a copy",
          "[unit][descriptors][builder][matcher]") {
  tests::AnalysisTraceBuilder builder;
  builder.NodeEval(0u, "is_fragment");

  AnalysisTrace first = builder.Build();
  builder.NodeEval(1u, "is_joint");
  AnalysisTrace second = builder.Build();

  REQUIRE(first.size() == 1);
  REQUIRE(second.size() == 2);
}

TEST_CASE("AnalysisTraceBuilder resolves string IDs via id_to_part_graph_id",
          "[unit][descriptors][builder][matcher]") {
  const std::unordered_map<std::string, uint32_t> id_to_part_graph_id{
      {"f0", 0u}, {"j0", 2u}};

  tests::AnalysisTraceBuilder builder{id_to_part_graph_id};
  builder
      .ScopeBegin("chain", ScopeKind::Chain, 0u, "f0")
      .NodeEval("f0", "is_fragment", 1u)
      .NodeResult("f0", "is_fragment", true, "node holds FragmentInstance", 1u)
      .MovingToNeighbour("f0", "j0", 0u, 1u)
      .Backtracking("j0", 1u);

  const AnalysisTrace trace = builder.Build();
  REQUIRE(trace.size() == 5);
  REQUIRE(trace[0].anchor_id.has_value());
  REQUIRE(trace[0].anchor_id.value() == 0u);
  REQUIRE(trace[1].part_id == 0u);
  REQUIRE(trace[2].part_id == 0u);
  REQUIRE(trace[3].from_id == 0u);
  REQUIRE(trace[3].to_id == 2u);
  REQUIRE(trace[4].from_id == 2u);
}

TEST_CASE("AnalysisTraceBuilder string-ID overloads validate alias map",
          "[unit][descriptors][builder][matcher]") {
  SECTION("throws when string overload is used without alias map") {
    tests::AnalysisTraceBuilder builder;
    REQUIRE_THROWS_AS(builder.NodeEval("f0", "is_fragment"), std::logic_error);
  }

  SECTION("throws when alias is missing from alias map") {
    const std::unordered_map<std::string, uint32_t> id_to_part_graph_id{
        {"f0", 0u}};
    tests::AnalysisTraceBuilder builder{id_to_part_graph_id};
    REQUIRE_THROWS_AS(builder.NodeEval("missing", "is_fragment"),
                      std::out_of_range);
  }
}

// ─── TraceEqualsMatcher ──────────────────────────────────────────────────────

TEST_CASE("TraceEqualsMatcher matches identical traces",
          "[unit][descriptors][builder][matcher]") {
  TerminalDescriptorFormatter fmt;

  tests::AnalysisTraceBuilder builder;
  builder
    .NodeEval(0u, "is_fragment")
    .NodeResult(0u, "is_fragment", true, "node holds FragmentInstance");

  const AnalysisTrace trace = builder.Build();

  REQUIRE_THAT(trace, tests::EqualsTrace(trace, fmt));
}

TEST_CASE("TraceEqualsMatcher rejects traces that differ",
          "[unit][descriptors][builder][matcher]") {
  TerminalDescriptorFormatter fmt;

  tests::AnalysisTraceBuilder expected_builder;
  expected_builder
    .NodeEval(0u, "is_fragment")
    .NodeResult(0u, "is_fragment", true, "node holds FragmentInstance");

  tests::AnalysisTraceBuilder actual_builder;
  actual_builder
    .NodeEval(0u, "is_joint")
    .NodeResult(0u, "is_joint", false, "node holds FragmentInstance");

  REQUIRE_THAT(actual_builder.Build(),
               !tests::EqualsTrace(expected_builder.Build(), fmt));
}

TEST_CASE("TraceEqualsMatcher rejects traces with different lengths",
          "[unit][descriptors][builder][matcher]") {
  TerminalDescriptorFormatter fmt;

  tests::AnalysisTraceBuilder expected_builder;
  expected_builder
    .NodeEval(0u, "is_fragment")
    .NodeResult(0u, "is_fragment", true, "node holds FragmentInstance");

  tests::AnalysisTraceBuilder actual_builder;
  actual_builder.NodeEval(0u, "is_fragment");

  REQUIRE_THAT(actual_builder.Build(),
               !tests::EqualsTrace(expected_builder.Build(), fmt));
}

TEST_CASE("TraceEqualsMatcher::describe returns 'equals trace' when matched",
          "[unit][descriptors][builder][matcher]") {
  TerminalDescriptorFormatter fmt;
  tests::AnalysisTraceBuilder builder;
  builder.NodeEval(0u, "is_fragment");
  const AnalysisTrace trace = builder.Build();

  auto matcher = tests::EqualsTrace(trace, fmt);
  matcher.match(trace);

  REQUIRE(matcher.describe() == "equals trace");
}

TEST_CASE(
    "TraceEqualsMatcher::describe reports mismatch details on failure",
    "[unit][descriptors][builder][matcher]") {
  TerminalDescriptorFormatter fmt;

  tests::AnalysisTraceBuilder expected_builder;
  expected_builder
    .NodeEval(0u, "is_fragment")
    .NodeResult(0u, "is_fragment", true, "node holds FragmentInstance");

  tests::AnalysisTraceBuilder actual_builder;
  actual_builder
    .NodeEval(0u, "is_joint")
    .NodeResult(0u, "is_joint", false, "node holds FragmentInstance");

  const AnalysisTrace expected_trace = expected_builder.Build();
  const AnalysisTrace actual_trace = actual_builder.Build();

  auto matcher = tests::EqualsTrace(expected_trace, fmt);
  matcher.match(actual_trace);

  const std::string description = matcher.describe();
  REQUIRE(description.find("equals trace") != std::string::npos);
  REQUIRE(description.find("expected formatted output") != std::string::npos);
  REQUIRE(description.find("actual formatted output") != std::string::npos);
  REQUIRE(description.find("first mismatch") != std::string::npos);
}

TEST_CASE("TraceEqualsMatcher matches empty traces",
          "[unit][descriptors][builder][matcher]") {
  TerminalDescriptorFormatter fmt;
  const AnalysisTrace empty;
  REQUIRE_THAT(empty, tests::EqualsTrace(empty, fmt));
}

TEST_CASE("TraceEqualsMatcher rejects non-empty trace against empty expected",
          "[unit][descriptors][builder][matcher]") {
  TerminalDescriptorFormatter fmt;
  tests::AnalysisTraceBuilder builder;
  builder.NodeEval(0u, "is_fragment");

  const AnalysisTrace empty;
  REQUIRE_THAT(builder.Build(), !tests::EqualsTrace(empty, fmt));
}
