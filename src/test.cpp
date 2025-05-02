// library 
#include <SFML/Graphics.hpp>
#include <iostream>

using std::cout;

int main() {
    sf::RenderWindow window(sf::VideoMode({ 300, 300 }), "games");

    window.setKeyRepeatEnabled(false);

    // App loop.
    while (window.isOpen()) {

        // handel events 
        while (std::optional event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (auto* key = event->getIf<sf::Event::KeyPressed>()) {
                
                if (key->scancode >= sf::Keyboard::Scancode::A and
                    key->scancode <= sf::Keyboard::Scancode::Z)
                {
                    if (key->control)
                    cout << "\"";
                    cout << sf::Keyboard::getDescription(key->scancode).toAnsiString();
                    cout << "\" - Pressed\n";
                }

            }
        }

        
    // Draw fill color.
    window.clear(sf::Color(232, 5, 142, 255));
    window.display();
    
    }

    return 0;
}