#include "GameEngine.h"
#include <iostream> // For std::cerr
#include <cstdlib> // For std::exit

GameEngine::GameEngine()
    : window(sf::VideoMode(1600, 900), "SteamRot"),
    backgroundColor(sf::Color::Green),
    timeSinceLastUpdate(sf::Time::Zero),
    frames(0),
    updates(0),
    fpsUpdateTime(sf::Time::Zero) {

    // Use the defined macro to get the full path to the font
    std::string fontPath = std::string(ASSETS_PATH) + "/fonts/AmaticSC-Bold.ttf";

    // Load a font from the assets folder using the defined path
    if (!font.loadFromFile(fontPath)) {
        // Handle error
        std::cerr << "Failed to load font from " << fontPath << std::endl;
        std::exit(8008); // Exit with code 8008
    }

    // Set up FPS text
    fpsText.setFont(font);
    fpsText.setCharacterSize(20);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(window.getSize().x - 100, 10);

    // Set up UPS text
    upsText.setFont(font);
    upsText.setCharacterSize(20);
    upsText.setFillColor(sf::Color::White);
    upsText.setPosition(window.getSize().x - 100, 30);
}

void GameEngine::run() {
    while (window.isOpen()) {
        processEvents();

        // Measure elapsed time since last frame
        sf::Time elapsedTime = frameClock.restart();
        timeSinceLastUpdate += elapsedTime;
        fpsUpdateTime += elapsedTime;

        // Update as many times as necessary to catch up
        while (timeSinceLastUpdate >= TimePerUpdate) {
            timeSinceLastUpdate -= TimePerUpdate;
            update(TimePerUpdate);
            updates++;
        }

        render();
        frames++;

        // Update FPS and UPS every second
        if (fpsUpdateTime >= sf::seconds(1.0f)) {
            updateFPSandUPS(fpsUpdateTime);
            fpsUpdateTime -= sf::seconds(1.0f);
        }
    }
}

void GameEngine::runSimulation(int steps) {
    for (int i = 0; i < steps; ++i) {
        update(TimePerUpdate);
        logState(i); // Logging the state for debugging/testing
    }
}

void GameEngine::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

void GameEngine::update(sf::Time deltaTime) {
    // Update game state (e.g., handle logic, physics, etc.)
    // For now, we don't have much to update, but this is where it would go
}

void GameEngine::render() {
    window.clear(backgroundColor);

    // Draw the FPS and UPS text
    window.draw(fpsText);
    window.draw(upsText);

    window.display();

    // Ensure we limit the frame rate
    sf::sleep(TimePerFrame - frameClock.getElapsedTime());
}

void GameEngine::updateFPSandUPS(sf::Time elapsedTime) {
    // Calculate FPS and UPS
    float fps = static_cast<float>(frames) / elapsedTime.asSeconds();
    float ups = static_cast<float>(updates) / elapsedTime.asSeconds();

    // Update the text objects
    fpsText.setString("FPS: " + std::to_string(static_cast<int>(fps)));
    upsText.setString("UPS: " + std::to_string(static_cast<int>(ups)));

    // Reset the counters
    frames = 0;
    updates = 0;
}

void GameEngine::logState(int step) {
    // Example of logging internal state for testing
    std::cout << "Cheeky Game Update" << step << std::endl;
    // Log any relevant state information here
}