#include "GameEngine.h"
#include "GamePaths.h"
#include "spdlog/spdlog.h"
#include <iostream>
int main() {
  // start the logger

  // wrap the whole game engine in a try-catch block to catch any exceptions
  try {
    steamrot::GamePaths game_paths;
    steamrot::GameEngine steam_rot(game_paths);
    steam_rot.RunGame();

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
