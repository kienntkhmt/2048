#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <map>
#include <string>
#include "Tile.h"

class SFMLGameAnimated {
private:
    sf::RenderWindow* window;
    sf::Font font;
    int score = 0;
    int highScore = 0;
    std::vector<std::vector<Tile>> board;
    std::map<int, sf::Texture> tileTextures;
    sf::Texture blankTexture;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::SoundBuffer mergeBuffer;
    sf::Sound mergeSound;
    sf::SoundBuffer moveBuffer;
    sf::Sound moveSound;
    float tileSize = 125.f;
    float gap = 10.f;
    sf::Clock dtClock;

public:
    SFMLGameAnimated(sf::RenderWindow* win);
    void loadTextures();
    void resetBoard();
    void handleInput(int key);
    void updateAnimation();
    void draw();
    bool isWin();
    bool isGameOver();
    void updateHighScore();
    int loadHighScore();
    void saveScore(int hs);
    sf::Font& getFont();
private:
    sf::Vector2f getPixelPos(int row, int col);
    void addRandomTile();
    std::vector<int> mergeLine(std::vector<int> line);
    bool moveLeft();
    bool moveRight();
    bool moveUp();
    bool moveDown();
};
