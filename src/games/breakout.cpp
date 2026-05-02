#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "breakout.h"
#include "../renderer.h"
#include "../input.h"

Breakout::Breakout()
    : input(nullptr)
    , done(false)
    , paddleX(0), paddleY(690)
    , paddleW(100), paddleH(12)
    , ballX(0), ballY(0)
    , ballVX(0), ballVY(0)
    , ballSpeed(350.0f)
    , ballR(5.0f)
    , brickW(80), brickH(20)
    , brickOffsetX(0), brickOffsetY(50)
    , score(0)
    , lives(3)
    , gameOver(false)
    , levelComplete(false)
    , state(SERVING)
    , stateTimer(0)
{
    memset(bricks, 0, sizeof(bricks));
}

const char* Breakout::getName() { return "BREAKOUT"; }

void Breakout::init(Renderer *renderer, Input *input) {
    this->input = input;
    done = false;
    score = 0;
    lives = 3;
    gameOver = false;
    levelComplete = false;
    state = SERVING;
    stateTimer = 1.0f;

    float w = renderer->getWidth();
    paddleX = w * 0.5f;
    paddleY = renderer->getHeight() - 30;

    brickW = (w - 40) / BRICK_COLS;
    brickH = 20;
    brickOffsetX = 20;
    brickOffsetY = 50;

    for (int col = 0; col < BRICK_COLS; col++) {
        for (int row = 0; row < BRICK_ROWS; row++) {
            bricks[col][row].active = true;
            bricks[col][row].hits = 0;
            bricks[col][row].maxHits = (row < 2) ? 3 : (row < 4) ? 2 : 1;
        }
    }

    resetBall();
}

void Breakout::resetBall() {
    float w = 1280.0f;
    ballX = paddleX;
    ballY = paddleY - paddleH * 0.5f - ballR;
    ballVX = 0;
    ballVY = 0;
}

void Breakout::resetGame() {
    lives = 3;
    score = 0;
    state = SERVING;
    stateTimer = 1.0f;

    for (int col = 0; col < BRICK_COLS; col++) {
        for (int row = 0; row < BRICK_ROWS; row++) {
            bricks[col][row].active = true;
            bricks[col][row].hits = 0;
        }
    }
}

void Breakout::update(float dt) {
    if (done) return;

    float w = 1280.0f, h = 720.0f;

    if (gameOver) {
        if (input->justPressed(BTN_CROSS)) {
            resetGame();
        }
        if (input->justPressed(BTN_CIRCLE)) {
            done = true;
        }
        return;
    }

    if (input->isDown(BTN_LEFT)) paddleX -= 400.0f * dt;
    if (input->isDown(BTN_RIGHT)) paddleX += 400.0f * dt;
    paddleX = fmaxf(paddleW * 0.5f, fminf(w - paddleW * 0.5f, paddleX));

    stateTimer -= dt;

    if (state == SERVING) {
        ballX = paddleX;
        ballY = paddleY - paddleH * 0.5f - ballR;
        if (input->justPressed(BTN_CROSS) || stateTimer <= 0) {
            state = PLAYING;
            ballVX = (float)(rand() % 200 - 100) * 0.5f;
            ballVY = -ballSpeed;
            float len = sqrtf(ballVX * ballVX + ballVY * ballVY);
            ballVX = (ballVX / len) * ballSpeed;
            ballVY = (ballVY / len) * ballSpeed;
        }
        return;
    }

    if (state == LOST_BALL) {
        if (stateTimer <= 0) {
            if (lives <= 0) {
                gameOver = true;
            } else {
                state = SERVING;
                stateTimer = 1.0f;
                resetBall();
            }
        }
        return;
    }

    ballX += ballVX * dt;
    ballY += ballVY * dt;

    if (ballX - ballR < 0) { ballX = ballR; ballVX = -ballVX; }
    if (ballX + ballR > w) { ballX = w - ballR; ballVX = -ballVX; }
    if (ballY - ballR < 0) { ballY = ballR; ballVY = -ballVY; }

    if (ballY + ballR > paddleY - paddleH * 0.5f &&
        ballY + ballR < paddleY + paddleH * 0.5f &&
        ballX > paddleX - paddleW * 0.5f &&
        ballX < paddleX + paddleW * 0.5f &&
        ballVY > 0) {
        ballY = paddleY - paddleH * 0.5f - ballR;
        float offset = (ballX - paddleX) / (paddleW * 0.5f);
        ballVX = offset * ballSpeed;
        ballVY = -ballSpeed;
        float len = sqrtf(ballVX * ballVX + ballVY * ballVY);
        ballVX = (ballVX / len) * ballSpeed;
        ballVY = (ballVY / len) * ballSpeed;
    }

    if (ballY + ballR > paddleY + paddleH * 0.5f + 50) {
        lives--;
        state = LOST_BALL;
        stateTimer = 1.0f;
    }

    for (int col = 0; col < BRICK_COLS; col++) {
        for (int row = 0; row < BRICK_ROWS; row++) {
            if (!bricks[col][row].active) continue;

            float bx = brickOffsetX + col * brickW + brickW * 0.5f;
            float by = brickOffsetY + row * brickH + brickH * 0.5f;

            float closestX = fmaxf(bx - brickW * 0.5f, fminf(ballX, bx + brickW * 0.5f));
            float closestY = fmaxf(by - brickH * 0.5f, fminf(ballY, by + brickH * 0.5f));

            float dx = ballX - closestX;
            float dy = ballY - closestY;

            if (dx * dx + dy * dy < ballR * ballR) {
                bricks[col][row].hits++;
                if (bricks[col][row].hits >= bricks[col][row].maxHits) {
                    bricks[col][row].active = false;
                }

                score += 10 * bricks[col][row].maxHits;

                if (closestX == bx - brickW * 0.5f || closestX == bx + brickW * 0.5f) {
                    ballVX = -ballVX;
                }
                if (closestY == by - brickH * 0.5f || closestY == by + brickH * 0.5f) {
                    ballVY = -ballVY;
                }

                goto check_level;
            }
        }
    }

check_level:
    levelComplete = true;
    for (int col = 0; col < BRICK_COLS; col++) {
        for (int row = 0; row < BRICK_ROWS; row++) {
            if (bricks[col][row].active) {
                levelComplete = false;
                break;
            }
        }
        if (!levelComplete) break;
    }

    if (levelComplete) {
        state = LEVEL_COMPLETE;
        stateTimer = 3.0f;
    }
}

void Breakout::render(Renderer *renderer) {
    float w = renderer->getWidth();
    float h = renderer->getHeight();

    for (int col = 0; col < BRICK_COLS; col++) {
        for (int row = 0; row < BRICK_ROWS; row++) {
            if (!bricks[col][row].active) continue;

            float x = brickOffsetX + col * brickW;
            float y = brickOffsetY + row * brickH;

            u32 colors[7] = {
                0xFF4444, 0xFF6644, 0xFFAA44,
                0xFFFF44, 0x44FF44, 0x44AAFF, 0xAA44FF
            };
            renderer->setColor(colors[row]);

            int remaining = bricks[col][row].maxHits - bricks[col][row].hits;
            float inset = (remaining < bricks[col][row].maxHits) ? 2.0f : 0;
            renderer->drawRect(x + inset, y + inset, brickW - inset * 2, brickH - inset * 2, true);
            renderer->setColor(0x222222);
            renderer->drawRect(x, y, brickW, brickH);
        }
    }

    renderer->setColor(0xFFFFFF);
    renderer->drawRect(paddleX - paddleW * 0.5f, paddleY - paddleH * 0.5f, paddleW, paddleH, true);

    renderer->drawCircle(ballX, ballY, ballR);

    char buf[32];
    sprintf(buf, "SCORE: %d", score);
    renderer->setColor(0xFFFFFF);
    renderer->drawString(20, 20, buf);

    sprintf(buf, "LIVES: %d", lives);
    renderer->drawString(20, 35, buf);

    renderer->setColor(0x444444);
    renderer->drawStringCentered(640, 700, "CIRCLE: Exit");

    if (state == SERVING) {
        renderer->setColor(0xFFAA00);
        renderer->drawStringCentered(640, 400, "Press X to serve");
    }

    if (gameOver) {
        renderer->setColor(0xFF0000);
        renderer->drawStringCentered(640, 360, "GAME OVER");
        renderer->setColor(0xFFFFFF);
        renderer->drawStringCentered(640, 380, "Press X to restart");
    }

    if (state == LEVEL_COMPLETE) {
        renderer->setColor(0x00FF00);
        renderer->drawStringCentered(640, 360, "LEVEL COMPLETE!");
    }
}

bool Breakout::isDone() { return done; }

void Breakout::cleanup() {}
