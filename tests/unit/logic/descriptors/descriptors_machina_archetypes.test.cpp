/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the MachinaArchetype library
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "descriptors_machina_archetypes.h"
#include "AnalysisEvent.h"
#include "AnalysisTraceBuilder.h"
#include "DescriptorResult.h"
#include "PartGraphBuilder.h"
#include "TerminalDescriptorFormatter.h"
#include "TraceEqualsMatcher.h"
#include "descriptors_chain_descriptors.h"
#include "descriptors_node_descriptors.h"
#include <catch2/catch_test_macros.hpp>
#include <variant>

using namespace steamrot::logic::descriptors;
TEST_CASE("MachinaArchetype Grab tests") {

  // some general assertions about the descriptor instance
  REQUIRE(MA::Grab().GetName() == "Grab");
  REQUIRE(MA::Grab().GetNumberOfSteps() == 2);

  SECTION("Grab returns false on empty graph") {
    // test predicate
    steamrot::PartGraph empty_graph;
    MachinaArchetypeResult result = MA::Grab()(empty_graph, 0);

    // build expected trace
    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{{}}.EmptyPartGraph().Build();

    // assert result and trace
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
    REQUIRE_FALSE(result);
  }

  SECTION("Grab evalutes a single joint node graph") {

    // build part graph with a single joint node
    steamrot::tests::PartGraphPackage pkg =
        steamrot::tests::PartGraphBuilder{}
            .AddJoint(steamrot::tests::JointNames::TwoSockets, "j0")
            .Build();

    // build expected trace
    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            .ScopeBegin(MA::Grab().GetName(), ScopeKind::MachinaArchetype, 0,
                        "j0")
            .ScopeBegin(is_joint_chain().GetName(), ScopeKind::Chain, 1, "j0")
            .NodeEval("j0", is_joint().GetName(), true, 2,
                      "node holds JointInstance")
            .ScopeEnd(is_joint_chain().GetName(), ScopeKind::Chain, true, 1)
            .MachinaPartResult(is_joint_chain().GetName(), 1)
            .MachinaPartResult(is_serial_chain_with_minimum_length_3().GetName(),
                               false, 0)
            .ScopeEnd(MA::Grab().GetName(), ScopeKind::MachinaArchetype, false, 0)
            .Build();

    // test predicate
    MachinaArchetypeResult result =
        MA::Grab()(pkg.part_graph, pkg.id_to_part_graph_id.at("j0"), 0);

    // assert result and trace
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
    REQUIRE_FALSE(result);
  }

  SECTION("Grab evalutes a serial chain of length 2") {
    // build part graph with a serial chain of length 2
    steamrot::tests::PartGraphPackage pkg =
        steamrot::tests::PartGraphBuilder{}
            .AddJoint(steamrot::tests::JointNames::TwoSockets, "j0")
            .AddFragment(steamrot::tests::FragmentNames::OneSocket, "f0")
            .Connect("j0", 0, "f0", 0) // j0.socket[0] ↔ f0.socket[0]
            .Build();
    // test predicate
    MachinaArchetypeResult result =
        MA::Grab()(pkg.part_graph, pkg.id_to_part_graph_id.at("j0"), 0);

    // build expected trace
    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            .ScopeBegin(MA::Grab().GetName(), ScopeKind::MachinaArchetype, 0,
                        "j0")
            .ScopeBegin(is_joint_chain().GetName(), ScopeKind::Chain, 1, "j0")
            .NodeEval("j0", is_joint().GetName(), true, 2,
                      "node holds JointInstance")
            .ScopeEnd(is_joint_chain().GetName(), ScopeKind::Chain, true, 1)
            .MachinaPartResult(is_joint_chain().GetName(), true, 0)
            .ScopeBegin(is_serial_chain_with_minimum_length_3().GetName(),
                        ScopeKind::Chain, 1, "f0")
            .NodeEval("f0", is_serial().GetName(), false, 2,
                      "connection_count=1, expected==2")
            .ScopeEnd(is_serial_chain_with_minimum_length_3().GetName(),
                      ScopeKind::Chain, false, 1)
            .MachinaPartResult(is_serial_chain_with_minimum_length_3().GetName(),
                               false, 0)
            .ScopeEnd(MA::Grab().GetName(), ScopeKind::MachinaArchetype, false, 0)
            .Build();
    // assert result
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
    REQUIRE_FALSE(result);
  }

  SECTION(
      "Grab evaluates a chain of length 3 with a joint as the middle node") {
    // build part graph with a serial chain of length 3 with a joint as the
    // middle node
    steamrot::tests::PartGraphPackage pkg =
        steamrot::tests::PartGraphBuilder{}
            .AddJoint(steamrot::tests::JointNames::TwoSockets, "j0")
            .AddFragment(steamrot::tests::FragmentNames::OneSocket, "f0")
            .AddFragment(steamrot::tests::FragmentNames::OneSocket, "f1")
            .Connect("j0", 0, "f0", 0) // j0.socket[0] ↔ f0.socket[0]
            .Connect("j0", 1, "f1", 0) // j0.socket[1] ↔ f1.socket[0]
            .Build();
    // test predicate
    MachinaArchetypeResult result =
        MA::Grab()(pkg.part_graph, pkg.id_to_part_graph_id.at("j0"), 0);
    // build expected trace
    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            .ScopeBegin(MA::Grab().GetName(), ScopeKind::MachinaArchetype, 0,
                        "j0")
            .ScopeBegin(is_joint_chain().GetName(), ScopeKind::Chain, 1, "j0")
            .NodeEval("j0", is_joint().GetName(), true, 2,
                      "node holds JointInstance")
            .ScopeEnd(is_joint_chain().GetName(), ScopeKind::Chain, true, 1)
            .MachinaPartResult(is_joint_chain().GetName(), true, 0)
            .ScopeBegin(is_serial_chain_with_minimum_length_3().GetName(),
                        ScopeKind::Chain, 1, "f0")
            .NodeEval("f0", is_serial().GetName(), false, 2,
                      "connection_count=1, expected==2")
            .ScopeEnd(is_serial_chain_with_minimum_length_3().GetName(),
                      ScopeKind::Chain, false, 1)
            .ScopeBegin(is_serial_chain_with_minimum_length_3().GetName(),
                        ScopeKind::Chain, 1, "f1")
            .NodeEval("f1", is_serial().GetName(), false, 2,
                      "connection_count=1, expected==2")
            .ScopeEnd(is_serial_chain_with_minimum_length_3().GetName(),
                      ScopeKind::Chain, false, 1)
            .MachinaPartResult(is_serial_chain_with_minimum_length_3().GetName(),
                               false, 0)
            .ScopeEnd(MA::Grab().GetName(), ScopeKind::MachinaArchetype, false, 0)
            .Build();
    // assert
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
    REQUIRE_FALSE(result);
  }

  SECTION(
      "Grab evaluates a chain of length 5 with a joint as the middle node") {
    // build part graph with a serial chain of length 5 with a joint as the
    // middle node
    steamrot::tests::PartGraphPackage pkg =
        steamrot::tests::PartGraphBuilder{}
            .AddJoint(steamrot::tests::JointNames::TwoSockets, "j0")
            .AddFragment(steamrot::tests::FragmentNames::TwoSockets, "f0")
            .AddJoint(steamrot::tests::JointNames::OneSocket, "j1")
            .AddFragment(steamrot::tests::FragmentNames::TwoSockets, "f1")
            .AddJoint(steamrot::tests::JointNames::OneSocket, "j2")
            .Connect("j0", 0, "f0", 0) // j0.socket[0] ↔ f0.socket[0]
            .Connect("f0", 1, "j1", 0) // f0.socket[1] ↔ j1.socket[0]
            .Connect("j0", 1, "f1", 0) // j0.socket[1] ↔ f1.socket[0]
            .Connect("f1", 1, "j2", 0) // f1.socket[1] ↔ j2.socket[0]
            .Build();
    // test predicate
    MachinaArchetypeResult result =
        MA::Grab()(pkg.part_graph, pkg.id_to_part_graph_id.at("j0"), 0);
    // build expected trace
    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            .ScopeBegin(MA::Grab().GetName(), ScopeKind::MachinaArchetype, 0,
                        "j0")
            .ScopeBegin(is_joint_chain().GetName(), ScopeKind::Chain, 1, "j0")
            .NodeEval("j0", is_joint().GetName(), true, 2,
                      "node holds JointInstance")
            .ScopeEnd(is_joint_chain().GetName(), ScopeKind::Chain, true, 1)
            .MachinaPartResult(is_joint_chain().GetName(), true, 0)
            .ScopeBegin(is_serial_chain_with_minimum_length_3().GetName(),
                        ScopeKind::Chain, 1, "f0")
            .NodeEval("f0", is_serial().GetName(), true, 2,
                      "connection_count=2, expected==2")
            .MovingToNeighbour("f0", 1, "j1", 0, 2)
            .NodeEval("j1", is_serial().GetName(), false, 3,
                      "connection_count=1, expected==2")
            .Backtracking("j1", 0, "f0", 1, 2)
            .ScopeEnd(is_serial_chain_with_minimum_length_3().GetName(),
                      ScopeKind::Chain, false, 1)
            .ScopeBegin(is_serial_chain_with_minimum_length_3().GetName(),
                        ScopeKind::Chain, 1, "f1")
            .NodeEval("f1", is_serial().GetName(), true, 2,
                      "connection_count=2, expected==2")
            .MovingToNeighbour("f1", 1, "j2", 0, 2)
            .NodeEval("j2", is_serial().GetName(), false, 3,
                      "connection_count=1, expected==2")
            .Backtracking("j2", 0, "f1", 1, 2)
            .ScopeEnd(is_serial_chain_with_minimum_length_3().GetName(),
                      ScopeKind::Chain, false, 1)
            .MachinaPartResult(is_serial_chain_with_minimum_length_3().GetName(),
                               false, 0)
            .ScopeEnd(MA::Grab().GetName(), ScopeKind::MachinaArchetype, false, 0)
            .Build();
    // assert
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
    REQUIRE_FALSE(result);
  }

  SECTION(
      "Grab evaluates a chain of length 7 with a joint as the middle node") {
    // build part graph with a serial chain of length 7 with a joint as the
    // middle node
    steamrot::tests::PartGraphPackage pkg =
        steamrot::tests::PartGraphBuilder{}
            .AddFragment(steamrot::tests::FragmentNames::TwoSockets, "f0")
            .AddJoint(steamrot::tests::JointNames::TwoSockets, "j1")
            .AddFragment(steamrot::tests::FragmentNames::TwoSockets, "f2")
            .AddJoint(steamrot::tests::JointNames::TwoSockets, "j3")
            .AddFragment(steamrot::tests::FragmentNames::TwoSockets, "f4")
            .AddJoint(steamrot::tests::JointNames::TwoSockets, "j5")
            .AddFragment(steamrot::tests::FragmentNames::TwoSockets, "f6")
            .Connect("f0", 0, "j1", 0)
            .Connect("j1", 1, "f2", 0)
            .Connect("f2", 1, "j3", 0)
            .Connect("j3", 1, "f4", 0)
            .Connect("f4", 1, "j5", 0)
            .Connect("j5", 1, "f6", 0)
            .Build();
    // test predicate, feed it the middle joint node
    MachinaArchetypeResult result =
        MA::Grab()(pkg.part_graph, pkg.id_to_part_graph_id.at("j3"), 0);

    // build expected trace: we expect the joint node to be found and two chains
    // of length 3 to be found which should suceed
    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            .ScopeBegin(MA::Grab().GetName(), ScopeKind::MachinaArchetype, 0,
                        "j3")
            .ScopeBegin(is_joint_chain().GetName(), ScopeKind::Chain, 1, "j3")
            .NodeEval("j3", is_joint().GetName(), true, 2,
                      "node holds JointInstance")
            .ScopeEnd(is_joint_chain().GetName(), ScopeKind::Chain, true, 1)
            .MachinaPartResult(is_joint_chain().GetName(), true, 0)
            .ScopeBegin(is_serial_chain_with_minimum_length_3().GetName(),
                        ScopeKind::Chain, 1, "f2")
            .NodeEval("f2", is_serial().GetName(), true, 2,
                      "connection_count=2, expected==2")
            .MovingToNeighbour("f2", 0, "j1", 1, 2)
            .NodeEval("j1", is_serial().GetName(), true, 3,
                      "connection_count=2, expected==2")
            .MovingToNeighbour("j1", 0, "f0", 0, 3)
            .NodeEval("f0", is_serial().GetName(), false, 4,
                      "connection_count=1, expected==2")
            .NodeEval("f0", is_terminal().GetName(), true, 4,
                      "connection_count=1, expected==1")
            .Backtracking("f0", 0, "j1", 0, 3)
            .Backtracking("j1", 1, "f2", 0, 2)
            .ScopeEnd(is_serial_chain_with_minimum_length_3().GetName(),
                      ScopeKind::Chain, true, 1)
            .ScopeBegin(is_serial_chain_with_minimum_length_3().GetName(),
                        ScopeKind::Chain, 1, "f4")
            .NodeEval("f4", is_serial().GetName(), true, 2,
                      "connection_count=2, expected==2")
            .MovingToNeighbour("f4", 1, "j5", 0, 2)
            .NodeEval("j5", is_serial().GetName(), true, 3,
                      "connection_count=2, expected==2")
            .MovingToNeighbour("j5", 1, "f6", 0, 3)
            .NodeEval("f6", is_serial().GetName(), false, 4,
                      "connection_count=1, expected==2")
            .NodeEval("f6", is_terminal().GetName(), true, 4,
                      "connection_count=1, expected==1")
            .Backtracking("f6", 0, "j5", 1, 3)
            .Backtracking("j5", 0, "f4", 1, 2)
            .ScopeEnd(is_serial_chain_with_minimum_length_3().GetName(),
                      ScopeKind::Chain, true, 1)
            .MachinaPartResult(is_serial_chain_with_minimum_length_3().GetName(),
                               true, 0)
            .ScopeEnd(MA::Grab().GetName(), ScopeKind::MachinaArchetype, true, 0)
            .Build();

    // assert
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
    REQUIRE(result);

    // inspect the GrabResult
    const GrabResult *grab_result =
        std::get_if<GrabResult>(&result.result_sub_graphs);
    REQUIRE(grab_result);
    REQUIRE(grab_result->anchor.size() == 1);
    REQUIRE(grab_result->anchor[0] == pkg.id_to_part_graph_id.at("j3"));
    REQUIRE(grab_result->arms.size() == 2);
  }
}
