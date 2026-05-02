#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "asteroids.h"
#include "../renderer.h"
#include "../input.h"

Asteroids::Asteroids()
    : input(nullptr)
    , done(false)
    , shipX(640), shipY(360)
    , shipVX(0), shipVY(0)
    , shipAngle(-M_PI_2)
    , shipR(14.0f)
    , shipActive(true)
    , bulletCooldown(0)
    , score(0)
    , lives(3)
    , respawnTimer(0)
    , gameOver(false)
    , invulnTimer(0)
{
    memset(asteroids, 0, sizeof(asteroids));
    memset(bullets, 0, sizeof(bullets));
    memset(particles, 0, sizeof(particles));
}

const char* Asteroids::getName() { return "ASTEROIDS"; }

void Asteroids::init(Renderer *renderer, Input *input) {
    this->input = input;
    done = false;
    score = 0;
    lives = 3;
    gameOver = false;
    invulnTimer = 3.0f;
    bulletCooldown = 0;

    for (int i = 0; i < MAX_ASTEROIDS; i++) asteroids[i].active = false;
    for (int i = 0; i < MAX_BULLETS; i++) bullets[i].life = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) particles[i].life = 0;

    initShip();

    for (int i = 0; i < 3; i++) {
        float x, y;
        do {
            x = (float)(rand() % 1280);
            y = (float)(rand() % 720);
        } while (fabsf(x - 640) < 200 && fabsf(y - 360) < 200);
        spawnAsteroid(3, x, y);
    }
}

void Asteroids::initShip() {
    shipX = 640;
    shipY = 360;
    shipVX = 0;
    shipVY = 0;
    shipAngle = -M_PI_2;
    shipActive = true;
}

void Asteroids::spawnAsteroid(int level, float x, float y) {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (!asteroids[i].active) {
            asteroids[i].active = true;
            asteroids[i].x = x;
            asteroids[i].y = y;
            asteroids[i].level = level;
            asteroids[i].radius = level * 12.0f;
            asteroids[i].vx = (float)(rand() % 200 - 100) * 0.5f;
            asteroids[i].vy = (float)(rand() % 200 - 100) * 0.5f;
            asteroids[i].rot = 0;
            asteroids[i].rotSpeed = (float)(rand() % 200 - 100) * 0.01f;
            asteroids[i].verts = 7 + rand() % (MAX_ASTEROID_VERTS - 7);
            for (int j = 0; j < asteroids[i].verts; j++) {
                asteroids[i].vertAngles[j] = (float)j / asteroids[i].verts * M_PI * 2;
                asteroids[i].vertRadii[j] = asteroids[i].radius * (0.7f + (float)(rand() % 60) * 0.01f);
            }
            return;
        }
    }
}

void Asteroids::splitAsteroid(int idx) {
    Asteroid &a = asteroids[idx];
    if (a.level > 1) {
        for (int i = 0; i < 2; i++) {
            float angle = (float)(rand() % 628) * 0.01f;
            spawnAsteroid(a.level - 1, a.x + cosf(angle) * 10, a.y + sinf(angle) * 10);
        }
    }
    a.active = false;
}

void Asteroids::spawnParticles(float x, float y, float r, float g, float b, int count) {
    for (int i = 0; i < MAX_PARTICLES && count > 0; i++) {
        if (particles[i].life <= 0) {
            float angle = (float)(rand() % 628) * 0.01f;
            float speed = (float)(rand() % 200 + 50);
            particles[i].x = x;
            particles[i].y = y;
            particles[i].vx = cosf(angle) * speed;
            particles[i].vy = sinf(angle) * speed;
            particles[i].life = 0.3f + (float)(rand() % 100) * 0.005f;
            particles[i].maxLife = particles[i].life;
            particles[i].r = r;
            particles[i].g = g;
            particles[i].b = b;
            count--;
        }
    }
}

void Asteroids::update(float dt) {
    if (done) return;

    if (gameOver) {
        if (input->justPressed(BTN_CROSS)) {
            init(0, input);
        }
        if (input->justPressed(BTN_CIRCLE)) {
            done = true;
        }
        return;
    }

    float w = 1280.0f, h = 720.0f;

    if (invulnTimer > 0) invulnTimer -= dt;

    if (shipActive) {
        if (input->isDown(BTN_LEFT)) shipAngle -= 4.0f * dt;
        if (input->isDown(BTN_RIGHT)) shipAngle += 4.0f * dt;

        if (input->isDown(BTN_UP)) {
            float thrust = 300.0f;
            shipVX += cosf(shipAngle) * thrust * dt;
            shipVY += sinf(shipAngle) * thrust * dt;
        }

        shipX += shipVX * dt;
        shipY += shipVY * dt;

        shipVX *= 0.99f;
        shipVY *= 0.99f;

        float maxSpeed = 400.0f;
        float speed = sqrtf(shipVX * shipVX + shipVY * shipVY);
        if (speed > maxSpeed) {
            shipVX = (shipVX / speed) * maxSpeed;
            shipVY = (shipVY / speed) * maxSpeed;
        }

        if (shipX < -20) shipX = w + 20;
        if (shipX > w + 20) shipX = -20;
        if (shipY < -20) shipY = h + 20;
        if (shipY > h + 20) shipY = -20;

        bulletCooldown -= dt;
        if (input->isDown(BTN_CROSS) && bulletCooldown <= 0) {
            bulletCooldown = 0.2f;
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bullets[i].life <= 0) {
                    bullets[i].x = shipX + cosf(shipAngle) * shipR;
                    bullets[i].y = shipY + sinf(shipAngle) * shipR;
                    bullets[i].vx = cosf(shipAngle) * 600.0f;
                    bullets[i].vy = sinf(shipAngle) * 600.0f;
                    bullets[i].life = 1.5f;
                    break;
                }
            }
        }
    } else {
        respawnTimer -= dt;
        if (respawnTimer <= 0 && lives > 0) {
            initShip();
            invulnTimer = 2.0f;
        }
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].life > 0) {
            bullets[i].x += bullets[i].vx * dt;
            bullets[i].y += bullets[i].vy * dt;
            bullets[i].life -= dt;

            if (bullets[i].x < -50 || bullets[i].x > w + 50 ||
                bullets[i].y < -50 || bullets[i].y > h + 50) {
                bullets[i].life = 0;
            }
        }
    }

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (asteroids[i].active) {
            asteroids[i].x += asteroids[i].vx * dt;
            asteroids[i].y += asteroids[i].vy * dt;
            asteroids[i].rot += asteroids[i].rotSpeed * dt;

            if (asteroids[i].x < -60) asteroids[i].x = w + 60;
            if (asteroids[i].x > w + 60) asteroids[i].x = -60;
            if (asteroids[i].y < -60) asteroids[i].y = h + 60;
            if (asteroids[i].y > h + 60) asteroids[i].y = -60;
        }
    }

    for (int bi = 0; bi < MAX_BULLETS; bi++) {
        if (bullets[bi].life <= 0) continue;
        for (int ai = 0; ai < MAX_ASTEROIDS; ai++) {
            if (!asteroids[ai].active) continue;
            float dx = bullets[bi].x - asteroids[ai].x;
            float dy = bullets[bi].y - asteroids[ai].y;
            if (dx * dx + dy * dy < asteroids[ai].radius * asteroids[ai].radius) {
                bullets[bi].life = 0;
                spawnParticles(asteroids[ai].x, asteroids[ai].y, 1, 0.5f, 0, 15);
                score += asteroids[ai].level * 100;
                splitAsteroid(ai);
                break;
            }
        }
    }

    if (shipActive && invulnTimer <= 0) {
        for (int ai = 0; ai < MAX_ASTEROIDS; ai++) {
            if (!asteroids[ai].active) continue;
            float dx = shipX - asteroids[ai].x;
            float dy = shipY - asteroids[ai].y;
            if (dx * dx + dy * dy < (shipR + asteroids[ai].radius) * (shipR + asteroids[ai].radius)) {
                shipActive = false;
                spawnParticles(shipX, shipY, 1, 1, 1, 30);
                lives--;
                respawnTimer = 2.0f;
                if (lives <= 0) {
                    gameOver = true;
                }
                break;
            }
        }
    }

    int activeCount = 0;
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (asteroids[i].active) activeCount++;
    }
    if (activeCount == 0 && shipActive) {
        for (int i = 0; i < 4; i++) {
            float x = (float)(rand() % 1280);
            float y = (float)(rand() % 720);
            spawnAsteroid(3, x, y);
        }
    }

    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].life > 0) {
            particles[i].x += particles[i].vx * dt;
            particles[i].y += particles[i].vy * dt;
            particles[i].life -= dt;
        }
    }
}

void Asteroids::render(Renderer *renderer) {
    float w = renderer->getWidth();
    float h = renderer->getHeight();

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (asteroids[i].active) {
            Asteroid &a = asteroids[i];
            float points[20];
            for (int j = 0; j < a.verts; j++) {
                float angle = a.vertAngles[j] + a.rot;
                points[j * 2] = a.x + cosf(angle) * a.vertRadii[j];
                points[j * 2 + 1] = a.y + sinf(angle) * a.vertRadii[j];
            }
            float t = (float)a.level / 3.0f;
            renderer->setColor(0.6f + 0.4f * (1 - t), 0.6f + 0.4f * t, 0.6f, 1.0f);
            renderer->drawPoly(points, a.verts, true);
        }
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].life > 0) {
            renderer->setColor(0xFFFF44);
            renderer->drawRect(bullets[i].x - 2, bullets[i].y - 2, 4, 4, true);
        }
    }

    if (shipActive) {
        if (invulnTimer <= 0 || fmodf(invulnTimer * 10, 2) < 1) {
            float tipX = shipX + cosf(shipAngle) * shipR;
            float tipY = shipY + sinf(shipAngle) * shipR;
            float leftX = shipX + cosf(shipAngle + 2.3f) * shipR * 0.7f;
            float leftY = shipY + sinf(shipAngle + 2.3f) * shipR * 0.7f;
            float rightX = shipX + cosf(shipAngle - 2.3f) * shipR * 0.7f;
            float rightY = shipY + sinf(shipAngle - 2.3f) * shipR * 0.7f;
            float backX = shipX + cosf(shipAngle + M_PI) * shipR * 0.5f;
            float backY = shipY + sinf(shipAngle + M_PI) * shipR * 0.5f;

            float thrust = sqrtf(shipVX * shipVX + shipVY * shipVY);
            if (input && input->isDown(BTN_UP)) {
                renderer->setColor(0xFF6600);
                renderer->drawLine(backX, backY,
                    shipX + cosf(shipAngle + M_PI + 0.3f) * (shipR * 0.3f + thrust * 0.05f),
                    shipY + sinf(shipAngle + M_PI + 0.3f) * (shipR * 0.3f + thrust * 0.05f));
                renderer->drawLine(backX, backY,
                    shipX + cosf(shipAngle + M_PI - 0.3f) * (shipR * 0.3f + thrust * 0.05f),
                    shipY + sinf(shipAngle + M_PI - 0.3f) * (shipR * 0.3f + thrust * 0.05f));
            }

            renderer->setColor(0x00AAFF);
            renderer->drawLine(tipX, tipY, leftX, leftY);
            renderer->drawLine(tipX, tipY, rightX, rightY);
            renderer->drawLine(leftX, leftY, backX, backY);
            renderer->drawLine(rightX, rightY, backX, backY);
        }
    }

    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].life > 0) {
            float alpha = particles[i].life / particles[i].maxLife;
            renderer->setColor(particles[i].r * alpha, particles[i].g * alpha, particles[i].b * alpha, alpha);
            renderer->drawRect(particles[i].x - 1, particles[i].y - 1, 2, 2, true);
        }
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

bool Asteroids::isDone() { return done; }

void Asteroids::cleanup() {}
