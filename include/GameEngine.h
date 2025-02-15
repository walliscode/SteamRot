#include "DisplayManager.h"
#include "EventFlags.h"
#include "SceneManager.h"
#include <SFML/Graphics.hpp>

// #include <map>
#include <nlohmann/json.hpp>

// typedef std::map<std::string, std::shared_ptr<Scene>> SceneList;

class GameEngine {
private:
  DisplayManager m_displayManager;
  SceneManager m_sceneManager;

  size_t m_loopNumber = 0;

  // event management
  void sUserInput();
  EventFlags m_event_flags{0};

  // test window, for sepearating out the display manager from the scene
  // drawables will probably be removed but this allows for separation of
  // concerns
  sf::RenderWindow m_test_window;

public:
  GameEngine();
  void init();
  void run(size_t numLoops = 0, bool use_test_window = false);
  void update();

  void test_render(std::map<std::string, SceneDrawables> &test_drawables);

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
