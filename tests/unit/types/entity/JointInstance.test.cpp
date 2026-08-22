
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
  JointInstance instance{1, parts::JointSquareWithOneSocket};

  SECTION("MaximizeDistance strategy does not throw") {
    REQUIRE_NOTHROW(instance.PositionSockets(
        JointSocketPositioningStrategy::MaximizeDistance));
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
  }
}
} // namespace steamrot::tests
