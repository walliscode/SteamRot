#include "GameEngine.h"
#include "PathProvider.h"
#include "spdlog/spdlog.h"
#include <iostream>
int main() {
  // start the logger

  // wrap the whole game engine in a try-catch block to catch any exceptions
  try {
    steamrot::PathProvider path_provider{steamrot::EnvironmentType::Production};
    steamrot::GameEngine steam_rot(steamrot::EnvironmentType::Production);
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
