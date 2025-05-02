
#include <SFML/Graphics.hpp>
#include <iostream>

sf::Texture loadTextureFromSTB(const std::string& filename) {

    // Create SFML image from raw pixel data
    sf::Image image("example.png");

    // Load texture from SFML image
    sf::Texture texture;
    texture.loadFromImage(image);

    return texture;
}


int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "STB + SFML");

    sf::Texture texture = loadTextureFromSTB("my_image.png");
    if (texture.getSize().x == 0) {
        std::cerr << "Failed to load texture\n";
        return 1;
    }

    sf::Sprite sprite(texture);

    while (window.isOpen()) {
        while (std::optional event = window.pollEvent())
            if (event->is<sf::Event::Closed>())
                window.close();

        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}
