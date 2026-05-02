#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include "game.h"

#define MAX_ASTEROIDS 12
#define MAX_BULLETS 20
#define MAX_ASTEROID_VERTS 10

struct Asteroid {
    float x, y, vx, vy;
    float rot, rotSpeed;
    float radius;
    int verts;
    float vertAngles[MAX_ASTEROID_VERTS];
    float vertRadii[MAX_ASTEROID_VERTS];
    bool active;
    int level;
};

struct Bullet {
    float x, y, vx, vy;
    float life;
};

struct Particle {
    float x, y, vx, vy;
    float life;
    float maxLife;
    float r, g, b;
};

#define MAX_PARTICLES 100

class Asteroids : public Game {
public:
    Asteroids();
    const char* getName() override;
    void init(Renderer *renderer, Input *input) override;
    void update(float dt) override;
    void render(Renderer *renderer) override;
    bool isDone() override;
    void cleanup() override;

private:
    void spawnAsteroid(int level, float x, float y);
    void splitAsteroid(int idx);
    void initShip();
    void spawnParticles(float x, float y, float r, float g, float b, int count);

    Input *input;
    bool done;

    float shipX, shipY;
    float shipVX, shipVY;
    float shipAngle;
    float shipR;
    bool shipActive;

    float bulletCooldown;

    Asteroid asteroids[MAX_ASTEROIDS];
    Bullet bullets[MAX_BULLETS];
    Particle particles[MAX_PARTICLES];

    int score;
    int lives;
    float respawnTimer;
    bool gameOver;

    float invulnTimer;
};

#endif
