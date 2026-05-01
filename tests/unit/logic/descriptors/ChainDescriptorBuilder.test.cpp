/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for ChainDescriptorBuilder, ChainStep, and ChainStepKind.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ChainDescriptorBuilder.h"
#include "PartGraph.h"
#include "descriptors_general.h"
#include "descriptors_node_descriptors.h"
#include "part_library.h"
#include <catch2/catch_test_macros.hpp>

namespace descriptors = steamrot::logic::descriptors;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

/// Build a PartGraph from a scaffold and return the indices (node_index_by_id)
/// of the anchor nodes used by most tests.
static steamrot::PartGraph
BuildGraph(const steamrot::tests::ScaffoldResult &result) {
  return descriptors::build_part_graph(result.scaffold);
}

// ---------------------------------------------------------------------------
// Sequence-only chains (StartWith / Then / End)
// ---------------------------------------------------------------------------

TEST_CASE("ChainDescriptorBuilder sequence: two-node chain",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  // Graph: fragment_one_socket -- fragment_one_socket (both terminal)
  steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
      {"fragment_one_socket", "fragment_one_socket"}, {}, {{0, 0, 1, 0}});
  steamrot::PartGraph graph = BuildGraph(result);
  REQUIRE(graph.nodes.size() == 2);

  SECTION("Matches terminal -> terminal starting from either end") {
    descriptors::ChainDescriptor two_terminals =
        descriptors::ChainDescriptorBuilder{}
            .StartWith(descriptors::is_terminal)
            .End(descriptors::is_terminal);

    // Both nodes are terminal and directly connected, so both should match.
    REQUIRE(two_terminals(graph, graph.nodes[0]));
    REQUIRE(two_terminals(graph, graph.nodes[1]));
  }

  SECTION("Does not match when start predicate fails") {
    descriptors::ChainDescriptor requires_joint_start =
        descriptors::ChainDescriptorBuilder{}
            .StartWith(descriptors::is_joint) // neither node is a joint
            .End(descriptors::is_terminal);

    REQUIRE_FALSE(requires_joint_start(graph, graph.nodes[0]));
    REQUIRE_FALSE(requires_joint_start(graph, graph.nodes[1]));
  }

  SECTION("Does not match when end predicate fails") {
    descriptors::ChainDescriptor requires_joint_end =
        descriptors::ChainDescriptorBuilder{}
            .StartWith(descriptors::is_terminal)
            .End(descriptors::is_joint); // neither node is a joint

    REQUIRE_FALSE(requires_joint_end(graph, graph.nodes[0]));
    REQUIRE_FALSE(requires_joint_end(graph, graph.nodes[1]));
  }
}

TEST_CASE("ChainDescriptorBuilder sequence: three-node chain via linear_chain "
          "scenario",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();

  // linear_chain: fragment(0) -- joint(2) -- fragment(1)
  // Nodes: [0]=fragment, [1]=fragment, [2]=joint  (fragments first)
  const steamrot::MachinaFormScaffold &scaffold =
      lib.scaffold_scenarios.at(steamrot::tests::ScaffoldScenario::LinearChain);
  steamrot::PartGraph graph = descriptors::build_part_graph(scaffold);
  REQUIRE(graph.nodes.size() == 3);

  SECTION("terminal -> serial -> terminal matches from fragment endpoints") {
    descriptors::ChainDescriptor three_node =
        descriptors::ChainDescriptorBuilder{}
            .StartWith(descriptors::is_terminal)
            .Then(descriptors::is_serial)
            .End(descriptors::is_terminal);

    // Fragment nodes are the endpoints; they should match.
    REQUIRE(three_node(graph, graph.nodes[0]));
    REQUIRE(three_node(graph, graph.nodes[1]));
    // Joint is in the middle, so it fails the first predicate.
    REQUIRE_FALSE(three_node(graph, graph.nodes[2]));
  }

  SECTION("Does not match a wrong ordering") {
    descriptors::ChainDescriptor wrong_order =
        descriptors::ChainDescriptorBuilder{}
            .StartWith(descriptors::is_serial) // joint first
            .Then(descriptors::is_terminal)
            .End(descriptors::is_terminal);

    // Starting at a fragment: is_serial(fragment) = false
    REQUIRE_FALSE(wrong_order(graph, graph.nodes[0]));
    REQUIRE_FALSE(wrong_order(graph, graph.nodes[1]));
    // Starting at joint: serial passes, then needs two terminal neighbours
    // but only one hop is possible from the joint in one direction at a time.
    // Both neighbours are terminal, so this should succeed from the joint.
    REQUIRE(wrong_order(graph, graph.nodes[2]));
  }
}

// ---------------------------------------------------------------------------
// WhileIsTrue — zero iterations
// ---------------------------------------------------------------------------

TEST_CASE("ChainDescriptorBuilder WhileIsTrue: zero iterations",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  // Graph: fragment_one_socket -- fragment_one_socket
  // Neither node satisfies is_serial, so WhileIsTrue(is_serial) consumes 0.
  steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
      {"fragment_one_socket", "fragment_one_socket"}, {}, {{0, 0, 1, 0}});
  steamrot::PartGraph graph = BuildGraph(result);
  REQUIRE(graph.nodes.size() == 2);

  SECTION("terminal -> WhileIsTrue(serial)[0 iterations] -> terminal") {
    descriptors::ChainDescriptor desc =
        descriptors::ChainDescriptorBuilder{}
            .StartWith(descriptors::is_terminal)
            .WhileIsTrue(descriptors::is_serial) // no serial node present
            .End(descriptors::is_terminal);

    // WhileIsTrue consumes 0 nodes and passes the neighbour to End.
    REQUIRE(desc(graph, graph.nodes[0]));
    REQUIRE(desc(graph, graph.nodes[1]));
  }
}

// ---------------------------------------------------------------------------
// WhileIsTrue — exactly one iteration (linear_chain scenario)
// ---------------------------------------------------------------------------

TEST_CASE("ChainDescriptorBuilder WhileIsTrue: one iteration via linear_chain",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();

  // linear_chain: fragment(0) -- joint(2) -- fragment(1)
  // joint is serial; both fragments are terminal.
  const steamrot::MachinaFormScaffold &scaffold =
      lib.scaffold_scenarios.at(steamrot::tests::ScaffoldScenario::LinearChain);
  steamrot::PartGraph graph = descriptors::build_part_graph(scaffold);
  REQUIRE(graph.nodes.size() == 3);

  SECTION(
      "terminal -> WhileIsTrue(serial)[1 iteration] -> terminal matches from "
      "fragment endpoints") {
    descriptors::ChainDescriptor desc =
        descriptors::ChainDescriptorBuilder{}
            .StartWith(descriptors::is_terminal)
            .WhileIsTrue(descriptors::is_serial)
            .End(descriptors::is_terminal);

    // Fragments are the endpoints; each walks through the serial joint.
    REQUIRE(desc(graph, graph.nodes[0]));
    REQUIRE(desc(graph, graph.nodes[1]));
    // Starting at the joint: is_terminal(joint) = false
    REQUIRE_FALSE(desc(graph, graph.nodes[2]));
  }
}

// ---------------------------------------------------------------------------
// WhileIsTrue — multiple iterations
// ---------------------------------------------------------------------------

TEST_CASE("ChainDescriptorBuilder WhileIsTrue: multiple iterations",
          "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  // Build: fragment_one_socket -- joint_two_sockets -- joint_two_sockets
  //        -- fragment_one_socket
  // Indices: fragment[0]=0, fragment[1]=1, joint[0]=2, joint[1]=3
  // Connections: 0-2, 2-3, 3-1  (fragment0 - joint0 - joint1 - fragment1)
  steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
      {"fragment_one_socket", "fragment_one_socket"},
      {"joint_two_sockets", "joint_two_sockets"},
      {
          {0, 0, 2, 0}, // fragment[0].socket[0] -> joint[0].socket[0]
          {2, 1, 3, 0}, // joint[0].socket[1]    -> joint[1].socket[0]
          {3, 1, 1, 0}, // joint[1].socket[1]    -> fragment[1].socket[0]
      });
  steamrot::PartGraph graph = BuildGraph(result);
  REQUIRE(graph.nodes.size() == 4);

  SECTION("terminal -> WhileIsTrue(serial)[2 iterations] -> terminal") {
    descriptors::ChainDescriptor desc =
        descriptors::ChainDescriptorBuilder{}
            .StartWith(descriptors::is_terminal)
            .WhileIsTrue(descriptors::is_serial)
            .End(descriptors::is_terminal);

    // nodes[0] and [1] are the fragment endpoints (terminal); they should
    // each find a path through both serial joints to the other terminal.
    REQUIRE(desc(graph, graph.nodes[0]));
    REQUIRE(desc(graph, graph.nodes[1]));

    // Joint nodes are serial, not terminal, so they fail StartWith.
    REQUIRE_FALSE(desc(graph, graph.nodes[2]));
    REQUIRE_FALSE(desc(graph, graph.nodes[3]));
  }
}

// ---------------------------------------------------------------------------
// ChainStepKind — extensibility check
// ---------------------------------------------------------------------------

TEST_CASE("ChainStepKind enum values are distinct",
          "[unit][analysis][grimoire_machina]") {
  REQUIRE(descriptors::ChainStepKind::Sequence !=
          descriptors::ChainStepKind::WhileIsTrue);
}

// ---------------------------------------------------------------------------
// ChainStep — struct construction
// ---------------------------------------------------------------------------

TEST_CASE("ChainStep default kind is Sequence",
          "[unit][analysis][grimoire_machina]") {
  descriptors::ChainStep step{descriptors::is_terminal};
  REQUIRE(step.kind == descriptors::ChainStepKind::Sequence);
}

TEST_CASE("ChainStep stores the provided kind",
          "[unit][analysis][grimoire_machina]") {
  descriptors::ChainStep step{descriptors::is_terminal,
                              descriptors::ChainStepKind::WhileIsTrue};
  REQUIRE(step.kind == descriptors::ChainStepKind::WhileIsTrue);
}
