
#include "GameEngine.h"
#include "spdlog/spdlog.h"
#include <iostream>
int main() {
  // start the logger

  // wrap the whole game engine in a try-catch block to catch any exceptions
  try {

    // create the game engine
    steamrot::GameEngine game_engine;
    // run the game engine
    game_engine.RunGame();

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
