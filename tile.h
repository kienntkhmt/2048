#pragma once
#include <SFML/Graphics.hpp>

struct Tile {
    int value = 0;
    sf::Vector2f curPos;
    sf::Vector2f destPos;
    bool merged = false;
    float scale = 1.f;
    bool merging = false;
    sf::Clock mergeClock;
};
