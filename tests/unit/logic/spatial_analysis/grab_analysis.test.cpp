/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the free functions for the spatial analysis of a grab
/// structure
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "grab_analysis.h"
#include "MachinaFormScaffold.h"
#include "PartGraphBuilder.h"
#include "descriptors_machina_archetypes.h"
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>
#include <variant>

namespace steamrot::tests {
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
  MachinaArchetypeResult result =
      descriptors::MA::Grab()(valid_grab_pkg.part_graph,
                              valid_grab_pkg.id_to_part_graph_id.at("j3"), 0);

  REQUIRE(result);
}

TEST_CASE("align_grab_structure tests") {
  // arrange
  // set up valid grab package and result
  steamrot::tests::PartGraphPackage valid_grab_pkg = create_valid_grab_pkg();
  steamrot::MachinaArchetypeResult ma_result =
      descriptors::MA::Grab()(valid_grab_pkg.part_graph,
                              valid_grab_pkg.id_to_part_graph_id.at("j3"), 0);

  // pull ou the GrabResult from the MachinaArchetypeResult variant
  REQUIRE(std::holds_alternative<steamrot::GrabResult>(
      ma_result.result_sub_graphs));
  steamrot::GrabResult grab_result =
      std::get<steamrot::GrabResult>(ma_result.result_sub_graphs);
  steamrot::PartGraph &graph = valid_grab_pkg.part_graph;

  // get the instance of the anchor joint
  const uint32_t anchor_id = grab_result.anchor;
  REQUIRE(valid_grab_pkg.id_to_part_graph_id.at("j3") == anchor_id);
  REQUIRE(std::holds_alternative<steamrot::JointInstance>(graph.at(anchor_id)));
  steamrot::JointInstance &anchor_instance =
      std::get<steamrot::JointInstance>(graph.at(anchor_id));

  SECTION("align_grab_structure sets the anchor's transform to the provided "
          "vector") {
    // arrange
    sf::Transform &anchor_transform = anchor_instance.transform;
    // add an arbitrary transform to the anchor joint instance to make sure the
    // function is actually changing it
    anchor_transform.translate({100.f, 50.f});
    REQUIRE(anchor_transform.transformPoint({0.f, 0.f}) ==
            sf::Vector2f{100.f, 50.f});

    // act
    std::vector<sf::Vector2f> target_positions{{{0.f, 0.f}, {0.f, 100.f}}};

    for (const sf::Vector2f &target_position : target_positions) {
      steamrot::logic::spatial_analysis::align_grab_structure(grab_result,
                                                              target_position);
      // assert
      REQUIRE(anchor_transform.transformPoint({0.f, 0.f}) == target_position);
    }
  }
}
} // namespace steamrot::tests
