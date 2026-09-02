/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the FragmentInstance class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FragmentInstance.h"
#include "ColorEqualsMatcher.h"
#include "JointInstance.h"
#include "SocketState.h"
#include "Vector2fEqualsMatcher.h"
#include "catch2/catch_approx.hpp"
#include "catch2/matchers/catch_matchers.hpp"
#include "fragment_library.h"
#include "joint_library.h"
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

TEST_CASE("FragmentInstance::FragmentInstance tests", "[FragmentInstance]") {
  SECTION("Construct with id and fragment") {
    FragmentInstance fragment_instance(42,
                                       parts::FragmentRectangleWithOneSocket);

    REQUIRE(fragment_instance.GetId() == 42);
    REQUIRE(fragment_instance.GetAlias() == "none");
    REQUIRE(&fragment_instance.GetPart() ==
            &parts::FragmentRectangleWithOneSocket);
    REQUIRE(fragment_instance.GetSocketCount() ==
            parts::FragmentRectangleWithOneSocket.sockets.size());
  }

  SECTION("Construct with id, fragment, and alias") {
    FragmentInstance fragment_instance(7, parts::FragmentRectangleWithOneSocket,
                                       "frag_A");

    REQUIRE(fragment_instance.GetId() == 7);
    REQUIRE(fragment_instance.GetAlias() == "frag_A");
    REQUIRE(&fragment_instance.GetPart() ==
            &parts::FragmentRectangleWithOneSocket);
    REQUIRE(fragment_instance.GetSocketCount() ==
            parts::FragmentRectangleWithOneSocket.sockets.size());
  }
}

TEST_CASE("FragmentInstance::GetPart tests", "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket,
                                     "test_fragment");

  SECTION("Returns referenced fragment") {
    REQUIRE(&fragment_instance.GetPart() ==
            &parts::FragmentRectangleWithOneSocket);
  }
}

TEST_CASE("FragmentInstance::GetId tests", "[FragmentInstance]") {
  FragmentInstance fragment_instance(123, parts::FragmentRectangleWithOneSocket,
                                     "test_fragment");

  SECTION("Returns stable id") { REQUIRE(fragment_instance.GetId() == 123); }
}

TEST_CASE("FragmentInstance::GetAlias tests", "[FragmentInstance]") {
  SECTION("Default alias is none") {
    FragmentInstance fragment_instance(1,
                                       parts::FragmentRectangleWithOneSocket);
    REQUIRE(fragment_instance.GetAlias() == "none");
  }

  SECTION("Custom alias is preserved") {
    FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket,
                                       "my_alias");
    REQUIRE(fragment_instance.GetAlias() == "my_alias");
  }
}

TEST_CASE("FragmentInstance::GetSockets tests", "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket);

  SECTION("Socket map size matches fragment socket definition") {
    REQUIRE(fragment_instance.GetSockets().size() ==
            parts::FragmentRectangleWithOneSocket.sockets.size());
  }
}

TEST_CASE("FragmentInstance::GetSocketCount tests", "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket);

  SECTION("Returns number of initialized sockets") {
    REQUIRE(fragment_instance.GetSocketCount() ==
            parts::FragmentRectangleWithOneSocket.sockets.size());
  }
}

TEST_CASE("FragmentInstance::TryGetSocket tests", "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket);

  SECTION("Returns nullptr when socket does not exist") {
    REQUIRE(fragment_instance.TryGetSocket(999) == nullptr);
  }

  SECTION("Returns pointer when socket exists") {
    if (fragment_instance.GetSocketCount() == 0) {
      SUCCEED("No sockets defined by fixture; existence case not applicable.");
      return;
    }

    const auto *socket = fragment_instance.TryGetSocket(0);
    REQUIRE(socket != nullptr);
  }
}

TEST_CASE("FragmentInstance::GetNumberOfConnectedSockets tests",
          "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket);
  JointInstance joint_instance(2, parts::JointSquareWithOneSocket);

  SECTION("Initially zero") {
    REQUIRE(fragment_instance.GetNumberOfConnectedSockets() == 0);
    REQUIRE(joint_instance.GetNumberOfConnectedSockets() == 0);
  }

  SECTION("Increments after creating Fragment-Joint connection") {
    if (fragment_instance.GetSocketCount() == 0 ||
        joint_instance.GetSocketCount() == 0) {
      SUCCEED(
          "No sockets defined by fixtures; connection case not applicable.");
      return;
    }

    auto result = fragment_instance.CreateConnectionWithOtherInstance(
        0, joint_instance, 0);
    REQUIRE(result.has_value());
    REQUIRE(fragment_instance.GetNumberOfConnectedSockets() == 1);
    REQUIRE(joint_instance.GetNumberOfConnectedSockets() == 1);
  }
}

TEST_CASE("FragmentInstance::GetSocketLocalPosition tests",
          "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket);

  SECTION("Returns zero vector when socket does not exist") {
    const sf::Vector2f p = fragment_instance.GetSocketLocalPosition(999);
    REQUIRE(Catch::Approx(p.x) == 0.f);
    REQUIRE(Catch::Approx(p.y) == 0.f);
  }

  SECTION("Returns local socket position when socket exists") {
    if (fragment_instance.GetSocketCount() == 0) {
      SUCCEED("No sockets defined by fixture; existence case not applicable.");
      return;
    }

    const sf::Vector2f p = fragment_instance.GetSocketLocalPosition(0);
    const sf::Vector2f expected =
        parts::FragmentRectangleWithOneSocket.sockets[0].local_position;
    REQUIRE(Catch::Approx(p.x) == expected.x);
    REQUIRE(Catch::Approx(p.y) == expected.y);
  }
}

TEST_CASE("FragmentInstance::GetSocketWorldPosition tests",
          "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket);

  SECTION("Returns zero vector when socket does not exist") {
    const sf::Vector2f p = fragment_instance.GetSocketWorldPosition(999);
    REQUIRE(Catch::Approx(p.x) == 0.f);
    REQUIRE(Catch::Approx(p.y) == 0.f);
  }

  SECTION("Applies transform to local socket position") {
    fragment_instance.move(
        {10.f, 20.f}); // apply translation to the fragment instance

    const sf::Vector2f local =
        parts::FragmentRectangleWithOneSocket.sockets[0].local_position;
    const sf::Vector2f p = fragment_instance.GetSocketWorldPosition(0);

    REQUIRE(Catch::Approx(p.x) == local.x + 10.f);
    REQUIRE(Catch::Approx(p.y) == local.y + 20.f);
  }
}

TEST_CASE("FragmentInstance::GetSocketLocalAlignmentVector tests",
          "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket);

  SECTION("Returns zero vector when socket does not exist") {
    const sf::Vector2f v = fragment_instance.GetSocketLocalAlignmentVector(999);
    REQUIRE(Catch::Approx(v.x) == 0.f);
    REQUIRE(Catch::Approx(v.y) == 0.f);
  }

  SECTION("Returns local alignment vector when socket exists") {
    if (fragment_instance.GetSocketCount() == 0) {
      SUCCEED("No sockets defined by fixture; existence case not applicable.");
      return;
    }

    const sf::Vector2f v = fragment_instance.GetSocketLocalAlignmentVector(0);
    const sf::Vector2f expected =
        parts::FragmentRectangleWithOneSocket.sockets[0].alignment_vector;

    REQUIRE(Catch::Approx(v.x) == expected.x);
    REQUIRE(Catch::Approx(v.y) == expected.y);
  }
}

TEST_CASE("FragmentInstance::GetSocketWorldAlignmentVector tests",
          "[FragmentInstance]") {

  SECTION("Returns MissingData error when socket does not exist") {

    FragmentInstance fragment_instance(1,
                                       parts::FragmentRectangleWithOneSocket);

    const auto result = fragment_instance.GetSocketWorldAlignmentVector(999);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().mode == FailMode::MissingData);
  }

  SECTION("Returns local alignment vector modified by the FragmentInstances "
          "rotation") {

    FragmentInstance fragment_instance(0,
                                       parts::FragmentRectangleWithTwoSockets);

    REQUIRE_THAT(fragment_instance.GetSocketLocalAlignmentVector(0),
                 EqualsVector2f({1.f, 0.f}, 0.001f));
    REQUIRE_THAT(fragment_instance.GetSocketLocalAlignmentVector(1),
                 EqualsVector2f({-1.f, 0.f}, 0.001f));

    struct TestCase {
      float rotation_deg;
      sf::Vector2f expected_socket_0_world_alignment;
      sf::Vector2f expected_socket_1_world_alignment;
    };

    std::vector<TestCase> test_cases = {
        {0.f, {1.f, 0.f}, {0.f, 1.f}},
        {45.f, {0.707f, 0.707f}, {-0.707f, 0.707f}},
        {90.f, {0.f, 1.f}, {-1.f, 0.f}},
        {135.f, {-0.707f, 0.707f}, {-0.707f, -0.707f}},
        {180.f, {-1.f, 0.f}, {0.f, -1.f}},
        {225.f, {-0.707f, -0.707f}, {0.707f, -0.707f}},
        {270.f, {0.f, -1.f}, {1.f, 0.f}},
    };

    for (const auto &tc : test_cases) {
      DYNAMIC_SECTION("Rotation " << tc.rotation_deg << " degrees") {
        fragment_instance.rotate(sf::degrees(tc.rotation_deg));
        const auto result_0 =
            fragment_instance.GetSocketWorldAlignmentVector(0);
        REQUIRE(result_0.has_value());
        REQUIRE_THAT(
            result_0.value(),
            EqualsVector2f(tc.expected_socket_0_world_alignment, 0.001f));

        REQUIRE(result_0->length() == Catch::Approx(1.f));
      }
    }
  }
}

TEST_CASE("FragmentInstance::CheckMouseOverSockets tests",
          "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithTwoSockets);
  sf::Vector2f world_mouse(0.f, 0.f);

  const SocketState *socket_0 = fragment_instance.TryGetSocket(0);
  REQUIRE(socket_0);
  const SocketState *socket_1 = fragment_instance.TryGetSocket(1);
  REQUIRE(socket_1);

  REQUIRE_FALSE(socket_0->IsMouseOver());
  REQUIRE_FALSE(socket_1->IsMouseOver());

  // test positions of sockets in world space
  REQUIRE_THAT(fragment_instance.GetSocketWorldPosition(0),
               EqualsVector2f({0.f, 5.f}, 0.001f));
  REQUIRE_THAT(fragment_instance.GetSocketWorldPosition(1),
               EqualsVector2f({50.f, 5.f}, 0.001f));

  SECTION("Does not throw when called") {
    REQUIRE_NOTHROW(fragment_instance.CheckMouseOverSockets({0.f, 0.f}));
    REQUIRE_NOTHROW(fragment_instance.CheckMouseOverSockets({100.f, 100.f}));
  }

  SECTION("Sets mouse-over state for sockets within radius") {
    fragment_instance.CheckMouseOverSockets({0.f, 0.f});
    REQUIRE(socket_0->IsMouseOver());
    REQUIRE_FALSE(socket_1->IsMouseOver());
  }

  SECTION("Clears mouse-over state for sockets outside radius") {
    fragment_instance.CheckMouseOverSockets({0.f, 0.f});
    REQUIRE(socket_0->IsMouseOver());
    REQUIRE_FALSE(socket_1->IsMouseOver());

    fragment_instance.CheckMouseOverSockets({100.f, 100.f});
    REQUIRE_FALSE(socket_0->IsMouseOver());
    REQUIRE_FALSE(socket_1->IsMouseOver());
  }

  SECTION("moving the fragment instance updates mouse-over state") {
    fragment_instance.CheckMouseOverSockets({0.f, 0.f});
    REQUIRE(socket_0->IsMouseOver());
    REQUIRE_FALSE(socket_1->IsMouseOver());

    fragment_instance.move({50.f, 0.f});
    fragment_instance.CheckMouseOverSockets({0.f, 0.f});
    REQUIRE_FALSE(socket_0->IsMouseOver());
    REQUIRE_FALSE(socket_1->IsMouseOver());
  }
}

TEST_CASE("FragmentInstance::CheckIfSocketIsAvailable tests",
          "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket);
  JointInstance joint_instance(2, parts::JointSquareWithOneSocket);

  SECTION("False when socket does not exist") {
    REQUIRE_FALSE(fragment_instance.CheckIfSocketIsAvailable(999));
  }

  SECTION("True when socket exists and is unconnected") {
    if (fragment_instance.GetSocketCount() == 0) {
      SUCCEED("No sockets defined by fixture; availability case not "
              "applicable.");
      return;
    }
    REQUIRE(fragment_instance.CheckIfSocketIsAvailable(0));
  }

  SECTION("False after connection is created") {
    if (fragment_instance.GetSocketCount() == 0 ||
        joint_instance.GetSocketCount() == 0) {
      SUCCEED("No sockets defined by fixtures; connection case not "
              "applicable.");
      return;
    }

    auto result = fragment_instance.CreateConnectionWithOtherInstance(
        0, joint_instance, 0);
    REQUIRE(result.has_value());
    REQUIRE_FALSE(fragment_instance.CheckIfSocketIsAvailable(0));
  }
}

TEST_CASE("FragmentInstance::CheckIfAnySocketIsAvailable tests",
          "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket);
  JointInstance joint_instance(2, parts::JointSquareWithOneSocket);

  SECTION("Returns available socket id when one exists") {
    const auto result = fragment_instance.CheckIfAnySocketIsAvailable();
    if (fragment_instance.GetSocketCount() == 0) {
      REQUIRE_FALSE(result.has_value());
    } else {
      REQUIRE(result.has_value());
      REQUIRE(result.value() < fragment_instance.GetSocketCount());
    }
  }

  SECTION("Returns nullopt when all sockets are connected") {
    if (fragment_instance.GetSocketCount() == 0 ||
        joint_instance.GetSocketCount() == 0) {
      SUCCEED("No sockets defined by fixtures; all-connected case not "
              "applicable.");
      return;
    }

    auto result = fragment_instance.CreateConnectionWithOtherInstance(
        0, joint_instance, 0);
    REQUIRE(result.has_value());

    REQUIRE_FALSE(fragment_instance.CheckIfAnySocketIsAvailable().has_value());
  }
}

TEST_CASE("FragmentInstance::ResetAllSocketsInteractionState tests",
          "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket);

  SECTION("Clears distance/mouse-over interaction fields on all sockets") {

    auto *socket =
        const_cast<FragmentSocketState *>(fragment_instance.TryGetSocket(0));
    REQUIRE(socket != nullptr);

    socket->SetDistanceToNearestSocket(1.0f);
    socket->SetMouseOver(true);
    REQUIRE(socket->GetDistanceToNearestSocket().has_value());
    REQUIRE(socket->IsMouseOver());

    fragment_instance.ResetAllSocketsInteractionState();

    REQUIRE_FALSE(socket->GetDistanceToNearestSocket().has_value());
    REQUIRE_FALSE(socket->IsMouseOver());
  }
}

TEST_CASE("FragmentInstance::ResetAllSocketState tests", "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket);
  JointInstance joint_instance(2, parts::JointSquareWithOneSocket);

  SECTION("Clears connection and interaction fields on all sockets") {
    if (fragment_instance.GetSocketCount() == 0 ||
        joint_instance.GetSocketCount() == 0) {
      SUCCEED("No sockets defined by fixtures; reset case not applicable.");
      return;
    }

    auto result = fragment_instance.CreateConnectionWithOtherInstance(
        0, joint_instance, 0);
    REQUIRE(result.has_value());
    REQUIRE(fragment_instance.GetNumberOfConnectedSockets() == 1);

    fragment_instance.ResetAllSocketState();

    REQUIRE(fragment_instance.GetNumberOfConnectedSockets() == 0);
  }
}

TEST_CASE("FragmentInstance::CheckWithOtherInstanceForCollision tests",
          "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket);
  JointInstance joint_instance(2, parts::JointSquareWithOneSocket);

  SECTION("Does not throw for Fragment-Joint collision checks") {
    REQUIRE_NOTHROW(
        fragment_instance.CheckWithOtherInstanceForCollision(joint_instance));
  }
}

TEST_CASE("FragmentInstance::CreateConnectionWithOtherInstance tests",
          "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket);
  JointInstance joint_instance(2, parts::JointSquareWithOneSocket);

  SECTION("Returns MissingData when this socket is invalid") {
    const auto result = fragment_instance.CreateConnectionWithOtherInstance(
        999, joint_instance, 0);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().mode == FailMode::MissingData);
  }

  SECTION("Returns MissingData when other socket is invalid") {
    if (fragment_instance.GetSocketCount() == 0) {
      SUCCEED("No fragment sockets; invalid-other case not applicable.");
      return;
    }

    const auto result = fragment_instance.CreateConnectionWithOtherInstance(
        0, joint_instance, 999);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().mode == FailMode::MissingData);
  }

  SECTION("Creates reciprocal Fragment-Joint connection on success") {
    if (fragment_instance.GetSocketCount() == 0 ||
        joint_instance.GetSocketCount() == 0) {
      SUCCEED("No sockets defined by fixtures; success case not applicable.");
      return;
    }

    const auto result = fragment_instance.CreateConnectionWithOtherInstance(
        0, joint_instance, 0);
    REQUIRE(result.has_value());

    const auto *fragment_socket = fragment_instance.TryGetSocket(0);
    const auto *joint_socket = joint_instance.TryGetSocket(0);
    REQUIRE(fragment_socket != nullptr);
    REQUIRE(joint_socket != nullptr);

    REQUIRE(fragment_socket->GetConnection().has_value());
    REQUIRE(joint_socket->GetConnection().has_value());

    REQUIRE(fragment_socket->GetConnection()->peer_part_id ==
            joint_instance.GetId());
    REQUIRE(fragment_socket->GetConnection()->peer_socket_id == 0u);

    REQUIRE(joint_socket->GetConnection()->peer_part_id ==
            fragment_instance.GetId());
    REQUIRE(joint_socket->GetConnection()->peer_socket_id == 0u);
  }

  SECTION("Returns InvalidState when attempting duplicate connection") {
    if (fragment_instance.GetSocketCount() == 0 ||
        joint_instance.GetSocketCount() == 0) {
      SUCCEED("No sockets defined by fixtures; invalid-state case not "
              "applicable.");
      return;
    }

    auto first = fragment_instance.CreateConnectionWithOtherInstance(
        0, joint_instance, 0);
    REQUIRE(first.has_value());

    auto second = fragment_instance.CreateConnectionWithOtherInstance(
        0, joint_instance, 0);
    REQUIRE_FALSE(second.has_value());
    REQUIRE(second.error().mode == FailMode::InvalidState);
  }
}

TEST_CASE("FragmentInstance::CheckForFirstConnectionWithOtherInstance tests",
          "[FragmentInstance]") {
  SECTION("Returns nullopt when checking self") {
    FragmentInstance fragment_instance(1,
                                       parts::FragmentRectangleWithOneSocket);

    const auto result =
        fragment_instance.CheckForFirstConnectionWithOtherInstance(
            fragment_instance);

    REQUIRE_FALSE(result.has_value());
  }

  SECTION("Returns nullopt when other instance has same id") {
    FragmentInstance a(42, parts::FragmentRectangleWithOneSocket);
    JointInstance b(42,
                    parts::JointSquareWithOneSocket); // same ID, different type

    const auto result = a.CheckForFirstConnectionWithOtherInstance(b);
    REQUIRE_FALSE(result.has_value());
  }

  SECTION("Returns nullopt when no connection exists") {
    FragmentInstance fragment_instance(1,
                                       parts::FragmentRectangleWithOneSocket);
    JointInstance joint_instance(2, parts::JointSquareWithOneSocket);

    const auto result =
        fragment_instance.CheckForFirstConnectionWithOtherInstance(
            joint_instance);

    REQUIRE_FALSE(result.has_value());
  }

  SECTION("Returns first existing valid reciprocal connection") {
    FragmentInstance fragment_instance(1,
                                       parts::FragmentRectangleWithOneSocket);
    JointInstance joint_instance(2, parts::JointSquareWithOneSocket);

    if (fragment_instance.GetSocketCount() == 0 ||
        joint_instance.GetSocketCount() == 0) {
      FAIL("No sockets defined by fixtures; connection case not "
           "applicable.");
    }

    auto create_result = fragment_instance.CreateConnectionWithOtherInstance(
        0, joint_instance, 0);
    REQUIRE(create_result.has_value());

    const auto check_result =
        fragment_instance.CheckForFirstConnectionWithOtherInstance(
            joint_instance);

    REQUIRE(check_result.has_value());
    const auto &connection = check_result.value();
    REQUIRE(connection.this_id == fragment_instance.GetId());
    REQUIRE(connection.this_socket_id == 0u);
    REQUIRE(connection.other_id == joint_instance.GetId());
    REQUIRE(connection.other_socket_id == 0u);
  }

  SECTION("Symmetry: if A finds B, B finds A with swapped endpoints") {
    FragmentInstance a(10, parts::FragmentRectangleWithOneSocket);
    JointInstance b(20, parts::JointSquareWithOneSocket);

    if (a.GetSocketCount() == 0 || b.GetSocketCount() == 0) {
      FAIL("No sockets defined by fixtures; symmetry case not applicable.");
    }

    REQUIRE(a.CreateConnectionWithOtherInstance(0, b, 0).has_value());

    const auto a_to_b = a.CheckForFirstConnectionWithOtherInstance(b);
    const auto b_to_a = b.CheckForFirstConnectionWithOtherInstance(a);

    REQUIRE(a_to_b.has_value());
    REQUIRE(b_to_a.has_value());

    REQUIRE(a_to_b->this_id == a.GetId());
    REQUIRE(a_to_b->other_id == b.GetId());
    REQUIRE(a_to_b->this_socket_id == 0u);
    REQUIRE(a_to_b->other_socket_id == 0u);

    REQUIRE(b_to_a->this_id == b.GetId());
    REQUIRE(b_to_a->other_id == a.GetId());
    REQUIRE(b_to_a->this_socket_id == 0u);
    REQUIRE(b_to_a->other_socket_id == 0u);
  }

  SECTION("Returns nullopt when connection exists but points to different "
          "part") {
    // Requires fixture types with >= 1 socket each and ability to form
    // multiple links.
    FragmentInstance a(1, parts::FragmentRectangleWithOneSocket);
    JointInstance b(2, parts::JointSquareWithOneSocket);
    JointInstance c(3, parts::JointSquareWithOneSocket);

    if (a.GetSocketCount() == 0 || b.GetSocketCount() == 0 ||
        c.GetSocketCount() == 0) {
      FAIL("No sockets defined by fixtures; mismatch case not applicable.");
    }

    // Connect A to C, not B.
    REQUIRE(a.CreateConnectionWithOtherInstance(0, c, 0).has_value());

    const auto result = a.CheckForFirstConnectionWithOtherInstance(b);
    REQUIRE_FALSE(result.has_value());
  }

  // Optional: only include if you have fixtures with multiple sockets.
  SECTION("When multiple valid links exist, returns one valid link to target "
          "instance") {
    FragmentInstance a(
        100, parts::FragmentRectangleWithTwoSockets); // adjust fixture name
    JointInstance b(200,
                    parts::JointSquareWithTwoSockets); // adjust fixture name

    if (a.GetSocketCount() < 2 || b.GetSocketCount() < 2) {
      SUCCEED("Fixture does not provide >=2 sockets; skipping multi-link "
              "behavior check.");
      return;
    }

    REQUIRE(a.CreateConnectionWithOtherInstance(0, b, 1).has_value());
    REQUIRE(a.CreateConnectionWithOtherInstance(1, b, 0).has_value());

    const auto result = a.CheckForFirstConnectionWithOtherInstance(b);
    REQUIRE(result.has_value());

    // Assert result is one of the valid pairs (depends on socket iteration
    // order).
    const bool is_first_pair =
        result->this_socket_id == 0u && result->other_socket_id == 1u;
    const bool is_second_pair =
        result->this_socket_id == 1u && result->other_socket_id == 0u;

    REQUIRE(result->this_id == a.GetId());
    REQUIRE(result->other_id == b.GetId());
    REQUIRE((is_first_pair || is_second_pair));
  }
}

TEST_CASE("FragmentInstance::AlignOntoOtherPartInstance tests",
          "[FragmentInstance]") {
  // Arrange
  FragmentInstance fragment_instance{0, parts::FragmentRectangleWithTwoSockets};

  REQUIRE_THAT(fragment_instance.GetSocketLocalPosition(0),
               steamrot::tests::EqualsVector2f({0.f, 5.f}, 0.001f));
  REQUIRE_THAT(fragment_instance.GetSocketLocalPosition(1),
               steamrot::tests::EqualsVector2f({50.f, 5.f}, 0.001f));

  JointInstance joint_instance{1, parts::JointSquareWithTwoSockets};
  joint_instance.PositionSockets(
      JointSocketPositioningStrategy::MaximizeDistance);

  // check the positions of the sockets
  REQUIRE_THAT(joint_instance.GetSocketLocalPosition(0),
               steamrot::tests::EqualsVector2f({23.f, 10.f}, 0.001f));
  REQUIRE_THAT(joint_instance.GetSocketLocalPosition(1),
               steamrot::tests::EqualsVector2f({10.f, 23.f}, 0.001f));

  auto normalize_degrees_0_360 = [](float deg) {
    float d = std::fmod(deg, 360.f);
    if (d < 0.f)
      d += 360.f; // map negatives into [0,360)
    if (std::fabs(d) < 1e-4f || std::fabs(d - 360.f) < 1e-4f)
      d = 0.f;
    d = std::round(d * 10.f) / 10.f;
    return d;
  };

  SECTION("Returns Badvalue if this socket id is invalid") {
    const auto result =
        fragment_instance.AlignOntoOtherPartInstance(999, joint_instance, 0);

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().mode == FailMode::BadValue);
    REQUIRE(result.error().message ==
            "socket_id 999 does not exist on part instance 0");
  }

  SECTION("Returns Badvalue if other socket id is invalid") {
    const auto result =
        fragment_instance.AlignOntoOtherPartInstance(0, joint_instance, 999);

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().mode == FailMode::BadValue);
    REQUIRE(result.error().message ==
            "socket_id 999 does not exist on part instance 1");
  }

  SECTION("Returns InvalidState if no connection exists between fragment and "
          "joint") {
    const auto result =
        fragment_instance.AlignOntoOtherPartInstance(0, joint_instance, 0);

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().mode == FailMode::InvalidState);
    REQUIRE(result.error().message ==
            "no connection exists between part instance 0 and part instance 1");
  }

  SECTION("Alignment cases (table-driven)") {
    struct AlignmentCase {
      const char *name;
      uint32_t fragment_socket_id;
      uint32_t joint_socket_id;
      float joint_rotation_deg;
      float expected_fragment_rotation_deg;

      // expected world positions AFTER AlignOntoOtherPartInstance
      sf::Vector2f expected_fragment_socket_0_world_after_align;
      sf::Vector2f expected_fragment_socket_1_world_after_align;

      // expected joint world positions (arranged by joint rotation; should
      // not change)
      sf::Vector2f expected_joint_socket_0_world;
      sf::Vector2f expected_joint_socket_1_world;
      sf::Vector2f expected_joint_socket_pivot_world;

      float position_tolerance;
    };

    const std::vector<AlignmentCase> cases{
        {"joint 0°, connect frag[0] -> joint[0]",
         0,
         0,
         0.f,
         0.f,
         {13.f, 0.f},
         {63.f, 0.f},
         {13.f, 0.f},
         {0.f, 13.f},
         {0.f, 0.f},
         0.01f},
        {"joint 0°, connect frag[0] -> joint[1]",
         0,
         1,
         0.f,
         90.f,
         {0.f, 13.f},
         {0.f, 63.f},
         {13.f, 0.f},
         {0.f, 13.f},
         {0.f, 0.f},
         0.01f},

        {"joint 90°, connect frag[0] -> joint[0]",
         0,
         0,
         90.f,
         90.f,
         {0.f, 13.f},
         {0.f, 63.f},
         {0.f, 13.f},
         {-13.f, 0.f},
         {0.f, 0.f},
         0.01f},
        {"joint 90°, connect frag[0] -> joint[1]",
         0,
         1,
         90.f,
         -180.f,
         {-13.f, 0.f},
         {-63.f, 0.f},
         {0.f, 13.f},
         {-13.f, 0.f},
         {0.f, 0.f},
         0.01f},

        {"joint 180°, connect frag[0] -> joint[0]",
         0,
         0,
         180.f,
         -180.f,
         {-13.f, 0.f},
         {-63.f, 0.f},
         {-13.f, 0.f},
         {0.f, -13.f},
         {0.f, 0.f},
         0.01f},
        {"joint 270°, connect frag[0] -> joint[0]",
         0,
         0,
         270.f,
         -90.f,
         {0.f, -13.f},
         {0.f, -63.f},
         {0.f, -13.f},
         {13.f, 0.f},
         {0.f, 0.f},
         0.01f},

        {"joint 0°, connect frag[1] -> joint[0]",
         1,
         0,
         0.f,
         180.f,
         {63.f, 0.f},
         {13.f, 0.f},
         {13.f, 0.f},
         {0.f, 13.f},
         {0.f, 0.f},
         0.01f},
        {"joint 0°, connect frag[1] -> joint[1]",
         1,
         1,
         0.f,
         -90.f,
         {0.f, 63.f},
         {0.f, 13.f},
         {13.f, 0.f},
         {0.f, 13.f},
         {0.f, 0.f},
         0.01f},

        {"joint 90°, connect frag[1] -> joint[0]",
         1,
         0,
         90.f,
         -90.f,
         {0.f, 63.f},
         {0.f, 13.f},
         {0.f, 13.f},
         {-13.f, 0.f},
         {0.f, 0.f},
         0.01f},
        {"joint 270°, connect frag[1] -> joint[1]",
         1,
         1,
         270.f,
         180.f,
         {63.f, 0.f},
         {13.f, 0.f},
         {0.f, -13.f},
         {13.f, 0.f},
         {0.f, 0.f},
         0.01f},
    };

    for (const auto &tc : cases) {
      DYNAMIC_SECTION(tc.name) {
        // reset per-case state so cases are isolated
        fragment_instance.setPosition({0.f, 0.f});
        fragment_instance.setRotation(sf::degrees(0.f));
        joint_instance.setPosition({0.f, 0.f});
        joint_instance.setRotation(sf::degrees(0.f));
        joint_instance.PositionSockets(
            JointSocketPositioningStrategy::MaximizeDistance);

        // ARRANGE //
        if (tc.joint_rotation_deg != 0.f) {
          joint_instance.rotate(sf::degrees(tc.joint_rotation_deg));
        }

        auto connection_result =
            fragment_instance.CreateConnectionWithOtherInstance(
                tc.fragment_socket_id, joint_instance, tc.joint_socket_id);

        if (!connection_result.has_value()) {
          FAIL("Failed to create connection between fragment and joint");
        }

        // sanity check joint expected world positions before ACT
        REQUIRE_THAT(joint_instance.GetSocketWorldPosition(0),
                     EqualsVector2f(tc.expected_joint_socket_0_world,
                                    tc.position_tolerance));
        REQUIRE_THAT(joint_instance.GetSocketWorldPosition(1),
                     EqualsVector2f(tc.expected_joint_socket_1_world,
                                    tc.position_tolerance));
        REQUIRE_THAT(joint_instance.GetSocketPivotWorldPosition(),
                     EqualsVector2f(tc.expected_joint_socket_pivot_world,
                                    tc.position_tolerance));

        // ACT //
        const auto align_result = fragment_instance.AlignOntoOtherPartInstance(
            tc.fragment_socket_id, joint_instance, tc.joint_socket_id);

        if (!align_result.has_value()) {
          FAIL(align_result.error().message);
        }

        // ASSERT //
        REQUIRE_THAT(
            fragment_instance.GetSocketWorldPosition(0),
            EqualsVector2f(tc.expected_fragment_socket_0_world_after_align,
                           tc.position_tolerance));

        REQUIRE_THAT(
            fragment_instance.GetSocketWorldPosition(1),
            EqualsVector2f(tc.expected_fragment_socket_1_world_after_align,
                           tc.position_tolerance));

        REQUIRE(normalize_degrees_0_360(
                    fragment_instance.getRotation().asDegrees()) ==
                Catch::Approx(
                    normalize_degrees_0_360(tc.expected_fragment_rotation_deg))
                    .margin(0.1f));

        // joint should remain unchanged by fragment alignment

        REQUIRE_THAT(joint_instance.GetSocketWorldPosition(0),
                     EqualsVector2f(tc.expected_joint_socket_0_world,
                                    tc.position_tolerance));
        REQUIRE_THAT(joint_instance.GetSocketWorldPosition(1),
                     EqualsVector2f(tc.expected_joint_socket_1_world,
                                    tc.position_tolerance));

        // connected socket world positions should coincide
        const auto fragment_connected_socket_world =
            fragment_instance.GetSocketWorldPosition(tc.fragment_socket_id);
        const auto joint_connected_socket_world =
            joint_instance.GetSocketWorldPosition(tc.joint_socket_id);

        REQUIRE_THAT(fragment_connected_socket_world,
                     EqualsVector2f(joint_connected_socket_world, 0.001f));
      }
    }
  }
}
TEST_CASE("FragmentInstance::DrawSockets tests", "[unit][FragmentInstance]") {

  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  SECTION("FragmentRectangleWithTwoSockets") {
    FragmentInstance fragment_instance(1,
                                       parts::FragmentRectangleWithOneSocket);

    SECTION("DrawSockets produces white pixels at world_pos when not "
            "hovered/proximal") {

      // Arrange
      fragment_instance.ResetAllSocketsInteractionState();

      // Act
      fragment_instance.DrawSockets(texture);
      texture.display();

      const sf::Image image = texture.getTexture().copyToImage();

      // Assert
      for (const auto &[socket_id, socket_state] :
           fragment_instance.GetSockets()) {
        const sf::Vector2f world_pos =
            fragment_instance.GetSocketWorldPosition(socket_id);
        REQUIRE_THAT(image.getPixel({world_pos}),
                     ColorEqualsMatcher(sf::Color::White));
      }
    }

    SECTION(
        "DrawSockets produces blue pixels at world pos when mouse is over") {
      // Arrange
      fragment_instance.ResetAllSocketsInteractionState();
      fragment_instance.CheckMouseOverSockets({0.f, 0.f});
      // Act
      fragment_instance.DrawSockets(texture);
      texture.display();
      const sf::Image image = texture.getTexture().copyToImage();
      // Assert
      for (const auto &[socket_id, socket_state] :
           fragment_instance.GetSockets()) {
        const sf::Vector2f world_pos =
            fragment_instance.GetSocketWorldPosition(socket_id);
        if (socket_state.IsMouseOver()) {
          REQUIRE_THAT(image.getPixel({world_pos}),
                       ColorEqualsMatcher(sf::Color::Blue));
        } else {
          REQUIRE_THAT(image.getPixel({world_pos}),
                       ColorEqualsMatcher(sf::Color::White));
        }
      }
    }

    SECTION("DrawSockets produces green pixels at world pos when within "
            "connection distance") {
      // Arrange
      fragment_instance.ResetAllSocketsInteractionState();
      JointInstance joint_instance(2, parts::JointSquareWithOneSocket);

      // this should produce the socket at 9.19,9.19
      joint_instance.PositionSockets(
          JointSocketPositioningStrategy::MaximizeDistance);
      // move the joint instance to be near the fragment instance so that the
      // sockets are within connection distance
      joint_instance.move(
          {-9.f, -5.f}); // move the other fragment to be near the first
      fragment_instance.CheckWithOtherInstanceForCollision(joint_instance);

      // Act
      fragment_instance.DrawSockets(texture);
      texture.display();
      const sf::Image image = texture.getTexture().copyToImage();

      // Assert
      // both position for the fragment_socket 0 and joint socket 0 should be
      // green
      REQUIRE_THAT(
          image.getPixel({fragment_instance.GetSocketWorldPosition(0)}),
          ColorEqualsMatcher(sf::Color::Green));
      REQUIRE_THAT(image.getPixel({joint_instance.GetSocketWorldPosition(0)}),
                   ColorEqualsMatcher(sf::Color::Green));
    }

    SECTION("DrawSockets produces blue pixels at world pos when within "
            "proximal distance but not connections distance") {
      // Arrange
      fragment_instance.ResetAllSocketsInteractionState();
      JointInstance joint_instance(2, parts::JointSquareWithTwoSockets);
      joint_instance.PositionSockets(
          JointSocketPositioningStrategy::MaximizeDistance);
      joint_instance.move({-7, -3});
      fragment_instance.CheckWithOtherInstanceForCollision(joint_instance);

      // Act
      fragment_instance.DrawSockets(texture);
      texture.display();
      const sf::Image image = texture.getTexture().copyToImage();

      // Assert
      // both positions for the fragment_socket 0 and joint socket 0 should be
      // some modified blue
      // [TODO:] modify colour with brightness to check
    }
  }
}

TEST_CASE("FragmentInstance::DrawInstance tests", "[unit][FragmentInstance]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  FragmentInstance fragment_instance(0, parts::FragmentRectangleWithTwoSockets);

  SECTION(
      "DrawInstance produces non-black pixels at fragment's world position") {
    // Arrange
    fragment_instance.setPosition({10.f, 10.f});
    // Act
    fragment_instance.DrawInstance(texture, false);
    texture.display();
    const sf::Image image = texture.getTexture().copyToImage();
    // Assert
    // Check that the pixel at the fragment's world position is white

    const sf::Vector2f world_pos = fragment_instance.getPosition();
    REQUIRE_THAT(image.getPixel({15, 15}),
                 ColorEqualsMatcher(sf::Color::White));

    // this could do with some more fleshing out
  }
}

} // namespace steamrot::tests
