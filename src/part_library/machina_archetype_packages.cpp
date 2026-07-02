/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for running MachinaArchetype
/// evaluations on a PartGraph.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "machina_archetype_packages.h"
#include "fragment_library.h"
#include "joint_library.h"

namespace steamrot::tests {

/////////////////////////////////////////////////

PartGraphPackage create_valid_grab_pkg() {
  PartGraphPackage pkg =
      PartGraphBuilder{}
          .AddFragmentInstance(parts::FragmentRectangleWith2Sockets, "f0")
          .AddJointInstance(parts::JointSquareWith2Sockets, "j1")
          .AddFragmentInstance(parts::FragmentRectangleWith2Sockets, "f2")
          .AddJointInstance(parts::JointSquareWith2Sockets, "j3")
          .AddFragmentInstance(parts::FragmentRectangleWith2Sockets, "f4")
          .AddJointInstance(parts::JointSquareWith2Sockets, "j5")
          .AddFragmentInstance(parts::FragmentRectangleWith2Sockets, "f6")
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
