#ifndef BREAKOUT_H
#define BREAKOUT_H

#include "game.h"

#define BRICK_COLS 14
#define BRICK_ROWS 7
#define BRICK_TOTAL 98

struct Brick {
    bool active;
    int hits;
    int maxHits;
};

class Breakout : public Game {
public:
    Breakout();
    const char* getName() override;
    void init(Renderer *renderer, Input *input) override;
    void update(float dt) override;
    void render(Renderer *renderer) override;
    bool isDone() override;
    void cleanup() override;

private:
    void resetBall();
    void resetGame();

    Input *input;
    bool done;

    float paddleX, paddleY;
    float paddleW, paddleH;

    float ballX, ballY;
    float ballVX, ballVY;
    float ballSpeed;
    float ballR;

    Brick bricks[BRICK_COLS][BRICK_ROWS];
    float brickW, brickH;
    float brickOffsetX, brickOffsetY;

    int score;
    int lives;
    bool gameOver;
    bool levelComplete;

    enum State { SERVING, PLAYING, LOST_BALL, GAME_OVER, LEVEL_COMPLETE };
    State state;
    float stateTimer;
};

#endif
