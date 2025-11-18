#pragma once
#include <SFML/Graphics.hpp>

class Menu {
private:
    sf::Font font;
    sf::RenderWindow* window;
    int selected = 0;
    sf::Texture bgTexture;
    sf::Sprite bgSprite;

public:
    Menu(sf::RenderWindow* win);
    int run();
};
