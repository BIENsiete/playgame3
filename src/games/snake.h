#ifndef SNAKE_H
#define SNAKE_H

#include "game.h"

#define SNAKE_MAX_LENGTH 256

struct SnakeSegment {
    int x, y;
};

class Snake : public Game {
public:
    Snake();
    const char* getName() override;
    void init(Renderer *renderer, Input *input) override;
    void update(float dt) override;
    void render(Renderer *renderer) override;
    bool isDone() override;
    void cleanup() override;

private:
    void resetGame();
    void spawnFood();

    Input *input;
    bool done;

    static const int GRID_W = 40;
    static const int GRID_H = 30;
    static const int CELL_SIZE = 16;

    SnakeSegment body[SNAKE_MAX_LENGTH];
    int length;
    int dirX, dirY;
    int nextDirX, nextDirY;

    int foodX, foodY;

    int score;
    float moveTimer;
    float moveInterval;
    bool gameOver;

    int offsetX, offsetY;
};

#endif
