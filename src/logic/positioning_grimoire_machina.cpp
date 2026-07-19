/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for positioning grimoire machina
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_grimoire_machina.h"
#include "JointInstance.h"
#include "SocketState.h"
#include "action_grimoire_machina.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <variant>

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

  // check that the fragment socket id is valid
  auto fragment_socket_it = fragment_instance.sockets.find(fragment_socket_id);
  if (fragment_socket_it == fragment_instance.sockets.end()) {
    return;
  }
  FragmentSocketState &fragment_socket = fragment_socket_it->second;

  // check that the joint socket id is valid
  auto joint_socket_it = joint_instance.sockets.find(joint_socket_id);
  if (joint_socket_it == joint_instance.sockets.end()) {
    return;
  }
  const JointSocketState &joint_socket = joint_socket_it->second;

  // check that the fragment socket is connected to the joint socket
  auto check_connection_result =
      action::grimoire_machina::check_for_connected_sockets(joint_instance,
                                                            fragment_instance);
  if (!check_connection_result.has_value()) {
    return;
  }

  // cache world position of the sockets
  sf::Vector2f fragment_socket_world_position =
      fragment_instance.transform.transformPoint(
          fragment_socket.local_position);

  sf::Vector2f joint_socket_world_position =
      joint_instance.transform.transformPoint(joint_socket.local_position);

  // get the fragment socket alignment vector in world space and check it is not
  // 0,0
  sf::Vector2f fragment_socket_alignment_vector =
      calculate_alignment_vector(fragment_instance, fragment_socket_id);
  if (fragment_socket_alignment_vector == sf::Vector2f{0.f, 0.f}) {
    return;
  }

  // get the joint socket alignment vector in world space and check it is not
  // 0,0
  sf::Vector2f joint_socket_alignment_vector =
      calculate_alignment_vector(joint_instance, joint_socket_id);

  if (joint_socket_alignment_vector == sf::Vector2f{0.f, 0.f}) {

    return;
  }

  sf::Angle rotation_angle = rotation_of_vector_to_target_vector(
      fragment_socket_alignment_vector, joint_socket_alignment_vector);

  sf::Transform rotation_transform;
  rotation_transform.rotate(rotation_angle);

  const sf::Vector2f rotated_fragment_socket_world =
      rotation_transform.transformPoint(fragment_socket_world_position);

  // calculate the transform
  sf::Vector2f translation_vector =
      joint_socket_world_position -
      // apply the rotation to the fragment socket world position before
      // calculating the translation vector
      rotated_fragment_socket_world;

  // BUILDING THE TRANSFORM //
  // transforms are applied in reverse order, so we build translate then rotate

  // reset the transform of the fragment instance to identity
  fragment_instance.transform = sf::Transform::Identity;

  // transform the fragment instance to the joint socket position
  fragment_instance.transform.translate(translation_vector);

  // rotate the fragment instance to align the fragment socket alignment vector
  // with the joint socket alignment vector
  fragment_instance.transform.rotate(rotation_angle);

  // UPDATE FRAGMENT INSTANCE STATE //
  fragment_instance.total_rotation += rotation_angle;

  const sf::Vector2f fragment_socket_world_after =
      fragment_instance.transform.transformPoint(
          fragment_socket.local_position);
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

  // get the joint socket alignment vector in world space and check it is not
  // 0,0
  sf::Vector2f joint_socket_alignment_vector =
      calculate_alignment_vector(joint_instance, joint_socket_id);
  if (joint_socket_alignment_vector == sf::Vector2f{0.f, 0.f}) {
    return;
  }

  // calculate the rotation angle required to align the joint socket alignment
  // vector with the fragment socket alignment vector
  sf::Angle rotation_angle = rotation_of_vector_to_target_vector(
      joint_socket_alignment_vector, fragment_socket_alignment_vector);

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
bool check_if_allowed_joint_socket_configuration(
    const JointInstance &joint_instance) {

  // if the joint pointer is null, return false
  if (!joint_instance.joint) {
    std::cerr << "[SocketCheck] FAIL: joint_instance.joint is null\n";
    return false;
  }

  const SocketConfig &config = joint_instance.joint->socket_config;
  const sf::Vector2f pivot = joint_instance.joint->socket_pivot;

  constexpr float kPi = 3.14159265358979323846f;
  constexpr float kRadToDeg = 180.0f / kPi;

  auto normalize_degrees_0_360 = [](float deg) -> float {
    float normalized = std::fmod(deg, 360.0f);
    if (normalized < 0.0f)
      normalized += 360.0f;
    return normalized;
  };

  auto angle_degrees_from_position =
      [&](const sf::Vector2f &position) -> float {
    sf::Vector2f dir = position - pivot;
    float angle_rad = std::atan2(dir.y, dir.x);
    float angle_deg = angle_rad * kRadToDeg;
    return normalize_degrees_0_360(angle_deg);
  };

  auto is_angle_within_arc = [&](float angle_deg, float arc_min_deg,
                                 float arc_max_deg) -> bool {
    float min_norm = normalize_degrees_0_360(arc_min_deg);
    float max_norm = normalize_degrees_0_360(arc_max_deg);

    // Non-wrapping arc: [min, max]
    if (min_norm <= max_norm) {
      return angle_deg >= min_norm && angle_deg <= max_norm;
    }

    // Wrapping arc (e.g. 300..30): [min..360) U [0..max]
    return angle_deg >= min_norm || angle_deg <= max_norm;
  };

  auto smallest_angular_difference_degrees = [&](float a_deg,
                                                 float b_deg) -> float {
    float diff = std::abs(a_deg - b_deg);
    return std::min(diff, 360.0f - diff);
  };

  std::cerr << "[SocketCheck] START: sockets=" << joint_instance.sockets.size()
            << ", radius=" << config.radius
            << ", arc_min=" << config.rotation_arc_min
            << ", arc_max=" << config.rotation_arc_max
            << ", minimum_gap=" << config.minimum_gap << "\n";

  // check whether the local positions fall within the arc defined by the socket
  // config
  for (const auto &[socket_id, socket] : joint_instance.sockets) {

    sf::Vector2f direction = socket.local_position - pivot;
    float distance = direction.length();
    float radius_error = std::abs(distance - config.radius);

    float angle_deg = angle_degrees_from_position(socket.local_position);

    std::cerr << "[SocketCheck] socket_id=" << socket_id << " pos=("
              << socket.local_position.x << ", " << socket.local_position.y
              << ")"
              << " dir=(" << direction.x << ", " << direction.y << ")"
              << " dist=" << distance << " radius_error=" << radius_error
              << " angle_deg=" << angle_deg << "\n";

    // check distance to pivot matches configured radius
    if (radius_error > 0.01f) {
      std::cerr << "[SocketCheck] FAIL(socket_id=" << socket_id
                << "): distance-to-radius mismatch. dist=" << distance
                << ", expected=" << config.radius << ", error=" << radius_error
                << ", tolerance=0.01\n";
      return false;
    }

    // check whether angle is inside configured arc
    if (!is_angle_within_arc(angle_deg, config.rotation_arc_min,
                             config.rotation_arc_max)) {
      std::cerr << "[SocketCheck] FAIL(socket_id=" << socket_id
                << "): angle out of arc. angle=" << angle_deg << ", arc=["
                << config.rotation_arc_min << ", " << config.rotation_arc_max
                << "]\n";
      return false;
    }

    // check minimum angular gap against all other sockets
    for (const auto &[other_socket_id, other_socket] : joint_instance.sockets) {
      if (socket_id == other_socket_id)
        continue;

      float other_angle_deg =
          angle_degrees_from_position(other_socket.local_position);

      float angle_difference =
          smallest_angular_difference_degrees(angle_deg, other_angle_deg);

      std::cerr << "[SocketCheck] pair=(" << socket_id << "," << other_socket_id
                << ")"
                << " angle_a=" << angle_deg << ", angle_b=" << other_angle_deg
                << ", diff=" << angle_difference
                << ", min_gap=" << config.minimum_gap << "\n";

      if (angle_difference < config.minimum_gap) {
        std::cerr << "[SocketCheck] FAIL(pair=" << socket_id << ","
                  << other_socket_id
                  << "): angular gap too small. diff=" << angle_difference
                  << ", minimum_gap=" << config.minimum_gap << "\n";
        return false;
      }
    }
  }

  std::cerr << "[SocketCheck] PASS: configuration allowed\n";
  return true;
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
void position_part_graph(PartGraph &part_graph) {
  std::cout << "[position_part_graph] begin, size=" << part_graph.size()
            << "\n";

  if (part_graph.empty()) {
    std::cout << "[position_part_graph] graph is empty, returning\n";
    return;
  }

  std::unordered_set<uint32_t> visited;
  std::unordered_set<uint32_t> in_stack; // optional: cycle diagnostics

  auto position_part_graph_recursive = [&](this auto &&self,
                                           const uint32_t part_id) -> void {
    std::cout << "[position_part_graph_recursive] enter part_id=" << part_id
              << "\n";

    // already done
    if (visited.contains(part_id)) {
      std::cout << "  [skip] already visited part_id=" << part_id << "\n";
      return;
    }

    // part must exist
    auto current_it = part_graph.find(part_id);
    if (current_it == part_graph.end()) {
      std::cout << "  [warn] part_id not found: " << part_id << "\n";
      return;
    }

    // mark EARLY to prevent cycles/back-edges from re-processing this node
    visited.insert(part_id);
    in_stack.insert(part_id);

    std::cout << "  [visit] entering variant for part_id=" << part_id << "\n";

    std::visit(
        [&](auto &current_part_instance) {
          using CurrentT = std::decay_t<decltype(current_part_instance)>;

          std::cout << "  [visit] current type="
                    << (std::is_same_v<CurrentT, FragmentInstance>
                            ? "FragmentInstance"
                        : std::is_same_v<CurrentT, JointInstance>
                            ? "JointInstance"
                            : "Unknown")
                    << " sockets=" << current_part_instance.sockets.size()
                    << "\n";

          for (const auto &[current_socket_id, current_socket_state] :
               current_part_instance.sockets) {
            std::cout << "    [socket] current_socket_id=" << current_socket_id
                      << " state="
                      << static_cast<int>(current_socket_state.connection_state)
                      << " has_connected_to="
                      << (current_socket_state.connected_to.has_value()
                              ? "true"
                              : "false")
                      << "\n";

            if (current_socket_state.connection_state !=
                    SocketConnectionState::Connected ||
                !current_socket_state.connected_to) {
              std::cout << "      [skip] socket not fully connected\n";
              continue;
            }

            const SocketConnection &connection =
                *current_socket_state.connected_to;
            std::cout << "      [connection] peer_part_id="
                      << connection.peer_part_id
                      << " peer_socket_id=" << connection.peer_socket_id
                      << "\n";

            // early skip if peer already visited
            if (visited.contains(connection.peer_part_id)) {
              std::cout
                  << "      [skip] connected part already visited part_id="
                  << connection.peer_part_id << "\n";
              continue;
            }

            auto connected_it = part_graph.find(connection.peer_part_id);
            if (connected_it == part_graph.end()) {
              std::cout << "      [warn] connected part not found: "
                        << connection.peer_part_id << "\n";
              continue;
            }

            auto &connected_part_variant = connected_it->second;

            std::cout << "      [visit-check] current="
                      << (std::is_same_v<CurrentT, FragmentInstance>
                              ? "Fragment"
                              : "Joint")
                      << " connected_is_joint="
                      << (std::holds_alternative<JointInstance>(
                              connected_part_variant)
                              ? "true"
                              : "false")
                      << " connected_is_fragment="
                      << (std::holds_alternative<FragmentInstance>(
                              connected_part_variant)
                              ? "true"
                              : "false")
                      << "\n";

            if constexpr (std::is_same_v<CurrentT, FragmentInstance>) {
              if (std::holds_alternative<JointInstance>(
                      connected_part_variant)) {
                std::cout << "      [align] Joint onto Fragment "
                          << "(peer_socket=" << connection.peer_socket_id
                          << ", current_socket=" << current_socket_id << ")\n";

                align_joint_onto_fragment_socket(
                    std::get<JointInstance>(connected_part_variant),
                    connection.peer_socket_id, current_part_instance,
                    current_socket_id);
              } else {
                std::cout << "      [skip] Fragment -> non-Joint pairing\n";
                continue;
              }
            } else if constexpr (std::is_same_v<CurrentT, JointInstance>) {
              if (std::holds_alternative<FragmentInstance>(
                      connected_part_variant)) {
                std::cout << "      [align] Fragment onto Joint "
                          << "(peer_socket=" << connection.peer_socket_id
                          << ", current_socket=" << current_socket_id << ")\n";

                align_fragment_onto_joint_socket(
                    std::get<FragmentInstance>(connected_part_variant),
                    connection.peer_socket_id, current_part_instance,
                    current_socket_id);
              } else {
                std::cout << "      [skip] Joint -> non-Fragment pairing\n";
                continue;
              }
            }

            if (in_stack.contains(connection.peer_part_id)) {
              std::cout
                  << "      [cycle] peer currently in recursion stack part_id="
                  << connection.peer_part_id << "\n";
            }

            std::cout << "      [recurse] -> part_id="
                      << connection.peer_part_id << "\n";
            self(connection.peer_part_id);
          }
        },
        current_it->second);

    in_stack.erase(part_id);
    std::cout << "[position_part_graph_recursive] exit part_id=" << part_id
              << " visited_count=" << visited.size() << "\n";
  };

  if (part_graph.find(0) != part_graph.end()) {
    std::cout << "[start] root part_id=0\n";
    position_part_graph_recursive(0);
  } else {
    std::cout << "[warn] part_id 0 not found in part graph\n";
  }

  std::cout << "[position_part_graph] end\n";
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
