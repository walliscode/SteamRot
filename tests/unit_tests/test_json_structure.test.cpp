#include "GameEngine.h"
#include "TestUtility.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

// the GameEngine ExtractJSON will output the whole game state to a json file
// this test is to check that the outputted json file has the correct structure
// NOT to check the values of the data
//
// Each time a new data type is extracted, this test should be updated
TEST_CASE("test json structure", "[data_out]") {

  // set up resources to test (this should just be the whole game)
  GameEngine game;
  game.runSimulation(10);

  // define json structure to test against
  json defined_structure = {
      {"GameEngine",
       {{"m_loop_number", nullptr},
        {"m_event_flags", nullptr},
        {"m_scene_manager",
         {{"type", nullptr},
          {"m_all_scenes",
           {{"size", nullptr}, {"scenes", json::array({"name", nullptr})}}},
          {"m_active_scenes", {{"size", nullptr}}},
          {"m_inactive_scenes", {{"size", nullptr}}},
          {"m_interactive_scenes", {{"size", nullptr}}}}}}}};

  // import the test json file
  fs::path json_file_path = fs::path(DATA_OUT_DIR) / "test.json";
  std::ifstream f(json_file_path);
  json test_structure = json::parse(f);

  // check the structure using a recurvisve function
  // The REQUIRE marco is built into the function
  TestUtility::check_structure(defined_structure, test_structure);
};
