// src/main.cpp
// clang++ --target=x86_64-w64-mingw32 -o main .\src\main.cpp -Ivendor\include -Lvendor\lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -DSFML_STATIC -lwinmm -lopengl32 -lgdi32 -lfreetype -std=c++23 --static -pthread


#include <iostream>
#include <SFML/Graphics.hpp>


int main() {

    // File name with extnsion
    int width, height, channels;
    const char* imageFile = "example.png";

    sf::RenderWindow mainWindow(sf::VideoMode({ 800, 600 }), "F1-Player");
    mainWindow.setFramerateLimit(60);

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
        
        mainWindow.clear(sf::Color::Black);
        mainWindow.display();

    }

    std::cout << "Done" << "\n\n";
}
