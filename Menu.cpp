#include "Menu.h"
#include <iostream>
using namespace std;

Menu::Menu(sf::RenderWindow* win) : window(win) {
    if (!font.loadFromFile("arial.ttf"))
        throw runtime_error("Cannot load font\n");
    if (!bgTexture.loadFromFile("unnamed1.png")) {
        cerr << "Cannot load menu background\n";
    } else {
        bgSprite.setTexture(bgTexture);
        sf::Vector2u texSize = bgTexture.getSize();
        bgSprite.setScale(600.f / texSize.x, 700.f / texSize.y);
    }
}

int Menu::run() {
    while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window->close();
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W)
                    selected = (selected + 1) % 2;
                else if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S)
                    selected = (selected + 1) % 2;
                else if (event.key.code == sf::Keyboard::Enter)
                    return selected;
            }
        }

        window->clear();
        window->draw(bgSprite);

        sf::Text title("2048 Game", font, 48);
        title.setPosition(150, 80);
        title.setFillColor(sf::Color::White);
        title.setStyle(sf::Text::Bold);
        window->draw(title);

        sf::Text playText("Play", font, 36);
        sf::Text exitText("Exit", font, 36);
        playText.setPosition(250, 250);
        exitText.setPosition(250, 320);

        if (selected == 0) {
            playText.setFillColor(sf::Color::Yellow);
            exitText.setFillColor(sf::Color(200, 200, 200));
        } else {
            playText.setFillColor(sf::Color(200, 200, 200));
            exitText.setFillColor(sf::Color::Yellow);
        }

        window->draw(playText);
        window->draw(exitText);
        window->display();
    }
    return 1;
}

