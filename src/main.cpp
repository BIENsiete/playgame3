#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sysutil/sysutil.h>

#include "renderer.h"
#include "input.h"
#include "menu.h"
#include "games/game.h"
#include "games/pong.h"
#include "games/snake.h"
#include "games/asteroids.h"
#include "games/breakout.h"
#include "games/spaceinvaders.h"

static u64 getTimeUsec() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return (u64)tv.tv_sec * 1000000 + tv.tv_usec;
}

static bool quitApp = false;

void sysutilCallback(u64 status, u64 param, void *userdata) {
    switch (status) {
        case SYSUTIL_EXIT_GAME:
            quitApp = true;
            break;
        case SYSUTIL_DRAW_BEGIN:
        case SYSUTIL_DRAW_END:
            break;
        default:
            break;
    }
}

int main() {
    srand((unsigned int)time(nullptr));

    sysUtilRegisterCallback(SYSUTIL_PRIORITY_DEFAULT, sysutilCallback, nullptr);

    Renderer renderer;
    if (!renderer.init()) {
        return 1;
    }

    Input input;
    if (!input.init()) {
        return 1;
    }

    Game *games[] = {
        new Pong(),
        new Snake(),
        new Asteroids(),
        new Breakout(),
        new SpaceInvaders()
    };
    int gameCount = sizeof(games) / sizeof(games[0]);

    Menu menu(games, gameCount);
    menu.init(&renderer, &input);

    Game *currentGame = nullptr;
    u64 lastTime = getTimeUsec();
    float dt = 1.0f / 60.0f;

    while (!quitApp) {
        u64 now = getTimeUsec();
        dt = (float)(now - lastTime) / 1000000.0f;
        if (dt > 0.1f) dt = 0.1f;
        if (dt < 0.001f) dt = 0.001f;
        lastTime = now;

        input.update();
        sysUtilCheckCallback();

        renderer.beginFrame();

        if (currentGame == nullptr) {
            menu.update(dt);
            menu.render(&renderer);

            if (menu.shouldStartGame()) {
                currentGame = menu.getSelectedGame();
                currentGame->init(&renderer, &input);
                menu.reset();
            }
        } else {
            currentGame->update(dt);
            currentGame->render(&renderer);

            if (currentGame->isDone()) {
                currentGame->cleanup();
                currentGame = nullptr;
            }

            if (input.justPressed(BTN_CIRCLE) && currentGame != nullptr) {
                currentGame->cleanup();
                currentGame = nullptr;
            }
        }

        renderer.endFrame();
    }

    for (int i = 0; i < gameCount; i++) {
        delete games[i];
    }

    sysUtilUnregisterCallback(SYSUTIL_PRIORITY_DEFAULT);

    return 0;
}
