#include "Animation.h"

static uint8_t reverseBits(uint8_t value) {
    value = static_cast<uint8_t>((value >> 4) | (value << 4));
    value = static_cast<uint8_t>(((value & 0xCC) >> 2) | ((value & 0x33) << 2));
    return static_cast<uint8_t>(((value & 0xAA) >> 1) | ((value & 0x55) << 1));
}

void AnimationPlayer::play(const AnimationData& animation) {
    currentAnimation = &animation;
    currentFrame = 0;
    previousTime = millis();
    framePending = true;
}

void AnimationPlayer::update(U8G2& display) {
    if (currentAnimation == nullptr ||
        currentAnimation->frames == nullptr ||
        currentAnimation->frameCount == 0) {
        return;
    }

    unsigned long now = millis();

    if (framePending || now - previousTime >= currentAnimation->frameDelay) {
        previousTime = now;
        framePending = false;

        display.clearBuffer();

        const uint8_t* bitmap = currentAnimation->frames[currentFrame];
        const size_t bytesPerRow = (currentAnimation->width + 7U) / 8U;
        uint8_t row[32];

        for (uint8_t y = 0; y < currentAnimation->height; ++y) {
            for (size_t byte = 0; byte < bytesPerRow; ++byte) {
                row[byte] = reverseBits(pgm_read_byte(bitmap + y * bytesPerRow + byte));
            }
            display.drawXBM(0, y, currentAnimation->width, 1, row);
        }

        display.sendBuffer();

        currentFrame++;

        if (currentFrame >= currentAnimation->frameCount) {
            currentFrame = 0;
        }
    }
}