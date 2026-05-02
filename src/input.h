#ifndef INPUT_H
#define INPUT_H

#include <ppu-types.h>

enum Button {
    BTN_UP = 0,
    BTN_DOWN,
    BTN_LEFT,
    BTN_RIGHT,
    BTN_CROSS,
    BTN_CIRCLE,
    BTN_SQUARE,
    BTN_TRIANGLE,
    BTN_L1,
    BTN_R1,
    BTN_L2,
    BTN_R2,
    BTN_L3,
    BTN_R3,
    BTN_START,
    BTN_SELECT,
    BTN_COUNT
};

enum PadAnalog {
    ANALOG_LX,
    ANALOG_LY,
    ANALOG_RX,
    ANALOG_RY
};

class Input {
public:
    Input();
    ~Input();

    bool init();
    void update();

    bool isDown(int button) const;
    bool justPressed(int button) const;
    bool justReleased(int button) const;
    u8 getAnalog(int stick) const;

private:
    u32 curButtons;
    u32 prevButtons;
    u8 analogLx, analogLy;
    u8 analogRx, analogRy;
    bool initialized;
};

#endif
