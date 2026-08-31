/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the named NodeDescriptors declared in
///        descriptors_node_descriptors.h.
///
/// Each TEST_CASE covers one named predicate and uses SECTIONs for success,
/// type-mismatch failure, and (where applicable) missing-key failure.
/// Expected traces are built with AnalysisTraceBuilder and compared via the
/// TraceEqualsMatcher so that test output pinpoints the diverging event.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "descriptors_node_descriptors.h"
#include "AnalysisTraceBuilder.h"
#include "PartGraphBuilder.h"
#include "TerminalDescriptorFormatter.h"
#include "TraceEqualsMatcher.h"
#include "part_graph_library.h"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

using namespace steamrot::logic::descriptors;

TEST_CASE("is_fragment", "[unit][logic][descriptors]") {
  TerminalDescriptorFormatter kFmt{};

  SECTION("returns true for a FragmentInstance") {

    // pair: f0=id0 (fragment), j0=id1 (joint), both connected
    const NodeDescriptorResult result = is_fragment()(pair().part_graph, 0);

    const steamrot::AnalysisTrace expected =
        AnalysisTraceBuilder{pair().id_to_part_graph_id}
            .NodeEval("f0", is_fragment().GetName(), true, 0,
                      "node holds FragmentInstance")
            .Build();

    REQUIRE(result);
    REQUIRE_THAT(result.m_trace, EqualsTrace(expected, kFmt));
  }

  SECTION("returns false for a JointInstance") {
    // pair: j0=id1
    const steamrot::NodeDescriptorResult result =
        is_fragment()(pair().part_graph, 1);

    const steamrot::AnalysisTrace expected =
        AnalysisTraceBuilder{pair().id_to_part_graph_id}
            .NodeEval("j0", is_fragment().GetName(), false, 0,
                      "node holds JointInstance")
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace, EqualsTrace(expected, kFmt));
  }

  SECTION("returns false for a missing part ID") {
    constexpr uint32_t kMissingId{99};
    const steamrot::NodeDescriptorResult result =
        is_fragment()(pair().part_graph, kMissingId);

    // The missing part has no alias in the graph, so the expected trace uses
    // the numeric-ID overloads to match the actual trace formatting.
    AnalysisTraceBuilder builder{pair().id_to_part_graph_id};
    const steamrot::AnalysisTrace expected =
        builder
            .NodeEvalById(kMissingId, is_fragment().GetName(), false, 0,
                          "incorrect key: part_id=99")
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace, EqualsTrace(expected, kFmt));
  }
}

/////////////////////////////////////////////////
/// is_joint
/////////////////////////////////////////////////

TEST_CASE("is_joint", "[unit][logic][descriptors]") {
  const TerminalDescriptorFormatter kFmt{};
  SECTION("returns true for a JointInstance") {
    // pair: j0=id1
    const steamrot::NodeDescriptorResult result =
        is_joint()(pair().part_graph, 1);

    const steamrot::AnalysisTrace expected =
        AnalysisTraceBuilder{pair().id_to_part_graph_id}
            .NodeEval("j0", is_joint().GetName(), true, 0,
                      "node holds JointInstance")
            .Build();

    REQUIRE(result);
    REQUIRE_THAT(result.m_trace, EqualsTrace(expected, kFmt));
  }

  SECTION("returns false for a FragmentInstance") {
    // pair: f0=id0
    const steamrot::NodeDescriptorResult result =
        is_joint()(pair().part_graph, 0);

    const steamrot::AnalysisTrace expected =
        AnalysisTraceBuilder{pair().id_to_part_graph_id}
            .NodeEval("f0", is_joint().GetName(), false, 0,
                      "node holds FragmentInstance")
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace, EqualsTrace(expected, kFmt));
  }

  SECTION("returns false for a missing part ID") {
    constexpr uint32_t kMissingId{99};
    const steamrot::NodeDescriptorResult result =
        is_joint()(pair().part_graph, kMissingId);

    // The missing part has no alias in the graph, so the expected trace uses
    // the numeric-ID overloads to match the actual trace formatting.
    AnalysisTraceBuilder builder{pair().id_to_part_graph_id};
    const steamrot::AnalysisTrace expected =
        builder
            .NodeEvalById(kMissingId, is_joint().GetName(), false, 0,
                          "incorrect key: part_id=99")
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace, EqualsTrace(expected, kFmt));
  }
}

/////////////////////////////////////////////////
/// is_terminal  (connection_count == 1)
/////////////////////////////////////////////////

TEST_CASE("is_terminal tests", "[unit][logic][descriptors]") {
  const TerminalDescriptorFormatter kFmt{};
  SECTION("returns true for a terminal node (connection_count == 1)") {
    // pair: f0=id0, connection_count=1
    const steamrot::NodeDescriptorResult result =
        is_terminal()(pair().part_graph, 0);

    const steamrot::AnalysisTrace expected =
        AnalysisTraceBuilder{pair().id_to_part_graph_id}
            .NodeEval("f0", is_terminal().GetName(), true, 0,
                      "connection_count=1, expected==1")
            .Build();

    REQUIRE(result);
    REQUIRE_THAT(result.m_trace, EqualsTrace(expected, kFmt));
  }

  SECTION("returns false for a serial node (connection_count == 2)") {
    // linear_chain_3: j0=id1, connection_count=2
    const steamrot::NodeDescriptorResult result =
        is_terminal()(linear_chain_3().part_graph, 1);

    const steamrot::AnalysisTrace expected =
        AnalysisTraceBuilder{linear_chain_3().id_to_part_graph_id}
            .NodeEval("j0", is_terminal().GetName(), false, 0,
                      "connection_count=2, expected==1")
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace, EqualsTrace(expected, kFmt));
  }
}

/////////////////////////////////////////////////
/// is_serial  (connection_count == 2)
/////////////////////////////////////////////////

TEST_CASE("is_serial tests", "[unit][logic][descriptors]") {
  const TerminalDescriptorFormatter kFmt{};
  SECTION("returns true for a serial node (connection_count == 2)") {
    // linear_chain_3: j0=id1, connection_count=2
    const steamrot::NodeDescriptorResult result =
        is_serial()(linear_chain_3().part_graph, 1);

    const steamrot::AnalysisTrace expected =
        AnalysisTraceBuilder{linear_chain_3().id_to_part_graph_id}
            .NodeEval("j0", is_serial().GetName(), true, 0,
                      "connection_count=2, expected==2")
            .Build();

    REQUIRE(result);
    REQUIRE_THAT(result.m_trace, EqualsTrace(expected, kFmt));
  }

  SECTION("returns false for a terminal node (connection_count == 1)") {
    // pair: f0=id0, connection_count=1
    const steamrot::NodeDescriptorResult result =
        is_serial()(pair().part_graph, 0);

    const steamrot::AnalysisTrace expected =
        AnalysisTraceBuilder{pair().id_to_part_graph_id}
            .NodeEval("f0", is_serial().GetName(), false, 0,
                      "connection_count=1, expected==2")
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace, EqualsTrace(expected, kFmt));
  }
}

/////////////////////////////////////////////////
/// is_branched  (connection_count >= 3)
/////////////////////////////////////////////////

TEST_CASE("is_branched", "[unit][logic][descriptors]") {
  const TerminalDescriptorFormatter kFmt{};
  // Build an anonymous graph: j0 (ThreeSockets) connected to f0, f1, f2
  // IDs: j0=0, f0=1, f1=2, f2=3
  const PartGraphPackage pkg =
      PartGraphBuilder{}
          .AddJointInstance(JointNames::ThreeSockets, "j0")
          .AddFragmentInstance(FragmentNames::OneSocket, "f0")
          .AddFragmentInstance(FragmentNames::OneSocket, "f1")
          .AddFragmentInstance(FragmentNames::OneSocket, "f2")
          .Connect("f0", 0, "j0", 0)
          .Connect("f1", 0, "j0", 1)
          .Connect("f2", 0, "j0", 2)
          .Build();

  SECTION("returns true for a branching node (connection_count == 3)") {
    // j0=id0, connection_count=3
    const steamrot::NodeDescriptorResult result =
        is_branched()(pkg.part_graph, 0);

    const steamrot::AnalysisTrace expected =
        AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            .NodeEval("j0", is_branched().GetName(), true, 0,
                      "connection_count=3, expected>=3")
            .Build();

    REQUIRE(result);
    REQUIRE_THAT(result.m_trace, EqualsTrace(expected, kFmt));
  }

  SECTION("returns false for a terminal node (connection_count == 1)") {
    // f0=id1, connection_count=1
    const steamrot::NodeDescriptorResult result =
        is_branched()(pkg.part_graph, 1);

    const steamrot::AnalysisTrace expected =
        AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            .NodeEval("f0", is_branched().GetName(), false, 0,
                      "connection_count=1, expected>=3")
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace, EqualsTrace(expected, kFmt));
  }
}
} // namespace steamrot::tests
