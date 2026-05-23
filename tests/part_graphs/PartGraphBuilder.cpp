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

  // create a new FragmentInstance and add it to the part graph with the next
  // available ID
  FragmentInstance instance = MakeFragmentInstance(name);
  const uint32_t instance_id = m_package.next_id;
  m_package.part_graph.emplace(instance_id, instance);
  m_package.next_id++;

  // map the user-friendly string ID to the stable uint32_t ID in the part graph
  m_package.id_to_part_graph_id.emplace(id, instance_id);

  return *this;
}

/////////////////////////////////////////////////
PartGraphBuilder &PartGraphBuilder::AddJoint(const JointNames name,
                                             const std::string id) {
  // create a new JointInstance and add it to the part graph with the next
  // available ID
  JointInstance instance = MakeJointInstance(name);
  const uint32_t instance_id = m_package.next_id;
  m_package.part_graph.emplace(instance_id, instance);
  m_package.next_id++;

  // map the user-friendly string ID to the stable uint32_t ID in the part graph
  m_package.id_to_part_graph_id.emplace(id, instance_id);
  return *this;
}
/////////////////////////////////////////////////
PartGraphBuilder &PartGraphBuilder::Connect(const std::string &from_id,
                                            const uint32_t from_socket_id,
                                            const std::string &to_id,
                                            const uint32_t to_socket_id) {
  // look up the stable uint32_t IDs for the from and to parts using the string
  // IDs provided by the user
  // if either ID is not found, throw an exception
  const auto from_it = m_package.id_to_part_graph_id.find(from_id);
  if (from_it == m_package.id_to_part_graph_id.end()) {
    FAIL("from_id '" << from_id << "' not found in id_to_part_graph_id");
  }
  const auto to_it = m_package.id_to_part_graph_id.find(to_id);
  if (to_it == m_package.id_to_part_graph_id.end()) {
    FAIL("to_id '" << to_id << "' not found in id_to_part_graph_id");
  }
  const uint32_t from_part_id = from_it->second;
  const uint32_t to_part_id = to_it->second;

  // use the create connection function that already exists in the logic code to
  // create the connection between the two parts
  auto result = steamrot::logic::action::grimoire_machina::create_connection(
      std::get<FragmentInstance>(m_package.part_graph.at(from_part_id)),
      from_socket_id,
      std::get<JointInstance>(m_package.part_graph.at(to_part_id)),
      to_socket_id);
  // return *this to allow chaining of Connect calls
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
