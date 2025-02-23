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

  // check SceneManager data
  json scene_manager_data = game_engine_data["m_scene_manager"];
  REQUIRE(scene_manager_data["type"] == "SceneManager");
  REQUIRE(scene_manager_data["m_all_scenes"]["size"] == 1);
  REQUIRE(scene_manager_data["m_active_scenes"]["size"] == 1);
  REQUIRE(scene_manager_data["m_inactive_scenes"]["size"] == 0);
  REQUIRE(scene_manager_data["m_interactive_scenes"]["size"] == 0);
};
