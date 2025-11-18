#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Menu.h"
#include "SFMLGameAnimated.h"

enum class GameState { MENU, PLAYING, WIN, GAMEOVER, EXIT };

class GameManager {
private:
    sf::RenderWindow window;
    Menu menu;
    SFMLGameAnimated game;
    GameState state;
    sf::Music bgMusic;

public:
    GameManager();
    void run();
    void showMessage(const std::string& msg, bool& backToMenu, bool& restartGame);
};
