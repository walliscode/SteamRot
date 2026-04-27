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
#include <unordered_set>

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

TEST_CASE("has_available_socket returns true when all sockets are Available",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::MachinaFormScaffold scaffold =
      builder.MakeScaffoldWithParts({"fragment_two_sockets"}, {});
  steamrot::PartGraph graph = agm::build_part_graph(scaffold);
  REQUIRE(graph.nodes.size() == 1);
  // Default SocketState is Available
  REQUIRE(agm::has_available_socket(graph.nodes[0]));
}

TEST_CASE("has_available_socket returns false when all sockets are Connected",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::MachinaFormScaffold scaffold =
      builder.MakeScaffoldWithParts({"fragment_two_sockets"}, {});

  // Mark all sockets on the single fragment as Connected
  for (auto &[id, variant] : scaffold.parts) {
    auto &frag = std::get<steamrot::FragmentInstance>(variant);
    for (auto &socket : frag.sockets)
      socket.state = steamrot::SocketState::Connected;
  }

  steamrot::PartGraph graph = agm::build_part_graph(scaffold);
  REQUIRE(graph.nodes.size() == 1);
  REQUIRE_FALSE(agm::has_available_socket(graph.nodes[0]));
}

TEST_CASE("has_available_socket returns false for a node with no sockets",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::MachinaFormScaffold scaffold =
      builder.MakeScaffoldWithParts({"fragment_no_socket"}, {});
  steamrot::PartGraph graph = agm::build_part_graph(scaffold);
  REQUIRE(graph.nodes.size() == 1);
  REQUIRE_FALSE(agm::has_available_socket(graph.nodes[0]));
}

TEST_CASE("NodeDescriptor can wrap atomic predicate functions",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::MachinaFormScaffold scaffold = builder.MakeScaffoldWithParts(
      {"fragment_two_sockets"}, {"joint_one_socket"});
  steamrot::PartGraph graph = agm::build_part_graph(scaffold);
  REQUIRE(graph.nodes.size() == 2);

  steamrot::NodeDescriptor is_frag = agm::is_fragment;
  steamrot::NodeDescriptor is_jnt = agm::is_joint;
  steamrot::NodeDescriptor has_socket = agm::has_available_socket;

  int fragment_count = 0;
  int joint_count = 0;
  int with_socket_count = 0;
  for (const auto &node : graph.nodes) {
    if (is_frag(node))
      ++fragment_count;
    if (is_jnt(node))
      ++joint_count;
    if (has_socket(node))
      ++with_socket_count;
  }
  REQUIRE(fragment_count == 1);
  REQUIRE(joint_count == 1);
  REQUIRE(with_socket_count == 2);
}

TEST_CASE("find_node returns nullptr for unknown id",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  const steamrot::MachinaFormScaffold &scaffold =
      builder.GetScenarioForAnalysis(steamrot::tests::ScaffoldScenario::LinearChain);
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
      builder.GetScenarioForAnalysis(steamrot::tests::ScaffoldScenario::LinearChain);
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
      builder.GetScenarioForAnalysis(steamrot::tests::ScaffoldScenario::LinearChain);
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

TEST_CASE("is_connected tests", "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  SECTION("empty scaffold is connected") {
    steamrot::MachinaFormScaffold empty;
    steamrot::PartGraph graph = agm::build_part_graph(empty);
    REQUIRE(agm::is_connected(graph));
  }

  SECTION("LinearChain is connected") {
    const auto &scaffold =
        builder.GetScenarioForAnalysis(steamrot::tests::ScaffoldScenario::LinearChain);
    steamrot::PartGraph graph = agm::build_part_graph(scaffold);
    REQUIRE(agm::is_connected(graph));
  }

  SECTION("Ring is connected") {
    const auto &scaffold =
        builder.GetScenarioForAnalysis(steamrot::tests::ScaffoldScenario::Ring);
    steamrot::PartGraph graph = agm::build_part_graph(scaffold);
    REQUIRE(agm::is_connected(graph));
  }

  SECTION("two parts with no connections are not connected") {
    steamrot::MachinaFormScaffold scaffold = builder.MakeScaffoldWithParts(
        {"fragment_no_socket", "fragment_no_socket"}, {});
    steamrot::PartGraph graph = agm::build_part_graph(scaffold);
    REQUIRE_FALSE(agm::is_connected(graph));
  }
}

TEST_CASE("bfs visits each node exactly once on LinearChain",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  const steamrot::MachinaFormScaffold &scaffold =
      builder.GetScenarioForAnalysis(steamrot::tests::ScaffoldScenario::LinearChain);
  steamrot::PartGraph graph = agm::build_part_graph(scaffold);

  std::vector<uint32_t> visited_ids;
  agm::bfs(graph, graph.nodes[0].id,
           [&visited_ids](const steamrot::PartNode &node) {
             visited_ids.push_back(node.id);
           });

  // All 3 nodes should be visited exactly once.
  REQUIRE(visited_ids.size() == 3);
  std::unordered_set<uint32_t> unique_ids(visited_ids.begin(),
                                          visited_ids.end());
  REQUIRE(unique_ids.size() == 3);
}

TEST_CASE("bfs on unknown start id is a no-op",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  const steamrot::MachinaFormScaffold &scaffold =
      builder.GetScenarioForAnalysis(steamrot::tests::ScaffoldScenario::LinearChain);
  steamrot::PartGraph graph = agm::build_part_graph(scaffold);

  int visit_count = 0;
  agm::bfs(graph, 9999,
           [&visit_count](const steamrot::PartNode &) { ++visit_count; });
  REQUIRE(visit_count == 0);
}

TEST_CASE("EdgeDescriptor connects_fragment_to_joint on LinearChain",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  const steamrot::MachinaFormScaffold &scaffold =
      builder.GetScenarioForAnalysis(steamrot::tests::ScaffoldScenario::LinearChain);
  steamrot::PartGraph graph = agm::build_part_graph(scaffold);

  steamrot::logic::analysis::grimoire_machina::EdgeDescriptor frag_to_joint =
      agm::connects_fragment_to_joint(graph);
  steamrot::logic::analysis::grimoire_machina::EdgeDescriptor frags =
      agm::connects_fragments(graph);
  steamrot::logic::analysis::grimoire_machina::EdgeDescriptor jnts =
      agm::connects_joints(graph);

  // LinearChain: fragment ─ joint ─ fragment; both edges are fragment–joint.
  REQUIRE_FALSE(graph.edges.empty());
  for (const auto &edge : graph.edges) {
    REQUIRE(frag_to_joint(edge));
    REQUIRE_FALSE(frags(edge));
    REQUIRE_FALSE(jnts(edge));
  }
}

TEST_CASE("EdgeDescriptor connects_joints on Ring",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  const steamrot::MachinaFormScaffold &scaffold =
      builder.GetScenarioForAnalysis(steamrot::tests::ScaffoldScenario::Ring);
  steamrot::PartGraph graph = agm::build_part_graph(scaffold);

  steamrot::logic::analysis::grimoire_machina::EdgeDescriptor jnts =
      agm::connects_joints(graph);
  steamrot::logic::analysis::grimoire_machina::EdgeDescriptor frag_to_joint =
      agm::connects_fragment_to_joint(graph);

  // Ring has only joints; all edges are joint–joint.
  REQUIRE_FALSE(graph.edges.empty());
  for (const auto &edge : graph.edges) {
    REQUIRE(jnts(edge));
    REQUIRE_FALSE(frag_to_joint(edge));
  }
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

  steamrot::NodeDescriptor is_both =
      agm::and_(agm::is_fragment, agm::is_joint);
  steamrot::NodeDescriptor is_either =
      agm::or_(agm::is_fragment, agm::is_joint);
  steamrot::NodeDescriptor not_fragment = agm::not_(agm::is_fragment);

  for (const auto &node : graph.nodes) {
    REQUIRE_FALSE(is_both(node));
    REQUIRE(is_either(node));
    REQUIRE(not_fragment(node) == agm::is_joint(node));
  }
}

TEST_CASE("has_maximum_n_connected_sockets tests",
          "[unit][analysis][grimoire_machina]") {

  // arrange
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  SECTION("Function construction does not throw") {
    REQUIRE_NOTHROW(agm::has_maximum_n_connected_sockets(0));
    REQUIRE_NOTHROW(agm::has_maximum_n_connected_sockets(1));
    REQUIRE_NOTHROW(agm::has_maximum_n_connected_sockets(2));
  }
  // set up some NodeDescriptors for testing
  steamrot::NodeDescriptor max_0 = agm::has_maximum_n_connected_sockets(0);
  steamrot::NodeDescriptor max_1 = agm::has_maximum_n_connected_sockets(1);
  steamrot::NodeDescriptor max_2 = agm::has_maximum_n_connected_sockets(2);

  // create a scaffold with two fragments and two joints, creating various
  // combos of connections
  steamrot::tests::ScaffoldResult scaffold_result =
      builder.MakeConnectedScaffold(
          {"fragment_two_sockets", "fragment_two_sockets"},
          {"joint_two_sockets", "joint_two_sockets"},
          {{0, 0, 2, 0},   // fragment[0].socket[0] -> joint[2].socket[0]
           {0, 1, 3, 0},   // fragment[0].socket[1] -> joint[3].socket[0]
           {1, 0, 2, 1}}); // fragment[1].socket[0] -> joint[2].socket[1]

  // build the graph
  steamrot::PartGraph graph = agm::build_part_graph(scaffold_result.scaffold);

  // act & assert
  for (const auto &node : graph.nodes) {
    // node 0: fragment with 2 connected sockets
    if (node.id == scaffold_result.part_ids[0]) {
      REQUIRE_FALSE(max_0(node));
      REQUIRE_FALSE(max_1(node));
      REQUIRE(max_2(node));
    }
    // node 1: fragment with 1 connected socket
    else if (node.id == scaffold_result.part_ids[1]) {
      REQUIRE_FALSE(max_0(node));
      REQUIRE(max_1(node));
      REQUIRE(max_2(node));
    }
    // node 2: joint with 2 connected sockets
    else if (node.id == scaffold_result.part_ids[2]) {
      REQUIRE_FALSE(max_0(node));
      REQUIRE_FALSE(max_1(node));
      REQUIRE(max_2(node));
    }
    // node 3: joint with 1 connected socket
    else if (node.id == scaffold_result.part_ids[3]) {
      REQUIRE_FALSE(max_0(node));
      REQUIRE(max_1(node));
      REQUIRE(max_2(node));
    } else {
      FAIL("Unexpected node ID " << node.id);
    }
  }
}
