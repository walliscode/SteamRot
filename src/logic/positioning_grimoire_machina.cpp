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
void maximise_joint_socket_spread(JointInstance &instance) {
  if (!instance.joint)
    return;

  // pull out the socket config from the joint instance
  const SocketConfig &config = instance.joint->socket_config;
  const float arc_min = config.rotation_arc_min;
  const float arc_max = config.rotation_arc_max;
  const float arc_range = arc_max - arc_min;

  // if socket count is 0, return early
  if (config.socket_count == 0) {
    return;
  }

  // if socket count is 1, place it at the midpoint of the arc
  if (config.socket_count == 1) {
    const float angle_deg = arc_min + arc_range / 2.f;
    const float angle_rad = sf::degrees(angle_deg).asRadians();
    instance.sockets.at(0).local_position =
        instance.joint->origin +
        sf::Vector2f{std::cos(angle_rad), std::sin(angle_rad)} * config.radius;

    // for greater that one socket, place them on the arc min/max range and then
    // evenly spread them out across the arc

  } else {
    const float angle_between_sockets = arc_range / (config.socket_count - 1);
    for (size_t i = 0; i < config.socket_count; ++i) {
      const float angle_deg = arc_min + (i * angle_between_sockets);
      const float angle_rad = sf::degrees(angle_deg).asRadians();
      instance.sockets.at(i).local_position =
          instance.joint->origin +
          sf::Vector2f{std::cos(angle_rad), std::sin(angle_rad)} *
              config.radius;
    }
  }
}

/////////////////////////////////////////////////
sf::Angle rotation_of_vector_to_target_vector(const sf::Vector2f &source,
                                              const sf::Vector2f &target) {

  // if the source or target vectors are zero, return early
  if (source == sf::Vector2f{0.f, 0.f} || target == sf::Vector2f{0.f, 0.f}) {
    return sf::Angle::Zero;
  }
  // normalize the source and target vectors (they may already be normalized,
  // but this is a safeguard)
  sf::Vector2f source_norm = source.normalized();
  sf::Vector2f target_norm = target.normalized();

  // calculate the angle between the source and target vectors using the dot
  // product and cross product The angle can be calculated using the atan2
  // function, which takes the cross product and dot product of the two vectors
  // as arguments. The cross product gives the sine of the angle, while the dot
  // product gives the cosine of the angle. The atan2 function returns the angle
  // in radians, which can then be used to rotate the transform.

  float angle =
      std::atan2(source_norm.cross(target_norm), source_norm.dot(target_norm));

  return sf::radians(angle);
}

/////////////////////////////////////////////////
void align_fragment_onto_joint_socket(FragmentInstance &fragment_instance,
                                      const uint32_t frament_socket_id,
                                      const JointInstance &joint_instance,
                                      const uint32_t joint_socket_id) {

  // check that the fragment socket id is valid
  auto fragment_socket_it = fragment_instance.sockets.find(frament_socket_id);
  if (fragment_socket_it == fragment_instance.sockets.end())
    return;
  FragmentSocketState &fragment_socket = fragment_socket_it->second;

  // check that the joint socket id is valid
  auto joint_socket_it = joint_instance.sockets.find(joint_socket_id);
  if (joint_socket_it == joint_instance.sockets.end())
    return;
  const JointSocketState &joint_socket = joint_socket_it->second;

  // check that the fragment socket is connected to the joint socket
  if (!fragment_socket_it->second.connected_to.has_value() ||
      !joint_socket_it->second.connected_to.has_value())
    return;
  if (fragment_socket_it->second.connected_to->peer_part_id !=
      joint_instance.id) {
    return;
  }
  if (joint_socket_it->second.connected_to->peer_part_id !=
      fragment_instance.id) {
    return;
  }

  // cache world position of the sockets
  sf::Vector2f fragment_socket_world_position =
      fragment_instance.transform.transformPoint(
          fragment_socket.local_position);

  sf::Vector2f joint_socket_world_position =
      joint_instance.transform.transformPoint(joint_socket.local_position);

  // calculate the rotation
  sf::Vector2f fragment_socket_alignment_vector =
      fragment_socket.alignment_vector.normalized();
  sf::Vector2f joint_socket_alignment_vector =
      joint_socket.local_position - joint_instance.joint->origin;
  sf::Angle rotation_angle = rotation_of_vector_to_target_vector(
      fragment_socket_alignment_vector, joint_socket_alignment_vector);
  sf::Transform rotation_transform;
  rotation_transform.rotate(rotation_angle);

  // calculate the transform
  sf::Vector2f translation_vector =
      joint_socket_world_position -
      // apply the rotation to the fragment socket world position before
      // calculating the translation vector
      rotation_transform.transformPoint(fragment_socket_world_position);

  // BUILDING THE TRANSFORM //
  // transforms are applied in reverse order, so we build translate then rotate

  // reset the transform of the fragment instance to identity
  fragment_instance.transform = sf::Transform::Identity;

  // transform the fragment instance to the joint socket position
  fragment_instance.transform.translate(translation_vector);

  // rotate the fragment instance to align the fragment socket alignment vector
  // with the joint socket alignment vector
  fragment_instance.transform.rotate(rotation_angle);
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
