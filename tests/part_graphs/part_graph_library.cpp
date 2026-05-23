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
const PartGraphPackage pair = PartGraphBuilder()
                                  .AddFragment(FragmentNames::OneSocket, "f0")
                                  .AddJoint(JointNames::OneSocket, "j0")
                                  .Connect("f0", 0, "j0", 0)
                                  .Build();

/////////////////////////////////////////////////
const PartGraphPackage linear_chain_3 =
    PartGraphBuilder()
        .AddFragment(FragmentNames::OneSocket, "f0")
        .AddJoint(JointNames::TwoSockets, "j0")
        .AddFragment(FragmentNames::OneSocket, "f1")
        .Connect("f0", 0, "j0", 0)
        .Connect("j0", 1, "f1", 0)
        .Build();

/////////////////////////////////////////////////
const PartGraphPackage linear_chain_5 =
    PartGraphBuilder()
        .AddFragment(FragmentNames::OneSocket, "f0")
        .AddJoint(JointNames::TwoSockets, "j0")
        .AddFragment(FragmentNames::TwoSockets, "f1")
        .AddJoint(JointNames::TwoSockets, "j1")
        .AddFragment(FragmentNames::OneSocket, "f2")
        .Connect("f0", 0, "j0", 0)
        .Connect("j0", 1, "f1", 0)
        .Connect("f1", 1, "j1", 0)
        .Connect("j1", 1, "f2", 0)
        .Build();

} // namespace steamrot::tests
