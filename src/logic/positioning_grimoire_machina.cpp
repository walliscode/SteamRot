/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for positioning grimoire machina
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_grimoire_machina.h"
#include "FailInfo.h"
#include "JointInstance.h"
#include "MachinaFormScaffold.h"
#include "PartInstance.h"
#include "part_instance_ops.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>

#include <expected>
#include <format>
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

std::expected<std::monostate, FailInfo>
position_from_node(PartGraph &part_graph, uint32_t part_id,
                   std::unordered_set<uint32_t> &visited,
                   std::unordered_set<uint32_t> &in_stack) {
  // if the part graph is empty, return early
  if (part_graph.empty())
    return std::monostate{};

  // if the part has already been visited, return early
  if (visited.contains(part_id))
    return std::monostate{};

  // cycles are currently not allowed or not handled, so if a cycle is detected,
  // return an error
  if (in_stack.contains(part_id)) {
    return std::unexpected(
        FailInfo{FailMode::BadValue,
                 std::format("cycle detected at part_id {}", part_id)});
  }

  // if the part_id does not exist in the part graph, return an error
  const auto parent_it = part_graph.find(part_id);
  if (parent_it == part_graph.end()) {
    return std::unexpected(FailInfo{
        FailMode::BadValue,
        std::format("part_id {} does not exist in part graph", part_id)});
  }

  // mark the part as visited and add it to the in_stack set
  visited.insert(part_id);
  in_stack.insert(part_id);

  // use a guard to ensure that the part is removed from the in_stack set when
  // stack unwinds, even if an error occurs
  // neat little RAII trick
  struct StackGuard {
    std::unordered_set<uint32_t> &stack;
    uint32_t id;

    ~StackGuard() { stack.erase(id); }
  } guard{in_stack, part_id};

  // std::visit needed as we are passing a variant to the function
  return std::visit(
      [&](auto &parent_part) -> std::expected<std::monostate, FailInfo> {
        // iterate through each socket of the parent part
        for (const auto &[socket_id, socket_state] : parent_part.GetSockets()) {

          // if the socket is available, skip it
          if (socket_state.IsAvailable()) {
            continue;
          }

          // get the connection for the socket
          const SocketConnection &connection = *socket_state.GetConnection();

          // if the connected part has already been visited, skip it
          if (visited.contains(connection.peer_part_id)) {
            continue;
          }

          auto connected_it = part_graph.find(connection.peer_part_id);
          if (connected_it == part_graph.end()) {
            continue;
          }

          auto &child_part = connected_it->second;
          const uint32_t child_id = connected_it->first;

          // align the child part instance to the parent part
          auto align_result = std::visit(
              [&](auto &child_part) -> std::expected<std::monostate, FailInfo> {
                // the first instance passed through gets aligned to the second
                // instance, so we need to pass the parent part first and the
                // child part second
                return try_align_part_instances(child_part,
                                                connection.peer_socket_id,
                                                parent_part, socket_id);
              },
              child_part);

          // uniwind the stack and return the error if alignment fails
          if (!align_result.has_value()) {
            return align_result;
          }

          // recursively position the child part and its connected parts
          auto recurse_result =
              position_from_node(part_graph, child_id, visited, in_stack);

          // uniwind the stack and return the error if recursion fails
          if (!recurse_result.has_value())
            return recurse_result;
        }

        // return std::monostate to indicate success
        return std::monostate{};
      },
      parent_it->second);
}
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
position_part_graph_from_first_added(PartGraph &part_graph) {

  // set up variables to for calling position_from_node
  std::unordered_set<uint32_t> visited;
  std::unordered_set<uint32_t> in_stack;

  // the first part added should have an id of 0
  return position_from_node(part_graph, 0, visited, in_stack);
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
