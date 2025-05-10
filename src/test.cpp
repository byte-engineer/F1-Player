// src/main.cpp
// g++ -o main .\src\main.cpp -Ivendor\include -Ivendor/imgui -Lvendor\lib -Ivendor/imgui-sfml-3 -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -DSFML_STATIC -lwinmm -lopengl32 -lgdi32 -lfreetype -std=c++23 --static -pthread -Os


#include <iostream>
#include <thread>
#include <chrono>


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
    settings.antiAliasingLevel = 8;

    sf::RenderWindow mainWindow(sf::VideoMode({1920/2, 1080/2}), "F1-Player",sf::State::Windowed ,  settings);
    mainWindow.setFramerateLimit(60);

    // Initialize ImGui-SFML
    ImGui::SFML::Init(mainWindow);

    // Load the image and create the texture
    sf::Image image;
    if (!image.loadFromFile(imageFile)) {
        std::cerr << "Error loading image file!" << std::endl;
        return -1;
    }

    sf::Texture texture;
    if (!texture.loadFromImage(image)) {
        std::cerr << "Error loading texture from image!" << std::endl;
        return -1;
    }

    sf::Clock deltaClock;

    std::this_thread::sleep_for(300ms);

    // Main loop
    while (mainWindow.isOpen()) {
        while (std::optional event = mainWindow.pollEvent()) {
            ImGui::SFML::ProcessEvent(mainWindow, event.value());

            if (event->is<sf::Event::Closed>())
                mainWindow.close();
        }

        // Update ImGui
        ImGui::SFML::Update(mainWindow, deltaClock.restart());


        // Example ImGui window
        ImGui::Begin("Controls");
        ImGui::Text("Press Esc or close the window to exit.");
        ImGui::Text("Image size: %dx%d", texture.getSize().x, texture.getSize().y);
        ImGui::End();

        // Sprite logic
        sf::Sprite sprite(texture);
        sprite.setOrigin({0, 0});
        sprite.setPosition({0, 0});
        sprite.setScale({
            static_cast<float>(mainWindow.getSize().x) / texture.getSize().x,
            static_cast<float>(mainWindow.getSize().y) / texture.getSize().y
        });

        // Render
        mainWindow.clear();
        mainWindow.draw(sprite);
        ImGui::SFML::Render(mainWindow);
        mainWindow.display();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Escape)) {
            mainWindow.close();
        }
    }

    std::cout << "Done\n";
    return 0;
}

// Optional: alignSprite could later help with sprite layout logic
sf::Sprite alignSprite(sf::Sprite& sprite, const sf::Vector2u windowSize) {
    // Placeholder for future alignment logic
    return sprite;
}
