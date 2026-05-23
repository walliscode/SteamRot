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
#include "descriptors_node_descriptors.h"
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
            // node 2 (joint with 2 connections) satisfies is_serial
            .NodeEval(2, "is_serial", 1)
            .NodeResult(2, "is_serial", true, "connection_count=2, expected==2",
                        1)
            // socket[0] of joint leads to frag0 (id=0)
            .MovingToNeighbour(2, 0, 0, 1)
            // frag0 has 1 connection, so is_serial fails
            .NodeEval(0, "is_serial", 2)
            .NodeResult(0, "is_serial", false,
                        "connection_count=1, expected==2", 2)
            // WhileIsTrue consumed at least one node, so re-evaluate frag0
            // against the next step (is_terminal) without consuming it first
            .NodeEval(0, "is_terminal", 2)
            .NodeResult(0, "is_terminal", true,
                        "connection_count=1, expected==1", 2)
            .Backtracking(0, 1)
            // socket[1] of joint leads to frag1 (id=1)
            .MovingToNeighbour(2, 1, 1, 1)
            // frag1 also has 1 connection, same pattern as frag0
            .NodeEval(1, "is_serial", 2)
            .NodeResult(1, "is_serial", false,
                        "connection_count=1, expected==2", 2)
            .NodeEval(1, "is_terminal", 2)
            .NodeResult(1, "is_terminal", true,
                        "connection_count=1, expected==1", 2)
            .Backtracking(1, 1)
            .ScopeEnd("is_serial_chain", ScopeKind::Chain, true, 0)
            .Build();

    // assert result and trace
    REQUIRE(result);
    REQUIRE(result.valid_subgraphs.size() == 2);
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }

  SECTION("is_serial_chain evalutes serial chain that is 5 parts long") {
    // test predicate
    const steamrot::tests::ScaffoldResult scaffold_result =
        builder.MakeConnectedScaffold(
            {"fragment_two_sockets", "fragment_two_sockets",
             "fragment_two_sockets"},
            {"joint_two_sockets", "joint_two_sockets"},
            {{0, 0, 3, 0},   // fragment[0].socket[1] -> joint[0].socket[0]
             {3, 1, 1, 0},   // joint[0].socket[1]    -> fragment[1].socket[0]
             {1, 1, 4, 0},   // fragment[1].socket[1] -> joint[1].socket[0]
             {4, 1, 2, 0}}); // joint[1].socket[1]    -> fragment[2].socket[0]
    const steamrot::PartGraph &parts = scaffold_result.scaffold.parts;

    SECTION("is_serial_chain evaluates from the start of the chain") {
      // build expected trace
      AnalysisTrace expected_trace =
          steamrot::tests::AnalysisTraceBuilder{}
              .ScopeBegin(is_serial_chain.GetName(), ScopeKind::Chain, 0, 0)
              .NodeEval(0, is_serial.GetName(), 1)
              .NodeResult(0, is_serial.GetName(), false,
                          "connection_count=1, expected==2", 1)
              .ScopeEnd(is_serial_chain.GetName(), ScopeKind::Chain, false, 0)
              .Build();
      ChainDescriptorResult result = is_serial_chain(parts, 0);
      // assert result
      REQUIRE_FALSE(result);
      REQUIRE(result.valid_subgraphs.size() == 0);
      REQUIRE_THAT(result.m_trace,
                   steamrot::tests::EqualsTrace(expected_trace,
                                                TerminalDescriptorFormatter{}));
    }

    SECTION("is_serial_chain evalutes from the second node") {
      // build expected trace
      AnalysisTrace expected_trace =
          steamrot::tests::AnalysisTraceBuilder{}
              .ScopeBegin(is_serial_chain.GetName(), ScopeKind::Chain, 0, 3)
              .NodeEval(3, is_serial.GetName(), 1)
              .NodeResult(3, is_serial.GetName(), true,
                          "connection_count=2, expected==2", 1)
              .MovingToNeighbour(3, 0, 0, 1)
              .NodeEval(0, is_serial.GetName(), 2)
              .NodeResult(0, is_serial.GetName(), false,
                          "connection_count=1, expected==2", 2)
              .NodeEval(0, is_terminal.GetName(), 2)
              .NodeResult(0, is_terminal.GetName(), true,
                          "connection_count=1, expected==1", 2)
              .Backtracking(0, 1)
              .MovingToNeighbour(3, 1, 1, 1)
              .NodeEval(1, is_serial.GetName(), 2)
              .NodeResult(1, is_serial.GetName(), true,
                          "connection_count=2, expected==2", 2)
              .MovingToNeighbour(1, 4, 1, 2)
              .NodeEval(0, "is_terminal", 3)
              .NodeResult(0, "is_terminal", true,
                          "connection_count=1, expected==1", 3)
              .Backtracking(0, 1)
              .MovingToNeighbour(3, 1, 4, 0)
              .NodeEval(4, is_serial.GetName(), 3)
              .NodeResult(4, is_serial.GetName(), true,
                          "connection_count=2, expected==2", 3)
              .MovingToNeighbour(4, 0, 2, 0)
              .NodeEval(2, is_serial.GetName(), 4)
              .NodeResult(2, is_serial.GetName(), false,
                          "connection_count=1, expected==2", 4)
              .NodeEval(2, "is_terminal", 4)
              .NodeResult(2, "is_terminal", true,
                          "connection_count=1, expected==1", 4)
              .Backtracking(2, 1)
              .Backtracking(4, 1)
              .Backtracking(3, 1)
              .ScopeEnd(is_serial_chain.GetName(), ScopeKind::Chain, true, 0)
              .Build();
      ChainDescriptorResult result = is_serial_chain(parts, 3);
      // assert result/
      REQUIRE_THAT(result.m_trace,
                   steamrot::tests::EqualsTrace(expected_trace,
                                                TerminalDescriptorFormatter{}));
      REQUIRE(result);
      REQUIRE(result.valid_subgraphs.size() == 1);
    }
  }
}
