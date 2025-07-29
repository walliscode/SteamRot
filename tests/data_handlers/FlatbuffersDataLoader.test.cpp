/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersDataLoader
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersDataLoader.h"
#include "fragments_generated.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Dataloader fails to initiate PathProvider", "[DataLoader]") {
  REQUIRE_THROWS(
      steamrot::FlatbuffersDataLoader{steamrot::EnvironmentType::None});
}

TEST_CASE("DataLoader can be initiated with a valid environment",
          "[DataLoader]") {
  REQUIRE_NOTHROW(
      steamrot::FlatbuffersDataLoader{steamrot::EnvironmentType::Test});
}

TEST_CASE("DataLoader can be called with a None environment after being set",
          "[DataLoader]") {
  steamrot::FlatbuffersDataLoader data_loader{steamrot::EnvironmentType::Test};
  REQUIRE_NOTHROW(steamrot::FlatbuffersDataLoader{});
}

TEST_CASE("DataLoader can only initiate PathProvider once", "[DataLoader]") {
  steamrot::FlatbuffersDataLoader data_loader{steamrot::EnvironmentType::Test};
  REQUIRE_THROWS(
      steamrot::FlatbuffersDataLoader{steamrot::EnvironmentType::Production});
}

TEST_CASE("FlatbuffersDataLoader returns unexpected when non-existent fragment "
          "is provided",
          "[FlatbuffersDataLoader]") {
  steamrot::FlatbuffersDataLoader data_loader{steamrot::EnvironmentType::Test};
  auto result = data_loader.ProvideFragment("non_existent_fragment");
  REQUIRE(result.has_value() == false);
  REQUIRE(result.error().first == steamrot::DataFailMode::FileNotFound);
  REQUIRE(result.error().second == "file not found");
}

TEST_CASE("FlatbuffersDataLoader returns unexpected when fragment data has not "
          "socket data vertices",
          "[FlatbuffersDataLoader]") {
  steamrot::FlatbuffersDataLoader data_loader{steamrot::EnvironmentType::Test};
  auto result =
      data_loader.ProvideFragment("invalid_fragment_no_socket_data_vertices");
  REQUIRE(result.has_value() == false);
  REQUIRE(result.error().first ==
          steamrot::DataFailMode::FlatbufferDataNotFound);
  REQUIRE(result.error().second == "fragment socket data vertices not found");
}

TEST_CASE("FlatbuffersDataLoader returns unexpected when fragment data has "
          "invalid socket data vertices",
          "[FlatbuffersDataLoader]") {
  steamrot::FlatbuffersDataLoader data_loader{steamrot::EnvironmentType::Test};
  auto result = data_loader.ProvideFragment("missing_vertices_x");
  REQUIRE(result.has_value() == false);
  REQUIRE(result.error().first ==
          steamrot::DataFailMode::FlatbufferDataNotFound);
  REQUIRE(result.error().second == "vertex from socket data is incomplete");
}

TEST_CASE("FlatbuffersDataLoader returns unexpected when fragment data has "
          "no views",
          "[FlatbuffersDataLoader]") {
  steamrot::FlatbuffersDataLoader data_loader{steamrot::EnvironmentType::Test};
  auto result = data_loader.ProvideFragment("missing_render_views");
  REQUIRE(result.has_value() == false);
  REQUIRE(result.error().first ==
          steamrot::DataFailMode::FlatbufferDataNotFound);
  REQUIRE(result.error().second == "fragment render views not found");
}

TEST_CASE(
    "FlatbuffersDataLoader returns unexpected when a view has no triangles",
    "[FlatbuffersDataLoader]") {
  steamrot::FlatbuffersDataLoader data_loader{steamrot::EnvironmentType::Test};
  auto result = data_loader.ProvideFragment("missing_view_triangles");
  REQUIRE(result.has_value() == false);
  REQUIRE(result.error().first ==
          steamrot::DataFailMode::FlatbufferDataNotFound);
  REQUIRE(result.error().second == "view triangles not found");
}

TEST_CASE("FlatbuffersDataLoader returns unexpected when fragment triangles "
          "have zero vertices",
          "[FlatbuffersDataLoader]") {
  steamrot::FlatbuffersDataLoader data_loader{steamrot::EnvironmentType::Test};
  auto result = data_loader.ProvideFragment("missing_triangle_vertices");
  REQUIRE(result.has_value() == false);
  REQUIRE(result.error().first ==
          steamrot::DataFailMode::FlatbufferDataNotFound);
  REQUIRE(result.error().second == "fragment triangles must have 3 vertices");
}

TEST_CASE("FlatbuffersDataLoader returns unexpected when view has no direction",
          "[FlatbuffersDataLoader]") {
  steamrot::FlatbuffersDataLoader data_loader{steamrot::EnvironmentType::Test};
  auto result = data_loader.ProvideFragment("missing_view_direction");
  REQUIRE(result.has_value() == false);
  REQUIRE(result.error().first ==
          steamrot::DataFailMode::FlatbufferDataNotFound);
  REQUIRE(result.error().second == "view direction not found");
}
TEST_CASE("Fragment data provided with correct values",
          "[FlatbuffersDataLoader]") {
  steamrot::FlatbuffersDataLoader data_loader{steamrot::EnvironmentType::Test};
  auto result = data_loader.ProvideFragment("valid_fragment");
  if (!result.has_value()) {
    std::string error_message =
        result.error().second.empty() ? "unknown error" : result.error().second;
    FAIL(error_message);
  }

  // test expected values
  REQUIRE(result->m_name == "valid_fragment");
  REQUIRE(result->m_overlays.size() == 1);
  REQUIRE(result->m_overlays.contains(
      steamrot::ViewDirection::ViewDirection_FRONT));
  REQUIRE(result->m_overlays[steamrot::ViewDirection_FRONT].getVertexCount() ==
          3);
  REQUIRE(result->m_overlays[steamrot::ViewDirection_FRONT][0].position.x ==
          5.0f);
  REQUIRE(result->m_overlays[steamrot::ViewDirection_FRONT][0].position.y ==
          10.0f);
  REQUIRE(result->m_overlays[steamrot::ViewDirection_FRONT][0].color.r == 255);
  REQUIRE(result->m_overlays[steamrot::ViewDirection_FRONT][0].color.g == 255);
  REQUIRE(result->m_overlays[steamrot::ViewDirection_FRONT][0].color.b == 255);
  REQUIRE(result->m_overlays[steamrot::ViewDirection_FRONT][0].color.a == 255);

  REQUIRE(result->m_transform == sf::Transform::Identity);
  REQUIRE(result->m_sockets.size() == 1);
  REQUIRE(result->m_sockets[0].x == 5.0f);
  REQUIRE(result->m_sockets[0].y == 7.0f);
}

TEST_CASE("Given GrimoireMachina data return a unordered map with all "
          "requested Fragments",
          "[FlatbuffersDataLoader]") {
  steamrot::FlatbuffersDataLoader data_loader{steamrot::EnvironmentType::Test};
}
