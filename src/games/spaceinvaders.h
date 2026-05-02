#ifndef SPACEINVADERS_H
#define SPACEINVADERS_H

#include "game.h"

#define INVADER_COLS 11
#define INVADER_ROWS 5
#define ALIEN_BULLETS 10
#define PLAYER_BULLETS 5

struct Invader {
    float x, y;
    bool active;
    int type;
    float animTimer;
    int frame;
};

struct Bullet2 {
    float x, y;
    float vy;
    bool active;
};

struct Barrier {
    float x, y;
    int hp;
    bool active;
};

class SpaceInvaders : public Game {
public:
    SpaceInvaders();
    const char* getName() override;
    void init(Renderer *renderer, Input *input) override;
    void update(float dt) override;
    void render(Renderer *renderer) override;
    bool isDone() override;
    void cleanup() override;

private:
    void resetGame();
    void spawnInvaders();
    void drawInvader(Renderer *renderer, int type, float x, float y, int frame);

    Input *input;
    bool done;

    float playerX, playerY;
    float playerW;
    bool playerActive;

    Invader invaders[INVADER_COLS][INVADER_ROWS];
    int invaderDir;
    float invaderSpeed;
    float invaderDrop;
    float moveTimer;
    float moveInterval;
    float baseInterval;

    Bullet2 pBullets[PLAYER_BULLETS];
    Bullet2 aBullets[ALIEN_BULLETS];

    Barrier barriers[4];
    static const int MAX_BARRIER_HP = 4;

    int score;
    int lives;
    bool gameOver;
    float gameTimer;
};

#endif
