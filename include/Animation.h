#pragma once
#include <U8g2lib.h>

class Animation {
    public:
        Animation(uint8_t frameNum, const uint8_t* frames)
            : frameNum(frameNum), frames(frames) {}
    private:
        const uint8_t frameNum;
        const uint8_t* frames;
};