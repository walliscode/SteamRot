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
#include "AnalysisTraceBuilder.h"
#include "PartGraphBuilder.h"
#include "KeyValueDescriptorFormatter.h"
#include "TraceEqualsMatcher.h"
#include "descriptors_node_descriptors.h"
#include "part_graph_library.h"
#include <catch2/catch_test_macros.hpp>
#include <unordered_map>

namespace descriptors = steamrot::logic::descriptors;
namespace tests = steamrot::tests;

namespace {
const descriptors::KeyValueDescriptorFormatter kFmt{};
} // namespace

/////////////////////////////////////////////////
/// is_fragment
/////////////////////////////////////////////////

TEST_CASE("is_fragment", "[unit][logic][descriptors]") {

  SECTION("returns true for a FragmentInstance") {
    // pair: f0=id0 (fragment), j0=id1 (joint), both connected
    const descriptors::NodeDescriptorResult result =
        descriptors::is_fragment(tests::pair.part_graph, 0);

    const descriptors::AnalysisTrace expected =
        tests::AnalysisTraceBuilder{tests::pair.id_to_part_graph_id}
            .NodeEval("f0", descriptors::is_fragment.GetName())
            .NodeResult("f0", descriptors::is_fragment.GetName(), true,
                        "node holds FragmentInstance")
            .Build();

    REQUIRE(result);
    REQUIRE_THAT(result.m_trace, tests::EqualsTrace(expected, kFmt));
  }

  SECTION("returns false for a JointInstance") {
    // pair: j0=id1
    const descriptors::NodeDescriptorResult result =
        descriptors::is_fragment(tests::pair.part_graph, 1);

    const descriptors::AnalysisTrace expected =
        tests::AnalysisTraceBuilder{tests::pair.id_to_part_graph_id}
            .NodeEval("j0", descriptors::is_fragment.GetName())
            .NodeResult("j0", descriptors::is_fragment.GetName(), false,
                        "node holds JointInstance")
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace, tests::EqualsTrace(expected, kFmt));
  }

  SECTION("returns false for a missing part ID") {
    constexpr uint32_t kMissingId{99};
    const descriptors::NodeDescriptorResult result =
        descriptors::is_fragment(tests::pair.part_graph, kMissingId);

    // The missing part has no alias in the graph, so the expected trace uses
    // the numeric-ID overloads to match the actual trace formatting.
    tests::AnalysisTraceBuilder builder{tests::pair.id_to_part_graph_id};
    const descriptors::AnalysisTrace expected =
        builder
            .NodeEvalById(kMissingId, descriptors::is_fragment.GetName())
            .NodeResultById(kMissingId, descriptors::is_fragment.GetName(),
                            false, "incorrect key: part_id=99")
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace, tests::EqualsTrace(expected, kFmt));
  }
}

/////////////////////////////////////////////////
/// is_joint
/////////////////////////////////////////////////

TEST_CASE("is_joint", "[unit][logic][descriptors]") {

  SECTION("returns true for a JointInstance") {
    // pair: j0=id1
    const descriptors::NodeDescriptorResult result =
        descriptors::is_joint(tests::pair.part_graph, 1);

    const descriptors::AnalysisTrace expected =
        tests::AnalysisTraceBuilder{tests::pair.id_to_part_graph_id}
            .NodeEval("j0", descriptors::is_joint.GetName())
            .NodeResult("j0", descriptors::is_joint.GetName(), true,
                        "node holds JointInstance")
            .Build();

    REQUIRE(result);
    REQUIRE_THAT(result.m_trace, tests::EqualsTrace(expected, kFmt));
  }

  SECTION("returns false for a FragmentInstance") {
    // pair: f0=id0
    const descriptors::NodeDescriptorResult result =
        descriptors::is_joint(tests::pair.part_graph, 0);

    const descriptors::AnalysisTrace expected =
        tests::AnalysisTraceBuilder{tests::pair.id_to_part_graph_id}
            .NodeEval("f0", descriptors::is_joint.GetName())
            .NodeResult("f0", descriptors::is_joint.GetName(), false,
                        "node holds FragmentInstance")
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace, tests::EqualsTrace(expected, kFmt));
  }

  SECTION("returns false for a missing part ID") {
    constexpr uint32_t kMissingId{99};
    const descriptors::NodeDescriptorResult result =
        descriptors::is_joint(tests::pair.part_graph, kMissingId);

    // The missing part has no alias in the graph, so the expected trace uses
    // the numeric-ID overloads to match the actual trace formatting.
    tests::AnalysisTraceBuilder builder{tests::pair.id_to_part_graph_id};
    const descriptors::AnalysisTrace expected =
        builder
            .NodeEvalById(kMissingId, descriptors::is_joint.GetName())
            .NodeResultById(kMissingId, descriptors::is_joint.GetName(), false,
                            "incorrect key: part_id=99")
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace, tests::EqualsTrace(expected, kFmt));
  }
}

/////////////////////////////////////////////////
/// is_terminal  (connection_count == 1)
/////////////////////////////////////////////////

TEST_CASE("is_terminal", "[unit][logic][descriptors]") {

  SECTION("returns true for a terminal node (connection_count == 1)") {
    // pair: f0=id0, connection_count=1
    const descriptors::NodeDescriptorResult result =
        descriptors::is_terminal(tests::pair.part_graph, 0);

    const descriptors::AnalysisTrace expected =
        tests::AnalysisTraceBuilder{tests::pair.id_to_part_graph_id}
            .NodeEval("f0", descriptors::is_terminal.GetName())
            .NodeResult("f0", descriptors::is_terminal.GetName(), true,
                        "connection_count=1, expected==1")
            .Build();

    REQUIRE(result);
    REQUIRE_THAT(result.m_trace, tests::EqualsTrace(expected, kFmt));
  }

  SECTION("returns false for a serial node (connection_count == 2)") {
    // linear_chain_3: j0=id1, connection_count=2
    const descriptors::NodeDescriptorResult result =
        descriptors::is_terminal(tests::linear_chain_3.part_graph, 1);

    const descriptors::AnalysisTrace expected =
        tests::AnalysisTraceBuilder{tests::linear_chain_3.id_to_part_graph_id}
            .NodeEval("j0", descriptors::is_terminal.GetName())
            .NodeResult("j0", descriptors::is_terminal.GetName(), false,
                        "connection_count=2, expected==1")
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace, tests::EqualsTrace(expected, kFmt));
  }
}

/////////////////////////////////////////////////
/// is_serial  (connection_count == 2)
/////////////////////////////////////////////////

TEST_CASE("is_serial", "[unit][logic][descriptors]") {

  SECTION("returns true for a serial node (connection_count == 2)") {
    // linear_chain_3: j0=id1, connection_count=2
    const descriptors::NodeDescriptorResult result =
        descriptors::is_serial(tests::linear_chain_3.part_graph, 1);

    const descriptors::AnalysisTrace expected =
        tests::AnalysisTraceBuilder{tests::linear_chain_3.id_to_part_graph_id}
            .NodeEval("j0", descriptors::is_serial.GetName())
            .NodeResult("j0", descriptors::is_serial.GetName(), true,
                        "connection_count=2, expected==2")
            .Build();

    REQUIRE(result);
    REQUIRE_THAT(result.m_trace, tests::EqualsTrace(expected, kFmt));
  }

  SECTION("returns false for a terminal node (connection_count == 1)") {
    // pair: f0=id0, connection_count=1
    const descriptors::NodeDescriptorResult result =
        descriptors::is_serial(tests::pair.part_graph, 0);

    const descriptors::AnalysisTrace expected =
        tests::AnalysisTraceBuilder{tests::pair.id_to_part_graph_id}
            .NodeEval("f0", descriptors::is_serial.GetName())
            .NodeResult("f0", descriptors::is_serial.GetName(), false,
                        "connection_count=1, expected==2")
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace, tests::EqualsTrace(expected, kFmt));
  }
}

/////////////////////////////////////////////////
/// is_branched  (connection_count >= 3)
/////////////////////////////////////////////////

TEST_CASE("is_branched", "[unit][logic][descriptors]") {
  // Build an anonymous graph: j0 (ThreeSockets) connected to f0, f1, f2
  // IDs: j0=0, f0=1, f1=2, f2=3
  const tests::PartGraphPackage pkg =
      tests::PartGraphBuilder{}
          .AddJoint(tests::JointNames::ThreeSockets, "j0")
          .AddFragment(tests::FragmentNames::OneSocket, "f0")
          .AddFragment(tests::FragmentNames::OneSocket, "f1")
          .AddFragment(tests::FragmentNames::OneSocket, "f2")
          .Connect("f0", 0, "j0", 0)
          .Connect("f1", 0, "j0", 1)
          .Connect("f2", 0, "j0", 2)
          .Build();

  SECTION("returns true for a branching node (connection_count == 3)") {
    // j0=id0, connection_count=3
    const descriptors::NodeDescriptorResult result =
        descriptors::is_branched(pkg.part_graph, 0);

    const descriptors::AnalysisTrace expected =
        tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            .NodeEval("j0", descriptors::is_branched.GetName())
            .NodeResult("j0", descriptors::is_branched.GetName(), true,
                        "connection_count=3, expected>=3")
            .Build();

    REQUIRE(result);
    REQUIRE_THAT(result.m_trace, tests::EqualsTrace(expected, kFmt));
  }

  SECTION("returns false for a terminal node (connection_count == 1)") {
    // f0=id1, connection_count=1
    const descriptors::NodeDescriptorResult result =
        descriptors::is_branched(pkg.part_graph, 1);

    const descriptors::AnalysisTrace expected =
        tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
            .NodeEval("f0", descriptors::is_branched.GetName())
            .NodeResult("f0", descriptors::is_branched.GetName(), false,
                        "connection_count=1, expected>=3")
            .Build();

    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace, tests::EqualsTrace(expected, kFmt));
  }
}
