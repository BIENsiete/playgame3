#ifndef GAME_H
#define GAME_H

class Renderer;
class Input;

class Game {
public:
    virtual ~Game() {}
    virtual const char* getName() = 0;
    virtual void init(Renderer *renderer, Input *input) = 0;
    virtual void update(float dt) = 0;
    virtual void render(Renderer *renderer) = 0;
    virtual bool isDone() = 0;
    virtual void cleanup() {}
};

#endif
