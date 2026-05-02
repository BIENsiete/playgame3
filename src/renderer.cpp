#define _USE_MATH_DEFINES
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <malloc.h>

#include "renderer.h"
#include "font.h"

Renderer::Renderer()
    : context(nullptr)
    , displayWidth(DISPLAY_WIDTH)
    , displayHeight(DISPLAY_HEIGHT)
    , vertices(nullptr)
    , vertexCount(0)
    , curR(1.0f)
    , curG(1.0f)
    , curB(1.0f)
    , curA(1.0f)
    , curBuf(0)
{
}

Renderer::~Renderer() {
    if (vertices) free(vertices);
}

bool Renderer::init() {
    gcmInitContext(&context, nullptr, 0x10000);

    displayWidth = DISPLAY_WIDTH;
    displayHeight = DISPLAY_HEIGHT;

    u32 pitch = displayWidth * 4;
    u32 bufSize = (pitch * displayHeight + 0xFFFFF) & ~0xFFFFF;

    for (int i = 0; i < 2; i++) {
        void *buf = memalign(1048576, bufSize);
        if (!buf) return false;
        memset(buf, 0, bufSize);
        colorBufOffset[i] = gcmGetAddressFromPointer(buf);
        colorBufId[i] = gcmSetDisplayBuffer(colorBufOffset[i], pitch, (u32)displayWidth, (u32)displayHeight);
    }

    void *depthBuf = memalign(1048576, bufSize);
    if (depthBuf) {
        gcmResetDefaultDepthBuffer(context, gcmGetAddressFromPointer(depthBuf), bufSize);
    }

    gcmSetFlipMode(context, GCM_FLIP_VSYNC);

    vertices = (Vertex *)memalign(128, MAX_VERTICES * VERTEX_SIZE);
    if (!vertices) return false;
    vertexCount = 0;

    return true;
}

void Renderer::beginFrame() {
    gcmSetWaitFlip(context);

    gcmSetRenderTarget(context, colorBufOffset[curBuf], 0, 0, (u32)displayWidth, (u32)displayHeight);
    gcmSetRenderTargetDepth(context, 0, 0, (u32)displayWidth, (u32)displayHeight);

    gcmSetViewport(context, 0, 0, (u32)displayWidth, (u32)displayHeight);

    gcmSetCullFaceEnable(context, GCM_FALSE);
    gcmSetDepthTestEnable(context, GCM_FALSE);
    gcmSetBlendEnable(context, GCM_TRUE);
    gcmSetBlendFunc(context, GCM_SRC_ALPHA, GCM_ONE_MINUS_SRC_ALPHA);
    gcmSetShadeModel(context, GCM_SMOOTH);

    u32 clearColor = 0x00000000;
    gcmSetClearColor(context, clearColor);
    gcmSetClearSurface(context, GCM_CLEAR_R | GCM_CLEAR_G | GCM_CLEAR_B | GCM_CLEAR_A);
    gcmFlush(context);

    vertexCount = 0;
}

void Renderer::endFrame() {
    flushVertices();

    gcmSetFlip(context, colorBufId[curBuf]);
    gcmFlush(context);

    curBuf ^= 1;
}

void Renderer::setColor(float r, float g, float b, float a) {
    curR = r; curG = g; curB = b; curA = a;
}

void Renderer::setColor(u32 hex) {
    curR = ((hex >> 16) & 0xFF) / 255.0f;
    curG = ((hex >> 8) & 0xFF) / 255.0f;
    curB = (hex & 0xFF) / 255.0f;
    curA = ((hex >> 24) & 0xFF) / 255.0f;
    if (curA == 0) curA = 1.0f;
}

void Renderer::clear() {
    gcmSetClearColor(context, 0xFF000000);
    gcmSetClearSurface(context, GCM_CLEAR_R | GCM_CLEAR_G | GCM_CLEAR_B | GCM_CLEAR_A);
}

void Renderer::addVertex(float x, float y, float z, float r, float g, float b, float a) {
    if (vertexCount >= MAX_VERTICES) {
        flushVertices();
    }
    Vertex &v = vertices[vertexCount++];
    v.x = x;
    v.y = y;
    v.z = z;
    v.r = r;
    v.g = g;
    v.b = b;
    v.a = a;
}

void Renderer::flushVertices() {
    if (vertexCount == 0) return;

    u32 vertSize = VERTEX_SIZE;
    u32 offset = gcmGetAddressFromPointer(vertices);

    gcmSetVertexDataArray(context, GCM_VERTEX_ATTRIB_POS, 0, vertSize, 3,
        GCM_VERTEX_DATA_TYPE_F32, GCM_LOCATION_RSX, offset);
    offset += sizeof(float) * 3;
    gcmSetVertexDataArray(context, GCM_VERTEX_ATTRIB_COL, 0, vertSize, 4,
        GCM_VERTEX_DATA_TYPE_F32, GCM_LOCATION_RSX, offset);

    gcmSetDrawArrays(context, GCM_MODE_LINES, 0, vertexCount);
    gcmFlush(context);

    vertexCount = 0;
}

void Renderer::reserveVertices(int count) {
    if (vertexCount + count > MAX_VERTICES) {
        flushVertices();
    }
}

void Renderer::drawLine(float x1, float y1, float x2, float y2) {
    reserveVertices(2);
    addVertex(x1, y1, 0.0f, curR, curG, curB, curA);
    addVertex(x2, y2, 0.0f, curR, curG, curB, curA);
}

void Renderer::drawRect(float x, float y, float w, float h, bool filled) {
    if (filled) {
        drawTriangle(x, y, x + w, y, x, y + h, true);
        drawTriangle(x + w, y, x + w, y + h, x, y + h, true);
    } else {
        drawLine(x, y, x + w, y);
        drawLine(x + w, y, x + w, y + h);
        drawLine(x + w, y + h, x, y + h);
        drawLine(x, y + h, x, y);
    }
}

void Renderer::drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, bool filled) {
    if (filled) {
        float cx = (x1 + x2 + x3) / 3.0f;
        float cy = (y1 + y2 + y3) / 3.0f;
        drawLine(x1, y1, cx, cy);
        drawLine(x2, y2, cx, cy);
        drawLine(x3, y3, cx, cy);
        drawLine(x1, y1, x2, y2);
        drawLine(x2, y2, x3, y3);
        drawLine(x3, y3, x1, y1);
    } else {
        drawLine(x1, y1, x2, y2);
        drawLine(x2, y2, x3, y3);
        drawLine(x3, y3, x1, y1);
    }
}

void Renderer::drawCircle(float cx, float cy, float r, int segments) {
    float step = 2.0f * M_PI / segments;
    float prevX = cx + r;
    float prevY = cy;
    for (int i = 1; i <= segments; i++) {
        float a = i * step;
        float x = cx + r * cosf(a);
        float y = cy + r * sinf(a);
        drawLine(prevX, prevY, x, y);
        prevX = x;
        prevY = y;
    }
}

void Renderer::drawCircleFilled(float cx, float cy, float r, int segments) {
    float step = 2.0f * M_PI / segments;
    for (int i = 0; i < segments; i++) {
        float a1 = i * step;
        float a2 = (i + 1) * step;
        float x1 = cx + r * cosf(a1);
        float y1 = cy + r * sinf(a1);
        float x2 = cx + r * cosf(a2);
        float y2 = cy + r * sinf(a2);
        drawLine(cx, cy, x1, y1);
        drawLine(cx, cy, x2, y2);
        drawLine(x1, y1, x2, y2);
    }
}

void Renderer::drawPoly(const float *points, int count, bool closed) {
    for (int i = 0; i < count - 1; i++) {
        drawLine(points[i * 2], points[i * 2 + 1], points[(i + 1) * 2], points[(i + 1) * 2 + 1]);
    }
    if (closed && count > 2) {
        drawLine(points[(count - 1) * 2], points[(count - 1) * 2 + 1], points[0], points[1]);
    }
}

void Renderer::drawString(float x, float y, const char *text) {
    float origX = x;
    while (*text) {
        char c = *text++;
        if (c == '\n') {
            x = origX;
            y += 10;
            continue;
        }
        if (c < 32 || c > 126) continue;
        int idx = c - 32;
        for (int row = 0; row < 8; row++) {
            u8 bits = font8x8[idx][row];
            for (int col = 0; col < 8; col++) {
                if (bits & (1 << (7 - col))) {
                    drawRect(x + col, y + row, 1, 1, true);
                }
            }
        }
        x += 8;
    }
}

void Renderer::drawStringCentered(float x, float y, const char *text) {
    int len = strlen(text);
    drawString(x - len * 4, y, text);
}
