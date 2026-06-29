/////////////////////////////////////////////////
/// @file
/// @brief Implementation of objects for the part graph library
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "part_graph_library.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
const PartGraphPackage pair =
    PartGraphBuilder()
        .AddFragmentInstance(FragmentNames::OneSocket, "f0")
        .AddJointInstance(JointNames::OneSocket, "j0")
        .Connect("f0", 0, "j0", 0)
        .Build();

/////////////////////////////////////////////////
const PartGraphPackage linear_chain_3 =
    PartGraphBuilder()
        .AddFragmentInstance(FragmentNames::OneSocket, "f0")
        .AddJointInstance(JointNames::TwoSockets, "j0")
        .AddFragmentInstance(FragmentNames::OneSocket, "f1")
        .Connect("f0", 0, "j0", 0)
        .Connect("j0", 1, "f1", 0)
        .Build();

/////////////////////////////////////////////////
const PartGraphPackage linear_chain_5 =
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

/////////////////////////////////////////////////
const PartGraphPackage ring =
    PartGraphBuilder()
        .AddJointInstance(JointNames::TwoSockets, "j0")
        .AddJointInstance(JointNames::TwoSockets, "j1")
        .AddJointInstance(JointNames::TwoSockets, "j2")
        .ConnectUnchecked("j0", 0, "j1", 0)
        .ConnectUnchecked("j1", 1, "j2", 0)
        .ConnectUnchecked("j2", 1, "j0", 1)
        .Build();

} // namespace steamrot::tests
