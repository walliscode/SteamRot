/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the FragmentInstance class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FragmentInstance.h"
#include "catch2/catch_approx.hpp"
#include "fragment_library.h"
#include <SFML/Graphics/Transform.hpp>
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

TEST_CASE("FragmentInstance::Constructors tests", "[FragmentInstance]") {
  SECTION("Construct with id and fragment") {
    FragmentInstance fragment_instance(42,
                                       parts::FragmentRectangleWithOneSocket);

    REQUIRE(fragment_instance.GetId() == 42);
    REQUIRE(fragment_instance.GetAlias() == "none");
    REQUIRE(&fragment_instance.GetPart() ==
            &parts::FragmentRectangleWithOneSocket);
    REQUIRE(fragment_instance.GetSocketCount() == 0);
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

TEST_CASE("FragmentInstance::GetTransform and SetTransform tests",
          "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket,
                                     "test_fragment");

  SECTION("Default transform is identity") {
    const sf::Vector2f p(3.f, 4.f);
    const sf::Vector2f out = fragment_instance.GetTransform().transformPoint(p);
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

TEST_CASE("FragmentInstance::AddTotalRotation tests", "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket,
                                     "test_fragment");

  SECTION("Initial total rotation is 0 degrees") {
    REQUIRE(Catch::Approx(fragment_instance.GetTotalRotation().asDegrees()) ==
            0.0f);
  }

  SECTION("Add total rotation and check the result") {
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
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket,
                                     "test_fragment");

  SECTION("Returns nullptr when socket does not exist") {
    const auto *const_socket = fragment_instance.TryGetSocket(999);
    REQUIRE(fragment_instance.TryGetSocket(999) == nullptr);
    REQUIRE(static_cast<const FragmentInstance &>(fragment_instance)
                .TryGetSocket(999) == nullptr);
  }

  SECTION("Returns pointer when socket exists (mutable + const)") {

    const auto *const_socket =
        static_cast<const FragmentInstance &>(fragment_instance)
            .TryGetSocket(5);

    REQUIRE(const_socket != nullptr);
  }
}

TEST_CASE("FragmentInstance::GetSocketWorldPosition tests",
          "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket,
                                     "test_fragment");

  SECTION("Returns zero vector when socket does not exist") {
    const sf::Vector2f p = fragment_instance.GetSocketWorldPosition(100);
    REQUIRE(Catch::Approx(p.x) == 0.f);
    REQUIRE(Catch::Approx(p.y) == 0.f);
  }

  SECTION("Applies transform to local socket position") {
    sf::Transform t;
    t.translate({10.f, 20.f});
    fragment_instance.SetTransform(t);

    const sf::Vector2f p = fragment_instance.GetSocketWorldPosition(1);
    REQUIRE(Catch::Approx(p.x) == 12.f);
    REQUIRE(Catch::Approx(p.y) == 23.f);
  }
}

TEST_CASE("FragmentInstance::GetSocketWorldAlignmentVector tests",
          "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket,
                                     "test_fragment");

  SECTION("Returns zero vector when socket does not exist") {
    const sf::Vector2f v = fragment_instance.GetSocketWorldAlignmentVector(100);
    REQUIRE(Catch::Approx(v.x) == 0.f);
    REQUIRE(Catch::Approx(v.y) == 0.f);
  }

  SECTION("Rotates local alignment vector by total rotation") {

    fragment_instance.AddToTotalRotation(sf::degrees(90.f));
    const sf::Vector2f v = fragment_instance.GetSocketWorldAlignmentVector(1);

    REQUIRE(Catch::Approx(v.x).margin(0.0001f) == 0.f);
    REQUIRE(Catch::Approx(v.y).margin(0.0001f) == 1.f);
  }
}

TEST_CASE("FragmentInstance::CheckMouseOverSockets tests",
          "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket,
                                     "test_fragment");

  SECTION("Does not throw with no sockets") {
    REQUIRE_NOTHROW(fragment_instance.CheckMouseOverSockets({0.f, 0.f}));
  }

  SECTION("Processes sockets without throwing") {

    REQUIRE_NOTHROW(fragment_instance.CheckMouseOverSockets({0.f, 0.f}));
    REQUIRE_NOTHROW(fragment_instance.CheckMouseOverSockets({100.f, 100.f}));
  }
}

TEST_CASE("FragmentInstance::CheckForConnectionReadiness tests",
          "[FragmentInstance]") {
  FragmentInstance fragment_instance(1, parts::FragmentRectangleWithOneSocket,
                                     "test_fragment");

  SECTION("Returns nullopt when no sockets are available") {
    REQUIRE_FALSE(fragment_instance.CheckIfAnySocketIsAvailable().has_value());
  }

  SECTION("Returns socket id when an available socket exists") {

    const auto result = fragment_instance.CheckIfAnySocketIsAvailable();
    REQUIRE(result.has_value());
    REQUIRE(result.value() == 20);
  }
}

} // namespace steamrot::tests
