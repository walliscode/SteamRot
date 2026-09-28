/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for positioning_ghost free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_ghost.h"
#include "FragmentInstance.h"
#include "MrGhost.h"
#include "Vector2fEqualsMatcher.h"
#include "catch2/catch_approx.hpp"
#include "fragment_library.h"
#include "joint_library.h"
#include <SFML/Graphics/VertexArray.hpp>
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

using namespace steamrot::logic::positioning::ghost;

TEST_CASE("rotate_ghost tests") {
  MrGhost mr_ghost;

  SECTION("rotate_ghost increments rotation by 90 degrees",
          "[unit][positioning_ghost]") {
    REQUIRE(mr_ghost.m_rotation_degrees == 0.f);

    rotate_ghost(mr_ghost);
    REQUIRE(mr_ghost.m_rotation_degrees == 90.f);
  }

  SECTION("rotate_ghost accumulates rotation across multiple calls",
          "[unit][positioning_ghost]") {

    rotate_ghost(mr_ghost);
    REQUIRE(mr_ghost.m_rotation_degrees == 90.f);

    rotate_ghost(mr_ghost);
    REQUIRE(mr_ghost.m_rotation_degrees == 180.f);

    rotate_ghost(mr_ghost);
    REQUIRE(mr_ghost.m_rotation_degrees == 270.f);
  }
}

TEST_CASE("update_position tests", "[unit][positioning_ghost]") {

  MrGhost mr_ghost;

  sf::Vector2f new_position(100.f, 200.f);
  SECTION("update_position sets the ghost's position to the new value",
          "[unit][positioning_ghost]") {
    mr_ghost.m_position = sf::Vector2f(0.f, 0.f);
    update_position(mr_ghost, new_position);
    REQUIRE_THAT(mr_ghost.m_position, EqualsVector2f(new_position));
  }

  SECTION("update_position updates the GhostInstance position  and rotation if "
          "it is a "
          "FragmentInstance",
          "[unit][positioning_ghost]") {
    // Arrange
    FragmentInstance fragment_instance{0,
                                       parts::FragmentRectangleWithOneSocket};
    fragment_instance.setPosition(sf::Vector2f(0.f, 0.f));
    mr_ghost.m_instance.emplace<FragmentInstance>(fragment_instance);
    mr_ghost.m_rotation_degrees = 0.f;
    const FragmentInstance &instance_ref =
        std::get<FragmentInstance>(mr_ghost.m_instance);

    REQUIRE_THAT(instance_ref.getPosition(),
                 EqualsVector2f(sf::Vector2f(0.f, 0.f)));
    REQUIRE(instance_ref.getRotation().asDegrees() == 0.f);

    // Act
    mr_ghost.m_rotation_degrees = 134.f;
    update_position(mr_ghost, new_position);

    // Assert
    REQUIRE_THAT(instance_ref.getPosition(), EqualsVector2f(new_position));
    REQUIRE(instance_ref.getRotation().asDegrees() ==
            Catch::Approx(134.f).margin(0.1f));
  }

  SECTION("update_position updates the GhostInstance position and rotation if "
          "it is a JointInstance",
          "[unit][positioning_ghost]") {
    // Arrange
    JointInstance joint_instance{0, parts::JointSquareWithOneSocket};
    joint_instance.setPosition(sf::Vector2f(0.f, 0.f));
    mr_ghost.m_instance.emplace<JointInstance>(joint_instance);
    mr_ghost.m_rotation_degrees = 0.f;
    const JointInstance &instance_ref =
        std::get<JointInstance>(mr_ghost.m_instance);
    REQUIRE_THAT(instance_ref.getPosition(),
                 EqualsVector2f(sf::Vector2f(0.f, 0.f)));
    REQUIRE(instance_ref.getRotation().asDegrees() == 0.f);
    // Act
    mr_ghost.m_rotation_degrees = 90.f;
    update_position(mr_ghost, new_position);
    // Assert
    REQUIRE_THAT(instance_ref.getPosition(), EqualsVector2f(new_position));
    REQUIRE(instance_ref.getRotation().asDegrees() ==
            Catch::Approx(90.f).margin(0.1f));
  }
}

TEST_CASE("process_subscribers tests", "[unit][positioning_ghost]") {
  MrGhost mr_ghost;
  std::vector<std::shared_ptr<Subscriber>> subscribers;

  SECTION(
      "process_subscribers does not rotate the ghost when no active subscriber "
      "has a ROTATE_GHOST action",
      "[unit][positioning_ghost]") {
    auto subscriber = std::make_shared<Subscriber>();
    subscriber->m_active = true;
    subscriber->captured_payload =
        InputPayload{InputPayload::InputAction::NONE};
    subscribers.push_back(subscriber);
    float initial_rotation = mr_ghost.m_rotation_degrees;
    process_subscribers(subscribers, mr_ghost);
    REQUIRE(mr_ghost.m_rotation_degrees == initial_rotation);
  }
  SECTION(
      "process_subscribers rotates the ghost when an active subscriber has a "
      "ROTATE_GHOST action",
      "[unit][positioning_ghost]") {
    auto subscriber = std::make_shared<Subscriber>();
    subscriber->m_active = true;
    subscriber->captured_payload =
        InputPayload{InputPayload::InputAction::ROTATE_GHOST};
    subscribers.push_back(subscriber);
    float initial_rotation = mr_ghost.m_rotation_degrees;
    process_subscribers(subscribers, mr_ghost);
    REQUIRE(mr_ghost.m_rotation_degrees ==
            Catch::Approx(initial_rotation + 90.f).margin(0.1f));
  }
}
} // namespace steamrot::tests
