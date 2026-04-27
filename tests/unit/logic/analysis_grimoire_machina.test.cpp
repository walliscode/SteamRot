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
