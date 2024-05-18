#include "GameEngine.h"
#include <iostream>

GameEngine::GameEngine()
    : window(sf::VideoMode(1600, 900), "Game Engine"), 
      backgroundColor(sf::Color(35, 135, 35)) {
}

void GameEngine::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        processEvents();
        update(clock.restart());
        render();
    }
}

void GameEngine::runSimulation(int steps) {
    sf::Clock clock;
    for (int i = 0; i < steps; ++i) {
        update(clock.restart());
        logState(i); // Logging the state for debugging/testing
    }
}

void GameEngine::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
}

void GameEngine::update(sf::Time deltaTime) {
    // update game state
}

void GameEngine::render() {
    window.clear(backgroundColor);
    window.display();
}

void GameEngine::logState(int step) {
    // Example of logging internal state for testing
    std::cout << "Cheeky Game Update" << step << std::endl;
    // Log any relevant state information here
}