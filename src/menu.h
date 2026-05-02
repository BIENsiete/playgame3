#ifndef MENU_H
#define MENU_H

#include "games/game.h"

class Renderer;
class Input;

class Menu {
public:
    Menu(Game **games, int gameCount);
    ~Menu();

    void init(Renderer *renderer, Input *input);
    void update(float dt);
    void render(Renderer *renderer);
    Game* getSelectedGame();
    bool shouldStartGame() const;
    void reset();

private:
    Game **games;
    int gameCount;
    int selection;
    bool startGame;
    Input *input;
    float timer;
};

#endif
