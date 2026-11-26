#pragma once
#include <U8g2lib.h>

struct AnimationData {
    const uint8_t* const* frames;
    uint8_t frameCount;
    uint16_t frameDelay;
    uint8_t width;
    uint8_t height;
};

class AnimationPlayer {
public:
    void play(const AnimationData& animation);
    void update(U8G2& display);

private:
    const AnimationData* currentAnimation = nullptr;

    uint8_t currentFrame = 0;
    unsigned long previousTime = 0;
    bool framePending = false;
};