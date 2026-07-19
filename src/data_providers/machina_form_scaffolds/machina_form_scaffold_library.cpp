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
#include "MachinaFormScaffold.h"

namespace steamrot {

/////////////////////////////////////////////////
void create_grab_scaffold_one(GrimoireMachina &grimoire) {
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
  // anchor node
  JointInstance ji_0{&grimoire.m_all_joints.at("joint_one")};
  ji_0.id = 0;
  // connect to FragmentInstance 1, socket 0
  ji_0.sockets.at(0).connection_state = SocketConnectionState::Connected;
  ji_0.sockets.at(0).connected_to =
      SocketConnection{1, 0}; // connected to FragmentInstance 1, socket 0
  // connect to FragmentInstance 4, socket 0
  ji_0.sockets.at(1).connection_state = SocketConnectionState::Connected;
  ji_0.sockets.at(1).connected_to = SocketConnection{4, 0};

  // arm
  FragmentInstance fi_1{&grimoire.m_all_fragments.at("fragment_one")};
  fi_1.id = 1;
  // connect to JointInstance 0, socket 0
  fi_1.sockets.at(0).connection_state = SocketConnectionState::Connected;
  fi_1.sockets.at(0).connected_to = SocketConnection{0, 0};
  // connect to JointInstance 2, socket 0
  fi_1.sockets.at(1).connection_state = SocketConnectionState::Connected;
  fi_1.sockets.at(1).connected_to = SocketConnection{2, 0};

  JointInstance ji_2{&grimoire.m_all_joints.at("joint_one")};
  ji_2.id = 2;
  // connect to FragmentInstance 1, socket 1
  ji_2.sockets.at(0).connection_state = SocketConnectionState::Connected;
  ji_2.sockets.at(0).connected_to = SocketConnection{1, 1};
  // connect to FragmentInstance 3, socket 0
  ji_2.sockets.at(1).connection_state = SocketConnectionState::Connected;
  ji_2.sockets.at(1).connected_to = SocketConnection{3, 0};
  FragmentInstance fi_3{&grimoire.m_all_fragments.at("fragment_one")};
  fi_3.id = 3;
  // connect to JointInstance 2, socket 0
  fi_3.sockets.at(0).connection_state = SocketConnectionState::Connected;
  fi_3.sockets.at(0).connected_to = SocketConnection{2, 0};
  // end socket 1 is available for connection
  fi_3.sockets.at(1).connection_state = SocketConnectionState::Available;

  // arm
  FragmentInstance fi_4{&grimoire.m_all_fragments.at("fragment_one")};
  fi_4.id = 4;
  // connect to JointInstance 0, socket 1
  fi_4.sockets.at(0).connection_state = SocketConnectionState::Connected;
  fi_4.sockets.at(0).connected_to = SocketConnection{0, 1};
  // connect to JointInstance 5, socket 0
  fi_4.sockets.at(1).connection_state = SocketConnectionState::Connected;
  fi_4.sockets.at(1).connected_to = SocketConnection{5, 0};
  JointInstance ji_5{&grimoire.m_all_joints.at("joint_one")};
  ji_5.id = 5;
  // connect to FragmentInstance 4, socket 1
  ji_5.sockets.at(0).connection_state = SocketConnectionState::Connected;
  ji_5.sockets.at(0).connected_to = SocketConnection{4, 1};
  // connect to FragmentInstance 6, socket 0
  ji_5.sockets.at(1).connection_state = SocketConnectionState::Connected;
  ji_5.sockets.at(1).connected_to = SocketConnection{6, 0};

  FragmentInstance fi_6{&grimoire.m_all_fragments.at("fragment_one")};
  fi_6.id = 6;
  // connect to JointInstance 5, socket 1
  fi_6.sockets.at(0).connection_state = SocketConnectionState::Connected;
  fi_6.sockets.at(0).connected_to = SocketConnection{5, 1};
  // end socket 1 is available for connection
  fi_6.sockets.at(1).connection_state = SocketConnectionState::Available;

  // assign to part graph with stable IDs
  part_graph.emplace(ji_0.id, ji_0);
  part_graph.emplace(fi_1.id, fi_1);
  part_graph.emplace(ji_2.id, ji_2);
  part_graph.emplace(fi_3.id, fi_3);
  part_graph.emplace(fi_4.id, fi_4);
  part_graph.emplace(ji_5.id, ji_5);
  part_graph.emplace(fi_6.id, fi_6);

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
}
} // namespace steamrot
