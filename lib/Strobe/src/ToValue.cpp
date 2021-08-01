#include "ToValue.h"

static const int FlashStrobeCode[19] = {
    0x80,
    0x80, 0x82, 0x85, 0x88, 0x8a, 0x8d,
    0x90, 0x92, 0x95, 0x98, 0x9a, 0x9d,
    0xa0, 0xa2, 0xa5, 0xa8, 0xaa, 0xad,
};

int Take4::Canon::IntToStrobePower(int value)
{
    for (int i = 0; i < 19; ++i) {
        if (value == FlashStrobeCode[i]) {
            return value;
        }
    }
    return -1;
}

int Take4::Canon::StrobePowerToInt(int value)
{
    if (value >= 0 && value <= 18) {
        return FlashStrobeCode[value];
    }
    return -1;
}