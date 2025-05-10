// src/main.cpp


#include <iostream>
#include <thread>
#include <chrono>


#include "SFML/Graphics.hpp"
// #include "vendor/imgui-sfml-3/imgui-SFML.h"


using namespace std::chrono_literals;


sf::Sprite alignSprite();


int main(int argc, char* argv[]) {
    std::string imageFile;
    
    // File name
    if (argc < 2) {
        imageFile = "example.png";
    } else {
        imageFile = argv[1];
    }

    sf::ContextSettings settings;
    settings.antiAliasingLevel = 400;

    // Create the window
    sf::RenderWindow mainWindow(sf::VideoMode({1920/2, 1080/2}), "F1-Player",sf::State::Windowed ,  settings);
    mainWindow.setFramerateLimit(60);

    

    // Load the image and create the texture
    sf::Image image;
    if (!image.loadFromFile(imageFile)) {
        std::cerr << "Error loading image file!" << std::endl;
        return -1; // Return an error code if loading fails
    }


    sf::Texture texture;
    if (!texture.loadFromImage(image)) {
        std::cerr << "Error loading texture from image!" << std::endl;
        return -1;
    }

    std::this_thread::sleep_for(300ms);

    // Main loop
    while (mainWindow.isOpen()) {
        // Handle events
        while ( std::optional event = mainWindow.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                mainWindow.close();
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Escape)) {
                mainWindow.close();
            }
        }

        sf::Vector2i mousePosition = sf::Mouse::getPosition(mainWindow);

        // Draw the texture in the window
        sf::Sprite sprite(texture);


        // --------------------------------------------------------------<<
        
        sprite.setOrigin({0, 0});
        sprite.setPosition({0, 0});

        

        sprite.setScale({
            mainWindow.getSize().x/texture.getSize().x,
            mainWindow.getSize().y/texture.getSize().y });

        if (mainWindow.getSize().x <= texture.getSize().x
        or  mainWindow.getSize().y <= texture.getSize().y) {
            texture.resize(mainWindow.getSize());
            sprite.setTexture(texture);    
        } 
            
        // sprite.setOrigin(sprite.Gety)

        

        // --------------------------------------------------------------<<


        // sprite.setOrigin((sf::Vector2f)mousePosition);       // {mainWindow.getSize().x/2, mainWindow.getSize().y/2}

        // Center the sprite in the window
        // sprite.setPosition({
        //     ((mainWindow.getSize().x - texture.getSize().x) * sprite.getScale().x) / 2.0f,
        //     ((mainWindow.getSize().y - texture.getSize().y) * sprite.getScale().y) / 2.0f
        // });


        // // Scale the sprite to fit the window size
        // sprite.setScale({
        //     (float)(mainWindow.getSize().x) / texture.getSize().x,
        //     (float)(mainWindow.getSize().y) / texture.getSize().y
        // });
        

        // Clear the window
        mainWindow.clear();
        
        // Draw the sprite
        mainWindow.draw(sprite);

        // Swap the back buffer
        mainWindow.display();
    }

    std::cout << "Done" << "\n\n";
}

sf::Sprite alignSprite(sf::Sprite& sprite, const sf::Vector2u windowSize) {

}