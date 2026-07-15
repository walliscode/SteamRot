/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for positioning grimoire machina
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_grimoire_machina.h"
#include "action_grimoire_machina.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>

namespace steamrot::logic::positioning::grimoire_machina {

/////////////////////////////////////////////////
sf::Vector2f
calculate_alignment_vector(const FragmentInstance &fragment_instance,
                           const uint32_t fragment_socket_id) {
  // check that the fragment socket id is valid
  auto socket_it = fragment_instance.sockets.find(fragment_socket_id);
  if (socket_it == fragment_instance.sockets.end())
    return sf::Vector2f{0.f, 0.f};

  // generate transform from the total rotation of the fragment instance
  sf::Transform rotation_transform;
  rotation_transform.rotate(fragment_instance.total_rotation);

  // apply the rotation transform to the fragment socket alignment vector
  sf::Vector2f alignment_vector =
      rotation_transform.transformPoint(socket_it->second.alignment_vector);

  // return the normalized alignment vector
  return alignment_vector.normalized();
}

/////////////////////////////////////////////////
sf::Vector2f calculate_alignment_vector(const JointInstance &joint_instance,
                                        const uint32_t joint_socket_id) {
  // check that the joint socket id is valid
  auto socket_it = joint_instance.sockets.find(joint_socket_id);
  if (socket_it == joint_instance.sockets.end())
    return sf::Vector2f{0.f, 0.f};

  // generate transform from the total rotation of the joint instance
  sf::Transform rotation_transform;
  rotation_transform.rotate(joint_instance.total_rotation);

  // generate a local alignment vector from the joint socket pivot to the joint
  // socket local position
  sf::Vector2f local_alignment_vector =
      socket_it->second.local_position - joint_instance.joint->socket_pivot;

  // apply the rotation transform to the local alignment vector to give the
  // world alignment vector
  sf::Vector2f alignment_vector =
      rotation_transform.transformPoint(local_alignment_vector);

  // return the normalized alignment vector
  return alignment_vector.normalized();
}

/////////////////////////////////////////////////
void initialize_joint_socket_positions(JointInstance &instance) {
  if (!instance.joint)
    return;

  std::vector<sf::Vector2f> positions;
  compute_socket_local_positions_even_spread(
      instance.joint->socket_config, instance.joint->socket_pivot, positions);

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
        instance.joint->socket_pivot +
        sf::Vector2f{std::cos(angle_rad), std::sin(angle_rad)} * config.radius;

    // for greater that one socket, place them on the arc min/max range and then
    // evenly spread them out across the arc

  } else {
    const float angle_between_sockets = arc_range / (config.socket_count - 1);
    for (size_t i = 0; i < config.socket_count; ++i) {
      const float angle_deg = arc_min + (i * angle_between_sockets);
      const float angle_rad = sf::degrees(angle_deg).asRadians();
      instance.sockets.at(i).local_position =
          instance.joint->socket_pivot +
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
void align_fragment_onto_joint_socket(FragmentInstance &fragment_instance,
                                      const uint32_t fragment_socket_id,
                                      const JointInstance &joint_instance,
                                      const uint32_t joint_socket_id) {

  std::cout << "[align_fragment] begin"
            << " fragment_id=" << fragment_instance.id
            << " fragment_socket_id=" << fragment_socket_id
            << " joint_id=" << joint_instance.id
            << " joint_socket_id=" << joint_socket_id << "\n";

  // check that the fragment socket id is valid
  auto fragment_socket_it = fragment_instance.sockets.find(fragment_socket_id);
  if (fragment_socket_it == fragment_instance.sockets.end()) {
    std::cout << "[align_fragment][skip] invalid fragment socket id="
              << fragment_socket_id << "\n";
    return;
  }
  FragmentSocketState &fragment_socket = fragment_socket_it->second;
  std::cout << "[align_fragment] fragment socket local=("
            << fragment_socket.local_position.x << ", "
            << fragment_socket.local_position.y << ")\n";

  // check that the joint socket id is valid
  auto joint_socket_it = joint_instance.sockets.find(joint_socket_id);
  if (joint_socket_it == joint_instance.sockets.end()) {
    std::cout << "[align_fragment][skip] invalid joint socket id="
              << joint_socket_id << "\n";
    return;
  }
  const JointSocketState &joint_socket = joint_socket_it->second;
  std::cout << "[align_fragment] joint socket local=("
            << joint_socket.local_position.x << ", "
            << joint_socket.local_position.y << ")\n";

  // check that the fragment socket is connected to the joint socket
  auto check_connection_result =
      action::grimoire_machina::check_for_connected_sockets(joint_instance,
                                                            fragment_instance);
  if (!check_connection_result.has_value()) {
    std::cout << "[align_fragment][skip] no socket connection between joint "
                 "and fragment\n";
    return;
  }
  std::cout << "[align_fragment] connection check passed\n";

  // cache world position of the sockets
  sf::Vector2f fragment_socket_world_position =
      fragment_instance.transform.transformPoint(
          fragment_socket.local_position);

  sf::Vector2f joint_socket_world_position =
      joint_instance.transform.transformPoint(joint_socket.local_position);

  std::cout << "[align_fragment] fragment socket world(before)=("
            << fragment_socket_world_position.x << ", "
            << fragment_socket_world_position.y << ")\n";
  std::cout << "[align_fragment] joint socket world=("
            << joint_socket_world_position.x << ", "
            << joint_socket_world_position.y << ")\n";

  // get the fragment socket alignment vector in world space and check it is not
  // 0,0
  sf::Vector2f fragment_socket_alignment_vector =
      calculate_alignment_vector(fragment_instance, fragment_socket_id);
  std::cout << "[align_fragment] fragment alignment vec=("
            << fragment_socket_alignment_vector.x << ", "
            << fragment_socket_alignment_vector.y << ")\n";
  if (fragment_socket_alignment_vector == sf::Vector2f{0.f, 0.f}) {
    std::cout << "[align_fragment][skip] fragment alignment vector is zero\n";
    return;
  }

  // get the joint socket alignment vector in world space and check it is not
  // 0,0
  sf::Vector2f joint_socket_alignment_vector =
      calculate_alignment_vector(joint_instance, joint_socket_id);
  std::cout << "[align_fragment] joint alignment vec=("
            << joint_socket_alignment_vector.x << ", "
            << joint_socket_alignment_vector.y << ")\n";
  if (joint_socket_alignment_vector == sf::Vector2f{0.f, 0.f}) {
    std::cout << "[align_fragment][skip] joint alignment vector is zero\n";
    return;
  }

  sf::Angle rotation_angle = rotation_of_vector_to_target_vector(
      fragment_socket_alignment_vector, joint_socket_alignment_vector);

  std::cout << "[align_fragment] rotation angle deg="
            << rotation_angle.asDegrees() << "\n";

  sf::Transform rotation_transform;
  rotation_transform.rotate(rotation_angle);

  const sf::Vector2f rotated_fragment_socket_world =
      rotation_transform.transformPoint(fragment_socket_world_position);

  std::cout << "[align_fragment] rotated fragment socket world=("
            << rotated_fragment_socket_world.x << ", "
            << rotated_fragment_socket_world.y << ")\n";

  // calculate the transform
  sf::Vector2f translation_vector =
      joint_socket_world_position -
      // apply the rotation to the fragment socket world position before
      // calculating the translation vector
      rotated_fragment_socket_world;

  std::cout << "[align_fragment] translation vec=(" << translation_vector.x
            << ", " << translation_vector.y << ")\n";

  // BUILDING THE TRANSFORM //
  // transforms are applied in reverse order, so we build translate then rotate

  std::cout << "[align_fragment] fragment total rotation before="
            << fragment_instance.total_rotation.asDegrees() << "\n";

  // reset the transform of the fragment instance to identity
  fragment_instance.transform = sf::Transform::Identity;
  std::cout << "[align_fragment] fragment transform reset to identity\n";

  // transform the fragment instance to the joint socket position
  fragment_instance.transform.translate(translation_vector);
  std::cout << "[align_fragment] fragment transform translated\n";

  // rotate the fragment instance to align the fragment socket alignment vector
  // with the joint socket alignment vector
  fragment_instance.transform.rotate(rotation_angle);
  std::cout << "[align_fragment] fragment transform rotated\n";

  // UPDATE FRAGMENT INSTANCE STATE //
  fragment_instance.total_rotation += rotation_angle;

  const sf::Vector2f fragment_socket_world_after =
      fragment_instance.transform.transformPoint(
          fragment_socket.local_position);

  std::cout << "[align_fragment] fragment socket world(after)=("
            << fragment_socket_world_after.x << ", "
            << fragment_socket_world_after.y << ")\n";
  std::cout << "[align_fragment] world delta to joint=("
            << (fragment_socket_world_after.x - joint_socket_world_position.x)
            << ", "
            << (fragment_socket_world_after.y - joint_socket_world_position.y)
            << ")\n";
  std::cout << "[align_fragment] fragment total rotation after="
            << fragment_instance.total_rotation.asDegrees() << "\n";
  std::cout << "[align_fragment] end\n";
}

/////////////////////////////////////////////////
void align_joint_onto_fragment_socket(JointInstance &joint_instance,
                                      const uint32_t joint_socket_id,
                                      const FragmentInstance &fragment_instance,
                                      const uint32_t fragment_socket_id) {

  // check that the joint socket id is valid
  auto joint_socket_it = joint_instance.sockets.find(joint_socket_id);
  if (joint_socket_it == joint_instance.sockets.end()) {
    return;
  }
  JointSocketState &joint_socket = joint_socket_it->second;

  // check that the fragment socket id is valid
  auto fragment_socket_it = fragment_instance.sockets.find(fragment_socket_id);
  if (fragment_socket_it == fragment_instance.sockets.end()) {
    return;
  }
  const FragmentSocketState &fragment_socket = fragment_socket_it->second;

  // check that the joint socket is connected to the fragment socket
  auto check_connection_result =
      action::grimoire_machina::check_for_connected_sockets(joint_instance,
                                                            fragment_instance);
  if (!check_connection_result.has_value()) {
    return;
  }

  // ROTATION //

  // get the fragment socket alignment vector in world space and check it is not
  // 0,0
  sf::Vector2f fragment_socket_alignment_vector =
      calculate_alignment_vector(fragment_instance, fragment_socket_id);
  if (fragment_socket_alignment_vector == sf::Vector2f{0.f, 0.f}) {
    return;
  }
  std::cout << "Fragment socket alignment vector: "
            << fragment_socket_alignment_vector.x << ", "
            << fragment_socket_alignment_vector.y << std::endl;

  // get the joint socket alignment vector in world space and check it is not
  // 0,0
  sf::Vector2f joint_socket_alignment_vector =
      calculate_alignment_vector(joint_instance, joint_socket_id);
  if (joint_socket_alignment_vector == sf::Vector2f{0.f, 0.f}) {
    return;
  }

  std::cout << "Joint socket alignment vector: "
            << joint_socket_alignment_vector.x << ", "
            << joint_socket_alignment_vector.y << std::endl;

  // calculate the rotation angle required to align the joint socket alignment
  // vector with the fragment socket alignment vector
  sf::Angle rotation_angle = rotation_of_vector_to_target_vector(
      joint_socket_alignment_vector, fragment_socket_alignment_vector);

  std::cout << "Rotation angle: " << rotation_angle.asDegrees() << " degrees"
            << std::endl;
  // build a rotation transform to apply before translation
  sf::Transform rotation_transform{sf::Transform::Identity};
  rotation_transform.rotate(rotation_angle);

  // TRANSLATION //
  // make sure to rotate the joint instance before calculating the translation
  // vector
  sf::Vector2f fragment_socket_world =
      fragment_instance.transform.transformPoint(
          fragment_socket.local_position);

  sf::Vector2f rotated_joint_socket_world = rotation_transform.transformPoint(
      joint_instance.transform.transformPoint(joint_socket.local_position));

  sf::Vector2f translation_vector =
      fragment_socket_world - rotated_joint_socket_world;

  // BUILD THE TRANSFORM //
  // transforms are applied in reverse order, so we build translate then rotate
  // to achieve rotation then translation
  joint_instance.transform = sf::Transform::Identity;
  joint_instance.transform.translate(translation_vector);
  joint_instance.transform.rotate(rotation_angle);

  // add to the total rotation of the joint instance
  joint_instance.total_rotation += rotation_angle;
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
    joint_instance->transform.translate(-joint_instance->joint->socket_pivot);

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
