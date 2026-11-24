#pragma once

#include <stdint.h>

struct ButtonInput {
    bool down = false;
    bool pressed = false;
    bool released = false;
    bool click = false;
};

class SingleButton {
  public:
    explicit SingleButton(uint16_t debounceMs = 30);

    void reset(bool rawDown, uint32_t nowMs);
    ButtonInput update(bool rawDown, uint32_t nowMs);

  private:
    uint16_t debounceMs_;

    bool rawDown_ = false;
    bool debouncedDown_ = false;
    uint32_t rawChangedAtMs_ = 0;
};

bool readActiveLowButton(uint8_t pin);
