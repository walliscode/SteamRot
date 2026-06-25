/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for positioning grimoire machina
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_grimoire_machina.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstdlib>

namespace steamrot::logic::positioning::grimoire_machina {

/////////////////////////////////////////////////
void initialize_joint_socket_positions(JointInstance &instance) {
  if (!instance.joint)
    return;

  std::vector<sf::Vector2f> positions;
  compute_socket_local_positions_even_spread(instance.joint->socket_config,
                                             instance.joint->origin, positions);

  for (auto &[socket_id, socket] : instance.sockets) {
    socket.local_position = positions[socket_id];
  }
}

/////////////////////////////////////////////////
void compute_socket_local_positions_even_spread(
    const SocketConfig &config, const sf::Vector2f &origin,
    std::vector<sf::Vector2f> &local_positions) {

  // if no sockets, return early (also avoids divide by zero in later
  // calculations)
  if (config.socket_count == 0) {
    return;
  }
  // extra guard statement to prevent undefined access
  local_positions.clear();
  local_positions.resize(config.socket_count);

  // calculate the angle at which to place the socket(s) using the socket
  // config's arc min and mx and socket count
  const float arc_min = config.rotation_arc_min;
  const float arc_max = config.rotation_arc_max;
  const float arc_range = arc_max - arc_min;

  // for a single socket, place it at the midpoint of the arc
  if (config.socket_count == 1) {
    const float angle_deg = arc_min + arc_range / 2.f;
    const float angle_rad = sf::degrees(angle_deg).asRadians();
    local_positions[0] =
        origin +
        sf::Vector2f{std::cos(angle_rad), std::sin(angle_rad)} * config.radius;
  } else {
    const float angle_between_sockets = arc_range / (config.socket_count - 1);
    // for greater that one socket, place them on the arc min/max range, evenly
    // spaced
    for (size_t i = 0; i < config.socket_count; ++i) {
      const float angle_deg = arc_min + (i * angle_between_sockets);
      const float angle_rad = sf::degrees(angle_deg).asRadians();
      local_positions[i] =
          origin + sf::Vector2f{std::cos(angle_rad), std::sin(angle_rad)} *
                       config.radius;
    }
  }
  return;
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

  // get variant
  if (FragmentInstance *fragment_instance =
          std::get_if<FragmentInstance>(&it->second)) {

    // if views is empty, return early
    if (fragment_instance->fragment->positioning_views.empty())
      return;

    // working off the FRONT view only, get the center of the bounding box of
    // the FRONT view's vertex array
    const sf::VertexArray &va =
        fragment_instance->fragment->positioning_views[ViewDirection::Front];
    // get centre using built in sfml functions
    sf::Vector2f center = va.getBounds().getCenter();

    // reset the transform of the fragment instance to identity
    fragment_instance->transform = sf::Transform::Identity;
    // apply the offset to the transform of the fragment instance
    fragment_instance->transform.translate(-center);

    return;
  }

  if (JointInstance *joint_instance = std::get_if<JointInstance>(&it->second)) {
    // if views is empty, return early
    if (joint_instance->joint->positioning_views.empty())
      return;

    // reset the transform of the joint instance to identity
    joint_instance->transform = sf::Transform::Identity;
    // we work of the origin of the joint for positioning
    // most Joints are likely to be set at 0,0 when creating, but we should
    // still account for the possibility of an offset
    joint_instance->transform.translate(-joint_instance->joint->origin);

    // populate socket positions now that the joint is placed
    initialize_joint_socket_positions(*joint_instance);

    return;
  }
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
    sf::FloatRect box =
        fragment_instance->fragment->positioning_views[ViewDirection::Front]
            .getBounds();
    // apply the local transform of the fragment instance to the bounding box
    return fragment_instance->transform.transformRect(box);

  } else if (const JointInstance *joint_instance =
                 std::get_if<JointInstance>(&part_variant)) {
    // get the bounding box of the FRONT view of the joint
    sf::FloatRect box =
        joint_instance->joint->positioning_views[ViewDirection::Front]
            .getBounds();
    // apply the local transform of the joint instance to the bounding box
    return joint_instance->transform.transformRect(box);
  }
  // if part_variant is neither a FragmentInstance nor a JointInstance, return
  // an empty bounding box
  return sf::FloatRect{};
}
/////////////////////////////////////////////////
sf::FloatRect calculate_outer_box(const PartGraph &part_graph,
                                  const SubGraph &sub_graph) {
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
    for (const auto &[id, part] : part_graph) {

      add_part_to_outer_box(part);
    }

  } else {
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
