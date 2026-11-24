#include "Animation.h"

void AnimationPlayer::play(const AnimationData& animation) {
    currentAnimation = &animation;
    currentFrame = 0;
    previousTime = millis();
}

void AnimationPlayer::update(U8G2& display) {
    if (currentAnimation == nullptr) {
        return;
    }

    unsigned long now = millis();

    if (now - previousTime >= currentAnimation->frameDelay) {
        previousTime = now;

        display.clearBuffer();

        display.drawXBMP(
            0,
            0,
            currentAnimation->width,
            currentAnimation->height,
            currentAnimation->frames[currentFrame]
        );

        display.sendBuffer();

        currentFrame++;

        if (currentFrame >= currentAnimation->frameCount) {
            currentFrame = 0;
        }
    }
}