/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for running MachinaArchetype
/// evaluations on a PartGraph.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "machina_archetype_packages.h"

namespace steamrot::tests {

/////////////////////////////////////////////////

PartGraphPackage create_valid_grab_pkg() {
  PartGraphPackage pkg = PartGraphBuilder{}
                             .AddFragment(FragmentNames::TwoSockets, "f0")
                             .AddJoint(JointNames::TwoSockets, "j1")
                             .AddFragment(FragmentNames::TwoSockets, "f2")
                             .AddJoint(JointNames::TwoSockets, "j3")
                             .AddFragment(FragmentNames::TwoSockets, "f4")
                             .AddJoint(JointNames::TwoSockets, "j5")
                             .AddFragment(FragmentNames::TwoSockets, "f6")
                             .Connect("f0", 0, "j1", 0)
                             .Connect("j1", 1, "f2", 0)
                             .Connect("f2", 1, "j3", 0)
                             .Connect("j3", 1, "f4", 0)
                             .Connect("f4", 1, "j5", 0)
                             .Connect("j5", 1, "f6", 0)
                             .Build();
  return pkg;
}
} // namespace steamrot::tests
