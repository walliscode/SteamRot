/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for providing custom
/// MachinaFormScaffold objects
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "machina_form_scaffold_library.h"
#include "DescriptorResult.h"
#include "FailInfo.h"
#include "MachinaFormScaffold.h"
#include <expected>
#include <variant>

namespace steamrot {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
create_grab_scaffold_one(GrimoireMachina &grimoire) {
  // clear the GrimoireMachina's existing data
  grimoire.m_all_fragments.clear();
  grimoire.m_all_joints.clear();
  grimoire.m_machina_forms.clear();
  grimoire.m_scaffold_form.reset();

  // populate the GrimoireMachina with Fragment(s)
  Fragment fragment_one;
  fragment_one.name = "fragment_one";
  sf::VertexArray &fragment_one_view =
      fragment_one.positioning_views[ViewDirection::Front];
  fragment_one_view.setPrimitiveType(sf::PrimitiveType::Triangles);
  fragment_one_view.append(sf::Vertex(sf::Vector2f(0.f, 0.f)));
  fragment_one_view.append(sf::Vertex(sf::Vector2f(50.f, 0.f)));
  fragment_one_view.append(sf::Vertex(sf::Vector2f(50.f, 10.f)));
  fragment_one_view.append(sf::Vertex(sf::Vector2f(0.f, 0.f)));
  fragment_one_view.append(sf::Vertex(sf::Vector2f(50.f, 10.f)));
  fragment_one_view.append(sf::Vertex(sf::Vector2f(0.f, 10.f)));
  fragment_one.sockets.emplace_back(sf::Vector2f(0.f, 5.f),
                                    sf::Vector2f(1.f, 0.f));
  fragment_one.sockets.emplace_back(sf::Vector2f(50.f, 5.f),
                                    sf::Vector2f(-1.f, 0.f));
  grimoire.m_all_fragments.emplace(fragment_one.name, fragment_one);

  // populate the GrimoireMachina with Joint(s)
  Joint joint_one;
  joint_one.name = "joint_one";
  sf::VertexArray &joint_one_view =
      joint_one.positioning_views[ViewDirection::Front];
  joint_one_view.setPrimitiveType(sf::PrimitiveType::Triangles);
  joint_one_view.append(sf::Vertex(sf::Vector2f(0.f, 0.f)));
  joint_one_view.append(sf::Vertex(sf::Vector2f(20.f, 0.f)));
  joint_one_view.append(sf::Vertex(sf::Vector2f(20.f, 20.f)));
  joint_one_view.append(sf::Vertex(sf::Vector2f(0.f, 0.f)));
  joint_one_view.append(sf::Vertex(sf::Vector2f(20.f, 20.f)));
  joint_one_view.append(sf::Vertex(sf::Vector2f(0.f, 20.f)));
  joint_one.socket_config.socket_count = 2;
  joint_one.socket_config.radius = 13.f;
  joint_one.socket_config.rotation_arc_min = 0.f;
  joint_one.socket_config.rotation_arc_max = 180.f;
  joint_one.socket_config.minimum_gap = 10.f;
  joint_one.socket_config.has_fixed_socket = false;
  joint_one.socket_config.fixed_socket_angle = 0.f;

  grimoire.m_all_joints.emplace(joint_one.name, joint_one);

  // create a PartGraph
  PartGraph part_graph;

  // create all instances first
  JointInstance ji_0{0, grimoire.m_all_joints.at("joint_one")};
  FragmentInstance fi_1{1, grimoire.m_all_fragments.at("fragment_one")};
  JointInstance ji_2{2, grimoire.m_all_joints.at("joint_one")};
  FragmentInstance fi_3{3, grimoire.m_all_fragments.at("fragment_one")};
  FragmentInstance fi_4{4, grimoire.m_all_fragments.at("fragment_one")};
  JointInstance ji_5{5, grimoire.m_all_joints.at("joint_one")};
  FragmentInstance fi_6{6, grimoire.m_all_fragments.at("fragment_one")};

  // anchor -> arm A root
  auto conn_ji_0_to_fi_1_result =
      ji_0.CreateConnectionWithOtherInstance(0, fi_1, 0);
  if (!conn_ji_0_to_fi_1_result)
    return std::unexpected(conn_ji_0_to_fi_1_result.error());

  // anchor -> arm B root
  auto conn_ji_0_to_fi_4_result =
      ji_0.CreateConnectionWithOtherInstance(1, fi_4, 0);
  if (!conn_ji_0_to_fi_4_result)
    return std::unexpected(conn_ji_0_to_fi_4_result.error());

  // arm A middle
  auto conn_fi_1_to_ji_2_result =
      fi_1.CreateConnectionWithOtherInstance(1, ji_2, 0);
  if (!conn_fi_1_to_ji_2_result)
    return std::unexpected(conn_fi_1_to_ji_2_result.error());

  // arm A end
  auto conn_ji_2_to_fi_3_result =
      ji_2.CreateConnectionWithOtherInstance(1, fi_3, 0);
  if (!conn_ji_2_to_fi_3_result)
    return std::unexpected(conn_ji_2_to_fi_3_result.error());

  // arm B middle
  auto conn_fi_4_to_ji_5_result =
      fi_4.CreateConnectionWithOtherInstance(1, ji_5, 0);
  if (!conn_fi_4_to_ji_5_result)
    return std::unexpected(conn_fi_4_to_ji_5_result.error());

  // arm B end
  auto conn_ji_5_to_fi_6_result =
      ji_5.CreateConnectionWithOtherInstance(1, fi_6, 0);
  if (!conn_ji_5_to_fi_6_result)
    return std::unexpected(conn_ji_5_to_fi_6_result.error());

  // assign to part graph with stable IDs
  part_graph.emplace(ji_0.GetId(), ji_0);
  part_graph.emplace(fi_1.GetId(), fi_1);
  part_graph.emplace(ji_2.GetId(), ji_2);
  part_graph.emplace(fi_3.GetId(), fi_3);
  part_graph.emplace(fi_4.GetId(), fi_4);
  part_graph.emplace(ji_5.GetId(), ji_5);
  part_graph.emplace(fi_6.GetId(), fi_6);

  auto scaffold = std::make_unique<MachinaFormScaffold>();
  scaffold->machina_form_name = "grab_scaffold_one";
  scaffold->next_id = 7; // next available stable ID after the last part
  scaffold->parts = std::move(part_graph);
  scaffold->are_sockets_visible = true;
  scaffold->structural_analysis_state = StructuralAnalysisState::Found;

  // create a dummy structural analysis result for the scaffold
  GrabResult grab_result;
  grab_result.anchor = 0;          // anchor node is JointInstance 0
  grab_result.arms.resize(2);      // two arms in the grab archetype
  grab_result.arms[0] = {1, 2, 3}; // first arm: FragmentInstance 1 ->
                                   // JointInstance 2 -> FragmentInstance 3
  grab_result.arms[1] = {4, 5, 6}; // second arm: FragmentInstance 4 ->
                                   // JointInstance 5 -> FragmentInstance 6

  // create a MachinaArchetypeResult for the grab archetype and add it to the
  // scaffold
  MachinaArchetypeResult archetype_result;
  archetype_result.result_sub_graphs = grab_result;
  scaffold->structural_analysis_results["Grab"].push_back(archetype_result);

  // Assign the populated scaffold to the GrimoireMachina
  grimoire.m_scaffold_form = std::move(scaffold);

  return std::monostate{};
}
} // namespace steamrot
