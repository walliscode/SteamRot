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
  return FragmentInstance{f};
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
  return JointInstance{j};
}

/////////////////////////////////////////////////
PartGraphBuilder &PartGraphBuilder::AddFragment(const FragmentNames name,
                                                const std::string id) {
  // create a new FragmentInstance, assign its stable ID, then insert
  const uint32_t instance_id = m_package.next_id++;
  FragmentInstance instance = MakeFragmentInstance(name);
  instance.id = instance_id;
  m_package.part_graph.emplace(instance_id, std::move(instance));

  // map the user-friendly string ID to the stable uint32_t ID in the part graph
  m_package.id_to_part_graph_id.emplace(id, instance_id);

  return *this;
}

/////////////////////////////////////////////////
PartGraphBuilder &PartGraphBuilder::AddJoint(const JointNames name,
                                             const std::string id) {
  // create a new JointInstance, assign its stable ID, then insert
  const uint32_t instance_id = m_package.next_id++;
  JointInstance instance = MakeJointInstance(name);
  instance.id = instance_id;
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
PartGraphPackage PartGraphBuilder::Build() {
  // create copy of the package to return, then reset the builder's internal
  // state for potential reuse
  PartGraphPackage package_copy = m_package;
  m_package = PartGraphPackage{};
  return package_copy;
}
} // namespace steamrot::tests
