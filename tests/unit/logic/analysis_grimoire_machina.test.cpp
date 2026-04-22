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
  steamrot::tests::TestPartLibrary lib = steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::MachinaFormScaffold scaffold =
      builder.MakeScaffoldWithParts({"fragment_one_socket"}, {"joint_one_socket"});

  steamrot::PartGraph graph = agm::build_part_graph(scaffold);
  REQUIRE(graph.nodes.size() == 2);
  REQUIRE(graph.edges.empty());
}

TEST_CASE("build_part_graph edges match scaffold connections count",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib = steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::MachinaFormScaffold scaffold =
      builder.MakeScaffoldWithParts({"fragment_two_sockets"}, {"joint_two_sockets"});

  // Retrieve the two node IDs so we can form a valid connection
  uint32_t frag_id = 0;
  uint32_t joint_id = 0;
  for (const auto &[id, variant] : scaffold.parts) {
    if (std::holds_alternative<steamrot::FragmentInstance>(variant))
      frag_id = id;
    else
      joint_id = id;
  }

  scaffold.connections.emplace_back(
      steamrot::Connection{steamrot::Connection::Endpoint{frag_id, 0},
                           steamrot::Connection::Endpoint{joint_id, 0}});

  steamrot::PartGraph graph = agm::build_part_graph(scaffold);
  REQUIRE(graph.nodes.size() == 2);
  REQUIRE(graph.edges.size() == 1);
}

TEST_CASE("is_fragment and is_joint correctly identify node types",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib = steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::MachinaFormScaffold scaffold =
      builder.MakeScaffoldWithParts({"fragment_one_socket"}, {"joint_one_socket"});

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

TEST_CASE("is_isolated returns true for a node with no edges",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib = steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::MachinaFormScaffold scaffold =
      builder.MakeScaffoldWithParts({"fragment_one_socket"}, {});

  steamrot::PartGraph graph = agm::build_part_graph(scaffold);
  REQUIRE(graph.nodes.size() == 1);
  REQUIRE(agm::is_isolated(graph.nodes[0], graph));
}

TEST_CASE("is_isolated returns false for a node that has an edge",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib = steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::MachinaFormScaffold scaffold =
      builder.MakeScaffoldWithParts({"fragment_one_socket"}, {"joint_one_socket"});

  uint32_t frag_id = 0;
  uint32_t joint_id = 0;
  for (const auto &[id, variant] : scaffold.parts) {
    if (std::holds_alternative<steamrot::FragmentInstance>(variant))
      frag_id = id;
    else
      joint_id = id;
  }

  scaffold.connections.emplace_back(
      steamrot::Connection{steamrot::Connection::Endpoint{frag_id, 0},
                           steamrot::Connection::Endpoint{joint_id, 0}});

  steamrot::PartGraph graph = agm::build_part_graph(scaffold);

  for (const auto &node : graph.nodes) {
    REQUIRE_FALSE(agm::is_isolated(node, graph));
  }
}

TEST_CASE("edge_count returns 0 for isolated node",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib = steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::MachinaFormScaffold scaffold =
      builder.MakeScaffoldWithParts({}, {"joint_two_sockets"});

  steamrot::PartGraph graph = agm::build_part_graph(scaffold);
  REQUIRE(graph.nodes.size() == 1);
  REQUIRE(agm::edge_count(graph.nodes[0], graph) == 0);
}

TEST_CASE("edge_count returns 1 on each node when connected by one edge",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib = steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::MachinaFormScaffold scaffold =
      builder.MakeScaffoldWithParts({"fragment_two_sockets"}, {"joint_two_sockets"});

  uint32_t frag_id = 0;
  uint32_t joint_id = 0;
  for (const auto &[id, variant] : scaffold.parts) {
    if (std::holds_alternative<steamrot::FragmentInstance>(variant))
      frag_id = id;
    else
      joint_id = id;
  }

  scaffold.connections.emplace_back(
      steamrot::Connection{steamrot::Connection::Endpoint{frag_id, 0},
                           steamrot::Connection::Endpoint{joint_id, 0}});

  steamrot::PartGraph graph = agm::build_part_graph(scaffold);

  for (const auto &node : graph.nodes) {
    REQUIRE(agm::edge_count(node, graph) == 1);
  }
}

TEST_CASE("edge_count returns 2 on the joint in a ring of 2 fragments + 1 joint",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib = steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  // 2 fragments + 1 joint, 2 connections (joint connects to each fragment)
  steamrot::MachinaFormScaffold scaffold =
      builder.MakeScaffoldWithParts({"fragment_two_sockets", "fragment_two_sockets"},
                                    {"joint_two_sockets"});

  uint32_t joint_id = 0;
  std::vector<uint32_t> frag_ids;
  for (const auto &[id, variant] : scaffold.parts) {
    if (std::holds_alternative<steamrot::FragmentInstance>(variant))
      frag_ids.push_back(id);
    else
      joint_id = id;
  }
  REQUIRE(frag_ids.size() == 2);

  scaffold.connections.emplace_back(
      steamrot::Connection{steamrot::Connection::Endpoint{frag_ids[0], 0},
                           steamrot::Connection::Endpoint{joint_id, 0}});
  scaffold.connections.emplace_back(
      steamrot::Connection{steamrot::Connection::Endpoint{frag_ids[1], 0},
                           steamrot::Connection::Endpoint{joint_id, 1}});

  steamrot::PartGraph graph = agm::build_part_graph(scaffold);

  // Find the joint node and verify it has edge_count == 2
  const steamrot::PartNode *joint_node = nullptr;
  for (const auto &node : graph.nodes) {
    if (agm::is_joint(node)) {
      joint_node = &node;
      break;
    }
  }
  REQUIRE(joint_node != nullptr);
  REQUIRE(agm::edge_count(*joint_node, graph) == 2);
}

TEST_CASE("socket_count matches the part definition's socket count",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib = steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  SECTION("fragment_three_sockets has socket_count 3") {
    steamrot::MachinaFormScaffold scaffold =
        builder.MakeScaffoldWithParts({"fragment_three_sockets"}, {});
    steamrot::PartGraph graph = agm::build_part_graph(scaffold);
    REQUIRE(graph.nodes.size() == 1);
    REQUIRE(agm::socket_count(graph.nodes[0]) == 3);
  }

  SECTION("joint_two_sockets has socket_count 2") {
    steamrot::MachinaFormScaffold scaffold =
        builder.MakeScaffoldWithParts({}, {"joint_two_sockets"});
    steamrot::PartGraph graph = agm::build_part_graph(scaffold);
    REQUIRE(graph.nodes.size() == 1);
    REQUIRE(agm::socket_count(graph.nodes[0]) == 2);
  }

  SECTION("fragment_no_socket has socket_count 0") {
    steamrot::MachinaFormScaffold scaffold =
        builder.MakeScaffoldWithParts({"fragment_no_socket"}, {});
    steamrot::PartGraph graph = agm::build_part_graph(scaffold);
    REQUIRE(graph.nodes.size() == 1);
    REQUIRE(agm::socket_count(graph.nodes[0]) == 0);
  }
}

TEST_CASE("has_available_socket returns true when all sockets are Available",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib = steamrot::tests::TestPartLibrary::Create();
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
  steamrot::tests::TestPartLibrary lib = steamrot::tests::TestPartLibrary::Create();
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
  steamrot::tests::TestPartLibrary lib = steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::MachinaFormScaffold scaffold =
      builder.MakeScaffoldWithParts({"fragment_no_socket"}, {});
  steamrot::PartGraph graph = agm::build_part_graph(scaffold);
  REQUIRE(graph.nodes.size() == 1);
  REQUIRE_FALSE(agm::has_available_socket(graph.nodes[0]));
}

TEST_CASE("NodeDescriptor can wrap atomic predicate functions",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib = steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  steamrot::MachinaFormScaffold scaffold =
      builder.MakeScaffoldWithParts({"fragment_two_sockets"}, {"joint_one_socket"});
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
