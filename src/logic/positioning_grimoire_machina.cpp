/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for positioning grimoire machina
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_grimoire_machina.h"
#include "JointInstance.h"
#include "MachinaFormScaffold.h"
#include "PartInstance.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>

#include <variant>

namespace steamrot::logic::positioning::grimoire_machina {

/////////////////////////////////////////////////
sf::Angle rotation_of_vector_to_target_vector(const sf::Vector2f &source,
                                              const sf::Vector2f &target) {

  // if the source or target vectors are zero, return early
  if (source == sf::Vector2f{0.f, 0.f} || target == sf::Vector2f{0.f, 0.f}) {
    return sf::Angle::Zero;
  }

  // normalize the source and target vectors
  sf::Vector2f source_norm = source.normalized();
  sf::Vector2f target_norm = target.normalized();

  // calculate cross and dot for atan2
  float cross_val = source_norm.cross(target_norm);
  float dot_val = source_norm.dot(target_norm);

  // angle in radians from source -> target
  float angle = std::atan2(cross_val, dot_val);
  sf::Angle result = sf::radians(angle);

  return result;
}

/////////////////////////////////////////////////
void position_first_part_of_machina_form_scaffold(PartGraph &parts) {
  // check that parts is not empty, if not return early
  if (parts.empty()) {
    return;
  }

  // if part 0 does not exist, return early
  auto it = parts.find(0);
  if (it == parts.end()) {
    return;
  }

  // this should be positioned at 0,0
  std::visit(
      [&](auto &part_instance) {
        // set the position of the part instance to 0,0 which should be the
        // center of the crafting scene
        part_instance.setPosition({0, 0});
      },
      it->second);
}

/////////////////////////////////////////////////
void position_machina_form_scaffold(PartGraph &parts) {
  // if parts is empty, return early
  if (parts.empty())
    return;

  // position the first part of the scaffold at 0,0
  position_first_part_of_machina_form_scaffold(parts);
}

/////////////////////////////////////////////////
void position_part_graph(PartGraph &part_graph) {

  // if (part_graph.empty()) {
  //   return;
  // }
  //
  // std::unordered_set<uint32_t> visited;
  // std::unordered_set<uint32_t> in_stack; // optional: cycle diagnostics
  //
  // auto position_part_graph_recursive = [&](this auto &&self,
  //                                          const uint32_t part_id) -> void {
  //   // already done
  //   if (visited.contains(part_id)) {
  //     return;
  //   }
  //
  //   // part must exist
  //   auto current_it = part_graph.find(part_id);
  //   if (current_it == part_graph.end()) {
  //     return;
  //   }
  //   const auto &current_part_variant = current_it->second;
  //
  //   // mark EARLY to prevent cycles/back-edges from re-processing this node
  //   visited.insert(part_id);
  //   in_stack.insert(part_id);
  //
  //   // define lambda for aligning two part instances togther
  //   auto align_parts = [](const PartInstanceVariant &parent_instance,
  //                         const uint32_t parent_socket_id,
  //                         PartInstanceVariant &child_instance,
  //                         const uint32_t child_socket_id) {
  //
  //   };
  //
  //   std::visit(
  //       [&](auto &current_part_instance) {
  //         for (const auto &[current_socket_id, current_socket_state] :
  //              current_part_instance.GetSockets()) {
  //
  //           if (current_socket_state.GetConnectionState() !=
  //                   SocketConnectionState::Connected ||
  //               !current_socket_state.GetConnection().has_value()) {
  //             continue;
  //           }
  //
  //           const SocketConnection &connection =
  //               *current_socket_state.GetConnection();
  //
  //           // early skip if peer already visited
  //           if (visited.contains(connection.peer_part_id)) {
  //             continue;
  //           }
  //
  //           auto connected_it = part_graph.find(connection.peer_part_id);
  //           if (connected_it == part_graph.end()) {
  //             continue;
  //           }
  //
  //           auto &connected_part_variant = connected_it->second;
  //
  //           std::visit(
  //               [&](auto &connected_part_instance) {
  //                 // align the connected part instance to the current part
  //                 auto result = try_align_part_instances(
  //                     current_part_instance, current_socket_id,
  //                     connected_part_instance, connection.peer_socket_id);
  //                 // recursively position the connected part
  //               },
  //               connected_part_variant);
  //         }
  //       },
  //       current_it->second);
  //
  //   in_stack.erase(part_id);
  //   std::cout << "[position_part_graph_recursive] exit part_id=" << part_id
  //             << " visited_count=" << visited.size() << "\n";
  // };
  //
  // if (part_graph.find(0) != part_graph.end()) {
  //   std::cout << "[start] root part_id=0\n";
  //   position_part_graph_recursive(0);
  // } else {
  //   std::cout << "[warn] part_id 0 not found in part graph\n";
  // }
  //
  // std::cout << "[position_part_graph] end\n";
}
/////////////////////////////////////////////////
void calculate_composite_box(sf::FloatRect &composite_box,
                             const sf::FloatRect &next_box) {
  auto left = std::min(composite_box.position.x, next_box.position.x);
  auto top = std::min(composite_box.position.y, next_box.position.y);

  auto right = std::max(composite_box.position.x + composite_box.size.x,
                        next_box.position.x + next_box.size.x);

  auto bottom = std::max(composite_box.position.y + composite_box.size.y,
                         next_box.position.y + next_box.size.y);

  composite_box = {{left, top}, {right - left, bottom - top}};
}

/////////////////////////////////////////////////
sf::FloatRect
get_transformed_bounding_box(const PartInstanceVariant &part_variant) {
  if (const FragmentInstance *fragment_instance =
          std::get_if<FragmentInstance>(&part_variant)) {
    // get the bounding box of the FRONT view of the fragment
    sf::FloatRect box = fragment_instance->GetPart()
                            .positioning_views[ViewDirection::Front]
                            .getBounds();
    // apply the local transform of the fragment instance to the bounding box
    return fragment_instance->getTransform().transformRect(box);

  } else if (const JointInstance *joint_instance =
                 std::get_if<JointInstance>(&part_variant)) {
    // get the bounding box of the FRONT view of the joint
    sf::FloatRect box = joint_instance->GetPart()
                            .positioning_views[ViewDirection::Front]
                            .getBounds();

    // apply the local transform of the joint instance to the bounding box
    return joint_instance->getTransform().transformRect(box);
  }
  // if part_variant is neither a FragmentInstance nor a JointInstance, return
  // an empty bounding box
  return sf::FloatRect{};
}
/////////////////////////////////////////////////
sf::FloatRect calculate_outer_box(const PartGraph &part_graph,
                                  const SubGraph &sub_graph,
                                  const bool use_minimum_bounding_box) {
  // initiliase box with a minimum size to prevent tiny boxes from being
  // returned
  sf::FloatRect outer_box{{-100.f, -100.f}, {200.f, 200.f}};

  // if PartGraph is empty, return early
  if (part_graph.empty()) {

    return outer_box;
  }

  // helper lambda for adding parts
  auto add_part_to_outer_box = [&outer_box](const PartInstanceVariant &part) {
    sf::FloatRect part_box = get_transformed_bounding_box(part);
    calculate_composite_box(outer_box, part_box);
  };

  // if subgraph is empty, calculate outer box for all parts in the part graph
  if (sub_graph.empty()) {

    // if use_minimum_bounding_box is false, set the outer box to the bounding
    // box of the first part
    if (!use_minimum_bounding_box)
      outer_box = get_transformed_bounding_box(part_graph.begin()->second);

    for (const auto &[id, part] : part_graph) {

      add_part_to_outer_box(part);
    }
  } else {
    // if a SubGraph is provided, the minimum bounding box will be based on
    // the first part
    outer_box = get_transformed_bounding_box(part_graph.at(*sub_graph.begin()));

    // calculate outer box for only the parts in the subgraph
    for (const auto &id : sub_graph) {
      auto it = part_graph.find(id);
      if (it != part_graph.end()) {

        add_part_to_outer_box(it->second);
      }
    }
  }
  return outer_box;
}
} // namespace steamrot::logic::positioning::grimoire_machina
