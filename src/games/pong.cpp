#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "pong.h"
#include "../renderer.h"
#include "../input.h"

Pong::Pong()
    : input(nullptr)
    , done(false)
    , paddle1Y(0), paddle2Y(0)
    , paddleSpeed(300.0f)
    , paddleH(80.0f)
    , ballX(0), ballY(0)
    , ballVX(0), ballVY(0)
    , ballSpeed(350.0f)
    , ballR(6.0f)
    , score1(0), score2(0)
    , winScore(5)
    , stateTimer(0)
    , state(SERVING)
    , serveDir(1)
{
}

const char* Pong::getName() { return "PONG"; }

void Pong::init(Renderer *renderer, Input *input) {
    this->input = input;
    done = false;
    score1 = 0;
    score2 = 0;
    paddle1Y = renderer->getHeight() * 0.5f;
    paddle2Y = renderer->getHeight() * 0.5f;
    state = SERVING;
    stateTimer = 1.0f;
    serveDir = 1;
    resetBall();
}

void Pong::resetBall() {
    float w = 1280.0f, h = 720.0f;
    ballX = w * 0.5f;
    ballY = h * 0.5f;
    ballVX = ballSpeed * serveDir;
    ballVY = (float)(rand() % 200 - 100) * 0.5f;
    serveDir = -serveDir;
}

void Pong::resetGame() {
    score1 = 0;
    score2 = 0;
    state = SERVING;
    stateTimer = 1.0f;
}

void Pong::update(float dt) {
    if (done) return;

    float w = 1280.0f, h = 720.0f;
    float paddleW = 12.0f;

    stateTimer -= dt;

    if (state == SERVING) {
        if (stateTimer <= 0) {
            state = PLAYING;
            resetBall();
        }
        return;
    }

    if (state == SCORED) {
        if (stateTimer <= 0) {
            if (score1 >= winScore || score2 >= winScore) {
                stateTimer = 3.0f;
            } else {
                state = SERVING;
                stateTimer = 1.0f;
                resetBall();
            }
        }
        return;
    }

    float deadZone = 30;
    if (input->isDown(BTN_UP)) paddle1Y -= paddleSpeed * dt;
    if (input->isDown(BTN_DOWN)) paddle1Y += paddleSpeed * dt;
    paddle1Y = fmaxf(paddleH * 0.5f, fminf(h - paddleH * 0.5f, paddle1Y));

    float aiTarget = ballY;
    float aiDiff = aiTarget - paddle2Y;
    if (fabsf(aiDiff) > 10) {
        paddle2Y += (aiDiff > 0 ? paddleSpeed : -paddleSpeed) * dt * 0.7f;
    }
    paddle2Y = fmaxf(paddleH * 0.5f, fminf(h - paddleH * 0.5f, paddle2Y));

    ballX += ballVX * dt;
    ballY += ballVY * dt;

    if (ballY - ballR < 0) { ballY = ballR; ballVY = -ballVY; }
    if (ballY + ballR > h) { ballY = h - ballR; ballVY = -ballVY; }

    if (ballX - ballR < paddleW) {
        if (ballY > paddle1Y - paddleH * 0.5f && ballY < paddle1Y + paddleH * 0.5f) {
            ballX = paddleW + ballR;
            ballVX = -ballVX;
            ballVY += (float)(rand() % 100 - 50) * 0.3f;
            if (ballVY > 300) ballVY = 300;
            if (ballVY < -300) ballVY = -300;
            ballSpeed += 10;
        }
    }

    if (ballX + ballR > w - paddleW) {
        if (ballY > paddle2Y - paddleH * 0.5f && ballY < paddle2Y + paddleH * 0.5f) {
            ballX = w - paddleW - ballR;
            ballVX = -ballVX;
            ballVY += (float)(rand() % 100 - 50) * 0.3f;
            ballSpeed += 10;
        }
    }

    if (ballX < -50) {
        score2++;
        state = SCORED;
        stateTimer = 2.0f;
        ballSpeed = 350.0f;
    }
    if (ballX > w + 50) {
        score1++;
        state = SCORED;
        stateTimer = 2.0f;
        ballSpeed = 350.0f;
    }

    float len = sqrtf(ballVX * ballVX + ballVY * ballVY);
    ballVX = (ballVX / len) * ballSpeed;
    ballVY = (ballVY / len) * ballSpeed;

    if (input->justPressed(BTN_CIRCLE)) {
        done = true;
    }
}

void Pong::render(Renderer *renderer) {
    float w = renderer->getWidth();
    float h = renderer->getHeight();
    float paddleW = 12.0f;

    renderer->setColor(0xFFFFFF);

    renderer->drawRect(0, paddle1Y - paddleH * 0.5f, paddleW, paddleH, true);
    renderer->drawRect(w - paddleW, paddle2Y - paddleH * 0.5f, paddleW, paddleH, true);

    renderer->drawCircle(ballX, ballY, ballR);

    for (int i = 0; i < 16; i++) {
        float y = i * (h / 16.0f);
        renderer->drawLine(w * 0.5f, y, w * 0.5f, y + h * 0.03125f);
    }

    char buf[16];
    sprintf(buf, "%d", score1);
    renderer->setColor(0x888888);
    renderer->drawStringCentered(w * 0.25f, 30, buf);
    sprintf(buf, "%d", score2);
    renderer->drawStringCentered(w * 0.75f, 30, buf);

    renderer->setColor(0x444444);
    renderer->drawStringCentered(w * 0.5f, h - 20, "CIRCLE: Exit");

    if (state == SERVING) {
        renderer->setColor(0xFFAA00);
        renderer->drawStringCentered(w * 0.5f, h * 0.5f - 40, "PRESS X TO SERVE");
    }
    if (state == SCORED) {
        renderer->setColor(0xFFFF00);
        if (score1 >= winScore || score2 >= winScore) {
            const char *winner = (score1 >= winScore) ? "PLAYER 1 WINS!" : "PLAYER 2 WINS!";
            renderer->drawStringCentered(w * 0.5f, h * 0.5f - 40, winner);
        }
    }

    if (state == SERVING || state == SCORED) {
        if (input && input->justPressed(BTN_CROSS)) {
            if (state == SERVING) state = PLAYING;
            resetBall();
        }
    }
}

bool Pong::isDone() { return done; }

void Pong::cleanup() {}
