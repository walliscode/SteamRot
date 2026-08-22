/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the FragmentInstance class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FragmentInstance.h"
#include "JointInstance.h"
#include "SocketState.h"
#include "catch2/catch_approx.hpp"
#include "fragment_library.h"
#include "joint_library.h"
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Angle.hpp>
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
    REQUIRE(Catch::Approx(fragment_instance.GetTotalRotation().asDegrees()) ==
            0.0f);
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

TEST_CASE("FragmentInstance::GetTransform and SetTransform tests",
          "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket,
                                     "test_fragment");

  SECTION("Default transform is identity") {
    const sf::Vector2f out =
        fragment_instance.GetTransform().transformPoint({3.f, 4.f});
    REQUIRE(Catch::Approx(out.x) == 3.f);
    REQUIRE(Catch::Approx(out.y) == 4.f);
  }

  SECTION("SetTransform replaces transform") {
    sf::Transform t;
    t.translate({10.f, 20.f});
    fragment_instance.SetTransform(t);

    const sf::Vector2f out =
        fragment_instance.GetTransform().transformPoint({1.f, 2.f});
    REQUIRE(Catch::Approx(out.x) == 11.f);
    REQUIRE(Catch::Approx(out.y) == 22.f);
  }
}

TEST_CASE("FragmentInstance::GetTotalRotation and SetTotalRotation tests",
          "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket);

  SECTION("Initial total rotation is 0 degrees") {
    REQUIRE(Catch::Approx(fragment_instance.GetTotalRotation().asDegrees()) ==
            0.0f);
  }

  SECTION("SetTotalRotation replaces tracked rotation") {
    fragment_instance.SetTotalRotation(sf::degrees(33.f));
    REQUIRE(Catch::Approx(fragment_instance.GetTotalRotation().asDegrees()) ==
            33.f);
  }
}

TEST_CASE("FragmentInstance::AddToTotalRotation tests", "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket);

  SECTION("Accumulates rotation values") {
    fragment_instance.AddToTotalRotation(sf::degrees(45.0f));
    REQUIRE(Catch::Approx(fragment_instance.GetTotalRotation().asDegrees()) ==
            45.0f);

    fragment_instance.AddToTotalRotation(sf::degrees(30.0f));
    REQUIRE(Catch::Approx(fragment_instance.GetTotalRotation().asDegrees()) ==
            75.0f);

    fragment_instance.AddToTotalRotation(sf::degrees(-90.0f));
    REQUIRE(Catch::Approx(fragment_instance.GetTotalRotation().asDegrees()) ==
            -15.0f);
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
    if (fragment_instance.GetSocketCount() == 0) {
      SUCCEED("No sockets defined by fixture; existence case not applicable.");
      return;
    }

    sf::Transform t;
    t.translate({10.f, 20.f});
    fragment_instance.SetTransform(t);

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
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket);

  SECTION("Returns MissingData error when socket does not exist") {
    const auto result = fragment_instance.GetSocketWorldAlignmentVector(999);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().mode == FailMode::MissingData);
  }

  SECTION("Returns local alignment vector when socket exists") {
    if (fragment_instance.GetSocketCount() == 0) {
      SUCCEED("No sockets defined by fixture; existence case not applicable.");
      return;
    }

    const auto result = fragment_instance.GetSocketWorldAlignmentVector(0);
    REQUIRE(result.has_value());

    const sf::Vector2f expected =
        parts::FragmentRectangleWithOneSocket.sockets[0].alignment_vector;
    REQUIRE(Catch::Approx(result.value().x) == expected.x);
    REQUIRE(Catch::Approx(result.value().y) == expected.y);
  }
}

TEST_CASE("FragmentInstance::CheckMouseOverSockets tests",
          "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket);

  SECTION("Does not throw when called") {
    REQUIRE_NOTHROW(fragment_instance.CheckMouseOverSockets({0.f, 0.f}));
    REQUIRE_NOTHROW(fragment_instance.CheckMouseOverSockets({100.f, 100.f}));
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
      SUCCEED(
          "No sockets defined by fixture; availability case not applicable.");
      return;
    }
    REQUIRE(fragment_instance.CheckIfSocketIsAvailable(0));
  }

  SECTION("False after connection is created") {
    if (fragment_instance.GetSocketCount() == 0 ||
        joint_instance.GetSocketCount() == 0) {
      SUCCEED(
          "No sockets defined by fixtures; connection case not applicable.");
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
      SUCCEED(
          "No sockets defined by fixtures; all-connected case not applicable.");
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
    if (fragment_instance.GetSocketCount() == 0) {
      SUCCEED("No sockets defined by fixture; reset case not applicable.");
      return;
    }

    auto *socket =
        const_cast<FragmentSocketState *>(fragment_instance.TryGetSocket(0));
    REQUIRE(socket != nullptr);

    socket->SetDistanceToNearestSocket(1.0f);
    socket->CheckMouseOver({0.f, 0.f}, {0.f, 0.f}, 5.f);
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
      SUCCEED(
          "No sockets defined by fixtures; invalid-state case not applicable.");
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

} // namespace steamrot::tests
