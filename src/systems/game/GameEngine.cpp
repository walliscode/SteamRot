#include "GameEngine.h"
#include <iostream>
#include <cstdlib> // For std::exit

GameEngine::GameEngine()
    : window(sf::VideoMode(1600, 900), "SteamRot"),
    backgroundColor(sf::Color(35, 135, 35)),
    timeSinceLastUpdate(sf::Time::Zero),
    frames(0),
    updates(0),
    fpsUpdateTime(sf::Time::Zero) {

    //get full path to the font
    std::string fontPath = std::string(ASSETS_PATH) + "/fonts/AmaticSC-Bold.ttf";

    //Load font from assets folder using the path
    if (!font.loadFromFile(fontPath)) {
        // Handle error
        std::cerr << "Failed to load font from " << fontPath << std::endl;
        std::exit(8008);
    }

    //FPS text
    fpsText.setFont(font);
    fpsText.setCharacterSize(30);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(window.getSize().x - 80, 10);

    //UPS text
    upsText.setFont(font);
    upsText.setCharacterSize(30);
    upsText.setFillColor(sf::Color::White);
    upsText.setPosition(window.getSize().x - 80, 40);
}

//Normal Game Running
void GameEngine::run() {
    sf::Clock clock;
    sf::Time timeSinceLastRender = sf::Time::Zero;

    while (window.isOpen()) {
        sf::Time elapsedTime = clock.restart();
        timeSinceLastUpdate += elapsedTime;
        timeSinceLastRender += elapsedTime;
        fpsUpdateTime += elapsedTime;

        processEvents();

        //Update to catch up
        while (timeSinceLastUpdate >= TimePerUpdate) {
            timeSinceLastUpdate -= TimePerUpdate;
            update(TimePerUpdate);
            updates++;
        }

        //Render at limited frame rate
        if (timeSinceLastRender >= TimePerFrame) {
            render();
            frames++;
            timeSinceLastRender -= TimePerFrame;
        }

        //Update FPS and UPS values every second
        if (fpsUpdateTime >= sf::seconds(1.0f)) {
            updateFPSandUPS(fpsUpdateTime);
            fpsUpdateTime -= sf::seconds(1.0f);
        }

        //Ensure frame rate is limited
        sf::Time sleepTime = TimePerFrame - clock.getElapsedTime();
        if (sleepTime > sf::Time::Zero) {
            sf::sleep(sleepTime);
        }
    }
}

//Simulation Game Running
void GameEngine::runSimulation(int steps) {
    sf::Clock clock;
    sf::Time timeSinceLastRender = sf::Time::Zero;

    for (int i = 0; i < steps; ++i) {
        sf::Time elapsedTime = clock.restart();
        timeSinceLastUpdate += elapsedTime;
        timeSinceLastRender += elapsedTime;
        fpsUpdateTime += elapsedTime;

        processEvents();

        //Update to catch up
        while (timeSinceLastUpdate >= TimePerUpdate) {
            timeSinceLastUpdate -= TimePerUpdate;
            update(TimePerUpdate);
            updates++;
            logUpdate(i); // Log the update
        }

        //Render at a limited frame rate
        if (timeSinceLastRender >= TimePerFrame) {
            render();
            frames++;
            timeSinceLastRender -= TimePerFrame;
        }

        //Update FPS and UPS values every second
        if (fpsUpdateTime >= sf::seconds(1.0f)) {
            updateFPSandUPS(fpsUpdateTime);
            fpsUpdateTime -= sf::seconds(1.0f);
        }

        //Ensure frame rate is limited
        sf::Time sleepTime = TimePerFrame - clock.getElapsedTime();
        if (sleepTime > sf::Time::Zero) {
            sf::sleep(sleepTime);
        }
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
    // Update game (handle logic, physics, etc.)
    
}

void GameEngine::render() {
    window.clear(backgroundColor);

    // Draw FPS and UPS text
    window.draw(fpsText);
    window.draw(upsText);

    window.display();
}

void GameEngine::updateFPSandUPS(sf::Time elapsedTime) {
    // Calculate FPS and UPS
    float fps = static_cast<float>(frames) / elapsedTime.asSeconds();
    float ups = static_cast<float>(updates) / elapsedTime.asSeconds();

    // Update FPS and UPS text objects
    fpsText.setString("FPS: " + std::to_string(static_cast<int>(fps)));
    upsText.setString("UPS: " + std::to_string(static_cast<int>(ups)));

    // Reset counters
    frames = 0;
    updates = 0;
}

void GameEngine::logUpdate(int step) {
    // Log update info:
    std::cout << "Cheeky Game Update: " << step << std::endl;
}