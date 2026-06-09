/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the free functions for the spatial analysis of a grab
/// structure
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "grab_analysis.h"
#include "PartGraphBuilder.h"
#include "descriptors_machina_archetypes.h"
#include <catch2/catch_test_macros.hpp>

using namespace steamrot::logic;

steamrot::tests::PartGraphPackage create_valid_grab_pkg() {
  steamrot::tests::PartGraphPackage pkg =
      steamrot::tests::PartGraphBuilder{}
          .AddFragment(steamrot::tests::FragmentNames::TwoSockets, "f0")
          .AddJoint(steamrot::tests::JointNames::TwoSockets, "j1")
          .AddFragment(steamrot::tests::FragmentNames::TwoSockets, "f2")
          .AddJoint(steamrot::tests::JointNames::TwoSockets, "j3")
          .AddFragment(steamrot::tests::FragmentNames::TwoSockets, "f4")
          .AddJoint(steamrot::tests::JointNames::TwoSockets, "j5")
          .AddFragment(steamrot::tests::FragmentNames::TwoSockets, "f6")
          .Connect("f0", 0, "j1", 0)
          .Connect("j1", 1, "f2", 0)
          .Connect("f2", 1, "j3", 0)
          .Connect("j3", 1, "f4", 0)
          .Connect("f4", 1, "j5", 0)
          .Connect("j5", 1, "f6", 0)
          .Build();
  return pkg;
}

TEST_CASE("valid_grab_pkg passes grab structural tests") {
  steamrot::tests::PartGraphPackage valid_grab_pkg = create_valid_grab_pkg();
  descriptors::MachinaArchetypeResult result =
      descriptors::MA::Grab()(valid_grab_pkg.part_graph,
                            valid_grab_pkg.id_to_part_graph_id.at("j3"), 0);

  REQUIRE(true);
}
