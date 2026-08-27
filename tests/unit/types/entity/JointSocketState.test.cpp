/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for JointSocketState and inherited SocketState behavior.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SocketState.h"
#include "catch2/catch_approx.hpp"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

TEST_CASE("JointSocketState::JointSocketState tests", "[JointSocketState]") {
  SECTION("Default constructor initializes local position to zero") {
    JointSocketState socket_state;
    REQUIRE(socket_state.GetLocalPosition() == sf::Vector2f(0.f, 0.f));
  }

  SECTION("Default constructor initializes inherited runtime state") {
    JointSocketState socket_state;
    REQUIRE_FALSE(socket_state.IsMouseOver());
    REQUIRE_FALSE(socket_state.GetDistanceToNearestSocket().has_value());
    REQUIRE_FALSE(socket_state.GetConnection().has_value());
    REQUIRE(socket_state.IsAvailable());
    REQUIRE(socket_state.IsWithinConnectionDistance() == false);
    REQUIRE(socket_state.GetConnectionState() ==
            SocketConnectionState::Available);
  }
}
TEST_CASE("JointSocketState::GetLocalPosition tests", "[JointSocketState]") {
  JointSocketState socket_state;

  SECTION("Returns default local position") {
    REQUIRE(socket_state.GetLocalPosition() == sf::Vector2f(0.f, 0.f));
  }

  SECTION("Returns updated local position after SetLocalPosition") {
    socket_state.SetLocalPosition(sf::Vector2f(5.f, -3.f));
    REQUIRE(socket_state.GetLocalPosition() == sf::Vector2f(5.f, -3.f));
  }
}

TEST_CASE("JointSocketState::SetLocalPosition tests", "[JointSocketState]") {
  JointSocketState socket_state;

  SECTION("Sets local position") {
    socket_state.SetLocalPosition(sf::Vector2f(2.f, 4.f));
    REQUIRE(socket_state.GetLocalPosition() == sf::Vector2f(2.f, 4.f));
  }

  SECTION("Overwrites previous local position") {
    socket_state.SetLocalPosition(sf::Vector2f(1.f, 1.f));
    socket_state.SetLocalPosition(sf::Vector2f(-8.f, 9.f));
    REQUIRE(socket_state.GetLocalPosition() == sf::Vector2f(-8.f, 9.f));
  }
}

TEST_CASE("SocketState::IsMouseOver tests", "[JointSocketState][SocketState]") {
  JointSocketState socket_state;

  SECTION("False by default") { REQUIRE_FALSE(socket_state.IsMouseOver()); }
}

TEST_CASE("SocketState::IsAnotherSocketNear tests",
          "[JointSocketState][SocketState]") {
  JointSocketState socket_state;

  SECTION("False when nearest distance is unset") {
    REQUIRE_FALSE(socket_state.IsAnotherSocketNear());
  }

  SECTION("True when distance is within proximity threshold") {
    socket_state.SetDistanceToNearestSocket(10.f);
    REQUIRE(socket_state.IsAnotherSocketNear());
  }

  SECTION("False when distance is outside proximity threshold") {
    socket_state.SetDistanceToNearestSocket(10.01f);
    REQUIRE_FALSE(socket_state.IsAnotherSocketNear());
  }
}

TEST_CASE("SocketState::IsWithinConnectionDistance tests",
          "[JointSocketState][SocketState]") {
  JointSocketState socket_state;

  SECTION("False when nearest distance is unset") {
    REQUIRE_FALSE(socket_state.IsWithinConnectionDistance());
  }

  SECTION("True at threshold") {
    socket_state.SetDistanceToNearestSocket(2.5f);
    REQUIRE(socket_state.IsWithinConnectionDistance());
  }

  SECTION("False above threshold") {
    socket_state.SetDistanceToNearestSocket(2.51f);
    REQUIRE_FALSE(socket_state.IsWithinConnectionDistance());
  }
}

TEST_CASE("SocketState::GetDistanceToNearestSocket tests",
          "[JointSocketState][SocketState]") {
  JointSocketState socket_state;

  SECTION("Returns nullopt by default") {
    REQUIRE_FALSE(socket_state.GetDistanceToNearestSocket().has_value());
  }

  SECTION("Returns value after setting distance") {
    socket_state.SetDistanceToNearestSocket(3.25f);
    REQUIRE(socket_state.GetDistanceToNearestSocket().has_value());
    REQUIRE(socket_state.GetDistanceToNearestSocket().value() ==
            Catch::Approx(3.25f));
  }

  SECTION("Can be cleared with nullopt") {
    socket_state.SetDistanceToNearestSocket(1.0f);
    socket_state.SetDistanceToNearestSocket(std::nullopt);
    REQUIRE_FALSE(socket_state.GetDistanceToNearestSocket().has_value());
  }
}

TEST_CASE("SocketState::SetDistanceToNearestSocket tests",
          "[JointSocketState][SocketState]") {
  JointSocketState socket_state;

  SECTION("Stores provided distance") {
    socket_state.SetDistanceToNearestSocket(4.0f);
    REQUIRE(socket_state.GetDistanceToNearestSocket().value() ==
            Catch::Approx(4.0f));
  }

  SECTION("Overwrites previous distance") {
    socket_state.SetDistanceToNearestSocket(8.0f);
    socket_state.SetDistanceToNearestSocket(2.0f);
    REQUIRE(socket_state.GetDistanceToNearestSocket().value() ==
            Catch::Approx(2.0f));
  }
}

TEST_CASE("SocketState::ConsiderCandidateDistance tests",
          "[JointSocketState][SocketState]") {
  JointSocketState socket_state;

  SECTION("Rejects candidate outside proximity threshold") {
    const bool accepted = socket_state.ConsiderCandidateDistance(10.1f);
    REQUIRE_FALSE(accepted);
    REQUIRE_FALSE(socket_state.GetDistanceToNearestSocket().has_value());
  }

  SECTION("Accepts first candidate within threshold") {
    const bool accepted = socket_state.ConsiderCandidateDistance(9.0f);
    REQUIRE(accepted);
    REQUIRE(socket_state.GetDistanceToNearestSocket().value() ==
            Catch::Approx(9.0f));
  }

  SECTION("Rejects farther candidate when one already exists") {
    REQUIRE(socket_state.ConsiderCandidateDistance(4.0f));
    const bool accepted = socket_state.ConsiderCandidateDistance(6.0f);
    REQUIRE_FALSE(accepted);
    REQUIRE(socket_state.GetDistanceToNearestSocket().value() ==
            Catch::Approx(4.0f));
  }

  SECTION("Rejects equal-distance candidate when one already exists") {
    REQUIRE(socket_state.ConsiderCandidateDistance(4.0f));
    const bool accepted = socket_state.ConsiderCandidateDistance(4.0f);
    REQUIRE_FALSE(accepted);
    REQUIRE(socket_state.GetDistanceToNearestSocket().value() ==
            Catch::Approx(4.0f));
  }

  SECTION("Accepts closer candidate and updates stored distance") {
    REQUIRE(socket_state.ConsiderCandidateDistance(7.0f));
    const bool accepted = socket_state.ConsiderCandidateDistance(3.0f);
    REQUIRE(accepted);
    REQUIRE(socket_state.GetDistanceToNearestSocket().value() ==
            Catch::Approx(3.0f));
  }
}

TEST_CASE("SocketState::SetConnection tests",
          "[JointSocketState][SocketState]") {
  JointSocketState socket_state;

  SECTION("Stores provided peer endpoint") {
    SocketConnection connection{42, 7};
    socket_state.SetConnection(connection);

    REQUIRE(socket_state.GetConnection().has_value());
    REQUIRE(socket_state.GetConnection().value() == connection);
  }
}

TEST_CASE("SocketState::GetConnection tests",
          "[JointSocketState][SocketState]") {
  JointSocketState socket_state;

  SECTION("Returns nullopt by default") {
    REQUIRE_FALSE(socket_state.GetConnection().has_value());
  }

  SECTION("Returns set connection value") {
    SocketConnection connection{100, 9};
    socket_state.SetConnection(connection);
    REQUIRE(socket_state.GetConnection().has_value());
    REQUIRE(socket_state.GetConnection().value() == connection);
  }
}

TEST_CASE("SocketState::ClearConnection tests",
          "[JointSocketState][SocketState]") {
  JointSocketState socket_state;
  socket_state.SetConnection(SocketConnection{1, 2});

  SECTION("Clears existing connection") {
    socket_state.ClearConnection();
    REQUIRE_FALSE(socket_state.GetConnection().has_value());
  }
}

TEST_CASE("SocketState::IsAvailable tests", "[JointSocketState][SocketState]") {
  JointSocketState socket_state;

  SECTION("True when no connection exists") {
    REQUIRE(socket_state.IsAvailable());
  }

  SECTION("False when connected") {
    socket_state.SetConnection(SocketConnection{3, 4});
    REQUIRE_FALSE(socket_state.IsAvailable());
  }
}

TEST_CASE("SocketState::GetConnectionState tests",
          "[JointSocketState][SocketState]") {
  JointSocketState socket_state;

  SECTION("Available when no connection exists") {
    REQUIRE(socket_state.GetConnectionState() ==
            SocketConnectionState::Available);
  }

  SECTION("Connected when connection exists") {
    socket_state.SetConnection(SocketConnection{3, 4});
    REQUIRE(socket_state.GetConnectionState() ==
            SocketConnectionState::Connected);
  }
}

TEST_CASE("SocketState::ResetInteractionState tests",
          "[JointSocketState][SocketState]") {
  JointSocketState socket_state;

  socket_state.SetDistanceToNearestSocket(2.0f);
  socket_state.SetConnection(SocketConnection{6, 7});

  SECTION("Clears interaction-only fields and preserves connection") {
    socket_state.ResetInteractionState();
    REQUIRE_FALSE(socket_state.IsMouseOver());
    REQUIRE_FALSE(socket_state.GetDistanceToNearestSocket().has_value());
    REQUIRE(socket_state.GetConnection().has_value());
    REQUIRE(socket_state.GetConnection().value() == SocketConnection{6, 7});
  }
}

TEST_CASE("SocketState::ResetAllState tests",
          "[JointSocketState][SocketState]") {
  JointSocketState socket_state;

  socket_state.SetDistanceToNearestSocket(1.5f);
  socket_state.SetConnection(SocketConnection{9, 10});

  SECTION("Clears interaction and connection fields") {
    socket_state.ResetAllState();
    REQUIRE_FALSE(socket_state.IsMouseOver());
    REQUIRE_FALSE(socket_state.GetDistanceToNearestSocket().has_value());
    REQUIRE_FALSE(socket_state.GetConnection().has_value());
  }
}

TEST_CASE("SocketState::GetSocketBrightness tests",
          "[JointSocketState][SocketState]") {
  JointSocketState socket_state;

  SECTION("Returns 0 when no nearby socket exists") {
    REQUIRE(socket_state.GetSocketBrightness() == 0u);
  }

  SECTION("Returns full brightness at connection threshold") {
    socket_state.SetDistanceToNearestSocket(2.5f);
    REQUIRE(socket_state.GetSocketBrightness() == 255u);
  }

  SECTION("Returns 0 at proximity threshold") {
    socket_state.SetDistanceToNearestSocket(10.f);
    REQUIRE(socket_state.GetSocketBrightness() == 0u);
  }

  SECTION("Returns interpolated brightness between thresholds") {
    socket_state.SetDistanceToNearestSocket(6.25f);
    REQUIRE(socket_state.GetSocketBrightness() == 127u);
  }

  SECTION("Clamps to 255 for distances below connection threshold") {
    socket_state.SetDistanceToNearestSocket(0.0f);
    REQUIRE(socket_state.GetSocketBrightness() == 255u);
  }
}

TEST_CASE("SocketConnection::operator== tests",
          "[JointSocketState][SocketState][SocketConnection]") {
  SECTION("True when both fields match") {
    REQUIRE(SocketConnection{1, 2} == SocketConnection{1, 2});
  }

  SECTION("False when peer_part_id differs") {
    REQUIRE_FALSE(SocketConnection{1, 2} == SocketConnection{9, 2});
  }

  SECTION("False when peer_socket_id differs") {
    REQUIRE_FALSE(SocketConnection{1, 2} == SocketConnection{1, 9});
  }
}

TEST_CASE("JointFragmentConnection::JointFragmentConnection tests",
          "[JointSocketState][JointFragmentConnection]") {
  SECTION("Stores all constructor parameters") {
    PartToPartConnection connection{11, 12, 21, 22};
    REQUIRE(connection.this_id == 11u);
    REQUIRE(connection.this_socket_id == 12u);
    REQUIRE(connection.other_id == 21u);
    REQUIRE(connection.other_socket_id == 22u);
  }
}

} // namespace steamrot::tests
