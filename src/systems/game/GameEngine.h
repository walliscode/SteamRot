#pragma once

#include <SFML/Graphics.hpp>

class GameEngine {
public:
    GameEngine();
    void run();
    void runSimulation(int steps);

private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();
    void logState(int step); // For logging state during simulation

    sf::RenderWindow window;
    sf::Color backgroundColor;
};