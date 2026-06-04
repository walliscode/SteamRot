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
#include "PartGraphBuilder.h"
#include "TerminalDescriptorFormatter.h"
#include "TraceEqualsMatcher.h"
#include "descriptors_chain_descriptors.h"
#include "descriptors_node_descriptors.h"
#include <catch2/catch_test_macros.hpp>

using namespace steamrot::logic::descriptors;
TEST_CASE("MachinaArchetype Grab tests") {

  // some general assertions about the descriptor instance
  REQUIRE(grab.GetName() == "Grab");
  REQUIRE(grab.GetNumberOfSteps() == 2);

  SECTION("Grab returns false on empty graph") {
    // test predicate
    steamrot::PartGraph empty_graph;
    MachinaArchetypeResult result = grab(empty_graph, 0);

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
            .ScopeBegin(grab.GetName(), ScopeKind::MachinaArchetype, 0, "j0")
            .NodeEval("j0", is_joint.GetName(), 1)
            .NodeResult("j0", is_joint.GetName(), true,
                        "node holds JointInstance", 1)
            .MachinaPartResult(is_joint.GetName(), 1)
            .MachinaPartResult(is_serial_chain_with_minimum_length_2.GetName(),
                               false, 0)
            .ScopeEnd(grab.GetName(), ScopeKind::MachinaArchetype, false, 0)
            .Build();

    // test predicate
    MachinaArchetypeResult result =
        grab(pkg.part_graph, pkg.id_to_part_graph_id.at("j0"), 0);

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
            .Build();
    // test predicate
    MachinaArchetypeResult result =
        grab(pkg.part_graph, pkg.id_to_part_graph_id.at("j0"), 0);

    // build expected trace
    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            .ScopeBegin(grab.GetName(), ScopeKind::MachinaArchetype, 0, "j0")
            .NodeEval("j0", is_joint.GetName(), 1)
            .NodeResult("j0", is_joint.GetName(), true,
                        "node holds JointInstance", 1)
            .MachinaPartResult(is_joint.GetName(), true, 0)
            .ScopeBegin(is_serial_chain_with_minimum_length_2.GetName(),
                        ScopeKind::Chain, 1, "f0")
            .ScopeEnd(is_serial_chain_with_minimum_length_2.GetName(),
                      ScopeKind::Chain, false, 2)
            .MachinaPartResult(is_serial_chain_with_minimum_length_2.GetName(),
                               false, 0)
            .ScopeEnd(grab.GetName(), ScopeKind::MachinaArchetype, false, 0)
            .Build();
    // assert result
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
    REQUIRE_FALSE(result);
  }
}
