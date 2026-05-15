/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the ChainDescriptorBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ChainDescriptorBuilder.h"
#include "AnalysisTraceBuilder.h"
#include "TerminalDescriptorFormatter.h"
#include "TraceEqualsMatcher.h"
#include "descriptors_node_descriptors.h"
#include "part_library.h"
#include <catch2/catch_test_macros.hpp>

using namespace steamrot::logic::descriptors;

namespace {

/////////////////////////////////////////////////
NodeDescriptor MakeTestPredicate() {
  return NodeDescriptor{
      "always_true",
      [](const steamrot::PartGraph &, uint32_t) -> NodeDescriptorResult {
        return NodeDescriptorResult{true};
      }};
}

} // namespace

TEST_CASE("ChainDescriptorBuilder::Then and ::WhileIsTrue append expected kinds",
          "[unit][descriptors][chain_builder]") {
  ChainDescriptorBuilder builder;
  NodeDescriptor predicate = MakeTestPredicate();

  builder.Then(predicate).WhileIsTrue(predicate).Then(predicate);

  const auto &steps = builder.GetSteps();
  REQUIRE(steps.size() == 3);
  REQUIRE(steps[0].kind == ChainStepKind::Sequence);
  REQUIRE(steps[1].kind == ChainStepKind::WhileIsTrue);
  REQUIRE(steps[2].kind == ChainStepKind::Sequence);
}

TEST_CASE("ChainDescriptorBuilder::Build handles empty graph and empty steps",
          "[unit][descriptors][chain_builder]") {
  TerminalDescriptorFormatter formatter;

  SECTION("empty graph emits scope and EmptyPartGraph") {
    ChainDescriptor descriptor =
        ChainDescriptorBuilder{}.Then(is_terminal).Build("empty_graph_case");
    steamrot::PartGraph parts;
    ChainDescriptorResult result = descriptor(parts, 0);

    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{}
            .ScopeBegin("empty_graph_case", ScopeKind::Chain, 0, 0)
            .EmptyPartGraph()
            .ScopeEnd("empty_graph_case", ScopeKind::Chain, false, 0)
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace, steamrot::tests::EqualsTrace(expected_trace,
                                                              formatter));
  }

  SECTION("empty steps emits scope and EmptyChainSteps") {
    ChainDescriptor descriptor = ChainDescriptorBuilder{}.Build("empty_steps");

    steamrot::tests::TestPartLibrary lib = steamrot::tests::TestPartLibrary::Create();
    steamrot::tests::PartLibraryBuilder builder{lib};
    steamrot::MachinaFormScaffold scaffold = builder.GetScenarioForAnalysis(
        steamrot::tests::ScaffoldScenario::IsolatedPair);
    ChainDescriptorResult result = descriptor(scaffold.parts, 0);

    AnalysisTrace expected_trace = steamrot::tests::AnalysisTraceBuilder{}
                                       .ScopeBegin("empty_steps", ScopeKind::Chain,
                                                   0, 0)
                                       .EmptyChainSteps()
                                       .ScopeEnd("empty_steps", ScopeKind::Chain,
                                                 false, 0)
                                       .Build();

    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace, steamrot::tests::EqualsTrace(expected_trace,
                                                              formatter));
  }
}

TEST_CASE("ChainDescriptorBuilder sequence completes on final node without "
          "neighbour traversal",
          "[unit][descriptors][chain_builder]") {
  TerminalDescriptorFormatter formatter;

  ChainDescriptor descriptor =
      ChainDescriptorBuilder{}.Then(is_terminal).Build("single_terminal");

  steamrot::tests::TestPartLibrary lib = steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};
  steamrot::MachinaFormScaffold scaffold =
      builder.GetScenarioForAnalysis(steamrot::tests::ScaffoldScenario::IsolatedPair);
  ChainDescriptorResult result = descriptor(scaffold.parts, 0);

  AnalysisTrace expected_trace =
      steamrot::tests::AnalysisTraceBuilder{}
          .ScopeBegin("single_terminal", ScopeKind::Chain, 0, 0)
          .NodeEval(0, "is_terminal", 1)
          .NodeResult(0, "is_terminal", true, "connection_count=1, expected==1", 1)
          .ValidSubgraphIsolated(1)
          .ScopeEnd("single_terminal", ScopeKind::Chain, true, 0)
          .Build();

  REQUIRE(result);
  REQUIRE(result.valid_subgraphs.size() == 1);
  REQUIRE(result.valid_subgraphs[0].size() == 1);
  REQUIRE(result.valid_subgraphs[0][0] == 0);
  REQUIRE_THAT(result.m_trace,
               steamrot::tests::EqualsTrace(expected_trace, formatter));
}

TEST_CASE("ChainDescriptorBuilder WhileIsTrue semantics are per-step and "
          "path-local",
          "[unit][descriptors][chain_builder]") {
  TerminalDescriptorFormatter formatter;
  ChainDescriptor descriptor = ChainDescriptorBuilder{}
                                   .WhileIsTrue(is_serial)
                                   .Then(is_terminal)
                                   .Build("while_then_terminal");

  steamrot::tests::TestPartLibrary lib = steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  SECTION("zero-match WhileIsTrue fails (one-or-more required)") {
    steamrot::MachinaFormScaffold scaffold = builder.GetScenarioForAnalysis(
        steamrot::tests::ScaffoldScenario::IsolatedPair);
    ChainDescriptorResult result = descriptor(scaffold.parts, 0);

    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{}
            .ScopeBegin("while_then_terminal", ScopeKind::Chain, 0, 0)
            .NodeEval(0, "is_serial", 1)
            .NodeResult(0, "is_serial", false,
                        "connection_count=1, expected==2", 1)
            .ScopeEnd("while_then_terminal", ScopeKind::Chain, false, 0)
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE(result.invalid_subgraphs.size() == 1);
    REQUIRE(result.invalid_subgraphs[0].empty());
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace, formatter));
  }

  SECTION("branching from a consumed WhileIsTrue node evaluates branches "
          "independently") {
    steamrot::MachinaFormScaffold scaffold = builder.GetScenarioForAnalysis(
        steamrot::tests::ScaffoldScenario::LinearChain);
    ChainDescriptorResult result = descriptor(scaffold.parts, 2);

    REQUIRE(result);
    REQUIRE(result.valid_subgraphs.size() == 2);
    REQUIRE(result.invalid_subgraphs.empty());

    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{}
            .ScopeBegin("while_then_terminal", ScopeKind::Chain, 0, 2)
            .NodeEval(2, "is_serial", 1)
            .NodeResult(2, "is_serial", true, "connection_count=2, expected==2",
                        1)
            .MovingToNeighbour(2, 0, 0, 1)
            .NodeEval(0, "is_serial", 2)
            .NodeResult(0, "is_serial", false,
                        "connection_count=1, expected==2", 2)
            .NodeEval(0, "is_terminal", 2)
            .NodeResult(0, "is_terminal", true,
                        "connection_count=1, expected==1", 2)
            .ValidSubgraphIsolated(2)
            .Backtracking(0, 1)
            .MovingToNeighbour(2, 1, 1, 1)
            .NodeEval(1, "is_serial", 2)
            .NodeResult(1, "is_serial", false,
                        "connection_count=1, expected==2", 2)
            .NodeEval(1, "is_terminal", 2)
            .NodeResult(1, "is_terminal", true,
                        "connection_count=1, expected==1", 2)
            .ValidSubgraphIsolated(2)
            .Backtracking(1, 1)
            .ScopeEnd("while_then_terminal", ScopeKind::Chain, true, 0)
            .Build();

    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace, formatter));
  }
}

TEST_CASE("ChainDescriptorBuilder DFS is cycle-safe for serial->terminal chain",
          "[unit][descriptors][chain_builder]") {
  ChainDescriptor descriptor = ChainDescriptorBuilder{}
                                   .WhileIsTrue(is_serial)
                                   .Then(is_terminal)
                                   .Build("cycle_safe_chain");

  steamrot::tests::TestPartLibrary lib = steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};
  steamrot::MachinaFormScaffold scaffold =
      builder.GetScenarioForAnalysis(steamrot::tests::ScaffoldScenario::Ring);

  ChainDescriptorResult result = descriptor(scaffold.parts, 0);

  REQUIRE_FALSE(result);
  REQUIRE(result.valid_subgraphs.empty());
}
