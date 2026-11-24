#include "../include/Button.h"

#include <Arduino.h>

SingleButton::SingleButton(uint16_t debounceMs)
    : debounceMs_(debounceMs) {}

void SingleButton::reset(bool rawDown, uint32_t nowMs) {
    rawDown_ = rawDown;
    debouncedDown_ = rawDown;
    rawChangedAtMs_ = nowMs;
}

ButtonInput SingleButton::update(bool rawDown, uint32_t nowMs) {
    ButtonInput input;

    if (rawDown != rawDown_) {
        rawDown_ = rawDown;
        rawChangedAtMs_ = nowMs;
    }

    if ((nowMs - rawChangedAtMs_) >= debounceMs_ && debouncedDown_ != rawDown_) {
        debouncedDown_ = rawDown_;
        if (debouncedDown_) {
            input.pressed = true;
        } else {
            input.released = true;
            input.click = true;
        }
    }

    input.down = debouncedDown_;
    return input;
}

bool readActiveLowButton(uint8_t pin) {
    return digitalRead(pin) == LOW;
}
