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
#include "PartGraphBuilder.h"
#include "TerminalDescriptorFormatter.h"
#include "TraceEqualsMatcher.h"
#include "descriptors_node_descriptors.h"
#include "part_graph_library.h"

#include <catch2/catch_test_macros.hpp>
#include <vector>

namespace {
constexpr uint32_t kMissingPartId{9999};
}

TEST_CASE("ChainDescriptor is_serial_chain tests") {

  using namespace steamrot::logic::descriptors;

  // some general assertions about the descriptor instance
  REQUIRE(is_serial_chain.GetName() == "is_serial_chain");

  SECTION("is_serial_chain returns result and correct trace with empty part "
          "graph") {
    // test predicate
    steamrot::PartGraph empty_graph;
    ChainDescriptorResult result = is_serial_chain(empty_graph, 0);

    // build expected trace
    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{{}}.EmptyPartGraph().Build();

    // assert result and trace
    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }

  SECTION("is_serial_chain evaluates IsolatedPair") {
    // test predicate: node 0 (fragment, connection_count=1) fails is_serial
    const steamrot::PartGraph &parts = steamrot::tests::pair.part_graph;

    ChainDescriptorResult result = is_serial_chain(
        parts, steamrot::tests::pair.id_to_part_graph_id.at("f0"));

    // build expected trace
    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{
            steamrot::tests::pair.id_to_part_graph_id}
            .ScopeBegin("is_serial_chain", ScopeKind::Chain, 0, "f0")
            .NodeEval("f0", "is_serial", 1)
            .NodeResult("f0", "is_serial", false,
                        "connection_count=1, expected==2", 1)
            .ScopeEnd("is_serial_chain", ScopeKind::Chain, false, 0)
            .Build();

    // assert result and trace
    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }

  SECTION("is_serial_chain offsets trace depth when requested") {
    const steamrot::PartGraph &parts = steamrot::tests::pair.part_graph;

    ChainDescriptorResult result = is_serial_chain(
        parts, steamrot::tests::pair.id_to_part_graph_id.at("f0"), 3);

    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{
            steamrot::tests::pair.id_to_part_graph_id}
            .ScopeBegin("is_serial_chain", ScopeKind::Chain, 3, "f0")
            .NodeEval("f0", "is_serial", 4)
            .NodeResult("f0", "is_serial", false,
                        "connection_count=1, expected==2", 4)
            .ScopeEnd("is_serial_chain", ScopeKind::Chain, false, 3)
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }

  SECTION("is_serial_chain evaluates LinearChain") {
    // test predicate: f0(id=0, terminal) ─ j0(id=2, serial) ─ f1(id=1,
    // terminal)
    //   f0.socket[1] ↔ j0.socket[0]
    //   j0.socket[1]  ↔ f1.socket[0]
    const steamrot::tests::PartGraphPackage pkg =
        steamrot::tests::PartGraphBuilder{}
            .AddFragment(steamrot::tests::FragmentNames::TwoSockets,
                         "f0") // id=0
            .AddFragment(steamrot::tests::FragmentNames::TwoSockets,
                         "f1")                                       // id=1
            .AddJoint(steamrot::tests::JointNames::TwoSockets, "j0") // id=2
            .Connect("f0", 1, "j0", 0) // f0.socket[1] ↔ j0.socket[0]
            .Connect("j0", 1, "f1", 0) // j0.socket[1] ↔ f1.socket[0]
            .Build();
    const steamrot::PartGraph &parts = pkg.part_graph;

    // feed the middle joint node of the LinearChain to prevent it failing
    // straight away on the first node; the joint has id=2
    ChainDescriptorResult result =
        is_serial_chain(parts, pkg.id_to_part_graph_id.at("j0"));

    // build expected trace
    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            // begin chain at node 2
            .ScopeBegin("is_serial_chain", ScopeKind::Chain, 0, "j0")
            // node 2 (joint with 2 connections) satisfies is_serial
            .NodeEval("j0", "is_serial", 1)
            .NodeResult("j0", "is_serial", true,
                        "connection_count=2, expected==2", 1)
            // socket[0] of joint leads to frag0 (id=0)
            .MovingToNeighbour("j0", 0, "f0", 1, 1)
            // frag0 has 1 connection, so is_serial fails
            .NodeEval("f0", "is_serial", 2)
            .NodeResult("f0", "is_serial", false,
                        "connection_count=1, expected==2", 2)
            // WhileIsTrue consumed at least one node, so re-evaluate frag0
            // against the next step (is_terminal) without consuming it first
            .NodeEval("f0", "is_terminal", 2)
            .NodeResult("f0", "is_terminal", true,
                        "connection_count=1, expected==1", 2)
            .Backtracking("f0", 1, "j0", 0, 1)
            .ScopeEnd("is_serial_chain", ScopeKind::Chain, true, 0)
            .Build();

    // assert result and trace
    REQUIRE(result);
    REQUIRE(result.valid_subgraph.has_value());
    REQUIRE(*result.valid_subgraph == std::vector<uint32_t>{2, 0});
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }

  SECTION("is_serial_chain evalutes serial chain that is 5 parts long") {
    // f0(id=0, terminal) ─ j0(id=3, serial) ─ f1(id=1, serial) ─ j1(id=4,
    // serial) ─ f2(id=2, terminal)
    //   f0.socket[0] ↔ j0.socket[0]
    //   j0.socket[1] ↔ f1.socket[0]
    //   f1.socket[1] ↔ j1.socket[0]
    //   j1.socket[1] ↔ f2.socket[0]
    const steamrot::tests::PartGraphPackage pkg =
        steamrot::tests::PartGraphBuilder{}
            .AddFragment(steamrot::tests::FragmentNames::OneSocket, "f0")
            .AddFragment(steamrot::tests::FragmentNames::TwoSockets, "f1")
            .AddFragment(steamrot::tests::FragmentNames::OneSocket, "f2")
            .AddJoint(steamrot::tests::JointNames::TwoSockets, "j0")
            .AddJoint(steamrot::tests::JointNames::TwoSockets, "j1")
            .Connect("f0", 0, "j0", 0) // f0.socket[0] ↔ j0.socket[0]
            .Connect("j0", 1, "f1", 0) // j0.socket[1] ↔ f1.socket[0]
            .Connect("f1", 1, "j1", 0) // f1.socket[1] ↔ j1.socket[0]
            .Connect("j1", 1, "f2", 0) // j1.socket[1] ↔ f2.socket[0]
            .Build();
    const steamrot::PartGraph &parts = pkg.part_graph;

    SECTION("is_serial_chain evaluates from the start of the chain") {
      // build expected trace
      AnalysisTrace expected_trace =
          steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
              .ScopeBegin(is_serial_chain.GetName(), ScopeKind::Chain, 0, "f0")
              .NodeEval("f0", is_serial.GetName(), 1)
              .NodeResult("f0", is_serial.GetName(), false,
                          "connection_count=1, expected==2", 1)
              .ScopeEnd(is_serial_chain.GetName(), ScopeKind::Chain, false, 0)
              .Build();
      ChainDescriptorResult result =
          is_serial_chain(parts, pkg.id_to_part_graph_id.at("f0"));
      // assert result
      REQUIRE_FALSE(result);
      REQUIRE_FALSE(result.valid_subgraph.has_value());
      REQUIRE_THAT(result.m_trace,
                   steamrot::tests::EqualsTrace(expected_trace,
                                                TerminalDescriptorFormatter{}));
    }

    SECTION("is_serial_chain evalutes from node j0") {
      // build expected trace
      AnalysisTrace expected_trace =
          steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
              .ScopeBegin(is_serial_chain.GetName(), ScopeKind::Chain, 0, "j0")
              .NodeEval("j0", is_serial.GetName(), 1)
              .NodeResult("j0", is_serial.GetName(), true,
                          "connection_count=2, expected==2", 1)
              // this is going to try socket[0] first
              .MovingToNeighbour("j0", 0, "f0", 0, 1)
              .NodeEval("f0", is_serial.GetName(), 2)
              .NodeResult("f0", is_serial.GetName(), false,
                          "connection_count=1, expected==2", 2)
              .NodeEval("f0", is_terminal.GetName(), 2)
              .NodeResult("f0", is_terminal.GetName(), true,
                          "connection_count=1, expected==1", 2)
              // this should store a valid subgraph here
              .Backtracking("f0", 0, "j0", 0, 1)
              .ScopeEnd(is_serial_chain.GetName(), ScopeKind::Chain, true, 0)
              .Build();
      ChainDescriptorResult result =
          is_serial_chain(parts, pkg.id_to_part_graph_id.at("j0"));
      // assert result/
      REQUIRE_THAT(result.m_trace,
                   steamrot::tests::EqualsTrace(expected_trace,
                                                TerminalDescriptorFormatter{}));
      REQUIRE(result);
      REQUIRE(result.valid_subgraph.has_value());
      REQUIRE(*result.valid_subgraph == std::vector<uint32_t>{3, 0});
    }
  }
}

TEST_CASE("ChainDescriptor is_joint_chain tests") {
  using namespace steamrot::logic::descriptors;

  // some general assertions about the descriptor instance
  REQUIRE(is_joint_chain.GetName() == "is_joint");

  SECTION("is_joint_chain returns false with empty part graph") {
    steamrot::PartGraph empty_graph;
    ChainDescriptorResult result = is_joint_chain(empty_graph, 0);

    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{{}}.EmptyPartGraph().Build();

    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }

  SECTION("is_joint_chain returns true for a joint node") {
    const steamrot::tests::PartGraphPackage pkg =
        steamrot::tests::PartGraphBuilder{}
            .AddJoint(steamrot::tests::JointNames::TwoSockets, "j0")
            .Build();

    ChainDescriptorResult result =
        is_joint_chain(pkg.part_graph, pkg.id_to_part_graph_id.at("j0"));

    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            .ScopeBegin(is_joint_chain.GetName(), ScopeKind::Chain, 0, "j0")
            .NodeEval("j0", is_joint.GetName(), 1)
            .NodeResult("j0", is_joint.GetName(), true,
                        "node holds JointInstance", 1)
            .ScopeEnd(is_joint_chain.GetName(), ScopeKind::Chain, true, 0)
            .Build();

    REQUIRE(result);
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }

  SECTION("is_joint_chain returns false for a fragment node") {
    const steamrot::tests::PartGraphPackage pkg =
        steamrot::tests::PartGraphBuilder{}
            .AddFragment(steamrot::tests::FragmentNames::OneSocket, "f0")
            .Build();

    ChainDescriptorResult result =
        is_joint_chain(pkg.part_graph, pkg.id_to_part_graph_id.at("f0"));

    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            .ScopeBegin(is_joint_chain.GetName(), ScopeKind::Chain, 0, "f0")
            .NodeEval("f0", is_joint.GetName(), 1)
            .NodeResult("f0", is_joint.GetName(), false,
                        "node holds FragmentInstance", 1)
            .ScopeEnd(is_joint_chain.GetName(), ScopeKind::Chain, false, 0)
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }

  SECTION("is_joint_chain offsets trace depth when requested") {
    const steamrot::tests::PartGraphPackage pkg =
        steamrot::tests::PartGraphBuilder{}
            .AddJoint(steamrot::tests::JointNames::TwoSockets, "j0")
            .Build();

    ChainDescriptorResult result =
        is_joint_chain(pkg.part_graph, pkg.id_to_part_graph_id.at("j0"), 2);

    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            .ScopeBegin(is_joint_chain.GetName(), ScopeKind::Chain, 2, "j0")
            .NodeEval("j0", is_joint.GetName(), 3)
            .NodeResult("j0", is_joint.GetName(), true,
                        "node holds JointInstance", 3)
            .ScopeEnd(is_joint_chain.GetName(), ScopeKind::Chain, true, 2)
            .Build();

    REQUIRE(result);
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }
}

TEST_CASE("ChainDescriptor is_serial_chain_with_minimum_length_2 tests") {
  using namespace steamrot::logic::descriptors;

  // some general assertions about the descriptor instance
  REQUIRE(is_serial_chain_with_minimum_length_2.GetName() ==
          "is_serial_chain_with_minimum_length_2");

  SECTION(
      "is_serial_chain_with_minimum_length_2 returns result and correct trace "
      "with empty part graph") {
    // test predicate
    steamrot::PartGraph empty_graph;
    ChainDescriptorResult result =
        is_serial_chain_with_minimum_length_2(empty_graph, 0);
    // build expected trace
    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{{}}.EmptyPartGraph().Build();
    // assert result and trace
    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }

  SECTION("is_serial_chain_with_minimum_length_2 evalutes a chain that is one "
          "node long") {
    // test predicate: node 0 (fragment, connection_count=1) fails is_serial

    const steamrot::tests::PartGraphPackage pkg =
        steamrot::tests::PartGraphBuilder{}
            .AddFragment(steamrot::tests::FragmentNames::OneSocket, "f0")
            .Build();
    ChainDescriptorResult result = is_serial_chain_with_minimum_length_2(
        pkg.part_graph, pkg.id_to_part_graph_id.at("f0"));
    // build expected trace
    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{
            steamrot::tests::pair.id_to_part_graph_id}
            .ScopeBegin("is_serial_chain_with_minimum_length_2",
                        ScopeKind::Chain, 0, "f0")
            .NodeEval("f0", "is_serial", 1)
            .NodeResult("f0", "is_serial", false,
                        "connection_count=0, expected==2", 1)
            .ScopeEnd("is_serial_chain_with_minimum_length_2", ScopeKind::Chain,
                      false, 0)
            .Build();
    // assert result and trace
    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }

  SECTION("is_serial_chain_with_minimum_length_2 evalutes a chain that is three"
          "nodes long") {
    // test predicate: f0(id=0, terminal) ─ j0(id=1, serial) ─ f1(id=2,
    // terminal)
    //   f0.socket[0] ↔ j0.socket[0]
    //   j0.socket[1]  ↔ f1.socket[0]
    const steamrot::tests::PartGraphPackage pkg =
        steamrot::tests::PartGraphBuilder{}
            .AddFragment(steamrot::tests::FragmentNames::OneSocket, "f0")
            .AddFragment(steamrot::tests::FragmentNames::OneSocket, "f1")
            .AddJoint(steamrot::tests::JointNames::TwoSockets, "j0")
            .Connect("f0", 0, "j0", 0) // f0.socket[0] ↔ j0.socket[0]
            .Connect("j0", 1, "f1", 0) // j0.socket[1] ↔ f1.socket[0]
            .Build();
    ChainDescriptorResult result = is_serial_chain_with_minimum_length_2(
        pkg.part_graph, pkg.id_to_part_graph_id.at("j0"));
    // build expected trace
    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            .ScopeBegin("is_serial_chain_with_minimum_length_2",
                        ScopeKind::Chain, 0, "j0")
            .NodeEval("j0", "is_serial", 1)
            .NodeResult("j0", "is_serial", true,
                        "connection_count=2, expected==2", 1)
            .MovingToNeighbour("j0", 0, "f0", 0, 1)
            .NodeEval("f0", "is_serial", 2)
            .NodeResult("f0", "is_serial", false,
                        "connection_count=1, expected==2", 2)
            // as the result is false without the minimum length requirement, it
            // should not try the next step and backtrack straight away
            .Backtracking("f0", 0, "j0", 0, 1)
            .MovingToNeighbour("j0", 1, "f1", 0, 1)
            .NodeEval("f1", "is_serial", 2)
            .NodeResult("f1", "is_serial", false,
                        "connection_count=1, expected==2", 2)
            .Backtracking("f1", 0, "j0", 1, 1)
            .ScopeEnd("is_serial_chain_with_minimum_length_2", ScopeKind::Chain,
                      false, 0)
            .Build();

    // assert result and trace
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
    REQUIRE_FALSE(result);
  }

  SECTION("is_serial_chain_with_minimum_length_2 evaluates a chain that is 5 "
          "nodes long") {
    // f0(id=0, terminal) ─ j0(id=3, serial) ─ f1(id=1, serial) ─ j1(id=4,
    // serial) ─ f2(id=2, terminal)
    //   f0.socket[0] ↔ j0.socket[0]
    //   j0.socket[1] ↔ f1.socket[0]
    //   f1.socket[1] ↔ j1.socket[0]
    //   j1.socket[1] ↔ f2.socket[0]
    const steamrot::tests::PartGraphPackage pkg =
        steamrot::tests::PartGraphBuilder{}
            .AddFragment(steamrot::tests::FragmentNames::OneSocket, "f0")
            .AddFragment(steamrot::tests::FragmentNames::TwoSockets, "f1")
            .AddFragment(steamrot::tests::FragmentNames::OneSocket, "f2")
            .AddJoint(steamrot::tests::JointNames::TwoSockets, "j0")
            .AddJoint(steamrot::tests::JointNames::TwoSockets, "j1")
            .Connect("f0", 0, "j0", 0) // f0.socket[0] ↔ j0.socket[0]
            .Connect("j0", 1, "f1", 0) // j0.socket[1] ↔ f1.socket[0]
            .Connect("f1", 1, "j1", 0) // f1.socket[1] ↔ j1.socket[0]
            .Connect("j1", 1, "f2", 0) // j1.socket[1] ↔ f2.socket[0]
            .Build();
    ChainDescriptorResult result = is_serial_chain_with_minimum_length_2(
        pkg.part_graph, pkg.id_to_part_graph_id.at("j0"));

    // build expected trace
    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            .ScopeBegin("is_serial_chain_with_minimum_length_2",
                        ScopeKind::Chain, 0, "j0")
            .NodeEval("j0", "is_serial", 1)
            .NodeResult("j0", "is_serial", true,
                        "connection_count=2, expected==2", 1)
            .MovingToNeighbour("j0", 0, "f0", 0, 1)
            .NodeEval("f0", "is_serial", 2)
            .NodeResult("f0", "is_serial", false,
                        "connection_count=1, expected==2", 2)
            // as the result is false without the minimum length requirement, it
            // should not try the next step and backtrack straight away
            .Backtracking("f0", 0, "j0", 0, 1)
            .MovingToNeighbour("j0", 1, "f1", 0, 1)
            .NodeEval("f1", "is_serial", 2)
            .NodeResult("f1", "is_serial", true,
                        "connection_count=2, expected==2", 2)
            .MovingToNeighbour("f1", 1, "j1", 0, 2)
            .NodeEval("j1", "is_serial", 3)
            .NodeResult("j1", "is_serial", true,
                        "connection_count=2, expected==2", 3)
            .MovingToNeighbour("j1", 1, "f2", 0, 3)
            .NodeEval("f2", "is_serial", 4)
            .NodeResult("f2", "is_serial", false,
                        "connection_count=1, expected==2", 4)
            .NodeEval("f2", "is_terminal", 4)
            .NodeResult("f2", "is_terminal", true,
                        "connection_count=1, expected==1", 4)
            .Backtracking("f2", 0, "j1", 1, 3)
            .Backtracking("j1", 0, "f1", 1, 2)
            .Backtracking("f1", 0, "j0", 1, 1)
            .ScopeEnd("is_serial_chain_with_minimum_length_2", ScopeKind::Chain,
                      true, 0)
            .Build();

    // assert result and trace
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
    REQUIRE(result);
    REQUIRE(result.valid_subgraph.has_value());
    REQUIRE(result.valid_subgraph.value() ==
            std::vector<uint32_t>{pkg.id_to_part_graph_id.at("j0"),
                                  pkg.id_to_part_graph_id.at("f1"),
                                  pkg.id_to_part_graph_id.at("j1"),
                                  pkg.id_to_part_graph_id.at("f2")});
  }
}
