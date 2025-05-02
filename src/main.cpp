// src/main.cpp
// clang++ --target=x86_64-w64-mingw32 -o main .\src\main.cpp -Ivendor\include -Lvendor\lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -DSFML_STATIC -lwinmm -lopengl32 -lgdi32 -lfreetype -std=c++23 --static -pthread


#include <iostream>
#include <thread>
#include <chrono>

#include "SFML/Graphics.hpp"

using namespace std::chrono_literals;


int main() {

    // File name
    int width, height, channels;
    const char* imageFile = "example.png";

    sf::RenderWindow mainWindow(sf::VideoMode( {1920/2, 1080/2} ), "F1-Player");
    mainWindow.setFramerateLimit(60);


    sf::Image image(imageFile);
    sf::Texture texture(image);

    while (mainWindow.isOpen()) {

        // Handel the events.
        while (std::optional event =  mainWindow.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                mainWindow.close();
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Escape)) {
                mainWindow.close();
            }
        }

        // Drawing in back buffer
        sf::Sprite sprite(texture);
        
        
        sprite.setScale({
            (float)(mainWindow.getSize().x) / texture.getSize().x,
            (float)(mainWindow.getSize().y) / texture.getSize().y
        });
        
        
        sprite.setPosition({
            ((mainWindow.getSize().x - texture.getSize().x) / 2.0f), 
            ((mainWindow.getSize().y - texture.getSize().y) / 2.0f)
        });

        
        
        mainWindow.draw(sprite);
        
        // Swap the back buffer.
        mainWindow.display();
    }

    std::cout << "Done" << "\n\n";
}
