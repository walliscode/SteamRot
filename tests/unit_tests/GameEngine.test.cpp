#include "GameEngine.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

TEST_CASE("Simulation is run", "[GameEngine]") {

  // set up resources to test (this should just be the whole game)
  GameEngine game;
  game.runSimulation(10);

  // import json file and check that the loop number is 10

  fs::path json_file_path = fs::path(DATA_OUT_DIR) / "test.json";
  std::ifstream f(json_file_path);
  json j = json::parse(f);

  // first test strucutre of json is correct
  REQUIRE(j.contains("GameEngine"));
  REQUIRE(j.at("GameEngine").contains("m_loop_number"));
  REQUIRE(j["GameEngine"]["m_loop_number"] == 10);
};
