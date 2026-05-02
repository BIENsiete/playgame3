#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "spaceinvaders.h"
#include "../renderer.h"
#include "../input.h"

SpaceInvaders::SpaceInvaders()
    : input(nullptr)
    , done(false)
    , playerX(640), playerY(680)
    , playerW(40)
    , playerActive(true)
    , invaderDir(1)
    , invaderSpeed(60.0f)
    , invaderDrop(10.0f)
    , moveTimer(0)
    , moveInterval(0.5f)
    , baseInterval(0.5f)
    , score(0)
    , lives(3)
    , gameOver(false)
    , gameTimer(0)
{
    memset(invaders, 0, sizeof(invaders));
    memset(pBullets, 0, sizeof(pBullets));
    memset(aBullets, 0, sizeof(aBullets));
    memset(barriers, 0, sizeof(barriers));
}

const char* SpaceInvaders::getName() { return "SPACE INVADERS"; }

void SpaceInvaders::init(Renderer *renderer, Input *input) {
    this->input = input;
    done = false;
    score = 0;
    lives = 3;
    gameOver = false;
    gameTimer = 0;
    playerActive = true;
    invaderDir = 1;
    baseInterval = 0.5f;
    moveInterval = baseInterval;
    moveTimer = 0;

    playerX = renderer->getWidth() * 0.5f;
    playerY = renderer->getHeight() - 40;

    for (int i = 0; i < PLAYER_BULLETS; i++) pBullets[i].active = false;
    for (int i = 0; i < ALIEN_BULLETS; i++) aBullets[i].active = false;

    for (int i = 0; i < 4; i++) {
        float w = renderer->getWidth();
        barriers[i].x = (i + 1) * w / 5.0f;
        barriers[i].y = renderer->getHeight() - 120;
        barriers[i].hp = MAX_BARRIER_HP;
        barriers[i].active = true;
    }

    spawnInvaders();
}

void SpaceInvaders::spawnInvaders() {
    for (int col = 0; col < INVADER_COLS; col++) {
        for (int row = 0; row < INVADER_ROWS; row++) {
            invaders[col][row].x = 60 + col * 80;
            invaders[col][row].y = 60 + row * 50;
            invaders[col][row].active = true;
            invaders[col][row].type = (row < 1) ? 2 : (row < 3) ? 1 : 0;
            invaders[col][row].animTimer = 0;
            invaders[col][row].frame = 0;
        }
    }
}

void SpaceInvaders::resetGame() {
    lives = 3;
    score = 0;
    baseInterval = 0.5f;
    moveInterval = baseInterval;
    gameOver = false;
    playerActive = true;
    invaderDir = 1;

    for (int i = 0; i < PLAYER_BULLETS; i++) pBullets[i].active = false;
    for (int i = 0; i < ALIEN_BULLETS; i++) aBullets[i].active = false;

    for (int i = 0; i < 4; i++) {
        barriers[i].hp = MAX_BARRIER_HP;
        barriers[i].active = true;
    }

    spawnInvaders();
}

void SpaceInvaders::drawInvader(Renderer *renderer, int type, float x, float y, int frame) {
    if (type == 0) {
        float p[][2] = {
            {0,0},{6,0},{6,4},{10,4},{10,8},{14,8},{14,4},
            {18,4},{18,0},{24,0},{24,4},{20,4},{20,8},{16,8},
            {16,12},{12,12},{12,16},{8,16},{8,12},{4,12},{4,8},
            {0,8},{0,4},{4,4}
        };
        if (frame == 1) {
            for (int i = 0; i < 24; i++) p[i][0] += (i % 2 == 0) ? 2 : -2;
        }
        int n = sizeof(p) / sizeof(p[0]);
        float pts[48];
        for (int i = 0; i < n; i++) {
            pts[i*2] = x + p[i][0] - 12;
            pts[i*2+1] = y + p[i][1] - 8;
        }
        renderer->drawPoly(pts, n, true);
    } else if (type == 1) {
        float p[][2] = {
            {2,0},{8,0},{8,4},{12,4},{12,8},{16,8},{16,4},{20,4},{20,0},{26,0},
            {26,4},{22,4},{22,8},{18,8},{18,12},{14,12},{14,16},{10,16},{10,12},{6,12},{6,8},{2,8},{2,4},{6,4}
        };
        if (frame == 1) {
            for (int i = 0; i < 24; i++) p[i][0] += (i % 2 == 1) ? 2 : -2;
        }
        int n = sizeof(p) / sizeof(p[0]);
        float pts[48];
        for (int i = 0; i < n; i++) {
            pts[i*2] = x + p[i][0] - 14;
            pts[i*2+1] = y + p[i][1] - 8;
        }
        renderer->drawPoly(pts, n, true);
    } else {
        float p[][2] = {
            {4,0},{8,0},{8,4},{12,4},{12,8},{16,8},{16,12},{20,12},{20,8},{24,8},
            {24,4},{28,4},{28,0},{32,0},{32,4},{28,4},{24,8},{20,12},{16,16},{12,16},{16,12},
            {12,8},{8,4},{4,4}
        };
        if (frame == 1) {
            for (int i = 0; i < 24; i++) p[i][1] += (i % 3 == 0) ? 2 : -1;
        }
        int n = sizeof(p) / sizeof(p[0]);
        float pts[48];
        for (int i = 0; i < n; i++) {
            pts[i*2] = x + p[i][0] - 18;
            pts[i*2+1] = y + p[i][1] - 8;
        }
        renderer->drawPoly(pts, n, true);
    }
}

void SpaceInvaders::update(float dt) {
    if (done) return;

    float w = 1280.0f, h = 720.0f;
    gameTimer += dt;

    if (gameOver) {
        if (input->justPressed(BTN_CROSS)) {
            resetGame();
        }
        if (input->justPressed(BTN_CIRCLE)) {
            done = true;
        }
        return;
    }

    if (playerActive) {
        if (input->isDown(BTN_LEFT)) playerX -= 350.0f * dt;
        if (input->isDown(BTN_RIGHT)) playerX += 350.0f * dt;
        playerX = fmaxf(playerW * 0.5f, fminf(w - playerW * 0.5f, playerX));

        if (input->justPressed(BTN_CROSS)) {
            for (int i = 0; i < PLAYER_BULLETS; i++) {
                if (!pBullets[i].active) {
                    pBullets[i].x = playerX;
                    pBullets[i].y = playerY;
                    pBullets[i].vy = -500.0f;
                    pBullets[i].active = true;
                    break;
                }
            }
        }
    }

    moveTimer += dt;
    if (moveTimer >= moveInterval) {
        moveTimer = 0;
        bool changeDir = false;
        int activeCount = 0;

        for (int col = 0; col < INVADER_COLS; col++) {
            for (int row = 0; row < INVADER_ROWS; row++) {
                if (!invaders[col][row].active) continue;
                activeCount++;
                invaders[col][row].animTimer += dt;
                if (invaders[col][row].animTimer > 0.2f) {
                    invaders[col][row].frame ^= 1;
                    invaders[col][row].animTimer = 0;
                }

                float newX = invaders[col][row].x + invaderDir * 20;
                if (newX < 40 || newX > w - 40) {
                    changeDir = true;
                }
            }
        }

        if (activeCount > 0) {
            baseInterval = 0.1f + (1.0f - (float)activeCount / (INVADER_COLS * INVADER_ROWS)) * 0.4f;
            moveInterval = baseInterval;
        }

        if (changeDir) {
            invaderDir = -invaderDir;
            for (int col = 0; col < INVADER_COLS; col++) {
                for (int row = 0; row < INVADER_ROWS; row++) {
                    if (invaders[col][row].active) {
                        invaders[col][row].y += invaderDrop;
                    }
                }
            }
        } else {
            for (int col = 0; col < INVADER_COLS; col++) {
                for (int row = 0; row < INVADER_ROWS; row++) {
                    if (invaders[col][row].active) {
                        invaders[col][row].x += invaderDir * 20;
                    }
                }
            }
        }
    }

    for (int i = 0; i < PLAYER_BULLETS; i++) {
        if (pBullets[i].active) {
            pBullets[i].y += pBullets[i].vy * dt;
            if (pBullets[i].y < -20) {
                pBullets[i].active = false;
                continue;
            }
        }
    }

    for (int i = 0; i < ALIEN_BULLETS; i++) {
        if (aBullets[i].active) {
            aBullets[i].y += aBullets[i].vy * dt;
            if (aBullets[i].y > h + 20) {
                aBullets[i].active = false;
            }
        }
    }

    int shootTimer = (int)(gameTimer * 10) % 50;
    if (shootTimer == 0) {
        int activeCols[INVADER_COLS];
        int ac = 0;
        for (int col = 0; col < INVADER_COLS; col++) {
            for (int row = INVADER_ROWS - 1; row >= 0; row--) {
                if (invaders[col][row].active) {
                    activeCols[ac++] = col;
                    break;
                }
            }
        }
        if (ac > 0) {
            int shooter = rand() % ac;
            int col = activeCols[shooter];
            int row;
            for (row = INVADER_ROWS - 1; row >= 0; row--) {
                if (invaders[col][row].active) break;
            }
            if (row >= 0) {
                for (int i = 0; i < ALIEN_BULLETS; i++) {
                    if (!aBullets[i].active) {
                        aBullets[i].x = invaders[col][row].x;
                        aBullets[i].y = invaders[col][row].y + 20;
                        aBullets[i].vy = 300.0f;
                        aBullets[i].active = true;
                        break;
                    }
                }
            }
        }
    }

    for (int i = 0; i < PLAYER_BULLETS; i++) {
        if (!pBullets[i].active) continue;
        for (int col = 0; col < INVADER_COLS; col++) {
            for (int row = 0; row < INVADER_ROWS; row++) {
                if (!invaders[col][row].active) continue;
                float dx = pBullets[i].x - invaders[col][row].x;
                float dy = pBullets[i].y - invaders[col][row].y;
                if (fabsf(dx) < 20 && fabsf(dy) < 20) {
                    invaders[col][row].active = false;
                    pBullets[i].active = false;
                    score += 10 * (invaders[col][row].type + 1) * 10;
                    goto bullet_done;
                }
            }
        }
        bullet_done:;
    }

    for (int i = 0; i < ALIEN_BULLETS; i++) {
        if (!aBullets[i].active) continue;
        if (playerActive) {
            float dx = aBullets[i].x - playerX;
            float dy = aBullets[i].y - playerY;
            if (fabsf(dx) < playerW * 0.5f + 5 && fabsf(dy) < 15) {
                aBullets[i].active = false;
                playerActive = false;
                lives--;
                if (lives <= 0) {
                    gameOver = true;
                } else {
                    playerActive = true;
                    playerX = w * 0.5f;
                }
                continue;
            }
        }
        for (int b = 0; b < 4; b++) {
            if (!barriers[b].active) continue;
            float dx = aBullets[i].x - barriers[b].x;
            float dy = aBullets[i].y - barriers[b].y;
            if (fabsf(dx) < 30 && fabsf(dy) < 15) {
                aBullets[i].active = false;
                barriers[b].hp--;
                if (barriers[b].hp <= 0) barriers[b].active = false;
            }
        }
    }

    for (int col = 0; col < INVADER_COLS; col++) {
        for (int row = 0; row < INVADER_ROWS; row++) {
            if (!invaders[col][row].active) continue;
            if (invaders[col][row].y > h - 100) {
                gameOver = true;
                return;
            }
        }
    }

    int alive = 0;
    for (int col = 0; col < INVADER_COLS; col++)
        for (int row = 0; row < INVADER_ROWS; row++)
            if (invaders[col][row].active) alive++;
    if (alive == 0) {
        spawnInvaders();
        invaderSpeed += 20;
    }
}

void SpaceInvaders::render(Renderer *renderer) {
    float w = renderer->getWidth();

    for (int col = 0; col < INVADER_COLS; col++) {
        for (int row = 0; row < INVADER_ROWS; row++) {
            if (!invaders[col][row].active) continue;
            u32 colors[3] = { 0x00FF44, 0x44FFAA, 0xFF4444 };
            renderer->setColor(colors[invaders[col][row].type]);
            drawInvader(renderer, invaders[col][row].type,
                       invaders[col][row].x, invaders[col][row].y,
                       invaders[col][row].frame);
        }
    }

    for (int i = 0; i < PLAYER_BULLETS; i++) {
        if (pBullets[i].active) {
            renderer->setColor(0x00FF00);
            renderer->drawRect(pBullets[i].x - 2, pBullets[i].y - 5, 4, 10, true);
        }
    }

    for (int i = 0; i < ALIEN_BULLETS; i++) {
        if (aBullets[i].active) {
            renderer->setColor(0xFF4444);
            renderer->drawRect(aBullets[i].x - 2, aBullets[i].y - 5, 4, 10, true);
        }
    }

    for (int i = 0; i < 4; i++) {
        if (barriers[i].active) {
            float hp = (float)barriers[i].hp / MAX_BARRIER_HP;
            renderer->setColor(0.0f, hp, 0.0f, 1.0f);
            float bw = 60, bh = 25;
            renderer->drawRect(barriers[i].x - bw * 0.5f, barriers[i].y - bh * 0.5f, bw, bh, true);
            renderer->setColor(0x00AA00);
            renderer->drawRect(barriers[i].x - bw * 0.5f, barriers[i].y - bh * 0.5f, bw, bh);
        }
    }

    if (playerActive) {
        renderer->setColor(0x00AAFF);
        float pts[] = {
            playerX, playerY - 15,
            playerX - playerW * 0.5f, playerY + 10,
            playerX + playerW * 0.5f, playerY + 10
        };
        renderer->drawPoly(pts, 3, true);
        renderer->drawLine(playerX, playerY - 15, playerX, playerY - 25);
    }

    char buf[32];
    sprintf(buf, "SCORE: %d", score);
    renderer->setColor(0xFFFFFF);
    renderer->drawString(20, 20, buf);

    sprintf(buf, "LIVES: %d", lives);
    renderer->drawString(20, 35, buf);

    renderer->setColor(0x444444);
    renderer->drawStringCentered(640, 700, "CIRCLE: Exit");

    if (gameOver) {
        renderer->setColor(0xFF0000);
        renderer->drawStringCentered(640, 360, "GAME OVER");
        renderer->setColor(0xFFFFFF);
        renderer->drawStringCentered(640, 380, "Press X to restart");
    }
}

bool SpaceInvaders::isDone() { return done; }

void SpaceInvaders::cleanup() {}
