////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "GameEngine.h"

#include "log_handler.h"
#include "spdlog/common.h"

#include <SFML/Graphics.hpp>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_iostream.hpp>
#include <memory>
#include <stdexcept>

namespace fs = std::filesystem;

using namespace magic_enum::bitwise_operators;

namespace steamrot {

///////////////////////////////////////////////////////////
GameEngine::GameEngine()
    : m_display_manager(), m_logger("global_logger"), m_event_handler() {

  // set up data manager so it can be passed to scene manager
  m_data_manager = std::make_shared<DataManager>();
  m_scene_manager = std::make_unique<SceneManager>(m_data_manager);

  log_handler::ProcessLog(spdlog::level::level_enum::info,
                          log_handler::LogCode::kNoCode,
                          "GameEngine constructor called");
}

////////////////////////////////////////////////////////////
void GameEngine::RunGame(size_t numLoops, bool use_test_window) {
  // set up resources
  m_scene_manager->StartUp();

  // Run the program as long as the window is open
  while (m_display_manager.GetWindow().isOpen()) {

    // handle loop number increase at beginning of loop
    m_loop_number++;

    // Handle events
    m_event_handler.HandleEvents(m_display_manager.GetWindow());

    // Handle all system updates
    UpdateSystems();

    // Render all game drawables
    sRender();

    // statement to test whether to break the loop, must be called at end
    if (numLoops > 0 && m_loop_number >= numLoops) {
      // export data to json, first variable is the directory name, second is
      // the file name
      ExportSimulationData("test");
      break;
    };
  }
  // Shut down the game engine
  ShutDown();
};

////////////////////////////////////////////////////////////
void GameEngine::UpdateSystems() {

  // update display manager actions and call any logic systems

  m_display_manager.Update();

  // call the update function of the scene manager
  m_scene_manager->UpdateScenes();
}

////////////////////////////////////////////////////////////
void GameEngine::sRender() {

  // textures package should only live for the duration of the render call so
  // called by value
  TexturesPackage textures_package = m_scene_manager->ProvideTexturesPackage();
  m_display_manager.Render(textures_package);
};

////////////////////////////////////////////////////////////
size_t GameEngine::getLoopNumber() { return m_loop_number; }

////////////////////////////////////////////////////////////
void GameEngine::RunSimulation(int loops) {
  // prevent undefined behaviour of simulation loop
  if (loops <= 0) {
    throw std::invalid_argument("The number of loops must be greater than 0");
  } else {
    GameEngine::RunGame(loops);
  }
}

////////////////////////////////////////////////////////////
void to_json(json &j, const GameEngine &ge) {
  j = json{{"GameEngine",
            {{"m_loop_number", ge.m_loop_number},

             {"m_scene_manager", *ge.m_scene_manager}}}

  };
};

////////////////////////////////////////////////////////////
void GameEngine::ExportSimulationData(const std::string &file_name) {

  // create directory if it does not exist
  fs::create_directories(DATA_OUT_DIR);

  // create file name with json extension
  std::string full_file_name = file_name + ".json";

  // create path to file
  fs::path json_file_path = fs::path(DATA_OUT_DIR) / full_file_name;

  // create ifstream object
  std::ofstream json_file(json_file_path);

  // check if file exists
  if (!fs::exists(json_file_path)) {
    throw std::runtime_error("File does not exist");
  }

  // add to open file
  if (json_file.is_open()) {

    // create json object
    json j = *this;

    // write json object to file
    json_file << j;
    // close file
    json_file.close();
  } else {
    throw std::runtime_error("Could not open file");
  }
}

void GameEngine::ShutDown() {

  // shut down global logger
  m_logger.CloseLogger();
}

} // namespace steamrot
