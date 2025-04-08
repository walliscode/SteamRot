#include "DisplayManager.h"
#include "EventFlags.h"
#include "SceneManager.h"
#include <SFML/Graphics.hpp>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class GameEngine {
private:
  DisplayManager m_displayManager;
  SceneManager m_scene_manager;

  size_t m_loop_number = 0;

  // event management
  void sUserInput();
  EventFlags m_event_flags{0};

  // test window, for sepearating out the display manager from the scene
  // drawables will probably be removed but this allows for separation of
  // concerns
  sf::RenderWindow m_test_window;

  friend void to_json(json &j, const GameEngine &ge);

public:
  GameEngine();

  void RunGame(size_t numLoops = 0, bool use_test_window = false);
  void Update();

  void test_render(std::map<std::string, SceneDrawables> &test_drawables);

  // ######### Simulation Functions #########
  size_t getLoopNumber();
  void runSimulation(int loops);
  void ExportJSON(const std::string &file_name);

  // ######### Scene Management Functions #########
  // SceneManager &getSceneManager(); // Get the scene manager
};

// json functions that nlohmann needs to convert the class to json
void to_json(json &j, const GameEngine &ge);
