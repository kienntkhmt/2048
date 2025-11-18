#include "GameManager.h"
#include <iostream>
#include <algorithm>

using namespace std;

GameManager::GameManager()
    : window(sf::VideoMode(600, 700), "2048 Animated"),
      menu(&window),
      game(&window),
      state(GameState::MENU)
{
    if (!bgMusic.openFromFile("ChillLofi.ogg"))
        cerr << "Cannot load background music\n";
    else {
        bgMusic.setLoop(true);
        bgMusic.setVolume(70.f);
        bgMusic.play();
    }
}

// ---------------------------------------------------
// Hiển thị thông báo (Win / Game Over)
// ---------------------------------------------------
void GameManager::showMessage(const string &msg, bool &backToMenu, bool &restartGame) {
    backToMenu = false;
    restartGame = false;

    // --- Tải ảnh thông báo ---
    sf::Texture msgTexture;
    if (msg.find("Win") != string::npos)
        msgTexture.loadFromFile("win.png");
    else
        msgTexture.loadFromFile("gameover.png");

    sf::Sprite msgSprite;
    msgSprite.setTexture(msgTexture);

    // --- Scale để ảnh phủ kín toàn cửa sổ ---
    sf::Vector2u texSize = msgTexture.getSize();
    sf::Vector2u winSize = window.getSize();

    float scaleX = static_cast<float>(winSize.x) / texSize.x;
    float scaleY = static_cast<float>(winSize.y) / texSize.y;

    msgSprite.setScale(scaleX, scaleY);
    msgSprite.setPosition(0.f, 0.f);

    // --- Âm thanh ---
    sf::SoundBuffer soundBuffer;
    sf::Sound sound;
    if (msg.find("Win") != string::npos)
        soundBuffer.loadFromFile("win_sound.wav");
    else
        soundBuffer.loadFromFile("gameover.wav");

    sound.setBuffer(soundBuffer);
    sound.play();

    // --- Hiệu ứng fade-in ---
    sf::Clock clock;
    const float duration = 1.5f; // thời gian mờ dần (giây)

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();

            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::R) {
                    restartGame = true;
                    return;
                }
                if (e.key.code == sf::Keyboard::Escape) {
                    backToMenu = true;
                    return;
                }
            }
        }

        // --- Tính độ mờ (alpha) ---
        float t = clock.getElapsedTime().asSeconds();
        int alpha = std::min(255, static_cast<int>(255 * (t / duration)));
        msgSprite.setColor(sf::Color(255, 255, 255, alpha));

        // --- Vẽ ---
        window.clear();
        window.draw(msgSprite);
        window.display();
    }
}

// ---------------------------------------------------
// Vòng lặp chính của GameManager
// ---------------------------------------------------
void GameManager::run() {
    while (window.isOpen()) {
        if (state == GameState::MENU) {
            int choice = menu.run();
            if (choice == 0) {
                game.resetBoard();
                state = GameState::PLAYING;
            } else {
                state = GameState::EXIT;
                window.close();
            }
        }

        else if (state == GameState::PLAYING) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    return;
                } else if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::R)
                        game.resetBoard();
                    else if (event.key.code == sf::Keyboard::Escape)
                        state = GameState::MENU;
                    else
                        game.handleInput(event.key.code);
                }
            }

            game.updateAnimation();
            game.draw();

            if (game.isWin()) {
                game.updateHighScore();
                state = GameState::WIN;
            } else if (game.isGameOver()) {
                game.updateHighScore();
                state = GameState::GAMEOVER;
            }
        }

        else if (state == GameState::WIN) {
            bool backToMenu = false, restartGame = false;
            showMessage("You Win! Press R to Restart or Esc for Menu", backToMenu, restartGame);
            if (restartGame) {
                game.resetBoard();
                state = GameState::PLAYING;
            } else if (backToMenu)
                state = GameState::MENU;
        }

        else if (state == GameState::GAMEOVER) {
            bool backToMenu = false, restartGame = false;
            showMessage("Game Over! Press R to Restart or Esc for Menu", backToMenu, restartGame);
            if (restartGame) {
                game.resetBoard();
                state = GameState::PLAYING;
            } else if (backToMenu)
                state = GameState::MENU;
        }

        else {
            state = GameState::EXIT;
        }

        sf::sleep(sf::milliseconds(16)); // Giới hạn FPS ~60
    }
}

