////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "GameEngine.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <SFML/Window/VideoMode.hpp>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_iostream.hpp>
#include <stdexcept>

namespace fs = std::filesystem;

using namespace magic_enum::bitwise_operators;

////////////////////////////////////////////////////////////
GameEngine::GameEngine() : m_displayManager(), m_scene_manager() {}

////////////////////////////////////////////////////////////
void GameEngine::RunGame(size_t numLoops, bool use_test_window) {

  // create test window if use_test_window is true
  if (use_test_window) {
    m_test_window = sf::RenderWindow(sf::VideoMode({800, 600}), "Test Window");
  }

  // Run the program as long as the window is open
  while (m_displayManager.GetWindow().isOpen()) {

    // handle loop number increase at beginning of loop
    m_loop_number++;

    // Handle external input
    GameEngine::sUserInput();

    // Handle all system updates
    GameEngine::UpdateSystems();

    // call all the necessary drawables and pass to display manager
    std::map<std::string, SceneDrawables> game_drawables =
        m_scene_manager.ProvideSceneDrawables();

    if (use_test_window) {
      TestRender(game_drawables);
    } else {
      // End the current frame and display its contents on screen
      m_displayManager.Cycle(game_drawables);
    }
    // statement to test whether to break the loop, must be called at end
    if (numLoops > 0 && m_loop_number >= numLoops) {
      // export data to json, first variable is the directory name, second is
      // the file name
      ExportSimulationData("test");
      break;
    }
  }
}

////////////////////////////////////////////////////////////
void GameEngine::UpdateSystems() {

  // update display manager actions and call any logic systems
  m_displayManager.PopulateActions(m_event_flags);
  m_displayManager.Update();

  // call the update function of the scene manager
  m_scene_manager.update();
}

////////////////////////////////////////////////////////////
void GameEngine::sUserInput() {

  // Check all the window's events that were triggered since the last iteration
  // of the loop
  while (const std::optional event = m_displayManager.GetWindow().pollEvent()) {

    // "close requested" event: we close the window
    if (event->is<sf::Event::Closed>()) {
      m_displayManager.GetWindow().close();
      m_test_window.close();
    }

    // handle key pressed events
    else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {

      // get the description of the key pressed
      std::string key_name = sf::Keyboard::getDescription(keyPressed->scancode);

      // check if this string matches any of the event flags
      auto event_flag = magic_enum::enum_cast<EventFlags>(key_name);

      // if the event_flag has a value (i.e. the key pressed has a match in the
      // enum), flip flag to on
      if (event_flag.has_value()) {
        m_event_flags |= event_flag.value();
      }

      // some keys need to be handled separately as the physical key pressed and
      // system interpretation are different check if alt key is pressed
      if (keyPressed->alt) {

        m_event_flags |= EventFlags::ALT;
      }

      // check if control key is pressed
      if (keyPressed->control) {

        m_event_flags |= EventFlags::CONTROL;
      }

    }

    // handle key released events
    else if (const auto *keyReleased = event->getIf<sf::Event::KeyReleased>()) {
      // get the description of the key released
      std::string key_name =
          sf::Keyboard::getDescription(keyReleased->scancode);

      // check if this string matches any of the event flags
      auto event_flag = magic_enum::enum_cast<EventFlags>(key_name);

      // if it matches, then flip flag to off (0)
      if (event_flag.has_value()) {
        m_event_flags &= ~event_flag.value();
      }

      // add in special handling for alt and control keys
      if (keyReleased->alt) {
        m_event_flags &= ~EventFlags::ALT;
      }

      if (keyReleased->control) {

        m_event_flags &= ~EventFlags::CONTROL;
      }
    }

    // // handle mouse button pressed events
    // else if (const auto *mouseButtonPressed =
    //              event->getIf<sf::Event::MouseButtonPressed>()) {
    //   // set position on the user input bitset, shift by number of keys
    //   size_t mouse_button{static_cast<size_t>(mouseButtonPressed->button) +
    //                       static_cast<size_t>(sf::Keyboard::KeyCount)};
    //   m_userInput.set(mouse_button);
    //   // std::cout << "Mouse Button Pressed: " << mouse_button << std::endl;
    //   // std::cout << "User Input: " << m_userInput << std::endl;
    // }
    //
    // // handle mouse button released events
    // else if (const auto *mouseButtonReleased =
    //              event->getIf<sf::Event::MouseButtonReleased>()) {
    //   // set position on the user input bitset, shift by number of keys
    //   size_t mouse_button{static_cast<size_t>(mouseButtonReleased->button) +
    //                       static_cast<size_t>(sf::Keyboard::KeyCount)};
    //
    //   m_userInput.reset(mouse_button);
    //   // std::cout << "Mouse Button Released: " << mouse_button << std::endl;
    //   // std::cout << "User Input: " << m_userInput << std::endl;
    // }
  }
};
// SceneManager &GameEngine::getSceneManager() { return m_sceneManager; }

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
             {"m_event_flags", ge.m_event_flags},
             {"m_scene_manager", ge.m_scene_manager}}}

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

void GameEngine::TestRender(
    std::map<std::string, SceneDrawables> &test_drawables) {
  // clear the test window
  m_test_window.clear(sf::Color::Red);
  // cycle through and draw to test window
  //
  // for (auto &scene_drawables : test_drawables) {
  //   for (auto &drawable_iter : scene_drawables.second) {
  //     m_test_window.draw(*drawable_iter);
  //   }
  // }
  //
  m_test_window.display();
}
