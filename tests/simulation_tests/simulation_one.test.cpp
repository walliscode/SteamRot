#include "GameEngine.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

TEST_CASE("simple simulation test", "[simulation]") {
  // set up resources to test
  GameEngine game;
  game.runSimulation(10);

  // import the test json file
  fs::path json_file_path = fs::path(DATA_OUT_DIR) / "test.json";
  std::ifstream f(json_file_path);
  json simulation_data = json::parse(f);

  // check GameEngine data
  json game_engine_data = simulation_data["GameEngine"];
  REQUIRE(game_engine_data["m_loop_number"] == 10);
  REQUIRE(game_engine_data["m_event_flags"] == 0);
};
