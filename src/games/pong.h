#ifndef PONG_H
#define PONG_H

#include "game.h"

class Pong : public Game {
public:
    Pong();
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

    float paddle1Y, paddle2Y;
    float paddleSpeed;
    float paddleH;

    float ballX, ballY;
    float ballVX, ballVY;
    float ballSpeed;
    float ballR;

    int score1, score2;
    int winScore;

    float stateTimer;
    enum State { SERVING, PLAYING, SCORED };
    State state;
    int serveDir;
};

#endif
