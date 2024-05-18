
#include "systems/game/GameEngine.h"

int main() {
    GameEngine engine;
    // NORM //
    //engine.run();

    // TEST //
    engine.runSimulation(10000);
    //e.g. run the game for 10000 updates/steps

    return 0;
}