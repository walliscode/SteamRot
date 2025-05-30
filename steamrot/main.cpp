#include "GameEngine.h"
#include "Logger.h"
#include "spdlog/spdlog.h"
#include <iostream>
int main() {
  // start the logger
  Logger logger("global_logger");
  std::cout << "Starting SteamRot Game Engine..." << std::endl;
  // wrap the whole game engine in a try-catch block to catch any exceptions
  try {
    steamrot::GameEngine steam_rot;
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
  // shut down the logger
  logger.CloseLogger();

  return 0;
}
