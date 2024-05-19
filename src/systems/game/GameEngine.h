#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class GameEngine {
public:
    GameEngine();
    void run();
    void runSimulation(int steps);

private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();
    void updateFPSandUPS(sf::Time elapsedTime);
    void logUpdate(int step); // For logging updates during simulation
    void runStep(sf::Time elapsedTime, bool log = false, int step = 0); // Common logic for both run and runSimulation

    sf::RenderWindow window;
    sf::Color backgroundColor;

    const sf::Time TimePerFrame = sf::seconds(1.f / 60.f); // 60 frames per second
    const sf::Time TimePerUpdate = sf::seconds(1.f / 30.f); // 30 updates per second

    sf::Clock frameClock;
    sf::Time timeSinceLastUpdate;
    sf::Clock renderClock;

    // For displaying UPS and FPS
    sf::Font font;
    sf::Text fpsText;
    sf::Text upsText;

    int frames = 0;
    int updates = 0;
    sf::Time fpsUpdateTime;
};