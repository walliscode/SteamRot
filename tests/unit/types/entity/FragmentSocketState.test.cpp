/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the FragmentSocketState class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SocketState.h"
#include "catch2/catch_approx.hpp"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

TEST_CASE("FragmentSocketState::FragmentSocketState tests",
          "[FragmentSocketState]") {
  sf::Vector2f local_position(5.f, 5.f);
  sf::Vector2f alignment_vector(1.f, 0.f);
  FragmentSocketState socket_state{local_position, alignment_vector};

  SECTION("Local position and alignment vector are set correctly") {
    REQUIRE(socket_state.GetLocalPosition() == local_position);
    REQUIRE(socket_state.GetLocalAlignmentVector() == alignment_vector);
  }

  SECTION("Prove that references are being assigned") {
    local_position.x = 10.f;
    local_position.y = 10.f;
    alignment_vector.x = 0.f;
    alignment_vector.y = 1.f;
    REQUIRE(socket_state.GetLocalPosition() == local_position);
    REQUIRE(socket_state.GetLocalAlignmentVector() == alignment_vector);
  }
}

TEST_CASE("FragmentSocketState::GetLocalPosition tests",
          "[FragmentSocketState]") {
  sf::Vector2f local_position(5.f, 5.f);
  sf::Vector2f alignment_vector(1.f, 0.f);
  FragmentSocketState socket_state{local_position, alignment_vector};

  SECTION("Returns local position reference value") {
    REQUIRE(socket_state.GetLocalPosition() == local_position);
  }

  SECTION("Reference remains bound to source vector") {
    local_position.x = 10.f;
    local_position.y = -3.f;
    REQUIRE(socket_state.GetLocalPosition() == local_position);
  }
}

TEST_CASE("FragmentSocketState::GetLocalAlignmentVector tests",
          "[FragmentSocketState]") {
  sf::Vector2f local_position(0.f, 0.f);
  sf::Vector2f alignment_vector(1.f, 0.f);
  FragmentSocketState socket_state{local_position, alignment_vector};

  SECTION("Returns local alignment vector reference value") {
    REQUIRE(socket_state.GetLocalAlignmentVector() == alignment_vector);
  }

  SECTION("Reference remains bound to source vector") {
    alignment_vector.x = 0.f;
    alignment_vector.y = 1.f;
    REQUIRE(socket_state.GetLocalAlignmentVector() == alignment_vector);
  }
}

TEST_CASE("SocketState::IsMouseOver tests",
          "[FragmentSocketState][SocketState]") {
  sf::Vector2f local_position(1.f, 2.f);
  sf::Vector2f alignment_vector(1.f, 0.f);
  FragmentSocketState socket_state{local_position, alignment_vector};

  SECTION("False by default") { REQUIRE_FALSE(socket_state.IsMouseOver()); }
}

TEST_CASE("SocketState::IsAnotherSocketNear tests",
          "[FragmentSocketState][SocketState]") {
  sf::Vector2f local_position(0.f, 0.f);
  sf::Vector2f alignment_vector(1.f, 0.f);
  FragmentSocketState socket_state{local_position, alignment_vector};

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
          "[FragmentSocketState][SocketState]") {
  sf::Vector2f local_position(0.f, 0.f);
  sf::Vector2f alignment_vector(1.f, 0.f);
  FragmentSocketState socket_state{local_position, alignment_vector};

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
          "[FragmentSocketState][SocketState]") {
  sf::Vector2f local_position(0.f, 0.f);
  sf::Vector2f alignment_vector(1.f, 0.f);
  FragmentSocketState socket_state{local_position, alignment_vector};

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
          "[FragmentSocketState][SocketState]") {
  sf::Vector2f local_position(0.f, 0.f);
  sf::Vector2f alignment_vector(1.f, 0.f);
  FragmentSocketState socket_state{local_position, alignment_vector};

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
          "[FragmentSocketState][SocketState]") {
  sf::Vector2f local_position(0.f, 0.f);
  sf::Vector2f alignment_vector(1.f, 0.f);
  FragmentSocketState socket_state{local_position, alignment_vector};

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
          "[FragmentSocketState][SocketState]") {
  sf::Vector2f local_position(0.f, 0.f);
  sf::Vector2f alignment_vector(1.f, 0.f);
  FragmentSocketState socket_state{local_position, alignment_vector};

  SECTION("Stores provided peer endpoint") {
    SocketConnection connection{42, 7};
    socket_state.SetConnection(connection);

    REQUIRE(socket_state.GetConnection().has_value());
    REQUIRE(socket_state.GetConnection().value() == connection);
  }
}

TEST_CASE("SocketState::GetConnection tests",
          "[FragmentSocketState][SocketState]") {
  sf::Vector2f local_position(0.f, 0.f);
  sf::Vector2f alignment_vector(1.f, 0.f);
  FragmentSocketState socket_state{local_position, alignment_vector};

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
          "[FragmentSocketState][SocketState]") {
  sf::Vector2f local_position(0.f, 0.f);
  sf::Vector2f alignment_vector(1.f, 0.f);
  FragmentSocketState socket_state{local_position, alignment_vector};
  socket_state.SetConnection(SocketConnection{1, 2});

  SECTION("Clears existing connection") {
    socket_state.ClearConnection();
    REQUIRE_FALSE(socket_state.GetConnection().has_value());
  }
}

TEST_CASE("SocketState::IsAvailable tests",
          "[FragmentSocketState][SocketState]") {
  sf::Vector2f local_position(0.f, 0.f);
  sf::Vector2f alignment_vector(1.f, 0.f);
  FragmentSocketState socket_state{local_position, alignment_vector};

  SECTION("True when no connection exists") {
    REQUIRE(socket_state.IsAvailable());
  }

  SECTION("False when connected") {
    socket_state.SetConnection(SocketConnection{3, 4});
    REQUIRE_FALSE(socket_state.IsAvailable());
  }
}

TEST_CASE("SocketState::GetConnectionState tests",
          "[FragmentSocketState][SocketState]") {
  sf::Vector2f local_position(0.f, 0.f);
  sf::Vector2f alignment_vector(1.f, 0.f);
  FragmentSocketState socket_state{local_position, alignment_vector};

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

TEST_CASE("SocketState::IsReadyToConnect tests",
          "[FragmentSocketState][SocketState]") {
  sf::Vector2f local_position(0.f, 0.f);
  sf::Vector2f alignment_vector(1.f, 0.f);
  FragmentSocketState socket_state{local_position, alignment_vector};

  SECTION("True when available") { REQUIRE(socket_state.IsReadyToConnect()); }

  SECTION("False when connected") {
    socket_state.SetConnection(SocketConnection{11, 22});
    REQUIRE_FALSE(socket_state.IsReadyToConnect());
  }
}

TEST_CASE("SocketState::CheckMouseOver tests",
          "[FragmentSocketState][SocketState]") {
  sf::Vector2f local_position(0.f, 0.f);
  sf::Vector2f alignment_vector(1.f, 0.f);
  FragmentSocketState socket_state{local_position, alignment_vector};
  const sf::Vector2f socket_world_pos(10.f, 10.f);

  SECTION("Sets true when mouse is inside radius") {
    socket_state.CheckMouseOver(sf::Vector2f(12.f, 10.f), socket_world_pos,
                                3.f);
    REQUIRE(socket_state.IsMouseOver());
  }

  SECTION("Sets true when mouse is exactly on radius boundary") {
    socket_state.CheckMouseOver(sf::Vector2f(13.f, 10.f), socket_world_pos,
                                3.f);
    REQUIRE(socket_state.IsMouseOver());
  }

  SECTION("Sets false when mouse is outside radius") {
    socket_state.CheckMouseOver(sf::Vector2f(14.f, 10.f), socket_world_pos,
                                3.f);
    REQUIRE_FALSE(socket_state.IsMouseOver());
  }
}

TEST_CASE("SocketState::ResetInteractionState tests",
          "[FragmentSocketState][SocketState]") {
  sf::Vector2f local_position(0.f, 0.f);
  sf::Vector2f alignment_vector(1.f, 0.f);
  FragmentSocketState socket_state{local_position, alignment_vector};

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
          "[FragmentSocketState][SocketState]") {
  sf::Vector2f local_position(0.f, 0.f);
  sf::Vector2f alignment_vector(1.f, 0.f);
  FragmentSocketState socket_state{local_position, alignment_vector};

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
          "[FragmentSocketState][SocketState]") {
  sf::Vector2f local_position(0.f, 0.f);
  sf::Vector2f alignment_vector(1.f, 0.f);
  FragmentSocketState socket_state{local_position, alignment_vector};

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
    // midpoint between 2.5 and 10.0 is 6.25 => t = 0.5 => ~127
    socket_state.SetDistanceToNearestSocket(6.25f);
    REQUIRE(socket_state.GetSocketBrightness() == 127u);
  }

  SECTION("Clamps to 255 for distances below connection threshold") {
    socket_state.SetDistanceToNearestSocket(0.0f);
    REQUIRE(socket_state.GetSocketBrightness() == 255u);
  }
}

TEST_CASE("SocketConnection::operator== tests",
          "[FragmentSocketState][SocketState][SocketConnection]") {
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
} // namespace steamrot::tests
