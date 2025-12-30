
#include "GameEngine.h"
#include "error_loop.h"
#include "spdlog/spdlog.h"
#include <iostream>
int main() {
  // start the logger

  // wrap the whole game engine in a try-catch block to catch any exceptions
  try {

    // create the game engine
    steamrot::GameEngine game_engine;
    // run the game engine
    auto run_game_result = game_engine.RunGame();
    if (!run_game_result) {

      // print error message to console
      std::cerr << "Error: " << run_game_result.error().message << std::endl;
      // run the error loop with the captured fail info
      error_loop::RunErrorLoop(run_game_result.error());
    }

  } catch (const std::exception &e) {
    // log the exception message

    std::cerr << "Exception caught: " << e.what() << std::endl;
    spdlog::get("global_logger")->error("Exception: {}", e.what());
  } catch (...) {
    // log an unknown exception

    std::cerr << "Unknown exception caught" << std::endl;
    spdlog::get("global_logger")->error("Unknown exception occurred");
  }

  return 0;
}
