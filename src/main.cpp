#include <iostream> 

#include <SFML/Graphics.hpp>
#include <vlc/vlc.h>

unsigned width = 640;
unsigned height = 360;


sf::Texture texture({width, height});
sf::Sprite sprite(texture);
uint8_t* pixelBuffer;


void* lock(void* opaque, void** planes) {
    *planes = pixelBuffer;
    return nullptr;
}

void unlock(void* opaque, void* picture, void* const* planes) {
    // nothing to do here
}

void display(void* opaque, void* picture) {
    texture.update(pixelBuffer); // upload new frame to texture
}

int main(int argc, char** argv) {
    sf::RenderWindow window(sf::VideoMode({width, height}), "LibVLC + SFML");

    if (argc < 2) {
        std::cout << "No enough arguments" << std::endl;
    }

    // Allocate buffer: RGBA = 4 bytes per pixel
    pixelBuffer = new uint8_t[width * height * 4];
    sprite.setTexture(texture);

    // VLC setup
    libvlc_instance_t* inst = libvlc_new(0, nullptr);
    libvlc_media_t* media = libvlc_media_new_path(inst, argv[1]);
    libvlc_media_player_t* player = libvlc_media_player_new_from_media(media);

    libvlc_video_set_callbacks(player, lock, unlock, display, nullptr);
    libvlc_video_set_format(player, "RGBA", width, height, width * 4);

    libvlc_media_player_play(player);

    // Main loop
    while (window.isOpen()) {
        while (std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>() || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Escape))
                window.close();
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }

    // Cleanup
    libvlc_media_player_stop(player);
    libvlc_media_player_release(player);
    libvlc_release(inst);
    delete[] pixelBuffer;

    return 0;
}
