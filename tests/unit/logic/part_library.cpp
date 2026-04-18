/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the PartLibrary test helper.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "part_library.h"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

/////////////////////////////////////////////////
PartLibrary &PartLibrary::AddFragment(std::string name, Fragment fragment) {
  fragment.name = name; // copy before name is moved below
  m_fragments.insert_or_assign(std::move(name), std::move(fragment));
  return *this;
}

/////////////////////////////////////////////////
PartLibrary &PartLibrary::AddJoint(std::string name, Joint joint) {
  joint.name = name; // copy before name is moved below
  m_joints.insert_or_assign(std::move(name), std::move(joint));
  return *this;
}

/////////////////////////////////////////////////
Fragment &PartLibrary::GetFragment(const std::string &name) {
  auto it = m_fragments.find(name);
  REQUIRE(it != m_fragments.end());
  return it->second;
}

/////////////////////////////////////////////////
const Fragment &PartLibrary::GetFragment(const std::string &name) const {
  auto it = m_fragments.find(name);
  REQUIRE(it != m_fragments.end());
  return it->second;
}

/////////////////////////////////////////////////
Joint &PartLibrary::GetJoint(const std::string &name) {
  auto it = m_joints.find(name);
  REQUIRE(it != m_joints.end());
  return it->second;
}

/////////////////////////////////////////////////
const Joint &PartLibrary::GetJoint(const std::string &name) const {
  auto it = m_joints.find(name);
  REQUIRE(it != m_joints.end());
  return it->second;
}

/////////////////////////////////////////////////
std::vector<Fragment *>
PartLibrary::GetFragments(const std::vector<std::string> &names) {
  std::vector<Fragment *> result;
  result.reserve(names.size());
  for (const auto &name : names) {
    result.push_back(&GetFragment(name));
  }
  return result;
}

/////////////////////////////////////////////////
std::vector<Joint *>
PartLibrary::GetJoints(const std::vector<std::string> &names) {
  std::vector<Joint *> result;
  result.reserve(names.size());
  for (const auto &name : names) {
    result.push_back(&GetJoint(name));
  }
  return result;
}

/////////////////////////////////////////////////
FragmentInstance
PartLibrary::MakeFragmentInstance(const std::string &name,
                                  sf::Transform initial_transform) {
  Fragment &fragment = GetFragment(name);
  FragmentInstance instance{&fragment, initial_transform};
  instance.id = m_next_id++;
  return instance;
}

/////////////////////////////////////////////////
JointInstance
PartLibrary::MakeJointInstance(const std::string &name,
                               sf::Transform initial_transform) {
  Joint &joint = GetJoint(name);
  JointInstance instance{&joint, initial_transform};
  instance.id = m_next_id++;
  return instance;
}

/////////////////////////////////////////////////
PartMap PartLibrary::MakePartMap(const std::vector<std::string> &fragment_names,
                                 const std::vector<std::string> &joint_names) {
  PartMap parts;

  for (const auto &name : fragment_names) {
    FragmentInstance instance = MakeFragmentInstance(name);
    parts.emplace(instance.id, std::move(instance));
  }

  for (const auto &name : joint_names) {
    JointInstance instance = MakeJointInstance(name);
    parts.emplace(instance.id, std::move(instance));
  }

  return parts;
}

/////////////////////////////////////////////////
std::vector<std::string> PartLibrary::FragmentNames() const {
  std::vector<std::string> names;
  names.reserve(m_fragments.size());
  for (const auto &[name, _] : m_fragments) {
    names.push_back(name);
  }
  return names;
}

/////////////////////////////////////////////////
std::vector<std::string> PartLibrary::JointNames() const {
  std::vector<std::string> names;
  names.reserve(m_joints.size());
  for (const auto &[name, _] : m_joints) {
    names.push_back(name);
  }
  return names;
}

} // namespace steamrot::tests
