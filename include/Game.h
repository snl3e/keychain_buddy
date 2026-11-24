#pragma once

#include <stdint.h>

#include "Button.h"

class U8G2;

enum class GamePhase {
    Start,
    Running,
    End
};

class Game {
    protected:
        const char* title;
        uint32_t width;
        uint32_t height;

        void endGame();

    public:
        Game(const char* title, uint32_t width, uint32_t height);
        virtual ~Game() = default;

        void begin(uint32_t nowMs, bool button1Down, bool button2Down);
        void tick(uint32_t nowMs, bool button1Down, bool button2Down);
        void render(U8G2& u8g2);

        bool shouldExitToMenu() const;
        void clearExitRequest();
        GamePhase phase() const;
        const char* gameTitle() const;

    protected:
        virtual void onGameReset();
        virtual void updateRunning(uint32_t deltaMs, const ButtonInput& input) = 0;
        virtual void drawRunning(U8G2& u8g2) = 0;
        virtual void drawStart(U8G2& u8g2);
        virtual void drawEnd(U8G2& u8g2);

    private:
        void startRunning();

        GamePhase phase_ = GamePhase::Start;
        SingleButton button1_;
        SingleButton button2_;
        uint32_t lastUpdateMs_ = 0;
        bool gameOver_ = false;
        bool exitToMenuRequested_ = false;
};