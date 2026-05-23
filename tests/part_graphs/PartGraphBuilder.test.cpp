/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for PartGraphBuilder.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "PartGraphBuilder.h"
#include "MachinaFormScaffold.h"
#include <catch2/catch_test_macros.hpp>

namespace tests = steamrot::tests;

/////////////////////////////////////////////////
/// AddFragment / AddJoint — basic insertion
/////////////////////////////////////////////////

TEST_CASE("AddFragment inserts a FragmentInstance into the part graph",
          "[unit][part_graphs][PartGraphBuilder]") {
  tests::PartGraphBuilder builder;
  const tests::PartGraphPackage pkg =
      builder.AddFragment(tests::FragmentNames::TwoSockets, "f0").Build();

  REQUIRE(pkg.part_graph.size() == 1);
  REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(
      pkg.part_graph.at(0)));
}

TEST_CASE("AddJoint inserts a JointInstance into the part graph",
          "[unit][part_graphs][PartGraphBuilder]") {
  tests::PartGraphBuilder builder;
  const tests::PartGraphPackage pkg =
      builder.AddJoint(tests::JointNames::TwoSockets, "j0").Build();

  REQUIRE(pkg.part_graph.size() == 1);
  REQUIRE(
      std::holds_alternative<steamrot::JointInstance>(pkg.part_graph.at(0)));
}

TEST_CASE("AddFragment assigns the correct stable ID to the instance",
          "[unit][part_graphs][PartGraphBuilder]") {
  tests::PartGraphBuilder builder;
  builder.AddFragment(tests::FragmentNames::OneSocket, "f0")
      .AddFragment(tests::FragmentNames::OneSocket, "f1");
  const tests::PartGraphPackage pkg = builder.Build();

  const auto &f0 = std::get<steamrot::FragmentInstance>(pkg.part_graph.at(0));
  const auto &f1 = std::get<steamrot::FragmentInstance>(pkg.part_graph.at(1));

  REQUIRE(f0.id == 0);
  REQUIRE(f1.id == 1);
}

TEST_CASE("AddJoint assigns the correct stable ID to the instance",
          "[unit][part_graphs][PartGraphBuilder]") {
  tests::PartGraphBuilder builder;
  builder.AddJoint(tests::JointNames::OneSocket, "j0")
      .AddJoint(tests::JointNames::OneSocket, "j1");
  const tests::PartGraphPackage pkg = builder.Build();

  const auto &j0 = std::get<steamrot::JointInstance>(pkg.part_graph.at(0));
  const auto &j1 = std::get<steamrot::JointInstance>(pkg.part_graph.at(1));

  REQUIRE(j0.id == 0);
  REQUIRE(j1.id == 1);
}

TEST_CASE("AddFragment and AddJoint assign IDs from a shared counter",
          "[unit][part_graphs][PartGraphBuilder]") {
  tests::PartGraphBuilder builder;
  builder.AddFragment(tests::FragmentNames::TwoSockets, "f0")
      .AddJoint(tests::JointNames::TwoSockets, "j0")
      .AddFragment(tests::FragmentNames::TwoSockets, "f1");
  const tests::PartGraphPackage pkg = builder.Build();

  REQUIRE(pkg.part_graph.size() == 3);
  REQUIRE(std::get<steamrot::FragmentInstance>(pkg.part_graph.at(0)).id == 0);
  REQUIRE(std::get<steamrot::JointInstance>(pkg.part_graph.at(1)).id == 1);
  REQUIRE(std::get<steamrot::FragmentInstance>(pkg.part_graph.at(2)).id == 2);
}

TEST_CASE("id_to_part_graph_id maps string alias to stable uint32_t ID",
          "[unit][part_graphs][PartGraphBuilder]") {
  tests::PartGraphBuilder builder;
  builder.AddFragment(tests::FragmentNames::TwoSockets, "my_fragment")
      .AddJoint(tests::JointNames::TwoSockets, "my_joint");
  const tests::PartGraphPackage pkg = builder.Build();

  REQUIRE(pkg.id_to_part_graph_id.at("my_fragment") == 0);
  REQUIRE(pkg.id_to_part_graph_id.at("my_joint") == 1);
}

TEST_CASE("AddFragment creates a FragmentInstance with the correct socket count",
          "[unit][part_graphs][PartGraphBuilder]") {
  SECTION("TwoSockets fragment has two sockets") {
    tests::PartGraphBuilder builder;
    const tests::PartGraphPackage pkg =
        builder.AddFragment(tests::FragmentNames::TwoSockets, "f0").Build();
    const auto &fi =
        std::get<steamrot::FragmentInstance>(pkg.part_graph.at(0));
    REQUIRE(fi.sockets.size() == 2);
  }
  SECTION("NoSocket fragment has zero sockets") {
    tests::PartGraphBuilder builder;
    const tests::PartGraphPackage pkg =
        builder.AddFragment(tests::FragmentNames::NoSocket, "f0").Build();
    const auto &fi =
        std::get<steamrot::FragmentInstance>(pkg.part_graph.at(0));
    REQUIRE(fi.sockets.empty());
  }
}

TEST_CASE("AddJoint creates a JointInstance with the correct socket count",
          "[unit][part_graphs][PartGraphBuilder]") {
  SECTION("ThreeSockets joint has three sockets") {
    tests::PartGraphBuilder builder;
    const tests::PartGraphPackage pkg =
        builder.AddJoint(tests::JointNames::ThreeSockets, "j0").Build();
    const auto &ji = std::get<steamrot::JointInstance>(pkg.part_graph.at(0));
    REQUIRE(ji.sockets.size() == 3);
  }
  SECTION("NoSocket joint has zero sockets") {
    tests::PartGraphBuilder builder;
    const tests::PartGraphPackage pkg =
        builder.AddJoint(tests::JointNames::NoSocket, "j0").Build();
    const auto &ji = std::get<steamrot::JointInstance>(pkg.part_graph.at(0));
    REQUIRE(ji.sockets.empty());
  }
}

/////////////////////////////////////////////////
/// Connect — successful connections
/////////////////////////////////////////////////

TEST_CASE("Connect(fragment, joint) wires socket state on both ends",
          "[unit][part_graphs][PartGraphBuilder]") {
  tests::PartGraphBuilder builder;
  const tests::PartGraphPackage pkg =
      builder.AddFragment(tests::FragmentNames::TwoSockets, "frag")
          .AddJoint(tests::JointNames::TwoSockets, "joint")
          .Connect("frag", 0, "joint", 0)
          .Build();

  const auto &fi = std::get<steamrot::FragmentInstance>(pkg.part_graph.at(0));
  const auto &ji = std::get<steamrot::JointInstance>(pkg.part_graph.at(1));

  REQUIRE(fi.sockets.at(0).state == steamrot::SocketState::Connected);
  REQUIRE(ji.sockets.at(0).state == steamrot::SocketState::Connected);
}

TEST_CASE("Connect(joint, fragment) wires socket state on both ends",
          "[unit][part_graphs][PartGraphBuilder]") {
  // Joint listed first, fragment second — order should not matter
  tests::PartGraphBuilder builder;
  const tests::PartGraphPackage pkg =
      builder.AddJoint(tests::JointNames::TwoSockets, "joint")
          .AddFragment(tests::FragmentNames::TwoSockets, "frag")
          .Connect("joint", 1, "frag", 0)
          .Build();

  const auto &ji = std::get<steamrot::JointInstance>(pkg.part_graph.at(0));
  const auto &fi = std::get<steamrot::FragmentInstance>(pkg.part_graph.at(1));

  REQUIRE(ji.sockets.at(1).state == steamrot::SocketState::Connected);
  REQUIRE(fi.sockets.at(0).state == steamrot::SocketState::Connected);
}

TEST_CASE("Connect sets connected_to peer IDs correctly",
          "[unit][part_graphs][PartGraphBuilder]") {
  tests::PartGraphBuilder builder;
  const tests::PartGraphPackage pkg =
      builder.AddFragment(tests::FragmentNames::TwoSockets, "frag")
          .AddJoint(tests::JointNames::TwoSockets, "joint")
          .Connect("frag", 1, "joint", 0)
          .Build();

  const auto &fi = std::get<steamrot::FragmentInstance>(pkg.part_graph.at(0));
  const auto &ji = std::get<steamrot::JointInstance>(pkg.part_graph.at(1));

  // Fragment socket[1] points to joint (id=1, socket=0)
  REQUIRE(fi.sockets.at(1).connected_to.has_value());
  REQUIRE(fi.sockets.at(1).connected_to->peer_part_id == 1);
  REQUIRE(fi.sockets.at(1).connected_to->peer_socket_id == 0);

  // Joint socket[0] points back to fragment (id=0, socket=1)
  REQUIRE(ji.sockets.at(0).connected_to.has_value());
  REQUIRE(ji.sockets.at(0).connected_to->peer_part_id == 0);
  REQUIRE(ji.sockets.at(0).connected_to->peer_socket_id == 1);
}

TEST_CASE("Connect increments connection_count on both ends",
          "[unit][part_graphs][PartGraphBuilder]") {
  tests::PartGraphBuilder builder;
  const tests::PartGraphPackage pkg =
      builder.AddFragment(tests::FragmentNames::TwoSockets, "frag")
          .AddJoint(tests::JointNames::TwoSockets, "joint")
          .Connect("frag", 0, "joint", 0)
          .Build();

  const auto &fi = std::get<steamrot::FragmentInstance>(pkg.part_graph.at(0));
  const auto &ji = std::get<steamrot::JointInstance>(pkg.part_graph.at(1));

  REQUIRE(fi.connection_count == 1);
  REQUIRE(ji.connection_count == 1);
}

TEST_CASE("Connect can chain multiple connections",
          "[unit][part_graphs][PartGraphBuilder]") {
  // f0 ─── j0 ─── f1   (linear chain)
  tests::PartGraphBuilder builder;
  const tests::PartGraphPackage pkg =
      builder
          .AddFragment(tests::FragmentNames::TwoSockets, "f0")
          .AddFragment(tests::FragmentNames::TwoSockets, "f1")
          .AddJoint(tests::JointNames::TwoSockets, "j0")
          .Connect("f0", 0, "j0", 0)
          .Connect("j0", 1, "f1", 0)
          .Build();

  REQUIRE(pkg.part_graph.size() == 3);

  const auto &f0 = std::get<steamrot::FragmentInstance>(pkg.part_graph.at(0));
  const auto &f1 = std::get<steamrot::FragmentInstance>(pkg.part_graph.at(1));
  const auto &j0 = std::get<steamrot::JointInstance>(pkg.part_graph.at(2));

  REQUIRE(f0.connection_count == 1);
  REQUIRE(f1.connection_count == 1);
  REQUIRE(j0.connection_count == 2);
}

/////////////////////////////////////////////////
/// Connect — error cases
/////////////////////////////////////////////////

TEST_CASE("Connect fails when from_id is not in the graph",
          "[unit][part_graphs][PartGraphBuilder]") {
  tests::PartGraphBuilder builder;
  builder.AddFragment(tests::FragmentNames::TwoSockets, "f0")
      .AddJoint(tests::JointNames::TwoSockets, "j0");

  REQUIRE_THROWS(builder.Connect("unknown", 0, "j0", 0).Build());
}

TEST_CASE("Connect fails when to_id is not in the graph",
          "[unit][part_graphs][PartGraphBuilder]") {
  tests::PartGraphBuilder builder;
  builder.AddFragment(tests::FragmentNames::TwoSockets, "f0")
      .AddJoint(tests::JointNames::TwoSockets, "j0");

  REQUIRE_THROWS(builder.Connect("f0", 0, "unknown", 0).Build());
}

TEST_CASE("Connect fails when both parts are fragments (same type)",
          "[unit][part_graphs][PartGraphBuilder]") {
  tests::PartGraphBuilder builder;
  builder.AddFragment(tests::FragmentNames::TwoSockets, "f0")
      .AddFragment(tests::FragmentNames::TwoSockets, "f1");

  REQUIRE_THROWS(builder.Connect("f0", 0, "f1", 0).Build());
}

TEST_CASE("Connect fails when both parts are joints (same type)",
          "[unit][part_graphs][PartGraphBuilder]") {
  tests::PartGraphBuilder builder;
  builder.AddJoint(tests::JointNames::TwoSockets, "j0")
      .AddJoint(tests::JointNames::TwoSockets, "j1");

  REQUIRE_THROWS(builder.Connect("j0", 0, "j1", 0).Build());
}

TEST_CASE("Connect fails when socket ID does not exist on the fragment",
          "[unit][part_graphs][PartGraphBuilder]") {
  tests::PartGraphBuilder builder;
  builder.AddFragment(tests::FragmentNames::TwoSockets, "f0")
      .AddJoint(tests::JointNames::TwoSockets, "j0");

  // fragment_two_sockets has sockets 0 and 1 — socket 99 does not exist
  REQUIRE_THROWS(builder.Connect("f0", 99, "j0", 0).Build());
}

TEST_CASE("Connect fails when socket ID does not exist on the joint",
          "[unit][part_graphs][PartGraphBuilder]") {
  tests::PartGraphBuilder builder;
  builder.AddFragment(tests::FragmentNames::TwoSockets, "f0")
      .AddJoint(tests::JointNames::TwoSockets, "j0");

  // joint_two_sockets has sockets 0 and 1 — socket 99 does not exist
  REQUIRE_THROWS(builder.Connect("f0", 0, "j0", 99).Build());
}

/////////////////////////////////////////////////
/// Build — resets builder state
/////////////////////////////////////////////////

TEST_CASE("Build returns a copy and resets the builder for reuse",
          "[unit][part_graphs][PartGraphBuilder]") {
  tests::PartGraphBuilder builder;
  builder.AddFragment(tests::FragmentNames::TwoSockets, "f0");

  const tests::PartGraphPackage first = builder.Build();

  // The builder has been reset; a second Build should return an empty package
  builder.AddJoint(tests::JointNames::TwoSockets, "j0");
  const tests::PartGraphPackage second = builder.Build();

  REQUIRE(first.part_graph.size() == 1);
  REQUIRE(second.part_graph.size() == 1);
  REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(
      first.part_graph.at(0)));
  REQUIRE(std::holds_alternative<steamrot::JointInstance>(
      second.part_graph.at(0)));
}

TEST_CASE("Build resets the ID counter so the next build starts from 0",
          "[unit][part_graphs][PartGraphBuilder]") {
  tests::PartGraphBuilder builder;
  builder.AddFragment(tests::FragmentNames::TwoSockets, "f0");
  builder.Build(); // consumes and resets

  builder.AddJoint(tests::JointNames::TwoSockets, "j0");
  const tests::PartGraphPackage pkg = builder.Build();

  // First ID in the fresh build must be 0
  REQUIRE(pkg.part_graph.count(0) == 1);
  REQUIRE(std::get<steamrot::JointInstance>(pkg.part_graph.at(0)).id == 0);
}
