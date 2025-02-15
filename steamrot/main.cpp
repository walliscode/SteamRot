#include "GameEngine.h"
#include <iostream>
int main() {
  GameEngine game;
  bool use_test_window = true;
  game.run(0, use_test_window);
  std::cout << "SteamRot fucks!" << std::endl;
  return 0;
}
