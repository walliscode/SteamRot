/////////////////////////////////////////////////
/// @file
/// @brief Units tests for the ChainDescriptor class and related free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "descriptors_chain_descriptors.h"
#include "AnalysisEvent.h"
#include "AnalysisTraceBuilder.h"
#include "MachinaFormScaffold.h"
#include "TerminalDescriptorFormatter.h"
#include "TraceEqualsMatcher.h"
#include "part_library.h"
#include <catch2/catch_test_macros.hpp>

namespace {
constexpr uint32_t kMissingPartId{9999};
}

TEST_CASE("ChainDescriptor is_serial_chain") {

  using namespace steamrot::logic::descriptors;

  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  REQUIRE(is_serial_chain.GetName() == "is_serial_chain");

  SECTION("is_serial_chain returns result and correct trace with empty part "
          "graph") {
    steamrot::PartGraph empty_graph;
    ChainDescriptorResult result = is_serial_chain(empty_graph, 0);

    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{}
            .ScopeBegin("is_serial_chain", ScopeKind::Chain, 0, 0)
            .EmptyPartGraph()
            .ScopeEnd("is_serial_chain", ScopeKind::Chain, false, 0)
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }

  SECTION("is_serial_chain evaluates IsolatedPair (WhileIsTrue one-or-more)") {
    steamrot::MachinaFormScaffold scaffold = builder.GetScenarioForAnalysis(
        steamrot::tests::ScaffoldScenario::IsolatedPair);
    steamrot::PartGraph &parts = scaffold.parts;

    ChainDescriptorResult result = is_serial_chain(parts, 0);

    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{}
            .ScopeBegin("is_serial_chain", ScopeKind::Chain, 0, 0)
            .NodeEval(0, "is_serial", 1)
            .NodeResult(0, "is_serial", false,
                        "connection_count=1, expected==2", 1)
            .ScopeEnd("is_serial_chain", ScopeKind::Chain, false, 0)
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE(result.invalid_subgraphs.size() == 1);
    REQUIRE(result.invalid_subgraphs[0].empty());
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }

  SECTION("is_serial_chain evaluates LinearChain with deterministic branching") {
    steamrot::MachinaFormScaffold scaffold = builder.GetScenarioForAnalysis(
        steamrot::tests::ScaffoldScenario::LinearChain);
    steamrot::PartGraph &parts = scaffold.parts;

    auto it = parts.find(2);
    REQUIRE(it != parts.end());
    ChainDescriptorResult result = is_serial_chain(parts, 2);

    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{}
            .ScopeBegin("is_serial_chain", ScopeKind::Chain, 0, 2)
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
            .ScopeEnd("is_serial_chain", ScopeKind::Chain, true, 0)
            .Build();

    REQUIRE(result);
    REQUIRE(result.valid_subgraphs.size() == 2);
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }

  SECTION("is_serial_chain handles missing anchor via node trace events") {
    steamrot::MachinaFormScaffold scaffold = builder.GetScenarioForAnalysis(
        steamrot::tests::ScaffoldScenario::LinearChain);
    steamrot::PartGraph &parts = scaffold.parts;

    ChainDescriptorResult result = is_serial_chain(parts, kMissingPartId);

    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{}
            .ScopeBegin("is_serial_chain", ScopeKind::Chain, 0, kMissingPartId)
            .NodeEval(kMissingPartId, "is_serial", 1)
            .NodeResult(kMissingPartId, "is_serial", false,
                        "incorrect key: part_id=9999", 1)
            .ScopeEnd("is_serial_chain", ScopeKind::Chain, false, 0)
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE(result.invalid_subgraphs.size() == 1);
    REQUIRE(result.invalid_subgraphs[0].empty());
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }
}
