
/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the JointInstance class and inherited PartInstance
/// behavior.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "JointInstance.h"
#include "FragmentInstance.h"
#include "SocketState.h"
#include "Vector2fEqualsMatcher.h"
#include "catch2/catch_approx.hpp"
#include "fragment_library.h"
#include "joint_library.h"
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Angle.hpp>
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

TEST_CASE("JointInstance::JointInstance tests", "[JointInstance]") {
  SECTION("Construct with id and joint") {
    JointInstance instance{1, parts::JointSquareWithOneSocket};

    REQUIRE(instance.GetId() == 1u);
    REQUIRE(instance.GetAlias() == "none");
    REQUIRE(&instance.GetPart() == &parts::JointSquareWithOneSocket);
    REQUIRE(instance.GetSocketCount() ==
            parts::JointSquareWithOneSocket.socket_config.socket_count);
    REQUIRE(Catch::Approx(instance.GetTotalRotation().asDegrees()) == 0.f);
  }

  SECTION("Construct with id, joint, and alias") {
    JointInstance instance{7, parts::JointSquareWithOneSocket, "joint_A"};

    REQUIRE(instance.GetId() == 7u);
    REQUIRE(instance.GetAlias() == "joint_A");
    REQUIRE(&instance.GetPart() == &parts::JointSquareWithOneSocket);
    REQUIRE(instance.GetSocketCount() ==
            parts::JointSquareWithOneSocket.socket_config.socket_count);
  }
}

TEST_CASE("JointInstance::GetPart tests", "[JointInstance]") {
  JointInstance instance{1, parts::JointSquareWithOneSocket};

  SECTION("Returns referenced joint") {
    REQUIRE(&instance.GetPart() == &parts::JointSquareWithOneSocket);
  }
}

TEST_CASE("JointInstance::GetId tests", "[JointInstance]") {
  JointInstance instance{42, parts::JointSquareWithOneSocket};

  SECTION("Returns stable id") { REQUIRE(instance.GetId() == 42u); }
}

TEST_CASE("JointInstance::GetAlias tests", "[JointInstance]") {
  SECTION("Default alias is none") {
    JointInstance instance{1, parts::JointSquareWithOneSocket};
    REQUIRE(instance.GetAlias() == "none");
  }

  SECTION("Custom alias is preserved") {
    JointInstance instance{1, parts::JointSquareWithOneSocket,
                           "my_joint_alias"};
    REQUIRE(instance.GetAlias() == "my_joint_alias");
  }
}

TEST_CASE("JointInstance::GetSockets tests", "[JointInstance]") {
  JointInstance instance{1, parts::JointSquareWithOneSocket};

  SECTION("Socket map size matches joint socket config") {
    REQUIRE(instance.GetSockets().size() ==
            parts::JointSquareWithOneSocket.socket_config.socket_count);
  }
}

TEST_CASE("JointInstance::GetSocketCount tests", "[JointInstance]") {
  JointInstance instance{1, parts::JointSquareWithOneSocket};

  SECTION("Returns number of initialized sockets") {
    REQUIRE(instance.GetSocketCount() ==
            parts::JointSquareWithOneSocket.socket_config.socket_count);
  }
}

TEST_CASE("JointInstance::GetTransform and SetTransform tests",
          "[JointInstance]") {
  JointInstance instance{1, parts::JointSquareWithOneSocket};

  SECTION("Default transform is identity") {
    const sf::Vector2f out = instance.GetTransform().transformPoint({3.f, 4.f});
    REQUIRE(Catch::Approx(out.x) == 3.f);
    REQUIRE(Catch::Approx(out.y) == 4.f);
  }

  SECTION("SetTransform replaces transform") {
    sf::Transform t;
    t.translate({5.f, 10.f});
    instance.SetTransform(t);

    const sf::Vector2f out = instance.GetTransform().transformPoint({1.f, 2.f});
    REQUIRE(Catch::Approx(out.x) == 6.f);
    REQUIRE(Catch::Approx(out.y) == 12.f);
  }
}

TEST_CASE("JointInstance::GetTotalRotation and SetTotalRotation tests",
          "[JointInstance]") {
  JointInstance instance{1, parts::JointSquareWithOneSocket};

  SECTION("Initial total rotation is 0 degrees") {
    REQUIRE(Catch::Approx(instance.GetTotalRotation().asDegrees()) == 0.f);
  }

  SECTION("SetTotalRotation replaces tracked rotation") {
    instance.SetTotalRotation(sf::degrees(45.f));
    REQUIRE(Catch::Approx(instance.GetTotalRotation().asDegrees()) == 45.f);
  }
}

TEST_CASE("JointInstance::AddToTotalRotation tests", "[JointInstance]") {
  JointInstance instance{1, parts::JointSquareWithOneSocket};

  SECTION("Accumulates rotation values") {
    instance.AddToTotalRotation(sf::degrees(15.f));
    REQUIRE(Catch::Approx(instance.GetTotalRotation().asDegrees()) == 15.f);

    instance.AddToTotalRotation(sf::degrees(20.f));
    REQUIRE(Catch::Approx(instance.GetTotalRotation().asDegrees()) == 35.f);

    instance.AddToTotalRotation(sf::degrees(-50.f));
    REQUIRE(Catch::Approx(instance.GetTotalRotation().asDegrees()) == -15.f);
  }
}

TEST_CASE("JointInstance::TryGetSocket tests", "[JointInstance]") {
  JointInstance instance{1, parts::JointSquareWithOneSocket};

  SECTION("Returns nullptr when socket does not exist") {
    REQUIRE(instance.TryGetSocket(999) == nullptr);
  }

  SECTION("Returns pointer when socket exists") {
    if (instance.GetSocketCount() == 0) {
      SUCCEED("No sockets defined by fixture; existence case not applicable.");
      return;
    }

    const auto *socket = instance.TryGetSocket(0);
    REQUIRE(socket != nullptr);
  }
}

TEST_CASE("JointInstance::GetNumberOfConnectedSockets tests",
          "[JointInstance]") {
  JointInstance joint_instance{1, parts::JointSquareWithOneSocket};
  FragmentInstance fragment_instance{2, parts::FragmentRectangleWithOneSocket};

  SECTION("Initially zero") {
    REQUIRE(joint_instance.GetNumberOfConnectedSockets() == 0u);
    REQUIRE(fragment_instance.GetNumberOfConnectedSockets() == 0u);
  }

  SECTION("Increments after creating Joint-Fragment connection") {
    if (joint_instance.GetSocketCount() == 0 ||
        fragment_instance.GetSocketCount() == 0) {
      SUCCEED(
          "No sockets defined by fixtures; connection case not applicable.");
      return;
    }

    auto result = joint_instance.CreateConnectionWithOtherInstance(
        0, fragment_instance, 0);
    REQUIRE(result.has_value());
    REQUIRE(joint_instance.GetNumberOfConnectedSockets() == 1u);
    REQUIRE(fragment_instance.GetNumberOfConnectedSockets() == 1u);
  }
}

TEST_CASE("JointInstance::GetSocketLocalPosition tests", "[JointInstance]") {
  JointInstance instance{1, parts::JointSquareWithOneSocket};

  SECTION("Returns zero vector when socket does not exist") {
    const sf::Vector2f p = instance.GetSocketLocalPosition(999);
    REQUIRE(Catch::Approx(p.x) == 0.f);
    REQUIRE(Catch::Approx(p.y) == 0.f);
  }

  SECTION("Returns local position when socket exists") {
    if (instance.GetSocketCount() == 0) {
      SUCCEED("No sockets defined by fixture; existence case not applicable.");
      return;
    }

    const sf::Vector2f p = instance.GetSocketLocalPosition(0);
    const sf::Vector2f expected = instance.TryGetSocket(0)->GetLocalPosition();
    REQUIRE(Catch::Approx(p.x) == expected.x);
    REQUIRE(Catch::Approx(p.y) == expected.y);
  }
}

TEST_CASE("JointInstance::GetSocketWorldPosition tests", "[JointInstance]") {
  JointInstance instance{1, parts::JointSquareWithOneSocket};

  SECTION("Returns zero vector when socket does not exist") {
    const sf::Vector2f p = instance.GetSocketWorldPosition(999);
    REQUIRE(Catch::Approx(p.x) == 0.f);
    REQUIRE(Catch::Approx(p.y) == 0.f);
  }

  SECTION("Applies transform to local socket position") {
    if (instance.GetSocketCount() == 0) {
      SUCCEED("No sockets defined by fixture; existence case not applicable.");
      return;
    }

    sf::Transform t;
    t.translate({10.f, 20.f});
    instance.SetTransform(t);

    const sf::Vector2f local = instance.TryGetSocket(0)->GetLocalPosition();
    const sf::Vector2f p = instance.GetSocketWorldPosition(0);

    REQUIRE(Catch::Approx(p.x) == local.x + 10.f);
    REQUIRE(Catch::Approx(p.y) == local.y + 20.f);
  }
}

TEST_CASE("JointInstance::GetSocketWorldAlignmentVector tests",
          "[JointInstance]") {
  JointInstance instance{1, parts::JointSquareWithOneSocket};
  instance.PositionSockets(JointSocketPositioningStrategy::MaximizeDistance);

  SECTION("If socket does not exist, returns MissingData") {
    const auto result = instance.GetSocketWorldAlignmentVector(999);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().mode == FailMode::MissingData);
    REQUIRE(result.error().message == "Socket ID 999 does not exist.");
  }

  SECTION("Returns normalised world alignment vector when socket exists") {

    // the pivot position is at {10, 10} and the socket should be at
    // (45 degrees) after maximise_joint_socket_spread, so the
    // alignment vector should be {0.717, 0.717} as we are going straight down
    // from the pivot to the socket
    //
    // changin rotation should change the world alignment vector
    SECTION("Total rotation = 0 degrees") {
      instance.SetTotalRotation(sf::degrees(0.f));
      const auto result = instance.GetSocketWorldAlignmentVector(0);

      if (!result.has_value())
        FAIL(result.error().message);

      const sf::Vector2f &world_alignment = result.value();

      REQUIRE(world_alignment.length() == Catch::Approx(1.f));
      REQUIRE_THAT(world_alignment, EqualsVector2f({0.707f, 0.707f}, 0.001f));
    }

    SECTION("Total rotation = 45 degrees") {
      instance.SetTotalRotation(sf::degrees(45.f));
      const auto result = instance.GetSocketWorldAlignmentVector(0);
      if (!result.has_value())
        FAIL(result.error().message);
      const sf::Vector2f &world_alignment = result.value();
      REQUIRE(world_alignment.length() == Catch::Approx(1.f));
      REQUIRE_THAT(world_alignment, EqualsVector2f({0.f, 1.f}, 0.001f));
    }
    SECTION("Total rotation = 90 degrees") {
      instance.SetTotalRotation(sf::degrees(90.f));
      const auto result = instance.GetSocketWorldAlignmentVector(0);
      if (!result.has_value())
        FAIL(result.error().message);
      const sf::Vector2f &world_alignment = result.value();
      REQUIRE(world_alignment.length() == Catch::Approx(1.f));
      REQUIRE_THAT(world_alignment, EqualsVector2f({-0.707f, 0.707f}, 0.001f));
    }
    SECTION("Total rotation = 180 degrees") {
      instance.SetTotalRotation(sf::degrees(180.f));
      const auto result = instance.GetSocketWorldAlignmentVector(0);
      if (!result.has_value())
        FAIL(result.error().message);
      const sf::Vector2f &world_alignment = result.value();
      REQUIRE(world_alignment.length() == Catch::Approx(1.f));
      REQUIRE_THAT(world_alignment, EqualsVector2f({-0.707f, -0.707f}, 0.001f));
    }
    SECTION("Total rotation = 270 degrees") {
      instance.SetTotalRotation(sf::degrees(270.f));
      const auto result = instance.GetSocketWorldAlignmentVector(0);
      if (!result.has_value())
        FAIL(result.error().message);
      const sf::Vector2f &world_alignment = result.value();
      REQUIRE(world_alignment.length() == Catch::Approx(1.f));
      REQUIRE_THAT(world_alignment, EqualsVector2f({0.707f, -0.707f}, 0.001f));
    }
    SECTION("Total rotation = 360 degrees") {
      instance.SetTotalRotation(sf::degrees(360.f));
      const auto result = instance.GetSocketWorldAlignmentVector(0);
      if (!result.has_value())
        FAIL(result.error().message);
      const sf::Vector2f &world_alignment = result.value();
      REQUIRE(world_alignment.length() == Catch::Approx(1.f));
      REQUIRE_THAT(world_alignment, EqualsVector2f({0.707f, 0.707f}, 0.001f));
    }
  }
}

TEST_CASE("JointInstance::GetSocketPivotWorldPosition tests",
          "[JointInstance]") {
  JointInstance instance{1, parts::JointSquareWithOneSocket};

  SECTION("Matches transform applied to part.socket_pivot") {
    const sf::Vector2f expected =
        instance.GetTransform().transformPoint(instance.GetPart().socket_pivot);
    REQUIRE_THAT(instance.GetSocketPivotWorldPosition(),
                 EqualsVector2f(expected, 0.0001f));
  }

  SECTION("Tracks updated world position when transform changes") {
    sf::Transform t;
    t.translate({7.f, -3.f});
    instance.SetTransform(t);

    const sf::Vector2f expected =
        instance.GetTransform().transformPoint(instance.GetPart().socket_pivot);
    REQUIRE_THAT(instance.GetSocketPivotWorldPosition(),
                 EqualsVector2f(expected, 0.0001f));
  }
}

TEST_CASE("JointInstance::CheckMouseOverSockets tests", "[JointInstance]") {
  JointInstance instance{1, parts::JointSquareWithOneSocket};

  SECTION("Does not throw when called") {
    REQUIRE_NOTHROW(instance.CheckMouseOverSockets({0.f, 0.f}));
    REQUIRE_NOTHROW(instance.CheckMouseOverSockets({100.f, 100.f}));
  }
}

TEST_CASE("JointInstance::CheckIfSocketIsAvailable tests", "[JointInstance]") {
  JointInstance joint_instance{1, parts::JointSquareWithOneSocket};
  FragmentInstance fragment_instance{2, parts::FragmentRectangleWithOneSocket};

  SECTION("False when socket does not exist") {
    REQUIRE_FALSE(joint_instance.CheckIfSocketIsAvailable(999));
  }

  SECTION("True when socket exists and is unconnected") {
    if (joint_instance.GetSocketCount() == 0) {
      SUCCEED(
          "No sockets defined by fixture; availability case not applicable.");
      return;
    }
    REQUIRE(joint_instance.CheckIfSocketIsAvailable(0));
  }

  SECTION("False after connection is created") {
    if (joint_instance.GetSocketCount() == 0 ||
        fragment_instance.GetSocketCount() == 0) {
      SUCCEED(
          "No sockets defined by fixtures; connection case not applicable.");
      return;
    }

    auto result = joint_instance.CreateConnectionWithOtherInstance(
        0, fragment_instance, 0);
    REQUIRE(result.has_value());
    REQUIRE_FALSE(joint_instance.CheckIfSocketIsAvailable(0));
  }
}

TEST_CASE("JointInstance::CheckIfAnySocketIsAvailable tests",
          "[JointInstance]") {
  JointInstance joint_instance{1, parts::JointSquareWithOneSocket};
  FragmentInstance fragment_instance{2, parts::FragmentRectangleWithOneSocket};

  SECTION("Returns available socket id when one exists") {
    const auto result = joint_instance.CheckIfAnySocketIsAvailable();
    if (joint_instance.GetSocketCount() == 0) {
      REQUIRE_FALSE(result.has_value());
    } else {
      REQUIRE(result.has_value());
      REQUIRE(result.value() < joint_instance.GetSocketCount());
    }
  }

  SECTION("Returns nullopt when all sockets are connected") {
    if (joint_instance.GetSocketCount() == 0 ||
        fragment_instance.GetSocketCount() == 0) {
      SUCCEED(
          "No sockets defined by fixtures; all-connected case not applicable.");
      return;
    }

    auto result = joint_instance.CreateConnectionWithOtherInstance(
        0, fragment_instance, 0);
    REQUIRE(result.has_value());

    REQUIRE_FALSE(joint_instance.CheckIfAnySocketIsAvailable().has_value());
  }
}

TEST_CASE("JointInstance::ResetAllSocketsInteractionState tests",
          "[JointInstance]") {
  JointInstance instance{1, parts::JointSquareWithOneSocket};

  SECTION("Clears distance/mouse-over interaction fields on all sockets") {
    if (instance.GetSocketCount() == 0) {
      SUCCEED("No sockets defined by fixture; reset case not applicable.");
      return;
    }

    auto *socket = const_cast<JointSocketState *>(instance.TryGetSocket(0));
    REQUIRE(socket != nullptr);

    socket->SetDistanceToNearestSocket(1.0f);
    socket->CheckMouseOver({0.f, 0.f}, {0.f, 0.f}, 5.f);
    REQUIRE(socket->GetDistanceToNearestSocket().has_value());
    REQUIRE(socket->IsMouseOver());

    instance.ResetAllSocketsInteractionState();

    REQUIRE_FALSE(socket->GetDistanceToNearestSocket().has_value());
    REQUIRE_FALSE(socket->IsMouseOver());
  }
}

TEST_CASE("JointInstance::ResetAllSocketState tests", "[JointInstance]") {
  JointInstance joint_instance{1, parts::JointSquareWithOneSocket};
  FragmentInstance fragment_instance{2, parts::FragmentRectangleWithOneSocket};

  SECTION("Clears connection and interaction fields on all sockets") {
    if (joint_instance.GetSocketCount() == 0 ||
        fragment_instance.GetSocketCount() == 0) {
      SUCCEED("No sockets defined by fixtures; reset case not applicable.");
      return;
    }

    auto result = joint_instance.CreateConnectionWithOtherInstance(
        0, fragment_instance, 0);
    REQUIRE(result.has_value());
    REQUIRE(joint_instance.GetNumberOfConnectedSockets() == 1);

    joint_instance.ResetAllSocketState();

    REQUIRE(joint_instance.GetNumberOfConnectedSockets() == 0);
  }
}

TEST_CASE("JointInstance::CheckWithOtherInstanceForCollision tests",
          "[JointInstance]") {
  JointInstance joint_instance{1, parts::JointSquareWithOneSocket};
  FragmentInstance fragment_instance{2, parts::FragmentRectangleWithOneSocket};

  SECTION("Does not throw for Joint-Fragment collision checks") {
    REQUIRE_NOTHROW(
        joint_instance.CheckWithOtherInstanceForCollision(fragment_instance));
  }
}

TEST_CASE("JointInstance::CreateConnectionWithOtherInstance tests",
          "[JointInstance]") {
  JointInstance joint_instance{1, parts::JointSquareWithOneSocket};
  FragmentInstance fragment_instance{2, parts::FragmentRectangleWithOneSocket};

  SECTION("Returns MissingData when this socket is invalid") {
    const auto result = joint_instance.CreateConnectionWithOtherInstance(
        999, fragment_instance, 0);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().mode == FailMode::MissingData);
  }

  SECTION("Returns MissingData when other socket is invalid") {
    if (joint_instance.GetSocketCount() == 0) {
      SUCCEED("No joint sockets; invalid-other case not applicable.");
      return;
    }

    const auto result = joint_instance.CreateConnectionWithOtherInstance(
        0, fragment_instance, 999);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().mode == FailMode::MissingData);
  }

  SECTION("Creates reciprocal Joint-Fragment connection on success") {
    if (joint_instance.GetSocketCount() == 0 ||
        fragment_instance.GetSocketCount() == 0) {
      SUCCEED("No sockets defined by fixtures; success case not applicable.");
      return;
    }

    const auto result = joint_instance.CreateConnectionWithOtherInstance(
        0, fragment_instance, 0);
    REQUIRE(result.has_value());

    const auto *joint_socket = joint_instance.TryGetSocket(0);
    const auto *fragment_socket = fragment_instance.TryGetSocket(0);
    REQUIRE(joint_socket != nullptr);
    REQUIRE(fragment_socket != nullptr);

    REQUIRE(joint_socket->GetConnection().has_value());
    REQUIRE(fragment_socket->GetConnection().has_value());

    REQUIRE(joint_socket->GetConnection()->peer_part_id ==
            fragment_instance.GetId());
    REQUIRE(joint_socket->GetConnection()->peer_socket_id == 0u);

    REQUIRE(fragment_socket->GetConnection()->peer_part_id ==
            joint_instance.GetId());
    REQUIRE(fragment_socket->GetConnection()->peer_socket_id == 0u);
  }

  SECTION("Returns InvalidState when attempting duplicate connection") {
    if (joint_instance.GetSocketCount() == 0 ||
        fragment_instance.GetSocketCount() == 0) {
      SUCCEED(
          "No sockets defined by fixtures; invalid-state case not applicable.");
      return;
    }

    auto first = joint_instance.CreateConnectionWithOtherInstance(
        0, fragment_instance, 0);
    REQUIRE(first.has_value());

    auto second = joint_instance.CreateConnectionWithOtherInstance(
        0, fragment_instance, 0);
    REQUIRE_FALSE(second.has_value());
    REQUIRE(second.error().mode == FailMode::InvalidState);
  }
}

TEST_CASE("JointInstance::PositionSockets tests", "[JointInstance]") {

  SECTION("Joint with 1 socket") {
    JointInstance instance{1, parts::JointSquareWithOneSocket};
    REQUIRE(instance.GetPart().socket_pivot == sf::Vector2f{10.f, 10.f});
    REQUIRE(instance.GetPart().socket_config.radius == 13.f);
    REQUIRE(instance.GetSocketCount() == 1);

    SECTION("MaximiseDistance strategty ") {

      SECTION("Positions socket at arc midpoint") {
        instance.PositionSockets(
            JointSocketPositioningStrategy::MaximizeDistance);
        // 45 degrees from pivot (10, 10) with radius 13 => (10 + 13/sqrt(2), 10
        // + 13/sqrt(2))
        const sf::Vector2f expected_position = {10.f + 13.f / std::sqrt(2.f),
                                                10.f + 13.f / std::sqrt(2.f)};

        REQUIRE_THAT(instance.GetSocketLocalPosition(0),
                     EqualsVector2f(expected_position, 0.001f));
      }
    }
  }

  SECTION("Joint with 2 sockets") {
    JointInstance instance{1, parts::JointSquareWithTwoSockets};
    REQUIRE(instance.GetPart().socket_pivot == sf::Vector2f{10.f, 10.f});
    REQUIRE(instance.GetPart().socket_config.radius == 13.f);
    REQUIRE(instance.GetSocketCount() == 2);
    SECTION("MaximiseDistance strategy") {
      SECTION("Positions sockets at arc endpoints") {
        instance.PositionSockets(
            JointSocketPositioningStrategy::MaximizeDistance);
        // 0 degrees from pivot (10, 10) with radius 13 => (10 + 13, 10)
        const sf::Vector2f expected_position_0 = {10.f + 13.f, 10.f};
        // 90 degrees from pivot (10, 10) with radius 13 => (10, 10 + 13)
        const sf::Vector2f expected_position_1 = {10.f, 10.f + 13.f};
        REQUIRE_THAT(instance.GetSocketLocalPosition(0),
                     EqualsVector2f(expected_position_0, 0.001f));
        REQUIRE_THAT(instance.GetSocketLocalPosition(1),
                     EqualsVector2f(expected_position_1, 0.001f));
      }
    }
  }

  SECTION("Joint with 3 sockets") {
    JointInstance instance{1, parts::JointSquareWithThreeSockets};
    REQUIRE(instance.GetPart().socket_pivot == sf::Vector2f{10.f, 10.f});
    REQUIRE(instance.GetPart().socket_config.radius == 13.f);
    REQUIRE(instance.GetSocketCount() == 3);
    SECTION("MaximiseDistance strategy") {
      SECTION("Positions sockets evenly across arc") {
        instance.PositionSockets(
            JointSocketPositioningStrategy::MaximizeDistance);
        // 0 degrees from pivot (10, 10) with radius 13 => (10 + 13, 10)
        const sf::Vector2f expected_position_0 = {10.f + 13.f, 10.f};
        // 45 degrees from pivot (10, 10) with radius 13 => (10 + 13/sqrt(2), 10
        // + 13/sqrt(2))
        const sf::Vector2f expected_position_1 = {10.f + 13.f / std::sqrt(2.f),
                                                  10.f + 13.f / std::sqrt(2.f)};
        // 90 degrees from pivot (10, 10) with radius 13 => (10, 10 + 13)
        const sf::Vector2f expected_position_2 = {10.f, 10.f + 13.f};
        REQUIRE_THAT(instance.GetSocketLocalPosition(0),
                     EqualsVector2f(expected_position_0, 0.001f));
        REQUIRE_THAT(instance.GetSocketLocalPosition(1),
                     EqualsVector2f(expected_position_1, 0.001f));
        REQUIRE_THAT(instance.GetSocketLocalPosition(2),
                     EqualsVector2f(expected_position_2, 0.001f));
      }
    }
  }
}
TEST_CASE("maximise_joint_socket_spread tests",
          "[JointInstance][positioning_grimoire_machina]") {
  JointInstance::Sockets sockets;
  sf::Vector2f pivot{0.f, 0.f};
  SocketConfig config;

  SECTION("Does not throw with zero sockets") {
    config.socket_count = 0;
    REQUIRE_NOTHROW(maximise_joint_socket_spread(sockets, pivot, config));
  }

  SECTION("Does not throw when sockets exist but socket_count is zero") {
    sockets.emplace(0, JointSocketState{});
    config.socket_count = 0;
    config.rotation_arc_min = 0.f;
    config.rotation_arc_max = 180.f;
    config.radius = 10.f;

    // Depending on implementation, this either early-returns or still places.
    // We only enforce safety here.
    REQUIRE_NOTHROW(maximise_joint_socket_spread(sockets, pivot, config));
  }

  SECTION("Joint with 1 socket") {
    config.socket_count = 1;
    sockets.emplace(0, JointSocketState{});
    REQUIRE(sockets.size() == 1);

    SECTION("0-90 degree arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 90.f;
      config.radius = 10.f;
      maximise_joint_socket_spread(sockets, pivot, config);
      REQUIRE_THAT(sockets.at(0).GetLocalPosition(),
                   EqualsVector2f({7.071f, 7.071f}, 0.001f));
    }

    SECTION("0-180 degree arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 180.f;
      config.radius = 10.f;
      maximise_joint_socket_spread(sockets, pivot, config);
      REQUIRE_THAT(sockets.at(0).GetLocalPosition(),
                   EqualsVector2f({0.f, 10.f}, 0.001f));
    }

    SECTION("0-360 degree arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 360.f;
      config.radius = 10.f;
      maximise_joint_socket_spread(sockets, pivot, config);
      REQUIRE_THAT(sockets.at(0).GetLocalPosition(),
                   EqualsVector2f({-10.f, 0.f}, 0.001f));
    }

    SECTION("90-270 degree arc") {
      config.rotation_arc_min = 90.f;
      config.rotation_arc_max = 270.f;
      config.radius = 10.f;
      maximise_joint_socket_spread(sockets, pivot, config);
      REQUIRE_THAT(sockets.at(0).GetLocalPosition(),
                   EqualsVector2f({-10.f, 0.f}, 0.001f));
    }
  }

  SECTION("Joint with 2 sockets") {
    config.socket_count = 2;
    sockets.emplace(0, JointSocketState{});
    sockets.emplace(1, JointSocketState{});
    REQUIRE(sockets.size() == 2);

    SECTION("90 degree arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 90.f;
      config.radius = 10.f;
      maximise_joint_socket_spread(sockets, pivot, config);

      REQUIRE_THAT(sockets.at(0).GetLocalPosition(),
                   EqualsVector2f({10.f, 0.f}, 0.001f));
      REQUIRE_THAT(sockets.at(1).GetLocalPosition(),
                   EqualsVector2f({0.f, 10.f}, 0.001f));
    }

    SECTION("180 degree arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 180.f;
      config.radius = 10.f;
      maximise_joint_socket_spread(sockets, pivot, config);

      REQUIRE_THAT(sockets.at(0).GetLocalPosition(),
                   EqualsVector2f({10.f, 0.f}, 0.001f));
      REQUIRE_THAT(sockets.at(1).GetLocalPosition(),
                   EqualsVector2f({-10.f, 0.f}, 0.001f));
    }

    SECTION("360 degree arc (full-circle branch: no duplicated endpoint)") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 360.f;
      config.radius = 10.f;
      maximise_joint_socket_spread(sockets, pivot, config);

      // step = 360 / 2 => 0, 180
      REQUIRE_THAT(sockets.at(0).GetLocalPosition(),
                   EqualsVector2f({10.f, 0.f}, 0.001f));
      REQUIRE_THAT(sockets.at(1).GetLocalPosition(),
                   EqualsVector2f({-10.f, 0.f}, 0.001f));
    }

    SECTION("negative full circle (-360)") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = -360.f;
      config.radius = 10.f;
      maximise_joint_socket_spread(sockets, pivot, config);

      // step = -360 / 2 => 0, -180 (same point as +180)
      REQUIRE_THAT(sockets.at(0).GetLocalPosition(),
                   EqualsVector2f({10.f, 0.f}, 0.001f));
      REQUIRE_THAT(sockets.at(1).GetLocalPosition(),
                   EqualsVector2f({-10.f, 0.f}, 0.001f));
    }
  }

  SECTION("Joint with 3 sockets") {
    config.socket_count = 3;
    sockets.emplace(0, JointSocketState{});
    sockets.emplace(1, JointSocketState{});
    sockets.emplace(2, JointSocketState{});
    REQUIRE(sockets.size() == 3);

    SECTION("90 degree arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 90.f;
      config.radius = 10.f;
      maximise_joint_socket_spread(sockets, pivot, config);
      REQUIRE_THAT(sockets.at(0).GetLocalPosition(),
                   EqualsVector2f({10.f, 0.f}, 0.001f));
      REQUIRE_THAT(sockets.at(1).GetLocalPosition(),
                   EqualsVector2f({7.071f, 7.071f}, 0.001f));
      REQUIRE_THAT(sockets.at(2).GetLocalPosition(),
                   EqualsVector2f({0.f, 10.f}, 0.001f));
    }

    SECTION("180 degree arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 180.f;
      config.radius = 10.f;
      maximise_joint_socket_spread(sockets, pivot, config);
      REQUIRE_THAT(sockets.at(0).GetLocalPosition(),
                   EqualsVector2f({10.f, 0.f}, 0.001f));
      REQUIRE_THAT(sockets.at(1).GetLocalPosition(),
                   EqualsVector2f({0.f, 10.f}, 0.001f));
      REQUIRE_THAT(sockets.at(2).GetLocalPosition(),
                   EqualsVector2f({-10.f, 0.f}, 0.001f));
    }

    SECTION("360 degree arc (should be 0,120,240)") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 360.f;
      config.radius = 10.f;
      maximise_joint_socket_spread(sockets, pivot, config);

      REQUIRE_THAT(sockets.at(0).GetLocalPosition(),
                   EqualsVector2f({10.f, 0.f}, 0.001f));
      REQUIRE_THAT(sockets.at(1).GetLocalPosition(),
                   EqualsVector2f({-5.f, 8.660f}, 0.002f));
      REQUIRE_THAT(sockets.at(2).GetLocalPosition(),
                   EqualsVector2f({-5.f, -8.660f}, 0.002f));
    }

    SECTION(">360 degree arc (0 to 450 => step 150)") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 450.f;
      config.radius = 10.f;
      maximise_joint_socket_spread(sockets, pivot, config);

      // expected angles: 0, 150, 300
      REQUIRE_THAT(sockets.at(0).GetLocalPosition(),
                   EqualsVector2f({10.f, 0.f}, 0.001f));
      REQUIRE_THAT(sockets.at(1).GetLocalPosition(),
                   EqualsVector2f({-8.660f, 5.f}, 0.002f));
      REQUIRE_THAT(sockets.at(2).GetLocalPosition(),
                   EqualsVector2f({5.f, -8.660f}, 0.002f));
    }

    SECTION("negative arc (90 to -90 => clockwise through 0)") {
      config.rotation_arc_min = 90.f;
      config.rotation_arc_max = -90.f;
      config.radius = 10.f;
      maximise_joint_socket_spread(sockets, pivot, config);

      // expected: 90, 0, -90
      REQUIRE_THAT(sockets.at(0).GetLocalPosition(),
                   EqualsVector2f({0.f, 10.f}, 0.001f));
      REQUIRE_THAT(sockets.at(1).GetLocalPosition(),
                   EqualsVector2f({10.f, 0.f}, 0.001f));
      REQUIRE_THAT(sockets.at(2).GetLocalPosition(),
                   EqualsVector2f({0.f, -10.f}, 0.001f));
    }
  }

  SECTION("Uses socket container size safely when socket_count mismatches") {
    // Protects against accidental out-of-range if implementation loops by
    // config.socket_count.
    config.socket_count = 5; // mismatch on purpose
    sockets.emplace(0, JointSocketState{});
    sockets.emplace(1, JointSocketState{});
    config.rotation_arc_min = 0.f;
    config.rotation_arc_max = 180.f;
    config.radius = 10.f;

    REQUIRE_NOTHROW(maximise_joint_socket_spread(sockets, pivot, config));
  }
}
TEST_CASE("JointInstance::CheckForFirstConnectionWithOtherInstance tests",
          "[JointInstance]") {
  SECTION("Returns nullopt when checking self") {
    JointInstance joint_instance(1, parts::JointSquareWithOneSocket);

    const auto result =
        joint_instance.CheckForFirstConnectionWithOtherInstance(joint_instance);

    REQUIRE_FALSE(result.has_value());
  }

  SECTION("Returns nullopt when other instance has same id") {
    JointInstance a(42, parts::JointSquareWithOneSocket);
    FragmentInstance b(
        42, parts::FragmentRectangleWithOneSocket); // same ID, different type

    const auto result = a.CheckForFirstConnectionWithOtherInstance(b);
    REQUIRE_FALSE(result.has_value());
  }

  SECTION("Returns nullopt when no connection exists") {
    JointInstance joint_instance(1, parts::JointSquareWithOneSocket);
    FragmentInstance fragment_instance(2,
                                       parts::FragmentRectangleWithOneSocket);

    const auto result = joint_instance.CheckForFirstConnectionWithOtherInstance(
        fragment_instance);

    REQUIRE_FALSE(result.has_value());
  }

  SECTION("Returns first existing valid reciprocal connection") {
    JointInstance joint_instance(1, parts::JointSquareWithOneSocket);
    FragmentInstance fragment_instance(2,
                                       parts::FragmentRectangleWithOneSocket);

    if (joint_instance.GetSocketCount() == 0 ||
        fragment_instance.GetSocketCount() == 0) {
      FAIL("No sockets defined by fixtures; connection case not applicable.");
    }

    auto create_result = joint_instance.CreateConnectionWithOtherInstance(
        0, fragment_instance, 0);
    REQUIRE(create_result.has_value());

    const auto check_result =
        joint_instance.CheckForFirstConnectionWithOtherInstance(
            fragment_instance);

    REQUIRE(check_result.has_value());
    const auto &connection = check_result.value();
    REQUIRE(connection.this_id == joint_instance.GetId());
    REQUIRE(connection.this_socket_id == 0u);
    REQUIRE(connection.other_id == fragment_instance.GetId());
    REQUIRE(connection.other_socket_id == 0u);
  }

  SECTION("Symmetry: if A finds B, B finds A with swapped endpoints") {
    JointInstance a(10, parts::JointSquareWithOneSocket);
    FragmentInstance b(20, parts::FragmentRectangleWithOneSocket);

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

  SECTION(
      "Returns nullopt when connection exists but points to different part") {
    JointInstance a(1, parts::JointSquareWithOneSocket);
    FragmentInstance b(2, parts::FragmentRectangleWithOneSocket);
    FragmentInstance c(3, parts::FragmentRectangleWithOneSocket);

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
    JointInstance a(100,
                    parts::JointSquareWithTwoSockets); // adjust fixture name
    FragmentInstance b(
        200, parts::FragmentRectangleWithTwoSockets); // adjust fixture name

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
} // namespace steamrot::tests
