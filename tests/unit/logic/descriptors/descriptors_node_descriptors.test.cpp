/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for analysis_grimoire_machina free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "descriptors_node_descriptors.h"
#include "AnalysisTraceBuilder.h"
#include "MachinaFormScaffold.h"
#include "TerminalDescriptorFormatter.h"
#include "TraceEqualsMatcher.h"
#include "descriptors_general.h"
#include "part_library.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <vector>

namespace descriptors = steamrot::logic::descriptors;
namespace {
constexpr uint32_t kMissingPartId{9999};
}

TEST_CASE("empty scaffold has no parts", "[unit][analysis][grimoire_machina]") {
  steamrot::MachinaFormScaffold scaffold;
  REQUIRE(scaffold.parts.empty());
}

TEST_CASE("scaffold with unconnected parts has zero connection_count",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::MachinaFormScaffold scaffold = builder.MakeScaffoldWithParts(
      {"fragment_one_socket"}, {"joint_one_socket"});

  REQUIRE(scaffold.parts.size() == 2);
  for (const auto &[id, variant] : scaffold.parts) {
    const size_t count = std::visit(
        [](const auto &inst) -> size_t { return inst.connection_count; },
        variant);
    REQUIRE(count == 0);
  }
}

TEST_CASE("connection_count is maintained after connecting parts",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  // fragment[0].socket[0] -> joint[0].socket[0]
  steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
      {{"frag0", "fragment_two_sockets"}}, {{"joint0", "joint_two_sockets"}},
      {{"frag0", 0, "joint0", 0}});

  REQUIRE(result.scaffold.parts.size() == 2);
  // Both endpoints gain connection_count == 1 from the single connection.
  for (const auto &id : result.part_ids) {
    const size_t count = std::visit(
        [](const auto &inst) -> size_t { return inst.connection_count; },
        result.scaffold.parts.at(id));
    REQUIRE(count == 1);
  }
}

TEST_CASE("is_fragment and is_joint correctly identify node types",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  SECTION("Scaffold with one fragment and one joint") {
    steamrot::MachinaFormScaffold scaffold = builder.MakeScaffoldWithParts(
        {"fragment_one_socket"}, {"joint_one_socket"});

    REQUIRE(scaffold.parts.size() == 2);

    int fragment_count = 0;
    int joint_count = 0;
    for (const auto &[id, variant] : scaffold.parts) {
      if (descriptors::is_fragment(scaffold.parts, id))
        ++fragment_count;
      if (descriptors::is_joint(scaffold.parts, id))
        ++joint_count;
    }
    REQUIRE(fragment_count == 1);
    REQUIRE(joint_count == 1);
  }

  SECTION("Analyses all ScaffoldScenarios correctly for is_fragment") {
    steamrot::tests::CheckNodeDescriptorForAllScenarios(
        descriptors::is_fragment,
        {.linear_chain = {true, true, false},
         .ring = {false, false, false},
         .isolated_pair = {true, true},
         .simple_branch = {true, true, true, false}},
        lib);
  }

  SECTION("Analyses all ScaffoldScenarios correctly for is_joint") {
    steamrot::tests::CheckNodeDescriptorForAllScenarios(
        descriptors::is_joint,
        {.linear_chain = {false, false, true},
         .ring = {true, true, true},
         .isolated_pair = {false, false},
         .simple_branch = {false, false, false, true}},
        lib);
  }
}

TEST_CASE("NodeDescriptor fails with reason for incorrect key",
          "[unit][analysis][grimoire_machina]") {
  const steamrot::PartGraph empty_parts{};

  const descriptors::NodeDescriptorResult result =
      descriptors::is_fragment(empty_parts, kMissingPartId);

  REQUIRE_FALSE(result);
  REQUIRE(result.m_reason == "incorrect key: part_id=9999");

  steamrot::logic::descriptors::AnalysisTrace expected_trace =
      steamrot::tests::AnalysisTraceBuilder{}
          .NodeEval(kMissingPartId, "is_fragment", 0)
          .NodeResult(kMissingPartId, "is_fragment", false,
                      "incorrect key: part_id=9999")
          .Build();

  REQUIRE_THAT(result.m_trace,
               steamrot::tests::EqualsTrace(
                   expected_trace, descriptors::TerminalDescriptorFormatter{}));
}

TEST_CASE("NodeDescriptor emits focused trace details for is_serial",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
      {{"frag0", "fragment_two_sockets"}, {"frag1", "fragment_two_sockets"}},
      {{"joint0", "joint_two_sockets"}},
      {{"frag0", 0, "joint0", 0}, {"frag1", 0, "joint0", 1}});
  REQUIRE(result.scaffold.parts.size() == 3);

  SECTION("is_serial trace includes predicate reason for a matching node") {
    const uint32_t matching_id = result.alias_to_id.at("joint0");
    const descriptors::NodeDescriptorResult descriptor_result =
        descriptors::is_serial(result.scaffold.parts, matching_id);

    REQUIRE(descriptor_result);
    REQUIRE(descriptor_result.m_reason == "connection_count=2, expected==2");

    steamrot::logic::descriptors::AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{}
            .BindAliases(result)
            .NodeEvalNamed("joint0", "is_serial", 0)
            .NodeResultNamed("joint0", "is_serial", true,
                             "connection_count=2, expected==2", 0)
            .Build();

    REQUIRE_THAT(
        descriptor_result.m_trace,
        steamrot::tests::EqualsTrace(
            expected_trace, descriptors::TerminalDescriptorFormatter{}));
  }

  SECTION("NodeDescriptor trace preserves explicit depth for a non-match") {
    const uint32_t non_matching_id = result.alias_to_id.at("frag0");
    const uint32_t depth = 3;
    const descriptors::NodeDescriptorResult descriptor_result =
        descriptors::is_serial(result.scaffold.parts, non_matching_id, depth);

    REQUIRE_FALSE(descriptor_result);
    REQUIRE(descriptor_result.m_reason == "connection_count=1, expected==2");

    steamrot::logic::descriptors::AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{}
            .BindAliases(result)
            .NodeEvalNamed("frag0", "is_serial", depth)
            .NodeResultNamed("frag0", "is_serial", false,
                             "connection_count=1, expected==2", depth)
            .Build();

    REQUIRE_THAT(
        descriptor_result.m_trace,
        steamrot::tests::EqualsTrace(
            expected_trace, descriptors::TerminalDescriptorFormatter{}));
  }
}

TEST_CASE("predicate combinators compose correctly",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::MachinaFormScaffold scaffold = builder.MakeScaffoldWithParts(
      {"fragment_two_sockets"}, {"joint_two_sockets"});
  REQUIRE(scaffold.parts.size() == 2);

  descriptors::NodeDescriptor is_both =
      descriptors::and_(descriptors::is_fragment, descriptors::is_joint);
  descriptors::NodeDescriptor is_either =
      descriptors::or_(descriptors::is_fragment, descriptors::is_joint);
  descriptors::NodeDescriptor not_fragment =
      descriptors::not_(descriptors::is_fragment);

  for (const auto &[id, variant] : scaffold.parts) {
    REQUIRE_FALSE(is_both(scaffold.parts, id));
    REQUIRE(is_either(scaffold.parts, id));
    REQUIRE(not_fragment(scaffold.parts, id) ==
            descriptors::is_joint(scaffold.parts, id));
  }

  SECTION("Analyses all ScaffoldScenarios correctly for and_(is_fragment, "
          "is_joint)") {
    steamrot::tests::CheckNodeDescriptorForAllScenarios(
        descriptors::and_(descriptors::is_fragment, descriptors::is_joint),
        {.linear_chain = {false, false, false},
         .ring = {false, false, false},
         .isolated_pair = {false, false},
         .simple_branch = {false, false, false, false}},
        lib);
  }

  SECTION("Analyses all ScaffoldScenarios correctly for or_(is_fragment, "
          "is_joint)") {
    steamrot::tests::CheckNodeDescriptorForAllScenarios(
        descriptors::or_(descriptors::is_fragment, descriptors::is_joint),
        {.linear_chain = {true, true, true},
         .ring = {true, true, true},
         .isolated_pair = {true, true},
         .simple_branch = {true, true, true, true}},
        lib);
  }

  SECTION("Analyses all ScaffoldScenarios correctly for not_(is_fragment)") {
    steamrot::tests::CheckNodeDescriptorForAllScenarios(
        descriptors::not_(descriptors::is_fragment),
        {.linear_chain = {false, false, true},
         .ring = {true, true, true},
         .isolated_pair = {false, false},
         .simple_branch = {false, false, false, true}},
        lib);
  }
}

TEST_CASE("has_exactly_n_edges tests", "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::logic::descriptors::NodeDescriptor has_0 =
      descriptors::has_exactly_n_edges(0, "has_exactly_0_edges");
  steamrot::logic::descriptors::NodeDescriptor has_1 =
      descriptors::has_exactly_n_edges(1, "has_exactly_1_edge");
  steamrot::logic::descriptors::NodeDescriptor has_2 =
      descriptors::has_exactly_n_edges(2, "has_exactly_2_edges");

  SECTION("Nodes with 0 edges") {
    steamrot::MachinaFormScaffold scaffold = builder.MakeScaffoldWithParts(
        {"fragment_two_sockets"}, {"joint_two_sockets"});
    REQUIRE(scaffold.parts.size() == 2);
    // No connections, so both parts have 0 edges.
    for (const auto &[id, variant] : scaffold.parts) {
      REQUIRE(has_0(scaffold.parts, id));
      REQUIRE_FALSE(has_1(scaffold.parts, id));
      REQUIRE_FALSE(has_2(scaffold.parts, id));
    }
  }

  SECTION("Nodes with 1 edge") {
    steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
        {{"frag0", "fragment_two_sockets"}}, {{"joint0", "joint_two_sockets"}},
        {{"frag0", 0, "joint0", 0}});
    REQUIRE(result.scaffold.parts.size() == 2);
    // The fragment and the joint each have 1 edge.
    for (const auto &id : result.part_ids) {
      REQUIRE_FALSE(has_0(result.scaffold.parts, id));
      REQUIRE(has_1(result.scaffold.parts, id));
      REQUIRE_FALSE(has_2(result.scaffold.parts, id));
    }
  }

  SECTION("Nodes with 2 edges") {
    steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
        {{"frag0", "fragment_two_sockets"}, {"frag1", "fragment_two_sockets"}},
        {{"joint0", "joint_two_sockets"}},
        {{"frag0", 0, "joint0", 0}, // fragment[0].socket[0] -> joint[0].socket[0]
         {"frag1", 0, "joint0", 1}}); // fragment[1].socket[0] -> joint[0].socket[1]
    REQUIRE(result.scaffold.parts.size() == 3);
    // fragment[0] and fragment[1] have 1 edge each; joint[0] has 2 edges.
    REQUIRE_FALSE(has_0(result.scaffold.parts, result.part_ids[0]));
    REQUIRE_FALSE(has_0(result.scaffold.parts, result.part_ids[1]));
    REQUIRE_FALSE(has_0(result.scaffold.parts, result.part_ids[2]));
    REQUIRE(has_1(result.scaffold.parts, result.part_ids[0]));
    REQUIRE(has_1(result.scaffold.parts, result.part_ids[1]));
    REQUIRE_FALSE(has_1(result.scaffold.parts, result.part_ids[2]));
    REQUIRE_FALSE(has_2(result.scaffold.parts, result.part_ids[0]));
    REQUIRE_FALSE(has_2(result.scaffold.parts, result.part_ids[1]));
    REQUIRE(has_2(result.scaffold.parts, result.part_ids[2]));
  }

  SECTION("Analyses all ScaffoldScenarios correctly for has_exactly_0_edges") {
    steamrot::tests::CheckNodeDescriptorForAllScenarios(
        descriptors::has_exactly_n_edges(0, "has_exactly_0_edges"),
        {.linear_chain = {false, false, false},
         .ring = {false, false, false},
         .isolated_pair = {false, false},
         .simple_branch = {false, false, false, false}},
        lib);
  }

  SECTION("Analyses all ScaffoldScenarios correctly for has_exactly_1_edge") {
    steamrot::tests::CheckNodeDescriptorForAllScenarios(
        descriptors::has_exactly_n_edges(1, "has_exactly_1_edge"),
        {.linear_chain = {true, true, false},
         .ring = {false, false, false},
         .isolated_pair = {true, true},
         .simple_branch = {true, true, true, false}},
        lib);
  }

  SECTION("Analyses all ScaffoldScenarios correctly for has_exactly_2_edges") {
    steamrot::tests::CheckNodeDescriptorForAllScenarios(
        descriptors::has_exactly_n_edges(2, "has_exactly_2_edges"),
        {.linear_chain = {false, false, true},
         .ring = {true, true, true},
         .isolated_pair = {false, false},
         .simple_branch = {false, false, false, false}},
        lib);
  }
}

TEST_CASE("is_serial tests", "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  // just test the pre-built scenarios for named predicates
  SECTION("Analyses all ScaffoldScenarios correctly for is_serial") {
    steamrot::tests::CheckNodeDescriptorForAllScenarios(
        descriptors::is_serial,
        {.linear_chain = {false, false, true},
         .ring = {true, true, true},
         .isolated_pair = {false, false},
         .simple_branch = {false, false, false, false}},
        lib);
  }
}

TEST_CASE("has_minimum_n_connected_sockets tests",
          "[unit][analysis][grimoire_machina]") {
  // arrange
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  // set up some NodeDescriptors for testing
  descriptors::NodeDescriptor min_0 = descriptors::has_minimum_n_edges(0);
  descriptors::NodeDescriptor min_1 = descriptors::has_minimum_n_edges(1);
  descriptors::NodeDescriptor min_2 = descriptors::has_minimum_n_edges(2);

  SECTION("Nodes with 0 edges") {
    steamrot::MachinaFormScaffold scaffold = builder.MakeScaffoldWithParts(
        {"fragment_two_sockets"}, {"joint_two_sockets"});
    REQUIRE(scaffold.parts.size() == 2);
    // No connections: both parts satisfy min_0, but not min_1 or min_2.
    for (const auto &[id, variant] : scaffold.parts) {
      REQUIRE(min_0(scaffold.parts, id));
      REQUIRE_FALSE(min_1(scaffold.parts, id));
      REQUIRE_FALSE(min_2(scaffold.parts, id));
    }
  }

  SECTION("Nodes with 1 edge") {
    steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
        {{"frag0", "fragment_two_sockets"}}, {{"joint0", "joint_two_sockets"}},
        {{"frag0", 0, "joint0", 0}});
    REQUIRE(result.scaffold.parts.size() == 2);
    // Both parts have 1 edge: satisfy min_0 and min_1, but not min_2.
    for (const auto &id : result.part_ids) {
      REQUIRE(min_0(result.scaffold.parts, id));
      REQUIRE(min_1(result.scaffold.parts, id));
      REQUIRE_FALSE(min_2(result.scaffold.parts, id));
    }
  }

  SECTION("Nodes with 2 edges") {
    steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
        {{"frag0", "fragment_two_sockets"}, {"frag1", "fragment_two_sockets"}},
        {{"joint0", "joint_two_sockets"}},
        {{"frag0", 0, "joint0", 0}, // fragment[0].socket[0] -> joint[0].socket[0]
         {"frag1", 0, "joint0", 1}}); // fragment[1].socket[0] -> joint[0].socket[1]
    REQUIRE(result.scaffold.parts.size() == 3);
    // fragment[0] and fragment[1] each have 1 edge; joint[0] has 2 edges.
    REQUIRE(min_0(result.scaffold.parts, result.part_ids[0]));
    REQUIRE(min_0(result.scaffold.parts, result.part_ids[1]));
    REQUIRE(min_0(result.scaffold.parts, result.part_ids[2]));
    REQUIRE(min_1(result.scaffold.parts, result.part_ids[0]));
    REQUIRE(min_1(result.scaffold.parts, result.part_ids[1]));
    REQUIRE(min_1(result.scaffold.parts, result.part_ids[2]));
    REQUIRE_FALSE(min_2(result.scaffold.parts, result.part_ids[0]));
    REQUIRE_FALSE(min_2(result.scaffold.parts, result.part_ids[1]));
    REQUIRE(min_2(result.scaffold.parts, result.part_ids[2]));
  }
}

TEST_CASE("is_branched tests", "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};
  // just test the pre-built scenarios for named predicates
  SECTION("Analyses all ScaffoldScenarios correctly for is_branched") {
    steamrot::tests::CheckNodeDescriptorForAllScenarios(
        descriptors::is_branched,
        {.linear_chain = {false, false, false},
         .ring = {false, false, false},
         .isolated_pair = {false, false},
         .simple_branch = {false, false, false, true}},
        lib);
  }
}

TEST_CASE("has_maximum_n_connected_sockets tests",
          "[unit][analysis][grimoire_machina]") {

  // arrange
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  // set up some NodeDescriptors for testing
  descriptors::NodeDescriptor max_0 = descriptors::has_maximum_n_edges(0);
  descriptors::NodeDescriptor max_1 = descriptors::has_maximum_n_edges(1);
  descriptors::NodeDescriptor max_2 = descriptors::has_maximum_n_edges(2);

  // create a scaffold with two fragments and two joints, creating various
  // combos of connections
  steamrot::tests::ScaffoldResult scaffold_result =
      builder.MakeConnectedScaffold(
          {{"frag0", "fragment_two_sockets"}, {"frag1", "fragment_two_sockets"}},
          {{"joint0", "joint_two_sockets"},
           {"joint1", "joint_two_sockets"},
           {"joint2", "joint_one_socket"}},
          {{"frag0", 0, "joint0", 0}, // fragment[0].socket[0] -> joint[2].socket[0]
           {"frag0", 1, "joint1", 0}, // fragment[0].socket[1] -> joint[3].socket[0]
           {"frag1", 0, "joint0", 1}}); // fragment[1].socket[0] -> joint[2].socket[1]

  // act & assert
  const std::vector<uint32_t> &part_ids = scaffold_result.part_ids;
  const steamrot::PartGraph &parts = scaffold_result.scaffold.parts;
  REQUIRE(parts.size() == 5);
  // fragment[0] has 2 edges, fragment[1] has 1 edge, joint[2] has 2 edges,
  // joint[3] has 1 edge, and joint[4] has 0 edges
  REQUIRE_FALSE(max_0(parts, part_ids[0]));
  REQUIRE_FALSE(max_0(parts, part_ids[1]));
  REQUIRE_FALSE(max_0(parts, part_ids[2]));
  REQUIRE_FALSE(max_0(parts, part_ids[3]));
  REQUIRE(max_0(parts, part_ids[4]));

  REQUIRE_FALSE(max_1(parts, part_ids[0]));
  REQUIRE(max_1(parts, part_ids[1]));
  REQUIRE_FALSE(max_1(parts, part_ids[2]));
  REQUIRE(max_1(parts, part_ids[3]));
  REQUIRE(max_1(parts, part_ids[4]));

  REQUIRE(max_2(parts, part_ids[0]));
  REQUIRE(max_2(parts, part_ids[1]));
  REQUIRE(max_2(parts, part_ids[2]));
  REQUIRE(max_2(parts, part_ids[3]));
  REQUIRE(max_2(parts, part_ids[4]));

  SECTION("Analyses all ScaffoldScenarios correctly for has_maximum_0_edges") {
    steamrot::tests::CheckNodeDescriptorForAllScenarios(
        descriptors::has_maximum_n_edges(0),
        {.linear_chain = {false, false, false},
         .ring = {false, false, false},
         .isolated_pair = {false, false},
         .simple_branch = {false, false, false, false}},
        lib);
  }

  SECTION("Analyses all ScaffoldScenarios correctly for has_maximum_1_edge") {
    steamrot::tests::CheckNodeDescriptorForAllScenarios(
        descriptors::has_maximum_n_edges(1),
        {.linear_chain = {true, true, false},
         .ring = {false, false, false},
         .isolated_pair = {true, true},
         .simple_branch = {true, true, true, false}},
        lib);
  }

  SECTION("Analyses all ScaffoldScenarios correctly for has_maximum_2_edges") {
    steamrot::tests::CheckNodeDescriptorForAllScenarios(
        descriptors::has_maximum_n_edges(2),
        {.linear_chain = {true, true, true},
         .ring = {true, true, true},
         .isolated_pair = {true, true},
         .simple_branch = {true, true, true, false}},
        lib);
  }
}

TEST_CASE("is_terminal tests", "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  descriptors::NodeDescriptor not_terminal =
      descriptors::not_(descriptors::is_terminal);

  SECTION("Returns false for nodes with 2 or more edges") {
    steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
        {{"frag0", "fragment_two_sockets"}, {"frag1", "fragment_two_sockets"}},
        {{"joint0", "joint_two_sockets"}},
        {{"frag0", 0, "joint0", 0}, // fragment[0].socket[0] -> joint[0].socket[0]
         {"frag1", 0, "joint0", 1}}); // fragment[1].socket[0] -> joint[0].socket[1]
    REQUIRE(result.scaffold.parts.size() == 3);
    // joint[0] has 2 edges, so it should not be terminal
    REQUIRE(not_terminal(result.scaffold.parts, result.part_ids[2]));
  }
  SECTION("Returns true for nodes with 0 or 1 edge") {
    steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
        {{"frag0", "fragment_two_sockets"}}, {{"joint0", "joint_two_sockets"}},
        {{"frag0", 0, "joint0", 0}}); // fragment[0].socket[0] -> joint[0].socket[0]
    REQUIRE(result.scaffold.parts.size() == 2);

    for (const auto &id : result.part_ids) {
      REQUIRE(descriptors::is_terminal(result.scaffold.parts, id));
    }
  }

  SECTION("Analyses all ScaffoldScenarios correctly") {
    steamrot::tests::CheckNodeDescriptorForAllScenarios(
        descriptors::is_terminal,
        {.linear_chain = {true, true, false},
         .ring = {false, false, false},
         .isolated_pair = {true, true},
         .simple_branch = {true, true, true, false}},
        lib);
  }
}

TEST_CASE(
    "TerminalDescriptorFormatter sense check — is_fragment on a fragment node",
    "[unit][analysis][grimoire_machina][formatter]") {
  // Arrange: single fragment with a predictable part ID.
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};
  steamrot::tests::ScaffoldResult result =
      builder.MakeConnectedScaffold({{"frag0", "fragment_one_socket"}}, {}, {});
  const uint32_t frag_id = result.alias_to_id.at("frag0");

  // Act: run the descriptor and capture the trace.
  const descriptors::NodeDescriptorResult nd_result =
      descriptors::is_fragment(result.scaffold.parts, frag_id);

  // Assert: the boolean result is correct.
  REQUIRE(nd_result.m_result == true);

  // Assert: the formatted trace matches the expected output produced by
  // AnalysisTraceBuilder, ensuring the formatter round-trips correctly.
  descriptors::TerminalDescriptorFormatter fmt;
  steamrot::tests::AnalysisTraceBuilder trace_builder;
  trace_builder.BindAliases(result)
      .NodeEvalNamed("frag0", "is_fragment", 0)
      .NodeResultNamed("frag0", "is_fragment", true,
                       "node holds FragmentInstance");

  REQUIRE_THAT(nd_result.m_trace,
               steamrot::tests::EqualsTrace(trace_builder.Build(), fmt));
}

TEST_CASE(
    "TerminalDescriptorFormatter sense check — is_fragment on a joint node",
    "[unit][analysis][grimoire_machina][formatter]") {
  // Arrange: single joint.
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};
  steamrot::tests::ScaffoldResult result =
      builder.MakeConnectedScaffold({}, {{"joint0", "joint_one_socket"}}, {});
  const uint32_t joint_id = result.alias_to_id.at("joint0");

  // Act: run the descriptor on a joint node — expected to return false.
  const descriptors::NodeDescriptorResult nd_result =
      descriptors::is_fragment(result.scaffold.parts, joint_id);

  // Assert: the boolean result is false for a joint.
  REQUIRE(nd_result.m_result == false);

  // Assert: trace matches expected.
  descriptors::TerminalDescriptorFormatter fmt;
  steamrot::tests::AnalysisTraceBuilder trace_builder;
  trace_builder.BindAliases(result)
      .NodeEvalNamed("joint0", "is_fragment", 0)
      .NodeResultNamed("joint0", "is_fragment", false, "node holds JointInstance");

  REQUIRE_THAT(nd_result.m_trace,
               steamrot::tests::EqualsTrace(trace_builder.Build(), fmt));
}
