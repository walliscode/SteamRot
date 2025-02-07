#include "DisplayManager.h"
#include <SFML/Graphics.hpp>

// #include <map>
#include <nlohmann/json.hpp>

// typedef std::map<std::string, std::shared_ptr<Scene>> SceneList;

class GameEngine {
private:
  DisplayManager m_displayManager;
  // SceneManager m_sceneManager;
  // SceneList m_scenes;
  size_t m_loopNumber = 0;

  // event management
  void sUserInput();
  std::bitset<SteamRot::kUserInputCount> m_userInput;

public:
  GameEngine();
  void init();
  void run(size_t numLoops = 0);
  void update();

  // ######### Simulation Functions #########
  size_t getLoopNumber();
  void runSimulation(int loops);
  json toJSON();

  void createJSON(const std::string &directoryName,
                  const std::string &fileName);
  json extractJSON(const std::string &directoryName,
                   const std::string &fileName);

  // ######### Scene Management Functions #########
  // SceneManager &getSceneManager(); // Get the scene manager
};
