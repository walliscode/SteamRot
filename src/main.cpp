
#include "systems/game/GameEngine.h"

int main() {
    GameEngine engine;
    // NORM //
    //engine.run();

    // TEST //
    engine.runSimulation(1000);
    //e.g. run the game for 1000 updates/steps

    return 0;
}