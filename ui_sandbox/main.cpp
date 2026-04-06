/////////////////////////////////////////////////
/// @file
/// @brief Entry point for the UI Explorer sandbox executable.
///
/// Launch with:
///   ./UISandbox
///
/// Controls:
///   Mouse click on selector entry → select that element
///   Left / Right arrow keys       → previous / next element
///   R key                         → reset current element to defaults
///   S key                         → save canvas render to PNG
///   ↑↓ arrow keys                 → pan canvas vertically
///   Window close / Alt+F4         → exit
/////////////////////////////////////////////////

#include "SandboxEngine.h"
#include "error_loop.h"
#include "spdlog/spdlog.h"
#include <iostream>

int main() {
  try {
    steamrot::SandboxEngine sandbox_engine;

    auto startup_result = sandbox_engine.StartUp();
    if (!startup_result) {
      std::cerr << "Error: " << startup_result.error().message << std::endl;
      error_loop::RunErrorLoop(startup_result.error());
      return 1;
    }

    auto run_result = sandbox_engine.RunGame();
    if (!run_result) {
      std::cerr << "Error: " << run_result.error().message << std::endl;
      error_loop::RunErrorLoop(run_result.error());
    }

  } catch (const std::exception &e) {
    std::cerr << "Exception caught: " << e.what() << std::endl;
    spdlog::get("global_logger")->error("Exception: {}", e.what());
  } catch (...) {
    std::cerr << "Unknown exception caught" << std::endl;
    spdlog::get("global_logger")->error("Unknown exception occurred");
  }

  return 0;
}
