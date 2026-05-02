#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "snake.h"
#include "../renderer.h"
#include "../input.h"

Snake::Snake()
    : input(nullptr)
    , done(false)
    , length(3)
    , dirX(1), dirY(0)
    , nextDirX(1), nextDirY(0)
    , foodX(0), foodY(0)
    , score(0)
    , moveTimer(0)
    , moveInterval(0.15f)
    , gameOver(false)
    , offsetX(0), offsetY(0)
{
    memset(body, 0, sizeof(body));
}

const char* Snake::getName() { return "SNAKE"; }

void Snake::init(Renderer *renderer, Input *input) {
    this->input = input;
    done = false;
    gameOver = false;
    score = 0;
    moveInterval = 0.15f;
    moveTimer = 0;

    float w = renderer->getWidth();
    float h = renderer->getHeight();
    offsetX = (int)(w - GRID_W * CELL_SIZE) / 2;
    offsetY = (int)(h - GRID_H * CELL_SIZE) / 2;

    resetGame();
}

void Snake::resetGame() {
    length = 3;
    dirX = 1;
    dirY = 0;
    nextDirX = 1;
    nextDirY = 0;
    gameOver = false;
    score = 0;
    moveInterval = 0.15f;

    int startX = GRID_W / 2;
    int startY = GRID_H / 2;

    body[0].x = startX;
    body[0].y = startY;
    body[1].x = startX - 1;
    body[1].y = startY;
    body[2].x = startX - 2;
    body[2].y = startY;

    spawnFood();
}

void Snake::spawnFood() {
    bool valid;
    do {
        valid = true;
        foodX = rand() % GRID_W;
        foodY = rand() % GRID_H;
        for (int i = 0; i < length; i++) {
            if (body[i].x == foodX && body[i].y == foodY) {
                valid = false;
                break;
            }
        }
    } while (!valid);
}

void Snake::update(float dt) {
    if (done) return;

    if (gameOver) {
        if (input->justPressed(BTN_CROSS)) {
            resetGame();
        }
        if (input->justPressed(BTN_CIRCLE)) {
            done = true;
        }
        return;
    }

    if (input->justPressed(BTN_UP) && dirY != 1) { nextDirX = 0; nextDirY = -1; }
    if (input->justPressed(BTN_DOWN) && dirY != -1) { nextDirX = 0; nextDirY = 1; }
    if (input->justPressed(BTN_LEFT) && dirX != 1) { nextDirX = -1; nextDirY = 0; }
    if (input->justPressed(BTN_RIGHT) && dirX != -1) { nextDirX = 1; nextDirY = 0; }

    moveTimer += dt;
    if (moveTimer >= moveInterval) {
        moveTimer = 0;

        dirX = nextDirX;
        dirY = nextDirY;

        int newX = body[0].x + dirX;
        int newY = body[0].y + dirY;

        if (newX < 0 || newX >= GRID_W || newY < 0 || newY >= GRID_H) {
            gameOver = true;
            return;
        }

        for (int i = 0; i < length; i++) {
            if (body[i].x == newX && body[i].y == newY) {
                gameOver = true;
                return;
            }
        }

        for (int i = length; i > 0; i--) {
            body[i] = body[i - 1];
        }
        body[0].x = newX;
        body[0].y = newY;

        if (newX == foodX && newY == foodY) {
            length++;
            if (length > SNAKE_MAX_LENGTH) length = SNAKE_MAX_LENGTH;
            body[length - 1] = body[length - 2];
            score += 10;
            if (moveInterval > 0.05f) moveInterval -= 0.003f;
            spawnFood();
        }
    }
}

void Snake::render(Renderer *renderer) {
    for (int i = 0; i < length; i++) {
        float x = offsetX + body[i].x * CELL_SIZE;
        float y = offsetY + body[i].y * CELL_SIZE;
        if (i == 0) {
            renderer->setColor(0x00FF44);
        } else {
            float t = 1.0f - (float)i / length;
            renderer->setColor(0.0f, 0.6f + 0.4f * t, 0.2f, 1.0f);
        }
        renderer->drawRect(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, true);
    }

    renderer->setColor(0xFF4444);
    renderer->drawCircle(offsetX + foodX * CELL_SIZE + CELL_SIZE / 2,
                        offsetY + foodY * CELL_SIZE + CELL_SIZE / 2,
                        CELL_SIZE / 2 - 1);

    renderer->setColor(0x333333);
    renderer->drawRect(offsetX - 2, offsetY - 2,
                      GRID_W * CELL_SIZE + 4, GRID_H * CELL_SIZE + 4);

    char buf[32];
    sprintf(buf, "SCORE: %d", score);
    renderer->setColor(0xFFFFFF);
    renderer->drawStringCentered(640, 20, buf);

    renderer->setColor(0x444444);
    renderer->drawStringCentered(640, 700, "CIRCLE: Exit");

    if (gameOver) {
        renderer->setColor(0xFF0000);
        renderer->drawStringCentered(640, 360, "GAME OVER");
        renderer->setColor(0xFFFFFF);
        renderer->drawStringCentered(640, 380, "Press X to restart");
    }
}

bool Snake::isDone() { return done; }

void Snake::cleanup() {}
