#include "GameEngine.h"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

TEST_CASE("Simulation is run", "[GameEngine]") {

  std::cout << "********************** Simulation Tests *********************"
            << std::endl;
  GameEngine game;
  int num = 0;
  REQUIRE(num == 0);
  std::cout << "Running the simulation for 0 loops\n";
  REQUIRE_THROWS(game.runSimulation(
      0)); // Check that the simulation does not run if loops = 0
  REQUIRE(game.getLoopNumber() == 0);
  std::cout << "Running the simulation for -5 loops\n";
  REQUIRE_THROWS(game.runSimulation(
      -5)); // Check that the simulation does not run if loops < 0
  REQUIRE(game.getLoopNumber() == 0);

  std::cout << "Running the simulation for 10 loops\n";
  game.runSimulation(10);
  REQUIRE(game.getLoopNumber() ==
          10); // Check that the simulation runs for 10 loops
  //
  // std::cout << "Creating a JSON file and checking contents\n";
  // game.runSimulation(24);
  // json sim_json = game.extractJSON("simulations", "test_data");
  // REQUIRE(sim_json["GameEngine"]["loopNumber"] ==
  //         24); // Check that the loop number is 24
  //
  std::cout
      << "********************** Simulation Tests END *********************"
      << std::endl;
};
