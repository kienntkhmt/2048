#include "SFMLGameAnimated.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

// ---------------------------------------------------
// CONSTRUCTOR
// ---------------------------------------------------
SFMLGameAnimated::SFMLGameAnimated(sf::RenderWindow* win) : window(win) {
    srand(static_cast<unsigned>(time(0)));

    if (!font.loadFromFile("arial.ttf"))
        cerr << "Cannot load arial.ttf\n";

    if (!backgroundTexture.loadFromFile("background.png"))
        cerr << "Cannot load background.png\n";
    else {
        float scaleX = static_cast<float>(window->getSize().x) / backgroundTexture.getSize().x;
        float scaleY = static_cast<float>(window->getSize().y) / backgroundTexture.getSize().y;
        backgroundSprite.setScale(scaleX, scaleY);
        backgroundSprite.setTexture(backgroundTexture);
    }

    if (!mergeBuffer.loadFromFile("merge.wav"))
        cerr << "Cannot load merge.wav\n";
    else
        mergeSound.setBuffer(mergeBuffer);

    if (!moveBuffer.loadFromFile("move.wav"))
        cerr << "Cannot load move.wav\n";
    else {
        moveSound.setBuffer(moveBuffer);
        moveSound.setVolume(50.f);
    }

    loadTextures();
    highScore = loadHighScore();
    resetBoard();
}

// ---------------------------------------------------
// FONT ACCESS
// ---------------------------------------------------
sf::Font& SFMLGameAnimated::getFont() {
    return font;
}

// ---------------------------------------------------
// LOAD TILE TEXTURES
// ---------------------------------------------------
void SFMLGameAnimated::loadTextures() {
    vector<int> values = {2,4,8,16,32,64,128,256,512,1024,2048};
    for (int v : values) {
        sf::Texture tex;
        if (!tex.loadFromFile("tile" + to_string(v) + ".png"))
            cerr << "Cannot load tile" << v << ".png\n";
        tileTextures[v] = tex;
    }
}

// ---------------------------------------------------
// POSITION CALCULATOR
// ---------------------------------------------------
sf::Vector2f SFMLGameAnimated::getPixelPos(int row, int col) {
    return sf::Vector2f(col * (tileSize + gap) + 30, row * (tileSize + gap) + 110);
}

// ---------------------------------------------------
// RESET BOARD
// ---------------------------------------------------
void SFMLGameAnimated::resetBoard() {
    board = vector<vector<Tile>>(4, vector<Tile>(4));
    score = 0;
    addRandomTile();
    addRandomTile();
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            board[i][j].curPos = getPixelPos(i, j);
            board[i][j].destPos = getPixelPos(i, j);
            board[i][j].scale = 1.f;
            board[i][j].merging = false;
        }
}

// ---------------------------------------------------
// ADD RANDOM TILE
// ---------------------------------------------------
void SFMLGameAnimated::addRandomTile() {
    vector<pair<int, int>> empty;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (board[i][j].value == 0) empty.push_back({i, j});

    if (empty.empty()) return;
    auto pos = empty[rand() % empty.size()];
    board[pos.first][pos.second].value = (rand() % 10 == 0) ? 4 : 2;
    board[pos.first][pos.second].curPos = getPixelPos(pos.first, pos.second);
    board[pos.first][pos.second].destPos = getPixelPos(pos.first, pos.second);
    board[pos.first][pos.second].scale = 0.f;
    board[pos.first][pos.second].merging = true;
    board[pos.first][pos.second].mergeClock.restart();
}

// ---------------------------------------------------
// MERGE LINE
// ---------------------------------------------------
vector<int> SFMLGameAnimated::mergeLine(vector<int> line) {
    vector<int> newLine;
    for (int x : line)
        if (x != 0) newLine.push_back(x);

    for (int i = 0; i + 1 < (int)newLine.size(); i++) {
        if (newLine[i] == newLine[i + 1]) {
            newLine[i] *= 2;
            score += newLine[i];
            newLine[i + 1] = 0;
            mergeSound.play();
            mergeSound.setVolume(60.f);
        }
    }

    vector<int> merged;
    for (int x : newLine)
        if (x != 0) merged.push_back(x);
    while (merged.size() < 4)
        merged.push_back(0);

    return merged;
}

// ---------------------------------------------------
// MOVE LOGIC
// ---------------------------------------------------
bool SFMLGameAnimated::moveLeft() {
    bool changed = false;
    auto oldBoard = board;

    for (int i = 0; i < 4; i++) {
        vector<int> line;
        for (int j = 0; j < 4; j++)
            line.push_back(board[i][j].value);

        vector<int> merged = mergeLine(line);

        for (int j = 0; j < 4; j++) {
            if (board[i][j].value != merged[j]) changed = true;
            board[i][j].value = merged[j];
            board[i][j].destPos = getPixelPos(i, j);

            if (merged[j] != 0) {
                for (int k = j; k < 4; k++) {
                    if (oldBoard[i][k].value == merged[j]) {
                        board[i][j].curPos = oldBoard[i][k].curPos;
                        oldBoard[i][k].value = -1;
                        break;
                    }
                }
            }
        }
    }

    if (changed) {
        moveSound.stop();
        moveSound.play();
        addRandomTile();
        updateHighScore();
    }
    return changed;
}

bool SFMLGameAnimated::moveRight() {
    bool changed = false;
    auto oldBoard = board;

    for (int i = 0; i < 4; i++) {
        vector<int> line;
        for (int j = 3; j >= 0; j--)
            line.push_back(board[i][j].value);

        vector<int> merged = mergeLine(line);

        for (int j = 3, k = 0; j >= 0; j--, k++) {
            if (board[i][j].value != merged[k]) changed = true;
            board[i][j].value = merged[k];
            board[i][j].destPos = getPixelPos(i, j);

            if (merged[k] != 0) {
                for (int p = j; p >= 0; p--) {
                    if (oldBoard[i][p].value == merged[k]) {
                        board[i][j].curPos = oldBoard[i][p].curPos;
                        oldBoard[i][p].value = -1;
                        break;
                    }
                }
            }
        }
    }

    if (changed) {
        moveSound.stop();
        moveSound.play();
        addRandomTile();
        updateHighScore();
    }
    return changed;
}

bool SFMLGameAnimated::moveUp() {
    bool changed = false;
    auto oldBoard = board;

    for (int j = 0; j < 4; j++) {
        vector<int> col;
        for (int i = 0; i < 4; i++)
            col.push_back(board[i][j].value);

        vector<int> merged = mergeLine(col);

        for (int i = 0; i < 4; i++) {
            if (board[i][j].value != merged[i]) changed = true;
            board[i][j].value = merged[i];
            board[i][j].destPos = getPixelPos(i, j);

            if (merged[i] != 0) {
                for (int k = i; k < 4; k++) {
                    if (oldBoard[k][j].value == merged[i]) {
                        board[i][j].curPos = oldBoard[k][j].curPos;
                        oldBoard[k][j].value = -1;
                        break;
                    }
                }
            }
        }
    }

    if (changed) {
        moveSound.stop();
        moveSound.play();
        addRandomTile();
        updateHighScore();
    }
    return changed;
}

bool SFMLGameAnimated::moveDown() {
    bool changed = false;
    auto oldBoard = board;

    for (int j = 0; j < 4; j++) {
        vector<int> col;
        for (int i = 3; i >= 0; i--)
            col.push_back(board[i][j].value);

        vector<int> merged = mergeLine(col);

        for (int i = 3, k = 0; i >= 0; i--, k++) {
            if (board[i][j].value != merged[k]) changed = true;
            board[i][j].value = merged[k];
            board[i][j].destPos = getPixelPos(i, j);

            if (merged[k] != 0) {
                for (int p = i; p >= 0; p--) {
                    if (oldBoard[p][j].value == merged[k]) {
                        board[i][j].curPos = oldBoard[p][j].curPos;
                        oldBoard[p][j].value = -1;
                        break;
                    }
                }
            }
        }
    }

    if (changed) {
        moveSound.stop();
        moveSound.play();
        addRandomTile();
        updateHighScore();
    }
    return changed;
}

// ---------------------------------------------------
// HANDLE INPUT
// ---------------------------------------------------
void SFMLGameAnimated::handleInput(int key) {
    if (key == sf::Keyboard::Up)
        moveUp();
    else if (key == sf::Keyboard::Down)
        moveDown();
    else if (key == sf::Keyboard::Left)
        moveLeft();
    else if (key == sf::Keyboard::Right)
        moveRight();
}

// ---------------------------------------------------
// UPDATE ANIMATION
// ---------------------------------------------------
void SFMLGameAnimated::updateAnimation() {
    float dt = dtClock.restart().asSeconds();
    float moveSpeed = 10.f;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            Tile &t = board[i][j];

            sf::Vector2f diff = t.destPos - t.curPos;
            float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
            if (dist > 1.f)
                t.curPos += diff * moveSpeed * dt;
            else
                t.curPos = t.destPos;

            if (t.merging) {
                float tSec = t.mergeClock.getElapsedTime().asSeconds();
                if (tSec < 0.2f)
                    t.scale = 1.f + 0.5f * (1 - pow((0.2f - tSec) / 0.2f, 2));
                else if (tSec < 0.4f)
                    t.scale = 1.f + 0.5f * pow((0.4f - tSec) / 0.2f, 2);
                else {
                    t.scale = 1.f;
                    t.merging = false;
                }
            }
        }
    }
}

// ---------------------------------------------------
// DRAW BOARD
// ---------------------------------------------------
void SFMLGameAnimated::draw() {
    window->clear();
    window->draw(backgroundSprite);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            Tile &t = board[i][j];
            sf::Sprite sprite;

            if (t.value != 0 && tileTextures.count(t.value))
                sprite.setTexture(tileTextures[t.value]);
            else
                sprite.setTexture(blankTexture);

            float baseScaleX = tileSize / sprite.getTexture()->getSize().x;
            float baseScaleY = tileSize / sprite.getTexture()->getSize().y;

            sprite.setScale(baseScaleX * t.scale, baseScaleY * t.scale);
            sprite.setOrigin(sprite.getTexture()->getSize().x / 2.f, sprite.getTexture()->getSize().y / 2.f);
            sprite.setPosition(t.curPos.x + tileSize / 2, t.curPos.y + tileSize / 2);
            window->draw(sprite);
        }
    }

    // SCORE & HIGH SCORE
    sf::Text scoreText("SCORE: " + to_string(score), font, 28);
    scoreText.setPosition(20, 30);
    scoreText.setFillColor(sf::Color::White);
    window->draw(scoreText);

    sf::Text highText("HIGH: " + to_string(highScore), font, 28);
    highText.setPosition(300, 30);
    highText.setFillColor(sf::Color::White);
    window->draw(highText);

    window->display();
}

// ---------------------------------------------------
// SCORE MANAGEMENT
// ---------------------------------------------------
int SFMLGameAnimated::loadHighScore() {
    ifstream f("score.txt");
    int hs = 0;
    if (f) f >> hs;
    return hs;
}

void SFMLGameAnimated::saveScore(int hs) {
    ofstream f("score.txt");
    if (f) f << hs;
}

void SFMLGameAnimated::updateHighScore() {
    if (score > highScore) {
        highScore = score;
        saveScore(highScore);
    }
}

// ---------------------------------------------------
// GAME STATUS
// ---------------------------------------------------
bool SFMLGameAnimated::isWin() {
    for (auto &r : board)
        for (auto &t : r)
            if (t.value == 2048) return true;
    return false;
}

bool SFMLGameAnimated::isGameOver() {
    for (auto &r : board)
        for (auto &t : r)
            if (t.value == 0) return false;

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            if (i < 3 && board[i][j].value == board[i + 1][j].value)
                return false;
            if (j < 3 && board[i][j].value == board[i][j + 1].value)
                return false;
        }

    return true;
}

