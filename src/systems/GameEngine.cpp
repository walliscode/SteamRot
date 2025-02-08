#include "GameEngine.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_iostream.hpp>
#include <stdexcept>

namespace fs = std::filesystem;

using namespace magic_enum::bitwise_operators;

GameEngine::GameEngine() : m_displayManager() {
  init(); // kick off the game, loading assets de.t.c
}

void GameEngine::init() {

  // print out the initial state of the m_event_flags enum
  using magic_enum::iostream_operators::operator<<;
  std::cout << "Initial Event flags: " << m_event_flags << std::endl;
  // create initial scenes
  // m_sceneManager.addScene("mainMenu", 10);
}

void GameEngine::run(size_t numLoops) {

  // Run the program as long as the window is open
  while (m_displayManager.GetWindow().isOpen()) {
    // increment the loop number by 1, the tick number is defined at the
    // beginning of the loop
    m_loopNumber++;
    // handle user input, this just effects the bitset, doesn't handle any
    // action
    GameEngine::sUserInput();
    // Update all the necessary components of the game
    GameEngine::update();

    // End the current frame and display its contents on screen
    m_displayManager.Cycle();

    // statement to test whether to break the loop, must be called at end
    if (numLoops > 0 && m_loopNumber >= numLoops) {
      // export data to json, first variable is the directory name, second is
      // the file name
      createJSON("simulations", "test_data");
      break;
    }
  }
}

void GameEngine::update() {

  // update display manager actions and call any logic systems
  m_displayManager.PopulateActions(m_event_flags);
  m_displayManager.Update();

  // call the update function of the scene manager
  // m_sceneManager.update();
}

void GameEngine::sUserInput() {
  // Check all the window's events that were triggered since the last iteration
  // of the loop
  while (const std::optional event = m_displayManager.GetWindow().pollEvent()) {
    // "close requested" event: we close the window
    if (event->is<sf::Event::Closed>()) {
      m_displayManager.GetWindow().close();
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

        // using magic_enum::iostream_operators::operator<<;
        // std::cout << "Event flag: " << event_flag.value() << std::endl;
        // std::cout << "Event flags: " << m_event_flags << std::endl;
        //
        // // cast m_event_flags to a bitset, this is for debugging purposes
        // int event_flags_bitset{static_cast<int>(m_event_flags)};
        // std::bitset<32> event_flags_bitset_bs(event_flags_bitset);
        // std::cout << "Event flags bitset: " << event_flags_bitset_bs
        //           << std::endl;
      }

      // some keys need to be handled separately as the physical key pressed and
      // system interpretation are different check if alt key is pressed
      if (keyPressed->alt) {
        std::cout << "Alt key pressed" << std::endl;
        m_event_flags |= EventFlags::ALT;
      }

      // check if control key is pressed
      if (keyPressed->control) {
        std::cout << "Control key pressed" << std::endl;
        m_event_flags |= EventFlags::CONTROL;
      }

    }

    // handle key released events
    else if (const auto *keyReleased = event->getIf<sf::Event::KeyReleased>()) {
      // get the description of the key released
      std::string key_name =
          sf::Keyboard::getDescription(keyReleased->scancode);
      std::cout << "Key Released: " << key_name << std::endl;

      // check if this string matches any of the event flags
      auto event_flag = magic_enum::enum_cast<EventFlags>(key_name);

      // if it matches, then flip flag to off (0)
      if (event_flag.has_value()) {
        m_event_flags &= ~event_flag.value();
        // using magic_enum::iostream_operators::operator<<;
        // std::cout << "Event flag: " << event_flag.value() << std::endl;
        // std::cout << "Event flags: " << m_event_flags << std::endl;
        //
        // // cast m_event_flags to a bitset, this is for debugging purposes
        // int event_flags_bitset{static_cast<int>(m_event_flags)};
        // std::bitset<32> event_flags_bitset_bs(event_flags_bitset);
        // std::cout << "Event flags bitset: " << event_flags_bitset_bs
        //           << std::endl;
      }

      // add in special handling for alt and control keys
      if (keyReleased->alt) {
        std::cout << "Alt key released" << std::endl;
        m_event_flags &= ~EventFlags::ALT;
      }

      if (keyReleased->control) {
        std::cout << "Control key released" << std::endl;
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

size_t GameEngine::getLoopNumber() { return m_loopNumber; }

void GameEngine::runSimulation(int loops) {
  // prevent undefined behaviour of simulation loop
  if (loops <= 0) {
    throw std::invalid_argument("The number of loops must be greater than 0");
  } else {
    GameEngine::run(loops);
  }
}

json GameEngine::toJSON() {
  json j; // create a json object
  j["loopNumber"] = m_loopNumber;
  return j;
}

void GameEngine::createJSON(const std::string &directoryName,
                            const std::string &fileName) {
  std::string fullFileName = fileName + ".json";
  // define the path to the directory
  fs::path filePath = fs::path(DATA_OUT_DIR) / directoryName / fullFileName;

  std::cout << "Creating JSON file: " << filePath << std::endl;
  // create the directory if it does not exist
  fs::create_directories(filePath.parent_path());

  // create ofstream object
  std::ofstream jsonFile(filePath);

  // check if the file is open
  if (jsonFile.is_open()) {
    // create json object to add other json objects to

    json mainJson;

    // add the json object to the main json object
    // mainJson["GameEngine"] = GameEngine::toJSON();
    // mainJson["SceneManager"] = m_sceneManager.toJSON();
    // mainJson["AssetManager"] = m_sceneManager.getAssetManager().ToJSON();
    //
    // write the json object to the file
    jsonFile << mainJson.dump(4);
    // close the file
    jsonFile.close();

    std::cout << "JSON file created successfully" << std::endl;
  } else {
    throw std::runtime_error("Could not open file");
  }
}

json GameEngine::extractJSON(const std::string &directoryName,
                             const std::string &fileName) {
  // define the full file name, adding the json extension
  std::string fullFileName = fileName + ".json";
  // define the path to the directory
  fs::path filePath = fs::path(DATA_OUT_DIR) / directoryName / fullFileName;

  std::cout << "File Path: " << filePath << std::endl;
  // create ifstream object
  std::ifstream jsonFile(filePath);

  // check if file exists
  if (!fs::exists(filePath)) {
    throw std::runtime_error("File does not exist");
  }
  // check if the file is open
  if (jsonFile.is_open()) {
    //
    json j;
    jsonFile >> j;
    return j;
  } else {
    throw std::runtime_error("Could not open file");
  }
}
