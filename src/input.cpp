#include <string.h>
#include <io/pad.h>

#include "input.h"

static u32 padToButton(u32 buttons) {
    u32 result = 0;
    if (buttons & PAD_UP) result |= (1 << BTN_UP);
    if (buttons & PAD_DOWN) result |= (1 << BTN_DOWN);
    if (buttons & PAD_LEFT) result |= (1 << BTN_LEFT);
    if (buttons & PAD_RIGHT) result |= (1 << BTN_RIGHT);
    if (buttons & PAD_CROSS) result |= (1 << BTN_CROSS);
    if (buttons & PAD_CIRCLE) result |= (1 << BTN_CIRCLE);
    if (buttons & PAD_SQUARE) result |= (1 << BTN_SQUARE);
    if (buttons & PAD_TRIANGLE) result |= (1 << BTN_TRIANGLE);
    if (buttons & PAD_L1) result |= (1 << BTN_L1);
    if (buttons & PAD_R1) result |= (1 << BTN_R1);
    if (buttons & PAD_L2) result |= (1 << BTN_L2);
    if (buttons & PAD_R2) result |= (1 << BTN_R2);
    if (buttons & PAD_L3) result |= (1 << BTN_L3);
    if (buttons & PAD_R3) result |= (1 << BTN_R3);
    if (buttons & PAD_START) result |= (1 << BTN_START);
    if (buttons & PAD_SELECT) result |= (1 << BTN_SELECT);
    return result;
}

Input::Input()
    : curButtons(0)
    , prevButtons(0)
    , analogLx(128)
    , analogLy(128)
    , analogRx(128)
    , analogRy(128)
    , initialized(false)
{
}

Input::~Input() {
}

bool Input::init() {
    padInfo padinfo;
    memset(&padinfo, 0, sizeof(padinfo));

    ioPadInit(7);
    initialized = true;

    update();
    prevButtons = curButtons;

    return true;
}

void Input::update() {
    if (!initialized) return;

    padInfo padinfo;
    memset(&padinfo, 0, sizeof(padinfo));
    ioPadGetInfo(&padinfo);

    prevButtons = curButtons;
    curButtons = 0;

    for (int i = 0; i < 7; i++) {
        if (padinfo.status[i]) {
            padData paddata;
            memset(&paddata, 0, sizeof(paddata));
            ioPadGetData(i, &paddata);

            curButtons |= padToButton(paddata.BTN);

            if (i == 0) {
                analogLx = paddata.STK_LX;
                analogLy = paddata.STK_LY;
                analogRx = paddata.STK_RX;
                analogRy = paddata.STK_RY;
            }
            break;
        }
    }
}

bool Input::isDown(int button) const {
    return (curButtons & (1 << button)) != 0;
}

bool Input::justPressed(int button) const {
    return (curButtons & (1 << button)) != 0 && (prevButtons & (1 << button)) == 0;
}

bool Input::justReleased(int button) const {
    return (curButtons & (1 << button)) == 0 && (prevButtons & (1 << button)) != 0;
}

u8 Input::getAnalog(int stick) const {
    switch (stick) {
        case ANALOG_LX: return analogLx;
        case ANALOG_LY: return analogLy;
        case ANALOG_RX: return analogRx;
        case ANALOG_RY: return analogRy;
        default: return 128;
    }
}
