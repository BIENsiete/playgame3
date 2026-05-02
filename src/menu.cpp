#include <string.h>
#include <stdio.h>

#include "menu.h"
#include "renderer.h"
#include "input.h"

Menu::Menu(Game **games, int gameCount)
    : games(games)
    , gameCount(gameCount)
    , selection(0)
    , startGame(false)
    , input(nullptr)
    , timer(0.0f)
{
}

Menu::~Menu() {
}

void Menu::init(Renderer *renderer, Input *input) {
    this->input = input;
    startGame = false;
    selection = 0;
    timer = 0.0f;
}

void Menu::update(float dt) {
    timer += dt;

    if (input->justPressed(BTN_DOWN)) {
        selection = (selection + 1) % gameCount;
        timer = 0;
    }
    if (input->justPressed(BTN_UP)) {
        selection = (selection - 1 + gameCount) % gameCount;
        timer = 0;
    }

    if (input->justPressed(BTN_CROSS) || input->justPressed(BTN_START)) {
        startGame = true;
    }
}

void Menu::render(Renderer *renderer) {
    float w = renderer->getWidth();
    float h = renderer->getHeight();
    float cy = h * 0.5f - (gameCount * 30.0f) * 0.5f;

    renderer->setColor(0x00AAFF);
    renderer->drawStringCentered(w * 0.5f, 60, "PLAYGAME3");
    renderer->setColor(0x666666);
    renderer->drawStringCentered(w * 0.5f, 75, "5 Vector Games Collection");
    renderer->drawLine(w * 0.3f, 90, w * 0.7f, 90);

    for (int i = 0; i < gameCount; i++) {
        float y = cy + i * 30.0f;
        if (i == selection) {
            float pulse = 0.7f + 0.3f * sinf(timer * 4.0f);
            renderer->setColor(1.0f * pulse, 1.0f * pulse, 0.2f * pulse, 1.0f);
            renderer->drawRect(w * 0.3f - 10, y - 10, 60, 22, true);
            renderer->setColor(0x000000);
            renderer->drawStringCentered(w * 0.3f + 20, y, games[i]->getName());
            renderer->setColor(1.0f, 1.0f, 0.2f, 1.0f);
            renderer->drawStringCentered(w * 0.3f + 20, y, ">");
            renderer->drawStringCentered(w * 0.7f + 20, y, "<");
        } else {
            renderer->setColor(0xCCCCCC);
            renderer->drawStringCentered(w * 0.5f, y, games[i]->getName());
        }
    }

    renderer->setColor(0x888888);
    renderer->drawStringCentered(w * 0.5f, h - 50, "D-Pad: Navigate  |  X: Select");

    char buf[32];
    sprintf(buf, "v1.0 - PSL1GHT");
    renderer->setColor(0x444444);
    renderer->drawStringCentered(w * 0.5f, h - 20, buf);
}

Game* Menu::getSelectedGame() {
    return games[selection];
}

bool Menu::shouldStartGame() const {
    return startGame;
}

void Menu::reset() {
    startGame = false;
    timer = 0.0f;
}
