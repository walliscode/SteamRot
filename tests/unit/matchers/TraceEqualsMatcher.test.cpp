/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for AnalysisTraceBuilder and TraceEqualsMatcher.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TraceEqualsMatcher.h"
#include "AnalysisTraceBuilder.h"
#include "TerminalDescriptorFormatter.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <unordered_map>

using namespace steamrot::logic::descriptors;
namespace tests = steamrot::tests;

namespace {
const std::unordered_map<std::string, uint32_t> kPartIds{
    {"f0", 0u}, {"j0", 1u}, {"f1", 2u}, {"j1", 3u},      {"f2", 4u},
    {"p7", 7u}, {"p3", 3u}, {"p5", 5u}, {"missing", 99u}};
}

// ─── AnalysisTraceBuilder ────────────────────────────────────────────────────

TEST_CASE("AnalysisTraceBuilder produces an empty trace by default",
          "[unit][descriptors][builder][matcher]") {
  tests::AnalysisTraceBuilder builder{{}};
  REQUIRE(builder.Build().empty());
}

TEST_CASE("AnalysisTraceBuilder::NodeEval appends correct event",
          "[unit][descriptors][builder][matcher]") {
  tests::AnalysisTraceBuilder builder{kPartIds};
  builder.NodeEval("p7", "is_fragment", true, 2u,
                   "node holds FragmentInstance");

  const AnalysisTrace trace = builder.Build();
  REQUIRE(trace.size() == 1);

  const AnalysisEvent &ev = trace[0];
  REQUIRE(ev.kind == TraceEventKind::NodeEval);
  REQUIRE(ev.part_id == 7u);
  REQUIRE(ev.predicate_name == "is_fragment");
  REQUIRE(ev.depth == 2u);
}

TEST_CASE("AnalysisTraceBuilder::MovingToNeighbour appends correct event",
          "[unit][descriptors][builder][matcher]") {
  tests::AnalysisTraceBuilder builder{kPartIds};
  builder.MovingToNeighbour("j0", 4u, "f1", 9u, 3u);

  const AnalysisTrace trace = builder.Build();
  REQUIRE(trace.size() == 1);

  const AnalysisEvent &ev = trace[0];
  REQUIRE(ev.kind == TraceEventKind::MovingToNeighbour);
  REQUIRE(ev.from_id == 1u);
  REQUIRE(ev.from_socket_id == 4u);
  REQUIRE(ev.to_id == 2u);
  REQUIRE(ev.to_socket_id == 9u);
  REQUIRE(ev.depth == 3u);
}

TEST_CASE("AnalysisTraceBuilder::Backtracking appends correct event",
          "[unit][descriptors][builder][matcher]") {
  tests::AnalysisTraceBuilder builder{kPartIds};
  builder.Backtracking("p5", 6u, "p3", 2u, 2u);

  const AnalysisTrace trace = builder.Build();
  REQUIRE(trace.size() == 1);

  const AnalysisEvent &ev = trace[0];
  REQUIRE(ev.kind == TraceEventKind::Backtracking);
  REQUIRE(ev.from_id == 5u);
  REQUIRE(ev.from_socket_id == 6u);
  REQUIRE(ev.to_id == 3u);
  REQUIRE(ev.to_socket_id == 2u);
  REQUIRE(ev.depth == 2u);
}

TEST_CASE("AnalysisTraceBuilder::ScopeBegin appends correct event",
          "[unit][descriptors][builder][matcher]") {
  tests::AnalysisTraceBuilder builder{kPartIds};

  SECTION("Chain scope with anchor") {
    builder.ScopeBegin("my_chain", ScopeKind::Chain, 0u, "f2");

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

  SECTION("MachinaArchetype scope without anchor") {
    builder.ScopeBegin("my_graph", ScopeKind::MachinaArchetype, 0u);

    const AnalysisTrace trace = builder.Build();
    REQUIRE(trace[0].scope_kind == ScopeKind::MachinaArchetype);
    REQUIRE_FALSE(trace[0].anchor_id.has_value());
  }
}

TEST_CASE("AnalysisTraceBuilder::ScopeEnd appends correct event",
          "[unit][descriptors][builder][matcher]") {
  tests::AnalysisTraceBuilder builder{{}};
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

TEST_CASE(
    "AnalysisTraceBuilder supports method chaining across all event types",
    "[unit][descriptors][builder][matcher]") {
  tests::AnalysisTraceBuilder builder{kPartIds};
  builder.ScopeBegin("chain", ScopeKind::Chain, 0u, "j0")
      .NodeEval("j0", "is_terminal", true, 1u,
                "connection_count=1, expected<=1")
      .MovingToNeighbour("j0", 0u, "f1", 1u, 1u)
      .NodeEval("f1", "is_terminal", false, 2u,
                "connection_count=2, expected<=1")
      .Backtracking("f1", 1u, "j0", 0u, 1u)
      .ScopeEnd("chain", ScopeKind::Chain, false, 0u);

  REQUIRE(builder.Build().size() == 6);
}

TEST_CASE("AnalysisTraceBuilder::Build returns a copy",
          "[unit][descriptors][builder][matcher]") {
  tests::AnalysisTraceBuilder builder{kPartIds};
  builder.NodeEval("f0", "is_fragment", true, 0u,
                   "node holds FragmentInstance");

  AnalysisTrace first = builder.Build();
  builder.NodeEval("j0", "is_joint", true, 0u, "node holds JointInstance");
  AnalysisTrace second = builder.Build();

  REQUIRE(first.size() == 1);
  REQUIRE(second.size() == 2);
}

TEST_CASE("AnalysisTraceBuilder resolves string IDs via id_to_part_graph_id",
          "[unit][descriptors][builder][matcher]") {
  const std::unordered_map<std::string, uint32_t> id_to_part_graph_id{
      {"f0", 0u}, {"j0", 1u}};

  tests::AnalysisTraceBuilder builder{id_to_part_graph_id};
  builder.ScopeBegin("chain", ScopeKind::Chain, 0u, "f0")
      .NodeEval("f0", "is_fragment", true, 1u, "node holds FragmentInstance")
      .MovingToNeighbour("f0", 0u, "j0", 1u, 1u)
      .Backtracking("j0", 1u, "f0", 0u, 1u);

  const AnalysisTrace trace = builder.Build();
  REQUIRE(trace.size() == 4);
  REQUIRE(trace[0].anchor_id.has_value());
  REQUIRE(trace[0].anchor_id.value() == 0u);
  REQUIRE(trace[1].part_id == 0u);
  REQUIRE(trace[2].part_id == 0u);
  REQUIRE(trace[3].from_id == 1u);
  REQUIRE(trace[3].to_id == 0u);
}

TEST_CASE("AnalysisTraceBuilder string-ID overloads validate alias map",
          "[unit][descriptors][builder][matcher]") {
  SECTION("throws when alias is missing from alias map") {
    // construct map as we are passing a reference so it needs to outlive the
    // builder
    std::unordered_map<std::string, uint32_t> id_to_part_graph_id{{"f0", 0u}};
    tests::AnalysisTraceBuilder builder{id_to_part_graph_id};
    REQUIRE_THROWS_AS(builder.NodeEval("missing", "is_fragment"),
                      std::out_of_range);
  }
}

// ─── TraceEqualsMatcher ──────────────────────────────────────────────────────

TEST_CASE("TraceEqualsMatcher matches identical traces",
          "[unit][descriptors][builder][matcher]") {
  TerminalDescriptorFormatter fmt;

  tests::AnalysisTraceBuilder builder{kPartIds};
  builder.NodeEval("f0", "is_fragment", true, 0u,
                   "node holds FragmentInstance");

  const AnalysisTrace trace = builder.Build();

  REQUIRE_THAT(trace, tests::EqualsTrace(trace, fmt));
}

TEST_CASE("TraceEqualsMatcher rejects traces that differ",
          "[unit][descriptors][builder][matcher]") {
  TerminalDescriptorFormatter fmt;

  tests::AnalysisTraceBuilder expected_builder{kPartIds};
  expected_builder.NodeEval("f0", "is_fragment", true, 0u,
                            "node holds FragmentInstance");

  tests::AnalysisTraceBuilder actual_builder{kPartIds};
  actual_builder.NodeEval("f0", "is_joint", false, 0u,
                          "node holds FragmentInstance");

  REQUIRE_THAT(actual_builder.Build(),
               !tests::EqualsTrace(expected_builder.Build(), fmt));
}

TEST_CASE("TraceEqualsMatcher rejects traces with different lengths",
          "[unit][descriptors][builder][matcher]") {
  TerminalDescriptorFormatter fmt;

  tests::AnalysisTraceBuilder expected_builder{kPartIds};
  expected_builder.NodeEval("f0", "is_fragment", true, 0u,
                            "node holds FragmentInstance");

  tests::AnalysisTraceBuilder actual_builder{kPartIds};
  actual_builder.NodeEval("f0", "is_fragment", true, 0u,
                          "node holds FragmentInstance");
  actual_builder.NodeEval("j0", "is_joint", true, 0u);

  REQUIRE_THAT(actual_builder.Build(),
               !tests::EqualsTrace(expected_builder.Build(), fmt));
}

TEST_CASE("TraceEqualsMatcher::describe returns 'equals trace' when matched",
          "[unit][descriptors][builder][matcher]") {
  TerminalDescriptorFormatter fmt;
  tests::AnalysisTraceBuilder builder{kPartIds};
  builder.NodeEval("f0", "is_fragment", true, 0u,
                   "node holds FragmentInstance");
  const AnalysisTrace trace = builder.Build();

  auto matcher = tests::EqualsTrace(trace, fmt);
  matcher.match(trace);

  REQUIRE(matcher.describe() == "equals trace");
}

TEST_CASE("TraceEqualsMatcher::describe reports mismatch details on failure",
          "[unit][descriptors][builder][matcher]") {
  TerminalDescriptorFormatter fmt;

  tests::AnalysisTraceBuilder expected_builder{kPartIds};
  expected_builder.NodeEval("f0", "is_fragment", true, 0u,
                            "node holds FragmentInstance");

  tests::AnalysisTraceBuilder actual_builder{kPartIds};
  actual_builder.NodeEval("f0", "is_joint", false, 0u,
                          "node holds FragmentInstance");

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
  tests::AnalysisTraceBuilder builder{kPartIds};
  builder.NodeEval("f0", "is_fragment", true, 0u,
                   "node holds FragmentInstance");

  const AnalysisTrace empty;
  REQUIRE_THAT(builder.Build(), !tests::EqualsTrace(empty, fmt));
}
