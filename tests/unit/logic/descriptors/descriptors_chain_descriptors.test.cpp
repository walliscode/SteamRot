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

  // arrange test library
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  // some general assertions about the descriptor instance
  REQUIRE(is_serial_chain.GetName() == "is_serial_chain");

  SECTION("is_serial_chain returns result and correct trace with empty part "
          "graph") {
    // test predicate
    steamrot::PartGraph empty_graph;
    ChainDescriptorResult result = is_serial_chain(empty_graph, 0);

    // build expected trace
    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{}.EmptyPartGraph().Build();

    // assert result and trace
    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }

  SECTION("is_serial_chain evaluates IsolatedPair") {
    // test predicate
    steamrot::MachinaFormScaffold scaffold = builder.GetScenarioForAnalysis(
        steamrot::tests::ScaffoldScenario::IsolatedPair);
    steamrot::PartGraph &parts = scaffold.parts;

    ChainDescriptorResult result = is_serial_chain(parts, 0);

    // build expected trace
    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{}
            .ScopeBegin("is_serial_chain", ScopeKind::Chain, 0, 0)
            .NodeEval(0, "is_serial", 1)
            .NodeResult(0, "is_serial", false,
                        "connection_count=1, expected==2", 1)
            .ScopeEnd("is_serial_chain", ScopeKind::Chain, false, 0)
            .Build();

    // assert result and trace
    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }

  SECTION("is_serial_chain evaluates LinearChain") {
    // test predicate
    steamrot::MachinaFormScaffold scaffold = builder.GetScenarioForAnalysis(
        steamrot::tests::ScaffoldScenario::LinearChain);
    steamrot::PartGraph &parts = scaffold.parts;

    // feed the middle joint node of the LinearChain to prevent it failing
    // straight away on the first node due to the scenarios are made, the middle
    // joint node has id 2, but to be safe we can check if it exists first
    auto it = parts.find(2);
    REQUIRE(it != parts.end());

    ChainDescriptorResult result = is_serial_chain(parts, 2);

    // build expected trace
    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{} // begin chain at node 2
            .ScopeBegin("is_serial_chain", ScopeKind::Chain, 0, 2)
            // evaluate node 2 against is_serial
            .NodeEval(2, "is_serial", 1)
            // this should pass since node 2 has 2 connections
            .NodeResult(2, "is_serial", true, "connection_count=2, expected==2",
                        1)
            // move to first node connected to node 2, which should be node 0
            .MovingToNeighbour(2, 0, 0, 1)
            // evaluate node 0 against is_serial
            .NodeEval(0, "is_serial", 2)
            // this should fail since node 0 has only 1 connection
            .NodeResult(0, "is_serial", false,
                        "connection_count=1, expected==2", 2)
            // as this is a WhileIsTrue step, so as long as one node in the
            // chain was correct, it moves to the next step and evaluates the
            // same node
            .NodeEval(0, "is_terminal", 2)
            .NodeResult(0, "is_terminal", true,
                        "connection_count=1, expected==1", 1)
            .ScopeEnd("is_serial_chain", ScopeKind::Chain, true, 0)
            .Build();

    // assert result and trace

    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
    REQUIRE(result);
  }
}
