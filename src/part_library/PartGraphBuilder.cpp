/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the PartGraphBuilder class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "PartGraphBuilder.h"
#include "action_grimoire_machina.h"
#include "catch2/catch_test_macros.hpp"
#include "part_library.h"
#include <expected>

namespace steamrot::tests {
/////////////////////////////////////////////////
FragmentInstance
PartGraphBuilder::MakeFragmentInstance(const FragmentNames name) {
  const Fragment *f;
  switch (name) {
  case FragmentNames::NoSocket:
    f = &fragment_no_socket;
    break;
  case FragmentNames::OneSocket:
    f = &fragment_one_socket;
    break;
  case FragmentNames::TwoSockets:
    f = &fragment_two_sockets;
    break;
  case FragmentNames::ThreeSockets:
    f = &fragment_three_sockets;
    break;
  }
  FragmentInstance instance{f};
  instance.id = m_package.next_id++;
  return instance;
}

/////////////////////////////////////////////////
JointInstance PartGraphBuilder::MakeJointInstance(const JointNames name) {
  const Joint *j;
  switch (name) {
  case JointNames::NoSocket:
    j = &joint_no_socket;
    break;
  case JointNames::OneSocket:
    j = &joint_one_socket;
    break;
  case JointNames::TwoSockets:
    j = &joint_two_sockets;
    break;
  case JointNames::ThreeSockets:
    j = &joint_three_sockets;
    break;
  }
  JointInstance instance{j};
  instance.id = m_package.next_id++;
  return instance;
}

/////////////////////////////////////////////////
PartGraphBuilder &
PartGraphBuilder::AddFragmentInstance(const FragmentNames name,
                                      const std::string id) {
  // create a new FragmentInstance (assigns stable ID), then insert
  FragmentInstance instance = MakeFragmentInstance(name);
  instance.alias = id;
  const uint32_t instance_id = instance.id;
  m_package.part_graph.emplace(instance_id, std::move(instance));

  // map the user-friendly string ID to the stable uint32_t ID in the part graph
  m_package.id_to_part_graph_id.emplace(id, instance_id);

  return *this;
}

/////////////////////////////////////////////////
PartGraphBuilder &
PartGraphBuilder::AddFragmentInstance(const Fragment &fragment,
                                      const std::string id) {
  FragmentInstance instance{&fragment};
  instance.id = m_package.next_id++;
  instance.alias = id;
  const uint32_t instance_id = instance.id;
  m_package.part_graph.emplace(instance_id, std::move(instance));

  // map the user-friendly string ID to the stable uint32_t ID in the part graph
  m_package.id_to_part_graph_id.emplace(id, instance_id);

  return *this;
}
/////////////////////////////////////////////////
PartGraphBuilder &PartGraphBuilder::AddJointInstance(const JointNames name,
                                                     const std::string id) {
  // create a new JointInstance (assigns stable ID), then insert
  JointInstance instance = MakeJointInstance(name);
  instance.alias = id;
  const uint32_t instance_id = instance.id;
  m_package.part_graph.emplace(instance_id, std::move(instance));

  // map the user-friendly string ID to the stable uint32_t ID in the part graph
  m_package.id_to_part_graph_id.emplace(id, instance_id);
  return *this;
}

/////////////////////////////////////////////////
PartGraphBuilder &PartGraphBuilder::AddJointInstance(const Joint &joint,
                                                     const std::string id) {
  JointInstance instance{&joint};
  instance.id = m_package.next_id++;
  instance.alias = id;
  const uint32_t instance_id = instance.id;
  m_package.part_graph.emplace(instance_id, std::move(instance));

  // map the user-friendly string ID to the stable uint32_t ID in the part graph
  m_package.id_to_part_graph_id.emplace(id, instance_id);

  return *this;
}

/////////////////////////////////////////////////
PartGraphBuilder &PartGraphBuilder::Connect(const std::string &from_id,
                                            const uint32_t from_socket_id,
                                            const std::string &to_id,
                                            const uint32_t to_socket_id) {
  // look up stable part IDs; fail immediately if either alias is unknown
  const auto from_it = m_package.id_to_part_graph_id.find(from_id);
  if (from_it == m_package.id_to_part_graph_id.end())
    FAIL("Connect: from_id '" << from_id << "' not found");

  const auto to_it = m_package.id_to_part_graph_id.find(to_id);
  if (to_it == m_package.id_to_part_graph_id.end())
    FAIL("Connect: to_id '" << to_id << "' not found");

  const uint32_t from_part_id = from_it->second;
  const uint32_t to_part_id = to_it->second;

  auto &from_variant = m_package.part_graph.at(from_part_id);
  auto &to_variant = m_package.part_graph.at(to_part_id);

  const bool from_is_fragment =
      std::holds_alternative<FragmentInstance>(from_variant);
  const bool to_is_fragment =
      std::holds_alternative<FragmentInstance>(to_variant);

  // only Fragment↔Joint connections are allowed — reject same-type pairs
  if (from_is_fragment == to_is_fragment)
    FAIL("Connect: only Fragment\xE2\x86\x94Joint connections are allowed ("
         << from_id << " and " << to_id << " are the same type)");

  // delegate to create_connection (always takes fragment first, then joint)
  std::expected<std::monostate, std::string> result;
  if (from_is_fragment) {
    result = steamrot::logic::action::grimoire_machina::create_connection(
        std::get<FragmentInstance>(from_variant), from_socket_id,
        std::get<JointInstance>(to_variant), to_socket_id);
  } else {
    result = steamrot::logic::action::grimoire_machina::create_connection(
        std::get<FragmentInstance>(to_variant), to_socket_id,
        std::get<JointInstance>(from_variant), from_socket_id);
  }

  if (!result.has_value())
    FAIL(result.error());

  return *this;
}
/////////////////////////////////////////////////
PartGraphBuilder &PartGraphBuilder::ConnectUnchecked(
    const std::string &from_id, const uint32_t from_socket_id,
    const std::string &to_id, const uint32_t to_socket_id) {
  const auto from_it = m_package.id_to_part_graph_id.find(from_id);
  if (from_it == m_package.id_to_part_graph_id.end())
    FAIL("ConnectUnchecked: from_id '" << from_id << "' not found");

  const auto to_it = m_package.id_to_part_graph_id.find(to_id);
  if (to_it == m_package.id_to_part_graph_id.end())
    FAIL("ConnectUnchecked: to_id '" << to_id << "' not found");

  const uint32_t from_part_id = from_it->second;
  const uint32_t to_part_id = to_it->second;

  // directly wire both socket endpoints without type-checking
  auto set_connected = [this](uint32_t part_id, uint32_t socket_id,
                              uint32_t peer_part_id, uint32_t peer_socket_id) {
    std::visit(
        [part_id, socket_id, peer_part_id, peer_socket_id](auto &instance) {
          if (!instance.sockets.count(socket_id))
            FAIL("ConnectUnchecked: socket_id ("
                 << socket_id << ") not found for part " << part_id);
          instance.sockets.at(socket_id).state = SocketState::Connected;
          instance.sockets.at(socket_id).connected_to =
              SocketConnection{peer_part_id, peer_socket_id};
          ++instance.connection_count;
        },
        m_package.part_graph.at(part_id));
  };

  set_connected(from_part_id, from_socket_id, to_part_id, to_socket_id);
  set_connected(to_part_id, to_socket_id, from_part_id, from_socket_id);

  return *this;
}

/////////////////////////////////////////////////
PartGraphPackage PartGraphBuilder::Build() {
  // create copy of the package to return, then reset the builder's internal
  // state for potential reuse
  PartGraphPackage package_copy = m_package;
  m_package = PartGraphPackage{};
  return package_copy;
}
} // namespace steamrot::tests
