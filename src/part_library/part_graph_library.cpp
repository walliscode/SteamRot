/////////////////////////////////////////////////
/// @file
/// @brief Implementation of objects for the part graph library.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "part_graph_library.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
const PartGraphPackage &pair() {
  static const PartGraphPackage value =
      PartGraphBuilder()
          .AddFragmentInstance(FragmentNames::OneSocket, "f0")
          .AddJointInstance(JointNames::OneSocket, "j0")
          .Connect("f0", 0, "j0", 0)
          .Build();
  return value;
}

/////////////////////////////////////////////////
const PartGraphPackage &linear_chain_3() {
  static const PartGraphPackage value =
      PartGraphBuilder()
          .AddFragmentInstance(FragmentNames::OneSocket, "f0")
          .AddJointInstance(JointNames::TwoSockets, "j0")
          .AddFragmentInstance(FragmentNames::OneSocket, "f1")
          .Connect("f0", 0, "j0", 0)
          .Connect("j0", 1, "f1", 0)
          .Build();
  return value;
}

/////////////////////////////////////////////////
const PartGraphPackage &linear_chain_5() {
  static const PartGraphPackage value =
      PartGraphBuilder()
          .AddFragmentInstance(FragmentNames::OneSocket, "f0")
          .AddJointInstance(JointNames::TwoSockets, "j0")
          .AddFragmentInstance(FragmentNames::TwoSockets, "f1")
          .AddJointInstance(JointNames::TwoSockets, "j1")
          .AddFragmentInstance(FragmentNames::OneSocket, "f2")
          .Connect("f0", 0, "j0", 0)
          .Connect("j0", 1, "f1", 0)
          .Connect("f1", 1, "j1", 0)
          .Connect("j1", 1, "f2", 0)
          .Build();
  return value;
}

/////////////////////////////////////////////////
const PartGraphPackage &ring() {
  static const PartGraphPackage value =
      PartGraphBuilder()
          .AddJointInstance(JointNames::TwoSockets, "j0")
          .AddFragmentInstance(FragmentNames::TwoSockets, "f1")
          .AddJointInstance(JointNames::TwoSockets, "j2")
          .AddFragmentInstance(FragmentNames::TwoSockets, "f3")
          .Connect("j0", 0, "f1", 0)
          .Connect("f1", 1, "j2", 0)
          .Connect("j2", 1, "f3", 0)
          .Connect("f3", 1, "j0", 1)
          .Build();
  return value;
}

} // namespace steamrot::tests
