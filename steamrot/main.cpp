#include "GameEngine.h"
#include <iostream>
int main() {
  GameEngine game;
  game.run();
  game.ExportJSON(std::string("test"));
  std::cout << "SteamRot fucks!" << std::endl;
  return 0;
}
