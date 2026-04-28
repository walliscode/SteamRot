/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for analysis_grimoire_machina free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "analysis_grimoire_machina.h"
#include "MachinaFormScaffold.h"
#include "PartGraph.h"
#include "part_library.h"
#include <catch2/catch_test_macros.hpp>
#include <vector>

namespace agm = steamrot::logic::analysis::grimoire_machina;

TEST_CASE("build_part_graph from empty scaffold yields empty graph",
          "[unit][analysis][grimoire_machina]") {
  steamrot::MachinaFormScaffold scaffold;
  steamrot::PartGraph graph = agm::build_part_graph(scaffold);
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

  steamrot::PartGraph graph = agm::build_part_graph(scaffold);
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

  steamrot::PartGraph graph = agm::build_part_graph(result.scaffold);
  REQUIRE(graph.nodes.size() == 2);
  REQUIRE(graph.edges.size() == 1);
}

TEST_CASE("is_fragment and is_joint correctly identify node types",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::MachinaFormScaffold scaffold = builder.MakeScaffoldWithParts(
      {"fragment_one_socket"}, {"joint_one_socket"});

  steamrot::PartGraph graph = agm::build_part_graph(scaffold);
  REQUIRE(graph.nodes.size() == 2);

  int fragment_count = 0;
  int joint_count = 0;
  for (const auto &node : graph.nodes) {
    if (agm::is_fragment(node))
      ++fragment_count;
    if (agm::is_joint(node))
      ++joint_count;
  }
  REQUIRE(fragment_count == 1);
  REQUIRE(joint_count == 1);
}

TEST_CASE("find_node returns nullptr for unknown id",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  const steamrot::MachinaFormScaffold &scaffold =
      builder.GetScenarioForAnalysis(
          steamrot::tests::ScaffoldScenario::LinearChain);
  steamrot::PartGraph graph = agm::build_part_graph(scaffold);

  REQUIRE(agm::find_node(graph, 9999) == nullptr);
}

TEST_CASE("find_node returns valid pointer for known id",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
      {"fragment_two_sockets"}, {"joint_two_sockets"}, {{0, 0, 1, 0}});
  steamrot::PartGraph graph = agm::build_part_graph(result.scaffold);

  const steamrot::PartNode *node = agm::find_node(graph, result.part_ids[0]);
  REQUIRE(node != nullptr);
  REQUIRE(node->id == result.part_ids[0]);
}

TEST_CASE("get_neighbors returns correct neighbors for LinearChain",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  const steamrot::MachinaFormScaffold &scaffold =
      builder.GetScenarioForAnalysis(
          steamrot::tests::ScaffoldScenario::LinearChain);
  steamrot::PartGraph graph = agm::build_part_graph(scaffold);

  // The sole joint is the middle node — it should have exactly 2 neighbors,
  // both of which are fragments.
  auto joints = agm::find_nodes_matching(graph, agm::is_joint);
  REQUIRE(joints.size() == 1);

  auto neighbors = agm::get_neighbors(graph, *joints[0]);
  REQUIRE(neighbors.size() == 2);
  for (const auto *n : neighbors)
    REQUIRE(agm::is_fragment(*n));
}

TEST_CASE("get_neighbors returns correct neighbors for Ring",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  const steamrot::MachinaFormScaffold &scaffold =
      builder.GetScenarioForAnalysis(steamrot::tests::ScaffoldScenario::Ring);
  steamrot::PartGraph graph = agm::build_part_graph(scaffold);

  // Every joint in the Ring has exactly 2 neighbors, both joints.
  for (const auto &node : graph.nodes) {
    auto neighbors = agm::get_neighbors(graph, node);
    REQUIRE(neighbors.size() == 2);
    for (const auto *n : neighbors)
      REQUIRE(agm::is_joint(*n));
  }
}

TEST_CASE("find_nodes_matching filters correctly",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  const steamrot::MachinaFormScaffold &scaffold =
      builder.GetScenarioForAnalysis(
          steamrot::tests::ScaffoldScenario::LinearChain);
  steamrot::PartGraph graph = agm::build_part_graph(scaffold);

  auto fragments = agm::find_nodes_matching(graph, agm::is_fragment);
  auto joints = agm::find_nodes_matching(graph, agm::is_joint);

  // LinearChain: fragment ─ joint ─ fragment
  REQUIRE(fragments.size() == 2);
  REQUIRE(joints.size() == 1);
}

TEST_CASE("count_nodes_matching counts correctly",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  const steamrot::MachinaFormScaffold &scaffold =
      builder.GetScenarioForAnalysis(steamrot::tests::ScaffoldScenario::Ring);
  steamrot::PartGraph graph = agm::build_part_graph(scaffold);

  // Ring has 3 joints and 0 fragments
  REQUIRE(agm::count_nodes_matching(graph, agm::is_joint) == 3);
  REQUIRE(agm::count_nodes_matching(graph, agm::is_fragment) == 0);
}

TEST_CASE("predicate combinators compose correctly",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::MachinaFormScaffold scaffold = builder.MakeScaffoldWithParts(
      {"fragment_two_sockets"}, {"joint_two_sockets"});
  steamrot::PartGraph graph = agm::build_part_graph(scaffold);
  REQUIRE(graph.nodes.size() == 2);

  steamrot::NodeDescriptor is_both = agm::and_(agm::is_fragment, agm::is_joint);
  steamrot::NodeDescriptor is_either =
      agm::or_(agm::is_fragment, agm::is_joint);
  steamrot::NodeDescriptor not_fragment = agm::not_(agm::is_fragment);

  for (const auto &node : graph.nodes) {
    REQUIRE_FALSE(is_both(node));
    REQUIRE(is_either(node));
    REQUIRE(not_fragment(node) == agm::is_joint(node));
  }
}

TEST_CASE("has_exactly_n_edges tests", "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::NodeDescriptor has_0 = agm::has_exactly_n_edges(0);
  steamrot::NodeDescriptor has_1 = agm::has_exactly_n_edges(1);
  steamrot::NodeDescriptor has_2 = agm::has_exactly_n_edges(2);

  SECTION("Nodes with 0 edges") {
    steamrot::MachinaFormScaffold scaffold = builder.MakeScaffoldWithParts(
        {"fragment_two_sockets"}, {"joint_two_sockets"});
    steamrot::PartGraph graph = agm::build_part_graph(scaffold);
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
    steamrot::PartGraph graph = agm::build_part_graph(result.scaffold);
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
    steamrot::PartGraph graph = agm::build_part_graph(result.scaffold);
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
}

TEST_CASE("has_maximum_n_connected_sockets tests",
          "[unit][analysis][grimoire_machina]") {

  // arrange
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  // set up some NodeDescriptors for testing
  steamrot::NodeDescriptor max_0 = agm::has_maximum_n_edges(0);
  steamrot::NodeDescriptor max_1 = agm::has_maximum_n_edges(1);
  steamrot::NodeDescriptor max_2 = agm::has_maximum_n_edges(2);

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
  steamrot::PartGraph graph = agm::build_part_graph(scaffold_result.scaffold);

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
}

TEST_CASE("is_terminal tests", "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::NodeDescriptor not_terminal = agm::not_(agm::is_terminal);

  SECTION("Returns false for nodes with 2 or more edges") {
    steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
        {"fragment_two_sockets", "fragment_two_sockets"}, {"joint_two_sockets"},
        {{0, 0, 2, 0},   // fragment[0].socket[0] -> joint[0].socket[0]
         {1, 0, 2, 1}}); // fragment[1].socket[0] -> joint[0].socket[1]
    steamrot::PartGraph graph = agm::build_part_graph(result.scaffold);
    REQUIRE(graph.nodes.size() == 3);
    // joint[0] has 2 edges, so it should not be terminal
    REQUIRE(not_terminal(graph.nodes[2]));
  }
  SECTION("Returns true for nodes with 0 or 1 edge") {
    steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
        {"fragment_two_sockets", "fragment_two_sockets"}, {"joint_two_sockets"},
        {{0, 0, 2, 0}}); // fragment[0].socket[0] -> joint[0].socket[0]
    steamrot::PartGraph graph = agm::build_part_graph(result.scaffold);
    REQUIRE(graph.nodes.size() == 3);
    // fragment[1] has 0 edges and joint[0] has 1 edge, so both should be
    // terminal
    REQUIRE(agm::is_terminal(graph.nodes[0]));
    REQUIRE(agm::is_terminal(graph.nodes[1]));
    REQUIRE(agm::is_terminal(graph.nodes[2]));
  }

  SECTION("Analyses ScaffoldScenario::LinearChain correctly") {
    const steamrot::MachinaFormScaffold &scaffold =
        builder.GetScenarioForAnalysis(
            steamrot::tests::ScaffoldScenario::LinearChain);
    steamrot::PartGraph graph = agm::build_part_graph(scaffold);
    REQUIRE(graph.nodes.size() == 3);
    // The two fragments at the ends of the chain should be terminal, but the
    // joint in the middle should not be
    // because of how the LinearChain is built, the FragentInstances are added
    // first so the joints is [2]
    REQUIRE(agm::is_terminal(graph.nodes[0]));
    REQUIRE(agm::is_terminal(graph.nodes[1]));
    REQUIRE(not_terminal(graph.nodes[2]));
  }

  SECTION("Analyses ScaffoldScenario::Ring correctly") {
    const steamrot::MachinaFormScaffold &scaffold =
        builder.GetScenarioForAnalysis(steamrot::tests::ScaffoldScenario::Ring);
    steamrot::PartGraph graph = agm::build_part_graph(scaffold);
    REQUIRE(graph.nodes.size() == 3);
    // In the Ring scenario, all joints have 2 edges, so none should be
    // terminal
    for (const auto &node : graph.nodes)
      REQUIRE(not_terminal(node));
  }

  SECTION("Analyses ScaffoldScenario::IsolatedPair correctly") {
    const steamrot::MachinaFormScaffold &scaffold =
        builder.GetScenarioForAnalysis(
            steamrot::tests::ScaffoldScenario::IsolatedPair);
    steamrot::PartGraph graph = agm::build_part_graph(scaffold);
    REQUIRE(graph.nodes.size() == 2);
    // Both fragments are connected to each other with 1 edge, so both should
    // be terminal
    REQUIRE(agm::is_terminal(graph.nodes[0]));
    REQUIRE(agm::is_terminal(graph.nodes[1]));
  }
}
