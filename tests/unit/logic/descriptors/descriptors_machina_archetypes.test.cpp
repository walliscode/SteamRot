/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the MachinaArchetype library
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "descriptors_machina_archetypes.h"
#include "AnalysisTraceBuilder.h"
#include "PartGraphBuilder.h"
#include "TerminalDescriptorFormatter.h"
#include "TraceEqualsMatcher.h"
#include "descriptors_node_descriptors.h"
#include <catch2/catch_test_macros.hpp>

using namespace steamrot::logic::descriptors;
TEST_CASE("MachinaArchetype Grab tests") {

  // some general assertions about the descriptor instance
  REQUIRE(grab.GetName() == "Grab");

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
            .ScopeEnd(grab.GetName(), ScopeKind::MachinaArchetype, true, 0)
            .Build();

    // test predicate
    MachinaArchetypeResult result =
        grab(pkg.part_graph, pkg.id_to_part_graph_id.at("j0"));

    // assert result and trace
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
    REQUIRE(result);
  }

  SECTION("Grab offsets trace depth when requested") {

    steamrot::tests::PartGraphPackage pkg =
        steamrot::tests::PartGraphBuilder{}
            .AddJoint(steamrot::tests::JointNames::TwoSockets, "j0")
            .Build();

    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            .ScopeBegin(grab.GetName(), ScopeKind::MachinaArchetype, 2, "j0")
            .NodeEval("j0", is_joint.GetName(), 3)
            .NodeResult("j0", is_joint.GetName(), true,
                        "node holds JointInstance", 3)
            .ScopeEnd(grab.GetName(), ScopeKind::MachinaArchetype, true, 2)
            .Build();

    MachinaArchetypeResult result =
        grab(pkg.part_graph, pkg.id_to_part_graph_id.at("j0"), 2);

    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
    REQUIRE(result);
  }
}
