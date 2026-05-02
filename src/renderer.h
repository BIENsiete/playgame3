#ifndef RENDERER_H
#define RENDERER_H

#include <ppu-types.h>
#include <rsx/gcm.h>

#define DISPLAY_WIDTH 1280
#define DISPLAY_HEIGHT 720

struct Vertex {
    float x, y, z;
    float r, g, b, a;
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool init();
    void beginFrame();
    void endFrame();

    void setColor(float r, float g, float b, float a);
    void setColor(u32 hex);
    void clear();

    void drawLine(float x1, float y1, float x2, float y2);
    void drawRect(float x, float y, float w, float h, bool filled = false);
    void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, bool filled = false);
    void drawCircle(float cx, float cy, float r, int segments = 24);
    void drawCircleFilled(float cx, float cy, float r, int segments = 24);
    void drawPoly(const float *points, int count, bool closed = true);
    void drawString(float x, float y, const char *text);
    void drawStringCentered(float x, float y, const char *text);

    float getWidth() const { return displayWidth; }
    float getHeight() const { return displayHeight; }

private:
    void flushVertices();
    void addVertex(float x, float y, float z, float r, float g, float b, float a);
    void reserveVertices(int count);

    gcmContextData *context;
    float displayWidth;
    float displayHeight;

    static const int MAX_VERTICES = 32768;
    static const int VERTEX_SIZE = 32;
    Vertex *vertices;
    int vertexCount;
    float curR, curG, curB, curA;

    u32 colorBufOffset[2];
    u32 colorBufId[2];
    int curBuf;
};

#endif
