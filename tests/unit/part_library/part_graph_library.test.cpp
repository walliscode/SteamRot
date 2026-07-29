/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the pre-built PartGraphPackage objects declared in
///        part_graph_library.h.
///
/// Each test verifies the structural invariants of a library object:
/// part count, part types, IDs, socket counts, connection_count, and the
/// bidirectional connected_to links stored on each socket.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "part_graph_library.h"
#include "MachinaFormScaffold.h"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

TEST_CASE("pair has exactly two parts",
          "[unit][part_graphs][part_graph_library]") {
  REQUIRE(tests::pair.part_graph.size() == 2);
}

TEST_CASE("pair part 0 is a FragmentInstance",
          "[unit][part_graphs][part_graph_library]") {
  REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(
      tests::pair.part_graph.at(0)));
}

TEST_CASE("pair part 1 is a JointInstance",
          "[unit][part_graphs][part_graph_library]") {
  REQUIRE(std::holds_alternative<steamrot::JointInstance>(
      tests::pair.part_graph.at(1)));
}

TEST_CASE("pair part IDs match their map keys",
          "[unit][part_graphs][part_graph_library]") {
  const auto &f0 =
      std::get<steamrot::FragmentInstance>(tests::pair.part_graph.at(0));
  const auto &j0 =
      std::get<steamrot::JointInstance>(tests::pair.part_graph.at(1));
  REQUIRE(f0.GetId() == 0);
  REQUIRE(j0.GetId() == 1);
}

TEST_CASE("pair id_to_part_graph_id contains correct aliases",
          "[unit][part_graphs][part_graph_library]") {
  REQUIRE(tests::pair.id_to_part_graph_id.at("f0") == 0);
  REQUIRE(tests::pair.id_to_part_graph_id.at("j0") == 1);
}

TEST_CASE("pair fragment has one socket",
          "[unit][part_graphs][part_graph_library]") {
  const auto &f0 =
      std::get<steamrot::FragmentInstance>(tests::pair.part_graph.at(0));
  REQUIRE(f0.GetSocketCount() == 1);
}

TEST_CASE("pair joint has one socket",
          "[unit][part_graphs][part_graph_library]") {
  const auto &j0 =
      std::get<steamrot::JointInstance>(tests::pair.part_graph.at(1));
  REQUIRE(j0.GetSocketCount() == 1);
}

TEST_CASE("pair both parts have connection_count 1",
          "[unit][part_graphs][part_graph_library]") {
  const auto &f0 =
      std::get<steamrot::FragmentInstance>(tests::pair.part_graph.at(0));
  const auto &j0 =
      std::get<steamrot::JointInstance>(tests::pair.part_graph.at(1));
  REQUIRE(f0.GetNumberOfConnectedSockets() == 1);
  REQUIRE(j0.GetNumberOfConnectedSockets() == 1);
}

TEST_CASE("pair sockets are Connected and reference each other",
          "[unit][part_graphs][part_graph_library]") {
  const auto &f0 =
      std::get<steamrot::FragmentInstance>(tests::pair.part_graph.at(0));
  const auto &j0 =
      std::get<steamrot::JointInstance>(tests::pair.part_graph.at(1));

  const auto *f0s0 = f0.TryGetSocket(0);
  REQUIRE(f0s0 != nullptr);
  REQUIRE(f0s0->GetConnectionState() ==
          steamrot::SocketConnectionState::Connected);
  REQUIRE(f0s0->GetConnection().has_value());
  REQUIRE(f0s0->GetConnection()->peer_part_id == 1);
  REQUIRE(f0s0->GetConnection()->peer_socket_id == 0);

  const auto *j0s0 = j0.TryGetSocket(0);
  REQUIRE(j0s0 != nullptr);
  REQUIRE(j0s0->GetConnectionState() ==
          steamrot::SocketConnectionState::Connected);
  REQUIRE(j0s0->GetConnection().has_value());
  REQUIRE(j0s0->GetConnection()->peer_part_id == 0);
  REQUIRE(j0s0->GetConnection()->peer_socket_id == 0);
}

/////////////////////////////////////////////////
/// linear_chain_3:  f0 ── j0 ── f1
///   IDs:            0    1    2
/////////////////////////////////////////////////

TEST_CASE("linear_chain_3 has exactly three parts",
          "[unit][part_graphs][part_graph_library]") {
  REQUIRE(tests::linear_chain_3.part_graph.size() == 3);
}

TEST_CASE("linear_chain_3 part types are Fragment, Joint, Fragment",
          "[unit][part_graphs][part_graph_library]") {
  REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(
      tests::linear_chain_3.part_graph.at(0)));
  REQUIRE(std::holds_alternative<steamrot::JointInstance>(
      tests::linear_chain_3.part_graph.at(1)));
  REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(
      tests::linear_chain_3.part_graph.at(2)));
}

TEST_CASE("linear_chain_3 part IDs match their map keys",
          "[unit][part_graphs][part_graph_library]") {
  const auto &f0 = std::get<steamrot::FragmentInstance>(
      tests::linear_chain_3.part_graph.at(0));
  const auto &j0 =
      std::get<steamrot::JointInstance>(tests::linear_chain_3.part_graph.at(1));
  const auto &f1 = std::get<steamrot::FragmentInstance>(
      tests::linear_chain_3.part_graph.at(2));
  REQUIRE(f0.GetId() == 0);
  REQUIRE(j0.GetId() == 1);
  REQUIRE(f1.GetId() == 2);
}

TEST_CASE("linear_chain_3 id_to_part_graph_id contains correct aliases",
          "[unit][part_graphs][part_graph_library]") {
  REQUIRE(tests::linear_chain_3.id_to_part_graph_id.at("f0") == 0);
  REQUIRE(tests::linear_chain_3.id_to_part_graph_id.at("j0") == 1);
  REQUIRE(tests::linear_chain_3.id_to_part_graph_id.at("f1") == 2);
}

TEST_CASE("linear_chain_3 terminal fragments have one socket each",
          "[unit][part_graphs][part_graph_library]") {
  const auto &f0 = std::get<steamrot::FragmentInstance>(
      tests::linear_chain_3.part_graph.at(0));
  const auto &f1 = std::get<steamrot::FragmentInstance>(
      tests::linear_chain_3.part_graph.at(2));
  REQUIRE(f0.GetSocketCount() == 1);
  REQUIRE(f1.GetSocketCount() == 1);
}

TEST_CASE("linear_chain_3 joint has two sockets",
          "[unit][part_graphs][part_graph_library]") {
  const auto &j0 =
      std::get<steamrot::JointInstance>(tests::linear_chain_3.part_graph.at(1));
  REQUIRE(j0.GetSocketCount() == 2);
}

TEST_CASE("linear_chain_3 connection counts are correct",
          "[unit][part_graphs][part_graph_library]") {
  const auto &f0 = std::get<steamrot::FragmentInstance>(
      tests::linear_chain_3.part_graph.at(0));
  const auto &j0 =
      std::get<steamrot::JointInstance>(tests::linear_chain_3.part_graph.at(1));
  const auto &f1 = std::get<steamrot::FragmentInstance>(
      tests::linear_chain_3.part_graph.at(2));

  REQUIRE(f0.GetNumberOfConnectedSockets() == 1);
  REQUIRE(j0.GetNumberOfConnectedSockets() == 2);
  REQUIRE(f1.GetNumberOfConnectedSockets() == 1);
}

TEST_CASE("linear_chain_3 socket connections are bidirectionally correct",
          "[unit][part_graphs][part_graph_library]") {
  const auto &f0 = std::get<steamrot::FragmentInstance>(
      tests::linear_chain_3.part_graph.at(0));
  const auto &j0 =
      std::get<steamrot::JointInstance>(tests::linear_chain_3.part_graph.at(1));
  const auto &f1 = std::get<steamrot::FragmentInstance>(
      tests::linear_chain_3.part_graph.at(2));

  const auto *f0s0 = f0.TryGetSocket(0);
  REQUIRE(f0s0 != nullptr);
  REQUIRE(f0s0->GetConnectionState() ==
          steamrot::SocketConnectionState::Connected);
  REQUIRE(f0s0->GetConnection().has_value());
  REQUIRE(f0s0->GetConnection()->peer_part_id == 1);

  const auto *j0s0 = j0.TryGetSocket(0);
  REQUIRE(j0s0 != nullptr);
  REQUIRE(j0s0->GetConnectionState() ==
          steamrot::SocketConnectionState::Connected);
  REQUIRE(j0s0->GetConnection().has_value());
  REQUIRE(j0s0->GetConnection()->peer_part_id == 0);

  const auto *j0s1 = j0.TryGetSocket(1);
  REQUIRE(j0s1 != nullptr);
  REQUIRE(j0s1->GetConnectionState() ==
          steamrot::SocketConnectionState::Connected);
  REQUIRE(j0s1->GetConnection().has_value());
  REQUIRE(j0s1->GetConnection()->peer_part_id == 2);

  const auto *f1s0 = f1.TryGetSocket(0);
  REQUIRE(f1s0 != nullptr);
  REQUIRE(f1s0->GetConnectionState() ==
          steamrot::SocketConnectionState::Connected);
  REQUIRE(f1s0->GetConnection().has_value());
  REQUIRE(f1s0->GetConnection()->peer_part_id == 1);
  REQUIRE(f1s0->GetConnection()->peer_socket_id == 1);
}

/////////////////////////////////////////////////
/// linear_chain_5:  f0 ── j0 ── f1 ── j1 ── f2
///   IDs:            0    1    2    3    4
/////////////////////////////////////////////////

TEST_CASE("linear_chain_5 has exactly five parts",
          "[unit][part_graphs][part_graph_library]") {
  REQUIRE(tests::linear_chain_5.part_graph.size() == 5);
}

TEST_CASE("linear_chain_5 part types alternate "
          "Fragment-Joint-Fragment-Joint-Fragment",
          "[unit][part_graphs][part_graph_library]") {
  REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(
      tests::linear_chain_5.part_graph.at(0)));
  REQUIRE(std::holds_alternative<steamrot::JointInstance>(
      tests::linear_chain_5.part_graph.at(1)));
  REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(
      tests::linear_chain_5.part_graph.at(2)));
  REQUIRE(std::holds_alternative<steamrot::JointInstance>(
      tests::linear_chain_5.part_graph.at(3)));
  REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(
      tests::linear_chain_5.part_graph.at(4)));
}

TEST_CASE("linear_chain_5 part IDs match their map keys",
          "[unit][part_graphs][part_graph_library]") {
  for (uint32_t i = 0; i < 5; ++i) {
    const auto &variant = tests::linear_chain_5.part_graph.at(i);
    const uint32_t id =
        std::visit([](const auto &inst) { return inst.GetId(); }, variant);
    REQUIRE(id == i);
  }
}

TEST_CASE("linear_chain_5 id_to_part_graph_id contains correct aliases",
          "[unit][part_graphs][part_graph_library]") {
  REQUIRE(tests::linear_chain_5.id_to_part_graph_id.at("f0") == 0);
  REQUIRE(tests::linear_chain_5.id_to_part_graph_id.at("j0") == 1);
  REQUIRE(tests::linear_chain_5.id_to_part_graph_id.at("f1") == 2);
  REQUIRE(tests::linear_chain_5.id_to_part_graph_id.at("j1") == 3);
  REQUIRE(tests::linear_chain_5.id_to_part_graph_id.at("f2") == 4);
}

TEST_CASE("linear_chain_5 terminal fragments have one socket, middle fragment "
          "has two",
          "[unit][part_graphs][part_graph_library]") {
  const auto &f0 = std::get<steamrot::FragmentInstance>(
      tests::linear_chain_5.part_graph.at(0));
  const auto &f1 = std::get<steamrot::FragmentInstance>(
      tests::linear_chain_5.part_graph.at(2));
  const auto &f2 = std::get<steamrot::FragmentInstance>(
      tests::linear_chain_5.part_graph.at(4));
  REQUIRE(f0.GetSocketCount() == 1);
  REQUIRE(f1.GetSocketCount() == 2);
  REQUIRE(f2.GetSocketCount() == 1);
}

TEST_CASE("linear_chain_5 joints each have two sockets",
          "[unit][part_graphs][part_graph_library]") {
  const auto &j0 =
      std::get<steamrot::JointInstance>(tests::linear_chain_5.part_graph.at(1));
  const auto &j1 =
      std::get<steamrot::JointInstance>(tests::linear_chain_5.part_graph.at(3));
  REQUIRE(j0.GetSocketCount() == 2);
  REQUIRE(j1.GetSocketCount() == 2);
}

TEST_CASE("linear_chain_5 connection counts are correct",
          "[unit][part_graphs][part_graph_library]") {
  const auto &f0 = std::get<steamrot::FragmentInstance>(
      tests::linear_chain_5.part_graph.at(0));
  const auto &j0 =
      std::get<steamrot::JointInstance>(tests::linear_chain_5.part_graph.at(1));
  const auto &f1 = std::get<steamrot::FragmentInstance>(
      tests::linear_chain_5.part_graph.at(2));
  const auto &j1 =
      std::get<steamrot::JointInstance>(tests::linear_chain_5.part_graph.at(3));
  const auto &f2 = std::get<steamrot::FragmentInstance>(
      tests::linear_chain_5.part_graph.at(4));

  REQUIRE(f0.GetNumberOfConnectedSockets() == 1);
  REQUIRE(j0.GetNumberOfConnectedSockets() == 2);
  REQUIRE(f1.GetNumberOfConnectedSockets() == 2);
  REQUIRE(j1.GetNumberOfConnectedSockets() == 2);
  REQUIRE(f2.GetNumberOfConnectedSockets() == 1);
}

TEST_CASE("linear_chain_5 socket connections are bidirectionally correct",
          "[unit][part_graphs][part_graph_library]") {
  const auto &f0 = std::get<steamrot::FragmentInstance>(
      tests::linear_chain_5.part_graph.at(0));
  const auto &j0 =
      std::get<steamrot::JointInstance>(tests::linear_chain_5.part_graph.at(1));
  const auto &f1 = std::get<steamrot::FragmentInstance>(
      tests::linear_chain_5.part_graph.at(2));
  const auto &j1 =
      std::get<steamrot::JointInstance>(tests::linear_chain_5.part_graph.at(3));
  const auto &f2 = std::get<steamrot::FragmentInstance>(
      tests::linear_chain_5.part_graph.at(4));

  const auto *f0s0 = f0.TryGetSocket(0);
  const auto *j0s0 = j0.TryGetSocket(0);
  REQUIRE(f0s0 != nullptr);
  REQUIRE(j0s0 != nullptr);
  REQUIRE(f0s0->GetConnection().has_value());
  REQUIRE(j0s0->GetConnection().has_value());
  REQUIRE(f0s0->GetConnection()->peer_part_id == 1);
  REQUIRE(f0s0->GetConnection()->peer_socket_id == 0);
  REQUIRE(j0s0->GetConnection()->peer_part_id == 0);
  REQUIRE(j0s0->GetConnection()->peer_socket_id == 0);

  const auto *j0s1 = j0.TryGetSocket(1);
  const auto *f1s0 = f1.TryGetSocket(0);
  REQUIRE(j0s1 != nullptr);
  REQUIRE(f1s0 != nullptr);
  REQUIRE(j0s1->GetConnection().has_value());
  REQUIRE(f1s0->GetConnection().has_value());
  REQUIRE(j0s1->GetConnection()->peer_part_id == 2);
  REQUIRE(j0s1->GetConnection()->peer_socket_id == 0);
  REQUIRE(f1s0->GetConnection()->peer_part_id == 1);
  REQUIRE(f1s0->GetConnection()->peer_socket_id == 1);

  const auto *f1s1 = f1.TryGetSocket(1);
  const auto *j1s0 = j1.TryGetSocket(0);
  REQUIRE(f1s1 != nullptr);
  REQUIRE(j1s0 != nullptr);
  REQUIRE(f1s1->GetConnection().has_value());
  REQUIRE(j1s0->GetConnection().has_value());
  REQUIRE(f1s1->GetConnection()->peer_part_id == 3);
  REQUIRE(f1s1->GetConnection()->peer_socket_id == 0);
  REQUIRE(j1s0->GetConnection()->peer_part_id == 2);
  REQUIRE(j1s0->GetConnection()->peer_socket_id == 1);

  const auto *j1s1 = j1.TryGetSocket(1);
  const auto *f2s0 = f2.TryGetSocket(0);
  REQUIRE(j1s1 != nullptr);
  REQUIRE(f2s0 != nullptr);
  REQUIRE(j1s1->GetConnection().has_value());
  REQUIRE(f2s0->GetConnection().has_value());
  REQUIRE(j1s1->GetConnection()->peer_part_id == 4);
  REQUIRE(j1s1->GetConnection()->peer_socket_id == 0);
  REQUIRE(f2s0->GetConnection()->peer_part_id == 3);
  REQUIRE(f2s0->GetConnection()->peer_socket_id == 1);
}
} // namespace steamrot::tests
