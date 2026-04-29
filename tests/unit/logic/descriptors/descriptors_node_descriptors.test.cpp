/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for analysis_grimoire_machina free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "descriptors_node_descriptors.h"
#include "MachinaFormScaffold.h"
#include "PartGraph.h"
#include "descriptors_general.h"
#include "part_library.h"
#include <catch2/catch_test_macros.hpp>
#include <vector>

namespace descriptors = steamrot::logic::descriptors;

TEST_CASE("build_part_graph from empty scaffold yields empty graph",
          "[unit][analysis][grimoire_machina]") {
  steamrot::MachinaFormScaffold scaffold;
  steamrot::PartGraph graph = descriptors::build_part_graph(scaffold);
  REQUIRE(graph.nodes.empty());
  REQUIRE(graph.edges.empty());
}

TEST_CASE("build_part_graph from scaffold with parts but no connections",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::MachinaFormScaffold scaffold = builder.MakeScaffoldWithParts(
      {"fragment_one_socket"}, {"joint_one_socket"});

  steamrot::PartGraph graph = descriptors::build_part_graph(scaffold);
  REQUIRE(graph.nodes.size() == 2);
  REQUIRE(graph.edges.empty());
}

TEST_CASE("build_part_graph edges match scaffold connections count",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  // fragment[0].socket[0] -> joint[0].socket[0]
  steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
      {"fragment_two_sockets"}, {"joint_two_sockets"}, {{0, 0, 1, 0}});

  steamrot::PartGraph graph = descriptors::build_part_graph(result.scaffold);
  REQUIRE(graph.nodes.size() == 2);
  REQUIRE(graph.edges.size() == 1);
}

TEST_CASE("is_fragment and is_joint correctly identify node types",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  SECTION("Scaffold with one fragment and one joint") {
    steamrot::MachinaFormScaffold scaffold = builder.MakeScaffoldWithParts(
        {"fragment_one_socket"}, {"joint_one_socket"});

    steamrot::PartGraph graph = descriptors::build_part_graph(scaffold);
    REQUIRE(graph.nodes.size() == 2);

    int fragment_count = 0;
    int joint_count = 0;
    for (const auto &node : graph.nodes) {
      if (descriptors::is_fragment(node))
        ++fragment_count;
      if (descriptors::is_joint(node))
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

TEST_CASE("predicate combinators compose correctly",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::MachinaFormScaffold scaffold = builder.MakeScaffoldWithParts(
      {"fragment_two_sockets"}, {"joint_two_sockets"});
  steamrot::PartGraph graph = descriptors::build_part_graph(scaffold);
  REQUIRE(graph.nodes.size() == 2);

  descriptors::NodeDescriptor is_both =
      descriptors::and_(descriptors::is_fragment, descriptors::is_joint);
  descriptors::NodeDescriptor is_either =
      descriptors::or_(descriptors::is_fragment, descriptors::is_joint);
  descriptors::NodeDescriptor not_fragment =
      descriptors::not_(descriptors::is_fragment);

  for (const auto &node : graph.nodes) {
    REQUIRE_FALSE(is_both(node));
    REQUIRE(is_either(node));
    REQUIRE(not_fragment(node) == descriptors::is_joint(node));
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
      descriptors::has_exactly_n_edges(0);
  steamrot::logic::descriptors::NodeDescriptor has_1 =
      descriptors::has_exactly_n_edges(1);
  steamrot::logic::descriptors::NodeDescriptor has_2 =
      descriptors::has_exactly_n_edges(2);

  SECTION("Nodes with 0 edges") {
    steamrot::MachinaFormScaffold scaffold = builder.MakeScaffoldWithParts(
        {"fragment_two_sockets"}, {"joint_two_sockets"});
    steamrot::PartGraph graph = descriptors::build_part_graph(scaffold);
    REQUIRE(graph.nodes.size() == 2);
    // No edges, so the fragment should have 0 edges and the joint should have 0
    // edges.
    REQUIRE(has_0(graph.nodes[0]));
    REQUIRE(has_0(graph.nodes[1]));
    REQUIRE_FALSE(has_1(graph.nodes[0]));
    REQUIRE_FALSE(has_1(graph.nodes[1]));
    REQUIRE_FALSE(has_2(graph.nodes[0]));
    REQUIRE_FALSE(has_2(graph.nodes[1]));
  }

  SECTION("Nodes with 1 edge") {
    steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
        {"fragment_two_sockets"}, {"joint_two_sockets"}, {{0, 0, 1, 0}});
    steamrot::PartGraph graph = descriptors::build_part_graph(result.scaffold);
    REQUIRE(graph.nodes.size() == 2);
    // The fragment should have 1 edge and the joint should have 1 edge.
    REQUIRE_FALSE(has_0(graph.nodes[0]));
    REQUIRE_FALSE(has_0(graph.nodes[1]));
    REQUIRE(has_1(graph.nodes[0]));
    REQUIRE(has_1(graph.nodes[1]));
    REQUIRE_FALSE(has_2(graph.nodes[0]));
    REQUIRE_FALSE(has_2(graph.nodes[1]));
  }

  SECTION("Nodes with 2 edges") {
    steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
        {"fragment_two_sockets", "fragment_two_sockets"}, {"joint_two_sockets"},
        {{0, 0, 2, 0},   // fragment[0].socket[0] -> joint[0].socket[0]
         {1, 0, 2, 1}}); // fragment[1].socket[0] -> joint[0].socket[1]
    steamrot::PartGraph graph = descriptors::build_part_graph(result.scaffold);
    REQUIRE(graph.nodes.size() == 3);
    // The joint should have 2 edges, and both fragments should have 1 edge.
    REQUIRE_FALSE(has_0(graph.nodes[0]));
    REQUIRE_FALSE(has_0(graph.nodes[1]));
    REQUIRE_FALSE(has_0(graph.nodes[2]));
    REQUIRE(has_1(graph.nodes[0]));
    REQUIRE(has_1(graph.nodes[1]));
    REQUIRE_FALSE(has_1(graph.nodes[2]));
    REQUIRE_FALSE(has_2(graph.nodes[0]));
    REQUIRE_FALSE(has_2(graph.nodes[1]));
    REQUIRE(has_2(graph.nodes[2]));
  }

  SECTION("Analyses all ScaffoldScenarios correctly for has_exactly_0_edges") {
    steamrot::tests::CheckNodeDescriptorForAllScenarios(
        descriptors::has_exactly_n_edges(0),
        {.linear_chain = {false, false, false},
         .ring = {false, false, false},
         .isolated_pair = {false, false},
         .simple_branch = {false, false, false, false}},
        lib);
  }

  SECTION("Analyses all ScaffoldScenarios correctly for has_exactly_1_edge") {
    steamrot::tests::CheckNodeDescriptorForAllScenarios(
        descriptors::has_exactly_n_edges(1),
        {.linear_chain = {true, true, false},
         .ring = {false, false, false},
         .isolated_pair = {true, true},
         .simple_branch = {true, true, true, false}},
        lib);
  }

  SECTION("Analyses all ScaffoldScenarios correctly for has_exactly_2_edges") {
    steamrot::tests::CheckNodeDescriptorForAllScenarios(
        descriptors::has_exactly_n_edges(2),
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
    steamrot::PartGraph graph = descriptors::build_part_graph(scaffold);
    REQUIRE(graph.nodes.size() == 2);
    // No edges, so both nodes should have at least 0 edges, but not at least 1
    // or 2 edges.
    REQUIRE(min_0(graph.nodes[0]));
    REQUIRE(min_0(graph.nodes[1]));
    REQUIRE_FALSE(min_1(graph.nodes[0]));
    REQUIRE_FALSE(min_1(graph.nodes[1]));
    REQUIRE_FALSE(min_2(graph.nodes[0]));
    REQUIRE_FALSE(min_2(graph.nodes[1]));
  }

  SECTION("Nodes with 1 edge") {
    steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
        {"fragment_two_sockets"}, {"joint_two_sockets"}, {{0, 0, 1, 0}});
    steamrot::PartGraph graph = descriptors::build_part_graph(result.scaffold);
    REQUIRE(graph.nodes.size() == 2);
    // Both nodes should have at least 0 edges and at least 1 edge, but not at
    // least 2 edges.
    REQUIRE(min_0(graph.nodes[0]));
    REQUIRE(min_0(graph.nodes[1]));
    REQUIRE(min_1(graph.nodes[0]));
    REQUIRE(min_1(graph.nodes[1]));
    REQUIRE_FALSE(min_2(graph.nodes[0]));
    REQUIRE_FALSE(min_2(graph.nodes[1]));
  }

  SECTION("Nodes with 2 edges") {
    steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
        {"fragment_two_sockets", "fragment_two_sockets"}, {"joint_two_sockets"},
        {{0, 0, 2, 0},   // fragment[0].socket[0] -> joint[0].socket[0]
         {1, 0, 2, 1}}); // fragment[1].socket[0] -> joint[0].socket[1]
    steamrot::PartGraph graph = descriptors::build_part_graph(result.scaffold);
    REQUIRE(graph.nodes.size() == 3);
    // All nodes should have at least 0 edges, all nodes should have at least
    // 1 edge, and the joint should have at least 2 edges but the fragments
    // should not.
    REQUIRE(min_0(graph.nodes[0]));
    REQUIRE(min_0(graph.nodes[1]));
    REQUIRE(min_0(graph.nodes[2]));
    REQUIRE(min_1(graph.nodes[0]));
    REQUIRE(min_1(graph.nodes[1]));
    REQUIRE(min_1(graph.nodes[2]));
    REQUIRE_FALSE(min_2(graph.nodes[0]));
    REQUIRE_FALSE(min_2(graph.nodes[1]));
    REQUIRE(min_2(graph.nodes[2]));
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
          {"fragment_two_sockets", "fragment_two_sockets"},
          {"joint_two_sockets", "joint_two_sockets", "joint_one_socket"},
          {{0, 0, 2, 0},   // fragment[0].socket[0] -> joint[2].socket[0]
           {0, 1, 3, 0},   // fragment[0].socket[1] -> joint[3].socket[0]
           {1, 0, 2, 1}}); // fragment[1].socket[0] -> joint[2].socket[1]

  // build the graph
  steamrot::PartGraph graph =
      descriptors::build_part_graph(scaffold_result.scaffold);

  // act & assert
  const std::vector<uint32_t> &part_ids = scaffold_result.part_ids;
  REQUIRE(graph.nodes.size() == 5);
  // fragment[0] has 2 edges, fragment[1] has 1 edge, joint[2] has 2 edges,
  // joint[3] has 1 edge, and joint[4] has 0 edges
  REQUIRE_FALSE(max_0(graph.nodes[0]));
  REQUIRE_FALSE(max_0(graph.nodes[1]));
  REQUIRE_FALSE(max_0(graph.nodes[2]));
  REQUIRE_FALSE(max_0(graph.nodes[3]));
  REQUIRE(max_0(graph.nodes[4]));

  REQUIRE_FALSE(max_1(graph.nodes[0]));
  REQUIRE(max_1(graph.nodes[1]));
  REQUIRE_FALSE(max_1(graph.nodes[2]));
  REQUIRE(max_1(graph.nodes[3]));
  REQUIRE(max_1(graph.nodes[4]));

  REQUIRE(max_2(graph.nodes[0]));
  REQUIRE(max_2(graph.nodes[1]));
  REQUIRE(max_2(graph.nodes[2]));
  REQUIRE(max_2(graph.nodes[3]));
  REQUIRE(max_2(graph.nodes[4]));

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
        {"fragment_two_sockets", "fragment_two_sockets"}, {"joint_two_sockets"},
        {{0, 0, 2, 0},   // fragment[0].socket[0] -> joint[0].socket[0]
         {1, 0, 2, 1}}); // fragment[1].socket[0] -> joint[0].socket[1]
    steamrot::PartGraph graph = descriptors::build_part_graph(result.scaffold);
    REQUIRE(graph.nodes.size() == 3);
    // joint[0] has 2 edges, so it should not be terminal
    REQUIRE(not_terminal(graph.nodes[2]));
  }
  SECTION("Returns true for nodes with 0 or 1 edge") {
    steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
        {"fragment_two_sockets", "fragment_two_sockets"}, {"joint_two_sockets"},
        {{0, 0, 2, 0}}); // fragment[0].socket[0] -> joint[0].socket[0]
    steamrot::PartGraph graph = descriptors::build_part_graph(result.scaffold);
    REQUIRE(graph.nodes.size() == 3);
    // fragment[1] has 0 edges and joint[0] has 1 edge, so both should be
    // terminal
    REQUIRE(descriptors::is_terminal(graph.nodes[0]));
    REQUIRE(descriptors::is_terminal(graph.nodes[1]));
    REQUIRE(descriptors::is_terminal(graph.nodes[2]));
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
