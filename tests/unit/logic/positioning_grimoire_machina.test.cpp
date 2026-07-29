/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for positioning grimoire machina free functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_grimoire_machina.h"
#include "Fragment.h"
#include "Joint.h"
#include "JointInstance.h"
#include "MachinaFormScaffold.h"
#include "PartGraphBuilder.h"
#include "Vector2fEqualsMatcher.h"
#include "ViewDirection.h"
#include "action_grimoire_machina.h"
#include "catch2/catch_approx.hpp"
#include "fragment_library.h"
#include "grimoire_machina_test_helpers.h"
#include "joint_library.h"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <iostream>
#include <string>

namespace steamrot::tests {

using namespace steamrot::logic::positioning::grimoire_machina;

TEST_CASE("position_first_part_of_machina_form tests",
          "[positioning_grimoire_machina]") {
  // Arrange
  steamrot::PartGraph parts;

  SECTION("Does not throw when parts is empty") {
    // Act & Assert
    REQUIRE_NOTHROW(position_first_part_of_machina_form_scaffold(parts));
  }

  SECTION("Does not throw when Fragment has no sockets") {
    // Arrange
    steamrot::Fragment fragment{}; // empty fragment
    steamrot::FragmentInstance fragment_instance{
        0, fragment}; // instance of that fragment
    parts.emplace(fragment_instance.GetId(),
                  fragment_instance); // add to parts map

    REQUIRE(parts.size() == 1); // sanity check
    // Act & Assert
    REQUIRE_NOTHROW(steamrot::logic::positioning::grimoire_machina::
                        position_first_part_of_machina_form_scaffold(parts));
  }

  SECTION("Positions centre of first Fragmentinstance's FRONT view at 0,0") {
    // Arrange
    auto fragment = steamrot::tests::MakeFragmentWithFrontView();
    steamrot::FragmentInstance fragment_instance{0, fragment};
    parts.emplace(fragment_instance.GetId(), fragment_instance);
    REQUIRE(parts.size() == 1); // sanity check
    // pull out reference to the Fragmentinstance we just added so we can check
    // its transform after
    steamrot::FragmentInstance &instance =
        std::get<steamrot::FragmentInstance>(parts.at(0));

    // Act
    steamrot::logic::positioning::grimoire_machina::
        position_first_part_of_machina_form_scaffold(parts);

    // we expect the center to be translated to the origin
    sf::Vector2f expected_position{0.f, 0.f};

    // centre of box around triangle is at {15,15}
    sf::Vector2f actual_position =
        instance.GetTransform().transformPoint({15.f, 15.f});

    REQUIRE_THAT(actual_position,
                 steamrot::tests::EqualsVector2f(expected_position));
  }

  SECTION("Positions origin of first JointInstance at 0,0") {
    // Arrange
    auto joint = steamrot::tests::MakeJointWithFrontView();
    joint.socket_pivot = {5.f, 5.f}; // set origin to (5,5)
    steamrot::JointInstance joint_instance{0, joint};
    parts.emplace(joint_instance.GetId(), joint_instance);
    REQUIRE(parts.size() == 1); // sanity check
    // pull out reference to the JointInstance we just added so we can check
    // its transform after
    steamrot::JointInstance &instance =
        std::get<steamrot::JointInstance>(parts.at(0));
    // Act
    steamrot::logic::positioning::grimoire_machina::
        position_first_part_of_machina_form_scaffold(parts);

    // we expect the joint origin to be translated to {0,0}
    sf::Vector2f expected_position{0.f, 0.f};

    sf::Vector2f actual_position =
        instance.GetTransform().transformPoint(instance.GetPart().socket_pivot);

    REQUIRE_THAT(actual_position,
                 steamrot::tests::EqualsVector2f(expected_position));
  }
}

// TEST_CASE("maximise_joint_socket_spread tests",
//           "[positioning_grimoire_machina]") {
//   // Arrange
//   Joint joint;
//   joint.socket_pivot = {0.f, 0.f};
//   SocketConfig &config = joint.socket_config;
//
//   SECTION("Does not throw with zero sockets") {
//
//     // Arrange
//     config.socket_count = 0;
//     JointInstance joint_instance{0, joint};
//     // Act & Assert
//     REQUIRE_NOTHROW(maximise_joint_socket_spread(joint_instance));
//   }
//
//   SECTION("Joint with 2 sockets") {
//     config.socket_count = 2;
//     JointInstance joint_instance{&joint};
//     REQUIRE(joint_instance.sockets.size() == 2); // sanity check
//
//     SECTION("90° arc") {
//       config.rotation_arc_min = 0.f;
//       config.rotation_arc_max = 90.f; // arc from 0° to 90°
//       config.radius = 10.f;
//       maximise_joint_socket_spread(joint_instance);
//       // Assert: sockets should be at radius distance from origin at 0° and
//       90° sf::Vector2f expected_position_0{10.f, 0.f}; sf::Vector2f
//       expected_position_1{0.f, 10.f}; REQUIRE_THAT(
//           joint_instance.sockets.at(0).local_position,
//           steamrot::tests::EqualsVector2f(expected_position_0, 0.001f));
//       REQUIRE_THAT(
//           joint_instance.sockets.at(1).local_position,
//           steamrot::tests::EqualsVector2f(expected_position_1, 0.001f));
//     }
//
//     SECTION("180° arc") {
//       config.rotation_arc_min = 0.f;
//       config.rotation_arc_max = 180.f; // arc from 0° to 180°
//       config.radius = 10.f;
//       maximise_joint_socket_spread(joint_instance);
//       // Assert: sockets should be at radius distance from origin at 0° and
//       180° sf::Vector2f expected_position_0{10.f, 0.f}; sf::Vector2f
//       expected_position_1{-10.f, 0.f}; REQUIRE_THAT(
//           joint_instance.sockets.at(0).local_position,
//           steamrot::tests::EqualsVector2f(expected_position_0, 0.001f));
//       REQUIRE_THAT(
//           joint_instance.sockets.at(1).local_position,
//           steamrot::tests::EqualsVector2f(expected_position_1, 0.001f));
//     }
//
//     SECTION("270° arc") {
//       config.rotation_arc_min = 0.f;
//       config.rotation_arc_max = 270.f; // arc from 0° to 270°
//       config.radius = 10.f;
//       maximise_joint_socket_spread(joint_instance);
//       // Assert: sockets should be at radius distance from origin at 0° and
//       270° sf::Vector2f expected_position_0{10.f, 0.f}; sf::Vector2f
//       expected_position_1{0.f, -10.f}; REQUIRE_THAT(
//           joint_instance.sockets.at(0).local_position,
//           steamrot::tests::EqualsVector2f(expected_position_0, 0.001f));
//       REQUIRE_THAT(
//           joint_instance.sockets.at(1).local_position,
//           steamrot::tests::EqualsVector2f(expected_position_1, 0.001f));
//     }
//   }
//
//   SECTION("Joint with 3 sockets") {
//     config.socket_count = 3;
//     JointInstance joint_instance{&joint};
//     REQUIRE(joint_instance.sockets.size() == 3); // sanity check
//     //
//     SECTION("90° arc") {
//       config.rotation_arc_min = 0.f;
//       config.rotation_arc_max = 90.f; // arc from 0° to 90°
//       config.radius = 10.f;
//       maximise_joint_socket_spread(joint_instance);
//       // Assert: sockets should be at radius distance from origin at 0°, 45°,
//       // and 90°
//       sf::Vector2f expected_position_0{10.f, 0.f};
//       sf::Vector2f expected_position_1{7.071f, 7.071f}; // (10 * cos(45°), 10
//       *
//                                                         // sin(45°))
//       sf::Vector2f expected_position_2{0.f, 10.f};
//       REQUIRE_THAT(
//           joint_instance.sockets.at(0).local_position,
//           steamrot::tests::EqualsVector2f(expected_position_0, 0.001f));
//       REQUIRE_THAT(
//           joint_instance.sockets.at(1).local_position,
//           steamrot::tests::EqualsVector2f(expected_position_1, 0.001f));
//       REQUIRE_THAT(
//           joint_instance.sockets.at(2).local_position,
//           steamrot::tests::EqualsVector2f(expected_position_2, 0.001f));
//     }
//
//     SECTION("180° arc") {
//       config.rotation_arc_min = 0.f;
//       config.rotation_arc_max = 180.f; // arc from 0° to 180°
//       config.radius = 10.f;
//       maximise_joint_socket_spread(joint_instance);
//       // Assert: sockets should be at radius distance from origin at 0°, 90°,
//       // and 180°
//       sf::Vector2f expected_position_0{10.f, 0.f};
//       sf::Vector2f expected_position_1{0.f, 10.f};
//       sf::Vector2f expected_position_2{-10.f, 0.f};
//       REQUIRE_THAT(
//           joint_instance.sockets.at(0).local_position,
//           steamrot::tests::EqualsVector2f(expected_position_0, 0.001f));
//       REQUIRE_THAT(
//           joint_instance.sockets.at(1).local_position,
//           steamrot::tests::EqualsVector2f(expected_position_1, 0.001f));
//       REQUIRE_THAT(
//           joint_instance.sockets.at(2).local_position,
//           steamrot::tests::EqualsVector2f(expected_position_2, 0.001f));
//     }
//
//     SECTION("270° arc") {
//       config.rotation_arc_min = 0.f;
//       config.rotation_arc_max = 270.f; // arc from 0° to 270°
//       config.radius = 10.f;
//       maximise_joint_socket_spread(joint_instance);
//       // Assert: sockets should be at radius distance from origin at 0°,
//       135°,
//       // and 270°
//       sf::Vector2f expected_position_0{10.f, 0.f};
//       sf::Vector2f expected_position_1{-7.071f, 7.071f}; // (10 * cos(135°),
//       10
//                                                          // * sin(135°))
//       sf::Vector2f expected_position_2{0.f, -10.f};
//       REQUIRE_THAT(
//           joint_instance.sockets.at(0).local_position,
//           steamrot::tests::EqualsVector2f(expected_position_0, 0.001f));
//       REQUIRE_THAT(
//           joint_instance.sockets.at(1).local_position,
//           steamrot::tests::EqualsVector2f(expected_position_1, 0.001f));
//       REQUIRE_THAT(
//           joint_instance.sockets.at(2).local_position,
//           steamrot::tests::EqualsVector2f(expected_position_2, 0.001f));
//     }
//   }
// }

TEST_CASE("rotate_vector_to_target_vector tests") {

  SECTION("Does not throw with zero vectors") {
    sf::Vector2f source_vector{0.f, 0.f};
    sf::Vector2f target_vector{0.f, 0.f};

    REQUIRE_NOTHROW(
        rotation_of_vector_to_target_vector(source_vector, target_vector));
  }
  SECTION("Rotates vector to target vector") {

    // this is SFML space so y axis is inverted, so up is negative y and down is
    // positive y
    std::vector<sf::Vector2f> source_vectors{
        {1.f, 0.f},  // pointing right
        {0.f, -1.f}, // pointing up
        {-1.f, 0.f}, // pointing left
        {0.f, 1.f},  // pointing down
        {1.f, 1.f},  // pointing down-right
        {-1.f, -1.f} // pointing up-left
    };
    sf::Vector2f target_vector{0.f, 1.f}; // pointing down

    for (const auto &source_vector : source_vectors) {

      // create a transform that rotates the source vector to the target vector
      sf::Transform transform = sf::Transform::Identity;
      transform.rotate(
          rotation_of_vector_to_target_vector(source_vector, target_vector));

      // The transform should now rotate the source vector to the target vector,
      // normalise to avoid new magnitude issues
      sf::Vector2f transformed_source =
          transform.transformPoint(source_vector).normalized();
      ;

      REQUIRE_THAT(transformed_source, steamrot::tests::EqualsVector2f(
                                           target_vector.normalized(), 0.001f));
    }
  }
}

TEST_CASE("align_fragment_onto_joint_socket tests") {
  FragmentInstance fragment_instance{0, parts::FragmentRectangleWithTwoSockets};
  fragment_instance.SetTransform(sf::Transform::Identity); // reset transform
  JointInstance joint_instance{0, parts::JointSquareWithTwoSockets};
  joint_instance.SetTransform(sf::Transform::Identity); // reset transform
  joint_instance.PositionSockets(
      JointSocketPositioningStrategy::MaximizeDistance);

  REQUIRE_THAT(joint_instance.GetSocketLocalPosition(0),
               EqualsVector2f({23.f, 10.f}, 0.001f));
  REQUIRE_THAT(joint_instance.GetSocketLocalPosition(1),
               EqualsVector2f({10.f, 23.f}, 0.001f));

  // Normalize to [-180, 180] to compare equivalent rotations
  // e.g. 270 == -90, 180 == -180.
  auto normalize_degrees = [](float deg) {
    float d = std::fmod(deg, 360.f);
    if (d < 0.f)
      d += 360.f; // map negatives into [0,360)

    // snap tiny noise near bounds
    if (std::fabs(d) < 1e-4f || std::fabs(d - 360.f) < 1e-4f)
      d = 0.f;

    // round to 1 decimal place
    d = std::round(d * 10.f) / 10.f;
    return d;
  };
  ;

  SECTION("Does not throw if fragment socket id is invalid") {
    REQUIRE_NOTHROW(align_fragment_onto_joint_socket(fragment_instance, 99,
                                                     joint_instance, 0));
  }

  SECTION("Does not throw if joint socket id is invalid") {
    REQUIRE_NOTHROW(align_fragment_onto_joint_socket(fragment_instance, 0,
                                                     joint_instance, 99));
  }

  SECTION("Does not align if no connection between fragment and joint") {
    align_fragment_onto_joint_socket(fragment_instance, 0, joint_instance, 0);

    REQUIRE_THAT(fragment_instance.GetTransform().transformPoint(
                     fragment_instance.GetSocketLocalPosition(0)),
                 !steamrot::tests::EqualsVector2f(
                     joint_instance.GetTransform().transformPoint(
                         joint_instance.GetSocketLocalPosition(0)),
                     0.001f));
  }

  SECTION("Alignment cases (table-driven)") {
    struct AlignmentCase {
      const char *name;
      uint32_t fragment_socket_id;
      uint32_t joint_socket_id;
      float joint_rotation_deg;
      float expected_fragment_rotation_deg;
      std::optional<sf::Vector2f> expected_fragment_socket1_world;
    };

    const std::vector<AlignmentCase> cases{
        {"joint 0°, connect frag[0] -> joint[0]", 0, 0, 0.f, 0.f,
         sf::Vector2f{73.f, 10.f}},
        {"joint 0°, connect frag[0] -> joint[1]", 0, 1, 0.f, 90.f,
         sf::Vector2f{10.f, 73.f}},
        {"joint 90°, connect frag[0] -> joint[0]", 0, 0, 90.f, 90.f,
         std::nullopt},
        // shortest signed angle is -180 (equivalent to 180)
        {"joint 90°, connect frag[0] -> joint[1]", 0, 1, 90.f, -180.f,
         std::nullopt},
        // shortest signed angle is -180 (equivalent to 180)
        {"joint 180°, connect frag[0] -> joint[0]", 0, 0, 180.f, -180.f,
         std::nullopt},
        // shortest signed angle is -90 (equivalent to 270)
        {"joint 270°, connect frag[0] -> joint[0]", 0, 0, 270.f, -90.f,
         std::nullopt},
    };

    for (const auto &tc : cases) {
      DYNAMIC_SECTION(tc.name) {

        // reset per-case state so DYNAMIC_SECTION cases remain isolated
        fragment_instance.SetTransform(sf::Transform::Identity);
        fragment_instance.SetTotalRotation(sf::degrees(0.f));
        joint_instance.SetTransform(sf::Transform::Identity);
        joint_instance.SetTotalRotation(sf::degrees(0.f));
        joint_instance.PositionSockets(
            JointSocketPositioningStrategy::MaximizeDistance);

        if (tc.joint_rotation_deg != 0.f) {
          // add rotation to the transform
          joint_instance.GetTransform().rotate(
              sf::degrees(tc.joint_rotation_deg), {10.f, 10.f});

          // add rotation to the total rotation
          joint_instance.SetTotalRotation(sf::degrees(tc.joint_rotation_deg));
        }

        auto connection_result =
            fragment_instance.CreateConnectionWithOtherInstance(
                tc.fragment_socket_id, joint_instance, tc.joint_socket_id);
        if (!connection_result.has_value()) {
          FAIL("Failed to create connection between fragment and joint");
        }

        align_fragment_onto_joint_socket(fragment_instance,
                                         tc.fragment_socket_id, joint_instance,
                                         tc.joint_socket_id);

        const auto frag_socket_world =
            fragment_instance.GetTransform().transformPoint(
                fragment_instance.GetSocketLocalPosition(
                    tc.fragment_socket_id));

        const auto joint_socket_world =
            joint_instance.GetTransform().transformPoint(
                joint_instance.GetSocketLocalPosition(tc.joint_socket_id));

        REQUIRE_THAT(frag_socket_world,
                     EqualsVector2f(joint_socket_world, 0.001f));

        REQUIRE(
            normalize_degrees(
                fragment_instance.GetTotalRotation().asDegrees()) ==
            Catch::Approx(normalize_degrees(tc.expected_fragment_rotation_deg))
                .margin(0.1f));

        if (tc.expected_fragment_socket1_world.has_value()) {
          REQUIRE_THAT(
              fragment_instance.GetTransform().transformPoint(
                  fragment_instance.GetSocketLocalPosition(1)),
              EqualsVector2f(*tc.expected_fragment_socket1_world, 0.001f));
        }
      }
    }
  }
}

TEST_CASE("align_joint_onto_fragment_socket tests",
          "[positioning_grimoire_machina]") {
  // Arrange
  FragmentInstance fragment_instance{0, parts::FragmentRectangleWithTwoSockets};
  fragment_instance.SetTransform(sf::Transform::Identity); // reset transform
  REQUIRE_THAT(fragment_instance.GetSocketLocalPosition(0),
               steamrot::tests::EqualsVector2f({0.f, 5.f}, 0.001f));
  REQUIRE_THAT(fragment_instance.GetSocketLocalPosition(1),
               steamrot::tests::EqualsVector2f({50.f, 5.f}, 0.001f));

  JointInstance joint_instance{1, parts::JointSquareWithTwoSockets};
  joint_instance.SetTransform(sf::Transform::Identity); // reset transform
  joint_instance.PositionSockets(
      JointSocketPositioningStrategy::MaximizeDistance);

  // check the positions of the sockets
  REQUIRE_THAT(joint_instance.GetSocketLocalPosition(0),
               steamrot::tests::EqualsVector2f({23.f, 10.f}, 0.001f));
  REQUIRE_THAT(joint_instance.GetSocketLocalPosition(1),
               steamrot::tests::EqualsVector2f({10.f, 23.f}, 0.001f));

  auto normalize_degrees_0_360 = [](float deg) {
    float d = std::fmod(deg, 360.f);
    std::cout << "normalize_degrees_0_360: deg=" << deg << ", d=" << d
              << std::endl;
    if (d < 0.f)
      d += 360.f; // map negatives into [0,360)

    // snap tiny noise near bounds
    if (std::fabs(d) < 1e-4f || std::fabs(d - 360.f) < 1e-4f)
      d = 0.f;

    // round to 1 decimal place
    d = std::round(d * 10.f) / 10.f;
    return d;
  };

  SECTION("Does not throw if fragment socket id is invalid") {
    REQUIRE_NOTHROW(align_joint_onto_fragment_socket(joint_instance, 0,
                                                     fragment_instance, 99));
  }

  SECTION("Does not throw if joint socket id is invalid") {
    REQUIRE_NOTHROW(align_joint_onto_fragment_socket(joint_instance, 99,
                                                     fragment_instance, 0));
  }

  SECTION("Does not align if no connection between fragment and joint") {
    // Act
    align_joint_onto_fragment_socket(joint_instance, 0, fragment_instance, 0);
    // Assert: joint should not be aligned to fragment socket
    REQUIRE_THAT(joint_instance.GetTransform().transformPoint(
                     joint_instance.GetSocketLocalPosition(0)),
                 !steamrot::tests::EqualsVector2f(
                     fragment_instance.GetTransform().transformPoint(
                         fragment_instance.GetSocketLocalPosition(0)),
                     0.001f));
  }

  SECTION("Alignment cases (table-driven)") {
    struct AlignmentCase {
      const char *name;
      uint32_t fragment_socket_id;
      uint32_t joint_socket_id;
      float fragment_rotation_deg;
      float expected_joint_rotation_deg;
      sf::Vector2f expected_fragment_socket_0_world;
      sf::Vector2f expected_fragment_socket_1_world;
      sf::Vector2f expected_joint_socket_0_world;
      sf::Vector2f expected_joint_socket_1_world;
      sf::Vector2f expected_joint_socket_pivot_world;
      float position_tolerance;
    };

    const std::vector<AlignmentCase> cases{
        {"fragment 0°, connect frag[0] -> joint[0]",
         0,
         0,
         0,
         0,
         {0.f, 5.f},
         {50.f, 5.f},
         {0.f, 5.f},
         {-13.f, 18.f},
         {-13.f, 5.f},
         0.01f},
        {"fragment 45°, connect frag[0] -> joint[0]",
         0,
         0,
         45.f,
         45.f,
         {-3.535f, 3.535f},
         {31.82f, 38.89f},
         {-3.535f, 3.535f},
         {-21.93f, 3.535f},
         {-12.73f, -5.66f},
         0.01f},
        {
            "fragment 90°, connect frag[0] -> joint[0]",
            0,
            0,
            90.f,
            90.f,
            {-5.f, 0.f},
            {-5.f, 50.f},
            {-5.f, 0.f},
            {-18.f, -13.f},
            {-5.f, -13.f},
            0.01f,
        },
        {"fragment 135°, connect frag[0] -> joint[0]",
         0,
         0,
         135.f,
         135.f,
         {-3.535f, -3.535f},
         {-38.89f, 31.82f},
         {-3.535f, -3.535f},
         {-3.535f, -21.93f},
         {5.66f, -12.73f},
         0.01f},
        {"fragment 180°, connect frag[0] -> joint[0]",
         0,
         0,
         180.f,
         180.f,
         {0.f, -5.f},
         {-50.f, -5.f},
         {0.f, -5.f},
         {13.f, -18.f},
         {13.f, -5.f},
         0.01f},
        {
            "fragment 270°, connect frag[0] -> joint[0]",
            0,
            0,
            270.f,
            270.f,
            {5.f, 0.f},
            {5.f, -50.f},
            {5.f, 0.f},
            {18.f, 13.f},
            {5.f, 13.f},
            0.01f,
        },
        {"fragment 0°, connect frag[0] -> joint[1]",
         0,
         1,
         0.f,
         -90.f,
         {0.f, 5.f},
         {50.f, 5.f},
         {-13.f, -8.f},
         {0.f, 5.f},
         {-13.f, 5.f},
         0.01f},
        {"fragment 45°, connect frag[0] -> joint[1]",
         0,
         1,
         45.f,
         -45.f,
         {-3.535f, 3.535f},
         {31.82f, 38.89f},
         {-3.535f, -14.84f},
         {-3.535f, 3.535f},
         {-12.73f, -5.66f},
         0.01f},
        {"fragment 90°, connect frag[0] -> joint[1]",
         0,
         1,
         90.f,
         0.f,
         {-5.f, 0.f},
         {-5.f, 50.f},
         {8.f, -13.f},
         {-5.f, 0.f},
         {-5.f, -13.f},
         0.01f},
        {"fragment 180°, connect frag[0] -> joint[1]",
         0,
         1,
         180.f,
         90.f,
         {0.f, -5.f},
         {-50.f, -5.f},
         {13.f, 8.f},
         {0.f, -5.f},
         {13.f, -5.f},
         0.01f},
        {"fragment 270°, connect frag[0] -> joint[1]",
         0,
         1,
         270.f,
         180.f,
         {5.f, 0.f},
         {5.f, -50.f},
         {-8.f, 13.f},
         {5.f, 0.f},
         {5.f, 13.f},
         0.01f},
        {"fragment 0°, connect frag[1] -> joint[0]",
         1,
         0,
         0.f,
         180.f,
         {0.f, 5.f},
         {50.f, 5.f},
         {50.f, 5.f},
         {63.f, -8.f},
         {63.f, 5.f},
         0.01f},
        {"fragment 90°, connect frag[1] -> joint[0]",
         1,
         0,
         90.f,
         -90.f,
         {-5.f, 0.f},
         {-5.f, 50.f},
         {-5.f, 50.f},
         {8.f, 63.f},
         {-5.f, 63.f},
         0.01f}

    };

    for (const auto &tc : cases) {
      DYNAMIC_SECTION(tc.name) {
        // reset per-case state so cases are isolated
        fragment_instance.SetTransform(sf::Transform::Identity);
        fragment_instance.SetTotalRotation(sf::degrees(0.f));
        joint_instance.SetTransform(sf::Transform::Identity);
        joint_instance.SetTotalRotation(sf::degrees(0.f));
        joint_instance.PositionSockets(
            JointSocketPositioningStrategy::MaximizeDistance);

        // ARRANGE //
        auto connection_result =
            fragment_instance.CreateConnectionWithOtherInstance(
                tc.fragment_socket_id, joint_instance, tc.joint_socket_id);

        if (!connection_result.has_value()) {
          FAIL("Failed to create connection between fragment and joint");
        }

        // apply the fragment rotation for this test case
        fragment_instance.GetTransform().rotate(
            sf::degrees(tc.fragment_rotation_deg));
        // add to the total rotation as well
        fragment_instance.AddToTotalRotation(
            sf::degrees(tc.fragment_rotation_deg));

        // sanity-check rotated fragment sockets (same pattern in each case)
        REQUIRE_THAT(fragment_instance.GetTransform().transformPoint(
                         fragment_instance.GetSocketLocalPosition(0)),
                     EqualsVector2f(tc.expected_fragment_socket_0_world,
                                    tc.position_tolerance));
        REQUIRE_THAT(fragment_instance.GetTransform().transformPoint(
                         fragment_instance.GetSocketLocalPosition(1)),
                     EqualsVector2f(tc.expected_fragment_socket_1_world,
                                    tc.position_tolerance));

        // ACT //
        align_joint_onto_fragment_socket(joint_instance, tc.joint_socket_id,
                                         fragment_instance,
                                         tc.fragment_socket_id);

        // ASSERT //
        REQUIRE_THAT(joint_instance.GetTransform().transformPoint(
                         joint_instance.GetSocketLocalPosition(0)),
                     EqualsVector2f(tc.expected_joint_socket_0_world,
                                    tc.position_tolerance));

        REQUIRE_THAT(joint_instance.GetTransform().transformPoint(
                         joint_instance.GetPart().socket_pivot),
                     EqualsVector2f(tc.expected_joint_socket_pivot_world,
                                    tc.position_tolerance));

        REQUIRE_THAT(joint_instance.GetTransform().transformPoint(
                         joint_instance.GetSocketLocalPosition(1)),
                     EqualsVector2f(tc.expected_joint_socket_1_world,
                                    tc.position_tolerance));

        REQUIRE(normalize_degrees_0_360(
                    joint_instance.GetTotalRotation().asDegrees()) ==
                Catch::Approx(
                    normalize_degrees_0_360(tc.expected_joint_rotation_deg))
                    .margin(0.1f));

        // connected socket world positions should coincide
        const auto joint_connected_socket_world =
            joint_instance.GetTransform().transformPoint(
                joint_instance.GetSocketLocalPosition(tc.joint_socket_id));
        const auto fragment_connected_socket_world =
            fragment_instance.GetTransform().transformPoint(
                fragment_instance.GetSocketLocalPosition(
                    tc.fragment_socket_id));

        REQUIRE_THAT(joint_connected_socket_world,
                     EqualsVector2f(fragment_connected_socket_world, 0.001f));
      }
    }
  }
}

// TEST_CASE("compute_socket_local_positions_even_spread tests",
//           "[positioning_grimoire_machina]") {
//   // Arrange
//   steamrot::SocketConfig config;
//   sf::Vector2f origin{0.f, 0.f};
//   std::vector<sf::Vector2f> local_positions; // prepare vector for 3 sockets
//
//   SECTION("Does not throw with zero sockets") {
//
//     // Arrange
//     config.socket_count = 0;
//     // Act & Assert
//     REQUIRE_NOTHROW(steamrot::logic::positioning::grimoire_machina::
//                         compute_socket_local_positions_even_spread(
//                             config, origin, local_positions));
//   }
//   SECTION("Clears and resizes local_positions to socket_count") {
//     // Arrange
//     config.socket_count = 3;
//     local_positions = {{1.f, 1.f},
//                        {2.f, 2.f},
//                        {3.f, 3.f},
//                        {4.f, 4.f}}; // start with 4 positions
//     // Act
//     REQUIRE_NOTHROW(steamrot::logic::positioning::grimoire_machina::
//                         compute_socket_local_positions_even_spread(
//                             config, origin, local_positions));
//     // Assert: local_positions should be cleared and resized to 3
//     REQUIRE(local_positions.size() == 3);
//   }
//   SECTION("One socket tests") {
//     // Arrange
//     config.socket_count = 1;
//
//     SECTION("90° arc") {
//
//       config.rotation_arc_min = 0.f;
//       config.rotation_arc_max = 90.f; // arc from 0° to 90°
//
//       SECTION("radius 0") {
//         config.radius = 0.f;
//         compute_socket_local_positions_even_spread(config, origin,
//                                                    local_positions);
//         // Assert: socket should be at origin
//         REQUIRE_THAT(local_positions[0],
//                      steamrot::tests::EqualsVector2f(origin, 0.001f));
//       }
//       SECTION("radius 10") {
//         config.radius = 10.f;
//         compute_socket_local_positions_even_spread(config, origin,
//                                                    local_positions);
//
//         // Assert: socket should be at radius distance from origin at 45°
//         sf::Vector2f expected_position{
//             7.071f, 7.071f}; // (10 * cos(45°), 10 * sin(45°))
//         REQUIRE_THAT(local_positions[0], steamrot::tests::EqualsVector2f(
//                                              expected_position, 0.001f));
//       }
//       SECTION("radius 20") {
//         config.radius = 20.f;
//         compute_socket_local_positions_even_spread(config, origin,
//                                                    local_positions);
//         // Assert: socket should be at radius distance from origin at 45°
//         sf::Vector2f expected_position{
//             14.142f, 14.142f}; // (20 * cos(45°), 20 * sin(45°))
//         REQUIRE_THAT(local_positions[0], steamrot::tests::EqualsVector2f(
//                                              expected_position, 0.001f));
//       }
//     }
//     SECTION("180° arc") {
//       config.rotation_arc_min = 0.f;
//       config.rotation_arc_max = 180.f; // arc from 0° to 180°
//       //
//       SECTION("radius 10") {
//         config.radius = 10.f;
//         compute_socket_local_positions_even_spread(config, origin,
//                                                    local_positions);
//         // Assert: socket should be at radius distance from origin at 90°
//         sf::Vector2f expected_position{0.f,
//                                        10.f}; // (10 * cos(90°), 10 *
//                                        sin(90°))
//         REQUIRE_THAT(local_positions[0], steamrot::tests::EqualsVector2f(
//                                              expected_position, 0.001f));
//       }
//       SECTION("radius 20") {
//         config.radius = 20.f;
//         compute_socket_local_positions_even_spread(config, origin,
//                                                    local_positions);
//         // Assert: socket should be at radius distance from origin at 90°
//         sf::Vector2f expected_position{0.f,
//                                        20.f}; // (20 * cos(90°), 20 *
//                                        sin(90°))
//         REQUIRE_THAT(local_positions[0], steamrot::tests::EqualsVector2f(
//                                              expected_position, 0.001f));
//       }
//     }
//   }
//
//   SECTION("Two socket tests") {
//     // arrange
//     config.socket_count = 2;
//
//     SECTION("90° arc") {
//       config.rotation_arc_min = 0.f;
//       config.rotation_arc_max = 90.f; // arc from 0° to 90°
//
//       SECTION("radius 0") {
//         config.radius = 0.f;
//         compute_socket_local_positions_even_spread(config, origin,
//                                                    local_positions);
//         // Assert: sockets should be at origin
//         REQUIRE_THAT(local_positions[0],
//                      steamrot::tests::EqualsVector2f(origin, 0.001f));
//         REQUIRE_THAT(local_positions[1],
//                      steamrot::tests::EqualsVector2f(origin, 0.001f));
//       }
//       SECTION("radius 10") {
//         config.radius = 10.f;
//         compute_socket_local_positions_even_spread(config, origin,
//                                                    local_positions);
//         // Assert: sockets should be at radius distance from origin at 0°
//         // and 90°
//         sf::Vector2f expected_position_0{10.f,
//                                          0.f}; // (10 * cos(0°), 10 *
//                                          sin(0°))
//         sf::Vector2f expected_position_1{
//             0.f, 10.f}; // (10 * cos(90°), 10 * sin(90°))
//         REQUIRE_THAT(local_positions[0], steamrot::tests::EqualsVector2f(
//                                              expected_position_0, 0.001f));
//         REQUIRE_THAT(local_positions[1], steamrot::tests::EqualsVector2f(
//                                              expected_position_1, 0.001f));
//       }
//       SECTION("radius 20") {
//         config.radius = 20.f;
//         compute_socket_local_positions_even_spread(config, origin,
//                                                    local_positions);
//         // Assert: sockets should be at radius distance from origin at 0°
//         // and 90°
//         sf::Vector2f expected_position_0{20.f,
//                                          0.f}; // (20 * cos(0°), 20 *
//                                          sin(0°))
//         sf::Vector2f expected_position_1{
//             0.f, 20.f}; // (20 * cos(90°), 20 * sin(90°))
//         REQUIRE_THAT(local_positions[0], steamrot::tests::EqualsVector2f(
//                                              expected_position_0, 0.001f));
//         REQUIRE_THAT(local_positions[1], steamrot::tests::EqualsVector2f(
//                                              expected_position_1, 0.001f));
//       }
//     }
//     SECTION("180° arc") {
//       config.rotation_arc_min = 0.f;
//       config.rotation_arc_max = 180.f; // arc from 0° to 180°
//       SECTION("radius 10") {
//         config.radius = 10.f;
//         compute_socket_local_positions_even_spread(config, origin,
//                                                    local_positions);
//         // Assert: sockets should be at radius distance from origin at 0°
//         // and 180°
//         sf::Vector2f expected_position_0{10.f,
//                                          0.f}; // (10 * cos(0°), 10 *
//                                          sin(0°))
//         sf::Vector2f expected_position_1{
//             -10.f, 0.f}; // (10 * cos(180°), 10 * sin(180°))
//         REQUIRE_THAT(local_positions[0], steamrot::tests::EqualsVector2f(
//                                              expected_position_0, 0.001f));
//         REQUIRE_THAT(local_positions[1], steamrot::tests::EqualsVector2f(
//                                              expected_position_1, 0.001f));
//       }
//       SECTION("radius 20") {
//         config.radius = 20.f;
//         compute_socket_local_positions_even_spread(config, origin,
//                                                    local_positions);
//         // Assert: sockets should be at radius distance from origin at 0°
//         // and 180°
//         sf::Vector2f expected_position_0{20.f,
//                                          0.f}; // (20 * cos(0°), 20 *
//                                          sin(0°))
//         sf::Vector2f expected_position_1{
//             -20.f, 0.f}; // (20 * cos(180°), 20 * sin(180°))
//         REQUIRE_THAT(local_positions[0], steamrot::tests::EqualsVector2f(
//                                              expected_position_0, 0.001f));
//         REQUIRE_THAT(local_positions[1], steamrot::tests::EqualsVector2f(
//                                              expected_position_1, 0.001f));
//       }
//     }
//   }
//   SECTION("Three socket tests") {
//     // arrange
//     config.socket_count = 3;
//     SECTION("90° arc") {
//       config.rotation_arc_min = 0.f;
//       config.rotation_arc_max = 90.f; // arc from 0° to 90°
//       SECTION("radius 10") {
//         config.radius = 10.f;
//         compute_socket_local_positions_even_spread(config, origin,
//                                                    local_positions);
//         // Assert: sockets should be at radius distance from origin at 0°,
//         // 45°, and 90°
//         sf::Vector2f expected_position_0{10.f,
//                                          0.f}; // (10 * cos(0°), 10 *
//                                          sin(0°))
//         sf::Vector2f expected_position_1{
//             7.071f, 7.071f}; // (10 * cos(45°), 10 * sin(45°))
//         sf::Vector2f expected_position_2{
//             0.f, 10.f}; // (10 * cos(90°), 10 * sin(90°))
//         REQUIRE_THAT(local_positions[0], steamrot::tests::EqualsVector2f(
//                                              expected_position_0, 0.001f));
//         REQUIRE_THAT(local_positions[1], steamrot::tests::EqualsVector2f(
//                                              expected_position_1, 0.001f));
//         REQUIRE_THAT(local_positions[2], steamrot::tests::EqualsVector2f(
//                                              expected_position_2, 0.001f));
//       }
//     }
//     SECTION("180° arc") {
//       config.rotation_arc_min = 0.f;
//       config.rotation_arc_max = 180.f; // arc from 0° to 180°
//       SECTION("radius 10") {
//         config.radius = 10.f;
//         compute_socket_local_positions_even_spread(config, origin,
//                                                    local_positions);
//         // Assert: sockets should be at radius distance from origin at 0°,
//         // 90°, and 180°
//         sf::Vector2f expected_position_0{10.f,
//                                          0.f}; // (10 * cos(0°), 10 *
//                                          sin(0°))
//         sf::Vector2f expected_position_1{
//             0.f, 10.f}; // (10 * cos(90°), 10 * sin(90°))
//         sf::Vector2f expected_position_2{
//             -10.f, 0.f}; // (10 * cos(180°), 10 * sin(180°))
//         REQUIRE_THAT(local_positions[0], steamrot::tests::EqualsVector2f(
//                                              expected_position_0, 0.001f));
//         REQUIRE_THAT(local_positions[1], steamrot::tests::EqualsVector2f(
//                                              expected_position_1, 0.001f));
//         REQUIRE_THAT(local_positions[2], steamrot::tests::EqualsVector2f(
//                                              expected_position_2, 0.001f));
//       }
//     }
//   }
// }

// TEST_CASE("check_if_allowed_joint_socket_configuration tests",
//           "[unit][positioning_grimoire_machina]") {
//
//   SECTION("Returns false if any socket is out of bounds by radius") {
//
//     steamrot::Joint joint;
//     joint.socket_pivot = {0.f, 0.f};
//     joint.socket_config.socket_count = 3;
//     joint.socket_config.radius = 10.f;
//     joint.socket_config.rotation_arc_min = 0.f;
//     joint.socket_config.rotation_arc_max = 270.f;
//     steamrot::JointInstance instance{&joint};
//     steamrot::logic::positioning::grimoire_machina::
//         initialize_joint_socket_positions(instance);
//
//     // Manually set one socket to be out of bounds
//     instance.sockets.at(1).local_position = {13.f, 0.f};
//     REQUIRE_FALSE(steamrot::logic::positioning::grimoire_machina::
//                       check_if_allowed_joint_socket_configuration(instance));
//   }
//
//   SECTION("Returns false if any socket is out of bounds by rotation arc") {
//
//     struct TestCase {
//       std::string name;
//       float rotation_arc_min;
//       float rotation_arc_max;
//       sf::Vector2f out_of_bounds_socket_position;
//     };
//     steamrot::Joint joint;
//     joint.socket_pivot = {0.f, 0.f};
//     joint.socket_config.socket_count = 3;
//     joint.socket_config.radius = 10.f;
//     steamrot::JointInstance instance{&joint};
//     steamrot::logic::positioning::grimoire_machina::
//         initialize_joint_socket_positions(instance);
//
//     // Test cases for out-of-bounds sockets
//     std::vector<TestCase> test_cases = {
//         {"Socket below min arc", 45.f, 270.f, {10.f, 0.f}},
//         {"Socket above max arc", 0.f, 180.f, {0.f, -10.f}},
//     };
//
//     for (const auto &tc : test_cases) {
//       DYNAMIC_SECTION(tc.name) {
//         joint.socket_config.rotation_arc_min = tc.rotation_arc_min;
//         joint.socket_config.rotation_arc_max = tc.rotation_arc_max;
//         // set one socket to be out of bounds
//         instance.sockets.at(1).local_position =
//             tc.out_of_bounds_socket_position;
//         REQUIRE_FALSE(
//             steamrot::logic::positioning::grimoire_machina::
//                 check_if_allowed_joint_socket_configuration(instance));
//       }
//     }
//   }
//
//   SECTION("Returns false if minimum_gap is not maintained between sockets") {
//     // Arrange
//     steamrot::Joint joint;
//     joint.socket_pivot = {0.f, 0.f};
//     joint.socket_config.socket_count = 3;
//     joint.socket_config.radius = 10.f;
//     joint.socket_config.rotation_arc_min = 0.f;
//     joint.socket_config.rotation_arc_max = 180.f;
//     steamrot::JointInstance instance{&joint};
//
//     // set up a test struct to initialize the joint socket positions
//     struct TestStruct {
//       std::string name;
//       float minimum_gap;
//       sf::Vector2f socket_0_position;
//       sf::Vector2f socket_1_position;
//       sf::Vector2f socket_2_position;
//     };
//
//     std::vector<TestStruct> test_cases = {
//         {"Sockets too close together_one",
//          15.f,
//          {10.f, 0.f},
//          {9.510565f, 3.090170f},
//          {8.660254f, 5.f}},
//         {"Sockets too close together_two",
//          10.f,
//          {10.f, 0.f},
//          {9.238795f, 3.826834f},
//          {8.660254f, 5.f}},
//     };
//
//     for (const auto &tc : test_cases) {
//       DYNAMIC_SECTION(tc.name) {
//         joint.socket_config.minimum_gap = tc.minimum_gap;
//         instance.sockets.at(0).local_position = tc.socket_0_position;
//         instance.sockets.at(1).local_position = tc.socket_1_position;
//         instance.sockets.at(2).local_position = tc.socket_2_position;
//         REQUIRE_FALSE(
//             steamrot::logic::positioning::grimoire_machina::
//                 check_if_allowed_joint_socket_configuration(instance));
//       }
//     }
//   }
//
//   SECTION("Returns false if socket order is not maintained (clockwise)") {
//     // Arrange
//     steamrot::Joint joint;
//     joint.socket_pivot = {0.f, 0.f};
//     joint.socket_config.socket_count = 3;
//     joint.socket_config.radius = 10.f;
//     joint.socket_config.rotation_arc_min = 0.f;
//     joint.socket_config.rotation_arc_max = 270.f;
//     joint.socket_config.minimum_gap = 5.f;
//     steamrot::JointInstance instance{&joint};
//     // Manually set sockets to be out of order (clockwise)
//     instance.sockets.at(0).local_position = {10.f, 0.f}; // 0°
//     // set socket 1 to be at 270° (out of order)
//     instance.sockets.at(1).local_position = {0.f, -10.f}; // 270°
//     // set socket 2 to be at 135° (in order)
//     instance.sockets.at(2).local_position = {-7.071f, 7.071f}; // 135°
//     // Act & Assert
//     REQUIRE_FALSE(steamrot::logic::positioning::grimoire_machina::
//                       check_if_allowed_joint_socket_configuration(instance));
//   }
// }

TEST_CASE("position_part_graph tests", "[unit][positioning_grimoire_machina]") {

  SECTION("Does not throw when part graph is empty") {
    steamrot::PartGraph part_graph;
    REQUIRE_NOTHROW(
        steamrot::logic::positioning::grimoire_machina::position_part_graph(
            part_graph));
  }

  SECTION("Positions a simple part graph with one fragment and one joint") {

    // ARRANGE //
    PartGraphBuilder builder;
    PartGraphPackage part_graph_package =
        builder.AddJointInstance(parts::JointSquareWithOneSocket, "j0")
            .AddFragmentInstance(parts::FragmentRectangleWithOneSocket, "f1")
            .Connect("f1", 0, "j0", 0)
            .Build();

    PartGraph &part_graph = part_graph_package.part_graph;
    // get references to the fragment and joint instances
    auto &joint_instance_result =
        part_graph.at(part_graph_package.id_to_part_graph_id.at("j0"));
    REQUIRE(std::holds_alternative<JointInstance>(joint_instance_result));
    JointInstance &joint_instance =
        std::get<JointInstance>(joint_instance_result); // get joint instance
    joint_instance.PositionSockets(
        JointSocketPositioningStrategy::MaximizeDistance);
    //
    auto &fragment_instance_result =
        part_graph.at(part_graph_package.id_to_part_graph_id.at("f1"));
    REQUIRE(std::holds_alternative<FragmentInstance>(fragment_instance_result));
    FragmentInstance &fragment_instance =
        std::get<FragmentInstance>(fragment_instance_result); // get fragment
    REQUIRE(logic::action::grimoire_machina::check_for_connected_sockets(
        joint_instance, fragment_instance));

    // check positions before //
    const sf::Vector2f expected_ji_socket_0_world_before{19.19f, 19.19f};
    REQUIRE_THAT(joint_instance.GetSocketWorldPosition(0),
                 EqualsVector2f(expected_ji_socket_0_world_before, 0.01f));
    const sf::Vector2f expected_ji_socket_pivot_world_before{10.f, 10.f};
    REQUIRE_THAT(joint_instance.GetSocketPivotWorldPosition(),
                 EqualsVector2f(expected_ji_socket_pivot_world_before, 0.01f));

    const sf::Vector2f expected_fi_socket_0_world_before{0.f, 5.f};
    REQUIRE_THAT(fragment_instance.GetSocketWorldPosition(0),
                 EqualsVector2f(expected_fi_socket_0_world_before, 0.01f));

    // ACT //
    position_part_graph(part_graph);

    // ASSERT //
    // joint and joint sockets should not have moved but the fragment should
    // have been positioned onto the joint socket via the connection
    REQUIRE_THAT(joint_instance.GetSocketWorldPosition(0),
                 EqualsVector2f(expected_ji_socket_0_world_before, 0.01f));
    REQUIRE_THAT(joint_instance.GetSocketPivotWorldPosition(),
                 EqualsVector2f(expected_ji_socket_pivot_world_before, 0.01f));

    const sf::Vector2f expected_fi_socket_0_world_after{19.19f, 19.19f};
    REQUIRE_THAT(fragment_instance.GetSocketWorldPosition(0),
                 EqualsVector2f(expected_fi_socket_0_world_after, 0.01f));

    // rotates 45 degrees to align the fragment socket with the joint socket
    REQUIRE(fragment_instance.GetTotalRotation().asDegrees() == 45.f);
  }
  SECTION("Positions a chain graph: j0 - f1 - j2") {
    PartGraphBuilder builder;
    PartGraphPackage pkg =
        builder.AddJointInstance(parts::JointSquareWithOneSocket, "j0")
            .AddFragmentInstance(parts::FragmentRectangleWithTwoSockets, "f1")
            .AddJointInstance(parts::JointSquareWithOneSocket, "j2")
            .Connect("f1", 0, "j0", 0)
            .Connect("f1", 1, "j2", 0)
            .Build();

    PartGraph &g = pkg.part_graph;

    auto &j0v = g.at(pkg.id_to_part_graph_id.at("j0"));
    auto &f1v = g.at(pkg.id_to_part_graph_id.at("f1"));
    auto &j2v = g.at(pkg.id_to_part_graph_id.at("j2"));

    auto &j0 = std::get<JointInstance>(j0v);
    auto &f1 = std::get<FragmentInstance>(f1v);
    auto &j2 = std::get<JointInstance>(j2v);

    j0.PositionSockets(JointSocketPositioningStrategy::MaximizeDistance);
    j2.PositionSockets(JointSocketPositioningStrategy::MaximizeDistance);

    position_part_graph(g);

    // f1 socket 0 should align to j0 socket 0
    REQUIRE_THAT(f1.GetSocketWorldPosition(0),
                 EqualsVector2f(j0.GetSocketWorldPosition(0), 0.01f));
  }

  SECTION("Positions a branching graph: j0 connected to f1 and f2") {
    PartGraphBuilder builder;
    PartGraphPackage pkg =
        builder.AddJointInstance(parts::JointSquareWithTwoSockets, "j0")
            .AddFragmentInstance(parts::FragmentRectangleWithOneSocket, "f1")
            .AddFragmentInstance(parts::FragmentRectangleWithOneSocket, "f2")
            .Connect("f1", 0, "j0", 0)
            .Connect("f2", 0, "j0", 1)
            .Build();

    PartGraph &g = pkg.part_graph;

    auto &j0 = std::get<JointInstance>(g.at(pkg.id_to_part_graph_id.at("j0")));
    j0.PositionSockets(JointSocketPositioningStrategy::MaximizeDistance);
    auto &f1 =
        std::get<FragmentInstance>(g.at(pkg.id_to_part_graph_id.at("f1")));
    auto &f2 =
        std::get<FragmentInstance>(g.at(pkg.id_to_part_graph_id.at("f2")));

    position_part_graph(g);

    REQUIRE_THAT(f1.GetSocketWorldPosition(0),
                 EqualsVector2f(j0.GetSocketWorldPosition(0), 0.01f));

    REQUIRE_THAT(f2.GetSocketWorldPosition(0),
                 EqualsVector2f(j0.GetSocketWorldPosition(1), 0.01f));
  }

  SECTION("Does not infinite-loop on a cyclic graph") {
    PartGraphBuilder builder;
    PartGraphPackage pkg =
        builder.AddJointInstance(parts::JointSquareWithTwoSockets, "j0")
            .AddFragmentInstance(parts::FragmentRectangleWithTwoSockets, "f1")
            .Connect("f1", 0, "j0", 0)
            .Connect("f1", 1, "j0",
                     1) // creates cycle-like revisit opportunities
            .Build();

    PartGraph &g = pkg.part_graph;
    auto &j0 = std::get<JointInstance>(g.at(pkg.id_to_part_graph_id.at("j0")));
    j0.PositionSockets(JointSocketPositioningStrategy::MaximizeDistance);

    REQUIRE_NOTHROW(position_part_graph(g));
  }
}

TEST_CASE("calculate_composite_box tests") {

  sf::FloatRect compounding_box{};
  REQUIRE(compounding_box.position.x == 0);
  REQUIRE(compounding_box.position.y == 0);
  REQUIRE(compounding_box.size.x == 0);
  REQUIRE(compounding_box.size.y == 0);

  SECTION(" next box is added to compounding_box at origin and no size") {
    sf::FloatRect next_box{{50, 50}, {50, 50}};
    calculate_composite_box(compounding_box, next_box);

    // this box is to the right of the compounding_box at both x and y so
    // position of compounding_box should not change
    REQUIRE(compounding_box.position.x == 0);
    REQUIRE(compounding_box.position.y == 0);
    REQUIRE(compounding_box.size.x == 100);
    REQUIRE(compounding_box.size.y == 100);
  }

  SECTION("the next box has left edge further left ") {
    compounding_box.position = {0, 0};
    compounding_box.size = {100, 100};
    sf::FloatRect next_box{{-25, 10}, {50, 50}};
    calculate_composite_box(compounding_box, next_box);

    REQUIRE(compounding_box.position.x == -25);
    REQUIRE(compounding_box.position.y == 0);
    REQUIRE(compounding_box.size.x == 125);
    REQUIRE(compounding_box.size.y == 100);
  }

  SECTION("the next box has a top edge further up") {

    compounding_box.position = {0, 0};
    compounding_box.size = {100, 100};
    sf::FloatRect next_box{{10, -30}, {50, 50}};
    calculate_composite_box(compounding_box, next_box);

    REQUIRE(compounding_box.position.x == 0);
    REQUIRE(compounding_box.position.y == -30);
    REQUIRE(compounding_box.size.x == 100);
    REQUIRE(compounding_box.size.y == 130);
  }

  SECTION("the next box is bigger in all dimensions") {

    compounding_box.position = {0, 0};
    compounding_box.size = {60, 60};
    sf::FloatRect next_box{{-25, -30}, {130, 130}};
    calculate_composite_box(compounding_box, next_box);

    REQUIRE(compounding_box.position.x == -25);
    REQUIRE(compounding_box.position.y == -30);
    REQUIRE(compounding_box.size.x == 130);
    REQUIRE(compounding_box.size.y == 130);
  }

  SECTION("10 boxes are added sequentially to the compounding box") {
    compounding_box.position = {0, 0};
    compounding_box.size = {0, 0};
    for (int i = 0; i < 10; ++i) {
      sf::FloatRect next_box{
          {static_cast<float>(i * 10), static_cast<float>(i * 10)},
          {10.f, 10.f}};
      calculate_composite_box(compounding_box, next_box);
    }
    REQUIRE(compounding_box.position.x == 0);
    REQUIRE(compounding_box.position.y == 0);
    REQUIRE(compounding_box.size.x == 100);
    REQUIRE(compounding_box.size.y == 100);
  }
}
TEST_CASE("calculate_outer_box tests") {

  // set up a fragment wiht a single triangle in the front view
  Fragment fragment;
  sf::VertexArray &front_array =
      fragment.positioning_views[ViewDirection::Front];
  front_array.setPrimitiveType(sf::PrimitiveType::Triangles);
  front_array.append(sf::Vertex(sf::Vector2f(0.f, 0.f)));
  front_array.append(sf::Vertex(sf::Vector2f(30.f, 0.f)));
  front_array.append(sf::Vertex(sf::Vector2f(15.f, 30.f)));

  Fragment fragment2;
  sf::VertexArray &front_array2 =
      fragment2.positioning_views[ViewDirection::Front];
  front_array2.setPrimitiveType(sf::PrimitiveType::Triangles);
  front_array2.append(sf::Vertex(sf::Vector2f(10.f, 10.f)));
  front_array2.append(sf::Vertex(sf::Vector2f(40.f, 10.f)));
  front_array2.append(sf::Vertex(sf::Vector2f(25.f, 40.f)));

  // large fragment
  Fragment fragment_large;
  sf::VertexArray &front_array_large =
      fragment_large.positioning_views[ViewDirection::Front];
  front_array_large.setPrimitiveType(sf::PrimitiveType::Triangles);
  front_array_large.append(sf::Vertex(sf::Vector2f(-100.f, -100.f)));
  front_array_large.append(sf::Vertex(sf::Vector2f(150.f, -100.f)));
  front_array_large.append(sf::Vertex(sf::Vector2f(25.f, 150.f)));

  SECTION("PartGraph has only one part and no subgraph provided, does not "
          "exceed minimum box size") {
    PartGraph part_graph;
    FragmentInstance fragment_instance{0, fragment};
    part_graph.emplace(fragment_instance.GetId(), fragment_instance);
    sf::FloatRect outer_box =
        calculate_outer_box(part_graph, SubGraph{}); // empty subgraph

    REQUIRE(outer_box.size.x == 200.f);
    REQUIRE(outer_box.size.y == 200.f);
    REQUIRE(outer_box.position.x == -100.f);
    REQUIRE(outer_box.position.y == -100.f);
  }

  SECTION("PartGraph has only one part and no subgraph provided, exceeds "
          "minimum size") {
    PartGraph part_graph;
    FragmentInstance fragment_instance{0, fragment_large};
    part_graph.emplace(fragment_instance.GetId(), fragment_instance);
    sf::FloatRect outer_box =
        calculate_outer_box(part_graph, SubGraph{}); // empty subgraph
    // The outer box should be the same as the fragment's bounding box since
    // it exceeds the minimum size
    REQUIRE(outer_box.size.x == 250.f);
    REQUIRE(outer_box.size.y == 250.f);
    REQUIRE(outer_box.position.x == -100.f);
    REQUIRE(outer_box.position.y == -100.f);
  }

  SECTION("PartGraph has only one part, no subgraph provided, and local "
          "transform applied, does not exceed minimum box size") {
    PartGraph part_graph;
    FragmentInstance fragment_instance{0, fragment};
    // apply a local transform to the fragment instance
    fragment_instance.GetTransform().translate({10.f, 10.f});
    part_graph.emplace(fragment_instance.GetId(), fragment_instance);
    sf::FloatRect outer_box =
        calculate_outer_box(part_graph, SubGraph{}); // empty subgraph

    REQUIRE(outer_box.size.x == 200.f);
    REQUIRE(outer_box.size.y == 200.f);
    REQUIRE(outer_box.position.x == -100.f);
    REQUIRE(outer_box.position.y == -100.f);
  }

  SECTION("PartGraph has only one part, no subgraph provided, and local "
          "transform applied, exceeds minimum box size") {
    PartGraph part_graph;
    FragmentInstance fragment_instance{0, fragment_large};
    // apply a local transform to the fragment instance
    fragment_instance.GetTransform().translate({10.f, 10.f});
    part_graph.emplace(fragment_instance.GetId(), fragment_instance);
    sf::FloatRect outer_box =
        calculate_outer_box(part_graph, SubGraph{}); // empty subgraph
    // The outer box should be the same as the fragment's bounding box since
    // it exceeds the minimum size
    REQUIRE(outer_box.size.x == 260.f);
    REQUIRE(outer_box.size.y == 260.f);
    REQUIRE(outer_box.position.x == -100.f);
    REQUIRE(outer_box.position.y == -100.f);
  }

  SECTION("PartGraph has multiple parts, no subgraph provided and transforms "
          "applied, does not exceed minimum box size") {
    PartGraph part_graph;
    FragmentInstance fragment_instance1{0, fragment};
    fragment_instance1.GetTransform().translate({10.f, 10.f});
    part_graph.emplace(fragment_instance1.GetId(), fragment_instance1);
    FragmentInstance fragment_instance2{1, fragment2};
    fragment_instance2.GetTransform().translate({-5.f, -5.f});
    part_graph.emplace(fragment_instance2.GetId(), fragment_instance2);
    sf::FloatRect outer_box =
        calculate_outer_box(part_graph, SubGraph{}); // empty subgraph

    REQUIRE(outer_box.size.x == 200.f);
    REQUIRE(outer_box.size.y == 200.f);
    REQUIRE(outer_box.position.x == -100.f);
    REQUIRE(outer_box.position.y == -100.f);
  }

  SECTION("PartGraph has multiple parts, subgraph provided and transforms "
          "applied") {
    PartGraph part_graph;
    FragmentInstance fragment_instance1{0, fragment};

    fragment_instance1.GetTransform().translate({10.f, 10.f});
    part_graph.emplace(fragment_instance1.GetId(), fragment_instance1);
    FragmentInstance fragment_instance2{1, fragment2};
    fragment_instance2.GetTransform().translate({-5.f, -5.f});
    part_graph.emplace(fragment_instance2.GetId(), fragment_instance2);
    SubGraph sub_graph{0}; // only include the first part
    sf::FloatRect outer_box = calculate_outer_box(part_graph, sub_graph);

    // because a subgraph has been provided, the minimum box size in the
    // function is not applied
    REQUIRE(outer_box.size.x == 30.f);
    REQUIRE(outer_box.size.y == 30.f);
    REQUIRE(outer_box.position.x == 10.f);
    REQUIRE(outer_box.position.y == 10.f);
  }
}

} // namespace steamrot::tests
