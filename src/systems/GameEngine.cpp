#include "GameEngine.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace fs = std::filesystem;

GameEngine::GameEngine() : m_displayManager() {
  init(); // kick off the game, loading assets de.t.c
}

void GameEngine::init() {

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
  m_displayManager.PopulateActions(m_userInput);
  m_displayManager.Update();

  // call the update function of the scene manager
  // m_sceneManager.update();
}

void GameEngine::sUserInput() {
  // Check all the window's events that were triggered since the last iteration
  // of the loop
  while (const std::optional event = m_displayManager.GetWindow().pollEvent()) {
    // "close requested" event: we close the window
    if (event->is<sf::Event::Closed>())
      m_displayManager.GetWindow().close();

    // handle key pressed events
    else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
      // set position on the user input bitset
      size_t key_code{static_cast<size_t>(keyPressed->scancode)};
      m_userInput.set(key_code);
      // std::cout << "Key Pressed: " << key_code << std::endl;
      // std::cout << "User Input: " << m_userInput << std::endl;
    }

    // handle key released events
    else if (const auto *keyReleased = event->getIf<sf::Event::KeyReleased>()) {
      // set position on the user input bitset
      size_t key_code{static_cast<size_t>(keyReleased->scancode)};
      m_userInput.reset(key_code);
      // std::cout << "Key Released: " << key_code << std::endl;
      // std::cout << "User Input: " << m_userInput << std::endl;
    }

    // handle mouse button pressed events
    else if (const auto *mouseButtonPressed =
                 event->getIf<sf::Event::MouseButtonPressed>()) {
      // set position on the user input bitset, shift by number of keys
      size_t mouse_button{static_cast<size_t>(mouseButtonPressed->button) +
                          static_cast<size_t>(sf::Keyboard::KeyCount)};
      m_userInput.set(mouse_button);
      // std::cout << "Mouse Button Pressed: " << mouse_button << std::endl;
      // std::cout << "User Input: " << m_userInput << std::endl;
    }

    // handle mouse button released events
    else if (const auto *mouseButtonReleased =
                 event->getIf<sf::Event::MouseButtonReleased>()) {
      // set position on the user input bitset, shift by number of keys
      size_t mouse_button{static_cast<size_t>(mouseButtonReleased->button) +
                          static_cast<size_t>(sf::Keyboard::KeyCount)};

      m_userInput.reset(mouse_button);
      // std::cout << "Mouse Button Released: " << mouse_button << std::endl;
      // std::cout << "User Input: " << m_userInput << std::endl;
    }
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
